---
layout: page
title: Using Themes/skins
breadcrumb: themes
---
<div>
  <h3 id="using-themes">Using themes</h3>
  <p>The Theme class is an easy interface to load and store the <a href="../renderers">renderers</a>. The renderers will be what actually defines how the the widgets will look, the theme is just responsible for loading and storing them.</p>

  <p class="SmallBottomMargin">By default, the theme will load renderers from a file which you can specify either when constructing the instance or later on by calling the load function.</p>
{% highlight c++ %}
Theme blackTheme{"TGUI/themes/Black.txt"};

Theme greyTheme;
greyTheme.load("TGUI/themes/TransparentGrey.txt");
{% endhighlight %}

  <p>Technically, it is actually an internal DefaultThemeLoader that handles the loading and you could <a href="../custom-theme-loader">specify a custom theme loader</a>, but that is not something the average user will need to worry about.</p>

  <p class="SmallBottomMargin">After the file has been parsed, you can access the renderers that were loaded by their id, which is their name in the loaded file. In the themes that ship with TGUI, the name of the renderer corresponds with the name of the widget. So if you want to use the black skin for your edit box instead of the default white one then just add the following line to your code:</p>
{% highlight c++ %}
editBox->setRenderer(blackTheme.getRenderer("EditBox"));
{% endhighlight %}

  <p>Note that the names are always case-insensitive, it won't make a difference if you pass "editbox", "EditBox" or "EDITBOX" to the getRenderer function.</p>
</div>

<div>
  <h3 id="adding-and-removing-renderers-manually">Adding and removing renderers manually</h3>
  <p class="SmallBottomMargin">The load function will add renderers to the theme, but you can also add renderers to the theme yourself:</p>
{% highlight c++ %}
theme.addRenderer("RendererName", widget->getRenderer());
{% endhighlight %}

  <p class="SmallBottomMargin">In a similar fashion you can remove renderers from a theme:</p>
{% highlight c++ %}
theme.removeRenderer("RendererName");
{% endhighlight %}
</div>

<div>
  <h3 id="reloading-themes">Reloading themes</h3>
  <p class="SmallBottomMargin">One cool thing that you can do with themes is reloading all widgets that are using renderers from that theme. Say for instance that you have loaded all widgets with the Black skin and you want all of these widgets to use the BabyBlue skin. This is as simple as calling the load function again:</p>
{% highlight c++ %}
Theme theme{"TGUI/themes/Black.txt"};
button->setRenderer(theme.getRenderer("Button")); // Button changes from White to Black skin
theme.load("TGUI/themes/BabyBlue.txt"); // Button changes from Black to BabyBlue skin
{% endhighlight %}
</div>
