---
layout: page
title: Layouts
breadcrumb: layouts
redirect_from: "/tutorials/v07/layouts/index.html"
---

### Relative to parent
In the [basic widget functions](../basic-widget-functions/) I showed that setPosition and setSize could be given constants or a value relative to the size of the parent widget.
```c++
widget->setPosition("10%", "5%");
widget->setSize("30%", "10%");
```

Relative positions can be useful, but is not enough to e.g. center a widget as "50%" would put the top left of the widget in the middle. The layout system allows referring to other widgets, so by using the size of the parent and the size of the widget itself it is easy to center a widget on the screen:
```c++
widget->setPosition("(parent.size - size) / 2");
```

The "parent" can be shortened to "&". With the following line the center of the widget is horizontally placed at 25% of the parent width while vertically positioned at the bottom of its parent.
```c++
widget->setPosition({"&.width / 4 - width / 2", "&.height - height"});
```

The layout system will make sure that when the size of the parent changes, the position and size of the widget will be updated as well.


### Binding other widgets
You can not only refer to the parent, but also to other widgets. In the example below, the button is positioned 50 pixels to the right of another button. The name used in the layout string has to match with the one given to the add function when the widget was added to the mutual parent.
```c++
group->add(button1, "ButtonName");
group->add(button2);
button2->setPosition({"ButtonName.right + 50", "ButtonName.top"});
```

There are also bind functions to refer to widgets directly without them needing a name or being added to any parent yet.
```c++
button2->setSize(bindSize(button1));
button2->setPosition({bindRight(button1) + 50, bindTop(button1)});
```

### Minimum / maximum
Expressions can also contain min or max to select the minimum or maximum of sub-expressions. The following example centers a picture in the window, filling as much space as possible while keeping the entire picture visible and maintaining its 4:3 ratio.
```c++
pic->setPosition("(&.size - size) / 2");
pic->setSize({"min(&.w, &.h * 4/3)", "min(&.h, &.w * 3/4)"});
```
