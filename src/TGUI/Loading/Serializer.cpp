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


#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/to_string.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        char decToSingleHex(unsigned char c)
        {
            assert(c < 16);

            if (c == 10)
                return 'A';
            else if (c == 11)
                return 'B';
            else if (c == 12)
                return 'C';
            else if (c == 13)
                return 'D';
            else if (c == 14)
                return 'E';
            else if (c == 15)
                return 'F';
            else
                return static_cast<char>(c + '0');
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string decToHex(unsigned char c)
        {
            return {decToSingleHex(c / 16), decToSingleHex(c % 16)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeEmptyObject(ObjectConverter&&)
        {
            throw Exception{"Can't serialize empty object"};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeBool(ObjectConverter&& value)
        {
            if (value.getBool())
                return "true";
            else
                return "false";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeFont(ObjectConverter&& value)
        {
            if (value.getFont() && !value.getFont().getId().empty())
                return Serializer::serialize({sf::String{value.getFont().getId()}});
            else
                return "null";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeColor(ObjectConverter&& value)
        {
            Color color = value.getColor();

            // Check if the color can be represented by a string with its name
            for (const auto& pair : Color::colorMap)
            {
                if (color == pair.second)
                    return pair.first;
            }

            // Return the color by its rgb value
            return "#" + decToHex(color.getRed()) + decToHex(color.getGreen()) + decToHex(color.getBlue()) + (color.getAlpha() < 255 ? decToHex(color.getAlpha()) : "");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeString(ObjectConverter&& value)
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeNumber(ObjectConverter&& value)
        {
            return to_string(value.getNumber());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeOutline(ObjectConverter&& value)
        {
            return value.getOutline().toString();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeTexture(ObjectConverter&& value)
        {
            Texture texture = value.getTexture();
            if (texture.getId().isEmpty())
                return "None";

            std::string result = "\"" + texture.getId() + "\"";

            if (texture.getData()->rect != sf::IntRect{})
            {
                result += " Part(" + to_string(texture.getData()->rect.left) + ", " + to_string(texture.getData()->rect.top)
                            + ", " + to_string(texture.getData()->rect.width) + ", " + to_string(texture.getData()->rect.height) + ")";
            }

            if (texture.getMiddleRect() != sf::IntRect{0, 0, static_cast<int>(texture.getData()->texture.getSize().x), static_cast<int>(texture.getData()->texture.getSize().y)})
            {
                result += " Middle(" + to_string(texture.getMiddleRect().left) + ", " + to_string(texture.getMiddleRect().top)
                              + ", " + to_string(texture.getMiddleRect().width) + ", " + to_string(texture.getMiddleRect().height) + ")";
            }

            if (texture.isSmooth())
                result += " Smooth";

            return result;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeTextStyle(ObjectConverter&& value)
        {
            const unsigned int style = value.getTextStyle();

            if (style == sf::Text::Regular)
                return "Regular";

            std::string encodedStyle;
            if (style & sf::Text::Bold)
                encodedStyle += " | Bold";
            if (style & sf::Text::Italic)
                encodedStyle += " | Italic";
            if (style & sf::Text::Underlined)
                encodedStyle += " | Underlined";
            if (style & sf::Text::StrikeThrough)
                encodedStyle += " | StrikeThrough";

            if (!encodedStyle.empty())
                return encodedStyle.substr(3);
            else // Something is wrong with the style parameter
                return "Regular";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeRendererData(ObjectConverter&& value)
        {
            auto node = std::make_unique<DataIO::Node>();
            for (const auto& pair : value.getRenderer()->propertyValuePairs)
            {
                sf::String strValue;
                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{ObjectConverter{pair.second}.getString()};
                    node->children.push_back(DataIO::parse(ss));
                    node->children.back()->name = pair.first;
                }
                else
                {
                    strValue = ObjectConverter{pair.second}.getString();
                    node->propertyValuePairs[pair.first] = std::make_unique<DataIO::ValueNode>(strValue);
                }
            }

            std::stringstream ss;
            DataIO::emit(node, ss);
            return ss.str();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ObjectConverter::Type, Serializer::SerializeFunc> Serializer::m_serializers =
        {
            {ObjectConverter::Type::None, serializeEmptyObject},
            {ObjectConverter::Type::Bool, serializeBool},
            {ObjectConverter::Type::Font, serializeFont},
            {ObjectConverter::Type::Color, serializeColor},
            {ObjectConverter::Type::String, serializeString},
            {ObjectConverter::Type::Number, serializeNumber},
            {ObjectConverter::Type::Outline, serializeOutline},
            {ObjectConverter::Type::Texture, serializeTexture},
            {ObjectConverter::Type::TextStyle, serializeTextStyle},
            {ObjectConverter::Type::RendererData, serializeRendererData}
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
