#include <TGUI/TGUI.hpp>

void loadObjects( tgui::Window& window )
{
    // Create the background image
    tgui::Picture::Ptr picture(window);
    picture->load("xubuntu_bg_aluminium.jpg");
    picture->setSize(800, 600);

    // Create the username label
    tgui::Label::Ptr labelUsername(window);
    labelUsername->setText("Username:");
    labelUsername->setPosition(200, 100);

    // Create the password label
    tgui::Label::Ptr labelPassword(window);
    labelPassword->setText("Password:");
    labelPassword->setPosition(200, 250);

    // Create the username edit box
    tgui::EditBox::Ptr editBoxUsername(window, "Username");
    editBoxUsername->load("../../objects/EditBox/Black");
    editBoxUsername->setSize(400, 40);
    editBoxUsername->setPosition(200, 140);

    // Create the password edit box (we will copy the previously created edit box)
    tgui::EditBox::Ptr editBoxPassword = window.copy(editBoxUsername, "Password");
    editBoxPassword->setPosition(200, 290);
    editBoxPassword->setPasswordChar('*');

    // Create the login button
    tgui::Button::Ptr button(window);
    button->load("../../objects/Button/Black");
    button->setSize(260, 60);
    button->setPosition(270, 440);
    button->setText("Login");
    button->bindCallback(tgui::Button::LeftMouseClicked);
    button->setCallbackId(1);
}

int main()
{
    // Create the window
    tgui::Window window(sf::VideoMode(800, 600), "TGUI window");

    // Load the font (you should check the return value to make sure that it is loaded)
    window.setGlobalFont("../../Fonts/DejaVuSans.ttf");

    // Load the objects
    loadObjects(window);

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Pass the event to all the object
            window.handleEvent(event);
        }

        // The callback loop
        tgui::Callback callback;
        while (window.pollCallback(callback))
        {
            // Make sure tha callback comes from the button
            if (callback.callbackId == 1)
            {
                // Get the username and password
                tgui::EditBox::Ptr editBoxUsername = window.get("Username");
                tgui::EditBox::Ptr editBoxPassword = window.get("Password");
                
                sf::String username = editBoxUsername->getText();
                sf::String password = editBoxPassword->getText();
                    
                // Continue here by checking if the username and password are correct ...
            }
        }

        window.clear();

        // Draw all created objects
        window.drawGUI();

        window.display();
    }

    return EXIT_SUCCESS;
}
