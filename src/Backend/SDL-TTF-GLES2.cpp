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


#include <TGUI/Backend/SDL-TTF-GLES2.hpp>

#include <TGUI/extlibs/IncludeSDL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    inline namespace SDL_TTF_GLES2
    {
        void Gui::setWindow(SDL_Window* window)
        {
            if (!isBackendSet())
            {
                auto backend = std::make_shared<BackendSDL>();
                backend->setFontBackend(std::make_shared<BackendFontFactoryImpl<BackendFontSDLttf>>());
                backend->setRenderer(std::make_shared<BackendRendererGLES2>(SDL_GL_GetProcAddress));
                backend->setDestroyOnLastGuiDetatch(true);
                setBackend(backend);
            }

            m_backendRenderTarget = std::make_shared<BackendRenderTargetGLES2>();

            setGuiWindow(window);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::presentScreen()
        {
            SDL_GL_SwapWindow(m_window);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION < 26))
        void Gui::updateFramebufferSize()
        {
            SDL_GL_GetDrawableSize(m_window, &m_framebufferSize.x, &m_framebufferSize.y);
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
