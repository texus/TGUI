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

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_opengles2.h>

#include <TGUI/Core.hpp>
#include <TGUI/Backends/SDL.hpp>
#include <TGUI/AllWidgets.hpp>

// The background image will rotate with the screen
// TODO: This isn't working. We might not be receiving SDL_WINDOWEVENT_SIZE_CHANGED events.
void setBackground(tgui::GuiBase& gui)
{
    const tgui::Vector2f screenSize = gui.get("Landscape")->getSize();
    if (screenSize.x > screenSize.y)
    {
        gui.get("Landscape")->setVisible(true);
        gui.get("Portrait")->setVisible(false);
    }
    else
    {
        gui.get("Landscape")->setVisible(false);
        gui.get("Portrait")->setVisible(true);
    }
}

bool runExample(tgui::GuiBase& gui)
{
    auto picLandscape = tgui::Picture::create("Background-Landscape.png");
    picLandscape->setSize({"100%", "100%"});
    gui.add(picLandscape, "Landscape");

    auto picPortrait = tgui::Picture::create("Background-Portrait.png");
    picPortrait->setSize({"100%", "100%"});
    gui.add(picPortrait, "Portrait");

    // The button will quit the program
    auto button = tgui::Button::create("Quit");
    button->setPosition(50, 50);
    button->setSize(200, 50);
    gui.add(button);

    // Clicking on this edit box will open the keyboard and allow you to type in it
    auto editBox = tgui::EditBox::create();
    editBox->setPosition(50, 150);
    editBox->setSize(400, 40);
    editBox->setDefaultText("Enter text here...");
    gui.add(editBox);

    setBackground(gui);
    return true;
}

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying SDL
void run_application(SDL_Window* window)
{
    tgui::GuiSDL gui{window};
    if (!runExample(gui))
        return;

    SDL_Event event;
    bool done = false;
    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            gui.handleEvent(event);

            if (event.type == SDL_QUIT)
                done = true;
            else if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
                setBackground(gui);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        gui.draw();
        SDL_GL_SwapWindow(window);
        SDL_Delay(50);
    }
}

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);

    // TGUI requires at least OpenGL ES 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    // TGUI requires a window created with the SDL_WINDOW_OPENGL flag and an OpenGL context
    SDL_Window* window = SDL_CreateWindow("TGUI window with SDL",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600, // ignored because of SDL_WINDOW_FULLSCREEN_DESKTOP flag
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glClearColor(0.8f, 0.8f, 0.8f, 1.f);

    // SDL_TTF needs to be initialized before using TGUI
    TTF_Init();

    run_application(window);

    // All TGUI resources must be destroyed before SDL_TTF is cleaned up
    TTF_Quit();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
