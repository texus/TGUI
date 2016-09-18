---
layout: page
title: Unbound optional parameters
breadcrumb: signals optional parameters
---

<p>In the <a href="../signals-introduction/">introduction to signals</a> you have seen how to connect functions with parameters that you provide. However, sometimes you want your function to have parameters which are filled with information about the widget (e.g. on which coordinate the click occured). You can do this with what I call the "optional parameter system".</p>

<p><span class="Red">Visual Studio users need at least VS2015 Update 2 to use this code.</span> If you are using VS2013 (or a non-updated VS2015) then you must use the deprecated <a href="../signals-connectEx/">connectEx function</a> to handle similar tasks.</p>

<div>
<h3 id="the-optional-parameter-system">The optional parameter system</h3>
<p class="SmallBottomMargin">In the introduction you have seen the following code:</p>
{% highlight c++ %}
void function();
button->connect("pressed", function);
{% endhighlight %}

<p class="SmallBottomMargin">If your compiler supports this optional parameter system, the function can actually have a parameter. The "pressed" signal also sends the text of the button with it. The 'function' could actually be defined as any of the following, while the call to the 'connect' function remains identical.</p>
{% highlight c++ %}
void function();
void function(std::string);
void function(const std::string&);
void function(sf::String);
void function(const sf::String&);
{% endhighlight %}

<p>As you can see there is some variety of what type this string parameter has. The signal sends a "const sf::String&", but you can use any type to which this can be implicitly converted.</p>

<p class="SmallBottomMargin">Let's look at another example. The "checked" and "unchecked" signals from Checkbox will pass a boolean as parameter. This parameter is always true for the "checked" signal and always false for the "unchecked" signal which allows the following code:</p>
{% highlight c++ %}
void function(bool checked)
{
    std::cout << "Checked? " << std::boolalpha << checked << std::endl;
}
checkbox->connect("checked unchecked", function);
{% endhighlight %}

<p class="SmallBottomMargin">What about the bound parameters?<br>
  This system is all about unbound parameters. If your function takes one parameter, and you provide one extra parameter to the connect function, then that parameter is bound like shown in the introduction. However, if your function takes two parameters then one parameter is unbound and the system will try to bind it with information about the signal. Here the function will be called with a reference to the Gui object that you provided and the coordinate of the mouse click which the system provided.</p>
{% highlight c++ %}
void function(tgui::Gui& gui, sf::Vector2f mousePos);
picture->connect("clicked", function, std::ref(gui));
{% endhighlight %}

<p>If you are wondering which optional parameters the system can provide for the signal, you should check the documentation of the widget. In the future I will likely create a page that list all widgets and their signals, but for now you will just have to put the pieces together by looking at the documentation.</p>
</div>
