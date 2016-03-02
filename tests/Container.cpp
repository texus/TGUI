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
#include <TGUI/TGUI.hpp>

TEST_CASE("[Container]") {
    auto container = std::make_shared<tgui::Gui>();

    auto widget1 = std::make_shared<tgui::Label>();
    auto widget2 = std::make_shared<tgui::Panel>();
    auto widget3 = std::make_shared<tgui::Label>();
    auto widget4 = std::make_shared<tgui::Label>();
    auto widget5 = std::make_shared<tgui::Label>();
    container->add(widget1, "w1");
    container->add(widget2, "w2");
    container->add(widget3, "w3");
    widget2->add(widget4, "w4");
    widget2->add(widget5, "w5");

    SECTION("default font in gui") {
        REQUIRE(std::make_shared<tgui::Gui>()->getFont() != nullptr);
        REQUIRE(std::make_shared<tgui::Panel>()->getFont() == nullptr);
    }

    SECTION("add") {
        container->removeAllWidgets();

        auto w1 = std::make_shared<tgui::ClickableWidget>();
        auto w2 = std::make_shared<tgui::ClickableWidget>();
        auto w3 = std::make_shared<tgui::ClickableWidget>();

        REQUIRE(container->getWidgets().empty());
        REQUIRE(container->getWidgetNames().empty());

        container->add(w1, "widget1");
        container->add(w2, "");
        container->add(w3, "widget3");

        REQUIRE(container->getWidgets().size() == 3);
        REQUIRE(container->getWidgets()[0] == w1);
        REQUIRE(container->getWidgets()[1] == w2);
        REQUIRE(container->getWidgets()[2] == w3);

        REQUIRE(container->getWidgetNames().size() == 3);
        REQUIRE(container->getWidgetNames()[0] == "widget1");
        REQUIRE(container->getWidgetNames()[1] == "");
        REQUIRE(container->getWidgetNames()[2] == "widget3");
    }

    SECTION("get") {
        SECTION("normal function") {
            SECTION("default non-recursive") {
                REQUIRE(container->get("w0") == nullptr);
                REQUIRE(container->get("w1") == widget1);
                REQUIRE(container->get("w2") == widget2);
                REQUIRE(container->get("w3") == widget3);
                REQUIRE(container->get("w4") == nullptr);
                REQUIRE(container->get("w5") == nullptr);
            }

            SECTION("recursive") {
                REQUIRE(container->get("w0", true) == nullptr);
                REQUIRE(container->get("w1", true) == widget1);
                REQUIRE(container->get("w2", true) == widget2);
                REQUIRE(container->get("w3", true) == widget3);
                REQUIRE(container->get("w4", true) == widget4);
                REQUIRE(container->get("w5", true) == widget5);
            }
        }

        SECTION("templated function") {
            SECTION("default non-recursive") {
                REQUIRE(container->get<tgui::Picture>("w0") == nullptr);
                REQUIRE(container->get<tgui::Label>("w1") == widget1);
                REQUIRE(container->get<tgui::Panel>("w2") == widget2);
                REQUIRE(container->get<tgui::Label>("w3") == widget3);
                REQUIRE(container->get<tgui::Label>("w4") == nullptr);
                REQUIRE(container->get<tgui::Label>("w5") == nullptr);
            }

            SECTION("recursive") {
                REQUIRE(container->get<tgui::Picture>("w0", true) == nullptr);
                REQUIRE(container->get<tgui::Label>("w1", true) == widget1);
                REQUIRE(container->get<tgui::Panel>("w2", true) == widget2);
                REQUIRE(container->get<tgui::Label>("w3", true) == widget3);
                REQUIRE(container->get<tgui::Label>("w4", true) == widget4);
                REQUIRE(container->get<tgui::Label>("w5", true) == widget5);
            }
        }
    }

    SECTION("remove") {
        SECTION("remove with correct parameter") {
            REQUIRE(container->getWidgets().size() == 3);
            REQUIRE(container->getWidgetNames().size() == 3);
            REQUIRE(widget2->getWidgets().size() == 2);
            REQUIRE(widget2->getWidgetNames().size() == 2);

            REQUIRE(container->remove(widget2));
            REQUIRE(widget2->remove(widget5));

            REQUIRE(container->getWidgets().size() == 2);
            REQUIRE(container->getWidgetNames().size() == 2);
            REQUIRE(container->getWidgets()[0] == widget1);
            REQUIRE(container->getWidgets()[1] == widget3);
            REQUIRE(container->getWidgetNames()[0] == "w1");
            REQUIRE(container->getWidgetNames()[1] == "w3");

            REQUIRE(widget2->getWidgets().size() == 1);
            REQUIRE(widget2->getWidgetNames().size() == 1);
            REQUIRE(widget2->getWidgets()[0] == widget4);
            REQUIRE(widget2->getWidgetNames()[0] == "w4");

            REQUIRE(widget2->remove(widget4));

            REQUIRE(widget2->getWidgets().empty());
            REQUIRE(widget2->getWidgetNames().empty());
        }

        SECTION("remove with wrong parameter") {
            REQUIRE(!container->remove(widget5));
            REQUIRE(!container->remove(nullptr));
            REQUIRE(!container->remove(std::make_shared<tgui::ClickableWidget>()));
        }

        SECTION("remove all widgets") {
            REQUIRE(container->getWidgets().size() == 3);
            REQUIRE(container->getWidgetNames().size() == 3);

            container->removeAllWidgets();

            REQUIRE(container->getWidgets().empty());
            REQUIRE(container->getWidgetNames().empty());
        }
    }

    SECTION("widget name") {
        REQUIRE(container->getWidgetNames().size() == 3);
        REQUIRE(container->getWidgetNames()[0] == container->getWidgetName(widget1));
        REQUIRE(container->getWidgetNames()[1] == container->getWidgetName(widget2));
        REQUIRE(container->getWidgetNames()[2] == container->getWidgetName(widget3));
        REQUIRE(container->getWidgetName(widget1) == "w1");
        REQUIRE(container->getWidgetName(widget2) == "w2");
        REQUIRE(container->getWidgetName(widget3) == "w3");

        REQUIRE(container->getWidgetName(widget4) == "");
        REQUIRE(container->getWidgetName(std::make_shared<tgui::ClickableWidget>()) == "");
        REQUIRE(container->getWidgetName(nullptr) == "");

        REQUIRE(container->setWidgetName(widget1, "w001"));
        REQUIRE(container->setWidgetName(widget3, "w003"));

        REQUIRE(!container->setWidgetName(widget4, "Hello"));
        REQUIRE(!container->setWidgetName(std::make_shared<tgui::ClickableWidget>(), "Hello"));
        REQUIRE(!container->setWidgetName(nullptr, "Hello"));

        REQUIRE(container->getWidgetNames()[0] == container->getWidgetName(widget1));
        REQUIRE(container->getWidgetNames()[1] == container->getWidgetName(widget2));
        REQUIRE(container->getWidgetNames()[2] == container->getWidgetName(widget3));
        REQUIRE(container->getWidgetName(widget1) == "w001");
        REQUIRE(container->getWidgetName(widget2) == "w2");
        REQUIRE(container->getWidgetName(widget3) == "w003");
    }
/**
    SECTION("focus") {
        auto editBox1 = std::make_shared<tgui::EditBox>();
        tgui::EditBox::Ptr editBox2 = std::make_shared<tgui::EditBox>();
        auto editBox3 = std::make_shared<tgui::EditBox>();

        container->removeAllWidgets();
        container->add(editBox1);
        container->add(editBox2);
        container->add(editBox3);

        container->focusWidget(editBox2);
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

        container->unfocusWidgets();
        REQUIRE(!editBox1->isFocused());
        REQUIRE(!editBox2->isFocused());
        REQUIRE(!editBox3->isFocused());
    }
*/
    SECTION("setOpacity") {
        REQUIRE(container->getOpacity() == 1);

        container->setOpacity(0.7f);
        REQUIRE(container->getOpacity() == 0.7f);
        REQUIRE(widget1->getRenderer()->getOpacity() == 0.7f);
        REQUIRE(widget2->getRenderer()->getOpacity() == 0.7f);
        REQUIRE(widget3->getRenderer()->getOpacity() == 0.7f);
        REQUIRE(widget4->getRenderer()->getOpacity() == 0.7f);
        REQUIRE(widget5->getRenderer()->getOpacity() == 0.7f);
    }
}
