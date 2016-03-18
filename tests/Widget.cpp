/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/TGUI.hpp>

TEST_CASE("[Widget]")
{
    tgui::Widget::Ptr widget = std::make_shared<tgui::ClickableWidget>();

    SECTION("Visibile")
    {
        REQUIRE(widget->isVisible());
        widget->hide();
        REQUIRE(!widget->isVisible());
        widget->show();
        REQUIRE(widget->isVisible());
    }

    SECTION("Enabled")
    {
        REQUIRE(widget->isEnabled());
        widget->disable();
        REQUIRE(!widget->isEnabled());
        widget->enable();
        REQUIRE(widget->isEnabled());
    }

    SECTION("Parent")
    {
        tgui::Panel::Ptr panel1 = std::make_shared<tgui::Panel>();
        tgui::Panel::Ptr panel2 = std::make_shared<tgui::Panel>();
        tgui::Panel::Ptr panel3 = std::make_shared<tgui::Panel>();
        
        REQUIRE(widget->getParent() == nullptr);
        panel1->add(widget);
        REQUIRE(widget->getParent() == panel1.get());
        panel1->remove(widget);
        REQUIRE(widget->getParent() == nullptr);
        panel2->add(widget);
        REQUIRE(widget->getParent() == panel2.get());
        widget->setParent(panel3.get());
        REQUIRE(widget->getParent() == panel3.get());
        widget->setParent(nullptr);
        REQUIRE(widget->getParent() == nullptr);
    }

    SECTION("Tooltip")
    {
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

    SECTION("Font")
    {
        widget = std::make_shared<tgui::ClickableWidget>();
        REQUIRE(widget->getFont() == nullptr);
        widget->setFont("resources/DroidSansArmenian.ttf");
        REQUIRE(widget->getFont() != nullptr);
        widget->setFont(nullptr);
        REQUIRE(widget->getFont() == nullptr);

        tgui::Gui gui;
        gui.add(widget);
        REQUIRE(widget->getFont() != nullptr);
    }

    SECTION("Move to front/back")
    {
        auto container = std::make_shared<tgui::Panel>();
        auto widget1 = std::make_shared<tgui::ClickableWidget>();
        auto widget2 = std::make_shared<tgui::ClickableWidget>();
        auto widget3 = std::make_shared<tgui::ClickableWidget>();
        container->add(widget1);
        container->add(widget2);
        container->add(widget3);

        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == widget1);
        REQUIRE(container->getWidgets()[1] == widget2);
        REQUIRE(container->getWidgets()[2] == widget3);

        widget1->moveToFront();
        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == widget2);
        REQUIRE(container->getWidgets()[1] == widget3);
        REQUIRE(container->getWidgets()[2] == widget1);

        widget3->moveToFront();
        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == widget2);
        REQUIRE(container->getWidgets()[1] == widget1);
        REQUIRE(container->getWidgets()[2] == widget3);

        widget1->moveToBack();
        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == widget1);
        REQUIRE(container->getWidgets()[1] == widget2);
        REQUIRE(container->getWidgets()[2] == widget3);

        widget2->moveToBack();
        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == widget2);
        REQUIRE(container->getWidgets()[1] == widget1);
        REQUIRE(container->getWidgets()[2] == widget3);
    }

    SECTION("Renderer")
    {
        // TODO: Other tests with the renderer class (e.g. sharing and copying)

        REQUIRE(widget->getRenderer()->getOpacity() == 1.f);

        widget->getRenderer()->setOpacity(0.5f);
        REQUIRE(widget->getRenderer()->getOpacity() == 0.5f);

        widget->getRenderer()->setOpacity(2.f);
        REQUIRE(widget->getRenderer()->getOpacity() == 1.f);

        widget->getRenderer()->setOpacity(-2.f);
        REQUIRE(widget->getRenderer()->getOpacity() == 0.f);
    }

    SECTION("Saving and loading widget with layouts from file")
    {
        auto parent = std::make_shared<tgui::Panel>();
        parent->add(widget, "Widget Name.With:Special{Chars}");

        SECTION("Without layout bindings")
        {
            widget->hide();
            widget->disable();

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileClickableWidget1.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileClickableWidget1.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileClickableWidget2.txt"));
            REQUIRE(compareFiles("WidgetFileClickableWidget1.txt", "WidgetFileClickableWidget2.txt"));

            REQUIRE(!parent->get("Widget Name.With:Special{Chars}")->isVisible());
            REQUIRE(!parent->get("Widget Name.With:Special{Chars}")->isEnabled());
        }

        SECTION("Bind 2d non-string")
        {
            widget->setPosition(tgui::bindPosition(parent));
            widget->setSize(tgui::bindSize(parent));

            parent->setSize(400, 300);
            parent->setPosition(50, 50);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget1.txt"));
            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileWidget1.txt"));
            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget2.txt"));
            REQUIRE(compareFiles("WidgetFileWidget1.txt", "WidgetFileWidget2.txt"));
            widget = parent->get("Widget Name.With:Special{Chars}");

            // Non-string layouts cannot be saved yet!
            parent->setPosition(100, 100);
            parent->setSize(800, 600);
            REQUIRE(widget->getPosition() == sf::Vector2f(50, 50));
            REQUIRE(widget->getSize() == sf::Vector2f(400, 300));
        }

        SECTION("Bind 1d non-strings and string combination")
        {
            widget->setPosition(tgui::bindLeft(parent), {"parent.top"});
            widget->setSize({"parent.width"}, tgui::bindHeight(parent));

            parent->setSize(400, 300);
            parent->setPosition(50, 50);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget1.txt"));
            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileWidget1.txt"));
            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget2.txt"));
            REQUIRE(compareFiles("WidgetFileWidget1.txt", "WidgetFileWidget2.txt"));
            widget = parent->get("Widget Name.With:Special{Chars}");

            // Non-string layout cannot be saved yet, string layout will have been saved correctly!
            parent->setPosition(100, 100);
            parent->setSize(800, 600);
            REQUIRE(widget->getPosition() == sf::Vector2f(50, 100));
            REQUIRE(widget->getSize() == sf::Vector2f(800, 300));
        }

        SECTION("Bind 1d strings")
        {
            widget->setPosition({"&.x"}, {"&.y"});
            widget->setSize({"&.w"}, {"&.h"});

            parent->setSize(400, 300);
            parent->setPosition(50, 50);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget1.txt"));
            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileWidget1.txt"));
            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget2.txt"));
            REQUIRE(compareFiles("WidgetFileWidget1.txt", "WidgetFileWidget2.txt"));
            widget = parent->get("Widget Name.With:Special{Chars}");

            parent->setPosition(100, 100);
            parent->setSize(800, 600);
            REQUIRE(widget->getPosition() == sf::Vector2f(100, 100));
            REQUIRE(widget->getSize() == sf::Vector2f(800, 600));
        }

        SECTION("Bind 2d strings")
        {
            widget->setPosition({"{&.x, &.y}"});
            widget->setSize({"parent.size"});

            parent->setSize(400, 300);
            parent->setPosition(50, 50);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget1.txt"));
            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileWidget1.txt"));
            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileWidget2.txt"));
            REQUIRE(compareFiles("WidgetFileWidget1.txt", "WidgetFileWidget2.txt"));
            widget = parent->get("Widget Name.With:Special{Chars}");

            parent->setPosition(100, 100);
            parent->setSize(800, 600);
            REQUIRE(widget->getPosition() == sf::Vector2f(100, 100));
            REQUIRE(widget->getSize() == sf::Vector2f(800, 600));
        }
    }
/**
    SECTION("Bug Fixes") {
        SECTION("Disabled widgets should not be focusable (https://forum.tgui.eu/index.php?topic=384)") {
            tgui::Panel::Ptr panel = std::make_shared<tgui::Panel>();
            tgui::EditBox::Ptr editBox = std::make_shared<tgui::EditBox>();
            panel->add(editBox);

            editBox->focus();
            REQUIRE(editBox->isFocused());

            editBox->disable();
            REQUIRE(!editBox->isFocused());

            editBox->focus();
            REQUIRE(!editBox->isFocused());
        }
    }
*/
}
