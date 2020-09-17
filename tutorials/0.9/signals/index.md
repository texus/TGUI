---
layout: page
title: Signals (event callbacks)
breadcrumb: signals
---

### Connecting to signals
When something happens, widgets can send out a signal (e.g. a button has a Press signal and a list box has an ItemSelect signal). The signals are public members of the widgets (starting with the "on" prefix, e.g. "onPress").

Here is an example of having a function called when a button has been pressed.
```c++
void callbackFunc()
{
    std::cout << "Button pressed" << std::endl;
}

button->onPress(&callbackFunc);
```

You could also use lambda functions of course if your function is very small. This is all the code you need to close the window when your quit button gets pressed:
```c++
quitButton->onPress([&]{ window.close(); });
```


### Custom parameters

Custom parameters can also be provided. They have to be provided when connecting the callback function and will be passed to the callback function when the event occurs. Keep in mind that when passing a variable, a copy will be made. If the signal handler needs to have access to the original variable, use a pointer or pass the reference with std::ref.
```c++
void signalHandler1(int i);
void signalHandler2(tgui::Gui& gui);
editBox->onTextChange(signalHandler1, 5);
editBox->onTextChange(signalHandler2, std::ref(gui));
```


### Class member functions

When connecting member functions, you must also keep in mind that the first parameter of such function is a hidden 'this' pointer. You must always provide the value of the 'this' pointer manually.
```c++
class Class
{
public:
    void signalHandler1();
    void signalHandler2(tgui::Gui& gui);

private:
    void f()
    {
        checkBox->onChange(&Class::signalHandler1, this);
        checkBox->onChange(&Class::signalHandler2, this, std::ref(gui));
    }
};

Class instance;
picture->onClick(&Class::signalHandler1, &instance);
picture->onClick(&Class::signalHandler2, &instance, std::ref(gui));
```


### Optional parameters

Widgets can optionally provide information about the event that occurred. For example, the onPress signal from buttons provides the text of the button as optional parameter, so that the signal handler can distinguish which button was pressed.
```c++
void buttonPressedCallback1();
button1->onPress(buttonPressedCallback1);

void buttonPressedCallback2(tgui::String buttonText);
button2->onPress(buttonPressedCallback2);

void buttonPressedCallback3(const tgui::String& buttonText);
button3->onPress(buttonPressedCallback3);
```


### Disconnecting signals

So far the signal has been treated as a callable function. It is actually a functor object where the call operator is just a shortcut for the "connect" function. Other functions also exist, such as the "disconnect" function.

A unique id is returned when connecting a signal, which can be used to diconnect the callback function later:
```c++
unsigned int id = slider->onValueChange(signalHandler);
slider->onValueChange.disconnect(id);
```


### Temporarily ignoring signals

When e.g. calling `checkBox->setChecked(true)`, the `onChange` signal would be emitted if the check box wasn't already checked. If you only want to receive the callback when the user changes the state and don't want to receive a callback for this manual change in the code then you can temporarily disable the signal:
```c++
checkBox->onChange.setEnabled(false);
checkBox->setChecked(true);
checkBox->onChange.setEnabled(true);
```


### Getting signal by name

To use the signals you already need to know the type of the widget, but this is not ideal if you want to e.g. load which signals to connect from a text file. So there is a method to access these signals by just specifying their name as a string.
```c++
widget->getSignal("TextChanged").connect([]{ std::cout << "Text was changed\n"; });
```

Note that when using this method, the optional parameters are not supported (as getSignal returns the base signal type instead of knowing the exact type). While the signal object is called onTextChange, the name is "TextChanged" (past tense). If you are uncertain about the string to use, you can find it by accessing `editBox->onTextChange.getName()`.

If you intent to create a single callback function for all signals then you can make use of the `connectEx` function. The callback function passed to this function has 2 parameters: the widget that triggers the signal and the name of the signal.
```c++
void callbackFunc(Widget::Ptr widget, const String& signalName);
widget->getSignal(signalName).connect(callbackFunc);
```
