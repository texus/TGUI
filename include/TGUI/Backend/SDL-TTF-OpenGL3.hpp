/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_BACKEND_SDL_TTF_OPENGL3_INCLUDE_HPP
#define TGUI_BACKEND_SDL_TTF_OPENGL3_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
    #error "TGUI wasn't build with the SDL_TTF_OPENGL3 backend"
#endif

#include <TGUI/Backend/Window/SDL/BackendSDL.hpp>
#include <TGUI/Backend/Renderer/OpenGL3/BackendRendererOpenGL3.hpp>
#include <TGUI/Backend/Font/SDL_ttf/BackendFontSDLttf.hpp>

namespace tgui
{
    inline namespace SDL_TTF_OPENGL3
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for SDL + OpenGL3 backend (using SDL_ttf for fonts)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API Gui : public BackendGuiSDL
        {
        public:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Default constructor
            ///
            /// @warning You must still call setWindow on the Gui before using any TGUI functions
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui() = default;


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Constructor that immediately sets the window on which the gui should be drawn
            ///
            /// @param window  The SDL window that will be used by the gui
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(SDL_Window* window)
            {
                setWindow(window);
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window on which the gui should be drawn
            ///
            /// @param window  The SDL window that will be used by the gui
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(SDL_Window* window);
        };
    }

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    #if !TGUI_USE_GLES
    using GuiSDL TGUI_DEPRECATED("GuiSDL was renamed to Gui ") = SDL_TTF_OPENGL3::Gui;
    #endif
#endif

}

#endif // TGUI_BACKEND_SDL_TTF_OPENGL3_INCLUDE_HPP
