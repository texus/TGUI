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

TEST_CASE("[tabContainer]")
{
    tgui::TabContainer::Ptr tabContainer = tgui::TabContainer::create();

    SECTION("Signals")
    {
        tabContainer->onSelectionChange([]() {});
        tabContainer->onSelectionChange([](int) {});
        tabContainer->onSelectionChanging([]() {});
        tabContainer->onSelectionChanging([](int, bool*) {});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(tabContainer)->getSignal("SelectionChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(tabContainer->getWidgetType() == "TabContainer");
    }

    SECTION("Position and Size")
    {
        tabContainer->setPosition(40, 30);
        tabContainer->setSize(25, 60);

        REQUIRE(tabContainer->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(tabContainer->getSize() == tgui::Vector2f(25, 60));
        REQUIRE(tabContainer->getFullSize() == tabContainer->getSize());
        REQUIRE(tabContainer->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Adding")
    {
        REQUIRE(tabContainer->getPanelCount() == 0);
        tabContainer->addTab("panel 1");
        REQUIRE(tabContainer->getPanelCount() == 1);
        tabContainer->addTab("panel 2", true);
        REQUIRE(tabContainer->getPanelCount() == 2);
    }

    SECTION("Inserting")
    {
        tabContainer->addTab("panel 1");

        auto p2 = tabContainer->insertTab(0, "panel 2", false);
        REQUIRE(tabContainer->getPanel(0) == p2);
        REQUIRE(tabContainer->getSelectedIndex() == 1);

        auto p3 = tabContainer->insertTab(1, "panel 3", false);
        REQUIRE(tabContainer->getPanel(1) == p3);
        REQUIRE(tabContainer->getSelectedIndex() == 2);

        // Too high index just adds panel at end
        auto p4 = tabContainer->insertTab(5, "panel 4");
        REQUIRE(tabContainer->getPanel(3) == p4);
    }

    SECTION("Removing")
    {
        auto p1 = tabContainer->addTab("panel 1");
        tabContainer->addTab("panel 2");
        tabContainer->addTab("panel 3", false);
        REQUIRE(!tabContainer->removeTab("panel 4"));
        REQUIRE(tabContainer->getPanelCount() == 3);
        REQUIRE(tabContainer->removeTab("panel 1"));
        REQUIRE(tabContainer->getPanelCount() == 2);
        REQUIRE(tabContainer->getSelectedIndex() == 0);
        REQUIRE(tabContainer->removeTab(1));
        REQUIRE(!tabContainer->removeTab(5));
        REQUIRE(tabContainer->getPanelCount() == 1);
    }

    SECTION("Indexing")
    {
        auto p1 = tabContainer->addTab("panel 1");

        REQUIRE(tabContainer->getPanel(1) == nullptr);
        REQUIRE(tabContainer->getPanel(0) == p1);
        REQUIRE(tabContainer->getIndex(p1) == 0);
        REQUIRE(tabContainer->getIndex(tgui::Panel::create()) == -1);
    }

    SECTION("Selecting")
    {
        auto p1 = tabContainer->addTab("Item 1", false);
        REQUIRE(tabContainer->getSelectedIndex() == -1);
        auto p2 = tabContainer->addTab("Item 2");
        REQUIRE(tabContainer->getSelectedIndex() == 1);
        auto p3 = tabContainer->addTab("Item 3", false);
        REQUIRE(tabContainer->getSelectedIndex() == 1);
        tabContainer->select(5);
        REQUIRE(tabContainer->getSelectedIndex() == 1);
    }

    SECTION("Changing items")
    {
        tabContainer->addTab("Item 1");
        tabContainer->addTab("Item 2");
        tabContainer->addTab("Item 3");

        REQUIRE(!tabContainer->changeTabText(3, "Item 00"));
        REQUIRE(tabContainer->getPanelCount() == 3);
        REQUIRE(tabContainer->getTabText(0) == "Item 1");
        REQUIRE(tabContainer->getTabText(1) == "Item 2");
        REQUIRE(tabContainer->getTabText(2) == "Item 3");
        REQUIRE(tabContainer->changeTabText(1, "Item 20"));
        REQUIRE(tabContainer->getPanelCount() == 3);
        REQUIRE(tabContainer->getTabText(0) == "Item 1");
        REQUIRE(tabContainer->getTabText(1) == "Item 20");
        REQUIRE(tabContainer->getTabText(2) == "Item 3");
    }

    SECTION("Events / Signals")
    {
        SECTION("SelectionChanged")
        {
            tabContainer->setSize({ 300, 100 });
            tabContainer->setTabsHeight(20);

            unsigned int tabContainerSelectedCount = 0;
            tabContainer->onSelectionChange(&genericCallback, std::ref(tabContainerSelectedCount));

            tabContainer->addTab("1");
            tabContainer->addTab("2");
            REQUIRE(tabContainerSelectedCount == 2);

            tabContainer->addTab("3", false);
            REQUIRE(tabContainerSelectedCount == 2);

            tabContainer->select(2);
            REQUIRE(tabContainerSelectedCount == 3);

            tabContainer->select(2);
            REQUIRE(tabContainerSelectedCount == 3);

            tabContainer->select(0);
            REQUIRE(tabContainerSelectedCount == 4);

            const tgui::Vector2f mousePos1{ 200, 10 };
            tabContainer->leftMousePressed(mousePos1);
            tabContainer->leftMouseReleased(mousePos1);
            REQUIRE(tabContainer->getTabText(static_cast<std::size_t>(tabContainer->getSelectedIndex())) == "3");
            REQUIRE(tabContainerSelectedCount == 5);

            const tgui::Vector2f mousePos2{ 199, 10 };
            tabContainer->leftMousePressed(mousePos2);
            tabContainer->leftMouseReleased(mousePos2);
            REQUIRE(tabContainer->getTabText(static_cast<std::size_t>(tabContainer->getSelectedIndex())) == "2");
            REQUIRE(tabContainerSelectedCount == 6);

            tabContainer->leftMousePressed(mousePos2);
            tabContainer->leftMouseReleased(mousePos2);
            REQUIRE(tabContainerSelectedCount == 6);
        }

        SECTION("SelectionChanging")
        {
            unsigned int tabContainerSelectedCount = 0;
            tabContainer->onSelectionChanging([&tabContainerSelectedCount](int idx, bool* Vetoed)
            {
                if (idx == 2)
                {
                    *Vetoed = true;
                }
                tabContainerSelectedCount++;
            });

            tabContainer->addTab("1");
            tabContainer->addTab("2");
            REQUIRE(tabContainerSelectedCount == 2);

            tabContainer->addTab("3", false);
            REQUIRE(tabContainerSelectedCount == 2);

            tabContainer->select(2);
            REQUIRE(tabContainer->getSelectedIndex() != 2);
            REQUIRE(tabContainerSelectedCount == 3);

            tabContainer->select(0);
            REQUIRE(tabContainerSelectedCount == 4);
        }
    }

    SECTION("Saving and loading from file")
    {
        testSavingWidget("TabContainer", tabContainer);
    }
}
