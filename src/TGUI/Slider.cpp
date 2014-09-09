/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Container.hpp>
#include <TGUI/Slider.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider()
    {
        m_callback.widgetType = WidgetType::Slider;
        m_draggableWidget = true;

        m_renderer = std::make_shared<SliderRenderer>(this);

        getRenderer()->setBorders({2, 2, 2, 2});

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto slider = std::make_shared<Slider>();

        if (themeFileFilename != "")
        {
            slider->getRenderer()->setBorders({0, 0, 0, 0});

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    slider->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            // Use the size of the images when images were loaded
            if ((slider->getRenderer()->m_textureTrackNormal.getData() != nullptr) && (slider->getRenderer()->m_textureThumbNormal.getData() != nullptr))
            {
                if (slider->getRenderer()->m_textureTrackNormal.getImageSize().x < slider->getRenderer()->m_textureTrackNormal.getImageSize().y)
                    slider->m_verticalImage = true;
                else
                    slider->m_verticalImage = false;

                slider->setSize(slider->getRenderer()->m_textureTrackNormal.getImageSize());
            }
        }

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setPosition(const Layout& position)
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

        if (getRenderer()->m_textureTrackNormal.getData() && getRenderer()->m_textureThumbNormal.getData())
        {
            getRenderer()->m_textureTrackNormal.setPosition(getPosition());
            getRenderer()->m_textureTrackHover.setPosition(getPosition());

            getRenderer()->m_textureThumbNormal.setPosition({m_thumb.left, m_thumb.top});
            getRenderer()->m_textureThumbHover.setPosition({m_thumb.left, m_thumb.top});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(const Layout& size)
    {
        Widget::setSize(size);

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        if (getRenderer()->m_textureTrackNormal.getData() && getRenderer()->m_textureThumbNormal.getData())
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
                m_thumb.width = getSize().x * 1.6;
                m_thumb.height = m_thumb.width / 2.0f;
            }
            else
            {
                m_thumb.height = getSize().y * 1.6;
                m_thumb.width = m_thumb.height / 2.0f;
            }
        }

        // Recalculate the position of the images
        updatePosition();
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
        if (m_value != value)
        {
            // Set the new value
            m_value = value;

            // When the value is below the minimum or above the maximum then adjust it
            if (m_value < m_minimum)
                m_value = m_minimum;
            else if (m_value > m_maximum)
                m_value = m_maximum;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ValueChanged].empty() == false)
            {
                m_callback.trigger = ValueChanged;
                m_callback.value   = m_value;
                addCallback();
            }

            // Recalculate the position of the thumb image
            updatePosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        getRenderer()->m_textureTrackNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureTrackHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureThumbNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureThumbHover.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(x, y))
        {
            m_mouseDownOnThumb = true;
            m_mouseDownOnThumbPos.x = x - m_thumb.left;
            m_mouseDownOnThumbPos.y = y - m_thumb.top;
            return true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Check if the mouse is on top of the track
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;

        if (m_mouseHover)
            mouseLeftWidget();

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
        m_mouseDown = false;
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
            }
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

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "tracknormalimage")
            extractTextureFromString(property, value, rootPath, m_textureTrackNormal);
        else if (property == "trackhoverimage")
            extractTextureFromString(property, value, rootPath, m_textureTrackHover);
        else if (property == "thumbnormalimage")
            extractTextureFromString(property, value, rootPath, m_textureThumbNormal);
        else if (property == "thumbhoverimage")
            extractTextureFromString(property, value, rootPath, m_textureThumbHover);
        else if (property == "trackcolor")
            setTrackColor(extractColorFromString(property, value));
        else if (property == "trackcolornormal")
            setTrackColorNormal(extractColorFromString(property, value));
        else if (property == "trackcolorhover")
            setTrackColorHover(extractColorFromString(property, value));
        else if (property == "thumbcolor")
            setThumbColor(extractColorFromString(property, value));
        else if (property == "thumbcolornormal")
            setThumbColorNormal(extractColorFromString(property, value));
        else if (property == "thumbcolorhover")
            setThumbColorHover(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackNormalImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureTrackNormal.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureTrackNormal = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureTrackHover.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureTrackHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbNormalImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureThumbNormal.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureThumbNormal = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureThumbHover.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureThumbHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackColor(const sf::Color& color)
    {
        m_trackColorNormal = color;
        m_trackColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackColorNormal(const sf::Color& color)
    {
        m_trackColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setTrackColorHover(const sf::Color& color)
    {
        m_trackColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbColor(const sf::Color& color)
    {
        m_thumbColorNormal = color;
        m_thumbColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbColorNormal(const sf::Color& color)
    {
        m_thumbColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setThumbColorHover(const sf::Color& color)
    {
        m_thumbColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::setBorderColor(const sf::Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SliderRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the track
        if (m_textureTrackNormal.getData() && m_textureThumbNormal.getData())
        {
            if (m_slider->m_mouseHover && m_textureTrackHover.getData())
                target.draw(m_textureTrackHover, states);
            else
                target.draw(m_textureTrackNormal, states);
        }
        else // There are no textures
        {
            sf::RectangleShape track{m_slider->getSize()};
            track.setPosition(m_slider->getPosition());

            if (m_slider->m_mouseHover)
                track.setFillColor(m_trackColorHover);
            else
                track.setFillColor(m_trackColorNormal);

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
                border.setFillColor(m_borderColor);
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
        if (m_textureTrackNormal.getData() && m_textureThumbNormal.getData())
        {
            if (m_slider->m_mouseHover && m_textureThumbHover.getData())
                target.draw(m_textureThumbHover, states);
            else
                target.draw(m_textureThumbNormal, states);
        }
        else // There are no textures
        {
            sf::RectangleShape thumb{{m_slider->m_thumb.width, m_slider->m_thumb.height}};
            thumb.setPosition({m_slider->m_thumb.left, m_slider->m_thumb.top});

            if (m_slider->m_mouseHover)
                thumb.setFillColor(m_thumbColorHover);
            else
                thumb.setFillColor(m_thumbColorNormal);

            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> SliderRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<SliderRenderer>(new SliderRenderer{*this});
        renderer->m_slider = static_cast<Slider*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
