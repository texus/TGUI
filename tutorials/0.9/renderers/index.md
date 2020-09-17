---
layout: page
title: Renderers
breadcrumb: renderers
---

### Accessing renderer properties
Every widget has a renderer which contains the properties of how the widget should look (e.g. background and text color).

The widgets all have a getRenderer() method which will return a pointer to the renderer. The renderer returned by this function will have setters and getters for the properties available for that particular type of widget (i.e. the renderer of a list box has different functions than the one from a button).
```c++
auto slider = tgui::Slider::create();
slider->getRenderer()->setTrackColor(sf::Color::Green);
sf::Color thumbColor = slider->getRenderer()->getThumbColor();
```

You can use the [documentation](/documentation/0.9/) to find which renderer properties exist. For the renderer properties of a widget of type Slider, you would have to look up the SliderRenderer class.

### Sharing renderers
Renderers (which typically come from [themes](../using-themes)) are shared by default. If the theme (or the renderer which represents the theme) is changed, all widgets using the renderer will be updated. When `widget->getRenderer()` is called, the widget will however switch to a local copy of the renderer. If you want to access the renderer that is shared between the widgets then you must use `widget->getSharedRenderer()` instead.
```c++
Theme theme{"TGUI/themes/Black.txt"};
button1->setRenderer(theme.getRenderer("Button"));
button2->setRenderer(theme.getRenderer("Button"));

button1->getSharedRenderer()->setBackgroundColor(sf::Color::Red); // Changes button2 too
button1->getRenderer()->setBackgroundColor(sf::Color::Green); // Does not affect button2
button1->getSharedRenderer()->setBackgroundColor(sf::Color::Blue); // Does not affect button2, relation was broken in previous line
```
