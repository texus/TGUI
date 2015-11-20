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
#include <TGUI/Widgets/CheckBox.hpp>

TEST_CASE("[CheckBox]") {
    tgui::CheckBox::Ptr checkBox = std::make_shared<tgui::CheckBox>();
    checkBox->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(checkBox->connect("Checked", [](){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](){}));

        REQUIRE_NOTHROW(checkBox->connect("Checked", [](bool){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](bool){}));
    }

    SECTION("WidgetType") {
        REQUIRE(checkBox->getWidgetType() == "CheckBox");
    }

    SECTION("Checked") {
        REQUIRE(!checkBox->isChecked());
        checkBox->check();
        REQUIRE(checkBox->isChecked());
        checkBox->uncheck();
        REQUIRE(!checkBox->isChecked());
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(checkBox = std::make_shared<tgui::Theme>()->load("CheckBox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(checkBox = theme->load("CheckBox"));
        REQUIRE(checkBox->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(checkBox->getSecondaryLoadingParameter() == "checkbox");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(checkBox);
        
        checkBox->check();
        checkBox->setText("SomeText");
        checkBox->setTextSize(25);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCheckBox1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileCheckBox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCheckBox2.txt"));
        REQUIRE(compareFiles("WidgetFileCheckBox1.txt", "WidgetFileCheckBox2.txt"));

        SECTION("Copying widget") {
            tgui::CheckBox temp;
            temp = *checkBox;

            parent->removeAllWidgets();
            parent->add(tgui::CheckBox::copy(std::make_shared<tgui::CheckBox>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCheckBox2.txt"));
            REQUIRE(compareFiles("WidgetFileCheckBox1.txt", "WidgetFileCheckBox2.txt"));
        }
    }
}
