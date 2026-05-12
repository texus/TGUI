////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/ObjectConverter.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& ObjectConverter::getString()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");

        if (m_serialized)
            return m_string;

        m_string = Serializer::serialize(ObjectConverter{*this});
        m_serialized = true;
        return m_string;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& ObjectConverter::getFont()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::Font || m_type == Type::String,
                    "ObjectConverter must contain font or serialized object to retrieve its font");

        if (m_type == Type::String)
        {
            m_value = Font(m_string);
            m_type = Type::Font;
        }

        return std::get<Font>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ObjectConverter::getColor()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::Color || m_type == Type::String,
                    "ObjectConverter must contain color or serialized object to retrieve its color");

        if (m_type == Type::String)
        {
            m_value = Color(m_string);
            m_type = Type::Color;
        }

        return std::get<Color>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::getBool()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::Bool || m_type == Type::String,
                    "ObjectConverter must contain bool or serialized object to retrieve its bool value");

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Bool, m_string).getBool();
            m_type = Type::Bool;
        }

        return std::get<bool>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ObjectConverter::getNumber()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::Number || m_type == Type::String,
                    "ObjectConverter must contain number or serialized object to retrieve its numeric value");

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Number, m_string).getNumber();
            m_type = Type::Number;
        }

        return std::get<float>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& ObjectConverter::getOutline()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::Outline || m_type == Type::String,
                    "ObjectConverter must contain outline or serialized object to retrieve its outline");

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Outline, m_string).getOutline();
            m_type = Type::Outline;
        }

        return std::get<Outline>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& ObjectConverter::getTexture()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::Texture || m_type == Type::String,
                    "ObjectConverter must contain texture or serialized object to retrieve its texture");

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Texture, m_string).getTexture();
            m_type = Type::Texture;
        }

        return std::get<Texture>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextStyles& ObjectConverter::getTextStyle()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::TextStyle || m_type == Type::String,
                    "ObjectConverter must contain text style or serialized object to retrieve its text style");

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::TextStyle, m_string).getTextStyle();
            m_type = Type::TextStyle;
        }

        return std::get<TextStyles>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<RendererData>& ObjectConverter::getRenderer()
    {
        TGUI_ASSERT(m_type != Type::None, "Empty ObjectConverter object shouldn't be accessed");
        TGUI_ASSERT(m_type == Type::RendererData || m_type == Type::String,
                    "ObjectConverter must contain renderer data or serialized object to retrieve its renderer data");

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::RendererData, m_string).getRenderer();
            m_type = Type::RendererData;
        }

        return std::get<std::shared_ptr<RendererData>>(m_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter::Type ObjectConverter::getType() const
    {
        return m_type;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::operator==(const ObjectConverter& right) const
    {
        if (this == &right)
            return true;

        if (m_type != right.m_type)
            return false;

        switch (m_type)
        {
            case Type::None:
                return true;
            case Type::String:
                return m_string == right.m_string;
            case Type::Bool:
                return std::get<bool>(m_value) == std::get<bool>(right.m_value);
            case Type::Font:
                return std::get<Font>(m_value) == std::get<Font>(right.m_value);
            case Type::Color:
                return std::get<Color>(m_value) == std::get<Color>(right.m_value);
            case Type::Number:
                return std::get<float>(m_value) == std::get<float>(right.m_value);
            case Type::Outline:
                return std::get<Outline>(m_value) == std::get<Outline>(right.m_value);
            case Type::Texture:
                return std::get<Texture>(m_value) == std::get<Texture>(right.m_value);
            case Type::TextStyle:
                return std::get<TextStyles>(m_value) == std::get<TextStyles>(right.m_value);
            case Type::RendererData:
                return std::get<std::shared_ptr<RendererData>>(m_value) == std::get<std::shared_ptr<RendererData>>(right.m_value);
            default: // This case should never occur, but prevents a warning that control reaches end of non-void function
                return false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::operator!=(const ObjectConverter& right) const
    {
        return !(*this == right);
    }
} // namespace tgui
