/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/RangeSlider.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSlider::RangeSlider()
    {
        m_type = "RangeSlider";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<RangeSliderRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSlider::Ptr RangeSlider::create(float minimum, float maximum)
    {
        auto slider = std::make_shared<RangeSlider>();

        slider->setMinimum(minimum);
        slider->setMaximum(maximum);

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSlider::Ptr RangeSlider::copy(RangeSlider::ConstPtr slider)
    {
        if (slider)
            return std::static_pointer_cast<RangeSlider>(slider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSliderRenderer* RangeSlider::getSharedRenderer()
    {
        return aurora::downcast<RangeSliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RangeSliderRenderer* RangeSlider::getSharedRenderer() const
    {
        return aurora::downcast<const RangeSliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSliderRenderer* RangeSlider::getRenderer()
    {
        return aurora::downcast<RangeSliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RangeSliderRenderer* RangeSlider::getRenderer() const
    {
        return aurora::downcast<const RangeSliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            float scaleFactor;
            if (m_verticalImage == m_verticalScroll)
            {
                m_spriteTrack.setSize(getInnerSize());
                m_spriteTrackHover.setSize(getInnerSize());

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().x;
                else
                    scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().y;
            }
            else // The image is rotated
            {
                m_spriteTrack.setSize({getInnerSize().y, getInnerSize().x});
                m_spriteTrackHover.setSize({getInnerSize().y, getInnerSize().x});

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().y;
                else
                    scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().x;
            }

            m_thumbs.first.width = scaleFactor * m_spriteThumb.getTexture().getImageSize().x;
            m_thumbs.first.height = scaleFactor * m_spriteThumb.getTexture().getImageSize().y;

            m_spriteThumb.setSize({m_thumbs.first.width, m_thumbs.first.height});
            m_spriteThumbHover.setSize({m_thumbs.first.width, m_thumbs.first.height});

            // Apply the rotation now that the size has been set
            if (m_verticalScroll != m_verticalImage)
            {
                m_spriteTrack.setRotation(-90);
                m_spriteTrackHover.setRotation(-90);
                m_spriteThumb.setRotation(-90);
                m_spriteThumbHover.setRotation(-90);
            }
            else
            {
                m_spriteTrack.setRotation(0);
                m_spriteTrackHover.setRotation(0);
                m_spriteThumb.setRotation(0);
                m_spriteThumbHover.setRotation(0);
            }
        }
        else // There are no textures
        {
            if (m_verticalScroll)
            {
                m_thumbs.first.width = getSize().x * 1.6f;
                m_thumbs.first.height = m_thumbs.first.width / 2.0f;
            }
            else
            {
                m_thumbs.first.height = getSize().y * 1.6f;
                m_thumbs.first.width = m_thumbs.first.height / 2.0f;
            }
        }

        m_thumbs.second.width = m_thumbs.first.width;
        m_thumbs.second.height = m_thumbs.first.height;

        updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RangeSlider::getFullSize() const
    {
        if (m_verticalScroll)
            return {std::max(getSize().x, m_thumbs.first.width), getSize().y + m_thumbs.first.height};
        else
            return {getSize().x + m_thumbs.first.width, std::max(getSize().y, m_thumbs.first.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RangeSlider::getWidgetOffset() const
    {
        if (m_verticalScroll)
            return {std::min(0.f, getSize().x - m_thumbs.first.width), -m_thumbs.first.height / 2.f};
        else
            return {-m_thumbs.first.width / 2.f, std::min(0.f, getSize().y - m_thumbs.first.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setMinimum(float minimum)
    {
        const auto oldMinimum = m_minimum;

        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the selection start equaled the minimum, move it as well, otherwise set it again to make sure it is still within minimum and maximum
        if (oldMinimum == m_selectionStart)
            setSelectionStart(m_minimum);
        else
            setSelectionStart(m_selectionStart);

        updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setMaximum(float maximum)
    {
        const auto oldMaximum = m_maximum;

        // Set the new maximum
        m_maximum = maximum;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the selection end equaled the maximum, move it as well, otherwise set it again to make sure it is still within minimum and maximum
        if (oldMaximum == m_selectionStart)
            setSelectionEnd(m_maximum);
        else
            setSelectionEnd(m_selectionEnd);

        updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setSelectionStart(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_selectionStart != value)
        {
            m_selectionStart = value;

            // Update the selection end when the selection start passed it
            if (m_selectionEnd < value)
                m_selectionEnd = value;

            onRangeChange.emit(this, m_selectionStart, m_selectionEnd);

            updateThumbPositions();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getSelectionStart() const
    {
        return m_selectionStart;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setSelectionEnd(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_selectionEnd != value)
        {
            m_selectionEnd = value;

            // Update the selection start when the selection end passed it
            if (m_selectionStart > value)
                m_selectionStart = value;

            onRangeChange.emit(this, m_selectionStart, m_selectionEnd);

            updateThumbPositions();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getSelectionEnd() const
    {
        return m_selectionEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setStep(float step)
    {
        m_step = step;

        // Reset the values in case it does not match the step
        setSelectionStart(m_selectionStart);
        setSelectionEnd(m_selectionEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RangeSlider::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the thumbs
        if (FloatRect(m_thumbs.second.left, m_thumbs.second.top, m_thumbs.second.width, m_thumbs.second.height).contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteThumb.isTransparentPixel(pos - m_thumbs.first.getPosition()))
                return true;
        }
        if (FloatRect(m_thumbs.first.left, m_thumbs.first.top, m_thumbs.first.width, m_thumbs.first.height).contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteThumb.isTransparentPixel(pos - m_thumbs.second.getPosition()))
                return true;
        }

        // Check if the mouse is on top of the track
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteTrack.isTransparentPixel(pos - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        // Refresh the value
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::leftMouseReleased(Vector2f)
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse button is down
        if (m_mouseDown && (m_mouseDownOnThumb != 0))
        {
            // Check in which direction the slider goes
            if (m_verticalScroll)
            {
                if (m_mouseDownOnThumb == 1)
                {
                    setSelectionStart(m_maximum - (((pos.y + (m_thumbs.first.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum)));

                    // Set the thumb position for smooth scrolling
                    const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop + (m_thumbs.first.height / 2.0f) > 0) && (thumbTop + (m_thumbs.first.height / 2.0f) < getSize().y))
                        m_thumbs.first.top = thumbTop;
                    else
                        m_thumbs.first.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_selectionStart)) - (m_thumbs.first.height / 2.0f);
                }
                else // if (m_mouseDownOnThumb == 2)
                {
                    setSelectionEnd(m_maximum - (((pos.y + (m_thumbs.second.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum)));

                    // Set the thumb position for smooth scrolling
                    const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop + (m_thumbs.second.height / 2.0f) > 0) && (thumbTop + (m_thumbs.second.height / 2.0f) < getSize().y))
                        m_thumbs.second.top = thumbTop;
                    else
                        m_thumbs.second.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_selectionEnd)) - (m_thumbs.second.height / 2.0f);
                }
            }
            else // the slider lies horizontal
            {
                if (m_mouseDownOnThumb == 1)
                {
                    setSelectionStart((((pos.x + (m_thumbs.first.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum);

                    // Set the thumb position for smooth scrolling
                    const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft + (m_thumbs.first.width / 2.0f) > 0) && (thumbLeft + (m_thumbs.first.width / 2.0f) < getSize().x))
                        m_thumbs.first.left = thumbLeft;
                    else
                        m_thumbs.first.left = (getSize().x / (m_maximum - m_minimum) * (m_selectionStart - m_minimum)) - (m_thumbs.first.width / 2.0f);
                }
                else // if (m_mouseDownOnThumb == 2)
                {
                    setSelectionEnd((((pos.x + (m_thumbs.second.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum);

                    // Set the thumb position for smooth scrolling
                    const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft + (m_thumbs.second.width / 2.0f) > 0) && (thumbLeft + (m_thumbs.second.width / 2.0f) < getSize().x))
                        m_thumbs.second.left = thumbLeft;
                    else
                        m_thumbs.second.left = (getSize().x / (m_maximum - m_minimum) * (m_selectionEnd - m_minimum)) - (m_thumbs.second.width / 2.0f);
                }
            }
        }
        else // Normal mouse move
        {
            // Set some variables so that when the mouse goes down we know whether it is on the track or not
            if (FloatRect(m_thumbs.second.left, m_thumbs.second.top, m_thumbs.second.width, m_thumbs.second.height).contains(pos))
            {
                m_mouseDownOnThumb = 2;
                m_mouseDownOnThumbPos.x = pos.x - m_thumbs.second.left;
                m_mouseDownOnThumbPos.y = pos.y - m_thumbs.second.top;
            }
            else if (FloatRect(m_thumbs.first.left, m_thumbs.first.top, m_thumbs.first.width, m_thumbs.first.height).contains(pos))
            {
                m_mouseDownOnThumb = 1;
                m_mouseDownOnThumbPos.x = pos.x - m_thumbs.first.left;
                m_mouseDownOnThumbPos.y = pos.y - m_thumbs.first.top;
            }
            else // The mouse is not on top of the thumb
                m_mouseDownOnThumb = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::mouseNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPositions();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& RangeSlider::getSignal(std::string signalName)
    {
        if (signalName == toLower(onRangeChange.getName()))
            return onRangeChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "texturetrack")
        {
            m_spriteTrack.setTexture(getSharedRenderer()->getTextureTrack());

            if (m_spriteTrack.getTexture().getImageSize().x < m_spriteTrack.getTexture().getImageSize().y)
                m_verticalImage = true;
            else
                m_verticalImage = false;

            setSize(m_size);
        }
        else if (property == "texturetrackhover")
        {
            m_spriteTrackHover.setTexture(getSharedRenderer()->getTextureTrackHover());
        }
        else if (property == "texturethumb")
        {
            m_spriteThumb.setTexture(getSharedRenderer()->getTextureThumb());
            setSize(m_size);
        }
        else if (property == "texturethumbhover")
        {
            m_spriteThumbHover.setTexture(getSharedRenderer()->getTextureThumbHover());
        }
        else if (property == "trackcolor")
        {
            m_trackColorCached = getSharedRenderer()->getTrackColor();
        }
        else if (property == "trackcolorhover")
        {
            m_trackColorHoverCached = getSharedRenderer()->getTrackColorHover();
        }
        else if (property == "selectedtrackcolor")
        {
            m_selectedTrackColorCached = getSharedRenderer()->getSelectedTrackColor();
        }
        else if (property == "selectedtrackcolorhover")
        {
            m_selectedTrackColorHoverCached = getSharedRenderer()->getSelectedTrackColorHover();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "thumbcolorhover")
        {
            m_thumbColorHoverCached = getSharedRenderer()->getThumbColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteTrack.setOpacity(m_opacityCached);
            m_spriteTrackHover.setOpacity(m_opacityCached);
            m_spriteThumb.setOpacity(m_opacityCached);
            m_spriteThumbHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> RangeSlider::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["SelectionStart"] = std::make_unique<DataIO::ValueNode>(to_string(m_selectionStart));
        node->propertyValuePairs["SelectionEnd"] = std::make_unique<DataIO::ValueNode>(to_string(m_selectionEnd));
        node->propertyValuePairs["Step"] = std::make_unique<DataIO::ValueNode>(to_string(m_step));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stof(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stof(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["selectionstart"])
            setSelectionStart(tgui::stof(node->propertyValuePairs["selectionstart"]->value));
        if (node->propertyValuePairs["selectionend"])
            setSelectionEnd(tgui::stof(node->propertyValuePairs["selectionend"]->value));
        if (node->propertyValuePairs["step"])
            setStep(tgui::stof(node->propertyValuePairs["step"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RangeSlider::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::updateThumbPositions()
    {
        const Vector2f innerSize = getInnerSize();

        if (m_verticalScroll)
        {
            m_thumbs.first.left = m_bordersCached.getLeft() + (innerSize.x - m_thumbs.first.width) / 2.0f;
            m_thumbs.first.top = (innerSize.y / (m_maximum - m_minimum) * (m_maximum - m_selectionStart)) - (m_thumbs.first.height / 2.0f);

            m_thumbs.second.left = m_bordersCached.getLeft() + (innerSize.x - m_thumbs.second.width) / 2.0f;
            m_thumbs.second.top = (innerSize.y / (m_maximum - m_minimum) * (m_maximum - m_selectionEnd)) - (m_thumbs.second.height / 2.0f);
        }
        else // horizontal
        {
            m_thumbs.first.left = (innerSize.x / (m_maximum - m_minimum) * (m_selectionStart - m_minimum)) - (m_thumbs.first.width / 2.0f);
            m_thumbs.first.top = m_bordersCached.getTop() + (innerSize.y - m_thumbs.first.height) / 2.0f;

            m_thumbs.second.left = (innerSize.x / (m_maximum - m_minimum) * (m_selectionEnd - m_minimum)) - (m_thumbs.second.width / 2.0f);
            m_thumbs.second.top = m_bordersCached.getTop() + (innerSize.y - m_thumbs.second.height) / 2.0f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders around the track
        if (m_bordersCached != Borders{0})
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the track
        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            if (m_mouseHover && m_spriteTrackHover.isSet())
                m_spriteTrackHover.draw(target, states);
            else
                m_spriteTrack.draw(target, states);
        }
        else // There are no textures
        {
            if (m_mouseHover && m_trackColorHoverCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_trackColorHoverCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_trackColorCached);

            if (m_selectedTrackColorCached.isSet())
            {
                sf::RenderStates selectedTrackStates = states;
                Vector2f size;

                if (m_verticalScroll)
                {
                    selectedTrackStates.transform.translate({0, m_thumbs.first.top + m_thumbs.first.height / 2.f});
                    size = {getInnerSize().x, m_thumbs.second.top - m_thumbs.first.top};
                }
                else
                {
                    selectedTrackStates.transform.translate({m_thumbs.first.left + m_thumbs.first.width / 2.f, 0});
                    size = {m_thumbs.second.left - m_thumbs.first.left, getInnerSize().y};
                }

                if (m_mouseHover && m_selectedTrackColorHoverCached.isSet())
                    drawRectangleShape(target, selectedTrackStates, size, m_selectedTrackColorHoverCached);
                else
                    drawRectangleShape(target, selectedTrackStates, size, m_selectedTrackColorCached);
            }
        }

        const auto oldStates = states;

        // Draw the first thumb
        {
            states.transform.translate({-m_bordersCached.getLeft() + m_thumbs.first.left, -m_bordersCached.getTop() + m_thumbs.first.top});

            // Draw the borders around the thumb when using colors
            if ((m_bordersCached != Borders{0}) && !(m_spriteTrack.isSet() && m_spriteThumb.isSet()))
            {
                if (m_mouseHover && m_borderColorHoverCached.isSet())
                    drawBorders(target, states, m_bordersCached, {m_thumbs.first.width, m_thumbs.first.height}, m_borderColorHoverCached);
                else
                    drawBorders(target, states, m_bordersCached, {m_thumbs.first.width, m_thumbs.first.height}, m_borderColorCached);

                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            // Draw the thumb
            if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
            {
                if (m_mouseHover && m_spriteThumbHover.isSet())
                    m_spriteThumbHover.draw(target, states);
                else
                    m_spriteThumb.draw(target, states);
            }
            else // There are no textures
            {
                const Vector2f thumbInnerSize = {m_thumbs.first.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                                     m_thumbs.first.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

                if (m_mouseHover && m_thumbColorHoverCached.isSet())
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorHoverCached);
                else
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorCached);
            }
        }

        states = oldStates;

        // Draw the second thumb
        {
            states.transform.translate({-m_bordersCached.getLeft() + m_thumbs.second.left, -m_bordersCached.getTop() + m_thumbs.second.top});

            // Draw the borders around the thumb when using colors
            if ((m_bordersCached != Borders{0}) && !(m_spriteTrack.isSet() && m_spriteThumb.isSet()))
            {
                if (m_mouseHover && m_borderColorHoverCached.isSet())
                    drawBorders(target, states, m_bordersCached, {m_thumbs.second.width, m_thumbs.second.height}, m_borderColorHoverCached);
                else
                    drawBorders(target, states, m_bordersCached, {m_thumbs.second.width, m_thumbs.second.height}, m_borderColorCached);

                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            // Draw the thumb
            if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
            {
                if (m_mouseHover && m_spriteThumbHover.isSet())
                    m_spriteThumbHover.draw(target, states);
                else
                    m_spriteThumb.draw(target, states);
            }
            else // There are no textures
            {
                const Vector2f thumbInnerSize = {m_thumbs.second.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                                     m_thumbs.second.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

                if (m_mouseHover && m_thumbColorHoverCached.isSet())
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorHoverCached);
                else
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
