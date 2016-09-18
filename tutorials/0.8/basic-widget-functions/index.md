---
layout: page
title: Basic widget functions
breadcrumb: "basic widget functions"
---
This tutorial is about the functions that you find in the Widget base class. These functions are available for all widgets.

<h3 id="position-and-size">Position and size</h3>
<p class="SmallBottomMargin">Every widget can be given a certain position and size.</p>
{% highlight c++ %}
widget->setPosition(50, 50);
sf::Vector2f position = widget->getPosition();

widget->setSize(200, 50);
sf::Vector2f size = widget->getSize();
{% endhighlight %}

<p>The position is relative to the parent. When a widget is added to the Gui then this does not matter, but when the widget is added to a child window then the position of the widget is relative to that child window. If you really need to know the exact position the widget is located then you can use the getAbsolutePosition() function.</p>

<p>Some widgets are larger than their size. Let's take a slider as an example. The size respresents the size of its track, but the thumb might be outside of this area. The getWidgetOffset() function returns the negative offset that the position of the thumb has. The getFullSize() function will return the entire size of the widget (in case of the slider it will include the area where the thumb could be).</p>

<p>Instead of constants, the setPosition and setSize functions can also be given <a href="../layouts/">layouts</a> as argument so that you can give your widget a relative position or size. You can tell widget B to have the same size as widget A and widget B will automatically change size when you give widget A a new size.</p>

### Visible and enabled
The enabled property determines if a widget receives events. If a widget is disabled, it will still be drawn but it will be unresponsive to your mouse clicks.

The visibility property determines if a widget is drawn. A hidden widget also no longer receives events, it is no longer there. The only way to have an invisible yet still functioning widget is to use ClickableWidget. Hiding and showing widgets is similar to adding and removing them, its just easier and faster. It can be very handy to put different screens into different Panel widgets and just hide all the panels except for one.

Every widget is visible and enabled by default.

``` c++
widget->hide();
widget->show();
widget->isVisible();

widget->enable();
widget->disable();
widget->isEnabled();
```

### Overlapping widgets
What happens when widgets overlap? Which is the topmost widget? The answer is simple: the last widget that you added to the parent. You can manipulate the order in which widgets are drawn by using the moveToFront and moveToBack functions though.

``` c++
widget->moveToFront();
widget->moveToBack();
```
