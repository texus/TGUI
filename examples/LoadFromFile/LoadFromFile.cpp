
#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI demo");
    tgui::Gui gui(window);

    if (gui.setGlobalFont("../../fonts/DejaVuSans.ttf") == false)
       return 1;

    // Load all the widgets from a file
    gui.loadWidgetsFromFile("form.txt");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
        }

        window.clear();
        gui.draw();
        window.display();

        sf::sleep(sf::milliseconds(1));
    }

    return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

