---
layout: page
title: Using Themes/skins
breadcrumb: themes
---

### Using themes
The Theme class is an easy interface to load and store the [renderers](../renderers). The renderers will be what actually defines how the the widgets will look, the theme is just responsible for loading and storing them.

By default, the theme will load renderers from a file which you can specify either when constructing the instance or later on by calling the load function.
```c++
tgui::Theme blackTheme{"TGUI/themes/Black.txt"};

tgui::Theme greyTheme;
greyTheme.load("TGUI/themes/TransparentGrey.txt");
```

After the file has been parsed, you can access the renderers that were loaded by their id, which is their name in the loaded file. In the themes that ship with TGUI, the name of the renderer corresponds with the name of the widget. So if you want to use the black skin for your edit box instead of the default white one then just add the following line to your code:
```c++
editBox->setRenderer(blackTheme.getRenderer("EditBox"));
```

Note that the names are always case-sensitive, you have to pass "EditBox" and NOT "editbox" to the getRenderer function.

### Adding and removing renderers manually
The load function will add renderers to the theme, but you can also add renderers to the theme yourself:
```c++
theme.addRenderer("RendererName", widget->getRenderer());
```

In a similar fashion you can remove renderers from a theme:
```c++
theme.removeRenderer("RendererName");
```

### Default theme
Instead of calling `widget->setRenderer(...)` on each widget, it is possible to set a default theme. The setDefault function can be passed a filename or a shared_ptr to an existing theme.
```c++
tgui::Theme::setDefault("TGUI/themes/Black.txt");
auto w1 = tgui::CheckBox::create();  // Uses the "CheckBox" section from the Black theme
auto w2 = tgui::ListBox::create();  // Uses the "ListBox" section from the Black theme
```

To reset the default theme to the default White theme, you can call the function with a nullptr as parameter.
```c++
tgui::Theme::setDefault(nullptr);
auto w3 = tgui::EditBox::create(); // Uses the "EditBox" section of the built-in White theme
```

Widgets will always load the section with their type as name (and will fall back to the White theme if no such section exists). If you want sections such as "GreenButton" and "BlueButton" then you have to set the renderer yourself with `widget->setRenderer(...)`.

### Changing renderer properties

All widgets loaded with the same theme share the same renderer (until `widget->getRenderer()` is called as shown in the [renderers tutorial](../renderers)).

This allows changing the looks of all these widgets at once. The code below will change the background color of all buttons loaded with the theme:
```c++
tgui::ButtonRenderer(theme.getRenderer("Button")).setBackgroundColor(sf::Color::Blue);
```

### Reloading themes
You can not only change the renderers property by property, but you can replace them all at once with a single instruction. For example, you have loaded all widgets with the Black skin and you want all of these widgets to use the BabyBlue skin. This is as simple as calling the load function again:
```c++
Theme theme{"TGUI/themes/Black.txt"};
button->setRenderer(theme.getRenderer("Button")); // Button changes from White to Black skin
theme.load("TGUI/themes/BabyBlue.txt"); // Button changes from Black to BabyBlue skin
```
