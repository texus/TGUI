#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/raylib.hpp>
#include <emscripten/emscripten.h>
#include <iostream>

EM_JS(int, canvas_get_width, (), { return canvas.width; });
EM_JS(int, canvas_get_height, (), { return canvas.height; });

std::unique_ptr<tgui::Gui> gui = nullptr;

void init()
{
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(800, 600, "TGUI example (RAYLIB)");
    SetTargetFPS(30);

    gui = std::make_unique<tgui::Gui>();

    auto picture = tgui::Picture::create("data/background.jpg");
    picture->setSize({400, 300});
    gui->add(picture);

    auto button = tgui::Button::create("Hello world");
    button->setPosition({20, 10});
    button->setSize({200, 30});
    button->onClick([] { std::cout << "Button clicked\n"; });
    gui->add(button);
}

void render()
{
    BeginDrawing();
    ClearBackground({240, 240, 240, 255});
    gui->draw();
    EndDrawing();
}

void mainLoop()
{
    gui->handleEvents();

    while (int pressedChar = GetCharPressed())
        gui->handleCharPressed(pressedChar);

    while (int pressedKey = GetKeyPressed())
        gui->handleKeyPressed(pressedKey);

    render();
}

int main(int, char**)
{
    init();

    emscripten_set_main_loop(mainLoop, -1, true);

    gui = nullptr;
    CloseWindow();
    return 0;
}
