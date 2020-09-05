---
layout: page
title: Minimal code with SFML
breadcrumb: minimal code with SFML
---

The following is the minimal code you need for an application using TGUI with SFML. Each part of the code and some alternatives are explained below.
{% raw %}
```c++
#include <TGUI/TGUI.hpp>

int main()
{
    sf::RenderWindow window{{800, 600}, "TGUI window with SFML"};
    tgui::GuiSFML gui{window};
    gui.mainLoop();
}
```
{% endraw %}


### Including TGUI

TGUI provides a single header that will include everything you need. This is the easiest way to include TGUI:
```c++
#include <TGUI/TGUI.hpp>
```

Alternatively, you can selectively include what you need. You will always need `TGUI/Core.hpp` and `TGUI/Backends/SFML.hpp`, but widgets can be included individually. There is also a `TGUI/AllWidgets.hpp` header that includes the headers for all of the widgets.
```c++
#include <TGUI/Core.hpp>
#include <TGUI/Backends/SFML.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
```


### Creating the gui object

Only one gui object should be created for each SFML window. The gui needs to know which window to render to, so it takes a reference to the window as parameter to the constructor:
```c++
tgui::GuiSFML gui{window};
```

The gui also has a default constructor. If you use it then you will however need to call setTarget before interacting with the gui object.
```c++
tgui::GuiSFML gui;
gui.setTarget(window);
```


### Main loop

Although TGUI provides a `gui.mainLoop()` function for convenience, in many cases you will need to use your own main loop (e.g. if you need to draw things other than the gui or run code that isn't event-based).

A minimal main loop would look like this:
```c++
while (window.isOpen())
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        gui.handleEvent(event);

        if (event.type == sf::Event::Closed)
            window.close();
    }

    window.clear();
    gui.draw();
    window.display();
}
```

In your event loop, `gui.handleEvent(event)` is used to inform the gui about the event. The gui will make sure that the event ends up at the widget that needs it. If all widgets ignored the event then `handleEvent` will return `false`. This could be used to e.g. check if a mouse event was handled by the gui or should still be handled by your own code.

To draw all widgets in the gui, you need to call `gui.draw()` once per frame. All widgets are drawn at once, if you need to have SFML rendering between 2 widgets (e.g. drawing something on the background of a child window) then you should have a look at the Canvas widget.


### Object lifetimes

TGUI relies on some global cleanup code that has to be finished before the end of the main function. By default, the initialization and destruction is linked to the gui object. When the last gui is destroyed, all global states are destroyed as well.

If for some reason you need to execute TGUI code while no gui object exist (before it is created or after it has already been destroyed), then you will need to manage the construction and destruction of the backend manually. The backend would have to be created before any TGUI objects are created and `setBackend(nullptr)` has to be called after all your TGUI objects have been destroyed.
```c++
int main()
{
    tgui::setBackend(std::make_shared<tgui::BackendSFML>());
    run_application();
    tgui::setBackend(nullptr);
}
```
