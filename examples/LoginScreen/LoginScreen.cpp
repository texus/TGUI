#include <TGUI/TGUI.hpp>

void loadWidgets( tgui::Gui& gui )
{
    // Create the background image
    tgui::Picture::Ptr picture(gui);
    picture->load("../xubuntu_bg_aluminium.jpg");
    picture->setSize(800, 600);

    // Create the username label
    tgui::Label::Ptr labelUsername(gui);
    labelUsername->setText("Username:");
    labelUsername->setPosition(200, 100);

    // Create the password label
    tgui::Label::Ptr labelPassword(gui);
    labelPassword->setText("Password:");
    labelPassword->setPosition(200, 250);

    // Create the username edit box
    tgui::EditBox::Ptr editBoxUsername(gui, "Username");
    editBoxUsername->load("../../widgets/Black.conf");
    editBoxUsername->setSize(400, 40);
    editBoxUsername->setPosition(200, 140);

    // Create the password edit box (we will copy the previously created edit box)
    tgui::EditBox::Ptr editBoxPassword = gui.copy(editBoxUsername, "Password");
    editBoxPassword->setPosition(200, 290);
    editBoxPassword->setPasswordCharacter('*');

    // Create the login button
    tgui::Button::Ptr button(gui);
    button->load("../../widgets/Black.conf");
    button->setSize(260, 60);
    button->setPosition(270, 440);
    button->setText("Login");
    button->bindCallback(tgui::Button::LeftMouseClicked);
    button->setCallbackId(1);
}

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI window");
    tgui::Gui gui(window);

    // Load the font (you should check the return value to make sure that it is loaded)
    gui.setGlobalFont("../../fonts/DejaVuSans.ttf");

    // Load the widgets
    loadWidgets(gui);

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Pass the event to all the widgets
            gui.handleEvent(event);
        }

        // The callback loop
        tgui::Callback callback;
        while (gui.pollCallback(callback))
        {
            // Make sure tha callback comes from the button
            if (callback.id == 1)
            {
                // Get the username and password
                tgui::EditBox::Ptr editBoxUsername = gui.get("Username");
                tgui::EditBox::Ptr editBoxPassword = gui.get("Password");

                sf::String username = editBoxUsername->getText();
                sf::String password = editBoxPassword->getText();

                // Continue here by checking if the username and password are correct ...
            }
        }

        window.clear();

        // Draw all created widgets
        gui.draw();

        window.display();
    }

    return EXIT_SUCCESS;
}

