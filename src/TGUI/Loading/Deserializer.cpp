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


#include <TGUI/Loading/Deserializer.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    unsigned char hexToDec(char c)
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        else if (c == 'A' || c == 'a')
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
        else
            return 0;
    }

    bool readIntRect(std::string value, sf::IntRect& rect)
    {
        // Make sure that the line isn't empty
        if (value.empty() == false)
        {
            // The first and last character have to be brackets
            if ((value[0] == '(') && (value[value.length()-1] == ')'))
            {
                // Remove the brackets
                value.erase(0, 1);
                value.erase(value.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = value.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the left value and delete this part of the string
                    rect.left = tgui::stoi(value.substr(0, commaPos));
                    value.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = value.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the top value and delete this part of the string
                        rect.top = tgui::stoi(value.substr(0, commaPos));
                        value.erase(0, commaPos+1);

                        // Search for the third comma
                        commaPos = value.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the width value and delete this part of the string
                            rect.width = tgui::stoi(value.substr(0, commaPos));
                            value.erase(0, commaPos+1);

                            // Get the height value
                            rect.height = tgui::stoi(value);

                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }
}

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeFont(const std::string& /*value*/)
    {
        return {std::shared_ptr<sf::Font>()}; /// TODO: Support deserializing fonts
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeColor(const std::string& value)
    {
        std::string string = value;
        sf::Color color;

        // Make sure that the line isn't empty
        if (!string.empty())
        {
            // The first way a color can be represented is with a hexadecimal number
            if (value[0] == '#')
            {
                // You can only have hex characters
                for (unsigned int i = 1; i < value.length(); ++i)
                {
                    if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F')  || (value[i] >= 'a' && value[i] <= 'f')))
                        throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but contained an invalid character afterwards."};
                }

                // Parse the different types of strings (#123, #1234, #112233 and #11223344)
                if (value.length() == 4)
                {
                    color = {static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                             static_cast<sf::Uint8>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                             static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[3]))};
                }
                else if (value.length() == 5)
                {
                    color = {static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                             static_cast<sf::Uint8>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                             static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[3])),
                             static_cast<sf::Uint8>(hexToDec(value[4]) * 16 + hexToDec(value[4]))};
                }
                else if (value.length() == 7)
                {
                    color = {static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                             static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                             static_cast<sf::Uint8>(hexToDec(value[5]) * 16 + hexToDec(value[6]))};
                }
                else if (value.length() == 9)
                {
                    color = {static_cast<sf::Uint8>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                             static_cast<sf::Uint8>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                             static_cast<sf::Uint8>(hexToDec(value[5]) * 16 + hexToDec(value[6])),
                             static_cast<sf::Uint8>(hexToDec(value[7]) * 16 + hexToDec(value[8]))};
                }
                else
                    throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but has the wrong length."};

                return {color};
            }

            // The string can optionally start with "rgb" or "rgba", but this is ignored
            if (string.substr(0, 3) == "rgb")
                string.erase(0, 3);
            else if (string.substr(0, 4) == "rgba")
                string.erase(0, 4);

            // Remove the first and last characters when they are brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                string.erase(0, 1);
                string.erase(string.length()-1);
            }

            // Search for the first comma
            std::string::size_type commaPos = string.find(',');
            if (commaPos != std::string::npos)
            {
                // Get the red value and delete this part of the string
                color.r = tgui::stoi(string.substr(0, commaPos));
                string.erase(0, commaPos+1);

                // Search for the second comma
                commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the green value and delete this part of the string
                    color.g = tgui::stoi(string.substr(0, commaPos));
                    string.erase(0, commaPos+1);

                    // Search for the third comma (optional)
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the blue value and delete this part of the string
                        color.b = tgui::stoi(string.substr(0, commaPos));
                        string.erase(0, commaPos+1);

                        // Get the alpha value
                        color.a = tgui::stoi(string);
                    }
                    else // No alpha value was passed
                    {
                        // Get the blue value
                        color.b = tgui::stoi(string.substr(0, commaPos));
                    }

                    return {color};
                }
            }
        }

        throw Exception{"Failed to deserialize color '" + value + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeString(const std::string& value)
    {
        /// TODO: When no quotes around it then just return it, otherwise remove quotes and replace stuff like \n in the string

        // Remove the first and last characters when they are quotes
        if (!value.empty() || ((value[0] == '"') && (value[value.length()-1] == '"')))
            return {sf::String{value.substr(1, value.length()-2)}};
        else
            return {sf::String{value}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API ObjectConverter deserializeBorders(const std::string& value)
    {
        std::string string = value;
        tgui::Borders borders;

        // Make sure that the line isn't empty
        if (string.empty() == false)
        {
            // Remove the first and last characters when they are brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                string.erase(0, 1);
                string.erase(string.length()-1);
            }

            // Search for the first comma
            std::string::size_type commaPos = string.find(',');
            if (commaPos != std::string::npos)
            {
                // Get the first value and delete this part of the string
                borders.left = tgui::stof(string.substr(0, commaPos));
                string.erase(0, commaPos+1);

                // Search for the second comma
                commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the second value and delete this part of the string
                    borders.top = tgui::stof(string.substr(0, commaPos));
                    string.erase(0, commaPos+1);

                    // Search for the third comma
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the third value and delete this part of the string
                        borders.right = tgui::stof(string.substr(0, commaPos));
                        string.erase(0, commaPos+1);

                        // Get the fourth value
                        borders.bottom = tgui::stof(string);

                        return {borders};
                    }
                }
            }
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
        bool repeat = false;

        while (tgui::removeWhitespace(value, c))
        {
            std::string word;
            auto openingBracketPos = value.find('(', c - value.begin());
            if (openingBracketPos != std::string::npos)
                word = value.substr(c - value.begin(), openingBracketPos - (c - value.begin()));
            else
                word = value.substr(c - value.begin(), value.length() - (c - value.begin()));

            if (toLower(word) == "stretch")
            {
                repeat = false;
                std::advance(c, 7);
            }
            else if (toLower(word) == "repeat")
            {
                repeat = true;
                std::advance(c, 6);
            }
            else
            {
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
                    throw tgui::Exception{"Failed to deserialize texture '" + value + "'. Unexpected word '" + word + "' in front of opening bracket."};

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
        }

        /// TODO: Change to oneliner once Texture constructor allows it
        tgui::Texture texture;
        texture.load(getResourcePath() + filename, partRect, middleRect, repeat);
        return {texture};
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
