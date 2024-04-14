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

#ifndef TGUI_BACKEND_RAYLIB_INCLUDE_HPP
#define TGUI_BACKEND_RAYLIB_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_RAYLIB
    #error "TGUI wasn't build with the RAYLIB backend"
#endif

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Raylib/BackendRaylib.hpp>
    #include <TGUI/Backend/Renderer/Raylib/BackendRendererRaylib.hpp>
    #include <TGUI/Backend/Font/Raylib/BackendFontRaylib.hpp>
#endif

#include <raylib.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    inline namespace RAYLIB
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for raylib
        /// @since TGUI 1.3
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API Gui : public BackendGuiRaylib
        {
        public:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui();
        };
    }
}

#endif // TGUI_BACKEND_RAYLIB_INCLUDE_HPP
