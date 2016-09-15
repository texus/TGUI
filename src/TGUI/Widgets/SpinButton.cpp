/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton()
    {
        m_callback.widgetType = "SpinButton";
        m_type = "SpinButton";

        addSignal<int>("ValueChanged");

        m_renderer = aurora::makeCopied<SpinButtonRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({2});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setBackgroundColorHover({255, 255, 255});
        getRenderer()->setArrowColor({60, 60, 60});
        getRenderer()->setArrowColorHover({0, 0, 0});
        getRenderer()->setSpaceBetweenArrows(2);

        setSize(20, 42);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::create(int minimum, int maximum)
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

    void SpinButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        sf::Vector2f arrowSize = getArrowSize();
        getRenderer()->getTextureArrowUp().setSize(arrowSize);
        getRenderer()->getTextureArrowUpHover().setSize(arrowSize);
        getRenderer()->getTextureArrowDown().setSize(arrowSize);
        getRenderer()->getTextureArrowDownHover().setSize(arrowSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMinimum(int minimum)
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

    int SpinButton::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMaximum(int maximum)
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

    int SpinButton::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setValue(int value)
    {
        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            m_callback.value = m_value;
            sendSignal("ValueChanged", value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int SpinButton::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setVerticalScroll(bool verticalScroll)
    {
        m_verticalScroll = verticalScroll;

        if (verticalScroll)
        {
            getRenderer()->getTextureArrowUp().setRotation(0);
            getRenderer()->getTextureArrowUpHover().setRotation(0);
            getRenderer()->getTextureArrowDown().setRotation(0);
            getRenderer()->getTextureArrowDownHover().setRotation(0);
        }
        else
        {
            getRenderer()->getTextureArrowUp().setRotation(-90);
            getRenderer()->getTextureArrowUpHover().setRotation(-90);
            getRenderer()->getTextureArrowDown().setRotation(-90);
            getRenderer()->getTextureArrowDownHover().setRotation(-90);
        }

        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMousePressed(sf::Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (sf::FloatRect{0, 0, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseDownOnTopArrow = true;
            else
                m_mouseDownOnTopArrow = false;
        }
        else
        {
            if (sf::FloatRect{0, 0, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseDownOnTopArrow = false;
            else
                m_mouseDownOnTopArrow = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMouseReleased(sf::Vector2f pos)
    {
        // Check if the mouse went down on the spin button
        if (m_mouseDown)
        {
            // Check if the arrow went down on the top/right arrow
            if (m_mouseDownOnTopArrow)
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (sf::FloatRect{0, 0, getSize().x, getSize().y / 2.f}.contains(pos)))
                 || (!m_verticalScroll && (!sf::FloatRect{0, 0, getSize().x / 2.f, getSize().y}.contains(pos))))
                {
                    // Increment the value
                    if (m_value < m_maximum)
                        setValue(m_value + 1);
                    else
                        return;
                }
                else
                    return;
            }
            else // The mouse went down on the bottom/left arrow
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (!sf::FloatRect{0, 0, getSize().x, getSize().y / 2.f}.contains(pos)))
                 || (!m_verticalScroll && (sf::FloatRect{0, 0, getSize().x / 2.f, getSize().y}.contains(pos))))
                {
                    // Decrement the value
                    if (m_value > m_minimum)
                        setValue(m_value - 1);
                    else
                        return;
                }
                else
                    return;
            }
        }

        ClickableWidget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(sf::Vector2f pos)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (sf::FloatRect{0, 0, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }
        else
        {
            if (sf::FloatRect{0, 0, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }

        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::widgetFocused()
    {
        // A spin button can't be focused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if ((property == "borders") || (property == "spacebetweenarrows"))
        {
            updateSize();
        }
        else if ((property == "texturearrowup") || (property == "texturearrowuphover") || (property == "texturearrowdown") || (property == "texturearrowdownhover"))
        {
            value.getTexture().setSize(getArrowSize());
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();
            getRenderer()->getTextureArrowUp().setOpacity(opacity);
            getRenderer()->getTextureArrowUpHover().setOpacity(opacity);
            getRenderer()->getTextureArrowDown().setOpacity(opacity);
            getRenderer()->getTextureArrowDownHover().setOpacity(opacity);
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolor")
              && (property != "backgroundcolorhover")
              && (property != "arrowcolor")
              && (property != "arrowcolorhover"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpinButton::getArrowSize() const
    {
        Borders borders = getRenderer()->getBorders();
        if (m_verticalScroll)
            return {getSize().x - borders.left - borders.right, (getSize().y - borders.top - borders.bottom - getRenderer()->getSpaceBetweenArrows()) / 2.0f};
        else
            return {getSize().y - borders.top - borders.bottom, (getSize().x - borders.left - borders.right - getRenderer()->getSpaceBetweenArrows()) / 2.0f};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        sf::Vector2f arrowSize = getArrowSize();
        float spaceBetweenArrows = getRenderer()->getSpaceBetweenArrows();

        // Draw the top/left arrow
        if (getRenderer()->getTextureArrowUp().isLoaded() && getRenderer()->getTextureArrowDown().isLoaded())
        {
            if (m_mouseHover && m_mouseHoverOnTopArrow && getRenderer()->getTextureArrowUpHover().isLoaded())
                getRenderer()->getTextureArrowUpHover().draw(target, states);
            else
                getRenderer()->getTextureArrowUp().draw(target, states);
        }
        else
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;

            if (m_verticalScroll)
            {
                arrowBack.setSize(arrowSize);

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y * 4/5});
                arrow.setPoint(1, {arrowBack.getSize().x / 2, arrowBack.getSize().y / 5});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y * 4/5});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({arrowSize.y, arrowSize.x});

                arrow.setPoint(0, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 2});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y * 4/5});
            }

            if (m_mouseHover && m_mouseHoverOnTopArrow && getRenderer()->getBackgroundColorHover().isSet())
                arrowBack.setFillColor(Color::calcColorOpacity(getRenderer()->getBackgroundColorHover(), getRenderer()->getOpacity()));
            else
                arrowBack.setFillColor(Color::calcColorOpacity(getRenderer()->getBackgroundColor(), getRenderer()->getOpacity()));

            if (m_mouseHover && m_mouseHoverOnTopArrow && getRenderer()->getArrowColorHover().isSet())
                arrow.setFillColor(Color::calcColorOpacity(getRenderer()->getArrowColorHover(), getRenderer()->getOpacity()));
            else
                arrow.setFillColor(Color::calcColorOpacity(getRenderer()->getArrowColor(), getRenderer()->getOpacity()));

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }

        // Draw the space between the arrows (if there is space)
        if (m_verticalScroll)
        {
            states.transform.translate({0, arrowSize.y});

            if (spaceBetweenArrows > 0)
            {
                drawRectangleShape(target, states, {arrowSize.x, spaceBetweenArrows}, getRenderer()->getBorderColor());
                states.transform.translate({0, spaceBetweenArrows});
            }
        }
        else // Horizontal orientation
        {
            states.transform.translate({arrowSize.y, 0});

            if (spaceBetweenArrows > 0)
            {
                drawRectangleShape(target, states, {spaceBetweenArrows, arrowSize.y}, getRenderer()->getBorderColor());
                states.transform.translate({spaceBetweenArrows, 0});
            }
        }

        // Draw the bottom/right arrow
        if (getRenderer()->getTextureArrowUp().isLoaded() && getRenderer()->getTextureArrowDown().isLoaded())
        {
            if (m_mouseHover && !m_mouseHoverOnTopArrow && getRenderer()->getTextureArrowDownHover().isLoaded())
                getRenderer()->getTextureArrowDownHover().draw(target, states);
            else
                getRenderer()->getTextureArrowDown().draw(target, states);
        }
        else // There are no images
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;

            if (m_verticalScroll)
            {
                arrowBack.setSize(arrowSize);

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x / 2, arrowBack.getSize().y * 4/5});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 5});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({arrowSize.y, arrowSize.x});

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 2});
                arrow.setPoint(2, {arrowBack.getSize().x / 5, arrowBack.getSize().y * 4/5});
            }

            if (m_mouseHover && !m_mouseHoverOnTopArrow)
                arrowBack.setFillColor(Color::calcColorOpacity(getRenderer()->getBackgroundColorHover(), getRenderer()->getOpacity()));
            else
                arrowBack.setFillColor(Color::calcColorOpacity(getRenderer()->getBackgroundColor(), getRenderer()->getOpacity()));

            if (m_mouseHover && !m_mouseHoverOnTopArrow)
                arrow.setFillColor(Color::calcColorOpacity(getRenderer()->getArrowColorHover(), getRenderer()->getOpacity()));
            else
                arrow.setFillColor(Color::calcColorOpacity(getRenderer()->getArrowColor(), getRenderer()->getOpacity()));

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
