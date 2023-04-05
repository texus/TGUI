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


#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Backend/Font/BackendFont.hpp>
#include <TGUI/Global.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
#endif

namespace tgui
{
    namespace
    {
        unsigned char hexToDec(char32_t c)
        {
            TGUI_ASSERT((c >= U'0' && c <= U'9') || (c >= U'a' && c <= U'f'), "hexToDec must be called with digit or lowercase hex character");

            if (c == U'a')
                return 10;
            else if (c == U'b')
                return 11;
            else if (c == U'c')
                return 12;
            else if (c == U'd')
                return 13;
            else if (c == U'e')
                return 14;
            else if (c == U'f')
                return 15;
            else // if (c >= U'0' && c <= U'9')
                return static_cast<unsigned char>(c - U'0');
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeWhitespace(const String& line, String::const_iterator& c)
        {
            while (c != line.end())
            {
                if ((*c == ' ') || (*c == '\t') || (*c == '\r'))
                    ++c;
                else
                    return true;
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool readUIntRect(String value, UIntRect& rect, bool rectRequiresFourValues)
        {
            if (!value.empty() && (value[0] == '(') && (value[value.length()-1] == ')'))
            {
                std::vector<String> tokens = Deserializer::split(value.substr(1, value.size()-2), ',');
                if (tokens.size() == 4)
                {
                    rect = {tokens[0].toUInt(), tokens[1].toUInt(), tokens[2].toUInt(), tokens[3].toUInt()};
                    return true;
                }
                else if (!rectRequiresFourValues && (tokens.size() == 2))
                {
                    rect = {tokens[0].toUInt(), tokens[1].toUInt(), 0, 0};
                    return true;
                }
                else if (!rectRequiresFourValues && (tokens.size() == 1))
                {
                    rect = {tokens[0].toUInt(), tokens[0].toUInt(), 0, 0};
                    return true;
                }
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeBool(const String& str)
        {
            if (viewEqualIgnoreCase(str, U"true") || viewEqualIgnoreCase(str, U"yes") || viewEqualIgnoreCase(str, U"on") || viewEqualIgnoreCase(str, U"1"))
                return {true};
            else if (viewEqualIgnoreCase(str, U"false") || viewEqualIgnoreCase(str, U"no") || viewEqualIgnoreCase(str, U"off") || viewEqualIgnoreCase(str, U"0"))
                return {false};
            else
                throw Exception{U"Failed to deserialize boolean from '" + str + U"'"};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeFont(const String& value)
        {
            if (viewEqualIgnoreCase(value, U"null") || viewEqualIgnoreCase(value, U"nullptr"))
                return Font{};

            String filename = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();
            if (filename.empty())
                return Font{};

            // Load the font but insert the resource path into the filename unless the filename is an absolute path
            auto font = getBackend()->createFont();
#ifdef TGUI_SYSTEM_WINDOWS
            if ((filename[0] != '/') && (filename[0] != '\\') && ((filename.length() <= 1) || (filename[1] != ':')))
#else
            if (filename[0] != '/')
#endif
            {
                if (!font->loadFromFile((getResourcePath() / filename).asString()))
                    throw Exception{U"Failed to load font from '" + (getResourcePath() / filename).asString() + U"'"};
            }
            else
            {
                if (!font->loadFromFile(filename))
                    throw Exception{U"Failed to load font from '" + filename + U"'"};
            }

            // We create the backend font manually first, as passing the string to the Font constructor would cause
            // an endless recursive call to this function.
            return Font{font, filename};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeColor(const String& value)
        {
            String str = value.trim().toLower();
            if (str.empty() || (str == U"none"))
                return Color();

            // Check if the color can be represented with a hexadecimal number
            if (str[0] == '#')
            {
                if ((str.length() != 4) && (str.length() != 5) && (str.length() != 7) && (str.length() != 9))
                    throw Exception{U"Failed to deserialize color '" + value + U"'. Value started but '#' but has the wrong length."};

                // You can only have hex characters
                for (std::size_t i = 1; i < str.length(); ++i)
                {
                    if (!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f')))
                        throw Exception{U"Failed to deserialize color '" + str + U"'. Value started but '#' but contained an invalid character afterwards."};
                }

                // Parse the different types of strings (#123, #1234, #112233 and #11223344)
                if (str.length() == 4)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(str[1]) * 16 + hexToDec(str[1])),
                                 static_cast<std::uint8_t>(hexToDec(str[2]) * 16 + hexToDec(str[2])),
                                 static_cast<std::uint8_t>(hexToDec(str[3]) * 16 + hexToDec(str[3]))};
                }
                else if (str.length() == 5)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(str[1]) * 16 + hexToDec(str[1])),
                                 static_cast<std::uint8_t>(hexToDec(str[2]) * 16 + hexToDec(str[2])),
                                 static_cast<std::uint8_t>(hexToDec(str[3]) * 16 + hexToDec(str[3])),
                                 static_cast<std::uint8_t>(hexToDec(str[4]) * 16 + hexToDec(str[4]))};
                }
                else if (str.length() == 7)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(str[1]) * 16 + hexToDec(str[2])),
                                 static_cast<std::uint8_t>(hexToDec(str[3]) * 16 + hexToDec(str[4])),
                                 static_cast<std::uint8_t>(hexToDec(str[5]) * 16 + hexToDec(str[6]))};
                }
                else // if (str.length() == 9)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(str[1]) * 16 + hexToDec(str[2])),
                                 static_cast<std::uint8_t>(hexToDec(str[3]) * 16 + hexToDec(str[4])),
                                 static_cast<std::uint8_t>(hexToDec(str[5]) * 16 + hexToDec(str[6])),
                                 static_cast<std::uint8_t>(hexToDec(str[7]) * 16 + hexToDec(str[8]))};
                }
            }
            else // Color doesn't start with '#'
            {
                // Check if the color is represented by a string with its name
                for (const auto& pair : Color::colorNamesMap)
                {
                    if (str == pair.first)
                        return pair.second;
                }
            }

            // The string can optionally start with "rgb" or "rgba", but this is ignored
            if (str.substr(0, 4) == U"rgba")
                str.erase(0, 4);
            else if (str.substr(0, 3) == U"rgb")
                str.erase(0, 3);

            // Remove the first and last characters when they are brackets
            if ((str[0] == '(') && (str[str.length()-1] == ')'))
                str = str.substr(1, str.length()-2);

            const std::vector<String> tokens = Deserializer::split(str, ',');
            if (tokens.size() == 3 || tokens.size() == 4)
            {
                return Color{static_cast<std::uint8_t>(tokens[0].toInt()),
                             static_cast<std::uint8_t>(tokens[1].toInt()),
                             static_cast<std::uint8_t>(tokens[2].toInt()),
                             static_cast<std::uint8_t>((tokens.size() == 4) ? tokens[3].toInt() : 255)};
            }

            throw Exception{U"Failed to deserialize color '" + value + U"'."};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeString(const String& value)
        {
            // Only deserialize the string when it is surrounded with quotes
            if ((value.size() >= 2) && ((value[0] == '"') && (value[value.length()-1] == '"')))
            {
                String result = value.substr(1, value.length()-2);

                std::size_t backslashPos = 0;
                while ((backslashPos = result.find('\\', backslashPos)) < result.size()-1)
                {
                    result.erase(backslashPos, 1);

                    if (result[backslashPos] == 'n')
                        result[backslashPos] = '\n';
                    else if (result[backslashPos] == 't')
                        result[backslashPos] = '\t';
                    else if (result[backslashPos] == 'v')
                        result[backslashPos] = '\v';

                    backslashPos++;
                }

                return {result};
            }
            else
                return {value};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeNumber(const String& value)
        {
            return {value.toFloat()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeOutline(const String& value)
        {
            String str = value.trim();

            if (str.empty())
                throw Exception{U"Failed to deserialize outline '" + value + U"'. String was empty."};

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
                return {Outline{0}};

            const std::vector<String> tokens = Deserializer::split(str, ',');
            if (tokens.size() == 1)
                return {Outline{tokens[0]}};
            else if (tokens.size() == 2)
                return {Outline{tokens[0], tokens[1]}};
            else if (tokens.size() == 4)
                return {Outline{tokens[0], tokens[1], tokens[2], tokens[3]}};
            else
                throw Exception{U"Failed to deserialize outline '" + value + U"'. Expected numbers separated with a comma."};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeTexture(const String& value)
        {
            if (value.empty() || viewEqualIgnoreCase(value, U"none"))
                return Texture{};

            String filename;
            UIntRect partRect;
            UIntRect middleRect;
            bool smooth = Texture::getDefaultSmooth();

            // If there are no quotes then the value just contains a filename
            if (value[0] != '"')
                filename = value;
            else
            {
                String::const_iterator c = value.begin();
                ++c; // Skip the opening quote

                // Look for the end quote
                char32_t prev = U'\0';
                bool filenameFound = false;
                while (c != value.end())
                {
                    if ((*c != U'"') || (prev == U'\\'))
                    {
                        prev = *c;
                        filename.push_back(*c);
                        ++c;
                    }
                    else
                    {
                        ++c;
                        filenameFound = true;
                        break;
                    }
                }

                if (!filenameFound)
                    throw Exception{U"Failed to deserialize texture '" + value + U"'. Failed to find the closing quote of the filename."};

                // There may be optional parameters
                while (removeWhitespace(value, c))
                {
                    const auto startOffset = static_cast<std::size_t>(c - value.begin());

                    String word;
                    auto openingBracketPos = value.find(U'(', startOffset);
                    if (openingBracketPos != String::npos)
                        word = value.substr(startOffset, openingBracketPos - startOffset);
                    else
                    {
                        const String& smoothParam = value.substr(startOffset).trim();
                        if (viewEqualIgnoreCase(smoothParam, U"smooth"))
                        {
                            smooth = true;
                            break;
                        }
                        else if (viewEqualIgnoreCase(smoothParam, U"nosmooth"))
                        {
                            smooth = false;
                            break;
                        }
                        else
                            throw Exception{U"Failed to deserialize texture '" + value + U"'. Invalid text found behind filename."};
                    }

                    if (word.empty())
                        throw Exception{U"Failed to deserialize texture '" + value + U"'. Expected 'Part' or 'Middle' in front of opening bracket."};

                    bool rectRequiresFourValues = true;
                    UIntRect* rect = nullptr;
                    if (viewEqualIgnoreCase(word, U"part"))
                    {
                        rect = &partRect;
                        std::advance(c, 4);
                    }
                    else if (viewEqualIgnoreCase(word, U"middle"))
                    {
                        rectRequiresFourValues = false;
                        rect = &middleRect;
                        std::advance(c, 6);
                    }
                    else
                        throw Exception{U"Failed to deserialize texture '" + value + U"'. Unexpected word '" + word + U"' in front of opening bracket. Expected 'Part' or 'Middle'."};

                    const auto endOffset = static_cast<std::size_t>(c - value.begin());
                    auto closeBracketPos = value.find(U')', endOffset);
                    if (closeBracketPos != String::npos)
                    {
                        if (!readUIntRect(value.substr(endOffset, closeBracketPos - endOffset + 1), *rect, rectRequiresFourValues))
                            throw Exception{U"Failed to parse " + word + U" rectangle while deserializing texture '" + value + U"'."};
                    }
                    else
                        throw Exception{U"Failed to deserialize texture '" + value + U"'. Failed to find closing bracket for " + word + U" rectangle."};

                    std::advance(c, static_cast<std::ptrdiff_t>(closeBracketPos - endOffset + 1));
                }
            }

            // Check if the texture is provided as a base64-encoded string
            if (filename.starts_with(U"data:"))
            {
                const auto foundIndex = filename.find(U";base64,");
                if (foundIndex == String::npos)
                    throw Exception{U"Failed to deserialize texture '" + value + U"'. Filename started with 'data:' but wasn't in format 'data:image/TYPE;base64,DATA'."};

                const auto dataIndex = foundIndex + 8;
                const std::string& encodedData = filename.toStdString();

                Texture texture;
                texture.loadFromBase64(CharStringView(encodedData.data() + dataIndex, encodedData.length() - dataIndex), partRect, middleRect, smooth);
                return texture;
            }

            return Texture{filename, partRect, middleRect, smooth};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeTextStyle(const String& style)
        {
            unsigned int decodedStyle = TextStyle::Regular;
            std::vector<String> styles = Deserializer::split(style, '|');
            for (const auto& elem : styles)
            {
                if (viewEqualIgnoreCase(elem, U"bold"))
                    decodedStyle |= TextStyle::Bold;
                else if (viewEqualIgnoreCase(elem, U"italic"))
                    decodedStyle |= TextStyle::Italic;
                else if (viewEqualIgnoreCase(elem, U"underlined"))
                    decodedStyle |= TextStyle::Underlined;
                else if (viewEqualIgnoreCase(elem, U"strikethrough"))
                    decodedStyle |= TextStyle::StrikeThrough;
            }

            return TextStyles(decodedStyle);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeRendererData(const String& renderer)
        {
            std::stringstream ss{renderer.toStdString()};
            auto node = DataIO::parse(ss);

            // The root node should contain exactly one child which is the node we need
            if (node->propertyValuePairs.empty() && (node->children.size() == 1))
                node = std::move(node->children[0]);

            auto rendererData = RendererData::create();
            for (const auto& pair : node->propertyValuePairs)
                rendererData->propertyValuePairs[pair.first] = ObjectConverter(pair.second->value); // Did not compile with VS2015 Update 2 when using braces

            for (const auto& child : node->children)
            {
                std::stringstream ss2;
                DataIO::emit(child, ss2);
                rendererData->propertyValuePairs[child->name] = {String("{\n" + ss2.str() + "}")};
            }

            return rendererData;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ObjectConverter::Type, Deserializer::DeserializeFunc> Deserializer::m_deserializers =
        {
            {ObjectConverter::Type::Bool, deserializeBool},
            {ObjectConverter::Type::Font, deserializeFont},
            {ObjectConverter::Type::Color, deserializeColor},
            {ObjectConverter::Type::String, deserializeString},
            {ObjectConverter::Type::Number, deserializeNumber},
            {ObjectConverter::Type::Outline, deserializeOutline},
            {ObjectConverter::Type::Texture, deserializeTexture},
            {ObjectConverter::Type::TextStyle, deserializeTextStyle},
            {ObjectConverter::Type::RendererData, deserializeRendererData}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter Deserializer::deserialize(ObjectConverter::Type type, const String& serializedString)
    {
        TGUI_ASSERT(m_deserializers.find(type) != m_deserializers.end(), "Type passed to deserializer must be valid");
        return m_deserializers[type](serializedString);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Deserializer::setFunction(ObjectConverter::Type type, const DeserializeFunc& deserializer)
    {
        m_deserializers[type] = deserializer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Deserializer::DeserializeFunc& Deserializer::getFunction(ObjectConverter::Type type)
    {
        return m_deserializers[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> Deserializer::split(const String& str, char delim)
    {
        std::vector<String> tokens;

        std::size_t start = 0;
        std::size_t end = 0;
        while ((end = str.find(delim, start)) != String::npos) {
            tokens.push_back(str.substr(start, end - start).trim());
            start = end + 1;
        }

        tokens.push_back(str.substr(start).trim());
        return tokens;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
