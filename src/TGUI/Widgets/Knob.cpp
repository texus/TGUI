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


#include <TGUI/Widgets/Knob.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    const float pi = 3.14159265358979f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob()
    {
        m_type = "Knob";
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<KnobRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(140, 140);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::create()
    {
        return std::make_shared<Knob>();
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

    KnobRenderer* Knob::getSharedRenderer()
    {
        return aurora::downcast<KnobRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const KnobRenderer* Knob::getSharedRenderer() const
    {
        return aurora::downcast<const KnobRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    KnobRenderer* Knob::getRenderer()
    {
        return aurora::downcast<KnobRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const KnobRenderer* Knob::getRenderer() const
    {
        return aurora::downcast<const KnobRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
        {
            m_spriteBackground.setSize(getInnerSize());
            m_spriteForeground.setSize({m_spriteForeground.getTexture().getImageSize().x / m_spriteBackground.getTexture().getImageSize().x * getInnerSize().x,
                                        m_spriteForeground.getTexture().getImageSize().y / m_spriteBackground.getTexture().getImageSize().y * getInnerSize().y});
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

    float Knob::getStartRotation() const
    {
        return m_startRotation;
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

    float Knob::getEndRotation() const
    {
        return m_endRotation;
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

    int Knob::getMinimum() const
    {
        return m_minimum;
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

    int Knob::getMaximum() const
    {
        return m_maximum;
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

            onValueChange.emit(this, m_value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setClockwiseTurning(bool clockwise)
    {
        m_clockwiseTurning = clockwise;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::getClockwiseTurning() const
    {
        return m_clockwiseTurning;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the widget
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
            {
                // Only return true when the pixel under the mouse isn't transparent
                if (!m_spriteBackground.isTransparentPixel(pos - m_bordersCached.getOffset()))
                    return true;
            }
            else // There is no texture, the widget has a circle shape
            {
                const Vector2f centerPoint = getSize() / 2.f;
                const float distance = std::sqrt(std::pow(centerPoint.x - pos.x, 2) + std::pow(centerPoint.y - pos.y, 2));
                return (distance <= std::min(getSize().x, getSize().y));
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMousePressed(Vector2f pos)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // Change the value of the knob depending on where you clicked
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMouseReleased(Vector2f)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        const Vector2f centerPosition = getSize() / 2.0f;

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Find out the direction that the knob should now point
            if (compareFloats(pos.x, centerPosition.x))
            {
                if (pos.y > centerPosition.y)
                    m_angle = 270;
                else if (pos.y < centerPosition.y)
                    m_angle = 90;
            }
            else
            {
                m_angle = std::atan2(centerPosition.y - pos.y, pos.x - centerPosition.x) * 180.0f / pi;
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
                        setValue(static_cast<int>((((360.0f - m_angle) + m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
            else // counter-clockwise
            {
                if (m_angle >= m_startRotation)
                    setValue(static_cast<int>(((m_angle - m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    setValue(static_cast<int>(((m_angle + (360.0f - m_startRotation)) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
        }
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

    Signal& Knob::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
            setSize(m_size);
        }
        else if (property == "textureforeground")
        {
            m_spriteForeground.setTexture(getSharedRenderer()->getTextureForeground());
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "imagerotation")
        {
            m_imageRotationCached = getSharedRenderer()->getImageRotation();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteForeground.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Knob::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["ClockwiseTurning"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_clockwiseTurning));
        node->propertyValuePairs["StartRotation"] = std::make_unique<DataIO::ValueNode>(to_string(m_startRotation));
        node->propertyValuePairs["EndRotation"] = std::make_unique<DataIO::ValueNode>(to_string(m_endRotation));
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["startrotation"])
            setStartRotation(tgui::stof(node->propertyValuePairs["startrotation"]->value));
        if (node->propertyValuePairs["endrotation"])
            setEndRotation(tgui::stof(node->propertyValuePairs["endrotation"]->value));
        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["clockwiseturning"])
            setClockwiseTurning(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["clockwiseturning"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Knob::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(), getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const float size = std::min(getInnerSize().x, getInnerSize().y);

        // Draw the borders
        const float borderThickness = std::min({m_bordersCached.getLeft(), m_bordersCached.getTop(), m_bordersCached.getRight(), m_bordersCached.getBottom()});
        if (borderThickness > 0)
        {
            states.transform.translate({borderThickness, borderThickness});

            sf::CircleShape bordersShape{size / 2};
            bordersShape.setFillColor(Color::Transparent);
            bordersShape.setOutlineColor(Color::calcColorOpacity(m_borderColorCached, m_opacityCached));
            bordersShape.setOutlineThickness(borderThickness);
            target.draw(bordersShape, states);
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
        {
            sf::CircleShape background{size / 2};
            background.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));
            target.draw(background, states);
        }

        // Draw the foreground
        if (m_spriteForeground.isSet())
        {
            states.transform.translate((getInnerSize() - m_spriteForeground.getSize()) / 2.f);

            // Give the image the correct rotation
            if (m_imageRotationCached > m_angle)
                states.transform.rotate(m_imageRotationCached - m_angle, (m_spriteForeground.getSize() / 2.f));
            else
                states.transform.rotate(360 - m_angle + m_imageRotationCached, (m_spriteForeground.getSize() / 2.f));

            m_spriteForeground.draw(target, states);
        }
        else
        {
            sf::CircleShape thumb{size / 10.0f};
            thumb.setFillColor(Color::calcColorOpacity(m_thumbColorCached, m_opacityCached));
            thumb.setPosition({(size / 2.0f) - thumb.getRadius() + (std::cos(m_angle / 180 * pi) * (size / 2) * 3/5),
                               (size / 2.0f) - thumb.getRadius() + (-std::sin(m_angle / 180 * pi) * (size / 2) * 3/5)});
            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
