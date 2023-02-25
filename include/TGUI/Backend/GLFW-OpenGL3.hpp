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

#ifndef TGUI_BACKEND_GLFW_OPENGL3_INCLUDE_HPP
#define TGUI_BACKEND_GLFW_OPENGL3_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_GLFW_OPENGL3
    #error "TGUI wasn't build with the GLFW_OPENGL3 backend"
#endif

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui.backend.window.glfw;
    import tgui.backend.renderer.opengl3;
    import tgui.backend.font.freetype;
#else
    #include <TGUI/Backend/Window/GLFW/BackendGLFW.hpp>
    #include <TGUI/Backend/Renderer/OpenGL3/BackendRendererOpenGL3.hpp>
    #include <TGUI/Backend/Font/FreeType/BackendFontFreeType.hpp>
#endif

using GLFWwindow = struct GLFWwindow;

TGUI_MODULE_EXPORT namespace tgui
{
    inline namespace GLFW_OPENGL3
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for GLFW + OpenGL3 backend (using FreeType for fonts)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API Gui : public BackendGuiGLFW
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
            /// @param window  The GLFW window that will be used by the gui
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(GLFWwindow* window)
            {
                setWindow(window);
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window on which the gui should be drawn
            ///
            /// @param window  The GLFW window that will be used by the gui
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(GLFWwindow* window);
        };
    }
}

#endif // TGUI_BACKEND_GLFW_OPENGL3_INCLUDE_HPP
