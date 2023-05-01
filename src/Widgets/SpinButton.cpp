/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Widgets/SpinButton.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char SpinButton::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<SpinButtonRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setSize(20, 42);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::create(float minimum, float maximum)
    {
        auto spinButton = std::make_shared<SpinButton>();

        spinButton->setMinimum(minimum);
        spinButton->setMaximum(maximum);

        return spinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::copy(const SpinButton::ConstPtr& spinButton)
    {
        if (spinButton)
            return std::static_pointer_cast<SpinButton>(spinButton->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButtonRenderer* SpinButton::getSharedRenderer()
    {
        return aurora::downcast<SpinButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SpinButtonRenderer* SpinButton::getSharedRenderer() const
    {
        return aurora::downcast<const SpinButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButtonRenderer* SpinButton::getRenderer()
    {
        return aurora::downcast<SpinButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else if (getSize().x > getSize().y)
            m_verticalScroll = false;

        if (m_verticalScroll)
        {
            m_spriteArrowUp.setRotation(0);
            m_spriteArrowUpHover.setRotation(0);
            m_spriteArrowDown.setRotation(0);
            m_spriteArrowDownHover.setRotation(0);
        }
        else
        {
            m_spriteArrowUp.setRotation(-90);
            m_spriteArrowUpHover.setRotation(-90);
            m_spriteArrowDown.setRotation(-90);
            m_spriteArrowDownHover.setRotation(-90);
        }

        const Vector2f arrowSize = getArrowSize();
        m_spriteArrowUp.setSize(arrowSize);
        m_spriteArrowUpHover.setSize(arrowSize);
        m_spriteArrowDown.setSize(arrowSize);
        m_spriteArrowDownHover.setSize(arrowSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMinimum(float minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            setMaximum(m_minimum);

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMaximum(float maximum)
    {
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setValue(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;
            onValueChange.emit(this, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setStep(float step)
    {
        m_step = step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setVerticalScroll(bool vertical)
    {
        if (m_verticalScroll == vertical)
            return;

        m_verticalScroll = vertical;
        setSize(getSize().y, getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        m_PressedAt = std::chrono::steady_clock::now();
        // Check if the mouse went down on the spin button
        if (m_mouseDown)
        {
            // Check if the mouse is on top of the upper/right arrow
            if (m_verticalScroll)
            {
                if (FloatRect{ getPosition().x, getPosition().y, getSize().x, getSize().y / 2.0f }.contains(pos))
                    m_mouseDownOnTopArrow = true;
                else
                    m_mouseDownOnTopArrow = false;
            }
            else
            {
                if (FloatRect{ getPosition().x, getPosition().y, getSize().x / 2.0f, getSize().y }.contains(pos))
                    m_mouseDownOnTopArrow = false;
                else
                    m_mouseDownOnTopArrow = true;
            }

            if (m_mouseDownOnTopArrow && m_value < m_maximum)
            {
                setValue(m_value + m_step);
                callMousePressPeriodically(m_PressedAt);
            }
            else if (!m_mouseDownOnTopArrow && m_value > m_minimum)
            {
                setValue(m_value - m_step);
                callMousePressPeriodically(m_PressedAt);
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMouseReleased(Vector2f pos)
    {
        ClickableWidget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(Vector2f pos)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }
        else
        {
            if (FloatRect{getPosition().x, getPosition().y, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }

        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& SpinButton::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"BorderBetweenArrows")
        {
            m_borderBetweenArrowsCached = getSharedRenderer()->getBorderBetweenArrows();
            setSize(m_size);
        }
        else if (property == U"TextureArrowUp")
        {
            m_spriteArrowUp.setTexture(getSharedRenderer()->getTextureArrowUp());
        }
        else if (property == U"TextureArrowUpHover")
        {
            m_spriteArrowUpHover.setTexture(getSharedRenderer()->getTextureArrowUpHover());
        }
        else if (property == U"TextureArrowDown")
        {
            m_spriteArrowDown.setTexture(getSharedRenderer()->getTextureArrowDown());
        }
        else if (property == U"TextureArrowDownHover")
        {
            m_spriteArrowDownHover.setTexture(getSharedRenderer()->getTextureArrowDownHover());
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == U"ArrowColor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == U"ArrowColorHover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteArrowUp.setOpacity(m_opacityCached);
            m_spriteArrowUpHover.setOpacity(m_opacityCached);
            m_spriteArrowDown.setOpacity(m_opacityCached);
            m_spriteArrowDownHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> SpinButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs[U"Minimum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimum));
        node->propertyValuePairs[U"Maximum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximum));
        node->propertyValuePairs[U"Value"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_value));
        node->propertyValuePairs[U"Step"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_step));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Minimum"])
            setMinimum(node->propertyValuePairs[U"Minimum"]->value.toFloat());
        if (node->propertyValuePairs[U"Maximum"])
            setMaximum(node->propertyValuePairs[U"Maximum"]->value.toFloat());
        if (node->propertyValuePairs[U"Value"])
            setValue(node->propertyValuePairs[U"Value"]->value.toFloat());
        if (node->propertyValuePairs[U"Step"])
            setStep(node->propertyValuePairs[U"Step"]->value.toFloat());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f SpinButton::getArrowSize() const
    {
        if (m_verticalScroll)
            return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                    (getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_borderBetweenArrowsCached) / 2.0f};
        else
            return {getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom(),
                    (getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_borderBetweenArrowsCached) / 2.0f};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const Vector2f arrowSize = getArrowSize();

        // Draw the top/left arrow
        if (m_spriteArrowUp.isSet())
        {
            if (m_mouseHover && m_mouseHoverOnTopArrow && m_spriteArrowUpHover.isSet())
                target.drawSprite(states, m_spriteArrowUpHover);
            else
                target.drawSprite(states, m_spriteArrowUp);
        }
        else
        {
            Color arrowBackColor;
            if (m_mouseHover && m_mouseHoverOnTopArrow && m_backgroundColorHoverCached.isSet())
                arrowBackColor = Color::applyOpacity(m_backgroundColorHoverCached, m_opacityCached);
            else
                arrowBackColor = Color::applyOpacity(m_backgroundColorCached, m_opacityCached);

            Vertex::Color arrowVertexColor;
            if (m_mouseHover && m_mouseHoverOnTopArrow && m_arrowColorHoverCached.isSet())
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorCached, m_opacityCached));

            if (m_verticalScroll)
            {
                target.drawFilledRect(states, arrowSize, arrowBackColor);

                target.drawTriangle(states,
                    {{arrowSize.x / 5, arrowSize.y * 4/5}, arrowVertexColor},
                    {{arrowSize.x / 2, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y * 4/5}, arrowVertexColor}
                );
            }
            else // Spin button lies horizontal
            {
                target.drawFilledRect(states, {arrowSize.y, arrowSize.x}, arrowBackColor);

                target.drawTriangle(states,
                    {{arrowSize.x * 4/5, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x / 5, arrowSize.y / 2}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y * 4/5}, arrowVertexColor}
                );
            }
        }

        // Draw the space between the arrows (if there is space)
        if (m_verticalScroll)
        {
            states.transform.translate({0, arrowSize.y});

            if (m_borderBetweenArrowsCached > 0)
            {
                target.drawFilledRect(states, {arrowSize.x, m_borderBetweenArrowsCached}, Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({0, m_borderBetweenArrowsCached});
            }
        }
        else // Horizontal orientation
        {
            states.transform.translate({arrowSize.y, 0});

            if (m_borderBetweenArrowsCached > 0)
            {
                target.drawFilledRect(states, {m_borderBetweenArrowsCached, arrowSize.x}, Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({m_borderBetweenArrowsCached, 0});
            }
        }

        // Draw the bottom/right arrow
        if (m_spriteArrowDown.isSet())
        {
            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_spriteArrowDownHover.isSet())
                target.drawSprite(states, m_spriteArrowDownHover);
            else
                target.drawSprite(states, m_spriteArrowDown);
        }
        else // There are no images
        {
            Color arrowBackColor;
            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_backgroundColorHoverCached.isSet())
                arrowBackColor = Color::applyOpacity(m_backgroundColorHoverCached, m_opacityCached);
            else
                arrowBackColor = Color::applyOpacity(m_backgroundColorCached, m_opacityCached);

            Vertex::Color arrowVertexColor;
            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_arrowColorHoverCached.isSet())
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorCached, m_opacityCached));

            if (m_verticalScroll)
            {
                target.drawFilledRect(states, arrowSize, arrowBackColor);

                target.drawTriangle(states,
                    {{arrowSize.x / 5, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x / 2, arrowSize.y * 4/5}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y / 5}, arrowVertexColor}
                );
            }
            else // Spin button lies horizontal
            {
                target.drawFilledRect(states, {arrowSize.y, arrowSize.x}, arrowBackColor);

                target.drawTriangle(states,
                    {{arrowSize.x / 5, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y / 2}, arrowVertexColor},
                    {{arrowSize.x / 5, arrowSize.y * 4/5}, arrowVertexColor}
                );
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::callMousePressPeriodically(std::chrono::time_point<std::chrono::steady_clock> clicked)
    {
        Timer::scheduleCallback([widget = shared_from_this(), clicked]()
        {
            SpinButton::Ptr spinButton = std::static_pointer_cast<SpinButton>(widget);
            if (spinButton)
            {
                // Mouse still over and the mouse press is current
                if (!spinButton->m_mouseHover || !spinButton->m_mouseDown || spinButton->m_PressedAt != clicked)
                    return;

                if (spinButton->m_value < spinButton->m_maximum &&
                    spinButton->m_mouseDownOnTopArrow && spinButton->m_mouseHoverOnTopArrow)
                {
                    spinButton->setValue(spinButton->m_value + spinButton->m_step);
                    spinButton->callMousePressPeriodically(clicked);
                }
                else if (spinButton->m_value > spinButton->m_minimum &&
                    !spinButton->m_mouseDownOnTopArrow && !spinButton->m_mouseHoverOnTopArrow)
                {
                    spinButton->setValue(spinButton->m_value - spinButton->m_step);
                    spinButton->callMousePressPeriodically(clicked);
                }
            }
        }, std::chrono::milliseconds(300));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr SpinButton::clone() const
    {
        return std::make_shared<SpinButton>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
