---
layout: page
title: ".Net binding"
breadcrumb: ".net"
---
This page contains information about the .Net binding.  

Please note that both the binding and TGUI 0.8 are still under development and may change at any time.


### Download and install

The latest version can be found [on Github](https://github.com/texus/TGUI.Net).

There are currently no precompiled binaries available.

You need to have [CTGUI](/bindings/c) installed before you can use TGUI.Net.

Inside the build folder you will find a TGUI.Net.sln project that you will need to build. You will have to manually change the references so that the sfmlnet libraries are found on your pc.


### Example code (C#)
``` c#
using System;
using SFML.System;
using SFML.Window;
using SFML.Graphics;
using TGUI;

class Example
{
    static void Main(string[] args)
    {
        const uint width = 400;
        const uint height = 300;

        RenderWindow window = new RenderWindow(new VideoMode(width, height), "TGUI.Net example");
        Gui gui = new Gui(window);

        window.Closed += (s,e) => window.Close();

        Picture picture = new Picture("background.jpg");
        picture.Size = new Vector2f(width, height);
        gui.Add(picture);

        EditBox editBoxUsername = new EditBox();
        editBoxUsername.Position = new Vector2f(width / 6, height / 6);
        editBoxUsername.Size = new Vector2f(width * 2/3, height / 8);
        editBoxUsername.DefaultText = "Username";
        gui.Add(editBoxUsername);

        EditBox editBoxPassword = new EditBox(editBoxUsername);
        editBoxPassword.Position = new Vector2f(width / 6, height * 5/12);
        editBoxPassword.PasswordCharacter = '*';
        editBoxPassword.DefaultText = "Password";
        gui.Add(editBoxPassword);

        Button button = new Button("Login");
        button.Position = new Vector2f(width / 4, height * 7/10);
        button.Size = new Vector2f(width / 2, height / 6);
        gui.Add(button);

        button.Pressed += (s, e) => Console.WriteLine("Username: " + editBoxUsername.Text + "\n"
                                                      + "Password: " + editBoxPassword.Text);

        while (window.IsOpen)
        {
            window.DispatchEvents();

            window.Clear();
            gui.Draw();
            window.Display();
        }
    }
}
```


### Example code (VB.Net)
TGUI.Net has not yet been tested with VB.Net.
