#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SDL-GLES2.hpp>
#include <emscripten/emscripten.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>
#include <iostream>

EM_JS(int, canvas_get_width, (), { return canvas.width; });
EM_JS(int, canvas_get_height, (), { return canvas.height; });

bool quit = false;
SDL_Window* window = nullptr;
SDL_GLContext glContext = nullptr;
std::unique_ptr<tgui::Gui> gui = nullptr;

void init()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        throw std::runtime_error("SDL failed to initialise");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow("TGUI - Emscripten - SDL_GLES2", 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        SDL_Quit();
        throw std::runtime_error("Failed to create window");
    }

    glContext = SDL_GL_CreateContext(window);

    gui = std::make_unique<tgui::Gui>(window);

    auto picture = tgui::Picture::create("data/background.jpg");
    picture->setSize({400, 300});
    gui->add(picture);

    auto button = tgui::Button::create("Hello world");
    button->setPosition({20, 10});
    button->setSize({200, 30});
    button->onClick([] { std::cout << "Button clicked\n"; });
    gui->add(button);
}

void input(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_QUIT)
        quit = true;

    gui->handleEvent(event);
}

void render()
{
    glClearColor(91.f / 255.f, 10.f / 255.f, 145.f / 255.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    gui->draw();
    SDL_GL_SwapWindow(window);
}

void mainLoop()
{
    render();

    SDL_Event event;
    while (SDL_PollEvent(&event))
        input(event);
}

int main(int, char**)
{
    init();

    emscripten_set_main_loop(mainLoop, -1, true);

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
