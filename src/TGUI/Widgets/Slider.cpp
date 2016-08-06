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


#include <TGUI/Widgets/Slider.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider()
    {
        m_callback.widgetType = "Slider";
        m_type = "Slider";

        m_draggableWidget = true;

        addSignal<int>("ValueChanged");

        m_renderer = aurora::makeCopied<SliderRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({2});
        getRenderer()->setTrackColor({245, 245, 245});
        getRenderer()->setTrackColorHover({255, 255, 255});
        getRenderer()->setThumbColor({245, 245, 245});
        getRenderer()->setThumbColorHover({255, 255, 255});
        getRenderer()->setBorderColor({60, 60, 60});
        getRenderer()->setBorderColorHover(sf::Color::Black);

        setSize(200, 16);
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

    void Slider::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        if (getRenderer()->getTextureTrack().isLoaded() && getRenderer()->getTextureThumb().isLoaded())
        {
            float scaleFactor;
            if (m_verticalImage == m_verticalScroll)
            {
                getRenderer()->getTextureTrack().setSize(getInnerSize());
                getRenderer()->getTextureTrackHover().setSize(getInnerSize());

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / getRenderer()->getTextureTrack().getImageSize().x;
                else
                    scaleFactor = getInnerSize().y / getRenderer()->getTextureTrack().getImageSize().y;
            }
            else // The image is rotated
            {
                getRenderer()->getTextureTrack().setSize({getInnerSize().y, getInnerSize().x});
                getRenderer()->getTextureTrackHover().setSize({getInnerSize().y, getInnerSize().x});

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / getRenderer()->getTextureTrack().getImageSize().y;
                else
                    scaleFactor = getInnerSize().y / getRenderer()->getTextureTrack().getImageSize().x;
            }

            m_thumb.width = scaleFactor * getRenderer()->getTextureThumb().getImageSize().x;
            m_thumb.height = scaleFactor * getRenderer()->getTextureThumb().getImageSize().y;

            getRenderer()->getTextureThumb().setSize({m_thumb.width, m_thumb.height});
            getRenderer()->getTextureThumbHover().setSize({m_thumb.width, m_thumb.height});

            // Apply the rotation now that the size has been set
            if (m_verticalScroll != m_verticalImage)
            {
                getRenderer()->getTextureTrack().setRotation(-90);
                getRenderer()->getTextureTrackHover().setRotation(-90);
                getRenderer()->getTextureThumb().setRotation(-90);
                getRenderer()->getTextureThumbHover().setRotation(-90);
            }
            else
            {
                getRenderer()->getTextureTrack().setRotation(0);
                getRenderer()->getTextureTrackHover().setRotation(0);
                getRenderer()->getTextureThumb().setRotation(0);
                getRenderer()->getTextureThumbHover().setRotation(0);
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

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getFullSize() const
    {
        if (m_verticalScroll)
            return {std::max(getSize().x, m_thumb.width), getSize().y + m_thumb.height};
        else
            return {getSize().x + m_thumb.width, std::max(getSize().y, m_thumb.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getWidgetOffset() const
    {
        if (m_verticalScroll)
            return {std::min(0.f, getSize().x - m_thumb.width), -m_thumb.height / 2.f};
        else
            return {-m_thumb.width / 2.f, std::min(0.f, getSize().y - m_thumb.height)};
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

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Slider::getMinimum() const
    {
        return m_minimum;
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

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Slider::getMaximum() const
    {
        return m_maximum;
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

            updateThumbPosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Slider::getValue() const
    {
        return m_value;
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
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            x -= getPosition().x;
            y -= getPosition().y;

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
                if (y - m_mouseDownOnThumbPos.y + (m_thumb.height / 2.0f) > 0)
                    setValue(static_cast<int>((((y + (m_thumb.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                else
                    setValue(m_minimum);

                // Set the thumb position for smooth scrolling
                float thumbTop = y - m_mouseDownOnThumbPos.y;
                if ((thumbTop + (m_thumb.height / 2.0f) > 0) && (thumbTop + (m_thumb.height / 2.0f) < getSize().y))
                    m_thumb.top = thumbTop;
                else
                    m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.height / 2.0f);
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
                if (x - m_mouseDownOnThumbPos.x + (m_thumb.width / 2.0f) > 0)
                    setValue(static_cast<int>((((x + (m_thumb.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                else
                    setValue(m_minimum);

                // Set the thumb position for smooth scrolling
                float thumbLeft = x - m_mouseDownOnThumbPos.x;
                if ((thumbLeft + (m_thumb.width / 2.0f) > 0) && (thumbLeft + (m_thumb.width / 2.0f) < getSize().x))
                    m_thumb.left = thumbLeft;
                else
                    m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f) ;
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
            updateThumbPosition();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "borders")
        {
            updateSize();
        }
        else if (property == "texturetrack")
        {
            if (value.getTexture().getImageSize().x < value.getTexture().getImageSize().y)
                m_verticalImage = true;
            else
                m_verticalImage = false;

            updateSize();
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if ((property == "texturetrackhover") || (property == "texturethumb") || (property == "texturethumbhover"))
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
        }
        else if ((property != "trackcolor")
              && (property != "trackcolorhover")
              && (property != "thumbcolor")
              && (property != "thumbcolorhover")
              && (property != "bordercolor")
              && (property != "bordercolorhover"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = (getSize().x - m_thumb.width) / 2.0f;
            m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.height / 2.0f);
        }
        else
        {
            m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f);
            m_thumb.top = (getSize().y - m_thumb.height) / 2.0f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders around the track
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            if (m_mouseHover && getRenderer()->getBorderColorHover().isSet())
                drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColorHover());
            else
                drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());

            states.transform.translate({borders.left, borders.top});
        }

        // Draw the track
        if (getRenderer()->getTextureTrack().isLoaded() && getRenderer()->getTextureThumb().isLoaded())
        {
            if (m_mouseHover && getRenderer()->getTextureTrackHover().isLoaded())
                getRenderer()->getTextureTrackHover().draw(target, states);
            else
                getRenderer()->getTextureTrack().draw(target, states);
        }
        else // There are no textures
        {
            if (m_mouseHover && getRenderer()->getTrackColorHover().isSet())
                drawRectangleShape(target, states, getInnerSize(), getRenderer()->getTrackColorHover());
            else
                drawRectangleShape(target, states, getInnerSize(), getRenderer()->getTrackColor());
        }

        states.transform.translate({-borders.left + m_thumb.left, -borders.top + m_thumb.top});

        // Draw the borders around the thumb
        if (borders != Borders{0})
        {
            if (m_mouseHover && getRenderer()->getBorderColorHover().isSet())
                drawBorders(target, states, borders, {m_thumb.width, m_thumb.height}, getRenderer()->getBorderColorHover());
            else
                drawBorders(target, states, borders, {m_thumb.width, m_thumb.height}, getRenderer()->getBorderColor());

            states.transform.translate({borders.left, borders.top});
        }

        // Draw the thumb
        if (getRenderer()->getTextureTrack().isLoaded() && getRenderer()->getTextureThumb().isLoaded())
        {
            if (m_mouseHover && getRenderer()->getTextureThumbHover().isLoaded())
                getRenderer()->getTextureThumbHover().draw(target, states);
            else
                getRenderer()->getTextureThumb().draw(target, states);
        }
        else // There are no textures
        {
            sf::Vector2f thumbInnerSize = {m_thumb.width - borders.left - borders.right, m_thumb.height - borders.top - borders.bottom};
            if (m_mouseHover && getRenderer()->getThumbColorHover().isSet())
                drawRectangleShape(target, states, thumbInnerSize, getRenderer()->getThumbColorHover());
            else
                drawRectangleShape(target, states, thumbInnerSize, getRenderer()->getThumbColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
