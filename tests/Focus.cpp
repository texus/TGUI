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
#include <TGUI/Widgets/RadioButtonGroup.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <stack>
#include <set>

static tgui::Container::Ptr rootContainer;

static bool noWidgetsFocused(tgui::Container::Ptr root)
{
    for (auto& widget : root->getWidgets())
    {
        if (widget->isFocused())
            return false;

        tgui::Container::Ptr container = std::dynamic_pointer_cast<tgui::Container>(widget);
        if (container != nullptr)
        {
            if (!noWidgetsFocused(container))
                return false;
        }
    }

    return true;
}

static bool widgetFocused(tgui::Widget::Ptr widget)
{
    if (!widget->isFocused())
        return false;

    std::set<tgui::Container*> parents;
    tgui::Container* parent = widget->getParent();
    while (parent)
    {
        parents.insert(parent);
        parent = parent->getParent();
    }

    std::stack<tgui::Container::Ptr> containers;
    containers.push(rootContainer);
    while (!containers.empty())
    {
        const tgui::Container::Ptr container = containers.top();
        containers.pop();

        for (auto& child : container->getWidgets())
        {
            if (child == widget)
                continue;

            tgui::Container::Ptr childContainer = std::dynamic_pointer_cast<tgui::Container>(child);
            if (childContainer)
            {
                containers.push(childContainer);

                // Parent of widget has to be focused, all other containers have to be unfocused
                if (parents.find(childContainer.get()) != parents.end())
                {
                    if (!child->isFocused())
                        return false;
                }
                else // Not a parent
                {
                    if (child->isFocused())
                        return false;
                }
            }
            else // Not a container
            {
                if (child->isFocused())
                    return false;
            }
        }
    }

    return true;
}


/// Widgets hierarchy:
//
// Button
// Group
//     CheckBox
//     Panel
//         Group
//             RadioButtonGroup
//                 RadioButton
//                 RadioButton
//         EditBox
//     TextBox
//     CheckBox
// Panel
//     EditBox
//     EditBox
//     Label
//     Button (invisible)
//     Button (disabled)
//     Button
// ChildWindow
//     Button
// ChildWindow

