---
layout: page
title: Layouts
breadcrumb: layouts
---

<div>
<h3 id="bind-functions">Bind functions</h3>
<p>In the <a href="../basic-widget-functions/">basic widget functions tutorial</a> I showed how you normally use the setPosition and setSize function. But they can do way more than just take constants.</p>

<p class="SmallBottomMargin">The best way to explain is with an example. In the code below you not only give button2 the same size as button1, but when button1 changes size, the size of button2 will automatically be updated as well.</p>
{% highlight c++ %}
button1->setSize(50, 10);
button2->setSize(tgui::bindSize(button1));

button1->setSize(100, 20);
assert(button2->getSize() == sf::Vector2f{100, 20});
{% endhighlight %}

<p class="SmallBottomMargin">There are actually a lot of bind functions: bindLeft, bindTop, bindRight, bindBottom, bindPosition, bindWidth, bindHeight, bindSize, bindMin, bindMax and bindIf.</p>

<p>You can also use all overloadable operators (+, -, *, /, %, ==, !=, <, >, <=, >=, && and ||) to combine the layouts that these bind functions returns.</p>

<p class="SmallBottomMargin">The following code will keep a square picture centered in the window, keep its square ratio and make it as large as possible while still fitting inside the windows.</p>
{% highlight c++ %}
auto width = bindWidth(gui);
auto height = bindHeight(gui);
pic->setSize(bindIf(width > height, {height, height}, {width, width}));
pic->setPosition(bindIf(width > height, {(width - bindWidth(pic)) / 2, 0}, {0, (height - bindHeight(pic)) / 2}));
{% endhighlight %}

<p>Note that when binding the Gui object, the size of its view is used as size, not the size of the window.</p>
</div>

<div>
<h3 id="layouts-in-a-string">Layouts in a string</h3>
<p class="SmallBottomMargin">Layouts can actually be created without access to the widget that you want to bind. Instead of using the bind functions you can also pass a string. Although slightly slower, this is probably a much nicer way to use the layouts.</p>
{% highlight c++ %}
button->setSize("0.6 * parent.width", "0.2 * parent.height");
button->setSize("{0.6 * parent.width, 0.2 * parent.height}");
{% endhighlight %}

<p class="SmallBottomMargin">Every widget has the following properties available: size, width, height, pos, left, top, right and bottom. You can access the parent widget by adding "parent." in front of it. You can also access sibling or child widgets directly by their name (which is the optional name passed to the add function when adding them to the parent). And you can even nest the parent access.</p>
{% highlight c++ %}
button2->setSize("button1.size");
button2->setPosition("parent.parent.MyButton.position");
{% endhighlight %}

<p class="SmallBottomMargin">There are also the min and max functions. The min and max are even more powerful than the bind version as they can take an arbitrary amount of parameters.</p>
{% highlight c++ %}
widget->setPosition("min(a.x, b.x, c.x, d.x)", "max(a.y, b.y)");
{% endhighlight %}

<p class="SmallBottomMargin">Finally there is the if-then-else statement. Let's use it in the example with the picture:</p>
{% highlight c++ %}
pic->setSize("if parent.width > parent.height then {parent.height, parent.height} else {parent.width, parent.width}");
pic->setPosition("if parent.width > parent.height then {(parent.width - width) / 2, 0} else {0, (parent.height - height) / 2}");
{% endhighlight %}

<p class="SmallBottomMargin">Those lines are just too long, let's make the final version a bit shorter. Parent can be written as an &amp; and for left, top, width and height we can use x, y, w and h. And then there is also the ?: operator.</p>
{% highlight c++ %}
pic->setSize("&.w > &.h ? {&.h, &.h} : {&.w, &.w}");
pic->setPosition("&.w > &.h ? {(&.w - w) / 2, 0} : {0, (&.h - h) / 2}");
{% endhighlight %}

<p>That's it. It won't get any shorter than that, unless you start dropping whitespace.</p>
</div>

<div>
<h3 id="simple-example">Simple example: centering a widget</h3>
<p class="SmallBottomMargin">The example with the picture was probably not realistic and was just to demonstrate the power of the layout system. A more useful example is for instance centering a widget. Thanks to the layout system this is extremely easy:</p>
{% highlight c++ %}
widget->setPosition("(parent.width - width) / 2");
{% endhighlight %}
</div>

<div>
<h3 id="final-warning">Final warning</h3>
<p class="SmallBottomMargin">Layouts are not normal objects, they should only be used as temporary objects. You can store them to not repeat an entire expression on multiple places, but you should best not change an existing layout. If you do have to change them, just make sure that the assigned layout does not appear on the right side of the assignment as well!</p>
{% highlight c++ %}
Layout x = 5;
x = x + 1; // Mathematically incorrect => may crash
Layout y = x + 1; // No problem
{% endhighlight %}
</div>
