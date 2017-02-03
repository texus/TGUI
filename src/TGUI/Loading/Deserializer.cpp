/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned char hexToDec(char c)
    {
        assert((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')  || (c >= 'a' && c <= 'f'));

        if (c == 'A' || c == 'a')
            return 10;
        else if (c == 'B' || c == 'b')
            return 11;
        else if (c == 'C' || c == 'c')
            return 12;
        else if (c == 'D' || c == 'd')
            return 13;
        else if (c == 'E' || c == 'e')
            return 14;
        else if (c == 'F' || c == 'f')
            return 15;
        else // if (c >= '0' && c <= '9')
            return c - '0';
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool readIntRect(std::string value, sf::IntRect& rect)
    {
        if (!value.empty() && (value[0] == '(') && (value[value.length()-1] == ')'))
        {
            std::vector<std::string> tokens = tgui::split(value.substr(1, value.size()-2), ',');
            if (tokens.size() == 4)
            {
                rect = {tgui::stoi(tokens[0]), tgui::stoi(tokens[1]), tgui::stoi(tokens[2]), tgui::stoi(tokens[3])};
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeFont(const std::string& value)
    {
        if (value == "null" || value == "nullptr")
            return std::shared_ptr<sf::Font>();

        auto font = std::make_shared<sf::Font>();
        font->loadFromFile(Deserializer::deserialize(ObjectConverter::Type::String, value).getString());
        return font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeColor(const std::string& value)
    {
        static std::map<std::string, sf::Color> colorMap =
        {
            {"black", sf::Color::Black},
            {"white", sf::Color::White},
            {"red", sf::Color::Red},
            {"yellow", sf::Color::Yellow},
            {"green", sf::Color::Green},
            {"cyan", sf::Color::Cyan},
            {"blue", sf::Color::Blue},
            {"magenta", sf::Color::Magenta},
            {"transparent", sf::Color::Transparent}
        };

        std::string str = tgui::trim(value);

        // Make sure that the line isn't empty
        if (!str.empty())
        {
            // Check if the color is represented by a string with its name
            auto it = colorMap.find(toLower(str));
            if (it != colorMap.end())
                return it->second;

            // The color can be represented with a hexadecimal number
            if (str[0] == '#')
            {
                // You can only have hex characters
                for (std::size_t i = 1; i < value.length(); ++i)
                {
                    if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F')  || (value[i] >= 'a' && value[i] <= 'f')))
                        throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but contained an invalid character afterwards."};
                }

                // Parse the different types of strings (#123, #1234, #112233 and #11223344)
                if (value.length() == 4)
                {
                    return sf::Color{static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                                     static_cast<sf::Uint8>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                                     static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[3]))};
                }
                else if (value.length() == 5)
                {
                    return sf::Color{static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                                     static_cast<sf::Uint8>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                                     static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[3])),
                                     static_cast<sf::Uint8>(hexToDec(value[4]) * 16 + hexToDec(value[4]))};
                }
                else if (value.length() == 7)
                {
                    return sf::Color{static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                                     static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                                     static_cast<sf::Uint8>(hexToDec(value[5]) * 16 + hexToDec(value[6]))};
                }
                else if (value.length() == 9)
                {
                    return sf::Color{static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                                     static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                                     static_cast<sf::Uint8>(hexToDec(value[5]) * 16 + hexToDec(value[6])),
                                     static_cast<sf::Uint8>(hexToDec(value[7]) * 16 + hexToDec(value[8]))};
                }
                else
                    throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but has the wrong length."};
            }

            // The string can optionally start with "rgb" or "rgba", but this is ignored
            if (str.substr(0, 4) == "rgba")
                str.erase(0, 4);
            else if (str.substr(0, 3) == "rgb")
                str.erase(0, 3);

            // Remove the first and last characters when they are brackets
            if ((str[0] == '(') && (str[str.length()-1] == ')'))
                str = str.substr(1, str.length()-2);

            std::vector<std::string> tokens = tgui::split(str, ',');
            if (tokens.size() == 3 || tokens.size() == 4)
            {
                return sf::Color{static_cast<sf::Uint8>(tgui::stoi(tokens[0])),
                                 static_cast<sf::Uint8>(tgui::stoi(tokens[1])),
                                 static_cast<sf::Uint8>(tgui::stoi(tokens[2])),
                                 static_cast<sf::Uint8>((tokens.size() == 4) ? tgui::stoi(tokens[3]) : 255)};
            }
        }

        throw Exception{"Failed to deserialize color '" + value + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeString(const std::string& value)
    {
        // Only deserialize the string when it is surrounded with quotes
        if (!value.empty() && ((value[0] == '"') && (value[value.length()-1] == '"')))
        {
            std::string result = value.substr(1, value.length()-2);

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

            return {sf::String{result}};
        }
        else
            return {sf::String{value}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeNumber(const std::string& value)
    {
        return {tgui::stof(value)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeBorders(const std::string& value)
    {
        std::string str = tgui::trim(value);

        // Make sure that the line isn't empty
        if (!str.empty())
        {
            // Remove the first and last characters when they are brackets
            if ((str[0] == '(') && (str[str.length()-1] == ')'))
                str = str.substr(1, str.length()-2);

            std::vector<std::string> tokens = tgui::split(str, ',');
            if (tokens.size() == 1)
                return tgui::Borders{tgui::stof(tokens[0])};
            else if (tokens.size() == 2)
                return tgui::Borders{tgui::stof(tokens[0]), tgui::stof(tokens[1])};
            else if (tokens.size() == 4)
                return tgui::Borders{tgui::stof(tokens[0]), tgui::stof(tokens[1]), tgui::stof(tokens[2]), tgui::stof(tokens[3])};
        }

        throw Exception{"Failed to deserialize borders '" + value + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeTexture(const std::string& value)
    {
        std::string::const_iterator c = value.begin();

        // Remove all whitespaces (string should still contains something)
        if (!tgui::removeWhitespace(value, c))
            throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Value is empty."};

        // There has to be a quote
        if (*c == '"')
            ++c;
        else
        {
            throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Expected an opening quote for the filename."};
        }

        std::string filename;
        char prev = '\0';

        // Look for the end quote
        bool filenameFound = false;
        while (c != value.end())
        {
            if ((*c != '"') || (prev == '\\'))
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
            throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Failed to find the closing quote of the filename."};

        // There may be optional parameters
        sf::IntRect partRect;
        sf::IntRect middleRect;

        while (tgui::removeWhitespace(value, c))
        {
            std::string word;
            auto openingBracketPos = value.find('(', c - value.begin());
            if (openingBracketPos != std::string::npos)
                word = value.substr(c - value.begin(), openingBracketPos - (c - value.begin()));
            else
                word = value.substr(c - value.begin(), value.length() - (c - value.begin()));

            sf::IntRect* rect = nullptr;
            if ((word == "Part") || (word == "part"))
            {
                rect = &partRect;
                std::advance(c, 4);
            }
            else if ((word == "Middle") || (word == "middle"))
            {
                rect = &middleRect;
                std::advance(c, 6);
            }
            else
            {
                if (word.empty())
                    throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Expected 'Part' or 'Middle' in front of opening bracket."};
                else
                    throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Unexpected word '" + word + "' in front of opening bracket. Expected 'Part' or 'Middle'."};
            }

            auto closeBracketPos = value.find(')', c - value.begin());
            if (closeBracketPos != std::string::npos)
            {
                if (!readIntRect(value.substr(c - value.begin(), closeBracketPos - (c - value.begin()) + 1), *rect))
                    throw tgui::Exception{"Failed to parse " + word + " rectangle while deserializing texture '" + value + "'."};
            }
            else
                throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Failed to find closing bracket for " + word + " rectangle."};

            std::advance(c, closeBracketPos - (c - value.begin()) + 1);
        }

        return tgui::Texture{getResourcePath() + filename, partRect, middleRect};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<ObjectConverter::Type, Deserializer::DeserializeFunc> Deserializer::m_deserializers =
        {
            {ObjectConverter::Type::Font, deserializeFont},
            {ObjectConverter::Type::Color, deserializeColor},
            {ObjectConverter::Type::String, deserializeString},
            {ObjectConverter::Type::Number, deserializeNumber},
            {ObjectConverter::Type::Borders, deserializeBorders},
            {ObjectConverter::Type::Texture, deserializeTexture}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter Deserializer::deserialize(ObjectConverter::Type type, const std::string& serializedString)
    {
        assert(m_deserializers.find(type) != m_deserializers.end());
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
