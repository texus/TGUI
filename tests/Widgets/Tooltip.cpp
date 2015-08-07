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
#include <TGUI/Widgets/Tooltip.hpp>
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[Tooltip]") {
    tgui::Tooltip::Ptr tooltip = std::make_shared<tgui::Tooltip>();

    SECTION("WidgetType") {
        REQUIRE(tooltip->getWidgetType() == "Tooltip");
    }

    SECTION("TimeToDisplay") {
        tgui::Tooltip::setTimeToDisplay(sf::milliseconds(280));
        REQUIRE(tgui::Tooltip::getTimeToDisplay() == sf::milliseconds(280));
    }

    SECTION("Saving and loading from file") {
        tgui::Tooltip::setTimeToDisplay(sf::milliseconds(280));

        SECTION("independent tooltip") {
            REQUIRE_NOTHROW(tooltip = std::make_shared<tgui::Theme>()->load("Tooltip"));

            auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            REQUIRE_NOTHROW(tooltip = theme->load("Tooltip"));

            auto parent = std::make_shared<tgui::GuiContainer>();
            parent->add(tooltip);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTooltip1.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileTooltip1.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTooltip2.txt"));
            REQUIRE(compareFiles("WidgetFileTooltip1.txt", "WidgetFileTooltip2.txt"));
        }

        SECTION("tooltip in widget") {
            auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            tgui::Button::Ptr button = theme->load("Button");
            tooltip = theme->load("Tooltip");
            tooltip->setText("Tooltip text");
            button->setTooltip(tooltip);
            
            auto parent = std::make_shared<tgui::GuiContainer>();
            parent->add(button, "Name");

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTooltip3.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileTooltip3.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTooltip4.txt"));
            REQUIRE(compareFiles("WidgetFileTooltip3.txt", "WidgetFileTooltip4.txt"));
        }
    }
}
