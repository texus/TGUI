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

Note that the names are always case-insensitive, it won't make a difference if you pass "editbox", "EditBox" or "EDITBOX" to the getRenderer function.

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
Instead of calling `widget->setRenderer(...)` on each widget, it is possible to set a default theme. The default theme uses a pointer, so you must keep the theme instance alive. If the theme passed to `setDefault` does get destructed, the default theme will automatically be reset.
```c++
{
    tgui::Theme theme{"TGUI/themes/Black.txt"};
    tgui::Theme::setDefault(&theme);
    auto w1 = tgui::CheckBox::create();  // Uses the "checkbox" section from the Black theme
    auto w2 = tgui::ListBox::create();  // Uses the "listbox" section from the Black theme
}
auto w3 = tgui::EditBox::create(); // Uses the "editbox" section of the built-in White theme
```

### Changing renderer properties

All widgets loaded with the same theme share the same renderer (until `widget->getRenderer()` is called as shown in the [renderers tutorial](../renderers)).

This allows changing the looks of all these widgets at once. The code below will change the background color of all buttons loaded with the theme:
```c++
tgui::ButtonRenderer(theme.getRenderer("button")).setBackgroundColor(sf::Color::Blue);
```

### Reloading themes
You can not only change the renderers property by property, but you can replace them all at once with a single instruction. For example, you have loaded all widgets with the Black skin and you want all of these widgets to use the BabyBlue skin. This is as simple as calling the load function again:
```c++
Theme theme{"TGUI/themes/Black.txt"};
button->setRenderer(theme.getRenderer("Button")); // Button changes from White to Black skin
theme.load("TGUI/themes/BabyBlue.txt"); // Button changes from Black to BabyBlue skin
```
