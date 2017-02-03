/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Loading/Theme.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar()
    {
        m_callback.widgetType = "Scrollbar";
        m_draggableWidget = true;

        addSignal<int>("ValueChanged");

        m_renderer = std::make_shared<ScrollbarRenderer>(this);
        reload();

        setSize(16, 160);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr Scrollbar::create()
    {
        return std::make_shared<Scrollbar>();
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

    void Scrollbar::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        m_arrowUp.left = getPosition().x;
        m_arrowUp.top = getPosition().y;

        if (m_verticalScroll)
        {
            m_arrowDown.left = getPosition().x;
            m_arrowDown.top = getPosition().y + getSize().y - m_arrowDown.height;

            m_track.left = getPosition().x;
            m_track.top = getPosition().y + m_arrowUp.height;

            m_thumb.left = getPosition().x;
            m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_lowValue));
        }
        else
        {
            m_arrowDown.left = getPosition().x + getSize().x - m_arrowDown.width;
            m_arrowDown.top = getPosition().y;

            m_track.left = getPosition().x + m_arrowUp.width;
            m_track.top = getPosition().y;

            m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_lowValue));
            m_thumb.top = getPosition().y;
        }

        // Pass the position to the textures if they exist
        if (getRenderer()->m_textureTrackNormal.isLoaded() && getRenderer()->m_textureThumbNormal.isLoaded()
         && getRenderer()->m_textureArrowUpNormal.isLoaded() && getRenderer()->m_textureArrowDownNormal.isLoaded())
        {
            getRenderer()->m_textureArrowUpNormal.setPosition({m_arrowUp.left, m_arrowUp.top});
            getRenderer()->m_textureArrowUpHover.setPosition({m_arrowUp.left, m_arrowUp.top});
            getRenderer()->m_textureArrowDownNormal.setPosition({m_arrowDown.left, m_arrowDown.top});
            getRenderer()->m_textureArrowDownHover.setPosition({m_arrowDown.left, m_arrowDown.top});
            getRenderer()->m_textureTrackNormal.setPosition({m_track.left, m_track.top});
            getRenderer()->m_textureTrackHover.setPosition({m_track.left, m_track.top});
            getRenderer()->m_textureThumbNormal.setPosition({m_thumb.left, m_thumb.top});
            getRenderer()->m_textureThumbHover.setPosition({m_thumb.left, m_thumb.top});
        }
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
        if (getRenderer()->m_textureTrackNormal.isLoaded() && getRenderer()->m_textureThumbNormal.isLoaded()
         && getRenderer()->m_textureArrowUpNormal.isLoaded() && getRenderer()->m_textureArrowDownNormal.isLoaded())
        {
            textured = true;
        }

        if (m_verticalScroll)
        {
            m_arrowUp.width = getSize().x;
            m_arrowDown.width = getSize().x;

            if (textured)
            {
                m_arrowUp.height = getSize().x * getRenderer()->m_textureArrowUpNormal.getImageSize().x / getRenderer()->m_textureArrowUpNormal.getImageSize().y;
                m_arrowDown.height = getSize().x * getRenderer()->m_textureArrowDownNormal.getImageSize().x / getRenderer()->m_textureArrowDownNormal.getImageSize().y;
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
                m_arrowUp.width = getSize().y * getRenderer()->m_textureArrowUpNormal.getImageSize().x / getRenderer()->m_textureArrowUpNormal.getImageSize().y;
                m_arrowDown.width = getSize().y * getRenderer()->m_textureArrowDownNormal.getImageSize().x / getRenderer()->m_textureArrowDownNormal.getImageSize().y;
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
            getRenderer()->m_textureArrowUpNormal.setSize({m_arrowUp.width, m_arrowUp.height});
            getRenderer()->m_textureArrowUpHover.setSize({m_arrowUp.width, m_arrowUp.height});
            getRenderer()->m_textureArrowDownNormal.setSize({m_arrowDown.width, m_arrowDown.height});
            getRenderer()->m_textureArrowDownHover.setSize({m_arrowDown.width, m_arrowDown.height});

            if (m_verticalScroll == m_verticalImage)
            {
                getRenderer()->m_textureTrackNormal.setSize({m_track.width, m_track.height});
                getRenderer()->m_textureTrackHover.setSize({m_track.width, m_track.height});
                getRenderer()->m_textureThumbNormal.setSize({m_thumb.width, m_thumb.height});
                getRenderer()->m_textureThumbHover.setSize({m_thumb.width, m_thumb.height});

                getRenderer()->m_textureTrackNormal.setRotation(0);
                getRenderer()->m_textureTrackHover.setRotation(0);
                getRenderer()->m_textureThumbNormal.setRotation(0);
                getRenderer()->m_textureThumbHover.setRotation(0);
            }
            else
            {
                getRenderer()->m_textureTrackNormal.setSize({m_track.height, m_track.width});
                getRenderer()->m_textureTrackHover.setSize({m_track.height, m_track.width});
                getRenderer()->m_textureThumbNormal.setSize({m_thumb.height, m_thumb.width});
                getRenderer()->m_textureThumbHover.setSize({m_thumb.height, m_thumb.width});

                getRenderer()->m_textureTrackNormal.setRotation(-90);
                getRenderer()->m_textureTrackHover.setRotation(-90);
                getRenderer()->m_textureThumbNormal.setRotation(-90);
                getRenderer()->m_textureThumbHover.setRotation(-90);
            }

            // Set the rotation or the arrows now that the size has been set
            if (m_verticalScroll)
            {
                getRenderer()->m_textureArrowUpNormal.setRotation(0);
                getRenderer()->m_textureArrowUpHover.setRotation(0);
                getRenderer()->m_textureArrowDownNormal.setRotation(0);
                getRenderer()->m_textureArrowDownHover.setRotation(0);
            }
            else
            {
                getRenderer()->m_textureArrowUpNormal.setRotation(-90);
                getRenderer()->m_textureArrowUpHover.setRotation(-90);
                getRenderer()->m_textureArrowDownNormal.setRotation(-90);
                getRenderer()->m_textureArrowDownHover.setRotation(-90);
            }
        }

        // Recalculate the position of the images
        updatePosition();
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

    void Scrollbar::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        getRenderer()->m_textureTrackNormal.setColor({getRenderer()->m_textureTrackNormal.getColor().r, getRenderer()->m_textureTrackNormal.getColor().g, getRenderer()->m_textureTrackNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureTrackHover.setColor({getRenderer()->m_textureTrackHover.getColor().r, getRenderer()->m_textureTrackHover.getColor().g, getRenderer()->m_textureTrackHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        getRenderer()->m_textureThumbNormal.setColor({getRenderer()->m_textureThumbNormal.getColor().r, getRenderer()->m_textureThumbNormal.getColor().g, getRenderer()->m_textureThumbNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureThumbHover.setColor({getRenderer()->m_textureThumbHover.getColor().r, getRenderer()->m_textureThumbHover.getColor().g, getRenderer()->m_textureThumbHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        getRenderer()->m_textureArrowUpNormal.setColor({getRenderer()->m_textureArrowUpNormal.getColor().r, getRenderer()->m_textureArrowUpNormal.getColor().g, getRenderer()->m_textureArrowUpNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowUpHover.setColor({getRenderer()->m_textureArrowUpHover.getColor().r, getRenderer()->m_textureArrowUpHover.getColor().g, getRenderer()->m_textureArrowUpHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        getRenderer()->m_textureArrowDownNormal.setColor({getRenderer()->m_textureArrowDownNormal.getColor().r, getRenderer()->m_textureArrowDownNormal.getColor().g, getRenderer()->m_textureArrowDownNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowDownHover.setColor({getRenderer()->m_textureArrowDownHover.getColor().r, getRenderer()->m_textureArrowDownHover.getColor().g, getRenderer()->m_textureArrowDownHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
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

        if (m_verticalScroll)
        {
            // Check if the arrows are drawn at full size
            if (getSize().y > m_arrowUp.height + m_arrowDown.height)
            {
                // Check if you clicked on one of the arrows
                if (y < getPosition().y + m_arrowUp.height)
                    m_mouseDownOnArrow = true;
                else if (y > getPosition().y + getSize().y - m_arrowUp.height)
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
                if (x < getPosition().x + m_arrowUp.height)
                    m_mouseDownOnArrow = true;
                else if (x > getPosition().x + getSize().x - m_arrowUp.height)
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
            mouseMoved(x, y);
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

                // Check in which direction the scrollbar lies
                if (m_verticalScroll)
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().y > m_arrowUp.height + m_arrowDown.height)
                    {
                        // Check if you clicked on the top arrow
                        if (y < getPosition().y + m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (y > getPosition().y + getSize().y - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (y < getPosition().y + (getSize().y * 0.5f))
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
                        if (x < getPosition().x + m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (x > getPosition().x + getSize().x - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (x < getPosition().x + (getSize().x * 0.5f))
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }

                if (valueDown)
                {
                    if (m_value > m_scrollAmount)
                        setValue(m_value - m_scrollAmount);
                    else
                        setValue(0);
                }
                else if (valueUp)
                {
                    if (m_value + m_scrollAmount < m_maximum - m_lowValue + 1)
                        setValue(m_value + m_scrollAmount);
                    else
                        setValue(m_maximum - m_lowValue);
                }
            }
        }

        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

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
                    if ((y - m_mouseDownOnThumbPos.y - getPosition().y - m_arrowUp.height) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((y - m_mouseDownOnThumbPos.y - getPosition().y - m_arrowUp.height)
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
                    if ((thumbTop - m_arrowUp.height > getPosition().y) && (thumbTop + m_thumb.height + m_arrowDown.height < getPosition().y + getSize().y))
                        m_thumb.top = thumbTop;
                    else
                        m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_lowValue));

                    if (getRenderer()->m_textureThumbNormal.isLoaded())
                    {
                        getRenderer()->m_textureThumbNormal.setPosition({m_thumb.left, m_thumb.top});
                        getRenderer()->m_textureThumbHover.setPosition({m_thumb.left, m_thumb.top});
                    }
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (y > getPosition().y + m_arrowUp.height)
                    {
                        // Make sure that you did not click on the down arrow
                        if (y <= getPosition().y + getSize().y - m_arrowUp.height)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((y - getPosition().y - m_arrowUp.height) / (getSize().y - m_arrowUp.height - m_arrowDown.height)) * m_maximum);

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
                    if ((x - m_mouseDownOnThumbPos.x - getPosition().x - m_arrowUp.height) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((x - m_mouseDownOnThumbPos.x - getPosition().x - m_arrowUp.height)
                                                                         / (getSize().x - m_arrowUp.height - m_arrowDown.height)) * m_maximum) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_lowValue))
                            setValue(value);
                        else
                            setValue(m_maximum - m_lowValue);
                    }
                    else // The mouse was above the scrollbar
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    float thumbLeft = x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft - m_arrowUp.width > getPosition().x) && (thumbLeft + m_thumb.width + m_arrowDown.width < getPosition().x + getSize().x))
                        m_thumb.left = thumbLeft;
                    else
                        m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_lowValue));

                    if (getRenderer()->m_textureThumbNormal.isLoaded())
                    {
                        getRenderer()->m_textureThumbNormal.setPosition({m_thumb.left, m_thumb.top});
                        getRenderer()->m_textureThumbHover.setPosition({m_thumb.left, m_thumb.top});
                    }
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (x > getPosition().x + m_arrowUp.height)
                    {
                        // Make sure that you did not click on the down arrow
                        if (x <= getPosition().x + getSize().x - m_arrowUp.height)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((x - getPosition().x - m_arrowUp.height) / (getSize().x - m_arrowUp.height - m_arrowDown.height)) * m_maximum);

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
            updatePosition();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setTrackColorNormal({245, 245, 245});
        getRenderer()->setTrackColorHover({255, 255, 255});
        getRenderer()->setThumbColorNormal({220, 220, 220});
        getRenderer()->setThumbColorHover({210, 210, 210});
        getRenderer()->setArrowColorNormal({60, 60, 60});
        getRenderer()->setArrowColorHover({0, 0, 0});
        getRenderer()->setTrackTexture({});
        getRenderer()->setTrackHoverTexture({});
        getRenderer()->setThumbTexture({});
        getRenderer()->setThumbHoverTexture({});
        getRenderer()->setArrowUpTexture({});
        getRenderer()->setArrowDownTexture({});
        getRenderer()->setArrowUpHoverTexture({});
        getRenderer()->setArrowDownHoverTexture({});

        if (m_theme && primary != "")
        {
            Widget::reload(primary, secondary, force);

            if (force)
            {
                // Make sure the required textures were loaded
                if (getRenderer()->m_textureTrackNormal.isLoaded() && getRenderer()->m_textureThumbNormal.isLoaded()
                 && getRenderer()->m_textureArrowUpNormal.isLoaded() && getRenderer()->m_textureArrowDownNormal.isLoaded())
                {
                    sf::Vector2f trackSize = getRenderer()->m_textureTrackNormal.getImageSize();
                    float arrowsHeight = getRenderer()->m_textureArrowUpNormal.getImageSize().y + getRenderer()->m_textureArrowDownNormal.getImageSize().y;

                    if (compareFloats(compareFloats(trackSize.x, getRenderer()->m_textureArrowUpNormal.getImageSize().x), getRenderer()->m_textureArrowDownNormal.getImageSize().y))
                        m_verticalImage = true;
                    else if (compareFloats(compareFloats(trackSize.y, getRenderer()->m_textureArrowUpNormal.getImageSize().x), getRenderer()->m_textureArrowDownNormal.getImageSize().y))
                        m_verticalImage = false;
                    if (trackSize.x <= trackSize.y)
                        m_verticalImage = true;
                    else
                        m_verticalImage = false;

                    if (m_verticalImage)
                        setSize({trackSize.x, trackSize.y + arrowsHeight});
                    else
                        setSize({trackSize.x + arrowsHeight, trackSize.y});
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw the scrollbar when it is not needed
        if (m_autoHide && (m_maximum <= m_lowValue))
            return;

        // Draw the scrollbar
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "trackcolor")
            setTrackColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "trackcolornormal")
            setTrackColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "trackcolorhover")
            setTrackColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "thumbcolor")
            setThumbColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "thumbcolornormal")
            setThumbColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "thumbcolorhover")
            setThumbColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowbackgroundcolor")
            setArrowBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowbackgroundcolornormal")
            setArrowBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowbackgroundcolorhover")
            setArrowBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolor")
            setArrowColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolornormal")
            setArrowColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolorhover")
            setArrowColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "trackimage")
            setTrackTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "trackhoverimage")
            setTrackHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "thumbimage")
            setThumbTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "thumbhoverimage")
            setThumbHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowupimage")
            setArrowUpTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowdownimage")
            setArrowDownTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowuphoverimage")
            setArrowUpHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowdownhoverimage")
            setArrowDownHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "trackcolor")
                setTrackColor(value.getColor());
            else if (property == "trackcolornormal")
                setTrackColorNormal(value.getColor());
            else if (property == "trackcolorhover")
                setTrackColorHover(value.getColor());
            else if (property == "thumbcolor")
                setThumbColor(value.getColor());
            else if (property == "thumbcolornormal")
                setThumbColorNormal(value.getColor());
            else if (property == "thumbcolorhover")
                setThumbColorHover(value.getColor());
            else if (property == "arrowbackgroundcolor")
                setArrowBackgroundColor(value.getColor());
            else if (property == "arrowbackgroundcolornormal")
                setArrowBackgroundColorNormal(value.getColor());
            else if (property == "arrowbackgroundcolorhover")
                setArrowBackgroundColorHover(value.getColor());
            else if (property == "arrowcolor")
                setArrowColor(value.getColor());
            else if (property == "arrowcolornormal")
                setArrowColorNormal(value.getColor());
            else if (property == "arrowcolorhover")
                setArrowColorHover(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "trackimage")
                setTrackTexture(value.getTexture());
            else if (property == "trackhoverimage")
                setTrackHoverTexture(value.getTexture());
            else if (property == "thumbimage")
                setThumbTexture(value.getTexture());
            else if (property == "thumbhoverimage")
                setThumbHoverTexture(value.getTexture());
            else if (property == "arrowupimage")
                setArrowUpTexture(value.getTexture());
            else if (property == "arrowdownimage")
                setArrowDownTexture(value.getTexture());
            else if (property == "arrowuphoverimage")
                setArrowUpHoverTexture(value.getTexture());
            else if (property == "arrowdownhoverimage")
                setArrowDownHoverTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ScrollbarRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "trackcolor")
            return m_trackColorNormal;
        else if (property == "trackcolornormal")
            return m_trackColorNormal;
        else if (property == "trackcolorhover")
            return m_trackColorHover;
        else if (property == "thumbcolor")
            return m_thumbColorNormal;
        else if (property == "thumbcolornormal")
            return m_thumbColorNormal;
        else if (property == "thumbcolorhover")
            return m_thumbColorHover;
        else if (property == "arrowbackgroundcolor")
            return m_arrowBackgroundColorNormal;
        else if (property == "arrowbackgroundcolornormal")
            return m_arrowBackgroundColorNormal;
        else if (property == "arrowbackgroundcolorhover")
            return m_arrowBackgroundColorHover;
        else if (property == "arrowcolor")
            return m_arrowColorNormal;
        else if (property == "arrowcolornormal")
            return m_arrowColorNormal;
        else if (property == "arrowcolorhover")
            return m_arrowColorHover;
        else if (property == "trackimage")
            return m_textureTrackNormal;
        else if (property == "trackhoverimage")
            return m_textureTrackHover;
        else if (property == "thumbimage")
            return m_textureThumbNormal;
        else if (property == "thumbhoverimage")
            return m_textureThumbHover;
        else if (property == "arrowupimage")
            return m_textureArrowUpNormal;
        else if (property == "arrowdownimage")
            return m_textureArrowDownNormal;
        else if (property == "arrowuphoverimage")
            return m_textureArrowUpHover;
        else if (property == "arrowdownhoverimage")
            return m_textureArrowDownHover;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ScrollbarRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
        {
            pairs["TrackImage"] = m_textureTrackNormal;
            pairs["ThumbImage"] = m_textureThumbNormal;
            pairs["ArrowUpImage"] = m_textureArrowUpNormal;
            pairs["ArrowDownImage"] = m_textureArrowDownNormal;
            if (m_textureTrackHover.isLoaded())
                pairs["TrackHoverImage"] = m_textureTrackHover;
            if (m_textureThumbHover.isLoaded())
                pairs["ThumbHoverImage"] = m_textureThumbHover;
            if (m_textureArrowUpHover.isLoaded())
                pairs["ArrowUpHoverImage"] = m_textureArrowUpHover;
            if (m_textureArrowDownHover.isLoaded())
                pairs["ArrowDownHoverImage"] = m_textureArrowDownHover;
        }
        else
        {
            pairs["TrackColorNormal"] = m_trackColorNormal;
            pairs["TrackColorHover"] = m_trackColorHover;
            pairs["ThumbColorNormal"] = m_thumbColorNormal;
            pairs["ThumbColorHover"] = m_thumbColorHover;
            pairs["ArrowBackgroundColorNormal"] = m_arrowBackgroundColorNormal;
            pairs["ArrowBackgroundColorHover"] = m_arrowBackgroundColorHover;
            pairs["ArrowColorNormal"] = m_arrowColorNormal;
            pairs["ArrowColorHover"] = m_arrowColorHover;
        }

        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackColor(const Color& color)
    {
        setTrackColorNormal(color);
        setTrackColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackColorNormal(const Color& color)
    {
        m_trackColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackColorHover(const Color& color)
    {
        m_trackColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbColor(const Color& color)
    {
        setThumbColorNormal(color);
        setThumbColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbColorNormal(const Color& color)
    {
        m_thumbColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbColorHover(const Color& color)
    {
        m_thumbColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowBackgroundColor(const Color& color)
    {
        setArrowBackgroundColorNormal(color);
        setArrowBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowBackgroundColorNormal(const Color& color)
    {
        m_arrowBackgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowBackgroundColorHover(const Color& color)
    {
        m_arrowBackgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowColor(const Color& color)
    {
        setArrowColorNormal(color);
        setArrowColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowColorNormal(const Color& color)
    {
        m_arrowColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowColorHover(const Color& color)
    {
        m_arrowColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackTexture(const Texture& texture)
    {
        m_textureTrackNormal = texture;
        if (m_textureTrackNormal.isLoaded())
        {
            m_textureTrackNormal.setColor({m_textureTrackNormal.getColor().r, m_textureTrackNormal.getColor().g, m_textureTrackNormal.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackHoverTexture(const Texture& texture)
    {
        m_textureTrackHover = texture;
        if (m_textureTrackHover.isLoaded())
        {
            m_textureTrackHover.setColor({m_textureTrackHover.getColor().r, m_textureTrackHover.getColor().g, m_textureTrackHover.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbTexture(const Texture& texture)
    {
        m_textureThumbNormal = texture;
        if (m_textureThumbNormal.isLoaded())
        {
            m_textureThumbNormal.setColor({m_textureThumbNormal.getColor().r, m_textureThumbNormal.getColor().g, m_textureThumbNormal.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbHoverTexture(const Texture& texture)
    {
        m_textureThumbHover = texture;
        if (m_textureThumbHover.isLoaded())
        {
            m_textureThumbHover.setColor({m_textureThumbHover.getColor().r, m_textureThumbHover.getColor().g, m_textureThumbHover.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowUpTexture(const Texture& texture)
    {
        m_textureArrowUpNormal = texture;
        if (m_textureArrowUpNormal.isLoaded())
        {
            m_textureArrowUpNormal.setColor({m_textureArrowUpNormal.getColor().r, m_textureArrowUpNormal.getColor().g, m_textureArrowUpNormal.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowDownTexture(const Texture& texture)
    {
        m_textureArrowDownNormal = texture;
        if (m_textureArrowDownNormal.isLoaded())
        {
            m_textureArrowDownNormal.setColor({m_textureArrowDownNormal.getColor().r, m_textureArrowDownNormal.getColor().g, m_textureArrowDownNormal.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowUpHoverTexture(const Texture& texture)
    {
        m_textureArrowUpHover = texture;
        if (m_textureArrowUpHover.isLoaded())
        {
            m_textureArrowUpHover.setColor({m_textureArrowUpHover.getColor().r, m_textureArrowUpHover.getColor().g, m_textureArrowUpHover.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowDownHoverTexture(const Texture& texture)
    {
        m_textureArrowDownHover = texture;
        if (m_textureArrowDownHover.isLoaded())
        {
            m_textureArrowDownHover.setColor({m_textureArrowDownHover.getColor().r, m_textureArrowDownHover.getColor().g, m_textureArrowDownHover.getColor().b, static_cast<sf::Uint8>(m_scrollbar->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_scrollbar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded() && m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
        {
            if (m_scrollbar->m_mouseHover && m_textureTrackHover.isLoaded() && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::Track))
                target.draw(m_textureTrackHover, states);
            else
                target.draw(m_textureTrackNormal, states);

            if (m_scrollbar->m_mouseHover && m_textureThumbHover.isLoaded() && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::Thumb))
                target.draw(m_textureThumbHover, states);
            else
                target.draw(m_textureThumbNormal, states);

            if (m_scrollbar->m_mouseHover && m_textureArrowUpHover.isLoaded() && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::ArrowUp))
                target.draw(m_textureArrowUpHover, states);
            else
                target.draw(m_textureArrowUpNormal, states);

            if (m_scrollbar->m_mouseHover && m_textureArrowDownHover.isLoaded() && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::ArrowDown))
                target.draw(m_textureArrowDownHover, states);
            else
                target.draw(m_textureArrowDownNormal, states);
        }
        else // There are no textures
        {
            sf::ConvexShape arrow{3};

            // Draw the track
            {
                sf::RectangleShape track{{m_scrollbar->m_track.width, m_scrollbar->m_track.height}};
                track.setPosition({m_scrollbar->m_track.left, m_scrollbar->m_track.top});

                if (m_scrollbar->m_mouseHover && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::Track))
                    track.setFillColor(calcColorOpacity(m_trackColorHover, m_scrollbar->getOpacity()));
                else
                    track.setFillColor(calcColorOpacity(m_trackColorNormal, m_scrollbar->getOpacity()));

                target.draw(track, states);
            }

            // Draw the thumb
            {
                sf::RectangleShape thumb{{m_scrollbar->m_thumb.width, m_scrollbar->m_thumb.height}};
                thumb.setPosition({m_scrollbar->m_thumb.left, m_scrollbar->m_thumb.top});

                if (m_scrollbar->m_mouseHover && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::Thumb))
                    thumb.setFillColor(calcColorOpacity(m_thumbColorHover, m_scrollbar->getOpacity()));
                else
                    thumb.setFillColor(calcColorOpacity(m_thumbColorNormal, m_scrollbar->getOpacity()));

                target.draw(thumb, states);
            }

            // Draw the up arrow
            {
                sf::RectangleShape arrowBack{{m_scrollbar->m_arrowUp.width, m_scrollbar->m_arrowUp.height}};
                arrowBack.setPosition({m_scrollbar->m_arrowUp.left, m_scrollbar->m_arrowUp.top});

                if (m_scrollbar->m_mouseHover && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::ArrowUp))
                {
                    arrowBack.setFillColor(calcColorOpacity(m_arrowBackgroundColorHover, m_scrollbar->getOpacity()));
                    arrow.setFillColor(calcColorOpacity(m_arrowColorHover, m_scrollbar->getOpacity()));
                }
                else
                {
                    arrowBack.setFillColor(calcColorOpacity(m_arrowBackgroundColorNormal, m_scrollbar->getOpacity()));
                    arrow.setFillColor(calcColorOpacity(m_arrowColorNormal, m_scrollbar->getOpacity()));
                }

                if (m_scrollbar->m_verticalScroll)
                {
                    arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                    arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x / 2), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                    arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                }
                else
                {
                    arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                    arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y / 2)});
                    arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                }

                target.draw(arrowBack, states);
                target.draw(arrow, states);
            }

            // Draw the down arrow
            {
                sf::RectangleShape arrowBack{{m_scrollbar->m_arrowDown.width, m_scrollbar->m_arrowDown.height}};
                arrowBack.setPosition({m_scrollbar->m_arrowDown.left, m_scrollbar->m_arrowDown.top});

                if (m_scrollbar->m_mouseHover && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::ArrowDown))
                {
                    arrowBack.setFillColor(calcColorOpacity(m_arrowBackgroundColorHover, m_scrollbar->getOpacity()));
                    arrow.setFillColor(calcColorOpacity(m_arrowColorHover, m_scrollbar->getOpacity()));
                }
                else
                {
                    arrowBack.setFillColor(calcColorOpacity(m_arrowBackgroundColorNormal, m_scrollbar->getOpacity()));
                    arrow.setFillColor(calcColorOpacity(m_arrowColorNormal, m_scrollbar->getOpacity()));
                }

                if (m_scrollbar->m_verticalScroll)
                {
                    arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                    arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x / 2), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                    arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                }
                else // Spin button lies horizontal
                {
                    arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                    arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y / 2)});
                    arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                }

                target.draw(arrowBack, states);
                target.draw(arrow, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ScrollbarRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ScrollbarRenderer>(*this);
        renderer->m_scrollbar = static_cast<Scrollbar*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
