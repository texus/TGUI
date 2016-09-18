---
layout: page
title: Renderers
breadcrumb: renderers
---
<div>
  <h3 id="accessing-renderer-properties">Accessing renderer properties</h3>
  <p>Every widget has a renderer which contains the properties of how the widget should look (e.g. background and text color). When creating a widget, such a renderer is automatically created with it.</p>

  <p class="SmallBottomMargin">The widgets all have a getRenderer() method which will return a pointer to the renderer. The renderer returned by this function will have setters and getters for the properties available for that particular type of widget (i.e. the renderer of a list box has different functions than the one from a button).</p>
{% highlight c++ %}
auto slider = tgui::Slider::create();
slider->getRenderer()->setTrackColor(sf::Color::Green);
sf::Color thumbColor = slider->getRenderer()->getThumbColor();
{% endhighlight %}
</div>

<div>
  <h3 id="sharing-renderer">Sharing renderer</h3>
  <p class="SmallBottomMargin">When not calling the setRenderer function, each widget will have its own renderer. You can however set the same renderer in multiple widgets so that changing the renderer changes all these widgets. Renderers that you get from <a href="../using-themes">themes</a> are always shared.</p>
{% highlight c++ %}
Theme theme{"TGUI/widgets/Black.txt"};
button1->setRenderer(theme.getRenderer("Button"));
button2->setRenderer(theme.getRenderer("Button"));
button3->setRenderer(button2->getRenderer());

button3->getRenderer()->setBackgroundColor(sf::Color::Red); // Changes button1 and button2 as well
{% endhighlight %}

  <p class="SmallBottomMargin">If you want to copy a renderer and not have other widgets be affected when you change it then you should clone the renderer:</p>
{% highlight c++ %}
button3->setRenderer(button2->getRenderer()->clone());

button3->getRenderer()->setBackgroundColor(sf::Color::Red); // Does not affect other buttons
{% endhighlight %}
</div>
