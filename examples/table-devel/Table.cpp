/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/devel/Table.hpp>

int main()
{
    sf::RenderWindow window({800,600}, "Table widget example");
    window.setFramerateLimit(60);

    tgui::Gui gui(window);

    auto table = std::make_shared<tgui::Table>();
    table->setSize({780, 580});
    table->setPosition({10,10});
    table->setHeaderColumns({"First Name", "Last Name", ""});
    table->setBackgroundColor({203,201,207});
    table->setFixedColumnWidth(0, 400);
    table->setStripesRowsColor({246,246,246}, {233,233,233});
    gui.add(table);

    auto button = std::make_shared<tgui::Button>();
    button->setText("Connect");
    button->setSize({100,30});
    button->connect("pressed", [](){ std::cout << "Button pressed" << std::endl; });

    auto tableRow = std::make_shared<tgui::TableRow>();
    tableRow->addItem("Eve");
    tableRow->addItem("Jackson");
    tableRow->add(button, true);

    table->add(tableRow);
    table->addRow({"John", "Doe", "80"});
    table->addRow({"Adam", "Johnson", "67"});
    table->addRow({"Jill", "Smith", "50"});

    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
        }

        window.clear();
        gui.draw();
        window.display();
    }

    return 0;
}
