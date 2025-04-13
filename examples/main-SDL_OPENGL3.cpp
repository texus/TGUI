/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SDL-OpenGL3.hpp>

#if SDL_MAJOR_VERSION >= 3
    #include <SDL3/SDL_main.h>
    #include <SDL3/SDL_opengl.h>
#else
    #include <SDL_opengl.h>
#endif

bool runExample(tgui::BackendGui& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying SDL
void run_application(SDL_Window* window)
{
    tgui::Gui gui(window);
    if (!runExample(gui))
        return;

    gui.mainLoop(); // To use your own main loop, see https://tgui.eu/tutorials/latest-stable/backend-sdl-opengl3/#main-loop
}

// Note that no error checking is performed on SDL initialization in this example code
int main(int, char **)
{
    SDL_Init(SDL_INIT_VIDEO);

    // The OpenGL renderer backend in TGUI requires at least OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // TGUI requires a window created with the SDL_WINDOW_OPENGL flag and an OpenGL context
#if SDL_MAJOR_VERSION >= 3
    SDL_Window* window = SDL_CreateWindow("TGUI example (SDL-OpenGL3)", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
    SDL_Window* window = SDL_CreateWindow("TGUI example (SDL-OpenGL3)",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // Note that the GL context has to exist before initializing TGUI
    run_application(window);

    // Note that all TGUI resources must be destroyed before SDL is cleaned up
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
