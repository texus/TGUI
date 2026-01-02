/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[GrowHorizontalLayout]")
{
    auto layout = tgui::GrowHorizontalLayout::create();
    layout->setHeight(300);
    layout->setPosition(50, 40);

    SECTION("Positions and sizes")
    {
        REQUIRE(layout->getSize() == tgui::Vector2f{0, 300});

        layout->getRenderer()->setSpaceBetweenWidgets(10);
        layout->setSize(300, 300);
        REQUIRE(layout->getSize() == tgui::Vector2f{0, 300});

        auto button = tgui::Button::create("Hello");
        button->setWidth(50);
        layout->add(button, "Btn");

        REQUIRE(layout->getSize() == tgui::Vector2f{50, 300});

        auto panel = tgui::Panel::create();
        layout->add(panel);
        panel->setWidth(30);

        REQUIRE(layout->getSize() == tgui::Vector2f{90, 300});

        auto button2 = tgui::Button::create("World");
        button2->setWidth("height * 0.4");
        layout->add(button2);

        REQUIRE(layout->getSize() == tgui::Vector2f{220, 300});

        auto editBox = tgui::EditBox::create();
        editBox->setWidth(20);
        layout->insert(1, editBox);

        REQUIRE(layout->getSize() == tgui::Vector2f{250, 300});

        layout->remove(panel);

        REQUIRE(layout->getSize() == tgui::Vector2f{210, 300});

        layout->setHeight(350);
        REQUIRE(layout->getSize() == tgui::Vector2f{230, 350});

        auto layout2 = tgui::GrowHorizontalLayout::copy(layout);

        layout2->get("Btn")->setWidth(100);
        REQUIRE(layout->getSize() == tgui::Vector2f{230, 350});
        REQUIRE(layout2->getSize() == tgui::Vector2f{280, 350});

        layout->removeAllWidgets();
        REQUIRE(layout->getSize() == tgui::Vector2f{0, 350});
        REQUIRE(layout2->getSize() == tgui::Vector2f{280, 350});
    }

    SECTION("Saving and loading from file")
    {
        layout->add(tgui::Button::create("Hello"));
        layout->add(tgui::Button::create("World"));
        testSavingWidget("GrowHorizontalLayout", layout, false);
    }
}
