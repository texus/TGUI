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
#include <TGUI/Knob.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob()
    {
        m_callback.widgetType = WidgetType::Knob;
        m_draggableWidget = true;

        addSignal<SignalInt>("ValueChanged");

        m_renderer = std::make_shared<KnobRenderer>(this);

        getRenderer()->setBorders(5, 5, 5, 5);

        setSize(140, 140);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto knob = std::make_shared<Knob>();

        if (themeFileFilename != "")
        {
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
                    knob->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }
        }

        if ((knob->getRenderer()->m_backgroundTexture.getData() != nullptr) || (knob->getRenderer()->m_foregroundTexture.getData() != nullptr))
        {
            knob->getRenderer()->m_foregroundTexture.setRotation(knob->m_startRotation - knob->getRenderer()->m_imageRotation);
            knob->setSize(knob->getRenderer()->m_backgroundTexture.getImageSize());
        }

        return knob;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::copy(Knob::ConstPtr knob)
    {
        if (knob)
            return std::make_shared<Knob>(*knob);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        getRenderer()->m_backgroundTexture.setPosition(getPosition());
        getRenderer()->m_foregroundTexture.setPosition(getPosition().x + ((getSize().x - getRenderer()->m_foregroundTexture.getSize().x) / 2.0f),
                                                       getPosition().y + ((getSize().y - getRenderer()->m_foregroundTexture.getSize().x) / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(const Layout& size)
    {
        Widget::setSize(size);

        if (getRenderer()->m_backgroundTexture.getData())
        {
            getRenderer()->m_backgroundTexture.setSize(getSize());
            getRenderer()->m_foregroundTexture.setSize({getRenderer()->m_foregroundTexture.getImageSize().x / getRenderer()->m_backgroundTexture.getImageSize().x * getSize().x,
                                                        getRenderer()->m_foregroundTexture.getImageSize().y / getRenderer()->m_backgroundTexture.getImageSize().y * getSize().y});
        }

        // Recalculate the position of the images
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setStartRotation(float startRotation)
    {
        while (startRotation >= 360)
            startRotation -= 360;
        while (startRotation < 0)
            startRotation += 360;

        m_startRotation = startRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setEndRotation(float endRotation)
    {
        while (endRotation >= 360)
            endRotation -= 360;
        while (endRotation < 0)
            endRotation += 360;

        m_endRotation = endRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMinimum(int minimum)
    {
        if (m_minimum != minimum)
        {
            // Set the new minimum
            m_minimum = minimum;

            // The maximum can't be below the minimum
            if (m_maximum < m_minimum)
                m_maximum = m_minimum;

            // When the value is below the minimum then adjust it
            if (m_value < m_minimum)
                setValue(m_minimum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMaximum(int maximum)
    {
        if (m_maximum != maximum)
        {
            // Set the new maximum
            if (maximum > 0)
                m_maximum = maximum;
            else
                m_maximum = 1;

            // The minimum can't be below the maximum
            if (m_minimum > m_maximum)
                m_minimum = m_maximum;

            // When the value is above the maximum then adjust it
            if (m_value > m_maximum)
                setValue(m_maximum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setValue(int value)
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

            // The knob might have to point in a different direction
            recalculateRotation();

            m_callback.value = m_value;
            sendSignal("ValueChanged", m_value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setClockwiseTurning(bool clockwise)
    {
        m_clockwiseTurning = clockwise;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        getRenderer()->m_backgroundTexture.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_foregroundTexture.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the widget
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
        {
            if (getRenderer()->m_backgroundTexture.getData())
            {
                // Only return true when the pixel under the mouse isn't transparent
                if (!getRenderer()->m_backgroundTexture.isTransparentPixel(x, y))
                    return true;
            }
            else // There is no texture, the widget has a circle shape
            {
                sf::Vector2f centerPoint = getPosition() + (getSize() / 2.0f);
                sf::Vector2f mousePoint = {x, y};
                float distance = std::sqrt(std::pow(centerPoint.x - mousePoint.x, 2) + std::pow(centerPoint.y - mousePoint.y, 2));
                return (distance <= std::min(getSize().x, getSize().y));
            }
        }

        if (m_mouseHover)
            mouseLeftWidget();

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // Change the value of the knob depending on where you clicked
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMouseReleased(float, float)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        sf::Vector2f centerPosition = getPosition() + (getSize() / 2.0f);

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Find out the direction that the knob should now point
            if (compareFloats(x, centerPosition.x))
            {
                if (y > centerPosition.y)
                    m_angle = 270;
                else if (y < centerPosition.y)
                    m_angle = 90;
            }
            else
            {
                m_angle = std::atan2(centerPosition.y - y, x - centerPosition.x) * 180.0f / pi;
                if (m_angle < 0)
                    m_angle += 360;
            }

            // The angle might lie on a part where it isn't allowed
            if (m_angle > m_startRotation)
            {
                if ((m_angle < m_endRotation) && (m_clockwiseTurning))
                {
                    if ((m_angle - m_startRotation) <= (m_endRotation - m_angle))
                        m_angle = m_startRotation;
                    else
                        m_angle = m_endRotation;
                }
                else if (m_angle > m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_angle - m_startRotation, 360 - m_angle + m_startRotation) <= std::min(m_angle - m_endRotation, 360 - m_angle + m_endRotation))
                            m_angle = m_startRotation;
                        else
                            m_angle = m_endRotation;
                    }
                }
            }
            else if (m_angle < m_startRotation)
            {
                if (m_angle < m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_startRotation - m_angle, 360 - m_startRotation + m_angle) <= std::min(m_endRotation - m_angle, 360 -m_endRotation + m_angle))
                            m_angle = m_startRotation;
                        else
                            m_angle = m_endRotation;
                    }
                }
                else if ((m_angle > m_endRotation) && (!m_clockwiseTurning))
                {
                    if ((m_startRotation - m_angle) <= (m_angle - m_endRotation))
                        m_angle = m_startRotation;
                    else
                        m_angle = m_endRotation;
                }
            }

            // Calculate the difference in degrees between the start and end rotation
            float allowedAngle = 0;
            if (compareFloats(m_startRotation, m_endRotation))
                allowedAngle = 360;
            else
            {
                if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
                 || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
                {
                    allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
                }
                else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                      || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
                {
                    allowedAngle = std::abs(m_endRotation - m_startRotation);
                }
            }

            // Calculate the right value
            if (m_clockwiseTurning)
            {
                if (m_angle < m_startRotation)
                    setValue(static_cast<int>(((m_startRotation - m_angle) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    if (compareFloats(m_angle, m_startRotation))
                        setValue(m_minimum);
                    else
                        setValue(static_cast<int>((((360.0 - m_angle) + m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
            else // counter-clockwise
            {
                if (m_angle >= m_startRotation)
                    setValue(static_cast<int>(((m_angle - m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    setValue(static_cast<int>(((m_angle + (360.0 - m_startRotation)) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::widgetFocused()
    {
        // A knob can't be focused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::recalculateRotation()
    {
        // Calculate the difference in degrees between the start and end rotation
        float allowedAngle = 0;
        if (compareFloats(m_startRotation, m_endRotation))
            allowedAngle = 360;
        else
        {
            if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
             || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
            {
                allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
            }
            else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                  || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
            {
                allowedAngle = std::abs(m_endRotation - m_startRotation);
            }
        }

        // Calculate the angle for the direction of the knob
        if (m_clockwiseTurning)
        {
            if (m_value == m_minimum)
                m_angle = m_startRotation;
            else
                m_angle = m_startRotation - (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle);
        }
        else // counter-clockwise
        {
            m_angle = (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle) + m_startRotation;
        }

        // Give the image the correct rotation
        if (getRenderer()->m_imageRotation > m_angle)
            getRenderer()->m_foregroundTexture.setRotation(getRenderer()->m_imageRotation - m_angle);
        else
            getRenderer()->m_foregroundTexture.setRotation(360 - m_angle + getRenderer()->m_imageRotation);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backgroundimage")
            extractTextureFromString(property, value, rootPath, m_backgroundTexture);
        else if (property == "foregroundimage")
            extractTextureFromString(property, value, rootPath, m_foregroundTexture);
        else if (property == "imagerotation")
            setImageRotation(tgui::stof(value));
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "thumbcolor")
            setThumbColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_backgroundTexture.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_backgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setForegroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_foregroundTexture.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_foregroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setImageRotation(float rotation)
    {
        m_imageRotation = rotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setBackgroundColor(const sf::Color& color)
    {
        m_backgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setThumbColor(const sf::Color& color)
    {
        m_thumbColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::setBorderColor(const sf::Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KnobRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_backgroundTexture.getData() && m_foregroundTexture.getData())
        {
            target.draw(m_backgroundTexture, states);
            target.draw(m_foregroundTexture, states);
        }
        else
        {
            float size = std::min(m_knob->getSize().x, m_knob->getSize().y);

            sf::CircleShape background{size / 2.0f};
            background.setPosition(m_knob->getPosition());
            background.setFillColor(m_backgroundColor);
            background.setOutlineColor(m_borderColor);
            background.setOutlineThickness(std::min({m_borders.left, m_borders.top, m_borders.right, m_borders.bottom}));
            target.draw(background, states);

            sf::CircleShape thumb{size / 10.0f};
            thumb.setFillColor(m_thumbColor);
            thumb.setPosition({m_knob->getPosition().x + (size / 2.0f) - thumb.getRadius() + (std::cos(m_knob->m_angle / 180 * pi) * background.getRadius() * 3/5),
                               m_knob->getPosition().y + (size / 2.0f) - thumb.getRadius() + (-std::sin(m_knob->m_angle / 180 * pi) * background.getRadius() * 3/5)});
            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> KnobRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<KnobRenderer>(new KnobRenderer{*this});
        renderer->m_knob = static_cast<Knob*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
