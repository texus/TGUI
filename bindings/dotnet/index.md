---
layout: page
title: ".Net binding"
breadcrumb: ".net"
---

Please note that both the binding and TGUI 0.8 are still under development and may change at any time.


### Download

{% include button.ext text="x64 windows libraries" link="https://bintray.com/texus/generic/download_file?file_path=TGUI.Net-0.8-dev-64bit.zip" style="Orange" %}

{% include button.ext text="Source code" link="https://github.com/texus/TGUI.Net/archive/master.zip" style="Orange" %}

To install, follow the instructions in the README.


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
``` vbnet
Imports SFML.System
Imports SFML.Window
Imports SFML.Graphics
Imports TGUI

Module Program

    Dim WithEvents window As RenderWindow
    Dim gui As Gui

    ''' <summary>
    ''' Entry point of application
    ''' </summary>
    Sub Main()
        Dim width = 400
        Dim height = 300

        window = New RenderWindow(New VideoMode(width, height), "TGUI.Net example (Visual Basic)")
        gui = New Gui(window)

        Dim picture = New Picture("background.jpg")
        picture.Size = New Vector2f(width, height)
        gui.Add(picture)

        Dim editBoxUsername = New EditBox()
        editBoxUsername.Position = New Vector2f(width / 6, height / 6)
        editBoxUsername.Size = New Vector2f(width * 2 / 3, height / 8)
        editBoxUsername.DefaultText = "Username"
        gui.Add(editBoxUsername)

        Dim editBoxPassword = New EditBox(editBoxUsername)
        editBoxPassword.Position = New Vector2f(width / 6, height * 5 / 12)
        editBoxPassword.PasswordCharacter = "*"
        editBoxPassword.DefaultText = "Password"
        gui.Add(editBoxPassword)

        Dim button = New Button("Login")
        button.Position = New Vector2f(width / 4, height * 7 / 10)
        button.Size = New Vector2f(width / 2, height / 6)
        gui.Add(button)

        AddHandler button.Pressed, Sub()
                                       System.Console.WriteLine("Username: " + editBoxUsername.Text)
                                       System.Console.WriteLine("Password: " + editBoxPassword.Text)
                                   End Sub

        While (window.IsOpen())
            window.DispatchEvents()

            window.Clear()
            gui.Draw()
            window.Display()

        End While

    End Sub

    ''' <summary>
    ''' Function called when the window is closed
    ''' </summary>
    Sub App_Closed(ByVal sender As Object, ByVal e As EventArgs) Handles window.Closed
        Dim window = CType(sender, RenderWindow)
        window.Close()
    End Sub

End Module
```
