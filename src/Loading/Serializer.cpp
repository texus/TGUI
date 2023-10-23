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


#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/Base64.hpp>
#include <cassert>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/extlibs/IncludeStbImageWrite.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        char decToSingleHex(unsigned char c)
        {
            TGUI_ASSERT(c < 16, "Value to decode to hex can't be larger than 16");

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

        String decToHex(unsigned char c)
        {
            return String({decToSingleHex(c / 16), decToSingleHex(c % 16)});
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeEmptyObject(ObjectConverter&&)
        {
            throw Exception{U"Can't serialize empty object"};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeBool(ObjectConverter&& value)
        {
            if (value.getBool())
                return "true";
            else
                return "false";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeFont(ObjectConverter&& value)
        {
            if (value.getFont() && !value.getFont().getId().empty())
                return Serializer::serialize({value.getFont().getId()});
            else
                return "null";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeColor(ObjectConverter&& value)
        {
            Color color = value.getColor();

            if (!color.isSet())
                return "None";

            // Check if the color can be represented by a string with its name
            for (const auto& pair : Color::colorNamesMap)
            {
                if (color == pair.second)
                    return String{pair.first};
            }

            // Return the color by its rgb value
            return "#" + decToHex(color.getRed()) + decToHex(color.getGreen()) + decToHex(color.getBlue()) + (color.getAlpha() < 255 ? decToHex(color.getAlpha()) : "");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeString(ObjectConverter&& value)
        {
            String result = value.getString();

            bool encodingRequired = false;
            if (result.empty())
                encodingRequired = true;
            else
            {
                for (const char32_t c : result)
                {
                    // Slashes have to be serialized because the DataIO parser doesn't like values starting with a slash
                    if ((c != U'%') && (c != U'_') && (c != U'@') && ((c < U'0') || (c > U'9')) && ((c < U'A') || (c > U'Z')) && ((c < U'a') || (c > U'z')))
                    {
                        encodingRequired = true;
                        break;
                    }
                }
            }

            if (!encodingRequired)
                return result;

            auto replace = [&](char32_t from, char32_t to)
                {
                    std::size_t pos = 0;
                    while ((pos = result.find(from, pos)) != String::npos)
                    {
                        result[pos] = to;
                        result.insert(pos, 1, U'\\');
                        pos += 2;
                    }
                };

            replace(U'\\', U'\\');
            replace(U'\"', U'\"');
            replace(U'\v', U'v');
            replace(U'\t', U't');
            replace(U'\n', U'n');
            replace(U'\0', U'0');

            return "\"" + result + "\"";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeNumber(ObjectConverter&& value)
        {
            return String::fromNumber(value.getNumber());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeOutline(ObjectConverter&& value)
        {
            return value.getOutline().toString();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeTexture(ObjectConverter&& value)
        {
            Texture texture = value.getTexture();
            if (!texture.getData())
                return "None";

            String result;
            if (!texture.getId().empty())
                result = "\"" + texture.getId() + "\"";
            else
            {
                if (!texture.getData()->backendTexture || !texture.getData()->backendTexture->getPixels())
                    return "None";

                // We don't have a filename for the image, so save it as base64
                const Vector2u imageSize = texture.getData()->backendTexture->getSize();
                const std::uint8_t* pixels = texture.getData()->backendTexture->getPixels();

                int dataLength = 0;
                unsigned char* pngData = stbi_write_png_to_mem(static_cast<const unsigned char*>(pixels),
                    static_cast<int>(imageSize.x * 4), static_cast<int>(imageSize.x), static_cast<int>(imageSize.y), 4, &dataLength);
                if (!pngData)
                    return "None";

                assert(dataLength >= 0); // Length is always positive, but returned as a signed int
                result = "\"data:image/png;base64," + base64Encode(pngData, static_cast<std::size_t>(dataLength)) + "\"";
                STBIW_FREE(pngData); // NOLINT(cppcoreguidelines-no-malloc)
            }

            if (texture.getData()->backendTexture)
            {
                const UIntRect& partRect = texture.getPartRect();
                if ((partRect != UIntRect{}) && (partRect != UIntRect{{0, 0}, texture.getData()->backendTexture->getSize()}))
                {
                    result += " Part(" + String::fromNumber(partRect.left) + ", " + String::fromNumber(partRect.top)
                                + ", " + String::fromNumber(partRect.width) + ", " + String::fromNumber(partRect.height) + ")";
                }

                const UIntRect& middleRect = texture.getMiddleRect();
                if (middleRect != UIntRect{{0, 0}, partRect.getSize()})
                {
                    result += " Middle(" + String::fromNumber(middleRect.left) + ", " + String::fromNumber(middleRect.top)
                                  + ", " + String::fromNumber(middleRect.width) + ", " + String::fromNumber(middleRect.height) + ")";
                }
            }

            if (texture.isSmooth())
                result += " Smooth";
            else
                result += " NoSmooth";

            return result;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeTextStyle(ObjectConverter&& value)
        {
            const unsigned int style = value.getTextStyle();

            if (style == TextStyle::Regular)
                return "Regular";

            String encodedStyle;
            if (style & TextStyle::Bold)
                encodedStyle += " | Bold";
            if (style & TextStyle::Italic)
                encodedStyle += " | Italic";
            if (style & TextStyle::Underlined)
                encodedStyle += " | Underlined";
            if (style & TextStyle::StrikeThrough)
                encodedStyle += " | StrikeThrough";

            if (!encodedStyle.empty())
                return encodedStyle.substr(3);
            else // Something is wrong with the style parameter
                return "Regular";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String serializeRendererData(ObjectConverter&& value)
        {
            auto node = std::make_unique<DataIO::Node>();
            for (const auto& pair : value.getRenderer()->propertyValuePairs)
            {
                String strValue;
                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{ObjectConverter{pair.second}.getString().toStdString()};
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

    String Serializer::serialize(ObjectConverter&& object)
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
