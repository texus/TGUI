---
layout: page
title: "Example: SFML backend"
breadcrumb: "sfml backend"
---

The code below shows the template that is used for the other examples when using the SFML backend.

The runExample function is present in all example codes and contains the code that is independent of the backend. The rest of the code depends on SFML and will thus only be shown here and not repeated in each example.

More information about the minimal code can be found in the [SFML backend tutorial](/tutorials/0.9/sfml-backend/).

``` c++
#include <TGUI/TGUI.hpp>

bool runExample(tgui::GuiBase& gui)
{
    return true;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI window");

    tgui::Gui gui(window);
    if (!runExample(gui))
        return EXIT_FAILURE;

    gui.mainLoop();
    return EXIT_SUCCESS;
}
```
