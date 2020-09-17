---
layout: page
title: HorizontalLayout and VerticalLayout
breadcrumb: box layouts
---

First of all, these box layout classes discussed here have no relation with the [layout](../layouts) system. The HorizontalLayout and VerticalLayout classes exist so that you can easily place widgets below or next each other without having to give them exact positions or sizes. There is also a HorizontalWrap widget to place widgets next to each other but starts a new row when the width gets too high.

Secondly, these box layout classes are intended for multiple widgets of the same type and size. They are simply not meant for positioning various widgets of different sizes.

You start by creating the layout object and give it the size that you want the widgets to fill. Here the widgets will cover the entire width while having a height of 10% of the window.
```c++
auto layout = tgui::HorizontalLayout::create();
layout->setSize("100%", "10%");
layout->setPosition(0, 100);
gui.add(layout);
```

Let's add some widgets to the currently empty layout.
```c++
auto button1 = tgui::Button::create();
button1->setText("First");
layout->add(button1);

auto button2 = tgui::Button::create();
button2->setText("Second");
layout->add(button2);
```

At this point you have two buttons each taking 50% of the width of the window and having a height of 10% of the window. Let's put some space between them, but we want the space to be smaller than the actual buttons. Each widget and space have a certain ratio, by default this ratio is 1. To calculate the size of the widget, the ratio is compared with the sum of all ratios. It is easy with an example, we already have two buttons with ratio 1 and we now add a space with ratio 0.5 which brings the total to 2.5. This means that the buttons will each take 40% (1 / 2.5) of the width while the space takes the remaining 20% (0.5 / 2.5). The first parameter to insertSpace is the index, which is 1 because we want to insert it between the two buttons.
```c++
layout->insertSpace(1, 0.5f);
```
