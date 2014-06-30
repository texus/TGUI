#include <TGUI/TGUI.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(400, 300), "TGUI window");
    tgui::Gui gui(window);
    gui.setGlobalFont("../../fonts/DejaVuSans.ttf");
    
    // Create the background image
    gui.add(tgui::Picture::create("../Linux.jpg"));

    // Create the first button
    // Left:   10% of window width
    // Top:    20% of window height
    // Width:  80% of window width
    // Height: 25% of window height
    auto play = tgui::Button::create("../../widgets/Black.conf");
    play->setPosition(tgui::bindWidth(gui.getContainer(), 0.1), tgui::bindHeight(gui.getContainer(), 0.2));
    play->setSize(tgui::bindWidth(gui.getContainer(), 0.8), tgui::bindHeight(gui.getContainer(), 0.25));
    play->setText("Play");
    gui.add(play);

    // Create the second button, which has the same size as the first one
    // Left:   10% of window width
    // Top:    60% of window height
    auto exit = tgui::Button::copy(play);
    exit->setPosition(tgui::bindWidth(gui.getContainer(), 0.1), tgui::bindHeight(gui.getContainer(), 0.6));
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
                window.setView(sf::View{{0, 0, (float)event.size.width, (float)event.size.height}});

            // Use the current view, not the default view based on the window size
            gui.handleEvent(event, false);
        }

        window.clear();
        
        // Use the current view, not default view based on the window size
        gui.draw(false);
        
        window.display();
    }

    return EXIT_SUCCESS;
}

