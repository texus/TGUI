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


#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Loading/Theme.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider()
    {
        m_callback.widgetType = "Slider";
        m_draggableWidget = true;

        addSignal<int>("ValueChanged");

        m_renderer = std::make_shared<SliderRenderer>(this);
        reload();

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::create(int minimum, int maximum)
    {
        auto slider = std::make_shared<Slider>();

        slider->setMinimum(minimum);
        slider->setMaximum(maximum);

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::copy(Slider::ConstPtr slider)
    {
        if (slider)
            return std::static_pointer_cast<Slider>(slider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        if (m_verticalScroll)
        {
            m_thumb.left = getPosition().x + ((getSize().x - m_thumb.width) / 2.0f);
            m_thumb.top = getPosition().y - (m_thumb.height / 2.0f) + (getSize().y / (m_maximum - m_minimum) * (m_value - m_minimum));
        }
        else
        {
            m_thumb.left = getPosition().x - (m_thumb.width / 2.0f) + (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum));
            m_thumb.top = getPosition().y + ((getSize().y - m_thumb.height) / 2.0f);
        }

        if (getRenderer()->m_textureTrackNormal.isLoaded() && getRenderer()->m_textureThumbNormal.isLoaded())
        {
            getRenderer()->m_textureTrackNormal.setPosition(getPosition());
            getRenderer()->m_textureTrackHover.setPosition(getPosition());

            getRenderer()->m_textureThumbNormal.setPosition({m_thumb.left, m_thumb.top});
            getRenderer()->m_textureThumbHover.setPosition({m_thumb.left, m_thumb.top});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        if (getRenderer()->m_textureTrackNormal.isLoaded() && getRenderer()->m_textureThumbNormal.isLoaded())
        {
            if (m_verticalImage == m_verticalScroll)
            {
                getRenderer()->m_textureTrackNormal.setSize(getSize());
                getRenderer()->m_textureTrackHover.setSize(getSize());

                if (m_verticalScroll)
                {
                    m_thumb.width = getSize().x / getRenderer()->m_textureTrackNormal.getImageSize().x * getRenderer()->m_textureThumbNormal.getImageSize().x;
                    m_thumb.height = getSize().x / getRenderer()->m_textureTrackNormal.getImageSize().x * getRenderer()->m_textureThumbNormal.getImageSize().y;
                }
                else
                {
                    m_thumb.width = getSize().y / getRenderer()->m_textureTrackNormal.getImageSize().y * getRenderer()->m_textureThumbNormal.getImageSize().x;
                    m_thumb.height = getSize().y / getRenderer()->m_textureTrackNormal.getImageSize().y * getRenderer()->m_textureThumbNormal.getImageSize().y;
                }
            }
            else // The image is rotated
            {
                getRenderer()->m_textureTrackNormal.setSize({getSize().y, getSize().x});
                getRenderer()->m_textureTrackHover.setSize({getSize().y, getSize().x});

                if (m_verticalScroll)
                {
                    m_thumb.width = getSize().x / getRenderer()->m_textureTrackNormal.getImageSize().y * getRenderer()->m_textureThumbNormal.getImageSize().x;
                    m_thumb.height = getSize().x / getRenderer()->m_textureTrackNormal.getImageSize().y * getRenderer()->m_textureThumbNormal.getImageSize().y;
                }
                else
                {
                    m_thumb.width = getSize().y / getRenderer()->m_textureTrackNormal.getImageSize().x * getRenderer()->m_textureThumbNormal.getImageSize().x;
                    m_thumb.height = getSize().y / getRenderer()->m_textureTrackNormal.getImageSize().x * getRenderer()->m_textureThumbNormal.getImageSize().y;
                }
            }

            getRenderer()->m_textureThumbNormal.setSize({m_thumb.width, m_thumb.height});
            getRenderer()->m_textureThumbHover.setSize({m_thumb.width, m_thumb.height});

            // Apply the rotation now that the size has been set
            if (m_verticalScroll != m_verticalImage)
            {
                getRenderer()->m_textureTrackNormal.setRotation(-90);
                getRenderer()->m_textureTrackHover.setRotation(-90);
                getRenderer()->m_textureThumbNormal.setRotation(-90);
                getRenderer()->m_textureThumbHover.setRotation(-90);
            }
            else
            {
                getRenderer()->m_textureTrackNormal.setRotation(0);
                getRenderer()->m_textureTrackHover.setRotation(0);
                getRenderer()->m_textureThumbNormal.setRotation(0);
                getRenderer()->m_textureThumbHover.setRotation(0);
            }
        }
        else // There are no textures
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

        // Recalculate the position of the images
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);

        // Recalculate the position of the thumb image
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_maximum = maximum;
        else
            m_maximum = 1;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);

        // Recalculate the position of the thumb image
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setValue(int value)
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
            sendSignal("ValueChanged", m_value);

            // Recalculate the position of the thumb image
            updatePosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        getRenderer()->m_textureTrackNormal.setColor({getRenderer()->m_textureTrackNormal.getColor().r, getRenderer()->m_textureTrackNormal.getColor().g, getRenderer()->m_textureTrackNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureTrackHover.setColor({getRenderer()->m_textureTrackHover.getColor().r, getRenderer()->m_textureTrackHover.getColor().g, getRenderer()->m_textureTrackHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureThumbNormal.setColor({getRenderer()->m_textureThumbNormal.getColor().r, getRenderer()->m_textureThumbNormal.getColor().g, getRenderer()->m_textureThumbNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureThumbHover.setColor({getRenderer()->m_textureThumbHover.getColor().r, getRenderer()->m_textureThumbHover.getColor().g, getRenderer()->m_textureThumbHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(float x, float y) const
    {
        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(x, y))
            return true;

        // Check if the mouse is on top of the track
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMousePressed(float x, float y)
    {
        m_mouseDown = true;

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(float, float)
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Check in which direction the slider goes
            if (m_verticalScroll)
            {
                // Check if the click occurred on the track
                if (!m_mouseDownOnThumb)
                {
                    m_mouseDownOnThumb = true;
                    m_mouseDownOnThumbPos.x = x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = m_thumb.height / 2.0f;
                }

                // Set the new value
                if ((y - m_mouseDownOnThumbPos.y + (m_thumb.height / 2.0f) - getPosition().y) > 0)
                    setValue(static_cast<int>((((y + (m_thumb.height / 2.0f) - m_mouseDownOnThumbPos.y - getPosition().y) / getSize().y) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                else
                    setValue(m_minimum);

                // Set the thumb position for smooth scrolling
                float thumbTop = y - m_mouseDownOnThumbPos.y;
                if ((thumbTop + (m_thumb.height / 2.0f) > getPosition().y) && (thumbTop + (m_thumb.height / 2.0f) < getPosition().y + getSize().y))
                    m_thumb.top = thumbTop;
                else
                    m_thumb.top = getPosition().y - (m_thumb.height / 2.0f) + (getSize().y / (m_maximum - m_minimum) * (m_value - m_minimum));
            }
            else // the slider lies horizontal
            {
                // Check if the click occurred on the track
                if (!m_mouseDownOnThumb)
                {
                    m_mouseDownOnThumb = true;
                    m_mouseDownOnThumbPos.x = m_thumb.width / 2.0f;
                    m_mouseDownOnThumbPos.y = y - m_thumb.top;
                }

                // Set the new value
                if ((x - m_mouseDownOnThumbPos.x + (m_thumb.width / 2.0f) - getPosition().x) > 0)
                    setValue(static_cast<int>((((x + (m_thumb.width / 2.0f) - m_mouseDownOnThumbPos.x - getPosition().x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                else
                    setValue(m_minimum);

                // Set the thumb position for smooth scrolling
                float thumbLeft = x - m_mouseDownOnThumbPos.x;
                if ((thumbLeft + (m_thumb.width / 2.0f) > getPosition().x) && (thumbLeft + (m_thumb.width / 2.0f) < getPosition().x + getSize().x))
                    m_thumb.left = thumbLeft;
                else
                    m_thumb.left = getPosition().x - (m_thumb.width / 2.0f) + (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum));
            }

            if (getRenderer()->m_textureThumbNormal.isLoaded())
            {
                getRenderer()->m_textureThumbNormal.setPosition({m_thumb.left, m_thumb.top});
                getRenderer()->m_textureThumbHover.setPosition({m_thumb.left, m_thumb.top});
            }
        }
        else // Normal mouse move
        {
            // Set some variables so that when the mouse goes down we know whether it is on the track or not
            if (sf::FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(x, y))
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = x - m_thumb.left;
                m_mouseDownOnThumbPos.y = y - m_thumb.top;
            }
            else // The mouse is not on top of the thumb
                m_mouseDownOnThumb = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseWheelMoved(int delta, int, int)
    {
        if (m_value - delta < m_minimum)
            setValue(m_minimum);
        else
            setValue(m_value - delta);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::widgetFocused()
    {
        // A slider can't be focused (yet)
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updatePosition();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setTrackColorNormal({245, 245, 245});
        getRenderer()->setTrackColorHover({255, 255, 255});
        getRenderer()->setThumbColorNormal({245, 245, 245});
        getRenderer()->setThumbColorHover({255, 255, 255});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setTrackTexture({});
        getRenderer()->setTrackHoverTexture({});
        getRenderer()->setThumbTexture({});
        getRenderer()->setThumbHoverTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                // Use the size of the images when images were loaded
                if (getRenderer()->m_textureTrackNormal.isLoaded() && getRenderer()->m_textureThumbNormal.isLoaded())
                {
                    if (getRenderer()->m_textureTrackNormal.getImageSize().x < getRenderer()->m_textureTrackNormal.getImageSize().y)
                        m_verticalImage = true;
                    else
                        m_verticalImage = false;

                    setSize(getRenderer()->m_textureTrackNormal.getImageSize());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "trackcolor")
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
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "trackimage")
            setTrackTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "trackhoverimage")
            setTrackHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "thumbimage")
            setThumbTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "thumbhoverimage")
            setThumbHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
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
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
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
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter SliderRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "trackcolor")
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
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "trackimage")
            return m_textureTrackNormal;
        else if (property == "trackhoverimage")
            return m_textureTrackHover;
        else if (property == "thumbimage")
            return m_textureThumbNormal;
        else if (property == "thumbhoverimage")
            return m_textureThumbHover;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> SliderRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
        {
            pairs["TrackImage"] = m_textureTrackNormal;
            pairs["ThumbImage"] = m_textureThumbNormal;
            if (m_textureTrackHover.isLoaded())
                pairs["TrackHoverImage"] = m_textureTrackHover;
            if (m_textureThumbHover.isLoaded())
                pairs["ThumbHoverImage"] = m_textureThumbHover;
        }
        else
        {
            pairs["TrackColorNormal"] = m_trackColorNormal;
            pairs["TrackColorHover"] = m_trackColorHover;
            pairs["ThumbColorNormal"] = m_thumbColorNormal;
            pairs["ThumbColorHover"] = m_thumbColorHover;
        }

        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackColor(const Color& color)
    {
        setTrackColorNormal(color);
        setTrackColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackColorNormal(const Color& color)
    {
        m_trackColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackColorHover(const Color& color)
    {
        m_trackColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbColor(const Color& color)
    {
        setThumbColorNormal(color);
        setThumbColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbColorNormal(const Color& color)
    {
        m_thumbColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbColorHover(const Color& color)
    {
        m_thumbColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackTexture(const Texture& texture)
    {
        m_textureTrackNormal = texture;
        if (m_textureTrackNormal.isLoaded())
        {
            m_textureTrackNormal.setColor({m_textureTrackNormal.getColor().r, m_textureTrackNormal.getColor().g, m_textureTrackNormal.getColor().b, static_cast<sf::Uint8>(m_slider->getOpacity() * 255)});

            if (m_textureTrackNormal.getImageSize().x < m_textureTrackNormal.getImageSize().y)
                m_slider->m_verticalImage = true;
            else
                m_slider->m_verticalImage = false;

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
                m_slider->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackHoverTexture(const Texture& texture)
    {
        m_textureTrackHover = texture;
        if (m_textureTrackHover.isLoaded())
        {
            m_textureTrackHover.setColor({m_textureTrackHover.getColor().r, m_textureTrackHover.getColor().g, m_textureTrackHover.getColor().b, static_cast<sf::Uint8>(m_slider->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
                m_slider->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbTexture(const Texture& texture)
    {
        m_textureThumbNormal = texture;
        if (m_textureThumbNormal.isLoaded())
        {
            m_textureThumbNormal.setColor({m_textureThumbNormal.getColor().r, m_textureThumbNormal.getColor().g, m_textureThumbNormal.getColor().b, static_cast<sf::Uint8>(m_slider->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
                m_slider->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbHoverTexture(const Texture& texture)
    {
        m_textureThumbHover = texture;
        if (m_textureThumbHover.isLoaded())
        {
            m_textureThumbHover.setColor({m_textureThumbHover.getColor().r, m_textureThumbHover.getColor().g, m_textureThumbHover.getColor().b, static_cast<sf::Uint8>(m_slider->getOpacity() * 255)});

            if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
                m_slider->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the track
        if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
        {
            if (m_slider->m_mouseHover && m_textureTrackHover.isLoaded())
                target.draw(m_textureTrackHover, states);
            else
                target.draw(m_textureTrackNormal, states);
        }
        else // There are no textures
        {
            sf::RectangleShape track{m_slider->getSize()};
            track.setPosition(m_slider->getPosition());

            if (m_slider->m_mouseHover)
                track.setFillColor(calcColorOpacity(m_trackColorHover, m_slider->getOpacity()));
            else
                track.setFillColor(calcColorOpacity(m_trackColorNormal, m_slider->getOpacity()));

            target.draw(track, states);
        }

        // Draw the borders around the track and thumb
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position[2] = {m_slider->getPosition(), {m_slider->m_thumb.left, m_slider->m_thumb.top}};
            sf::Vector2f size[2] = {m_slider->getSize(), {m_slider->m_thumb.width, m_slider->m_thumb.height}};

            for (unsigned int i = 0; i < 2; ++i)
            {
                // Draw left border
                sf::RectangleShape border({m_borders.left, size[i].y + m_borders.top});
                border.setPosition(position[i].x - m_borders.left, position[i].y - m_borders.top);
                border.setFillColor(calcColorOpacity(m_borderColor, m_slider->getOpacity()));
                target.draw(border, states);

                // Draw top border
                border.setSize({size[i].x + m_borders.right, m_borders.top});
                border.setPosition(position[i].x, position[i].y - m_borders.top);
                target.draw(border, states);

                // Draw right border
                border.setSize({m_borders.right, size[i].y + m_borders.bottom});
                border.setPosition(position[i].x + size[i].x, position[i].y);
                target.draw(border, states);

                // Draw bottom border
                border.setSize({size[i].x + m_borders.left, m_borders.bottom});
                border.setPosition(position[i].x - m_borders.left, position[i].y + size[i].y);
                target.draw(border, states);
            }
        }

        // Draw the thumb
        if (m_textureTrackNormal.isLoaded() && m_textureThumbNormal.isLoaded())
        {
            if (m_slider->m_mouseHover && m_textureThumbHover.isLoaded())
                target.draw(m_textureThumbHover, states);
            else
                target.draw(m_textureThumbNormal, states);
        }
        else // There are no textures
        {
            sf::RectangleShape thumb{{m_slider->m_thumb.width, m_slider->m_thumb.height}};
            thumb.setPosition({m_slider->m_thumb.left, m_slider->m_thumb.top});

            if (m_slider->m_mouseHover)
                thumb.setFillColor(calcColorOpacity(m_thumbColorHover, m_slider->getOpacity()));
            else
                thumb.setFillColor(calcColorOpacity(m_thumbColorNormal, m_slider->getOpacity()));

            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> SliderRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<SliderRenderer>(*this);
        renderer->m_slider = static_cast<Slider*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
