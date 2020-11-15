/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
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

    SECTION("Focusable")
    {
        REQUIRE(widget->isFocusable());
        widget->setFocusable(false);
        REQUIRE(!widget->isFocusable());
        widget->setFocusable(true);
        REQUIRE(widget->isFocusable());
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
        panel1->add(widget);

        // When the same widget is added to a different parent, a warning is generated but the widget gets moved
        std::streambuf *oldbuf = std::cerr.rdbuf(0);
        panel2->add(widget);
        std::cerr.rdbuf(oldbuf);
        REQUIRE(widget->getParent() == panel2.get());
        REQUIRE(panel1->getWidgets().size() == 0);
    }

    SECTION("ParentGui")
    {
        auto gui = std::make_unique<tgui::GuiSFML>();
        auto panel = tgui::Panel::create();
        panel->add(widget);

        // Before the widgets are added to the gui, they don't have a ParentGui
        REQUIRE(widget->getParentGui() == nullptr);
        REQUIRE(panel->getParentGui() == nullptr);

        // The pointer is set for all children when the container is added to the gui
        gui->add(panel);
        REQUIRE(widget->getParentGui() == gui.get());
        REQUIRE(panel->getParentGui() == gui.get());

        // When the gui is destroyed, the pointer is reset
        gui = nullptr;
        REQUIRE(widget->getParentGui() == nullptr);
        REQUIRE(panel->getParentGui() == nullptr);
    }

    SECTION("MouseCursor")
    {
        REQUIRE(widget->getMouseCursor() == tgui::Cursor::Type::Arrow);
        widget->setMouseCursor(tgui::Cursor::Type::Text);
        REQUIRE(widget->getMouseCursor() == tgui::Cursor::Type::Text);
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
        REQUIRE(widget->getPosition() == tgui::Vector2f(20, 10));

        // Position can be relative to the size of the parent
        widget->setPosition("20%", "10%");
        REQUIRE(widget->getPosition() == tgui::Vector2f(400*0.2f, 300*0.1f));

        // Relative positions are updated when the parent changes size
        container->setSize(800, 150);
        REQUIRE(widget->getPosition() == tgui::Vector2f(800*0.2f, 150*0.1f));

        auto widget2 = widget->clone();
        auto widget3 = widget->clone();

        // Widget clones are unaffected by changes to the original widget
        widget->setPosition("60%", "15%");
        REQUIRE(widget2->getPosition() == tgui::Vector2f(800*0.2f, 150*0.1f));

        // Relative positions of cloned widgets are still updated when the size of the parent changes
        container->add(widget2);
        container->setSize(400, 300);
        REQUIRE(widget2->getPosition() == tgui::Vector2f(400*0.2f, 300*0.1f));

        // There is however no relation with the old parent anymore unless the widget is added again
        REQUIRE(widget3->getPosition() == tgui::Vector2f(800*0.2f, 150*0.1f));
        container->add(widget3);
        REQUIRE(widget3->getPosition() == tgui::Vector2f(400*0.2f, 300*0.1f));

        // The position can be changed from a relative to an absolute value at any time
        widget2->setPosition(60, 70);
        REQUIRE(widget2->getPosition() == tgui::Vector2f(60, 70));

        // Relative positions only work with a parent
        auto widget4 = std::make_shared<tgui::ClickableWidget>();
        widget4->setPosition("20%", "10%");
        REQUIRE(widget4->getPosition() == tgui::Vector2f(0, 0));
        container->add(widget4);
        REQUIRE(widget4->getPosition() == tgui::Vector2f(400*0.2f, 300*0.1f));
    }

    SECTION("Size")
    {
        auto container = tgui::Panel::create();
        container->setSize(400, 300);
        container->add(widget);

        widget->setPosition(20, 10);
        widget->setSize(100, 30);

        // Size can be absolute
        REQUIRE(widget->getSize() == tgui::Vector2f(100, 30));

        // Size can be relative to the size of the parent
        widget->setSize("30%", "5%");
        REQUIRE(widget->getSize() == tgui::Vector2f(400*0.3f, 300*0.05f));

        // Relative sizes are updated when the parent changes size
        container->setSize(800, 150);
        REQUIRE(widget->getSize() == tgui::Vector2f(800*0.3f, 150*0.05f));

        auto widget2 = widget->clone();
        auto widget3 = widget->clone();

        // Widget clones are unaffected by changes to the original widget
        widget->setSize("60%", "15%");
        REQUIRE(widget2->getSize() == tgui::Vector2f(800*0.3f, 150*0.05f));

        // Relative sizes of cloned widgets are still updated when the size of the parent changes
        container->add(widget2);
        container->setSize(400, 300);
        REQUIRE(widget2->getSize() == tgui::Vector2f(400*0.3f, 300*0.05f));

        // There is however no relation with the old parent anymore unless the widget is added again
        REQUIRE(widget3->getSize() == tgui::Vector2f(800*0.3f, 150*0.05f));
        container->add(widget3);
        REQUIRE(widget3->getSize() == tgui::Vector2f(400*0.3f, 300*0.05f));

        // The size can be changed from a relative to an absolute value at any time
        widget2->setSize(70, 20);
        REQUIRE(widget2->getSize() == tgui::Vector2f(70, 20));

        // Relative sizes only work with a parent
        auto widget4 = std::make_shared<tgui::ClickableWidget>();
        widget4->setSize("30%", "5%");
        REQUIRE(widget4->getSize() == tgui::Vector2f(0, 0));
        container->add(widget4);
        REQUIRE(widget4->getSize() == tgui::Vector2f(400*0.3f, 300*0.05f));

        auto widget5 = tgui::ClickableWidget::copy(widget4);
        widget5->setWidth(120);
        REQUIRE(widget5->getSize() == tgui::Vector2f(120, 300*0.05f));

        auto widget6 = tgui::ClickableWidget::copy(widget4);
        widget6->setHeight(80);
        REQUIRE(widget6->getSize() == tgui::Vector2f(400*0.3f, 80));
    }

    SECTION("Origin")
    {
        REQUIRE(widget->getOrigin() == tgui::Vector2f(0, 0));
        widget->setOrigin(1, 0.5f);
        REQUIRE(widget->getOrigin() == tgui::Vector2f(1, 0.5f));
        widget->setOrigin({0.5, 1});
        REQUIRE(widget->getOrigin() == tgui::Vector2f(0.5f, 1));
    }

    SECTION("Scale")
    {
        REQUIRE(widget->getScale() == tgui::Vector2f(1, 1));
        widget->setScale({2, 0.5f});
        REQUIRE(widget->getScale() == tgui::Vector2f(2, 0.5f));
        widget->setScale(2);
        REQUIRE(widget->getScale() == tgui::Vector2f(2, 2));

        widget->setOrigin(0.3f, 0.4f);
        widget->setScale(3);
        REQUIRE(widget->getScaleOrigin() == tgui::Vector2f(0.3f, 0.4f));
        widget->setScale(3, {0.8f, 0.7f});
        REQUIRE(widget->getScaleOrigin() == tgui::Vector2f(0.8f, 0.7f));

        widget->setScale({1.5f, 4});
        REQUIRE(widget->getScaleOrigin() == tgui::Vector2f(0.3f, 0.4f));
        widget->setScale({1.5f, 4}, {0.8f, 0.7f});
        REQUIRE(widget->getScaleOrigin() == tgui::Vector2f(0.8f, 0.7f));
    }

    SECTION("Rotation")
    {
        REQUIRE(widget->getRotation() == 0);
        widget->setRotation(60);
        REQUIRE(widget->getRotation() == 60);

        widget->setOrigin(0.3f, 0.4f);
        widget->setRotation(50);
        REQUIRE(widget->getRotationOrigin() == tgui::Vector2f(0.3f, 0.4f));
        widget->setRotation(50, {0.8f, 0.7f});
        REQUIRE(widget->getRotationOrigin() == tgui::Vector2f(0.8f, 0.7f));
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
            REQUIRE(pairs["Opacity"].getNumber() == renderer->getOpacity());
            REQUIRE(renderer->getProperty("Opacity").getNumber() == renderer->getOpacity());
        }

        SECTION("OpacityDisabled")
        {
            // Default value is -1, indicating that the Opacity propery will be used if when the widget is disabled
            REQUIRE(renderer->getOpacityDisabled() == -1.f);

            renderer->setOpacityDisabled(0.5f);
            REQUIRE(renderer->getOpacityDisabled() == 0.5f);

            renderer->setOpacityDisabled(2.f);
            REQUIRE(renderer->getOpacityDisabled() == 1.f);

            renderer->setOpacityDisabled(-2.f);
            REQUIRE(renderer->getOpacityDisabled() == 0.f);

            // -1 is the only value outside the [0,1] range that is accepted
            renderer->setOpacityDisabled(-1.f);
            REQUIRE(renderer->getOpacityDisabled() == -1.f);

            auto pairs = renderer->getPropertyValuePairs();
            REQUIRE(pairs["OpacityDisabled"].getNumber() == renderer->getOpacityDisabled());
            REQUIRE(renderer->getProperty("OpacityDisabled").getNumber() == renderer->getOpacityDisabled());
        }

        SECTION("Font")
        {
            REQUIRE(renderer->getFont() == nullptr);

            renderer->setFont("resources/DejaVuSans.ttf");

            auto pairs = renderer->getPropertyValuePairs();
            REQUIRE(pairs["Font"].getFont() != nullptr);
            REQUIRE(renderer->getProperty("Font").getFont() != nullptr);
            REQUIRE(renderer->getFont() != nullptr);
            REQUIRE(renderer->getFont().getId() == "resources/DejaVuSans.ttf");

            renderer->setFont(nullptr);
            REQUIRE(renderer->getFont() == nullptr);

            // Inheriting the font does not change the renderer (but the widget will have a usable font)
            tgui::GuiSFML gui;
            gui.add(widget);
            REQUIRE(renderer->getFont() == nullptr);
        }

        SECTION("Non-existent property")
        {
            REQUIRE(renderer->getProperty("NonexistentProperty").getType() == tgui::ObjectConverter::Type::None);

            REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", "Text"), tgui::Exception);

            // Calling setProperty with a nonexistent property will not create that property
            REQUIRE(renderer->getProperty("NonexistentProperty").getType() == tgui::ObjectConverter::Type::None);
        }

        SECTION("Clone")
        {
            renderer->setOpacity(0.5f);
            renderer->setFont("resources/DejaVuSans.ttf");

            auto clonedRenderer = renderer->clone();
            REQUIRE(clonedRenderer != renderer->getData());
            REQUIRE(clonedRenderer->propertyValuePairs["Opacity"].getNumber() == 0.5f);
            REQUIRE(clonedRenderer->propertyValuePairs["Font"].getFont().getId() == "resources/DejaVuSans.ttf");
        }

        // TODO: Other tests with the renderer class (e.g. sharing and copying a renderer when using multiple widgets)
    }

    SECTION("Saving and loading widget from file")
    {
        auto parent = tgui::Panel::create();
        parent->add(widget, "Widget Name.With:Special{Chars}");

        widget->setVisible(false);
        widget->setEnabled(false);
        widget->setMouseCursor(tgui::Cursor::Type::Hand);
        widget->setPosition(50, "15%");
        widget->setSize("min(20% - (10 * 5), 100)", "70");

        tgui::String userData = "Main Widget User Data .With:Special{Chars}";
        widget->setUserData(userData);
        REQUIRE(widget->getUserData<tgui::String>() == userData);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileClickableWidget1.txt"));
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileClickableWidget1.txt"));
        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileClickableWidget2.txt"));
        REQUIRE(compareFiles("WidgetFileClickableWidget1.txt", "WidgetFileClickableWidget2.txt"));

        REQUIRE(!parent->get("Widget Name.With:Special{Chars}")->isVisible());
        REQUIRE(!parent->get("Widget Name.With:Special{Chars}")->isEnabled());
        REQUIRE(parent->get("Widget Name.With:Special{Chars}")->getUserData<tgui::String>() == userData);

        parent->removeAllWidgets();
        parent->loadWidgetsFromFile("WidgetFileClickableWidget1.txt");
        REQUIRE(parent->getWidgets().size() == 1);
        parent->loadWidgetsFromFile("WidgetFileClickableWidget1.txt");
        REQUIRE(parent->getWidgets().size() == 1);
        parent->loadWidgetsFromFile("WidgetFileClickableWidget1.txt", false);
        REQUIRE(parent->getWidgets().size() == 2);
    }

    SECTION("Draw")
    {
        SECTION("Origin, scale and rotation")
        {
            auto outerPanel = tgui::Panel::create();
            outerPanel->setSize(275, 220);
            outerPanel->setPosition(180, 180);
            outerPanel->getRenderer()->setBackgroundColor({180, 180, 180});
            outerPanel->getRenderer()->setBorders({2});

            TEST_DRAW_INIT(360, 360, outerPanel)

            auto innerPanel = tgui::Panel::create();
            innerPanel->setSize(150, 230);
            innerPanel->getRenderer()->setBackgroundColor({220, 220, 220});
            innerPanel->getRenderer()->setBorders({2});
            outerPanel->add(innerPanel);

            auto button = tgui::Button::create();
            button->setPosition(20, 130);
            button->setText("Done");
            innerPanel->add(button);

            auto label = tgui::Label::create();
            label->setText("Functions added to TGUI 0.9-dev:");
            label->setPosition(20, 20);
            outerPanel->add(label);

            auto checkBox = tgui::CheckBox::create();
            checkBox->setText("setOrigin");
            checkBox->setChecked(true);
            checkBox->setPosition({10, 10});
            innerPanel->add(checkBox);

            checkBox = tgui::CheckBox::create();
            checkBox->setText("setRotation");
            checkBox->setChecked(true);
            checkBox->setPosition({10, 50});
            innerPanel->add(checkBox);

            checkBox = tgui::CheckBox::create();
            checkBox->setText("setScale");
            checkBox->setChecked(true);
            checkBox->setPosition({10, 90});
            innerPanel->add(checkBox);

            outerPanel->setOrigin({0.5f, 0.5f});
            outerPanel->setRotation(-45);

            innerPanel->setOrigin({0, 1});
            innerPanel->setPosition(20, 45);
            innerPanel->setRotation(90);

            button->setScale({2, 4});

            TEST_DRAW("OriginScaleRotation.png")
        }
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
