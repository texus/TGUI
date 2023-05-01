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


#include <TGUI/Widgets/Scrollbar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Scrollbar::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ScrollbarRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setSize(getDefaultWidth(), 160);
        m_sizeSet = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr Scrollbar::create()
    {
        return std::make_shared<Scrollbar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr Scrollbar::copy(const Scrollbar::ConstPtr& scrollbar)
    {
        if (scrollbar)
            return std::static_pointer_cast<Scrollbar>(scrollbar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollbarRenderer* Scrollbar::getSharedRenderer()
    {
        return aurora::downcast<ScrollbarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollbarRenderer* Scrollbar::getSharedRenderer() const
    {
        return aurora::downcast<const ScrollbarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollbarRenderer* Scrollbar::getRenderer()
    {
        return aurora::downcast<ScrollbarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_sizeSet = true;
        updateSize();
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
        if (m_maximum < m_viewportSize)
            setValue(0);
        else if (m_value > m_maximum - m_viewportSize)
            setValue(m_maximum - m_viewportSize);

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
        if (m_maximum < m_viewportSize)
            value = 0;
        else if (value > m_maximum - m_viewportSize)
            value = m_maximum - m_viewportSize;

        if (m_value != value)
        {
            m_value = value;

            onValueChange.emit(this, m_value);

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

    void Scrollbar::setViewportSize(unsigned int viewportSize)
    {
        // Set the new value
        m_viewportSize = viewportSize;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_viewportSize)
            setValue(0);
        else if (m_value > m_maximum - m_viewportSize)
            setValue(m_maximum - m_viewportSize);

        // Recalculate the size and position of the thumb image
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getViewportSize() const
    {
        return m_viewportSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setScrollAmount(unsigned int scrollAmount)
    {
        m_scrollAmount = std::max(1u, scrollAmount);
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

    void Scrollbar::setVerticalScroll(bool vertical)
    {
        if (m_verticalScroll == vertical)
            return;

        m_verticalScroll = vertical;
        setSize(getSize().y, getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Scrollbar::getDefaultWidth()
    {
        if (m_spriteTrack.isSet())
            return static_cast<float>(m_spriteTrack.getTexture().getImageSize().x);
        else if (m_spriteThumb.isSet())
            return static_cast<float>(m_spriteThumb.getTexture().getImageSize().x);
        else
            return 16;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::isMouseOnWidget(Vector2f pos) const
    {
        // Don't make any calculations when no scrollbar is needed
        if (m_autoHide && (m_maximum <= m_viewportSize))
            return false;

        pos -= getPosition();
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (m_transparentTextureCached && m_spriteArrowDown.isTransparentPixel(pos - m_arrowDown.getPosition())
             && m_spriteTrack.isTransparentPixel(pos - m_track.getPosition()) && m_spriteArrowUp.isTransparentPixel(pos))
                return false;

            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;
        m_mouseDownOnArrow = false;

        if (m_verticalScroll)
        {
            // Check if the arrows are drawn at full size
            if (getSize().y > m_arrowUp.height + m_arrowDown.height)
            {
                // Check if you clicked on one of the arrows
                if ((pos.y < m_arrowUp.height) || (pos.y >= getSize().y - m_arrowUp.height))
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
                if ((pos.x < m_arrowUp.height) || (pos.x >= getSize().x - m_arrowUp.height))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }

        // Check if the mouse is on top of the thumb
        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
        {
            m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
            m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;

            m_mouseDownOnThumb = true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the scrollbar value
        if (!m_mouseDownOnArrow)
            mouseMoved(pos + getPosition());

        return !m_mouseDownOnArrow;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseReleased(Vector2f pos)
    {
        // Check if one of the arrows was clicked
        if (m_mouseDown && m_mouseDownOnArrow)
        {
            // Only continue when the calculations can be made
            if (m_maximum > m_viewportSize)
            {
                bool valueDown = false;
                bool valueUp = false;

                pos -= getPosition();

                // Check in which direction the scrollbar lies
                if (m_verticalScroll)
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().y > m_arrowUp.height + m_arrowDown.height)
                    {
                        // Check if you clicked on the top arrow
                        if (pos.y < m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (pos.y >= getSize().y - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (pos.y < getSize().y * 0.5f)
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
                        if (pos.x < m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (pos.x >= getSize().x - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (pos.x < getSize().x * 0.5f)
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
                    if (m_value + m_scrollAmount < m_maximum - m_viewportSize + 1)
                    {
                        if (m_value % m_scrollAmount)
                            setValue(m_value + (m_scrollAmount - (m_value % m_scrollAmount)));
                        else
                            setValue(m_value + m_scrollAmount);
                    }
                    else
                        setValue(m_maximum - m_viewportSize);
                }
            }
        }

        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseMoved(Vector2f pos)
    {
        // When dragging the scrollbar we can pass here without the mouse being on top of the scrollbar
        if (isMouseOnWidget(pos))
        {
            if (!m_mouseHover)
                mouseEnteredWidget();
        }
        else
        {
            if (m_mouseHover)
                mouseLeftWidget();
        }

        pos -= getPosition();

        // Check if the mouse button went down on top of the track (or thumb)
        if (m_mouseDown && !m_mouseDownOnArrow)
        {
            // Don't continue if the calculations can't be made
            if (!m_autoHide && (m_maximum <= m_viewportSize))
                return;

            // Check in which direction the scrollbar lies
            if (m_verticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((pos.y - m_mouseDownOnThumbPos.y - m_arrowUp.height) > 0)
                    {
                        // Calculate the new value
                        const unsigned int value = static_cast<unsigned int>((((pos.y - m_mouseDownOnThumbPos.y - m_arrowUp.height)
                            / (getSize().y - m_arrowUp.height - m_arrowDown.height - m_thumb.height)) * (m_maximum - m_viewportSize)) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_viewportSize))
                            setValue(value);
                        else
                            setValue(m_maximum - m_viewportSize);
                    }
                    else // The mouse was above the scrollbar
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop - m_arrowUp.height > 0) && (thumbTop + m_thumb.height + m_arrowDown.height < getSize().y))
                        m_thumb.top = thumbTop;
                    else // Prevent the thumb from going outside the scrollbar
                    {
                        if (m_maximum != m_viewportSize)
                            m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_viewportSize));
                        else
                            m_thumb.top = m_track.top;
                    }
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (pos.y >= m_arrowUp.height)
                    {
                        // Make sure that you did not click on the down arrow
                        if (pos.y < getSize().y - m_arrowUp.height)
                        {
                            // Calculate the exact position (a number between 0 and maximum), as if the top of the thumb will be where you clicked
                            const float scaleFactor = (m_maximum - m_viewportSize) / (getSize().y - m_arrowUp.height - m_arrowDown.height - m_thumb.height);
                            const float value = (pos.y - m_arrowUp.height) * scaleFactor;

                            // Check if you clicked above the thumb
                            if (value <= m_value)
                            {
                                // Try to place the thumb on 2/3 of the clicked position
                                const float subtractValue = (m_thumb.height / 3.0f) * scaleFactor;
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occurred below the thumb
                            {
                                // Try to place the thumb on 2/3 of the clicked position
                                const float subtractValue = (m_thumb.height * 2.0f / 3.0f) * scaleFactor;
                                if (value <= (m_maximum - m_viewportSize + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_viewportSize);
                            }
                        }
                    }

                    m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
                    m_mouseDownOnThumb = true;
                }
            }
            else // the scrollbar lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if (pos.x - m_mouseDownOnThumbPos.x - m_arrowUp.width > 0)
                    {
                        // Calculate the new value
                        const unsigned int value = static_cast<unsigned int>((((pos.x - m_mouseDownOnThumbPos.x - m_arrowUp.width)
                            / (getSize().x - m_arrowUp.width - m_arrowDown.width - m_thumb.width)) * (m_maximum - m_viewportSize)) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_viewportSize))
                            setValue(value);
                        else
                            setValue(m_maximum - m_viewportSize);
                    }
                    else // The mouse was to the left of the thumb
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft - m_arrowUp.width > 0) && (thumbLeft + m_thumb.width + m_arrowDown.width < getSize().x))
                        m_thumb.left = thumbLeft;
                    else // Prevent the thumb from going outside the scrollbar
                    {
                        if (m_maximum != m_viewportSize)
                            m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_viewportSize));
                        else
                            m_thumb.left = m_track.left;
                    }
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (pos.x >= m_arrowUp.width)
                    {
                        // Make sure that you did not click on the down arrow
                        if (pos.x < getSize().x - m_arrowUp.width)
                        {
                            // Calculate the exact position (a number between 0 and maximum), as if the left of the thumb will be where you clicked
                            const float scaleFactor = (m_maximum - m_viewportSize) / (getSize().x - m_arrowUp.width - m_arrowDown.width - m_thumb.width);
                            const float value = (pos.x - m_arrowUp.width) * scaleFactor;

                            // Check if you clicked to the left of the thumb
                            if (value <= m_value)
                            {
                                const float subtractValue = (m_thumb.width / 3.0f) * scaleFactor;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occurred to the right of the thumb
                            {
                                const float subtractValue = (m_thumb.width * 2.0f / 3.0f) * scaleFactor;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_maximum - m_viewportSize + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_viewportSize);
                            }
                        }
                    }

                    m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
                    m_mouseDownOnThumb = true;
                }
            }
        }

        if (FloatRect{m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height}.contains(pos))
            m_mouseHoverOverPart = Part::Thumb;
        else if (FloatRect{m_track.left, m_track.top, m_track.width, m_track.height}.contains(pos))
            m_mouseHoverOverPart = Part::Track;
        else if (FloatRect{m_arrowUp.left, m_arrowUp.top, m_arrowUp.width, m_arrowUp.height}.contains(pos))
            m_mouseHoverOverPart = Part::ArrowUp;
        else if (FloatRect{m_arrowDown.left, m_arrowDown.top, m_arrowDown.width, m_arrowDown.height}.contains(pos))
            m_mouseHoverOverPart = Part::ArrowDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::scrolled(float delta, Vector2f pos, bool touch)
    {
        const auto oldValue = m_value;

        const float amountToScroll = touch ? delta : (delta * m_scrollAmount);
        if (static_cast<int>(m_value) - static_cast<int>(amountToScroll) < 0)
            setValue(0);
        else
            setValue(static_cast<unsigned int>(m_value - (amountToScroll)));

        // Update over which part the mouse is hovering
        if (isMouseOnWidget(pos - getPosition()))
            mouseMoved(pos - getPosition());

        // If we aren't stuck at the lowest or highest value then we return true to mark the mouse wheel event as processed
        const bool scrollingIsPossible = ((oldValue != 0) || (delta <= 0)) && ((oldValue != m_maximum - m_viewportSize) || (delta >= 0));
        if (scrollingIsPossible)
        {
            m_lastSuccessfulScrollTime = std::chrono::steady_clock::now();
            m_lastSuccessfulScrollPos = pos;
            return true;
        }

        // If the mouse moved then make sure scrolling is no longer locked to this widget
        if (pos != m_lastSuccessfulScrollPos)
            m_lastSuccessfulScrollTime = std::chrono::steady_clock::time_point();

        // We can't scroll any further. In this case we want to let the event be handled by our parent in case of nested
        // scrollbars, but we don't want to do this when the user accidentally scrolled a bit too far.
        // So we will absorb the event (by returning true) if a previous scroll event was recent, or let our parent
        // handle the event (by returning false) if we were already stuck at the end of the scrollbar for a while.
        return ((m_lastSuccessfulScrollTime != std::chrono::steady_clock::time_point())
             && (Duration{std::chrono::steady_clock::now() - m_lastSuccessfulScrollTime} <= Duration{std::chrono::seconds(1)}));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseButtonNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();

        Widget::leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::updateSize()
    {
        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else if (getSize().x > getSize().y)
            m_verticalScroll = false;

        if (m_verticalScroll)
        {
            m_arrowUp.width = getSize().x;
            m_arrowDown.width = getSize().x;

            if (m_spriteArrowUp.isSet())
                m_arrowUp.height = getSize().x * m_spriteArrowUp.getTexture().getImageSize().x / m_spriteArrowUp.getTexture().getImageSize().y;
            else
                m_arrowUp.height = m_arrowUp.width;

            if (m_spriteArrowDown.isSet())
                m_arrowDown.height = getSize().x * m_spriteArrowDown.getTexture().getImageSize().x / m_spriteArrowDown.getTexture().getImageSize().y;
            else
                m_arrowDown.height = m_arrowUp.width;

            m_track.width = getSize().x;
            m_track.height = std::max(0.f, getSize().y - m_arrowUp.height - m_arrowDown.height);

            m_thumb.width = getSize().x;
            if (m_maximum > m_viewportSize)
            {
                m_thumb.height = m_track.height * m_viewportSize / m_maximum;

                // Don't allow the thumb to become smaller than the scrollbar width, unless there isn't enough room for it
                if (m_thumb.height < getSize().x)
                {
                    if (getSize().x < m_track.height)
                        m_thumb.height = getSize().x;
                    else // Track is too small to contain minimum the thumb size, so instead we use 3/4th of track as size
                        m_thumb.height = m_track.height * 0.75f;
                }
            }
            else
                m_thumb.height = m_track.height;
        }
        else // The scrollbar lies horizontally
        {
            m_arrowUp.height = getSize().y;
            m_arrowDown.height = getSize().y;

            if (m_spriteArrowUp.isSet())
                m_arrowUp.width = getSize().y * m_spriteArrowUp.getTexture().getImageSize().x / m_spriteArrowUp.getTexture().getImageSize().y;
            else
                m_arrowUp.width = m_arrowUp.height;

            if (m_spriteArrowDown.isSet())
                m_arrowDown.width = getSize().y * m_spriteArrowDown.getTexture().getImageSize().x / m_spriteArrowDown.getTexture().getImageSize().y;
            else
                m_arrowDown.width = m_arrowUp.height;

            m_track.width = std::max(0.f, getSize().x - m_arrowUp.height - m_arrowDown.height);
            m_track.height = getSize().y;

            m_thumb.height = getSize().y;
            if (m_maximum > m_viewportSize)
            {
                m_thumb.width = m_track.width * m_viewportSize / m_maximum;

                // Don't allow the thumb to become smaller than the scrollbar width, unless there isn't enough room for it
                if (m_thumb.width < getSize().y)
                {
                    if (getSize().y < m_track.width)
                        m_thumb.width = getSize().y;
                    else // Track is too small to contain minimum the thumb size, so instead we use 3/4th of track as size
                        m_thumb.width = m_track.width * 0.75f;
                }
            }
            else
                m_thumb.width = m_track.width;
        }

        // Set the texture sizes and rotation
        const float trackRotation = (m_verticalScroll == m_verticalImage) ? 0.f : -90.f;
        if (m_spriteTrack.isSet())
        {
            if (m_verticalScroll == m_verticalImage)
            {
                m_spriteTrack.setSize({m_track.width, m_track.height});
                m_spriteTrackHover.setSize({m_track.width, m_track.height});
            }
            else
            {
                m_spriteTrack.setSize({m_track.height, m_track.width});
                m_spriteTrackHover.setSize({m_track.height, m_track.width});
            }

            m_spriteTrack.setRotation(trackRotation);
            m_spriteTrackHover.setRotation(trackRotation);
        }
        if (m_spriteThumb.isSet())
        {
            if (m_verticalScroll == m_verticalImage)
            {
                m_spriteThumb.setSize({m_thumb.width, m_thumb.height});
                m_spriteThumbHover.setSize({m_thumb.width, m_thumb.height});
            }
            else
            {
                m_spriteThumb.setSize({m_thumb.height, m_thumb.width});
                m_spriteThumbHover.setSize({m_thumb.height, m_thumb.width});
            }

            m_spriteThumb.setRotation(trackRotation);
            m_spriteThumbHover.setRotation(trackRotation);
        }

        const float arrowRotation = m_verticalScroll ? 0.f : -90.f;
        if (m_spriteArrowUp.isSet())
        {
            m_spriteArrowUp.setSize({m_arrowUp.width, m_arrowUp.height});
            m_spriteArrowUpHover.setSize({m_arrowUp.width, m_arrowUp.height});
            m_spriteArrowUp.setRotation(arrowRotation);
            m_spriteArrowUpHover.setRotation(arrowRotation);
        }
        if (m_spriteArrowDown.isSet())
        {
            m_spriteArrowDown.setSize({m_arrowUp.width, m_arrowUp.height});
            m_spriteArrowDownHover.setSize({m_arrowUp.width, m_arrowUp.height});
            m_spriteArrowDown.setRotation(arrowRotation);
            m_spriteArrowDownHover.setRotation(arrowRotation);
        }

        // Recalculate the position of the track, thumb and arrows
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

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Scrollbar::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::rendererChanged(const String& property)
    {
        if (property == U"TextureTrack")
        {
            m_spriteTrack.setTexture(getSharedRenderer()->getTextureTrack());
            if (m_sizeSet)
                updateSize();
            else
            {
                if (m_verticalScroll)
                    setSize({getDefaultWidth(), getSize().y});
                else
                    setSize({getSize().x, getDefaultWidth()});

                m_sizeSet = false;
            }
        }
        else if (property == U"TextureTrackHover")
        {
            m_spriteTrackHover.setTexture(getSharedRenderer()->getTextureTrackHover());
        }
        else if (property == U"TextureThumb")
        {
            m_spriteThumb.setTexture(getSharedRenderer()->getTextureThumb());
            updateSize();
        }
        else if (property == U"TextureThumbHover")
        {
            m_spriteThumbHover.setTexture(getSharedRenderer()->getTextureThumbHover());
        }
        else if (property == U"TextureArrowUp")
        {
            m_spriteArrowUp.setTexture(getSharedRenderer()->getTextureArrowUp());
            updateSize();
        }
        else if (property == U"TextureArrowUpHover")
        {
            m_spriteArrowUpHover.setTexture(getSharedRenderer()->getTextureArrowUpHover());
        }
        else if (property == U"TextureArrowDown")
        {
            m_spriteArrowDown.setTexture(getSharedRenderer()->getTextureArrowDown());
            updateSize();
        }
        else if (property == U"TextureArrowDownHover")
        {
            m_spriteArrowDownHover.setTexture(getSharedRenderer()->getTextureArrowDownHover());
        }
        else if (property == U"TrackColor")
        {
            m_trackColorCached = getSharedRenderer()->getTrackColor();
        }
        else if (property == U"TrackColorHover")
        {
            m_trackColorHoverCached = getSharedRenderer()->getTrackColorHover();
        }
        else if (property == U"ThumbColor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == U"ThumbColorHover")
        {
            m_thumbColorHoverCached = getSharedRenderer()->getThumbColorHover();
        }
        else if (property == U"ArrowBackgroundColor")
        {
            m_arrowBackgroundColorCached = getSharedRenderer()->getArrowBackgroundColor();
        }
        else if (property == U"ArrowBackgroundColorHover")
        {
            m_arrowBackgroundColorHoverCached = getSharedRenderer()->getArrowBackgroundColorHover();
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

            m_spriteTrack.setOpacity(m_opacityCached);
            m_spriteTrackHover.setOpacity(m_opacityCached);
            m_spriteThumb.setOpacity(m_opacityCached);
            m_spriteThumbHover.setOpacity(m_opacityCached);
            m_spriteArrowUp.setOpacity(m_opacityCached);
            m_spriteArrowUpHover.setOpacity(m_opacityCached);
            m_spriteArrowDown.setOpacity(m_opacityCached);
            m_spriteArrowDownHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Scrollbar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs[U"AutoHide"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_autoHide));
        node->propertyValuePairs[U"ViewportSize"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_viewportSize));
        node->propertyValuePairs[U"Maximum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximum));
        node->propertyValuePairs[U"Value"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_value));
        node->propertyValuePairs[U"ScrollAmount"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_scrollAmount));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"ViewportSize"])
            setViewportSize(node->propertyValuePairs[U"ViewportSize"]->value.toUInt());
        if (node->propertyValuePairs[U"Maximum"])
            setMaximum(node->propertyValuePairs[U"Maximum"]->value.toUInt());
        if (node->propertyValuePairs[U"Value"])
            setValue(node->propertyValuePairs[U"Value"]->value.toUInt());
        if (node->propertyValuePairs[U"ScrollAmount"])
            setScrollAmount(node->propertyValuePairs[U"ScrollAmount"]->value.toUInt());
        if (node->propertyValuePairs[U"AutoHide"])
            setAutoHide(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"AutoHide"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = 0;
            if (m_maximum != m_viewportSize)
                m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_viewportSize));
            else
                m_thumb.top = m_track.top;
        }
        else
        {
            m_thumb.top = 0;
            if (m_maximum != m_viewportSize)
                m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_viewportSize));
            else
                m_thumb.left = m_track.left;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Don't draw the scrollbar when it is not needed
        if (m_autoHide && (m_maximum <= m_viewportSize))
            return;

        // Draw arrow up/left
        if (m_spriteArrowUp.isSet())
        {
            if (m_mouseHover && m_spriteArrowUpHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp))
                target.drawSprite(states, m_spriteArrowUpHover);
            else
                target.drawSprite(states, m_spriteArrowUp);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp) && m_arrowBackgroundColorHoverCached.isSet())
                target.drawFilledRect(states, {m_arrowUp.width, m_arrowUp.height}, Color::applyOpacity(m_arrowBackgroundColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, {m_arrowUp.width, m_arrowUp.height}, Color::applyOpacity(m_arrowBackgroundColorCached, m_opacityCached));

            Vertex::Color arrowVertexColor;
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp) && m_arrowBackgroundColorHoverCached.isSet())
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorCached, m_opacityCached));

            if (m_verticalScroll)
            {
                target.drawTriangle(states,
                    {{m_arrowUp.width / 5, m_arrowUp.height * 4/5}, arrowVertexColor},
                    {{m_arrowUp.width / 2, m_arrowUp.height / 5}, arrowVertexColor},
                    {{m_arrowUp.width * 4/5, m_arrowUp.height * 4/5}, arrowVertexColor}
                );
            }
            else // Spin button lies horizontal
            {
                target.drawTriangle(states,
                    {{m_arrowUp.width * 4/5, m_arrowUp.height / 5}, arrowVertexColor},
                    {{m_arrowUp.width / 5, m_arrowUp.height / 2}, arrowVertexColor},
                    {{m_arrowUp.width * 4/5, m_arrowUp.height * 4/5}, arrowVertexColor}
                );
            }
        }

        // Draw the track
        states.transform.translate(m_track.getPosition());
        if (m_spriteTrack.isSet())
        {
            if (m_mouseHover && m_spriteTrackHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::Track))
                target.drawSprite(states, m_spriteTrackHover);
            else
                target.drawSprite(states, m_spriteTrack);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::Track) && m_trackColorHoverCached.isSet())
                target.drawFilledRect(states, {m_track.width, m_track.height}, Color::applyOpacity(m_trackColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, {m_track.width, m_track.height}, Color::applyOpacity(m_trackColorCached, m_opacityCached));
        }
        states.transform.translate(-m_track.getPosition());

        // Draw the thumb
        states.transform.translate(m_thumb.getPosition());
        if (m_spriteThumb.isSet())
        {
            if (m_mouseHover && m_spriteThumbHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::Thumb))
                target.drawSprite(states, m_spriteThumbHover);
            else
                target.drawSprite(states, m_spriteThumb);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::Thumb) && m_thumbColorHoverCached.isSet())
                target.drawFilledRect(states, {m_thumb.width, m_thumb.height}, Color::applyOpacity(m_thumbColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, {m_thumb.width, m_thumb.height}, Color::applyOpacity(m_thumbColorCached, m_opacityCached));
        }
        states.transform.translate(-m_thumb.getPosition());

        // Draw arrow down/right
        states.transform.translate(m_arrowDown.getPosition());
        if (m_spriteArrowDown.isSet())
        {
            if (m_mouseHover && m_spriteArrowDownHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown))
                target.drawSprite(states, m_spriteArrowDownHover);
            else
                target.drawSprite(states, m_spriteArrowDown);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown) && m_arrowBackgroundColorHoverCached.isSet())
                target.drawFilledRect(states, {m_arrowDown.width, m_arrowDown.height}, Color::applyOpacity(m_arrowBackgroundColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, {m_arrowDown.width, m_arrowDown.height}, Color::applyOpacity(m_arrowBackgroundColorCached, m_opacityCached));

            Vertex::Color arrowVertexColor;
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown) && m_arrowBackgroundColorHoverCached.isSet())
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrowVertexColor = Vertex::Color(Color::applyOpacity(m_arrowColorCached, m_opacityCached));

            if (m_verticalScroll)
            {
                target.drawTriangle(states,
                    {{m_arrowDown.width / 5, m_arrowDown.height / 5}, arrowVertexColor},
                    {{m_arrowDown.width / 2, m_arrowDown.height * 4/5}, arrowVertexColor},
                    {{m_arrowDown.width * 4/5, m_arrowDown.height / 5}, arrowVertexColor}
                );
            }
            else // Spin button lies horizontal
            {
                target.drawTriangle(states,
                    {{m_arrowDown.width / 5, m_arrowDown.height / 5}, arrowVertexColor},
                    {{m_arrowDown.width * 4/5, m_arrowDown.height / 2}, arrowVertexColor},
                    {{m_arrowDown.width / 5, m_arrowDown.height * 4/5}, arrowVertexColor}
                );
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Scrollbar::clone() const
    {
        return std::make_shared<Scrollbar>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isMouseDownOnThumb() const
    {
        return m_mouseDownOnThumb;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isShown() const
    {
        return m_visible && (!m_autoHide || (m_maximum > m_viewportSize));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollbarChildWidget::draw(BackendRenderTarget& target, RenderStates states) const
    {
        states.transform.translate(getPosition());
        Scrollbar::draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
