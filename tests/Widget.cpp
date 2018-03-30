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
#include <TGUI/Gui.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/EditBox.hpp>

TEST_CASE("[Widget]")
{
    tgui::Widget::Ptr widget = tgui::ClickableWidget::create();

    SECTION("Visibile")
    {
        REQUIRE(widget->isVisible());
        widget->setVisible(false);
        REQUIRE(!widget->isVisible());
        widget->setVisible(true);
        REQUIRE(widget->isVisible());
    }

    SECTION("Enabled")
    {
        REQUIRE(widget->isEnabled());
        widget->setEnabled(false);
        REQUIRE(!widget->isEnabled());
        widget->setEnabled(true);
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

    SECTION("Position")
    {
        auto container = tgui::Panel::create();
        container->setSize(400, 300);
        container->add(widget);

        widget->setPosition(20, 10);
        widget->setSize(100, 30);

        // Position can be absolute
        REQUIRE(widget->getPosition() == sf::Vector2f(20, 10));

        // Position can be relative to the size of the parent
        widget->setPosition("20%", "10%");
        REQUIRE(widget->getPosition() == sf::Vector2f(400*0.2f, 300*0.1f));

        // Relative positions are updated when the parent changes size
        container->setSize(800, 150);
        REQUIRE(widget->getPosition() == sf::Vector2f(800*0.2f, 150*0.1f));

        auto widget2 = widget->clone();
        auto widget3 = widget->clone();

        // Widget clones are unaffected by changes to the original widget
        widget->setPosition("60%", "15%");
        REQUIRE(widget2->getPosition() == sf::Vector2f(800*0.2f, 150*0.1f));

        // Relative positions of cloned widgets are still updated when the size of the parent changes
        container->add(widget2);
        container->setSize(400, 300);
        REQUIRE(widget2->getPosition() == sf::Vector2f(400*0.2f, 300*0.1f));

        // There is however no relation with the old parent anymore unless the widget is added again
        REQUIRE(widget3->getPosition() == sf::Vector2f(800*0.2f, 150*0.1f));
        container->add(widget3);
        REQUIRE(widget3->getPosition() == sf::Vector2f(400*0.2f, 300*0.1f));

        // The position can be changed from a relative to an absolute value at any time
        widget2->setPosition(60, 70);
        REQUIRE(widget2->getPosition() == sf::Vector2f(60, 70));

        // Relative positions only work with a parent
        auto widget4 = std::make_shared<tgui::ClickableWidget>();
        widget4->setPosition("20%", "10%");
        REQUIRE(widget4->getPosition() == sf::Vector2f(0, 0));
        container->add(widget4);
        REQUIRE(widget4->getPosition() == sf::Vector2f(400*0.2f, 300*0.1f));
    }

    SECTION("Size")
    {
        auto container = tgui::Panel::create();
        container->setSize(400, 300);
        container->add(widget);

        widget->setPosition(20, 10);
        widget->setSize(100, 30);

        // Size can be absolute
        REQUIRE(widget->getSize() == sf::Vector2f(100, 30));

        // Size can be relative to the size of the parent
        widget->setSize("30%", "5%");
        REQUIRE(widget->getSize() == sf::Vector2f(400*0.3f, 300*0.05f));

        // Relative sizes are updated when the parent changes size
        container->setSize(800, 150);
        REQUIRE(widget->getSize() == sf::Vector2f(800*0.3f, 150*0.05f));

        auto widget2 = widget->clone();
        auto widget3 = widget->clone();

        // Widget clones are unaffected by changes to the original widget
        widget->setSize("60%", "15%");
        REQUIRE(widget2->getSize() == sf::Vector2f(800*0.3f, 150*0.05f));

        // Relative sizes of cloned widgets are still updated when the size of the parent changes
        container->add(widget2);
        container->setSize(400, 300);
        REQUIRE(widget2->getSize() == sf::Vector2f(400*0.3f, 300*0.05f));

        // There is however no relation with the old parent anymore unless the widget is added again
        REQUIRE(widget3->getSize() == sf::Vector2f(800*0.3f, 150*0.05f));
        container->add(widget3);
        REQUIRE(widget3->getSize() == sf::Vector2f(400*0.3f, 300*0.05f));

        // The size can be changed from a relative to an absolute value at any time
        widget2->setSize(70, 20);
        REQUIRE(widget2->getSize() == sf::Vector2f(70, 20));

        // Relative sizes only work with a parent
        auto widget4 = std::make_shared<tgui::ClickableWidget>();
        widget4->setSize("30%", "5%");
        REQUIRE(widget4->getSize() == sf::Vector2f(0, 0));
        container->add(widget4);
        REQUIRE(widget4->getSize() == sf::Vector2f(400*0.3f, 300*0.05f));
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

            // Inheriting the font does not change the renderer (but the widget will have a usable font)
            tgui::Gui gui;
            gui.add(widget);
            REQUIRE(renderer->getFont() == nullptr);
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

    SECTION("Saving and loading widget from file")
    {
        auto parent = tgui::Panel::create();
        parent->add(widget, "Widget Name.With:Special{Chars}");

        widget->setVisible(false);
        widget->setEnabled(false);
        widget->setPosition(50, "15%");
        widget->setSize("30%", "70");

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileClickableWidget1.txt"));
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileClickableWidget1.txt"));
        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileClickableWidget2.txt"));
        REQUIRE(compareFiles("WidgetFileClickableWidget1.txt", "WidgetFileClickableWidget2.txt"));

        REQUIRE(!parent->get("Widget Name.With:Special{Chars}")->isVisible());
        REQUIRE(!parent->get("Widget Name.With:Special{Chars}")->isEnabled());
    }

    SECTION("Bug Fixes")
    {
        SECTION("Disabled widgets should not be focusable (https://forum.tgui.eu/index.php?topic=384)")
        {
            tgui::Panel::Ptr panel = tgui::Panel::create();
            tgui::EditBox::Ptr editBox = tgui::EditBox::create();
            panel->add(editBox);

            editBox->setFocused(true);
            REQUIRE(editBox->isFocused());

            editBox->setEnabled(false);
            REQUIRE(!editBox->isFocused());

            editBox->setFocused(true);
            REQUIRE(!editBox->isFocused());
        }
    }
}
