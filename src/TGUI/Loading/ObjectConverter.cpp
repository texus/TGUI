/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Loading/ObjectConverter.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ObjectConverter::getString()
    {
        assert(m_type != Type::None);

        if (m_serialized || (m_type == Type::String))
            return m_string;

        if (m_type == Type::Font)
            m_string = Serializer::serialize(m_font);
        else if (m_type == Type::Color)
            m_string = Serializer::serialize(m_color);
        else if (m_type == Type::Number)
            m_string = Serializer::serialize(m_number);
        else if (m_type == Type::Outline)
            m_string = Serializer::serialize(m_outline);
        else if (m_type == Type::Texture)
            m_string = Serializer::serialize(m_texture);

        m_serialized = true;
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<sf::Font>& ObjectConverter::getFont()
    {
        assert(m_type == Type::Font || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_font = Deserializer::deserialize(ObjectConverter::Type::Font, m_string).getFont();
            m_type = Type::Font;
        }

        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ObjectConverter::getColor()
    {
        assert(m_type == Type::Color || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_color = Deserializer::deserialize(ObjectConverter::Type::Color, m_string).getColor();
            m_type = Type::Color;
        }

        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ObjectConverter::getNumber()
    {
        assert(m_type == Type::Number || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_number = Deserializer::deserialize(ObjectConverter::Type::Number, m_string).getNumber();
            m_type = Type::Number;
        }

        return m_number;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& ObjectConverter::getOutline()
    {
        assert(m_type == Type::Outline || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_outline = Deserializer::deserialize(ObjectConverter::Type::Outline, m_string).getOutline();
            m_type = Type::Outline;
        }

        return m_outline;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& ObjectConverter::getTexture()
    {
        assert(m_type == Type::Texture || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_texture = Deserializer::deserialize(ObjectConverter::Type::Texture, m_string).getTexture();
            m_type = Type::Texture;
        }

        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter::Type ObjectConverter::getType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
