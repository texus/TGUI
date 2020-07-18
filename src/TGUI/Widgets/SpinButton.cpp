/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton()
    {
        m_type = "SpinButton";

        m_renderer = aurora::makeCopied<SpinButtonRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

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

    SpinButton::Ptr SpinButton::copy(SpinButton::ConstPtr spinButton)
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

    const SpinButtonRenderer* SpinButton::getRenderer() const
    {
        return aurora::downcast<const SpinButtonRenderer*>(Widget::getRenderer());
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

    void SpinButton::leftMousePressed(Vector2f pos)
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
                CallMousePressPeriodically(m_PressedAt);
            }
            else if (!m_mouseDownOnTopArrow && m_value > m_minimum)
            {
                setValue(m_value - m_step);
                CallMousePressPeriodically(m_PressedAt);
            }
        }
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
        if (property == "Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "BorderBetweenArrows")
        {
            m_borderBetweenArrowsCached = getSharedRenderer()->getBorderBetweenArrows();
            setSize(m_size);
        }
        else if (property == "TextureArrowUp")
        {
            m_spriteArrowUp.setTexture(getSharedRenderer()->getTextureArrowUp());
        }
        else if (property == "TextureArrowUpHover")
        {
            m_spriteArrowUpHover.setTexture(getSharedRenderer()->getTextureArrowUpHover());
        }
        else if (property == "TextureArrowDown")
        {
            m_spriteArrowDown.setTexture(getSharedRenderer()->getTextureArrowDown());
        }
        else if (property == "TextureArrowDownHover")
        {
            m_spriteArrowDownHover.setTexture(getSharedRenderer()->getTextureArrowDownHover());
        }
        else if (property == "BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "ArrowColor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == "ArrowColorHover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if ((property == "Opacity") || (property == "OpacityDisabled"))
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
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_value));
        node->propertyValuePairs["Step"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_step));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["Minimum"])
            setMinimum(node->propertyValuePairs["Minimum"]->value.toFloat());
        if (node->propertyValuePairs["Maximum"])
            setMaximum(node->propertyValuePairs["Maximum"]->value.toFloat());
        if (node->propertyValuePairs["Value"])
            setValue(node->propertyValuePairs["Value"]->value.toFloat());
        if (node->propertyValuePairs["Step"])
            setStep(node->propertyValuePairs["Step"]->value.toFloat());
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

    void SpinButton::draw(RenderTargetBase& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const Vector2f arrowSize = getArrowSize();

        // Draw the top/left arrow
        if (m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
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

                target.drawTriangles(states, {
                    {{arrowSize.x / 5, arrowSize.y * 4/5}, arrowVertexColor},
                    {{arrowSize.x / 2, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y * 4/5}, arrowVertexColor}
                });
            }
            else // Spin button lies horizontal
            {
                target.drawFilledRect(states, {arrowSize.y, arrowSize.x}, arrowBackColor);

                target.drawTriangles(states, {
                    {{arrowSize.x * 4/5, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x / 5, arrowSize.y / 2}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y * 4/5}, arrowVertexColor}
                });
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
        if (m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
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

                target.drawTriangles(states, {
                    {{arrowSize.x / 5, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x / 2, arrowSize.y * 4/5}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y / 5}, arrowVertexColor}
                });
            }
            else // Spin button lies horizontal
            {
                target.drawFilledRect(states, {arrowSize.y, arrowSize.x}, arrowBackColor);

                target.drawTriangles(states, {
                    {{arrowSize.x / 5, arrowSize.y / 5}, arrowVertexColor},
                    {{arrowSize.x * 4/5, arrowSize.y / 2}, arrowVertexColor},
                    {{arrowSize.x / 5, arrowSize.y * 4/5}, arrowVertexColor}
                });
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::CallMousePressPeriodically(std::chrono::time_point<std::chrono::steady_clock> clicked)
    {
        tgui::Timer::scheduleCallback([this, clicked]()
            {
                // Mouse still over and the mouse press is current
                if (!m_mouseHover || !m_mouseDown || m_PressedAt != clicked)
                {
                    return;
                }

                if (m_value < m_maximum && m_mouseDownOnTopArrow && m_mouseHoverOnTopArrow)
                {
                    setValue(m_value + m_step);
                    CallMousePressPeriodically(clicked);
                }
                else if (m_value > m_minimum && !m_mouseDownOnTopArrow && !m_mouseHoverOnTopArrow)
                {
                    setValue(m_value - m_step);
                    CallMousePressPeriodically(clicked);
                }
            }, std::chrono::milliseconds(300));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
