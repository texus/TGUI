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


#include <TGUI/Widgets/Slider.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Slider::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<SliderRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::create(float minimum, float maximum)
    {
        auto slider = std::make_shared<Slider>();

        slider->setMinimum(minimum);
        slider->setMaximum(maximum);

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::copy(const Slider::ConstPtr& slider)
    {
        if (slider)
            return std::static_pointer_cast<Slider>(slider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* Slider::getSharedRenderer()
    {
        return aurora::downcast<SliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SliderRenderer* Slider::getSharedRenderer() const
    {
        return aurora::downcast<const SliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* Slider::getRenderer()
    {
        return aurora::downcast<SliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else if (getSize().x > getSize().y)
            m_verticalScroll = false;

        if (m_spriteTrack.isSet())
        {
            if (m_verticalImage == m_verticalScroll)
            {
                m_spriteTrack.setSize(getInnerSize());
                m_spriteTrackHover.setSize(getInnerSize());
                m_spriteTrack.setRotation(0);
                m_spriteTrackHover.setRotation(0);
            }
            else // The image is rotated
            {
                m_spriteTrack.setSize({getInnerSize().y, getInnerSize().x});
                m_spriteTrackHover.setSize({getInnerSize().y, getInnerSize().x});
                m_spriteTrack.setRotation(-90);
                m_spriteTrackHover.setRotation(-90);
            }
        }

        if (m_spriteThumb.isSet())
        {
            float scaleFactor = 1;
            if (m_spriteTrack.isSet())
            {
                if (m_verticalImage == m_verticalScroll)
                {
                    if (m_verticalScroll)
                        scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().x;
                    else
                        scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().y;
                }
                else // The image is rotated
                {
                    if (m_verticalScroll)
                        scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().y;
                    else
                        scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().x;
                }
            }

            m_thumb.width = scaleFactor * m_spriteThumb.getTexture().getImageSize().x;
            m_thumb.height = scaleFactor * m_spriteThumb.getTexture().getImageSize().y;

            m_spriteThumb.setSize({m_thumb.width, m_thumb.height});
            m_spriteThumbHover.setSize({m_thumb.width, m_thumb.height});

            // Apply the rotation now that the size has been set
            if (m_verticalScroll != m_verticalImage)
            {
                m_spriteThumb.setRotation(-90);
                m_spriteThumbHover.setRotation(-90);
                std::swap(m_thumb.width, m_thumb.height);
            }
            else
            {
                m_spriteThumb.setRotation(0);
                m_spriteThumbHover.setRotation(0);
            }
        }
        else // There is no thumb texture
        {
            if (m_verticalScroll)
            {
                m_thumb.width = getSize().x * 1.6f;
                m_thumb.height = m_thumb.width / 2.0f;
            }
            else
            {
                m_thumb.height = getSize().y * 1.6f;
                m_thumb.width = m_thumb.height / 2.0f;
            }
        }

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getFullSize() const
    {
        if (m_thumbWithinTrackCached)
        {
            if (m_verticalScroll)
                return {std::max(getSize().x, m_thumb.width), getSize().y};
            else
                return {getSize().x, std::max(getSize().y, m_thumb.height)};
        }
        else
        {
            if (m_verticalScroll)
                return {std::max(getSize().x, m_thumb.width), getSize().y + m_thumb.height};
            else
                return {getSize().x + m_thumb.width, std::max(getSize().y, m_thumb.height)};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getWidgetOffset() const
    {
        if (m_thumbWithinTrackCached)
        {
            if (m_verticalScroll)
                return {std::min(0.f, (getSize().x - m_thumb.width) / 2.f), 0};
            else
                return {0, std::min(0.f, (getSize().y - m_thumb.height) / 2.f)};
        }
        else
        {
            if (m_verticalScroll)
                return {std::min(0.f, (getSize().x - m_thumb.width) / 2.f), -m_thumb.height / 2.f};
            else
                return {-m_thumb.width / 2.f, std::min(0.f, (getSize().y - m_thumb.height) / 2.f)};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(float minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(float maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setValue(float value)
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

            onValueChange.emit(this, m_value);

            updateThumbPosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setStep(float step)
    {
        m_step = step;

        // Reset the value in case it does not match the step
        setValue(m_value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setVerticalScroll(bool vertical)
    {
        if (m_verticalScroll == vertical)
            return;

        m_verticalScroll = vertical;
        setSize(getSize().y, getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setInvertedDirection(bool invertedDirection)
    {
        m_invertedDirection = invertedDirection;
        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getInvertedDirection() const
    {
        return m_invertedDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setChangeValueOnScroll(bool changeValueOnScroll)
    {
        m_changeValueOnScroll = changeValueOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getChangeValueOnScroll() const
    {
        return m_changeValueOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::isMouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the thumb
        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
            return true;

        // Check if the mouse is on top of the track
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        const Vector2f localPos = pos - getPosition();
        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(localPos))
        {
            m_mouseDownOnThumb = true;
            m_mouseDownOnThumbPos.x = localPos.x - m_thumb.left;
            m_mouseDownOnThumbPos.y = localPos.y - m_thumb.top;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the value
        mouseMoved(pos);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(Vector2f)
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        if (!m_mouseDown)
            return;

        // Check in which direction the slider goes
        if (m_verticalScroll)
        {
            // Check if the click occurred on the track
            if (!m_mouseDownOnThumb)
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                m_mouseDownOnThumbPos.y = m_thumb.height / 2.0f;
            }

            float value = m_thumbWithinTrackCached
                ? m_maximum - (((pos.y - m_mouseDownOnThumbPos.y) / (getSize().y - m_thumb.height)) * (m_maximum - m_minimum))
                : m_maximum - (((pos.y + (m_thumb.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum));

            if (m_invertedDirection)
                value = m_maximum - (value - m_minimum);

            setValue(value);

            // Set the thumb position for smooth scrolling
            const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
            if ((m_thumbWithinTrackCached && ((thumbTop > 0) && (thumbTop + m_thumb.height < getSize().y)))
             || (!m_thumbWithinTrackCached && ((thumbTop + (m_thumb.height / 2.0f) > 0) && (thumbTop + (m_thumb.height / 2.0f) < getSize().y))))
                m_thumb.top = thumbTop;
            else
                updateThumbPosition();
        }
        else // the slider lies horizontal
        {
            // Check if the click occurred on the track
            if (!m_mouseDownOnThumb)
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = m_thumb.width / 2.0f;
                m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
            }

            float value = m_thumbWithinTrackCached
                ? (((pos.x - m_mouseDownOnThumbPos.x) / (getSize().x - m_thumb.width)) * (m_maximum - m_minimum)) + m_minimum
                : (((pos.x + (m_thumb.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum;

            if (m_invertedDirection)
                value = m_maximum - (value - m_minimum);

            setValue(value);

            // Set the thumb position for smooth scrolling
            const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
            if ((m_thumbWithinTrackCached && ((thumbLeft > 0) && (thumbLeft + m_thumb.width < getSize().x)))
             || (!m_thumbWithinTrackCached && ((thumbLeft + (m_thumb.width / 2.0f) > 0) && (thumbLeft + (m_thumb.width / 2.0f) < getSize().x))))
                m_thumb.left = thumbLeft;
            else
                updateThumbPosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::scrolled(float delta, Vector2f, bool touch)
    {
        // Don't react to two finger scrolling
        if (touch)
            return false;

        // Don't do anything when changing value on scroll is disabled
        if (!m_changeValueOnScroll)
            return false;

        if (m_invertedDirection)
            delta = -delta;

        if (m_step == 0)
            setValue(m_value + delta);
        else
        {
            if (std::abs(delta) <= 1)
            {
                if (delta > 0)
                    setValue(m_value + m_step);
                else if (delta < 0)
                    setValue(m_value - m_step);
            }
            else
                setValue(m_value + std::round(delta) * m_step);
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseButtonNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();

        Widget::leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Slider::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"TextureTrack")
        {
            m_spriteTrack.setTexture(getSharedRenderer()->getTextureTrack());

            if (m_spriteTrack.getTexture().getImageSize().x < m_spriteTrack.getTexture().getImageSize().y)
                m_verticalImage = true;
            else
                m_verticalImage = false;

            setSize(m_size);
        }
        else if (property == U"TextureTrackHover")
        {
            m_spriteTrackHover.setTexture(getSharedRenderer()->getTextureTrackHover());
        }
        else if (property == U"TextureThumb")
        {
            m_spriteThumb.setTexture(getSharedRenderer()->getTextureThumb());
            setSize(m_size);
        }
        else if (property == U"TextureThumbHover")
        {
            m_spriteThumbHover.setTexture(getSharedRenderer()->getTextureThumbHover());
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
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BorderColorHover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == U"ThumbWithinTrack")
        {
            m_thumbWithinTrackCached = getSharedRenderer()->getThumbWithinTrack();
            updateThumbPosition();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
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

    std::unique_ptr<DataIO::Node> Slider::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs[U"Minimum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimum));
        node->propertyValuePairs[U"Maximum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximum));
        node->propertyValuePairs[U"Value"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_value));
        node->propertyValuePairs[U"Step"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_step));
        node->propertyValuePairs[U"InvertedDirection"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_invertedDirection));
        node->propertyValuePairs[U"ChangeValueOnScroll"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_changeValueOnScroll));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
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
        if (node->propertyValuePairs[U"InvertedDirection"])
            setInvertedDirection(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"InvertedDirection"]->value).getBool());
        if (node->propertyValuePairs[U"ChangeValueOnScroll"])
            setChangeValueOnScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"ChangeValueOnScroll"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = m_bordersCached.getLeft() + (getInnerSize().x - m_thumb.width) / 2.0f;

            if (m_thumbWithinTrackCached)
                m_thumb.top = (getSize().y - m_thumb.height) / (m_maximum - m_minimum) * (m_maximum - m_value);
            else
                m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_value)) - (m_thumb.height / 2.0f);

            if (m_invertedDirection)
                m_thumb.top = getSize().y - m_thumb.top - m_thumb.height;
        }
        else
        {
            m_thumb.top = m_bordersCached.getTop() + (getInnerSize().y - m_thumb.height) / 2.0f;

            if (m_thumbWithinTrackCached)
                m_thumb.left = (getSize().x - m_thumb.width) / (m_maximum - m_minimum) * (m_value - m_minimum);
            else
                m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f);

            if (m_invertedDirection)
                m_thumb.left = getSize().x - m_thumb.left - m_thumb.width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders around the track
        if (m_bordersCached != Borders{0})
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorHoverCached, m_opacityCached));
            else
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));

            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the track
        if (m_spriteTrack.isSet())
        {
            if (m_mouseHover && m_spriteTrackHover.isSet())
                target.drawSprite(states, m_spriteTrackHover);
            else
                target.drawSprite(states, m_spriteTrack);
        }
        else // There are no textures
        {
            if (m_mouseHover && m_trackColorHoverCached.isSet())
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_trackColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_trackColorCached, m_opacityCached));
        }

        states.transform.translate({-m_bordersCached.getLeft() + m_thumb.left, -m_bordersCached.getTop() + m_thumb.top});

        // Draw the thumb
        if (m_spriteThumb.isSet())
        {
            if (m_mouseHover && m_spriteThumbHover.isSet())
                target.drawSprite(states, m_spriteThumbHover);
            else
                target.drawSprite(states, m_spriteThumb);
        }
        else // There are no textures
        {
            if (m_bordersCached != Borders{0})
            {
                // Draw the borders around the thumb
                if (m_mouseHover && m_borderColorHoverCached.isSet())
                    target.drawBorders(states, m_bordersCached, {m_thumb.width, m_thumb.height}, Color::applyOpacity(m_borderColorHoverCached, m_opacityCached));
                else
                    target.drawBorders(states, m_bordersCached, {m_thumb.width, m_thumb.height}, Color::applyOpacity(m_borderColorCached, m_opacityCached));

                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            const Vector2f thumbInnerSize = {m_thumb.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                             m_thumb.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

            if (m_mouseHover && m_thumbColorHoverCached.isSet())
                target.drawFilledRect(states, thumbInnerSize, Color::applyOpacity(m_thumbColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, thumbInnerSize, Color::applyOpacity(m_thumbColorCached, m_opacityCached));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Slider::clone() const
    {
        return std::make_shared<Slider>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
