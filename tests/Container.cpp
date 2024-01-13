/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[Container]")
{
    auto container = std::make_shared<GuiNull>();

    auto widget1 = tgui::Label::create();
    auto widget2 = tgui::Panel::create();
    auto widget3 = tgui::Label::create();
    auto widget4 = tgui::Label::create();
    auto widget5 = tgui::Label::create();
    container->add(widget1, "w1");
    container->add(widget2, "w2");
    container->add(widget3, "w3");
    widget2->add(widget4, "w4");
    widget2->add(widget5, "w5");

    SECTION("default font")
    {
        REQUIRE(tgui::Font::getGlobalFont() != nullptr);
        REQUIRE(std::make_shared<GuiNull>()->getFont() != nullptr);
        REQUIRE(tgui::Panel::create()->getRenderer()->getFont() == nullptr);
    }

    SECTION("add")
    {
        container->removeAllWidgets();

        auto w1 = tgui::ClickableWidget::create();
        auto w2 = tgui::ClickableWidget::create();
        auto w3 = tgui::ClickableWidget::create();

        REQUIRE(container->getWidgets().empty());

        container->add(w1, "widget1");
        container->add(w2, "");
        container->add(w3, "widget3");

        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == w1);
        REQUIRE(container->getWidgets()[1] == w2);
        REQUIRE(container->getWidgets()[2] == w3);

        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0]->getWidgetName() == "widget1");
        REQUIRE(container->getWidgets()[1]->getWidgetName() == "");
        REQUIRE(container->getWidgets()[2]->getWidgetName() == "widget3");
    }

    SECTION("get")
    {
        SECTION("normal function")
        {
            REQUIRE(container->get("w0") == nullptr); // non-existing child
            REQUIRE(container->get("w1") == widget1); // direct child
            REQUIRE(container->get("w5") == widget5); // indirect child
        }

        SECTION("templated function")
        {
            REQUIRE(container->get<tgui::Label>("w6") == nullptr); // non-existing child
            REQUIRE(container->get<tgui::Panel>("w2") == widget2); // direct child
            REQUIRE(container->get<tgui::Label>("w4") == widget4); // indirect child
        }

        SECTION("Recursive search")
        {
            container->removeAllWidgets();

            auto child1 = tgui::Panel::create();
            auto child2 = tgui::Panel::create();
            auto child21 = tgui::Panel::create();
            auto child211 = tgui::Button::create();
            auto child22 = tgui::Panel::create();
            auto child221 = tgui::TabContainer::create();
            container->add(child1, "1");
            container->add(child2, "2");
            child2->add(child21, "21");
            child21->add(child211, "211");
            child2->add(child22, "22");
            child22->add(child221, "221");

            child221->addTab("Tab1");
            child221->addTab("Tab2");
            auto tab3 = child221->addTab("Tab3");
            child221->addTab("Tab4");
            auto child22131 = tgui::SpinButton::create();
            tab3->add(child22131, "22131");

            REQUIRE(container->get("1") == child1);
            REQUIRE(container->get("2") == child2);
            REQUIRE(container->get("21") == child21);
            REQUIRE(container->get("211") == child211);
            REQUIRE(container->get("22") == child22);
            REQUIRE(container->get("221") == child221);
            REQUIRE(container->get("22131") == child22131);
        }

        SECTION("reusing name")
        {
            container->removeAllWidgets();

            auto child1 = tgui::Panel::create();
            auto child2 = tgui::Panel::create();
            auto child3 = tgui::Panel::create();
            auto child4 = tgui::Panel::create();

            // Container does not contain a widget with this name yet
            container->add(child1);
            REQUIRE(container->get("name") == nullptr);

            // The indirect child is found
            child1->add(child4, "name");
            REQUIRE(container->get("name") == child4);

            // The direct child is found
            container->add(child2, "name");
            REQUIRE(container->get("name") == child2);

            // The first direct child is still found
            container->add(child3, "name");
            REQUIRE(container->get("name") == child2);

            // The second direct child can be found if the order is manipulated
            child3->moveToBack();
            REQUIRE(container->get("name") == child3);

            // Removing the found child allows the previous child to be found again
            container->remove(child3);
            REQUIRE(container->get("name") == child2);

            // Removing the found child allows the indirect child to be found again
            container->remove(child2);
            REQUIRE(container->get("name") == child4);

            // Removing the found indirect child means that there is no more widget with that name
            child1->remove(child4);
            REQUIRE(container->get("name") == nullptr);
        }
    }

    SECTION("remove")
    {
        SECTION("remove with correct parameter")
        {
            REQUIRE(container->getWidgets().size() == 3);
            REQUIRE(widget2->getWidgets().size() == 2);

            REQUIRE(container->remove(widget2));
            REQUIRE(widget2->remove(widget5));

            REQUIRE(container->getWidgets().size() == 2);
            REQUIRE(container->getWidgets()[0] == widget1);
            REQUIRE(container->getWidgets()[1] == widget3);
            REQUIRE(container->getWidgets()[0]->getWidgetName() == "w1");
            REQUIRE(container->getWidgets()[1]->getWidgetName()  == "w3");

            REQUIRE(widget2->getWidgets().size() == 1);
            REQUIRE(widget2->getWidgets()[0] == widget4);
            REQUIRE(widget2->getWidgets()[0]->getWidgetName()  == "w4");

            REQUIRE(widget2->remove(widget4));

            REQUIRE(widget2->getWidgets().empty());
        }

        SECTION("remove with wrong parameter")
        {
            REQUIRE(!container->remove(widget5));
            REQUIRE(!container->remove(nullptr));
            REQUIRE(!container->remove(tgui::ClickableWidget::create()));
        }

        SECTION("remove all widgets")
        {
            REQUIRE(container->getWidgets().size() == 3);

            container->removeAllWidgets();

            REQUIRE(container->getWidgets().empty());
        }
    }

    SECTION("Change z-order")
    {
        widget2->removeAllWidgets();
        container->removeAllWidgets();

        // moveWidgetForward and moveWidgetBackward will return list size when widget hasn't been added yet
        REQUIRE(container->moveWidgetForward(widget3) == 0);
        container->add(widget1);
        container->add(widget2);
        REQUIRE(container->moveWidgetBackward(widget3) == 2);
        container->add(widget3);

        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == widget1);
        REQUIRE(container->getWidgets()[1] == widget2);
        REQUIRE(container->getWidgets()[2] == widget3);
        REQUIRE(container->getWidgetIndex(widget1) == 0);
        REQUIRE(container->getWidgetIndex(widget2) == 1);
        REQUIRE(container->getWidgetIndex(widget3) == 2);
        REQUIRE(container->getWidgetIndex(widget4) == -1);

        container->moveWidgetToFront(widget1);
        REQUIRE(container->getWidgets()[0] == widget2);
        REQUIRE(container->getWidgets()[1] == widget3);
        REQUIRE(container->getWidgets()[2] == widget1);

        container->moveWidgetToBack(widget1);
        REQUIRE(container->getWidgets()[0] == widget1);
        REQUIRE(container->getWidgets()[1] == widget2);
        REQUIRE(container->getWidgets()[2] == widget3);

        REQUIRE(container->moveWidgetForward(widget1) == 1);
        REQUIRE(container->moveWidgetForward(widget3) == 2);
        REQUIRE(container->getWidgets()[0] == widget2);
        REQUIRE(container->getWidgets()[1] == widget1);
        REQUIRE(container->getWidgets()[2] == widget3);

        REQUIRE(container->moveWidgetBackward(widget3) == 1);
        REQUIRE(container->moveWidgetBackward(widget2) == 0);
        REQUIRE(container->getWidgets()[0] == widget2);
        REQUIRE(container->getWidgets()[1] == widget3);
        REQUIRE(container->getWidgets()[2] == widget1);
        REQUIRE(container->getWidgetIndex(widget1) == 2);
        REQUIRE(container->getWidgetIndex(widget2) == 0);
        REQUIRE(container->getWidgetIndex(widget3) == 1);

        // No widgets were added or removed while changing z-order
        REQUIRE(container->getWidgets().size() == 3);

        container->removeAllWidgets();

        container->add(widget1);
        container->add(widget2);
        container->add(widget3);
        container->add(widget4);
        container->add(widget5);
        REQUIRE(container->setWidgetIndex(widget2, 3)); // Place widget2 after widget4
        REQUIRE(container->setWidgetIndex(widget1, 4)); // Place widget1 after widget5
        REQUIRE(container->setWidgetIndex(widget3, 0)); // Place widget3 before widget4
        REQUIRE(container->setWidgetIndex(widget4, 1)); // Keeps widget4 in the same spot
        REQUIRE(!container->setWidgetIndex(widget3, 5)); // Index too high
        REQUIRE(!container->setWidgetIndex(tgui::Button::create(), 1)); // No such widget

        REQUIRE(container->getWidgetIndex(widget1) == 4);
        REQUIRE(container->getWidgetIndex(widget2) == 2);
        REQUIRE(container->getWidgetIndex(widget3) == 0);
        REQUIRE(container->getWidgetIndex(widget4) == 1);
        REQUIRE(container->getWidgetIndex(widget5) == 3);
        REQUIRE(container->getWidgetIndex(tgui::Button::create()) == -1);

        // No widgets were added or removed while changing z-order
        REQUIRE(container->getWidgets().size() == 5);
    }

    SECTION("widget name")
    {
        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(widget1->getWidgetName() == "w1");
        REQUIRE(widget2->Widget::getWidgetName() == "w2");
        REQUIRE(widget3->getWidgetName() == "w3");

        widget1->setWidgetName("w001");
        widget3->setWidgetName("w003");

        REQUIRE(widget1->getWidgetName() == "w001");
        REQUIRE(widget2->Widget::getWidgetName() == "w2");
        REQUIRE(widget3->getWidgetName() == "w003");
    }

    SECTION("sortWidgets")
    {
        REQUIRE(container->getWidgets().size() == 3);
        widget1->setWidgetName("1");
        widget2->setWidgetName("2");
        widget3->setWidgetName("3");

        container->getContainer()->sortWidgets([](const tgui::Widget::Ptr& p, const tgui::Widget::Ptr& p2){
            return p->getWidgetName()[0] < p2->getWidgetName()[0];
        });

        REQUIRE(container->getWidgets()[0] == widget1);
        REQUIRE(container->getWidgets()[1] == widget2);
        REQUIRE(container->getWidgets()[2] == widget3);
    }

    SECTION("focus")
    {
        auto editBox1 = tgui::EditBox::create();
        tgui::EditBox::Ptr editBox2 = tgui::EditBox::create();
        auto editBox3 = tgui::EditBox::create();

        container->removeAllWidgets();
        container->add(editBox1);
        container->add(editBox2);
        container->add(editBox3);

        SECTION("Changing focus")
        {
            editBox2->setFocused(true);
            REQUIRE(!editBox1->isFocused());
            REQUIRE(editBox2->isFocused());
            REQUIRE(!editBox3->isFocused());

            container->focusNextWidget();
            REQUIRE(!editBox1->isFocused());
            REQUIRE(!editBox2->isFocused());
            REQUIRE(editBox3->isFocused());

            container->focusNextWidget();
            REQUIRE(editBox1->isFocused());
            REQUIRE(!editBox2->isFocused());
            REQUIRE(!editBox3->isFocused());

            container->focusPreviousWidget();
            REQUIRE(!editBox1->isFocused());
            REQUIRE(!editBox2->isFocused());
            REQUIRE(editBox3->isFocused());

            container->focusPreviousWidget();
            REQUIRE(!editBox1->isFocused());
            REQUIRE(editBox2->isFocused());
            REQUIRE(!editBox3->isFocused());

            container->unfocusAllWidgets();
            REQUIRE(!editBox1->isFocused());
            REQUIRE(!editBox2->isFocused());
            REQUIRE(!editBox3->isFocused());
        }

        SECTION("getFocusedChild / getFocusedLeaf")
        {
            auto childContainer = tgui::Group::create();
            container->add(childContainer);

            auto grandChildContainer = tgui::Group::create();
            childContainer->add(grandChildContainer);

            auto editBox4 = tgui::EditBox::create();
            grandChildContainer->add(editBox4);

            // No widgets focused by default
            REQUIRE(container->getFocusedChild() == nullptr);
            REQUIRE(container->getFocusedLeaf() == nullptr);

            // Direct non-container child focused
            editBox2->setFocused(true);
            REQUIRE(container->getFocusedChild() == editBox2);
            REQUIRE(container->getFocusedLeaf() == editBox2);

            // Direct container child focused
            childContainer->setFocused(true);
            REQUIRE(container->getFocusedChild() == childContainer);
            REQUIRE(container->getFocusedLeaf() == childContainer);

            // Focused leaf is a container
            grandChildContainer->setFocused(true);
            REQUIRE(container->getFocusedChild() == childContainer);
            REQUIRE(container->getFocusedLeaf() == grandChildContainer);

            // Focused leaf is not a container
            editBox4->setFocused(true);
            REQUIRE(container->getFocusedChild() == childContainer);
            REQUIRE(container->getFocusedLeaf() == editBox4);
        }
    }

    SECTION("setOpacity")
    {
        REQUIRE(container->getOpacity() == 1);

        container->setOpacity(0.7f);
        REQUIRE(container->getOpacity() == 0.7f);
        REQUIRE(widget1->getInheritedOpacity() == 0.7f);
        REQUIRE(widget2->getInheritedOpacity() == 0.7f);
        REQUIRE(widget3->getInheritedOpacity() == 0.7f);
        REQUIRE(widget4->getInheritedOpacity() == 0.7f);
        REQUIRE(widget5->getInheritedOpacity() == 0.7f);
    }

    SECTION("getWidgetAtPos / getWidgetBelowMouseCursor")
    {
        container->setAbsoluteView({-5, 40, 100, 400});

        container->removeAllWidgets();
        auto w1 = tgui::ClickableWidget::create();
        w1->setPosition({50, 100});
        w1->setSize({40, 80});
        container->add(w1);

        auto w2 = tgui::Panel::create();
        w2->setPosition({10, 40});
        w2->setSize({60, 100});
        container->add(w2);

        auto w3 = tgui::ClickableWidget::create();
        w3->setPosition({20, 40});
        w3->setSize({20, 40});
        w2->add(w3);

        REQUIRE(container->getWidgetAtPos({70, 99}, false) == nullptr);
        REQUIRE(container->getWidgetAtPos({70, 99}, true) == nullptr);
        REQUIRE(container->getWidgetAtPos({70, 100}, true) == w1);
        REQUIRE(container->getWidgetAtPos({69, 99}, true) == w2);
        REQUIRE(container->getWidgetAtPos({41, 120}, true) == w2);
        REQUIRE(container->getWidgetAtPos({41, 120}, false) == w2);
        REQUIRE(container->getWidgetAtPos({41, 119}, true) == w3);
        REQUIRE(container->getWidgetAtPos({41, 119}, false) == w2);

        REQUIRE(container->getWidgetBelowMouseCursor({150, 29}, false) == nullptr);
        REQUIRE(container->getWidgetBelowMouseCursor({150, 29}, true) == nullptr);
        REQUIRE(container->getWidgetBelowMouseCursor({150, 30}, true) == w1);
        REQUIRE(container->getWidgetBelowMouseCursor({149, 29}, true) == w2);
        REQUIRE(container->getWidgetBelowMouseCursor({93, 40}, true) == w2);
        REQUIRE(container->getWidgetBelowMouseCursor({93, 40}, false) == w2);
        REQUIRE(container->getWidgetBelowMouseCursor({93, 39}, true) == w3);
        REQUIRE(container->getWidgetBelowMouseCursor({93, 39}, false) == w2);
    }

    // TODO: Events
}
