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


#include <TGUI/Widgets/Knob.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>

    #if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
        #include <numbers>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    bool compareFloats(float x, float y)
    {
        return (std::abs(x - y) < 0.0000001f);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Knob::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<KnobRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setSize(140, 140);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::create()
    {
        return std::make_shared<Knob>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::copy(const Knob::ConstPtr& knob)
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

    void Knob::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (m_spriteBackground.isSet())
            m_spriteBackground.setSize(getSize());

        if (m_spriteForeground.isSet())
        {
            m_spriteForeground.setSize({(m_spriteForeground.getTexture().getImageSize().x / static_cast<float>(m_spriteBackground.getTexture().getImageSize().x)) * getSize().x,
                                        (m_spriteForeground.getTexture().getImageSize().y / static_cast<float>(m_spriteBackground.getTexture().getImageSize().y)) * getSize().y});
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

    void Knob::setMinimum(float minimum)
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

    float Knob::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMaximum(float maximum)
    {
        if (m_maximum != maximum)
        {
            // Set the new maximum
            m_maximum = maximum;

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

    float Knob::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setValue(float value)
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

    float Knob::getValue() const
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

    bool Knob::isMouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the widget
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
            {
                // Only return true when the pixel under the mouse isn't transparent
                if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos)
                 || !m_spriteForeground.isTransparentPixel(pos - ((getInnerSize() - m_spriteForeground.getSize()) / 2.f)))
                    return true;
            }
            else // There is no texture, the widget has a circle shape
            {
                const Vector2f centerPoint = getSize() / 2.f;
                const float distance = std::sqrt(std::pow(centerPoint.x - pos.x, 2.f) + std::pow(centerPoint.y - pos.y, 2.f));
                return (distance <= std::min(getSize().x, getSize().y));
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        // Change the value of the knob depending on where you clicked
        mouseMoved(pos);
        return true;
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
#if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
                const float pi = std::numbers::pi_v<float>;
#else
                const float pi = 3.14159265359f;
#endif
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
                    setValue(((m_startRotation - m_angle) / allowedAngle * (m_maximum - m_minimum)) + m_minimum);
                else
                {
                    if (compareFloats(m_angle, m_startRotation))
                        setValue(m_minimum);
                    else
                        setValue((((360.0f - m_angle) + m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum);
                }
            }
            else // counter-clockwise
            {
                if (m_angle >= m_startRotation)
                    setValue(((m_angle - m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum);
                else
                {
                    setValue(((m_angle + (360.0f - m_startRotation)) / allowedAngle * (m_maximum - m_minimum)) + m_minimum);
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
        if (m_maximum == m_minimum)
            m_angle = m_startRotation;
        else if (m_clockwiseTurning)
            m_angle = m_startRotation - (m_value - m_minimum) / (m_maximum - m_minimum) * allowedAngle;
        else // counter-clockwise
            m_angle = (((m_value - m_minimum) / (m_maximum - m_minimum)) * allowedAngle) + m_startRotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Knob::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
            setSize(m_size);
        }
        else if (property == U"TextureForeground")
        {
            m_spriteForeground.setTexture(getSharedRenderer()->getTextureForeground());
            setSize(m_size);
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"ThumbColor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == U"ImageRotation")
        {
            m_imageRotationCached = getSharedRenderer()->getImageRotation();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
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

        node->propertyValuePairs[U"ClockwiseTurning"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_clockwiseTurning));
        node->propertyValuePairs[U"StartRotation"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_startRotation));
        node->propertyValuePairs[U"EndRotation"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_endRotation));
        node->propertyValuePairs[U"Minimum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimum));
        node->propertyValuePairs[U"Maximum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximum));
        node->propertyValuePairs[U"Value"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_value));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"StartRotation"])
            setStartRotation(node->propertyValuePairs[U"StartRotation"]->value.toFloat());
        if (node->propertyValuePairs[U"EndRotation"])
            setEndRotation(node->propertyValuePairs[U"EndRotation"]->value.toFloat());
        if (node->propertyValuePairs[U"Minimum"])
            setMinimum(node->propertyValuePairs[U"Minimum"]->value.toFloat());
        if (node->propertyValuePairs[U"Maximum"])
            setMaximum(node->propertyValuePairs[U"Maximum"]->value.toFloat());
        if (node->propertyValuePairs[U"Value"])
            setValue(node->propertyValuePairs[U"Value"]->value.toFloat());
        if (node->propertyValuePairs[U"ClockwiseTurning"])
            setClockwiseTurning(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"ClockwiseTurning"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Knob::getInnerSize() const
    {
        if (m_spriteBackground.isSet())
            return getSize();
        else
            return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                    std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const float innerSize = std::min(getInnerSize().x, getInnerSize().y);

        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
        {
            const float borderThickness = std::min({m_bordersCached.getLeft(), m_bordersCached.getTop(), m_bordersCached.getRight(), m_bordersCached.getBottom()});
            if (borderThickness > 0)
            {
                states.transform.translate({borderThickness, borderThickness});
                target.drawCircle(states, innerSize, Color::applyOpacity(m_backgroundColorCached, m_opacityCached), borderThickness, Color::applyOpacity(m_borderColorCached, m_opacityCached));
            }
            else
                target.drawCircle(states, innerSize, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
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

            target.drawSprite(states, m_spriteForeground);
        }
        else
        {
#if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
            const float pi = std::numbers::pi_v<float>;
#else
            const float pi = 3.14159265359f;
#endif
            const float radius = innerSize / 10.0f;
            states.transform.translate({(innerSize / 2.0f) - radius + (std::cos(m_angle / 180 * pi) * (innerSize / 2.f) * 3.f/5.f),
                                        (innerSize / 2.0f) - radius + (-std::sin(m_angle / 180 * pi) * (innerSize / 2.f) * 3.f/5.f)});
            target.drawCircle(states, radius * 2.0f, Color::applyOpacity(m_thumbColorCached, m_opacityCached));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Knob::clone() const
    {
        return std::make_shared<Knob>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
