---
layout: page
title: Minimal code
breadcrumb: minimal code
---

There are four things you need to change in your code to get TGUI working.

The first thing to do is including it. TGUI will include "SFML/Graphics.hpp" for you.
```c++
#include <TGUI/TGUI.hpp>
```

The next thing to do is creating the Gui object.
{% raw %}
```c++
sf::RenderWindow window{{800, 600}, "Window"};
tgui::Gui gui{window};
```
{% endraw %}

When you will have widgets, they will need to receive events. Otherwise they wouldn’t know if your mouse is on top of them or not. Every time you receive an event, you will have to tell the gui about this event. So in your event loop you must add the following line:
```c++
gui.handleEvent(event);
```

The last thing to do is drawing the widgets on the screen. For this you must call the draw function of the gui.
```c++
gui.draw();
```

So the full code will look like this:
{% raw %}
```c++
#include <TGUI/TGUI.hpp>

int main()
{
    sf::RenderWindow window{{800, 600}, "Window"};
    tgui::Gui gui{window}; // Create the gui and attach it to the window

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event); // Pass the event to the widgets
        }

        window.clear();
        gui.draw(); // Draw all widgets
        window.display();
    }
}
```
{% endraw %}
