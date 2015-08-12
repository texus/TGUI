/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

    Scrollbar::Ptr Scrollbar::copy(Scrollbar::ConstPtr scrollbar)
    {
        if (scrollbar)
            return std::make_shared<Scrollbar>(*scrollbar);
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

            // Make sure the transparency is not lost
            setTransparency(m_opacity);
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

    void Scrollbar::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        getRenderer()->m_textureTrackNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureTrackHover.setColor(sf::Color(255, 255, 255, m_opacity));

        getRenderer()->m_textureThumbNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureThumbHover.setColor(sf::Color(255, 255, 255, m_opacity));

        getRenderer()->m_textureArrowUpNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureArrowUpHover.setColor(sf::Color(255, 255, 255, m_opacity));

        getRenderer()->m_textureArrowDownNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureArrowDownHover.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseOnWidget(float x, float y)
    {
        // Don't make any calculations when no scrollbar is needed
        if ((m_maximum <= m_lowValue) && (m_autoHide == true))
            return false;

        // Check if the mouse is on top of the scrollbar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;

        if (m_mouseHover)
            mouseLeftWidget();

        return false;
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
        if (m_mouseDownOnArrow == false)
            mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseReleased(float x, float y)
    {
        // Check if one of the arrows was clicked
        if ((m_mouseDown) && (m_mouseDownOnArrow))
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

        // The mouse is no longer down
        m_mouseDown = false;
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

    void Scrollbar::reload(const std::string& primary, const std::string& secondary, bool force)
    {
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
        else // Load white theme
        {
            getRenderer()->setTrackColorNormal({245, 245, 245});
            getRenderer()->setTrackColorHover({255, 255, 255});
            getRenderer()->setThumbColorNormal({245, 245, 245});
            getRenderer()->setThumbColorHover({255, 255, 255});
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

        if (property == toLower("TrackColor"))
            setTrackColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("TrackColorNormal"))
            setTrackColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("TrackColorHover"))
            setTrackColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ThumbColor"))
            setThumbColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ThumbColorNormal"))
            setThumbColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ThumbColorHover"))
            setThumbColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ArrowBackgroundColor"))
            setArrowBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ArrowBackgroundColorNormal"))
            setArrowBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ArrowBackgroundColorHover"))
            setArrowBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ArrowColor"))
            setArrowColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ArrowColorNormal"))
            setArrowColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ArrowColorHover"))
            setArrowColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("TrackImage"))
            setTrackTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("TrackHoverImage"))
            setTrackHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("ThumbImage"))
            setThumbTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("ThumbHoverImage"))
            setThumbHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("ArrowUpImage"))
            setArrowUpTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("ArrowDownImage"))
            setArrowDownTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("ArrowUpHoverImage"))
            setArrowUpHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("ArrowDownHoverImage"))
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
            if (property == toLower("TrackColor"))
                setTrackColor(value.getColor());
            else if (property == toLower("TrackColorNormal"))
                setTrackColorNormal(value.getColor());
            else if (property == toLower("TrackColorHover"))
                setTrackColorHover(value.getColor());
            else if (property == toLower("ThumbColor"))
                setThumbColor(value.getColor());
            else if (property == toLower("ThumbColorNormal"))
                setThumbColorNormal(value.getColor());
            else if (property == toLower("ThumbColorHover"))
                setThumbColorHover(value.getColor());
            else if (property == toLower("ArrowBackgroundColor"))
                setArrowBackgroundColor(value.getColor());
            else if (property == toLower("ArrowBackgroundColorNormal"))
                setArrowBackgroundColorNormal(value.getColor());
            else if (property == toLower("ArrowBackgroundColorHover"))
                setArrowBackgroundColorHover(value.getColor());
            else if (property == toLower("ArrowColor"))
                setArrowColor(value.getColor());
            else if (property == toLower("ArrowColorNormal"))
                setArrowColorNormal(value.getColor());
            else if (property == toLower("ArrowColorHover"))
                setArrowColorHover(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == toLower("TrackImage"))
                setTrackTexture(value.getTexture());
            else if (property == toLower("TrackHoverImage"))
                setTrackHoverTexture(value.getTexture());
            else if (property == toLower("ThumbImage"))
                setThumbTexture(value.getTexture());
            else if (property == toLower("ThumbHoverImage"))
                setThumbHoverTexture(value.getTexture());
            else if (property == toLower("ArrowUpImage"))
                setArrowUpTexture(value.getTexture());
            else if (property == toLower("ArrowDownImage"))
                setArrowDownTexture(value.getTexture());
            else if (property == toLower("ArrowUpHoverImage"))
                setArrowUpHoverTexture(value.getTexture());
            else if (property == toLower("ArrowDownHoverImage"))
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

        if (property == toLower("TrackColor"))
            return m_trackColorNormal;
        else if (property == toLower("TrackColorNormal"))
            return m_trackColorNormal;
        else if (property == toLower("TrackColorHover"))
            return m_trackColorHover;
        else if (property == toLower("ThumbColor"))
            return m_thumbColorNormal;
        else if (property == toLower("ThumbColorNormal"))
            return m_thumbColorNormal;
        else if (property == toLower("ThumbColorHover"))
            return m_thumbColorHover;
        else if (property == toLower("ArrowBackgroundColor"))
            return m_arrowBackgroundColorNormal;
        else if (property == toLower("ArrowBackgroundColorNormal"))
            return m_arrowBackgroundColorNormal;
        else if (property == toLower("ArrowBackgroundColorHover"))
            return m_arrowBackgroundColorHover;
        else if (property == toLower("ArrowColor"))
            return m_arrowColorNormal;
        else if (property == toLower("ArrowColorNormal"))
            return m_arrowColorNormal;
        else if (property == toLower("ArrowColorHover"))
            return m_arrowColorHover;
        else if (property == toLower("TrackImage"))
            return m_textureTrackNormal;
        else if (property == toLower("TrackHoverImage"))
            return m_textureTrackHover;
        else if (property == toLower("ThumbImage"))
            return m_textureThumbNormal;
        else if (property == toLower("ThumbHoverImage"))
            return m_textureThumbHover;
        else if (property == toLower("ArrowUpImage"))
            return m_textureArrowUpNormal;
        else if (property == toLower("ArrowDownImage"))
            return m_textureArrowDownNormal;
        else if (property == toLower("ArrowUpHoverImage"))
            return m_textureArrowUpHover;
        else if (property == toLower("ArrowDownHoverImage"))
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

    void ScrollbarRenderer::setTrackColor(const sf::Color& color)
    {
        m_trackColorNormal = color;
        m_trackColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackColorNormal(const sf::Color& color)
    {
        m_trackColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackColorHover(const sf::Color& color)
    {
        m_trackColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbColor(const sf::Color& color)
    {
        m_thumbColorNormal = color;
        m_thumbColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbColorNormal(const sf::Color& color)
    {
        m_thumbColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbColorHover(const sf::Color& color)
    {
        m_thumbColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowBackgroundColor(const sf::Color& color)
    {
        m_arrowBackgroundColorNormal = color;
        m_arrowBackgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowBackgroundColorNormal(const sf::Color& color)
    {
        m_arrowBackgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowBackgroundColorHover(const sf::Color& color)
    {
        m_arrowBackgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowColor(const sf::Color& color)
    {
        m_arrowColorNormal = color;
        m_arrowColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowColorNormal(const sf::Color& color)
    {
        m_arrowColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowColorHover(const sf::Color& color)
    {
        m_arrowColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackTexture(const Texture& texture)
    {
        m_textureTrackNormal = texture;
        m_textureTrackNormal.setPosition(m_scrollbar->getPosition());
        m_textureTrackNormal.setSize(m_scrollbar->getSize());
        m_textureTrackNormal.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setTrackHoverTexture(const Texture& texture)
    {
        m_textureTrackHover = texture;
        m_textureTrackHover.setPosition(m_scrollbar->getPosition());
        m_textureTrackHover.setSize(m_scrollbar->getSize());
        m_textureTrackHover.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbTexture(const Texture& texture)
    {
        m_textureThumbNormal = texture;
        m_textureThumbNormal.setPosition(m_scrollbar->getPosition());
        m_textureThumbNormal.setSize(m_scrollbar->getSize());
        m_textureThumbNormal.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setThumbHoverTexture(const Texture& texture)
    {
        m_textureThumbHover = texture;
        m_textureThumbHover.setPosition(m_scrollbar->getPosition());
        m_textureThumbHover.setSize(m_scrollbar->getSize());
        m_textureThumbHover.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowUpTexture(const Texture& texture)
    {
        m_textureArrowUpNormal = texture;
        m_textureArrowUpNormal.setPosition(m_scrollbar->getPosition());
        m_textureArrowUpNormal.setSize(m_scrollbar->getSize());
        m_textureArrowUpNormal.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowDownTexture(const Texture& texture)
    {
        m_textureArrowDownNormal = texture;
        m_textureArrowDownNormal.setPosition(m_scrollbar->getPosition());
        m_textureArrowDownNormal.setSize(m_scrollbar->getSize());
        m_textureArrowDownNormal.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowUpHoverTexture(const Texture& texture)
    {
        m_textureArrowUpHover = texture;
        m_textureArrowUpHover.setPosition(m_scrollbar->getPosition());
        m_textureArrowUpHover.setSize(m_scrollbar->getSize());
        m_textureArrowUpHover.setColor({255, 255, 255, m_scrollbar->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarRenderer::setArrowDownHoverTexture(const Texture& texture)
    {
        m_textureArrowDownHover = texture;
        m_textureArrowDownHover.setPosition(m_scrollbar->getPosition());
        m_textureArrowDownHover.setSize(m_scrollbar->getSize());
        m_textureArrowDownHover.setColor({255, 255, 255, m_scrollbar->getTransparency()});
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
                    track.setFillColor(m_trackColorHover);
                else
                    track.setFillColor(m_trackColorNormal);

                target.draw(track, states);
            }

            // Draw the thumb
            {
                sf::RectangleShape thumb{{m_scrollbar->m_thumb.width, m_scrollbar->m_thumb.height}};
                thumb.setPosition({m_scrollbar->m_thumb.left, m_scrollbar->m_thumb.top});

                if (m_scrollbar->m_mouseHover && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::Thumb))
                    thumb.setFillColor(m_thumbColorHover);
                else
                    thumb.setFillColor(m_thumbColorNormal);

                target.draw(thumb, states);
            }

            // Draw the up arrow
            {
                sf::RectangleShape arrowBack{{m_scrollbar->m_arrowUp.width, m_scrollbar->m_arrowUp.height}};
                arrowBack.setPosition({m_scrollbar->m_arrowUp.left, m_scrollbar->m_arrowUp.top});

                if (m_scrollbar->m_mouseHover && (m_scrollbar->m_mouseHoverOverPart == Scrollbar::Part::ArrowUp))
                {
                    arrowBack.setFillColor(m_arrowBackgroundColorHover);
                    arrow.setFillColor(m_arrowColorHover);
                }
                else
                {
                    arrowBack.setFillColor(m_arrowBackgroundColorNormal);
                    arrow.setFillColor(m_arrowColorNormal);
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
                    arrowBack.setFillColor(m_arrowBackgroundColorHover);
                    arrow.setFillColor(m_arrowColorHover);
                }
                else
                {
                    arrowBack.setFillColor(m_arrowBackgroundColorNormal);
                    arrow.setFillColor(m_arrowColorNormal);
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
        auto renderer = std::shared_ptr<ScrollbarRenderer>(new ScrollbarRenderer{*this});
        renderer->m_scrollbar = static_cast<Scrollbar*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
