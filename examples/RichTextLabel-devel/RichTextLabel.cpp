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
#include <TGUI/Widgets/devel/RichTextLabel.hpp>

int main()
{
    sf::RenderWindow window({800,600}, "RichTextLabel widget example");
    window.setFramerateLimit(60);
    tgui::Gui gui(window);

    tgui::RichTextLabel::Ptr label = std::make_shared<tgui::RichTextLabel>();
    label->setText(L"*bold*, ~italic~, _underlined_\n"
                    "#white unicode: áéíóúçÆæÞðë\n"
                    "#red red, #green green, #blue blue\n"
                    "#ff6600 sexy #33ff99 hex #9933ff code #ff33cc support!\n"
                    "#ffff33 ~*_EVERYTHING AT ONCE :D_*~\n"
                    "#white Escaping format characters is supported: \\~\\*\\#\\_");
    label->setPosition(50, 30);
    gui.add(label);

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
