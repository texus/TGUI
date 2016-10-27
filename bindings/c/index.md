---
layout: page
title: C binding
breadcrumb: c
---
This page contains information about the C binding.  
The main goal of the C binding was to more easily allow creating bindings to other languages, however it is still usable for C programmers. There are currently no tutorials or documentation available for this binding.

Please note that both the binding and TGUI 0.8 are still under development and may change at any time.


### Download and install

{% include button.ext text="Download source code" link="https://github.com/texus/CTGUI/archive/master.zip" style="Orange" %}

You need to have SFML and CSFML 2.4 installed.

You will have to use CMake in order to build the c library. A tutorial with screenshots will be written in the future.

Precompiled libraries will be added in the future as well.


### Example code
``` c++
#include <CTGUI/CTGUI.h>
#include <assert.h>
#include <stdio.h>

void func()
{
    printf("Button clicked\n");
}

void main()
{
    sfVideoMode videoMode = {400, 300, 32};
    sfRenderWindow* window = sfRenderWindow_create(videoMode, "CTGUI example", sfDefaultStyle, NULL);

    tguiGui* gui = tguiGui_createFromWindow(window);

    tguiWidget* button = tguiButton_create();
    tguiGui_add(gui, button, U"MyButton");

    tguiButton_setText(button, U"Hello");

    sfVector2f position = {40, 30};
    tguiWidget_setPosition(button, position);

    sfVector2f size = {200, 40};
    tguiWidget_setSize(button, size);

    const char* error;
    tguiWidget_connect(button, "pressed", func, &error);
    assert(error == NULL);

    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            tguiGui_handleEvent(gui, event);
        }

        sfRenderWindow_clear(window, sfBlack);
        tguiGui_draw(gui);
        sfRenderWindow_display(window);
    }

    tguiWidget_destroy(button);
    tguiGui_destroy(gui);
    sfRenderWindow_destroy(window);
}
```
