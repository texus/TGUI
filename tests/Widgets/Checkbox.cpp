/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "../Tests.hpp"
#include <TGUI/Widgets/Checkbox.hpp>

TEST_CASE("[Checkbox]") {
    tgui::Checkbox::Ptr checkbox = std::make_shared<tgui::Checkbox>();

    SECTION("Signals") {
        REQUIRE_NOTHROW(checkbox->connect("Checked", [](){}));
        REQUIRE_NOTHROW(checkbox->connect("Unchecked", [](){}));

        REQUIRE_NOTHROW(checkbox->connect("Checked", [](bool){}));
        REQUIRE_NOTHROW(checkbox->connect("Unchecked", [](bool){}));
    }

    SECTION("WidgetType") {
        REQUIRE(checkbox->getWidgetType() == "Checkbox");
    }

    SECTION("Checked") {
        REQUIRE(!checkbox->isChecked());
        checkbox->check();
        REQUIRE(checkbox->isChecked());
        checkbox->uncheck();
        REQUIRE(!checkbox->isChecked());
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(checkbox = std::make_shared<tgui::Theme>()->load("Checkbox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(checkbox = theme->load("Checkbox"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(checkbox);
        
        checkbox->check();
        checkbox->setText("SomeText");
        checkbox->setTextSize(25);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCheckbox1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileCheckbox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCheckbox2.txt"));
        REQUIRE(compareFiles("WidgetFileCheckbox1.txt", "WidgetFileCheckbox2.txt"));
    }
}
