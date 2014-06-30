#include <TGUI/TGUI.hpp>

void loadWidgets( tgui::Gui& gui )
{
    // Create the background image
    auto picture = tgui::Picture::create("../xubuntu_bg_aluminium.jpg");
    picture->setSize(800, 600);
    gui.add(picture);

    // Create the username label
    auto labelUsername = tgui::Label::create();
    labelUsername->setText("Username:");
    labelUsername->setPosition(200, 100);
    gui.add(labelUsername);

    // Create the password label
    auto labelPassword = tgui::Label::create();
    labelPassword->setText("Password:");
    labelPassword->setPosition(200, 250);
    gui.add(labelPassword);

    // Create the username edit box
    auto editBoxUsername = tgui::EditBox::create("../../widgets/Black.conf");
    editBoxUsername->setSize(400, 40);
    editBoxUsername->setPosition(200, 140);
    gui.add(editBoxUsername, "Username");

    // Create the password edit box (we will copy the previously created edit box)
    auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
    editBoxPassword->setPosition(200, 290);
    editBoxPassword->setPasswordCharacter('*');
    gui.add(editBoxPassword, "Password");

    // Create the login button
    auto button = tgui::Button::create("../../widgets/Black.conf");
    button->setSize(260, 60);
    button->setPosition(270, 440);
    button->setText("Login");
    button->bindCallback(tgui::Button::LeftMouseClicked);
    button->setCallbackId(1);
    gui.add(button);
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
                auto editBoxUsername = gui.get<tgui::EditBox>("Username");
                auto editBoxPassword = gui.get<tgui::EditBox>("Password");

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

