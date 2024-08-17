/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_BACKEND_RAYLIB_HPP
#define TGUI_BACKEND_RAYLIB_HPP

#include <TGUI/Config.hpp>
#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

#include <TGUI/Backend/Window/Raylib/BackendGuiRaylib.hpp>

#include <raylib.h>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <unordered_map>
    #include <map>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_IGNORE_DEPRECATED_WARNINGS_START // Required for VS2017 due to inheriting a function that we deprecated

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @since TGUI 1.3
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRaylib : public Backend
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the look of a certain mouse cursor by using a bitmap
        ///
        /// @param type    Cursor that should make use of the bitmap
        /// @param pixels  Pointer to an array with 4*size.x*size.y elements, representing the pixels in 32-bit RGBA format
        /// @param size    Size of the cursor
        /// @param hotspot Pixel coordinate within the cursor image which will be located exactly at the mouse pointer position
        ///
        /// @warning This function does nothing because raylib does not support changing the cursor style
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMouseCursorStyle(Cursor::Type type, const std::uint8_t* pixels, Vector2u size, Vector2u hotspot) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the look of a certain mouse cursor back to the system theme
        ///
        /// @param type  Cursor that should no longer use a custom bitmap
        ///
        /// @warning This function does nothing because raylib does not support changing the cursor style
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void resetMouseCursorStyle(Cursor::Type type) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the mouse cursor when the mouse is on top of the window to which the gui is attached
        ///
        /// @param gui   The gui that represents the window for which the mouse cursor should be changed
        /// @param type  Which cursor to use
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMouseCursor(BackendGui* gui, Cursor::Type type) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks the state for one of the modifier keys
        ///
        /// @param modifierKey  The modifier key of which the state is being queried
        ///
        /// @return Whether queries modifier key is being pressed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("Use gui.isKeyboardModifierPressed(modifierKey) instead") TGUI_NODISCARD bool isKeyboardModifierPressed(Event::KeyModifier modifierKey) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the contents of the clipboard
        ///
        /// @param contents  New contents of the clipboard
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setClipboard(const String& contents) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the contents of the clipboard
        ///
        /// @return Clipboard contents
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD String getClipboard() const override;

#ifdef TGUI_SYSTEM_ANDROID
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Uses the AssetManager on Android to read a file and return its contents
        ///
        /// @param filename  Path to the file to read
        /// @param fileSize  Size of the file, to be filled in by this function if loading succeeds (untouched on failure)
        ///
        /// @return File contents if the file was successfully read, or a nullptr on failure or if platform isn't Android.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::unique_ptr<std::uint8_t[]> readFileFromAndroidAssets(const String& filename, std::size_t& fileSize) const override;
#endif
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

TGUI_IGNORE_DEPRECATED_WARNINGS_END

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_RAYLIB_HPP
