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


#include <TGUI/Widgets/Knob.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob()
    {
        m_type = "Knob";
        m_callback.widgetType = "Knob";
        m_draggableWidget = true;

        addSignal<int>("ValueChanged");

        m_renderer = aurora::makeCopied<KnobRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({5});
        getRenderer()->setImageRotation(0);

        setSize(140, 140);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::copy(Knob::ConstPtr knob)
    {
        if (knob)
            return std::static_pointer_cast<Knob>(knob->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getRenderer()->getTextureBackground().isLoaded() && getRenderer()->getTextureForeground().isLoaded())
        {
            getRenderer()->getTextureBackground().setSize(getInnerSize());
            getRenderer()->getTextureForeground().setSize({getRenderer()->getTextureForeground().getImageSize().x / getRenderer()->getTextureBackground().getImageSize().x * getInnerSize().x,
                                                           getRenderer()->getTextureForeground().getImageSize().y / getRenderer()->getTextureBackground().getImageSize().y * getInnerSize().y});
        }
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

    bool Knob::mouseOnWidget(float x, float y) const
    {
        // Check if the mouse is on top of the widget
        if (sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(x, y))
        {
            if (getRenderer()->getTextureBackground().isLoaded() && getRenderer()->getTextureForeground().isLoaded())
            {
                // Only return true when the pixel under the mouse isn't transparent
                if (!getRenderer()->getTextureBackground().isTransparentPixel(x, y))
                    return true;
            }
            else // There is no texture, the widget has a circle shape
            {
                sf::Vector2f centerPoint = getSize() / 2.f;
                sf::Vector2f mousePoint = {x, y};
                float distance = std::sqrt(std::pow(centerPoint.x - mousePoint.x, 2) + std::pow(centerPoint.y - mousePoint.y, 2));
                return (distance <= std::min(getSize().x, getSize().y));
            }
        }

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

        sf::Vector2f centerPosition = getSize() / 2.0f;

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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "borders")
        {
            updateSize();
        }
        else if ((property == "texturebackground") || (property == "textureforeground"))
        {
            value.getTexture().setOpacity(getRenderer()->getOpacity());
            updateSize();
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();
            getRenderer()->getTextureBackground().setOpacity(opacity);
            getRenderer()->getTextureForeground().setOpacity(opacity);
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolor")
              && (property != "thumbcolor")
              && (property != "imagerotation"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Knob::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        float size = std::min(getInnerSize().x, getInnerSize().y);

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        float borderThickness = std::min({borders.left, borders.top, borders.right, borders.bottom});
        if (borderThickness > 0)
        {
            states.transform.translate({borderThickness, borderThickness});

            sf::CircleShape bordersShape{size / 2};
            bordersShape.setFillColor(sf::Color::Transparent);
            bordersShape.setOutlineColor(calcColorOpacity(getRenderer()->getBorderColor(), getRenderer()->getOpacity()));
            bordersShape.setOutlineThickness(borderThickness);
            target.draw(bordersShape, states);
        }

        // Draw the background
        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().draw(target, states);
        else
        {
            sf::CircleShape background{size / 2};
            background.setFillColor(calcColorOpacity(getRenderer()->getBackgroundColor(), getRenderer()->getOpacity()));
            target.draw(background, states);
        }

        // Draw the foreground
        if (getRenderer()->getTextureForeground().isLoaded())
        {
            states.transform.translate((getInnerSize() - getRenderer()->getTextureForeground().getSize()) / 2.f);

            // Give the image the correct rotation
            if (getRenderer()->getImageRotation() > m_angle)
                states.transform.rotate(getRenderer()->getImageRotation() - m_angle, (getRenderer()->getTextureForeground().getSize() / 2.f));
            else
                states.transform.rotate(360 - m_angle + getRenderer()->getImageRotation(), (getRenderer()->getTextureForeground().getSize() / 2.f));

            getRenderer()->getTextureForeground().draw(target, states);
        }
        else
        {
            sf::CircleShape thumb{size / 10.0f};
            thumb.setFillColor(calcColorOpacity(getRenderer()->getThumbColor(), getRenderer()->getOpacity()));
            thumb.setPosition({(size / 2.0f) - thumb.getRadius() + (std::cos(m_angle / 180 * pi) * (size / 2) * 3/5),
                               (size / 2.0f) - thumb.getRadius() + (-std::sin(m_angle / 180 * pi) * (size / 2) * 3/5)});
            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
