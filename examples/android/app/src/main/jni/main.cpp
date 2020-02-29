/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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

// The background image will rotate with the screen
void setBackground(tgui::Gui& gui, sf::View view)
{
    gui.get("Landscape")->setSize(view.getSize().x, view.getSize().y);
    gui.get("Portrait")->setSize(view.getSize().x, view.getSize().y);

    if (view.getSize().x > view.getSize().y)
    {
        gui.get("Landscape")->setVisible(true);
        gui.get("Portrait")->setVisible(false);
    }
    else
    {
        gui.get("Landscape")->setVisible(false);
        gui.get("Portrait")->setVisible(true);
    }
}

int main(int argc, char *argv[])
{
    sf::VideoMode screen(sf::VideoMode::getDesktopMode());
    sf::RenderWindow window(screen, "");
    window.setFramerateLimit(30);

    tgui::Gui gui(window);

    auto picLandscape = tgui::Picture::create("Background-Landscape.png");
    gui.add(picLandscape, "Landscape");

    auto picPortrait = tgui::Picture::create("Background-Portrait.png");
    gui.add(picPortrait, "Portrait");

    // The button will quit the program
    auto button = tgui::Button::create("Quit");
    button->setPosition(50, 50);
    button->setSize(200, 50);
    button->connect("clicked", [&](){ window.close(); });
    gui.add(button);

    // Clicking on this edit box will open the keyboard and allow you to type in it
    auto editBox = tgui::EditBox::create();
    editBox->setPosition(50, 150);
    editBox->setSize(400, 40);
    editBox->setDefaultText("Enter text here...");
    gui.add(editBox);

    setBackground(gui, window.getDefaultView());

    // We shouldn't try drawing to the screen while in background
    // so we'll have to track that. You can do minor background
    // work, but keep battery life in mind.
    bool active = true;

    while (window.isOpen())
    {
        sf::Event event;
        while (active ? window.pollEvent(event) : window.waitEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }
                case sf::Event::KeyPressed:
                {
                    if (event.key.code == sf::Keyboard::Escape)
                        window.close();

                    break;
                }
                case sf::Event::Resized:
                {
                    sf::View view = window.getView();
                    view.setSize(event.size.width, event.size.height);
                    view.setCenter(event.size.width / 2, event.size.height / 2);
                    window.setView(view);
                    gui.setView(view);

                    setBackground(gui, view);
                    break;
                }

                // On Android MouseLeft/MouseEntered are (for now) triggered,
                // whenever the app loses or gains focus.
                case sf::Event::MouseLeft:
                {
                    active = false;
                    break;
                }
                case sf::Event::MouseEntered:
                {
                    active = true;
                    break;
                }
                default:
                    break;
            }

            gui.handleEvent(event);
        }

        if (active)
        {
            window.clear();
            gui.draw();
            window.display();
        }
        else // Application is in background
            sf::sleep(sf::milliseconds(100));
    }
}
