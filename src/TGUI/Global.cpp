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


#include <TGUI/Global.hpp>
#include <TGUI/Clipboard.hpp>

#include <cctype>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TextureManager TGUI_TextureManager;

    Clipboard TGUI_Clipboard;

    bool TGUI_TabKeyUsageEnabled = true;

    std::string TGUI_ResourcePath = "";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void enableTabKeyUsage()
    {
        TGUI_TabKeyUsageEnabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void disableTabKeyUsage()
    {
        TGUI_TabKeyUsageEnabled = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const std::string& path)
    {
        TGUI_ResourcePath = path;

        if (!TGUI_ResourcePath.empty())
        {
            if (TGUI_ResourcePath[TGUI_ResourcePath.length()-1] != '/')
                TGUI_ResourcePath.push_back('/');
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& getResourcePath()
    {
        return TGUI_ResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color extractColor(std::string string)
    {
        int red;
        int green;
        int blue;
        int alpha = 255;

        // Make sure that the line isn't empty
        if (string.empty() == false)
        {
            // The first and last character have to be brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                // Remove the brackets
                string.erase(0, 1);
                string.erase(string.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the red value and delete this part of the string
                    red = atoi(string.substr(0, commaPos).c_str());
                    string.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the green value and delete this part of the string
                        green = atoi(string.substr(0, commaPos).c_str());
                        string.erase(0, commaPos+1);

                        // Search for the third comma (optional)
                        commaPos = string.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the blue value and delete this part of the string
                            blue = atoi(string.substr(0, commaPos).c_str());
                            string.erase(0, commaPos+1);

                            // Get the alpha value
                            alpha = atoi(string.c_str());
                        }
                        else // No alpha value was passed
                        {
                            // Get the blue value
                            blue = atoi(string.substr(0, commaPos).c_str());
                        }

                        // All values have to be unsigned chars
                        return sf::Color(static_cast <unsigned char> (red),
                                         static_cast <unsigned char> (green),
                                         static_cast <unsigned char> (blue),
                                         static_cast <unsigned char> (alpha));
                    }
                }
            }
        }

        // If you pass here then something is wrong about the line, the color will be black
        return sf::Color::Black;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string convertColorToString(const sf::Color& color)
    {
        // Return the color as a string
        if (color.a < 255)
            return "(" + tgui::to_string((unsigned int)color.r)
                 + "," + tgui::to_string((unsigned int)color.g)
                 + "," + tgui::to_string((unsigned int)color.b)
                 + "," + tgui::to_string((unsigned int)color.a)
                 + ")";
        else
            return "(" + tgui::to_string((unsigned int)color.r)
                 + "," + tgui::to_string((unsigned int)color.g)
                 + "," + tgui::to_string((unsigned int)color.b)
                 + ")";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractVector2f(std::string string, sf::Vector2f& vector)
    {
        // Make sure that the string isn't empty
        if (string.empty() == false)
        {
            // The first and last character have to be brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                // Remove the brackets
                string.erase(0, 1);
                string.erase(string.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the x value and delete this part of the string
                    vector.x = static_cast<float>(atof(string.substr(0, commaPos).c_str()));
                    string.erase(0, commaPos+1);

                    // Get the y value
                    vector.y = static_cast<float>(atof(string.c_str()));

                    return true;
                }
            }
        }

        // If you pass here then something is wrong with the string
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractVector2u(std::string string, sf::Vector2u& vector)
    {
        // Make sure that the string isn't empty
        if (string.empty() == false)
        {
            // The first and last character have to be brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                // Remove the brackets
                string.erase(0, 1);
                string.erase(string.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the x value and delete this part of the string
                    vector.x = static_cast<unsigned int>(atoi(string.substr(0, commaPos).c_str()));
                    string.erase(0, commaPos+1);

                    // Get the y value
                    vector.y = static_cast<unsigned int>(atoi(string.c_str()));

                    return true;
                }
            }
        }

        // If you pass here then something is wrong with the string
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractBorders(std::string string, Borders& borders)
    {
        // Make sure that the line isn't empty
        if (string.empty() == false)
        {
            // The first and last character have to be brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                // Remove the brackets
                string.erase(0, 1);
                string.erase(string.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the first value and delete this part of the string
                    borders.left = atoi(string.substr(0, commaPos).c_str());
                    string.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the second value and delete this part of the string
                        borders.top = atoi(string.substr(0, commaPos).c_str());
                        string.erase(0, commaPos+1);

                        // Search for the third comma
                        commaPos = string.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the third value and delete this part of the string
                            borders.right = atoi(string.substr(0, commaPos).c_str());
                            string.erase(0, commaPos+1);

                            // Get the fourth value
                            borders.bottom = atoi(string.c_str());

                            return true;
                        }
                    }
                }
            }
        }

        // If you pass here then something is wrong with the string
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void encodeString(const std::string& origString, std::string& encodedString)
    {
        encodedString = origString;

        // Escape backslashes
        std::string::size_type pos = encodedString.find('\\');
        while (pos != std::string::npos)
        {
            encodedString.replace(pos, 1, "\\\\");
            pos = encodedString.find('\\', pos + 2);
        }

        // Escape newlines
        pos = encodedString.find('\n');
        while (pos != std::string::npos)
        {
            encodedString.replace(pos, 1, "\\n");
            pos = encodedString.find('\n', pos + 2);
        }

        // Escape tabs
        pos = encodedString.find('\t');
        while (pos != std::string::npos)
        {
            encodedString.replace(pos, 1, "\\t");
            pos = encodedString.find('\t', pos + 2);
        }

        // Escape quotes
        pos = encodedString.find('\"');
        while (pos != std::string::npos)
        {
            encodedString.replace(pos, 1, "\\\"");
            pos = encodedString.find('\"', pos + 2);
        }

        // Escape commas
        pos = encodedString.find(',');
        while (pos != std::string::npos)
        {
            encodedString.insert(pos, "\\");
            pos = encodedString.find(',', pos + 2);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void decodeString(const std::string& encodedString, std::string& decodedString)
    {
        decodedString = "";

        for (auto it = encodedString.cbegin(); it != encodedString.cend(); ++it)
        {
            // Check if the character is an escape character
            if (*it == '\\')
            {
                auto next = it + 1;

                if (next == encodedString.cend())
                {
                    TGUI_OUTPUT("TGUI warning: Escape character at the end of the string. Ignoring character.");
                    continue;
                }
                else if ((*next == '\\') || (*next == '\"'))
                {
                    decodedString += *next;
                    ++it;
                }
                else if (*next == 'n')
                {
                    decodedString += '\n';
                    ++it;
                }
                else if (*next == 't')
                {
                    decodedString += '\t';
                    ++it;
                }
                else
                {
                    TGUI_OUTPUT(std::string("TGUI warning: Escape character in front of '") + *next + "'. Ignoring escape character.");
                    continue;
                }
            }
            else // No escape character, just a normal character to be added to the string
                decodedString += *it;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void encodeList(const std::vector<sf::String>& list, std::string& encodedString)
    {
        encodedString = "";

        for (auto it = list.cbegin(); it != list.cend(); ++it)
        {
            std::string item = *it;

            // Escape backslashes
            std::string::size_type pos = item.find('\\');
            while (pos != std::string::npos)
            {
                item.replace(pos, 1, "\\\\");
                pos = item.find('\\', pos + 2);
            }

            // Escape newlines
            pos = item.find('\n');
            while (pos != std::string::npos)
            {
                item.replace(pos, 1, "\\n");
                pos = item.find('\n', pos + 2);
            }

            // Escape tabs
            pos = item.find('\t');
            while (pos != std::string::npos)
            {
                item.replace(pos, 1, "\\t");
                pos = item.find('\t', pos + 2);
            }

            // Escape quotes
            pos = item.find('\"');
            while (pos != std::string::npos)
            {
                item.replace(pos, 1, "\\\"");
                pos = item.find('\"', pos + 2);
            }

            // Escape commas
            pos = item.find(',');
            while (pos != std::string::npos)
            {
                item.replace(pos, 1, "\\,");
                pos = item.find(',', pos + 2);
            }

            // Add the item to the string
            if (it == list.cbegin())
                encodedString += item;
            else
                encodedString += "," + item;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void decodeList(const std::string& encodedString, std::vector<sf::String>& list)
    {
        std::string item;

        for (auto it = encodedString.cbegin(); it != encodedString.cend(); ++it)
        {
            // Check if the character is an escape character
            if (*it == '\\')
            {
                auto next = it + 1;

                if (next == encodedString.cend())
                {
                    TGUI_OUTPUT("TGUI warning: Escape character at the end of the string. Ignoring character.");
                    continue;
                }
                else if ((*next == '\\') || (*next == '\"') || (*next == ','))
                {
                    item += *next;
                    ++it;
                }
                else if (*next == 'n')
                {
                    item += '\n';
                    ++it;
                }
                else if (*next == 't')
                {
                    item += '\t';
                    ++it;
                }
                else
                {
                    TGUI_OUTPUT(std::string("TGUI warning: Escape character in front of '") + *next + "'. Ignoring escape character.");
                    continue;
                }
            }
            else // No escape character
            {
                // Check if the next item starts here
                if (*it == ',')
                {
                    list.push_back(item);
                    item = "";
                }
                else // Just a normal character to be added to the string
                    item += *it;
            }
        }

        if (!item.empty())
            list.push_back(item);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string toLower(std::string str)
    {
        for (std::string::iterator i = str.begin(); i != str.end(); ++i)
            *i = static_cast<char>(std::tolower(*i));

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
