/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Loading/Serializer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeEmptyObject(ObjectConverter&&)
    {
        throw Exception{"Can't serialize empty object"};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeFont(ObjectConverter&&)
    {
        return "null";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeColor(ObjectConverter&& value)
    {
        sf::Color color = value.getColor();
        if (color.a < 255)
            return "rgba(" + tgui::to_string((int)color.r) + ", " + tgui::to_string((int)color.g) + ", " + tgui::to_string((int)color.b) + ", " + tgui::to_string((int)color.a) + ")";
        else
            return "rgb(" + tgui::to_string((int)color.r) + ", " + tgui::to_string((int)color.g) + ", " + tgui::to_string((int)color.b) + ")";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeString(ObjectConverter&& value)
    {
        std::string result = value.getString();

        auto replace = [&](char from, char to)
            {
                std::size_t pos = 0;
                while ((pos = result.find(from, pos)) != std::string::npos)
                {
                    result[pos] = to;
                    result.insert(pos, 1, '\\');
                    pos += 2;
                }
            };

        replace('\\', '\\');
        replace('\"', '\"');
        replace('\v', 'v');
        replace('\t', 't');
        replace('\n', 'n');
        replace('\0', '0');

        return "\"" + result + "\"";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeNumber(ObjectConverter&& value)
    {
        return tgui::to_string(value.getNumber());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeBorders(ObjectConverter&& value)
    {
        Borders borders = value.getBorders();
        return "(" + tgui::to_string(static_cast<unsigned int>(borders.left)) + ", " + tgui::to_string(static_cast<unsigned int>(borders.top))
             + ", " + tgui::to_string(static_cast<unsigned int>(borders.right)) + ", " + tgui::to_string(static_cast<unsigned int>(borders.bottom)) + ")";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::string serializeTexture(ObjectConverter&& value)
    {
        Texture texture = value.getTexture();
        std::string result = "\"" + texture.getId() + "\"";

        if (texture.getData()->rect != sf::IntRect{})
        {
            result += " Part(" + tgui::to_string(texture.getData()->rect.left) + ", " + tgui::to_string(texture.getData()->rect.top)
                        + ", " + tgui::to_string(texture.getData()->rect.width) + ", " + tgui::to_string(texture.getData()->rect.height) + ")";
        }
        if (texture.getMiddleRect() != sf::IntRect{0, 0, static_cast<int>(texture.getData()->texture.getSize().x), static_cast<int>(texture.getData()->texture.getSize().y)})
        {
            result += " Middle(" + tgui::to_string(texture.getMiddleRect().left) + ", " + tgui::to_string(texture.getMiddleRect().top)
                          + ", " + tgui::to_string(texture.getMiddleRect().width) + ", " + tgui::to_string(texture.getMiddleRect().height) + ")";
        }
        if (texture.getData()->texture.isRepeated())
            result += " Repeat";

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ObjectConverter::Type, Serializer::SerializeFunc> Serializer::m_serializers =
        {
            {ObjectConverter::Type::None, serializeEmptyObject},
            {ObjectConverter::Type::Font, serializeFont},
            {ObjectConverter::Type::Color, serializeColor},
            {ObjectConverter::Type::String, serializeString},
            {ObjectConverter::Type::Number, serializeNumber},
            {ObjectConverter::Type::Borders, serializeBorders},
            {ObjectConverter::Type::Texture, serializeTexture}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Serializer::serialize(ObjectConverter&& object)
    {
        return m_serializers[object.getType()](std::move(object));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Serializer::setFunction(ObjectConverter::Type type, const SerializeFunc& serializer)
    {
        m_serializers[type] = serializer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Serializer::SerializeFunc& Serializer::getFunction(ObjectConverter::Type type)
    {
        return m_serializers[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
