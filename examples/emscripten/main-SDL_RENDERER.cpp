#include <TGUI/TGUI.hpp>

#include <TGUI/Backend/SDL-Renderer.hpp>

#include <emscripten/emscripten.h>
#include <iostream>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
std::unique_ptr<tgui::Gui> gui = nullptr;

void init()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        throw std::runtime_error("SDL failed to initialise");

    TTF_Init();

    window = SDL_CreateWindow("TGUI - Emscripten - SDL_RENDERER", 640, 480, SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        SDL_Quit();
        throw std::runtime_error("Failed to create window");
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Failed to create renderer");
    }

    gui = std::make_unique<tgui::Gui>(window, renderer);

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
    gui->handleEvent(event);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 91, 10, 145, 255);
    SDL_RenderClear(renderer);
    gui->draw();
    SDL_RenderPresent(renderer);
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
    emscripten_set_main_loop(mainLoop, -1, true); // Never returns
    return 0; // Never reached, but required if SDL3/SDL_main.h is included
}