TEST_CASE("[Focussing widgets]")
{
    tgui::Gui gui;
    rootContainer = gui.getContainer();

    auto button = tgui::Button::create("Click me");
    button->setPosition({20, 10});
    button->setSize({80, 25});
    rootContainer->add(button);

    auto outerGroup = tgui::Group::create();
    outerGroup->setPosition({20, 50});
    outerGroup->setSize({220, 270});
    rootContainer->add(outerGroup);

    auto checkBox1 = tgui::CheckBox::create("Check 1");
    checkBox1->setPosition({10, 10});
    outerGroup->add(checkBox1);

    auto panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor(sf::Color::Yellow);
    panel->setPosition({10, 40});
    panel->setSize({200, 130});
    outerGroup->add(panel);

    auto group = tgui::Group::create();
    group->setPosition({10, 10});
    group->setSize({180, 110});
    panel->add(group);

    auto radioButtonGroup = tgui::RadioButtonGroup::create();
    group->add(radioButtonGroup);

    auto radioButton1 = tgui::RadioButton::create();
    radioButton1->setPosition({10, 10});
    radioButton1->setText("Option 1");
    radioButtonGroup->add(radioButton1);

    auto radioButton2 = tgui::RadioButton::create();
    radioButton2->setPosition({10, 40});
    radioButton2->setText("Option 2");
    radioButton2->setChecked(true);
    radioButtonGroup->add(radioButton2);

    auto editBox = tgui::EditBox::create();
    editBox->setPosition({10, 80});
    editBox->setSize({150, 20});
    editBox->setText("Some text");
    panel->add(editBox);

    auto textBox = tgui::TextBox::create();
    textBox->setSize({160, 60});
    textBox->setPosition({10, 180});
    textBox->setText("TextBox\nFilled with text");
    outerGroup->add(textBox);

    auto checkBox2 = tgui::CheckBox::create("Check 2");
    checkBox2->setPosition({10, 250});
    outerGroup->add(checkBox2);

    auto outerPanel = tgui::Panel::create();
    outerPanel->getRenderer()->setBackgroundColor(sf::Color::Green);
    outerPanel->setPosition({20, 330});
    outerPanel->setSize({200, 130});
    rootContainer->add(outerPanel);

    auto editBoxUsername = tgui::EditBox::create();
    editBoxUsername->setPosition({10, 10});
    editBoxUsername->setSize({180, 20});
    editBoxUsername->setDefaultText("Username");
    outerPanel->add(editBoxUsername);

    auto editBoxPassword = tgui::EditBox::create();
    editBoxPassword->setPosition({10, 40});
    editBoxPassword->setSize({180, 20});
    editBoxPassword->setDefaultText("Password");
    outerPanel->add(editBoxPassword);

    auto label = tgui::Label::create("Hidden button:");
    label->setPosition({10, 70});
    outerPanel->add(label);

    auto invisibleButton = tgui::Button::create("Hidden");
    invisibleButton->setPosition({120, 70});
    invisibleButton->setSize({60, 20});
    invisibleButton->setVisible(false);
    outerPanel->add(invisibleButton);

    auto buttonLogin = tgui::Button::create("Log in");
    buttonLogin->setPosition({10, 100});
    buttonLogin->setSize({85, 20});
    outerPanel->add(buttonLogin);

    auto buttonRegister = tgui::Button::create("Register");
    buttonRegister->setPosition({105, 100});
    buttonRegister->setSize({85, 20});
    buttonRegister->setEnabled(false);
    outerPanel->add(buttonRegister);

    auto childWindow = tgui::ChildWindow::create("Window 1");
    childWindow->setSize({100, 30});
    childWindow->setPosition({10, 470});
    rootContainer->add(childWindow);

    auto childButton = tgui::Button::create("Button");
    childButton->setSize({90, 20});
    childButton->setPosition({5, 5});
    childWindow->add(childButton);

    auto emptyChildWindow = tgui::ChildWindow::create("Window 2");
    emptyChildWindow->setSize({100, 30});
    emptyChildWindow->setPosition({120, 470});
    rootContainer->add(emptyChildWindow);

    SECTION("Focus method")
    {
        REQUIRE(noWidgetsFocused(rootContainer));

        button->setFocused(true);
        REQUIRE(widgetFocused(button));

        outerPanel->setFocused(true);
        REQUIRE(widgetFocused(outerPanel));

        radioButton2->setFocused(true);
        REQUIRE(widgetFocused(radioButton2));

        editBoxPassword->setFocused(true);
        REQUIRE(widgetFocused(editBoxPassword));

        // Invisible widget can't be focused
        invisibleButton->setFocused(true);
        REQUIRE(widgetFocused(editBoxPassword));

        // Disabled widget can't be focused
        buttonRegister->setFocused(true);
        REQUIRE(widgetFocused(editBoxPassword));

        // Some widgets, like Label, can't be focused
        label->setFocused(true);
        REQUIRE(widgetFocused(editBoxPassword));

        // Unfocusing container unfocuses all child widgets
        rootContainer->setFocused(false);
        REQUIRE(noWidgetsFocused(rootContainer));
    }

    SECTION("focusNextWidget")
    {
        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(button));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(checkBox1));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(textBox));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(checkBox2));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(button));

        radioButton2->setFocused(true);
        REQUIRE(widgetFocused(radioButton2));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(editBox));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(radioButton1));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(radioButton2));

        editBoxUsername->setFocused(true);
        REQUIRE(widgetFocused(editBoxUsername));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(editBoxPassword));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(buttonLogin));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(editBoxUsername));

        childButton->setFocused(true);
        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(childButton));

        outerPanel->setFocused(true);
        REQUIRE(widgetFocused(outerPanel));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(editBoxPassword)); // editBoxUsername was focused last time the panel was focused

        emptyChildWindow->setFocused(true);
        REQUIRE(widgetFocused(emptyChildWindow));

        rootContainer->focusNextWidget();
        REQUIRE(widgetFocused(button));
    }

    SECTION("focusPreviousWidget")
    {
        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(checkBox2));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(textBox));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(checkBox1));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(button));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(checkBox2));

        radioButton2->setFocused(true);
        REQUIRE(widgetFocused(radioButton2));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(radioButton1));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(editBox));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(radioButton2));

        editBoxPassword->setFocused(true);
        REQUIRE(widgetFocused(editBoxPassword));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(editBoxUsername));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(buttonLogin));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(editBoxPassword));

        childButton->setFocused(true);
        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(childButton));

        outerPanel->setFocused(true);
        REQUIRE(widgetFocused(outerPanel));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(editBoxUsername)); // editBoxPassword was focused last time the panel was focused

        emptyChildWindow->setFocused(true);
        REQUIRE(widgetFocused(emptyChildWindow));

        rootContainer->focusPreviousWidget();
        REQUIRE(widgetFocused(checkBox2));
    }

    SECTION("Gui interaction")
    {
        sf::RenderTexture target;
        target.create(250, 530);
        gui.setTarget(target);

        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton = sf::Event::MouseButtonEvent();
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = 55;
        event.mouseButton.y = 145;
        gui.handleEvent(event);
        REQUIRE(widgetFocused(radioButton2));

        event.type = sf::Event::MouseButtonReleased;
        gui.handleEvent(event);
        REQUIRE(widgetFocused(radioButton2));

        event.type = sf::Event::KeyPressed;
        event.key = sf::Event::KeyEvent();
        event.key.control = false;
        event.key.alt     = false;
        event.key.shift   = false;
        event.key.system  = false;
        event.key.code    = sf::Keyboard::Tab;
        gui.handleEvent(event);
        REQUIRE(widgetFocused(editBox));

        event.type = sf::Event::KeyReleased;
        gui.handleEvent(event);
        REQUIRE(widgetFocused(editBox));

        event.type = sf::Event::KeyPressed;
        event.key.shift = true;
        gui.handleEvent(event);
        event.type = sf::Event::KeyReleased;
        gui.handleEvent(event);
        REQUIRE(widgetFocused(radioButton2));

        event.type = sf::Event::KeyPressed;
        gui.handleEvent(event);
        event.type = sf::Event::KeyReleased;
        gui.handleEvent(event);
        REQUIRE(widgetFocused(radioButton1));
    }

    rootContainer = nullptr;
}
