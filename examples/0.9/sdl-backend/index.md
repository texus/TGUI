---
layout: page
title: "Example: SDL backend"
breadcrumb: "sdl backend"
---

The code below shows the template that is used for the other examples when using the SDL backend.

The runExample function is present in all example codes and contains the code that is independent of the backend. The rest of the code depends on SDL and will thus only be shown here and not repeated in each example.

More information about the minimal code can be found in the [SDL backend tutorial](/tutorials/0.9/sdl-backend/).

``` c++
#include <SDL.h>            // SDL 2.0 required
#include <SDL_ttf.h>        // Minimum 2.0.14 (released Feb 2016)
#include <SDL_image.h>
#include <SDL_opengl.h>

#include <TGUI/Core.hpp>
#include <TGUI/Backends/SDL.hpp>
#include <TGUI/AllWidgets.hpp>

bool runExample(tgui::GuiBase& gui)
{
    return true;
}

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying SDL
void run_application(SDL_Window* window)
{
    tgui::GuiSDL gui{window};
    if (!runExample(gui))
        return;

    gui.mainLoop();
}

// Note that no error checking is performed on SDL initialization in this example code
int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    // TGUI currently requires at least OpenGL 4.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // TGUI requires a window created with the SDL_WINDOW_OPENGL flag and an OpenGL context
    SDL_Window* window = SDL_CreateWindow("TGUI window with SDL",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glClearColor(0.8f, 0.8f, 0.8f, 1.f);

    // SDL_Image and SDL_TTF need to be initialized before using TGUI
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();

    run_application(window);

    // All TGUI resources must be destroyed before SDL_Image and SDL_TTF are cleaned up
    TTF_Quit();
    IMG_Quit();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
```
