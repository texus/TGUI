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
#include <TGUI/Gui.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/EditBox.hpp>

TEST_CASE("[Widget]")
{
    tgui::Widget::Ptr widget = tgui::ClickableWidget::create();

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
        tgui::Panel::Ptr panel1 = tgui::Panel::create();
        tgui::Panel::Ptr panel2 = tgui::Panel::create();
        tgui::Panel::Ptr panel3 = tgui::Panel::create();

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

    SECTION("Move to front/back")
    {
        auto widget1 = tgui::ClickableWidget::create();
        auto widget2 = tgui::ClickableWidget::create();
        auto widget3 = tgui::ClickableWidget::create();

        // Moving to front or back has no effect before widgets are added to a container
        widget2->moveToFront();
        widget2->moveToBack();

        auto container = tgui::Panel::create();
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
        auto renderer = widget->getRenderer();

        SECTION("Opacity")
        {
            REQUIRE(renderer->getOpacity() == 1.f);

            renderer->setOpacity(0.5f);
            REQUIRE(renderer->getOpacity() == 0.5f);

            renderer->setOpacity(2.f);
            REQUIRE(renderer->getOpacity() == 1.f);

            renderer->setOpacity(-2.f);
            REQUIRE(renderer->getOpacity() == 0.f);

            auto pairs = renderer->getPropertyValuePairs();
            REQUIRE(pairs["opacity"].getNumber() == renderer->getOpacity());
            REQUIRE(renderer->getProperty("Opacity").getNumber() == renderer->getOpacity());
        }

        SECTION("Font")
        {
            REQUIRE(renderer->getFont() == nullptr);

            renderer->setFont("resources/DejaVuSans.ttf");

            auto pairs = renderer->getPropertyValuePairs();
            REQUIRE(pairs["font"].getFont() != nullptr);
            REQUIRE(renderer->getProperty("font").getFont() != nullptr);
            REQUIRE(renderer->getFont() != nullptr);
            REQUIRE(renderer->getFont().getId() == "resources/DejaVuSans.ttf");

            renderer->setFont(nullptr);
            REQUIRE(renderer->getFont() == nullptr);

            tgui::Gui gui;
            gui.add(widget);
            REQUIRE(renderer->getFont() != nullptr);
            REQUIRE(renderer->getFont().getId() == "");
        }

        SECTION("Non-existent property")
        {
            REQUIRE(renderer->getProperty("NonexistentProperty").getType() == tgui::ObjectConverter::Type::None);

            REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", "Text"), tgui::Exception);

            // It might be unexpected, but the property is still set when an exception is thrown in setProperty.
            // This is because the property is set before alerting the widget about the property change.
            REQUIRE(renderer->getProperty("NonexistentProperty").getType() == tgui::ObjectConverter::Type::String);
            REQUIRE(renderer->getProperty("NonexistentProperty").getString() == "Text");
        }

        SECTION("Clone")
        {
            renderer->setOpacity(0.5f);
            renderer->setFont("resources/DejaVuSans.ttf");

            auto clonedRenderer = renderer->clone();
            REQUIRE(clonedRenderer != renderer->getData());
            REQUIRE(clonedRenderer->propertyValuePairs["opacity"].getNumber() == 0.5f);
            REQUIRE(clonedRenderer->propertyValuePairs["font"].getFont().getId() == "resources/DejaVuSans.ttf");
        }

        // TODO: Other tests with the renderer class (e.g. sharing and copying a renderer when using multiple widgets)
    }

    SECTION("Layouts")
    {
        auto container = tgui::Panel::create();
        auto widget2 = tgui::ClickableWidget::create();
        container->add(widget);
        container->add(widget2, "w2");

        widget2->setPosition(100, 50);
        widget2->setSize(600, 150);

        widget->setPosition(20, 10);
        widget->setSize(100, 30);

        SECTION("Position")
        {
            REQUIRE(widget->getPosition() == sf::Vector2f(20, 10));

            widget->setPosition(tgui::bindLeft(widget2), {"w2.y"});
            REQUIRE(widget->getPosition() == sf::Vector2f(100, 50));

            widget2->setPosition(50, 30);
            REQUIRE(widget->getPosition() == sf::Vector2f(50, 30));

            // String layout only works after adding widget to parent
            auto widget3 = widget->clone();
            REQUIRE(widget3->getPosition() == sf::Vector2f(50, 0));
            container->add(widget3);
            REQUIRE(widget3->getPosition() == sf::Vector2f(50, 30));

            // Layout can only be copied when it is a string
            widget2->setPosition(20, 40);
            REQUIRE(widget3->getPosition() == sf::Vector2f(50, 40));

            // String is re-evaluated and new widgets are bound after copying
            widget->setPosition({"{width, height}"});
            REQUIRE(widget->getPosition() == sf::Vector2f(100, 30));
            auto widget4 = widget->clone();
            widget->setSize(40, 50);
            REQUIRE(widget4->getPosition() == sf::Vector2f(100, 30));
            widget4->setSize(60, 70);
            REQUIRE(widget4->getPosition() == sf::Vector2f(60, 70));

            // The move function keeps the bound expression
            widget->setPosition("{width, height}");
            widget->setSize(40, 50);
            widget->move(10, 15);
            widget->move({25, 15});
            REQUIRE(widget->getPosition() == sf::Vector2f(75, 80));
            widget->setSize(20, 30);
            REQUIRE(widget->getPosition() == sf::Vector2f(55, 60));
        }

        SECTION("Size")
        {
            REQUIRE(widget->getSize() == sf::Vector2f(100, 30));

            widget->setSize({"w2.width"}, tgui::bindHeight(widget2));
            REQUIRE(widget->getSize() == sf::Vector2f(600, 150));

            widget2->setSize(50, 30);
            REQUIRE(widget->getSize() == sf::Vector2f(50, 30));

            // String layout only works after adding widget to parent
            auto widget3 = widget->clone();
            REQUIRE(widget3->getSize() == sf::Vector2f(0, 30));
            container->add(widget3);
            REQUIRE(widget3->getSize() == sf::Vector2f(50, 30));

            // Layout can only be copied when it is a string
            widget2->setSize(20, 40);
            REQUIRE(widget3->getSize() == sf::Vector2f(20, 30));

            // String is re-evaluated and new widgets are bound after copying
            widget->setSize({"position"});
            REQUIRE(widget->getSize() == sf::Vector2f(20, 10));
            auto widget4 = widget->clone();
            widget->setPosition(40, 50);
            REQUIRE(widget4->getSize() == sf::Vector2f(20, 10));
            widget4->setPosition(60, 70);
            REQUIRE(widget4->getSize() == sf::Vector2f(60, 70));

            // The scale function keeps the bound expression
            widget->setSize("position");
            widget->setPosition(40, 50);
            widget->scale(2, 5);
            widget->scale({0.25, 0.1});
            REQUIRE(widget->getSize() == sf::Vector2f(20, 25));
            widget->setPosition(20, 30);
            REQUIRE(widget->getSize() == sf::Vector2f(10, 15));
        }
    }

    SECTION("Saving and loading widget with layouts from file")
    {
        auto parent = tgui::Panel::create();
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

    SECTION("Bug Fixes")
    {
        SECTION("Disabled widgets should not be focusable (https://forum.tgui.eu/index.php?topic=384)")
        {
            tgui::Panel::Ptr panel = tgui::Panel::create();
            tgui::EditBox::Ptr editBox = tgui::EditBox::create();
            panel->add(editBox);

            editBox->focus();
            REQUIRE(editBox->isFocused());

            editBox->disable();
            REQUIRE(!editBox->isFocused());

            editBox->focus();
            REQUIRE(!editBox->isFocused());
        }
    }
}
