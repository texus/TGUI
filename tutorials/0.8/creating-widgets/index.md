---
layout: page
title: Creating, copying and removing widgets
breadcrumb: creating widgets
---
<div>
<h3 id="creating-widgets">Creating widgets</h3>
<p>Creating a widget goes in two steps: creating it and adding it to its parent.</p>
<p class="SmallBottomMargin">Every widget provides a static create function that you can use. For some widgets the function has optional parameters (e.g. a filename for Picture or a text for Button) but you can always call the create function without parameters and set these properties later.</p>
{% highlight c++ %}
tgui::Button::Ptr button = tgui::Button::create();
tgui::EditBox::Ptr editBox = tgui::EditBox::create();
{% endhighlight %}

<p class="SmallBottomMargin">The second step is to add the widget to its parent. Usually this is the Gui object, but it could also be e.g. a child window. The add function can take an optional name as second parameter which can be used to retrieve the widget from the parent again later, in case you don't store the widget yourself.</p>
{% highlight c++ %}
gui.add(button);
gui.add(editBox, "MyWidgetName");
{% endhighlight %}

<p>The widgets will by default make use of the build-in "White" theme. No external resources are needed for them and you have something on your screen immediately. Although you can customize the widgets by changing property by property, you probably want to <a href="../using-themes/">use themes</a> to give your widgets a different look. TGUI already ships with some theme files that you can load and use out of the box.</p>
</div>

<div>
<h3 id="retrieving-widgets">Retrieving widgets</h3>
<p class="SmallBottomMargin">Once a widget is added to a parent widget (or directly to the gui), you no longer have to store it yourself, you can just let your pointer go out of scope. When you need it later you can always get it back from the parent widget. The name that the get function takes as parameter was the optional parameter of the add function.
</p>
{% highlight c++ %}
tgui::EditBox::Ptr editBox = gui.get<tgui::EditBox>("MyWidgetName");
tgui::Widget::Ptr widget = gui.get("MyWidgetName");
{% endhighlight %}
</div>

<div>
<h3 id="copying-widgets">Copying widgets</h3>
<p class="SmallBottomMargin">When you know the type of your widget then you can simply use the copy function. The copied widget is however not bound to the same parent widget, you must add it again. This is done so that you can e.g. make a copy of a widget inside a child window and then add the copy to another child window.
</p>
{% highlight c++ %}
tgui::Button::Ptr newButton = tgui::Button::copy(oldButton);
gui.add(newButton);
{% endhighlight %}

<p class="SmallBottomMargin">It is of course possible that you do not know the type of the widget. If you have been storing all the widgets in a list and you want to copy the list then you cannot use the copy function. In this situation you must use the clone function instead.
</p>
{% highlight c++ %}
std::vector<tgui::Widget::Ptr> newList;
for (const auto& widget : oldList)
    newList.push_back(widget->clone());
{% endhighlight %}
</div>

<div>
<h3 id="removing-widgets">Removing widgets</h3>
<p class="SmallBottomMargin">Once a widget is no longer needed, you can remove it from its parent container.</p>
{% highlight c++ %}
gui.remove(button);
{% endhighlight %}

<p class="SmallBottomMargin">You could also remove all widgets inside a container at once.</p>
{% highlight c++ %}
gui.removeAllWidgets();
{% endhighlight %}
</div>
