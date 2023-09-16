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


#ifndef TGUI_WINDOWS_IMM_HPP
#define TGUI_WINDOWS_IMM_HPP

#include <TGUI/Vector2.hpp>

#include <TGUI/extlibs/IncludeWindows.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Dynamically loads and uses imm32.dll on Windows to control the IME
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API WindowsIMM
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // @brief Loads the imm32.dll and the functions that this class might access
        //
        // If this function is called multiple times then the resources are reused.
        //
        // @warning Every call to initialize() must have a corresponding call to release()
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void initialize();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // @brief Releases the resources from imm32.dll
        //
        // A reference counter was incremented on each initialize() call, this function does nothing until the count reaches 0.
        //
        // @warning This function must not be called without calling initialize() first
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void release();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is called by TGUI when the position of the cursor changes in a text field (EditBox or TextArea).
        ///        It may result in the IME window moving.
        ///
        /// @param hWnd      Handle to the window that contains the focused widget
        /// @param caretPos  Location of the text cursor, relative to the top-left of the window, in pixels
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setCandidateWindowPosition(HWND hWnd, Vector2f caretPos);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        using ImmGetContextFunc = HIMC(WINAPI*)(HWND);
        using ImmSetCompositionWindowFunc = BOOL(WINAPI*)(HIMC, LPCOMPOSITIONFORM);
        using ImmReleaseContextFunc = BOOL(WINAPI*)(HWND, HIMC);

        static ImmGetContextFunc m_dllImmGetContext;
        static ImmSetCompositionWindowFunc m_dllImmSetCompositionWindow;
        static ImmReleaseContextFunc m_dllImmReleaseContext;

        static HMODULE m_dllImmModuleHandle;

        static unsigned int m_referenceCount;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_WINDOWS_IMM_HPP
