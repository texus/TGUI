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


#ifndef TGUI_BORDERS_HPP
#define TGUI_BORDERS_HPP

#include <TGUI/Config.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Borders
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Borders(float size = 0) :
            left  (size),
            top   (size),
            right (size),
            bottom(size)
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that initializes the borders
        ///
        /// @param width   Width of the left and right borders
        /// @param height  Height of the top and bottom borders
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Borders(float width, float height) :
            left  (width),
            top   (height),
            right (width),
            bottom(height)
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that initializes the borders
        ///
        /// @param leftBorderWidth    Width of the left border
        /// @param topBorderHeight    Height of the top border
        /// @param rightBorderWidth   Width of the right border
        /// @param bottomBorderHeight Height of the bottom border
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Borders(float leftBorderWidth, float topBorderHeight, float rightBorderWidth, float bottomBorderHeight) :
            left  (leftBorderWidth),
            top   (topBorderHeight),
            right (rightBorderWidth),
            bottom(bottomBorderHeight)
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compare two borders
        ///
        /// @param borders  The borders to compare with this instance
        ///
        /// @return Whether the borders are equal or not.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool operator==(const Borders& borders) const
        {
            return (left == borders.left) && (top == borders.top) && (right == borders.right) && (bottom == borders.bottom);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compare two borders
        ///
        /// @param borders  The borders to compare with this instance
        ///
        /// @return Whether the borders are equal or not.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool operator!=(const Borders& borders) const
        {
            return !(*this == borders);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /// Width of the left border
        float left;

        /// Height of the top border
        float top;

        /// Width of the right border
        float right;

        /// Height of the bottom border
        float bottom;
    };


    using Padding = Borders;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BORDERS_HPP
