/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TextureManager TGUI_TextureManager;

    bool tabKeyUsageEnabled = true;

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

    bool extractVector2f(std::string string, Vector2f& vector)
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

    bool extractVector2u(std::string string, Vector2u& vector)
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

    bool extractVector4u(std::string string, Vector4u& vector)
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
                    vector.x1 = atoi(string.substr(0, commaPos).c_str());
                    string.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the second value and delete this part of the string
                        vector.x2 = atoi(string.substr(0, commaPos).c_str());
                        string.erase(0, commaPos+1);

                        // Search for the third comma
                        commaPos = string.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the third value and delete this part of the string
                            vector.x3 = atoi(string.substr(0, commaPos).c_str());
                            string.erase(0, commaPos+1);

                            // Get the fourth value
                            vector.x4 = atoi(string.c_str());

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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
