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
#include <TGUI/Widgets/ToolTip.hpp>
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[ToolTip]") {
    tgui::ToolTip::Ptr tooltip = std::make_shared<tgui::ToolTip>();

    SECTION("WidgetType") {
        REQUIRE(tooltip->getWidgetType() == "ToolTip");
    }

    SECTION("TimeToDisplay") {
        tgui::ToolTip::setTimeToDisplay(sf::milliseconds(280));
        REQUIRE(tgui::ToolTip::getTimeToDisplay() == sf::milliseconds(280));
    }

    SECTION("Saving and loading from file") {
        tgui::ToolTip::setTimeToDisplay(sf::milliseconds(280));

        SECTION("independent tooltip") {
            REQUIRE_NOTHROW(tooltip = std::make_shared<tgui::Theme>()->load("ToolTip"));

            auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            REQUIRE_NOTHROW(tooltip = theme->load("ToolTip"));

            auto parent = std::make_shared<tgui::GuiContainer>();
            parent->add(tooltip);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileToolTip1.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileToolTip1.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileToolTip2.txt"));
            REQUIRE(compareFiles("WidgetFileToolTip1.txt", "WidgetFileToolTip2.txt"));
        }

        SECTION("tooltip in widget") {
            auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            tgui::Button::Ptr button = theme->load("Button");
            tooltip = theme->load("ToolTip");
            REQUIRE(tooltip->getPrimaryLoadingParameter() == "resources/Black.txt");
            REQUIRE(tooltip->getSecondaryLoadingParameter() == "tooltip");

            tooltip->setText("ToolTip text");
            button->setToolTip(tooltip);
            
            auto parent = std::make_shared<tgui::GuiContainer>();
            parent->add(button, "Name");

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileToolTip3.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileToolTip3.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileToolTip4.txt"));
            REQUIRE(compareFiles("WidgetFileToolTip3.txt", "WidgetFileToolTip4.txt"));
        }
    }
}
