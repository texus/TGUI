
#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    tgui::Window window(sf::VideoMode(800, 600), "TGUI demo");

    if (window.setGlobalFont("../../Fonts/DejaVuSans.ttf") == false)
       return 1;

    // Load all the objects from a file
    window.loadObjectsFromFile("form.txt");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            window.handleEvent(event);
        }

        window.clear();
        window.drawGUI();
        window.display();

        sf::sleep(sf::milliseconds(1));
    }

    return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

