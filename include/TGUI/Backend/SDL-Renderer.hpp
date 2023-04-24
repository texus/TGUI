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

#ifndef TGUI_BACKEND_SDL_RENDERER_INCLUDE_HPP
#define TGUI_BACKEND_SDL_RENDERER_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_SDL_RENDERER
    #error "TGUI wasn't build with the SDL_RENDERER backend"
#endif

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui.backend.window.sdl;
    import tgui.backend.renderer.sdl_renderer;
    import tgui.backend.font.sdl_ttf;
#else
    #include <TGUI/Backend/Window/SDL/BackendSDL.hpp>
    #include <TGUI/Backend/Renderer/SDL_Renderer/BackendRendererSDL.hpp>
    #include <TGUI/Backend/Font/SDL_ttf/BackendFontSDLttf.hpp>
#endif

TGUI_MODULE_EXPORT namespace tgui
{
    inline namespace SDL_RENDERER
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for SDL + SDL_ttf (using SDL_Renderer struct for rendering)
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
            /// @brief Constructor that immediately sets the window and renderer on which the gui should be drawn
            ///
            /// @param window    The SDL window that will be used by the gui
            /// @param renderer  The SDL renderer that will be used for rendering the gui on the window
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(SDL_Window* window, SDL_Renderer* renderer)
            {
                setWindow(window, renderer);
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window and renderer on which the gui should be drawn
            ///
            /// @param window  The SDL window that will be used by the gui
            /// @param renderer  The SDL renderer that will be used for rendering the gui on the window
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(SDL_Window* window, SDL_Renderer* renderer);


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Returns the renderer that was provided to the gui
            ///
            /// @return Renderer that was set via the constructor or setWindow
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            SDL_Renderer* getRenderer() const;


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        protected:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Shows the drawn content with SDL_RenderPresent
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void presentScreen() override;

#if ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION < 26))
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Updates m_framebufferSize by calling SDL_GetRendererOutputSize
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void updateFramebufferSize() override;
#endif

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        protected:

            SDL_Renderer* m_renderer = nullptr;
        };
    }
}

#endif // TGUI_BACKEND_SDL_RENDERER_INCLUDE_HPP
