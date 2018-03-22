/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>

TEST_CASE("[Clipping]")
{
    SECTION("Panel")
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

        TEST_DRAW("Clipping_Panel.png")
    }

    SECTION("EditBox")
    {
        auto background = tgui::Panel::create({800, 600});
        background->getRenderer()->setBackgroundColor(sf::Color::Blue);
        TEST_DRAW_INIT(400, 300, background)

        auto editBox = tgui::EditBox::create();
        editBox->setPosition({260, 165});
        editBox->setSize({200, 32});
        editBox->setText("Too long to fit inside the EditBox");
        gui.add(editBox);

        editBox = tgui::EditBox::copy(editBox);
        editBox->setPosition({editBox->getPosition().x, editBox->getPosition().y + editBox->getSize().y + 2});
        gui.add(editBox);
        editBox->setCaretPosition(0);

        sf::View view({250, 160, 140, 57});
        view.setViewport({0.1f, 0.2f, 0.7f, 0.76f});
        gui.setView(view);

        TEST_DRAW("Clipping_EditBox.png")

        SECTION("Outside viewport")
        {
            sf::View view2({250, 160, 210, 38});
            view2.setViewport({0.2f, 0.1f, 0.7f, 0.6f});
            gui.setView(view2);

            TEST_DRAW("Clipping_EditBox_OutsideViewport.png")
        }
    }

    SECTION("Nesting")
    {
        auto panel = tgui::Panel::create({75, 25});
        panel->setPosition({10, 5});
        panel->getRenderer()->setBackgroundColor(sf::Color::Yellow);
        TEST_DRAW_INIT(100, 40, panel)

        auto group = tgui::Group::create();
        panel->add(group);

        auto layout = tgui::HorizontalLayout::create({150, 25});
        layout->setPosition({-30, -15});
        group->add(layout);

        auto label = tgui::Label::create("Hello");
        label->setTextSize(20);
        layout->add(label);

        label = tgui::Label::copy(label);
        label->setText("World");
        layout->add(label);

        layout = tgui::HorizontalLayout::copy(layout);
        layout->setPosition({-30, 15});
        group->add(layout);

        TEST_DRAW("Clipping_NestedLayers.png")
    }
}
