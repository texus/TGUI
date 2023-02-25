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


#ifndef TGUI_CURSOR_HPP
#define TGUI_CURSOR_HPP

#include <TGUI/Config.hpp>
#include <TGUI/Vector2.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Class that allows the cursor to be changed
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Cursor
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief List of available cursors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class Type
        {
            Arrow,                  //!< Arrow cursor (default)
            Text,                   //!< I-beam, cursor when hovering over a text field
            Hand,                   //!< Pointing hand cursor
            SizeLeft,               //!< Left arrow on Linux, horizontal double arrow cursor on Windows and macOS
            SizeRight,              //!< Right arrow on Linux, horizontal double arrow cursor on Windows and macOS
            SizeTop,                //!< Up arrow on Linux, vertical double arrow cursor on Windows and macOS
            SizeBottom,             //!< Down arrow on Linux, vertical double arrow cursor on Windows and macOS
            SizeTopLeft,            //!< Top-left arrow on Linux, double arrow cursor going from top-left to bottom-right on Windows and macOS
            SizeBottomRight,        //!< Bottom-right arrow on Linux, double arrow cursor going from top-left to bottom-right on Windows and
            SizeBottomLeft,         //!< Bottom-left arrow on Linux, double arrow cursor going from bottom-left to top-right on Windows and macOS
            SizeTopRight,           //!< Top-right arrow on Linux, double arrow cursor going from bottom-left to top-right on Windows and macOS
            SizeHorizontal,         //!< Horizontal double arrow cursor
            SizeVertical,           //!< Vertical double arrow cursor
            Crosshair,              //!< Crosshair cursor
            Help,                   //!< Help cursor
            NotAllowed              //!< Action not allowed cursor
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes how a specific cursor should look like
        ///
        /// @param type    Which cursor's look has to be changed?
        /// @param pixels  Pointer to an array with 4*size.x*size.y elements, representing the pixels in 32-bit RGBA format
        /// @param size    Size of the cursor
        /// @param hotspot Pixel coordinate within the cursor image which will be located exactly at the mouse pointer position
        ///
        /// By default the system cursors are used, this function allows replacing them with bitmaps.
        /// The effects of this function can be undone by calling resetCursorLook.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setStyle(Type type, const std::uint8_t* pixels, Vector2u size, Vector2u hotspot);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the look of the cursor back to the default system cursors
        ///
        /// @param type  Which cursor's look should be reset?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void resetStyle(Type type);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CURSOR_HPP
