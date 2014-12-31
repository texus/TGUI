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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Borders
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /// \param leftBorderWidth    Width of the left border
        /// \param topBorderHeight    Height of the top border
        /// \param rightBorderWidth   Width of the right border
        /// \param bottomBorderHeight Height of the bottom border
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Borders(unsigned int leftBorderWidth = 0,
                unsigned int topBorderHeight = 0,
                unsigned int rightBorderWidth = 0,
                unsigned int bottomBorderHeight = 0) :
            left  (leftBorderWidth),
            top   (topBorderHeight),
            right (rightBorderWidth),
            bottom(bottomBorderHeight)
        {
        }

        /// Width of the left border
        unsigned int left;

        /// Height of the top border
        unsigned int top;

        /// Width of the right border
        unsigned int right;

        /// Height of the bottom border
        unsigned int bottom;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BORDERS_HPP
