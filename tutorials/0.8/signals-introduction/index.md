---
layout: page
title: Introduction to signals
breadcrumb: signals intro
redirect_from:
  - /tutorials/0.8/signals-optional-parameters/
  - /tutorials/0.8/signals-connectEx/
---

### Connecting to signals
When something happens, widgets can send out a signal (e.g. a button has a "pressed" signal and a checkbox has "checked" and "unchecked" signals). You can bind a signal handler to such a signal with the connect function. The first parameter is the name of the signal, this is the trigger on which the callback should be send. This name is case-insensitive, so it does not matter if you write "pressed" or "Pressed". The second parameter is the function that you want to be called when the signal is send.

Here is an example of letting a function get called when a button has been pressed.
```c++
void signalHandler()
{
    std::cout << "Button pressed" << std::endl;
}

button->connect("pressed", signalHandler);
```

You could also use lambda functions of course if your function is very small. This is all the code you need to close the window when your quit button gets pressed:
```c++
quitButton->connect("pressed", [&](){ window.close(); });
```

The signal handler can also take two parameters: the widget that emitted the signal and the name of the signal.
```c++
void signalHandler(tgui::Widget::Ptr widget, const std::string& signalName);
editBox->connect("TextChanged", signalHandler);
```


### Custom parameters
Custom parameters can also be provided. They have to be provided when calling the connect function and will be passed to the signal handler when the event occurs. Keep in mind that when passing a variable, a copy will be made. If the signal handler needs to have access to the original variable, use std::ref.
```c++
void signalHandler1(int i);
void signalHandler2(tgui::Gui& gui, tgui::Widget::Ptr widget, const std::string& signalName);
editBox->connect("TextChanged", signalHandler1, 5);
editBox->connect("TextChanged", signalHandler2, std::ref(gui));
```

When connecting class member functions, you must also keep in mind that the first parameter of such function is a hidden 'this' pointer. You must always provide the value of the 'this' pointer manually.
```c++
struct Class {
    void signalHandler1();
    void signalHandler2(tgui::Widget::Ptr widget, const std::string& signalName);
    void signalHandler3(tgui::Gui& gui, tgui::Widget::Ptr widget, const std::string& signalName);
};

Class instance;

editBox->connect("TextChanged", &Class::signalHandler1, &instance);
editBox->connect("TextChanged", &Class::signalHandler2, &instance);
editBox->connect("TextChanged", &Class::signalHandler3, &instance, std::ref(gui));
```


### Optional parameters
Widgets can optionally provide information about the event that occurred. For example, the "pressed" signal from buttons provides the text of the button as optional parameter, so that the signal handler can distinguish which button was pressed. The connect function will verify at runtime that the Pressed signal supports an sf::String parameter and will allow the text on the button to be passed as argument when the button is pressed.
```c++
void buttonPressedCallback1();
button->connect("pressed", buttonPressedCallback1);

void buttonPressedCallback2(const sf::String& buttonText);
button->connect("pressed", buttonPressedCallback2);

void buttonPressedCallback3(tgui::Widget::Ptr widget, const std::string& signalName, const sf::String& buttonText);
button->connect("pressed", buttonPressedCallback3);
```


### Disconnecting signals
The connect function returns a unique id, which can be used to diconnect the signal handler.
```c++
unsigned int id = button1->connect("pressed", signalHandler);
button1->disconnect(id);
```

It is also possible to disconnect all signal handlers at once.
```c++
button1->disconnectAll("pressed");
button2->disconnectAll();
```


### List of supported signals

All signal names have an equivalent in the `tgui::Signals` namespace so that you can use `tgui::Signals::Button::Pressed` instead of `"pressed"`. The [Signals documentation](https://tgui.eu/documentation/0.8/namespacetgui_1_1Signals.html) can thus be used to find which signals each widget supports.
