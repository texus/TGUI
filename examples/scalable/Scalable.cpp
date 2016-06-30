/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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

int main()
{
    sf::RenderWindow window(sf::VideoMode(400, 300), "TGUI window");
    tgui::Gui gui(window);
    
    // Create the background image
    gui.add(std::make_shared<tgui::Picture>("../Linux.jpg"));

    // Get a bound version of the window size
    // Passing this to setPosition or setSize will make the widget automatically update when the view of the gui changes
    auto windowWidth = tgui::bindWidth(gui);
    auto windowHeight = tgui::bindHeight(gui);

    // Create the first button
    // Left:   10% of window width
    // Top:    20% of window height
    // Width:  80% of window width
    // Height: 25% of window height
    tgui::Button::Ptr play = std::make_shared<tgui::Button>();
    play->setPosition(windowWidth*0.1f, windowHeight*0.2f);
    play->setSize(windowWidth*0.8f, windowHeight*0.25f);
    play->setText("Play");
    gui.add(play);

    // Create the second button, which has the same size as the first one
    // Left:   10% of window width
    // Top:    60% of window height
    tgui::Button::Ptr exit = tgui::Button::copy(play);
    exit->setPosition(windowWidth*0.1f, windowHeight*0.6f);
    exit->setText("Exit");
    gui.add(exit);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // The view of the window has to change when the window is resized
            else if (event.type == sf::Event::Resized)
                gui.setView(sf::View{{0, 0, (float)event.size.width, (float)event.size.height}});

            // Use the current view, not the default view based on the window size
            gui.handleEvent(event);
        }

        window.clear();
        
        // Use the current view, not default view based on the window size
        gui.draw();
        
        window.display();
    }

    return EXIT_SUCCESS;
}

