/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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

#include "Tests.hpp"
#include <TGUI/Clipping.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[Clipping]")
{
    auto panel = tgui::Panel::create({40, 30});
    panel->setPosition(395, 305);
    panel->getRenderer()->setBackgroundColor(sf::Color::Green);

    auto clippedPanel = tgui::Panel::create({200, 200});
    clippedPanel->setPosition(-50, -50);
    clippedPanel->getRenderer()->setBackgroundColor({255, 0, 0, 100});
    panel->add(clippedPanel);

    TEST_DRAW_INIT(80, 80, panel)

    auto background = tgui::Panel::create();
    background->setSize({800, 600});
    background->getRenderer()->setBackgroundColor(sf::Color::Blue);
    gui.add(background);
    background->moveToBack();

    sf::View view = gui.getView();
    view.setViewport({0.1f, 0.2f, 0.8f, 0.6f});
    view.setCenter(415, 320);
    view.setSize(80, 40);
    gui.setView(view);

    TEST_DRAW("Clipping.png")
}
