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


#include <TGUI/Widgets/Scrollbar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar()
    {
        m_callback.widgetType = "Scrollbar";
        m_type = "Scrollbar";

        addSignal<int>("ValueChanged");
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ScrollbarRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setTrackColor({245, 245, 245});
        getRenderer()->setThumbColor({220, 220, 220});
        getRenderer()->setThumbColorHover({230, 230, 230});
        getRenderer()->setArrowBackgroundColor({245, 245, 245});
        getRenderer()->setArrowBackgroundColorHover({255, 255, 255});
        getRenderer()->setArrowColor({60, 60, 60});
        getRenderer()->setArrowColorHover({0, 0, 0});

        setSize(16, 160);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr Scrollbar::copy(Scrollbar::ConstPtr scrollbar)
    {
        if (scrollbar)
            return std::static_pointer_cast<Scrollbar>(scrollbar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getSize().x <= getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        bool textured = false;
        if (getRenderer()->getTextureTrack().isLoaded() && getRenderer()->getTextureThumb().isLoaded()
         && getRenderer()->getTextureArrowUp().isLoaded() && getRenderer()->getTextureArrowDown().isLoaded())
        {
            textured = true;
        }

        if (m_verticalScroll)
        {
            m_arrowUp.width = getSize().x;
            m_arrowDown.width = getSize().x;

            if (textured)
            {
                m_arrowUp.height = getSize().x * getRenderer()->getTextureArrowUp().getImageSize().x / getRenderer()->getTextureArrowUp().getImageSize().y;
                m_arrowDown.height = getSize().x * getRenderer()->getTextureArrowDown().getImageSize().x / getRenderer()->getTextureArrowDown().getImageSize().y;
            }
            else
            {
                m_arrowUp.height = m_arrowUp.width;
                m_arrowDown.height = m_arrowUp.width;
            }

            m_track.width = getSize().x;
            m_track.height = std::max(0.f, getSize().y - m_arrowUp.height - m_arrowDown.height);

            m_thumb.width = getSize().x;
            if (m_maximum > m_lowValue)
                m_thumb.height = m_track.height * m_lowValue / m_maximum;
            else
                m_thumb.height = m_track.height;
        }
        else // The scrollbar lies horizontally
        {
            m_arrowUp.height = getSize().y;
            m_arrowDown.height = getSize().y;

            if (textured)
            {
                m_arrowUp.width = getSize().y * getRenderer()->getTextureArrowUp().getImageSize().x / getRenderer()->getTextureArrowUp().getImageSize().y;
                m_arrowDown.width = getSize().y * getRenderer()->getTextureArrowDown().getImageSize().x / getRenderer()->getTextureArrowDown().getImageSize().y;
            }
            else
            {
                m_arrowUp.width = m_arrowUp.height;
                m_arrowDown.width = m_arrowUp.height;
            }

            m_track.width = std::max(0.f, getSize().x - m_arrowUp.height - m_arrowDown.height);
            m_track.height = getSize().y;

            m_thumb.height = getSize().y;
            if (m_maximum > m_lowValue)
                m_thumb.width = m_track.width * m_lowValue / m_maximum;
            else
                m_thumb.width = m_track.width;
        }

        if (textured)
        {
            getRenderer()->getTextureArrowUp().setSize({m_arrowUp.width, m_arrowUp.height});
            getRenderer()->getTextureArrowUpHover().setSize({m_arrowUp.width, m_arrowUp.height});
            getRenderer()->getTextureArrowDown().setSize({m_arrowDown.width, m_arrowDown.height});
            getRenderer()->getTextureArrowDownHover().setSize({m_arrowDown.width, m_arrowDown.height});

            if (m_verticalScroll == m_verticalImage)
            {
                getRenderer()->getTextureTrack().setSize({m_track.width, m_track.height});
                getRenderer()->getTextureTrackHover().setSize({m_track.width, m_track.height});
                getRenderer()->getTextureThumb().setSize({m_thumb.width, m_thumb.height});
                getRenderer()->getTextureThumbHover().setSize({m_thumb.width, m_thumb.height});

                getRenderer()->getTextureTrack().setRotation(0);
                getRenderer()->getTextureTrackHover().setRotation(0);
                getRenderer()->getTextureThumb().setRotation(0);
                getRenderer()->getTextureThumbHover().setRotation(0);
            }
            else
            {
                getRenderer()->getTextureTrack().setSize({m_track.height, m_track.width});
                getRenderer()->getTextureTrackHover().setSize({m_track.height, m_track.width});
                getRenderer()->getTextureThumb().setSize({m_thumb.height, m_thumb.width});
                getRenderer()->getTextureThumbHover().setSize({m_thumb.height, m_thumb.width});

                getRenderer()->getTextureTrack().setRotation(-90);
                getRenderer()->getTextureTrackHover().setRotation(-90);
                getRenderer()->getTextureThumb().setRotation(-90);
                getRenderer()->getTextureThumbHover().setRotation(-90);
            }

            // Set the rotation or the arrows now that the size has been set
            if (m_verticalScroll)
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
        }

        // Recalculate the position of the track, thumb and arrows
        updateThumbPosition();
        if (m_verticalScroll)
        {
            m_arrowDown.left = 0;
            m_arrowDown.top = getSize().y - m_arrowDown.height;

            m_track.left = 0;
            m_track.top = m_arrowUp.height;
        }
        else
        {
            m_arrowDown.left = getSize().x - m_arrowDown.width;
            m_arrowDown.top = 0;

            m_track.left = m_arrowUp.width;
            m_track.top = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_maximum = maximum;
        else
            m_maximum = 1;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_lowValue)
            setValue(0);
        else if (m_value > m_maximum - m_lowValue)
            setValue(m_maximum - m_lowValue);

        // Recalculate the size and position of the thumb image
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setValue(unsigned int value)
    {
        // When the value is above the maximum then adjust it
        if (m_maximum < m_lowValue)
            value = 0;
        else if (value > m_maximum - m_lowValue)
            value = m_maximum - m_lowValue;

        if (m_value != value)
        {
            m_value = value;

            m_callback.value = static_cast<int>(m_value);
            sendSignal("ValueChanged", static_cast<int>(m_value));

            // Recalculate the size and position of the thumb image
            updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setLowValue(unsigned int lowValue)
    {
        // Set the new value
        m_lowValue = lowValue;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_lowValue)
            setValue(0);
        else if (m_value > m_maximum - m_lowValue)
            setValue(m_maximum - m_lowValue);

        // Recalculate the size and position of the thumb image
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getLowValue() const
    {
        return m_lowValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setScrollAmount(unsigned int scrollAmount)
    {
        m_scrollAmount = scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getScrollAmount() const
    {
        return m_scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setAutoHide(bool autoHide)
    {
        m_autoHide = autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getAutoHide() const
    {
        return m_autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseOnWidget(float x, float y) const
    {
        // Don't make any calculations when no scrollbar is needed
        if ((m_maximum <= m_lowValue) && m_autoHide)
            return false;

        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMousePressed(float x, float y)
    {
        m_mouseDown = true;
        m_mouseDownOnArrow = false;

        x -= getPosition().x;
        y -= getPosition().y;

        if (m_verticalScroll)
        {
            // Check if the arrows are drawn at full size
            if (getSize().y > m_arrowUp.height + m_arrowDown.height)
            {
                // Check if you clicked on one of the arrows
                if ((y < m_arrowUp.height) || (y > getSize().y - m_arrowUp.height))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }
        else
        {
            // Check if the arrows are drawn at full size
            if (getSize().x > m_arrowUp.height + m_arrowDown.height)
            {
                // Check if you clicked on one of the arrows
                if ((x < m_arrowUp.height) || (x > getSize().x - m_arrowUp.height))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(x, y))
        {
            m_mouseDownOnThumbPos.x = x - m_thumb.left;
            m_mouseDownOnThumbPos.y = y - m_thumb.top;

            m_mouseDownOnThumb = true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the scrollbar value
        if (!m_mouseDownOnArrow)
            mouseMoved(getPosition().x + x, getPosition().y + y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseReleased(float x, float y)
    {
        // Check if one of the arrows was clicked
        if (m_mouseDown && m_mouseDownOnArrow)
        {
            // Only continue when the calculations can be made
            if (m_maximum > m_lowValue)
            {
                bool valueDown = false;
                bool valueUp = false;

                x -= getPosition().x;
                y -= getPosition().y;

                // Check in which direction the scrollbar lies
                if (m_verticalScroll)
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().y > m_arrowUp.height + m_arrowDown.height)
                    {
                        // Check if you clicked on the top arrow
                        if (y < m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (y > getSize().y - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (y < getSize().y * 0.5f)
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }
                else // the scrollbar lies horizontal
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().x > m_arrowUp.height + m_arrowDown.height)
                    {
                        // Check if you clicked on the top arrow
                        if (x < m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (x > getSize().x - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (x < getSize().x * 0.5f)
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }

                if (valueDown)
                {
                    if (m_value > m_scrollAmount)
                    {
                        if (m_value % m_scrollAmount)
                            setValue(m_value - (m_value % m_scrollAmount));
                        else
                            setValue(m_value - m_scrollAmount);
                    }
                    else
                        setValue(0);
                }
                else if (valueUp)
                {
                    if (m_value + m_scrollAmount < m_maximum - m_lowValue + 1)
                    {
                        if (m_value % m_scrollAmount)
                            setValue(m_value + (m_scrollAmount - (m_value % m_scrollAmount)));
                        else
                            setValue(m_value + m_scrollAmount);
                    }
                    else
                        setValue(m_maximum - m_lowValue);
                }
            }
        }

        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        x -= getPosition().x;
        y -= getPosition().y;

        // Check if the mouse button went down on top of the track (or thumb)
        if (m_mouseDown && !m_mouseDownOnArrow)
        {
            // Don't continue if the calculations can't be made
            if (!m_autoHide && (m_maximum <= m_lowValue))
                return;

            // Check in which direction the scrollbar lies
            if (m_verticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((y - m_mouseDownOnThumbPos.y - m_arrowUp.height) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((y - m_mouseDownOnThumbPos.y - m_arrowUp.height)
                                                                         / (getSize().y - m_arrowUp.height - m_arrowDown.height)) * m_maximum) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_lowValue))
                            setValue(value);
                        else
                            setValue(m_maximum - m_lowValue);
                    }
                    else // The mouse was above the scrollbar
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    float thumbTop = y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop - m_arrowUp.height > 0) && (thumbTop + m_thumb.height + m_arrowDown.height < getSize().y))
                        m_thumb.top = thumbTop;
                    else // Prevent the thumb from going outside the scrollbar
                        m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_lowValue));
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (y > m_arrowUp.height)
                    {
                        // Make sure that you did not click on the down arrow
                        if (y <= getSize().y - m_arrowUp.height)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((y - m_arrowUp.height) / (getSize().y - m_arrowUp.height - m_arrowDown.height)) * m_maximum);

                            // Check if you clicked above the thumb
                            if (value <= m_value)
                            {
                                float subtractValue = m_lowValue / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occurred below the thumb
                            {
                                float subtractValue = m_lowValue * 2.0f / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_maximum - m_lowValue + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_lowValue);
                            }
                        }
                    }

                    m_mouseDownOnThumbPos.x = x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = y - m_thumb.top;
                    m_mouseDownOnThumb = true;
                }
            }
            else // the scrollbar lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if (x - m_mouseDownOnThumbPos.x - m_arrowUp.width > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((x - m_mouseDownOnThumbPos.x - m_arrowUp.width)
                                                                         / (getSize().x - m_arrowUp.width - m_arrowDown.width)) * m_maximum) + 0.5f);
                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_lowValue))
                            setValue(value);
                        else
                            setValue(m_maximum - m_lowValue);
                    }
                    else // The mouse was to the left of the thumb
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    float thumbLeft = x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft - m_arrowUp.width > 0) && (thumbLeft + m_thumb.width + m_arrowDown.width < getSize().x))
                        m_thumb.left = thumbLeft;
                    else // Prevent the thumb from going outside the scrollbar
                        m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_lowValue));
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (x > m_arrowUp.width)
                    {
                        // Make sure that you did not click on the down arrow
                        if (x <= getSize().x - m_arrowUp.width)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((x - m_arrowUp.width) / (getSize().x - m_arrowUp.width - m_arrowDown.width)) * m_maximum);

                            // Check if you clicked to the left of the thumb
                            if (value <= m_value)
                            {
                                float subtractValue = m_lowValue / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occurred to the right of the thumb
                            {
                                float subtractValue = m_lowValue * 2.0f / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_maximum - m_lowValue + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_lowValue);
                            }
                        }
                    }

                    m_mouseDownOnThumbPos.x = x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = y - m_thumb.top;
                    m_mouseDownOnThumb = true;
                }
            }
        }

        if (sf::FloatRect{m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height}.contains(x, y))
            m_mouseHoverOverPart = Part::Thumb;
        else if (sf::FloatRect{m_track.left, m_track.top, m_track.width, m_track.height}.contains(x, y))
            m_mouseHoverOverPart = Part::Track;
        else if (sf::FloatRect{m_arrowUp.left, m_arrowUp.top, m_arrowUp.width, m_arrowUp.height}.contains(x, y))
            m_mouseHoverOverPart = Part::ArrowUp;
        else if (sf::FloatRect{m_arrowDown.left, m_arrowDown.top, m_arrowDown.width, m_arrowDown.height}.contains(x, y))
            m_mouseHoverOverPart = Part::ArrowDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseWheelMoved(int delta, int x, int y)
    {
        if (static_cast<int>(m_value) - static_cast<int>(delta * m_scrollAmount) < 0)
            setValue(0);
        else
            setValue(static_cast<unsigned int>(m_value - (delta * m_scrollAmount)));

        // Find out over which part the mouse is hovering
        if (sf::FloatRect{m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height}.contains(static_cast<float>(x), static_cast<float>(y)))
            m_mouseHoverOverPart = Part::Thumb;
        else if (sf::FloatRect{m_track.left, m_track.top, m_track.width, m_track.height}.contains(static_cast<float>(x), static_cast<float>(y)))
            m_mouseHoverOverPart = Part::Track;
        else if (sf::FloatRect{m_arrowUp.left, m_arrowUp.top, m_arrowUp.width, m_arrowUp.height}.contains(static_cast<float>(x), static_cast<float>(y)))
            m_mouseHoverOverPart = Part::ArrowUp;
        else if (sf::FloatRect{m_arrowDown.left, m_arrowDown.top, m_arrowDown.width, m_arrowDown.height}.contains(static_cast<float>(x), static_cast<float>(y)))
            m_mouseHoverOverPart = Part::ArrowDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::widgetFocused()
    {
        // A scrollbar can't be focused (yet)
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if ((property == "texturetrack") || (property == "texturetrackhover")
              || (property == "texturethumb") || (property == "texturethumbhover")
              || (property == "texturearrowup") || (property == "texturearrowuphover")
              || (property == "texturearrowdown") || (property == "texturearrowdownhover"))
        {
            updateSize();
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();
            getRenderer()->getTextureTrack().setOpacity(opacity);
            getRenderer()->getTextureTrackHover().setOpacity(opacity);
            getRenderer()->getTextureThumb().setOpacity(opacity);
            getRenderer()->getTextureThumbHover().setOpacity(opacity);
            getRenderer()->getTextureArrowUp().setOpacity(opacity);
            getRenderer()->getTextureArrowUpHover().setOpacity(opacity);
            getRenderer()->getTextureArrowDown().setOpacity(opacity);
            getRenderer()->getTextureArrowDownHover().setOpacity(opacity);
        }
        else if ((property != "trackcolor")
              && (property != "trackcolorhover")
              && (property != "thumbcolor")
              && (property != "thumbcolorhover")
              && (property != "arrowbackgroundcolor")
              && (property != "arrowbackgroundcolorhover")
              && (property != "arrowcolor")
              && (property != "arrowcolorhover")
              && (property != "bordercolor")
              && (property != "bordercolorhover"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = 0;
            m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_lowValue));
        }
        else
        {
            m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_lowValue));
            m_thumb.top = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw the scrollbar when it is not needed
        if (m_autoHide && (m_maximum <= m_lowValue))
            return;

        states.transform.translate(getPosition());

        bool textured = false;
        if (getRenderer()->getTextureTrack().isLoaded() && getRenderer()->getTextureThumb().isLoaded()
         && getRenderer()->getTextureArrowUp().isLoaded() && getRenderer()->getTextureArrowDown().isLoaded())
        {
            textured = true;
        }

        // Draw arrow up/left
        if (textured)
        {
            if (m_mouseHover && getRenderer()->getTextureArrowUpHover().isLoaded() && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp))
                getRenderer()->getTextureArrowUpHover().draw(target, states);
            else
                getRenderer()->getTextureArrowUp().draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp) && getRenderer()->getArrowBackgroundColorHover().isSet())
                drawRectangleShape(target, states, {m_arrowUp.width, m_arrowUp.height}, getRenderer()->getArrowBackgroundColorHover());
            else
                drawRectangleShape(target, states, {m_arrowUp.width, m_arrowUp.height}, getRenderer()->getArrowBackgroundColor());

            sf::ConvexShape arrow{3};
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp) && getRenderer()->getArrowColorHover().isSet())
                arrow.setFillColor(calcColorOpacity(getRenderer()->getArrowColorHover(), getRenderer()->getOpacity()));
            else
                arrow.setFillColor(calcColorOpacity(getRenderer()->getArrowColor(), getRenderer()->getOpacity()));

            if (m_verticalScroll)
            {
                arrow.setPoint(0, {m_arrowUp.width / 5, m_arrowUp.height * 4/5});
                arrow.setPoint(1, {m_arrowUp.width / 2, m_arrowUp.height / 5});
                arrow.setPoint(2, {m_arrowUp.width * 4/5, m_arrowUp.height * 4/5});
            }
            else
            {
                arrow.setPoint(0, {m_arrowUp.width * 4/5, m_arrowUp.height / 5});
                arrow.setPoint(1, {m_arrowUp.width / 5, m_arrowUp.height / 2});
                arrow.setPoint(2, {m_arrowUp.width * 4/5, m_arrowUp.height * 4/5});
            }

            target.draw(arrow, states);
        }

        // Draw the track
        states.transform.translate({m_track.left, m_track.top});
        if (textured)
        {
            if (m_mouseHover && getRenderer()->getTextureTrackHover().isLoaded() && (m_mouseHoverOverPart == Scrollbar::Part::Track))
                getRenderer()->getTextureTrackHover().draw(target, states);
            else
                getRenderer()->getTextureTrack().draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::Track) && getRenderer()->getTrackColorHover().isSet())
                drawRectangleShape(target, states, {m_track.width, m_track.height}, getRenderer()->getTrackColorHover());
            else
                drawRectangleShape(target, states, {m_track.width, m_track.height}, getRenderer()->getTrackColor());
        }
        states.transform.translate({-m_track.left, -m_track.top});

        // Draw the thumb
        states.transform.translate({m_thumb.left, m_thumb.top});
        if (textured)
        {
            if (m_mouseHover && getRenderer()->getTextureThumbHover().isLoaded() && (m_mouseHoverOverPart == Scrollbar::Part::Thumb))
                getRenderer()->getTextureThumbHover().draw(target, states);
            else
                getRenderer()->getTextureThumb().draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::Thumb) && getRenderer()->getThumbColorHover().isSet())
                drawRectangleShape(target, states, {m_thumb.width, m_thumb.height}, getRenderer()->getThumbColorHover());
            else
                drawRectangleShape(target, states, {m_thumb.width, m_thumb.height}, getRenderer()->getThumbColor());
        }
        states.transform.translate({-m_thumb.left, -m_thumb.top});

        // Draw arrow down/right
        states.transform.translate({m_arrowDown.left, m_arrowDown.top});
        if (textured)
        {
            if (m_mouseHover && getRenderer()->getTextureArrowDownHover().isLoaded() && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown))
                getRenderer()->getTextureArrowDownHover().draw(target, states);
            else
                getRenderer()->getTextureArrowDown().draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown) && getRenderer()->getArrowBackgroundColorHover().isSet())
                drawRectangleShape(target, states, {m_arrowDown.width, m_arrowDown.height}, getRenderer()->getArrowBackgroundColorHover());
            else
                drawRectangleShape(target, states, {m_arrowDown.width, m_arrowDown.height}, getRenderer()->getArrowBackgroundColor());

            sf::ConvexShape arrow{3};
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown) && getRenderer()->getArrowColorHover().isSet())
                arrow.setFillColor(calcColorOpacity(getRenderer()->getArrowColorHover(), getRenderer()->getOpacity()));
            else
                arrow.setFillColor(calcColorOpacity(getRenderer()->getArrowColor(), getRenderer()->getOpacity()));

            if (m_verticalScroll)
            {
                arrow.setPoint(0, {m_arrowDown.width / 5, m_arrowDown.height / 5});
                arrow.setPoint(1, {m_arrowDown.width / 2, m_arrowDown.height * 4/5});
                arrow.setPoint(2, {m_arrowDown.width * 4/5, m_arrowDown.height / 5});
            }
            else // Spin button lies horizontal
            {
                arrow.setPoint(0, {m_arrowDown.width / 5, m_arrowDown.height / 5});
                arrow.setPoint(1, {m_arrowDown.width * 4/5, m_arrowDown.height / 2});
                arrow.setPoint(2, {m_arrowDown.width / 5, m_arrowDown.height * 4/5});
            }

            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isMouseDown() const
    {
        return m_mouseDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isMouseDownOnThumb() const
    {
        return m_mouseDownOnThumb;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
