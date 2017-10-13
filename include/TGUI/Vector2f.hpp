/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_VECTOR2F_HPP
#define TGUI_VECTOR2F_HPP

#include <TGUI/Global.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Err.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API Vector2f : public sf::Vector2f
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor to create from X and Y values
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f(float xValue, float yValue) :
            sf::Vector2f{xValue, yValue}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor to create from a string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f(std::string str)
        {
            if (str.empty())
            {
                TGUI_PRINT_WARNING("Failed to parse Vector2f. String was empty.");
                return;
            }

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
            {
                x = 0;
                y = 0;
                return;
            }

            auto commaPos = str.find(',');
            if (commaPos == std::string::npos)
            {
                TGUI_PRINT_WARNING("Failed to parse Vector2f '" + str + "'. Expected numbers separated with a comma.");
                return;
            }

            x = tgui::stof(trim(str.substr(0, commaPos)));
            y = tgui::stof(trim(str.substr(commaPos + 1)));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_VECTOR2F_HPP
