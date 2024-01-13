/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/TGUI.hpp>
#include <iostream>

void login(const tgui::EditBox::Ptr& username, const tgui::EditBox::Ptr& password)
{
    std::cout << "Username: " << username->getText() << std::endl;
    std::cout << "Password: " << password->getText() << std::endl;
}

void updateTextSize(tgui::BackendGui& gui)
{
    // Update the text size of all widgets in the gui, based on the current window height
    const float windowHeight = gui.getView().getRect().height;
    gui.setTextSize(static_cast<unsigned int>(0.07f * windowHeight)); // 7% of height
}

void loadWidgets(tgui::BackendGui& gui)
{
    // Specify an initial text size instead of using the default value
    updateTextSize(gui);

    // We want the text size to be updated when the window is resized
    gui.onViewChange([&gui]{ updateTextSize(gui); });

    // Create the background image
    // The picture is of type tgui::Picture::Ptr which is actually just a typedef for std::shared_widget<tgui::Picture>
    // The picture will fit the entire window and will scale with it
    auto picture = tgui::Picture::create("xubuntu_bg_aluminium.jpg");
    picture->setSize({"100%", "100%"});
    gui.add(picture);

    // Create the username edit box
    // Similar to the picture, we set a relative position and size
    // In case it isn't obvious, the default text is the text that is displayed when the edit box is empty
    auto editBoxUsername = tgui::EditBox::create();
    editBoxUsername->setSize({"66.67%", "12.5%"});
    editBoxUsername->setPosition({"16.67%", "16.67%"});
    editBoxUsername->setDefaultText("Username");
    gui.add(editBoxUsername);

    // Create the password edit box
    // We copy the previous edit box here and keep the same size
    auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
    editBoxPassword->setPosition({"16.67%", "41.6%"});
    editBoxPassword->setPasswordCharacter('*');
    editBoxPassword->setDefaultText("Password");
    gui.add(editBoxPassword);

    // Create the login button
    auto button = tgui::Button::create("Login");
    button->setSize({"50%", "16.67%"});
    button->setPosition({"25%", "70%"});
    gui.add(button);

    // Call the login function when the button is pressed and pass the edit boxes that we created as parameters
    // The "&" in front of "login" can be removed on newer compilers, but is kept here for compatibility with GCC < 8.
    button->onPress(&login, editBoxUsername, editBoxPassword);
}

bool runExample(tgui::BackendGui& gui)
{
    try
    {
        loadWidgets(gui);
        return true;
    }
    catch (const tgui::Exception& e)
    {
        std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
        return false;
    }
}
