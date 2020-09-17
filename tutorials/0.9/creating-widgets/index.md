---
layout: page
title: Creating, copying and removing widgets
breadcrumb: creating widgets
---

### Creating widgets
Creating a widget goes in two steps: creating it and adding it to its parent.

Every widget provides a static create function that you can use. For some widgets the function has optional parameters (e.g. a filename for Picture or a text for Button) but you can always call the create function without parameters and set these properties later.
```c++
tgui::Button::Ptr button = tgui::Button::create();
auto editBox = tgui::EditBox::create();
```

The second step is to add the widget to its parent. Usually this is the Gui object, but it could also be e.g. a child window. The add function can take an optional name as second parameter which can be used to retrieve the widget from the parent again later, in case you don't store the widget yourself.
```c++
gui.add(button);
gui.add(editBox, "MyWidgetName");
childWindow->add(checkBox);
```

The widgets will by default make use of the build-in "White" theme. No external resources are needed for them and you have something on your screen immediately. Although you can customize the widgets by changing property by property, you probably want to [use themes](../using-themes/) to give your widgets a different look. TGUI already ships with some theme files that you can load and use out of the box.


### Retrieving widgets
Once a widget is added to a parent widget (or directly to the gui), you no longer have to store it yourself, you can just let your pointer go out of scope. When you need it later you can always get it back from the parent widget. The name that the get function takes as parameter was the optional parameter of the add function.
```c++
tgui::EditBox::Ptr editBox = gui.get<tgui::EditBox>("MyWidgetName");
tgui::Widget::Ptr widget = gui.get("MyWidgetName");
```


### Copying widgets
When you know the type of your widget then you can simply use the copy function. The copied widget is however not bound to the same parent widget, you must add it again. This is done so that you can e.g. make a copy of a widget inside a child window and then add the copy to another child window.
```c++
tgui::Button::Ptr newButton = tgui::Button::copy(oldButton);
gui.add(newButton);
```

It is of course possible that you do not know the type of the widget. If you have been storing all the widgets in a list and you want to copy the list then you cannot use the copy function. In this situation you must use the clone function instead.
```c++
std::vector<tgui::Widget::Ptr> newList;
for (const auto& widget : oldList)
    newList.push_back(widget->clone());
```


### Removing widgets
Once a widget is no longer needed, you can remove it from its parent container.
```c++
gui.remove(button);
```

You could also remove all widgets inside a container at once.
```c++
gui.removeAllWidgets();
```
