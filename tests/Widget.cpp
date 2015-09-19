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

#include "Tests.hpp"
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/ToolTip.hpp>

TEST_CASE("[Widget]") {
    tgui::Widget::Ptr widget = std::make_shared<tgui::Button>();

    SECTION("Visibile") {
        REQUIRE(widget->isVisible());
        widget->hide();
        REQUIRE(!widget->isVisible());
        widget->show();
        REQUIRE(widget->isVisible());
    }

    SECTION("Enabled") {
        REQUIRE(widget->isEnabled());
        widget->disable();
        REQUIRE(!widget->isEnabled());
        widget->enable();
        REQUIRE(widget->isEnabled());
    }

    SECTION("Parent") {
        tgui::Panel::Ptr panel1 = std::make_shared<tgui::Panel>();
        tgui::Panel::Ptr panel2 = std::make_shared<tgui::Panel>();
        
        REQUIRE(widget->getParent() == nullptr);
        panel1->add(widget);
        REQUIRE(widget->getParent() == panel1.get());
        panel1->remove(widget);
        panel2->add(widget);
        REQUIRE(widget->getParent() == panel2.get());
    }

    SECTION("Opacity") {
        REQUIRE(widget->getOpacity() == 1.F);

        widget->setOpacity(0.5f);
        REQUIRE(widget->getOpacity() == 0.5f);

        widget->setOpacity(2.f);
        REQUIRE(widget->getOpacity() == 1.f);

        widget->setOpacity(-2.f);
        REQUIRE(widget->getOpacity() == 0.f);
    }

    SECTION("Tooltip") {
        auto tooltip1 = std::make_shared<tgui::Label>();
        tooltip1->setText("some text");
        widget->setToolTip(tooltip1);
        REQUIRE(widget->getToolTip() == tooltip1);

        // ToolTip does not has to be a label
        auto tooltip2 = std::make_shared<tgui::Panel>();
        widget->setToolTip(tooltip2);
        REQUIRE(widget->getToolTip() == tooltip2);

        // ToolTip can be removed
        widget->setToolTip(nullptr);
        REQUIRE(widget->getToolTip() == nullptr);
    }
}
