---
layout: page
title: Introduction to signals
breadcrumb: signals intro
---
<div>
<h3 id="connecting-to-signals">Connecting to signals</h3>
<p>When something happens, widgets can send out a signal (e.g. a button has a "pressed" signal and a checkbox has "checked" and "unchecked" signals). You can bind a signal handler to such a signal with the connect function. The first parameter is the name of the signal, this is the trigger on which the callback should be send. This name is case-insensitive, so it does not matter if you write "pressed" or "Pressed". The second parameter is the function that you want to be called when the signal is send.</p>
<p class="SmallBottomMargin">Here is an example of letting a function get called when a button has been pressed.</p>
{% highlight c++ %}
void function()
{
    std::cout << "Button pressed" << std::endl;
}

button->connect("pressed", function);
{% endhighlight %}

<p class="SmallBottomMargin">You could also use lambda functions of course if your function is very small. This is all the code you need to close the window when your quit button gets pressed:</p>
{% highlight c++ %}
quitButton->connect("pressed", [&](){ window.close(); });
{% endhighlight %}
</div>

<div>
<h3 id="connecting-multiple-signals-at-once">Connecting multiple signals at once</h3>
<p class="SmallBottomMargin">The Checkbox class has a separate "Checked" and "Unchecked" signal. You may want the same function to handle this, so it would be a waste to bind them separately and thus duplicate code. So you can connect to multiple signals at once by putting a space between them in the string.</p>
{% highlight c++ %}
checkbox->connect("checked unchecked", function);
{% endhighlight %}
</div>

<div>
<h3 id="binding-parameters">Binding parameters</h3>
<p class="SmallBottomMargin">So far the functions did not have any parameters. But the function may e.g. need access to the Gui object. This can be solved by just passing the needed parameters to the connect function.</p>
{% highlight c++ %}
void function(tgui::Gui& gui, int i) {}
picture->connect("clicked", function, std::ref(gui), 5);
{% endhighlight %}
</div>

<div>
<h3 id="connecting-member-functions">Connecting member functions</h3>
<p>Member functions are special. You must first understand a technical detail in c++ in order to bind them.</p>
<p class="SmallBottomMargin">Suppose you have the following class:</p>
{% highlight c++ %}
class MyClass
{
    void function(int i);
}
{% endhighlight %}

<p class="SmallBottomMargin">That function takes one parameter, right? Wrong.<br>
That function takes 2 parameters. Behind the scenes the function looks like this:</p>
{% highlight c++ %}
void function(MyClass* this, int i);
{% endhighlight %}
<p>That is where your 'this' pointer is coming from.</p>
<p class="SmallBottomMargin">Now that you know this, you can connect the function.</p>
{% highlight c++ %}
int i = 2;
MyClass myClassInstance;

// &MyClass::function is the function pointer here
// and &myClassInstance is the value for 'this'
button->connect("pressed", &MyClass::function, &myClassInstance, i);
{% endhighlight %}
</div>

<div>
<h3 id="binding-function-calls">Binding function calls</h3>
<p class="SmallBottomMargin">Finally there is an awesome thing that you can do with std::bind. You can have a function called and then pass its return value as argument of the signal handler function. In the code below, func2 will be called when the button is pressed, with the value of the slider as parameter.</p>
{% highlight c++ %}
int getNum() { return 5; }

void func1(int i);
void func2(int value);

picture->connect("clicked", func1, std::bind(getNum));
button->connect("pressed", func2, std::bind(&tgui::Slider::getValue, slider));
{% endhighlight %}
</div>

<div>
<h3 id="disconnecting-signals">Disconnecting signals</h3>
<p class="SmallBottomMargin">The connect function returns an id, which can be used to diconnect that function.</p>
{% highlight c++ %}
unsigned int id = button->connect("pressed", function);
button->disconnect(id);
{% endhighlight %}

<p class="SmallBottomMargin">When multiple signals are bound at once, only the id of the last signal is returned. The signal before that is guaranteed to have an id of one less than the returned value.</p>
{% highlight c++ %}
unsigned int id = checkbox->connect("checked unchecked", function);
button->disconnect(id-1); // disconnect "checked" signal
button->disconnect(id); // disconnect "unchecked" signal
{% endhighlight %}

<p class="SmallBottomMargin">It is also possible to disconnect all function that connected to a specific signal or even all signals. Be aware that this might disconnect internally used signals as well (e.g. <a href="../layouts/">layouts</a> rely on PositionChanged and SizeChanged of widgets, if you disconnect them then other widgets that depend on the position or size will no longer receive updates).</p>
{% highlight c++ %}
picture->disconnectAll("clicked");
button->disconnectAll();
{% endhighlight %}
</div>

<div>
  <h3 id="receiving-information-from-widget">Receiving information from widget</h3>
  <p>The examples so far just called a function when something happens. But what if you want to receive information about the callback as parameter to your function? There are two ways to do this, you can use the new system with unbound optional parameters or you can use the older system with the Callback class.</p>
  <p><a href="../signals-optional-parameters/">Unbound optional parameters</a> (not supported by VS2013)</p>
  <p><a href="../signals-connectEx/">connectEx function</a> (deprecated)</p>
</div>
