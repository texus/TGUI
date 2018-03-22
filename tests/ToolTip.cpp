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
#include <TGUI/ToolTip.hpp>
#include <TGUI/TextureManager.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[ToolTip]")
{
    sf::Time oldTimeToDisplay = tgui::ToolTip::getTimeToDisplay();
    sf::Vector2f oldDistanceToMouse = tgui::ToolTip::getDistanceToMouse();

    SECTION("TimeToDisplay")
    {
        tgui::ToolTip::setTimeToDisplay(sf::milliseconds(280));
        REQUIRE(tgui::ToolTip::getTimeToDisplay() == sf::milliseconds(280));
    }

    SECTION("DistanceToMouse")
    {
        tgui::ToolTip::setDistanceToMouse({5, 5});
        REQUIRE(tgui::ToolTip::getDistanceToMouse() == sf::Vector2f(5, 5));
    }

    SECTION("Setting tool tip of widget")
    {
        auto widget = tgui::ClickableWidget::create();

        auto tooltip1 = tgui::Label::create();
        tooltip1->setRenderer(tgui::Theme("resources/Black.txt").getRenderer("ToolTip"));
        tooltip1->setText("some text");
        widget->setToolTip(tooltip1);
        REQUIRE(widget->getToolTip() == tooltip1);

        SECTION("Saving and loading from file")
        {
            tgui::ToolTip::setTimeToDisplay(sf::milliseconds(320));
            tgui::ToolTip::setDistanceToMouse({2, 1});

            testSavingWidget("ToolTip", widget, false);
        }

        // ToolTip does not has to be a label
        auto tooltip2 = tgui::Panel::create();
        widget->setToolTip(tooltip2);
        REQUIRE(widget->getToolTip() == tooltip2);

        // ToolTip can be removed
        widget->setToolTip(nullptr);
        REQUIRE(widget->getToolTip() == nullptr);
    }

    tgui::ToolTip::setTimeToDisplay(oldTimeToDisplay);
    tgui::ToolTip::setDistanceToMouse(oldDistanceToMouse);
}
