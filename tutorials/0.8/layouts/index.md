---
layout: page
title: Layouts
breadcrumb: layouts
redirect_from: "/tutorials/v07/layouts/index.html"
---

<div>
<h3 id="relative-to-parent">Relative to parent</h3>
<p class="SmallBottomMargin">In the <a href="../basic-widget-functions/">basic widget functions</a> I showed that setPosition and setSize could be given constants or a value relative to the size of the parent widget.</p>
{% highlight c++ %}
widget->setPosition("10%", "5%");
widget->setSize("30%", "10%");
{% endhighlight %}

<p class="SmallBottomMargin">Relative positions can be useful, but is not enough to e.g. center a widget as "50%" would put the top left of the widget in the middle. The layout system allows referring to other widgets, so by using the size of the parent and the size of the widget itself it is easy to center a widget on the screen:</p>
{% highlight c++ %}
widget->setPosition("(parent.size - size) / 2");
{% endhighlight %}

<p class="SmallBottomMargin">The "parent" can be shortened to "&". With the following line the center of the widget is horizontally placed at 25% of the parent width while vertically positioned at the bottom of its parent.</p>
{% highlight c++ %}
widget->setPosition({"&.width / 4 - width / 2", "&.height - height"});
{% endhighlight %}

The layout system will make sure that when the size of the parent changes, the position and size of the widget will be updated as well.
</div>

<div>
<h3 id="binding-other-widgets">Binding other widgets</h3>
<p class="SmallBottomMargin">You can not only refer to the parent, but also to other widgets. In the example below, the button is positioned 50 pixels to the right of another button. The name used in the layout string has to match with the one given to the add function when the widget was added to the mutual parent.</p>
{% highlight c++ %}
group->add(button1, "ButtonName");
group->add(button2);
button2->setPosition({"ButtonName.right + 50", "ButtonName.top"});
{% endhighlight %}

<p class="SmallBottomMargin">There are also bind functions to refer to widgets directly without them needing a name or being added to any parent yet.</p>
{% highlight c++ %}
button2->setSize(bindSize(button1));
button2->setPosition({bindRight(button1) + 50, bindTop(button1)});
{% endhighlight %}
</div>
