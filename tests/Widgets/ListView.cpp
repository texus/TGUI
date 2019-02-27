/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/ListView.hpp>
#include <TGUI/Widgets/Group.hpp>

TEST_CASE("[ListView]")
{
    tgui::ListView::Ptr listView = tgui::ListView::create();
    listView->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](){}));
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](int){}));
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, int){}));

        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](int){}));
        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, int){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(listView->getWidgetType() == "ListView");
    }

    SECTION("Position and Size")
    {
        listView->setPosition(40, 30);
        listView->setSize(150, 100);
        listView->getRenderer()->setBorders(2);

        REQUIRE(listView->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(listView->getSize() == sf::Vector2f(150, 100));
        REQUIRE(listView->getFullSize() == listView->getSize());
        REQUIRE(listView->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Adding columns")
    {
        REQUIRE(listView->getColumnCount() == 0);
        listView->addColumn("Col 1");
        REQUIRE(listView->getColumnCount() == 1);
        listView->addColumn("Col 2", 60, tgui::ListView::ColumnAlignment::Right);
        listView->addColumn("Col 3", 100, tgui::ListView::ColumnAlignment::Center);
        REQUIRE(listView->getColumnCount() == 3);

        REQUIRE(listView->getColumnText(0) == "Col 1");
        REQUIRE(listView->getColumnText(1) == "Col 2");
        REQUIRE(listView->getColumnText(2) == "Col 3");

        REQUIRE(listView->getColumnWidth(1) == 60);
        REQUIRE(listView->getColumnWidth(2) == 100);

        REQUIRE(listView->getColumnAlignment(0) == tgui::ListView::ColumnAlignment::Left);
        REQUIRE(listView->getColumnAlignment(1) == tgui::ListView::ColumnAlignment::Right);
        REQUIRE(listView->getColumnAlignment(2) == tgui::ListView::ColumnAlignment::Center);
    }

    SECTION("Changing columns")
    {
        listView->addColumn("Col 1");
        listView->addColumn("Col 2", 60, tgui::ListView::ColumnAlignment::Right);
        listView->addColumn("Col 3", 0, tgui::ListView::ColumnAlignment::Center);
        listView->addColumn("Col 4", 100, tgui::ListView::ColumnAlignment::Left);

        SECTION("Text")
        {
            float width1 = listView->getColumnWidth(0);
            float width2 = listView->getColumnWidth(1);
            float width3 = listView->getColumnWidth(2);
            float width4 = listView->getColumnWidth(3);

            listView->setColumnText(2, "New col 2");
            listView->setColumnText(3, "New col 3");

            REQUIRE(listView->getColumnText(2) == "New col 2");
            REQUIRE(listView->getColumnText(3) == "New col 3");
            REQUIRE(width1 == listView->getColumnWidth(0));
            REQUIRE(width2 == listView->getColumnWidth(1));
            REQUIRE(width3 != listView->getColumnWidth(2));
            REQUIRE(width4 == listView->getColumnWidth(3));
        }

        SECTION("Width")
        {
            listView->setColumnWidth(2, 0);
            listView->setColumnWidth(3, 75);

            REQUIRE(listView->getColumnWidth(2) > 0);
            REQUIRE(listView->getColumnWidth(2) != 60);
            REQUIRE(listView->getColumnWidth(3) == 75);
        }

        SECTION("Alignment")
        {
            listView->setColumnAlignment(0, tgui::ListView::ColumnAlignment::Right);
            REQUIRE(listView->getColumnAlignment(0) == tgui::ListView::ColumnAlignment::Right);
        }

        SECTION("Removing")
        {
            REQUIRE(listView->getColumnCount() == 4);
            listView->removeAllColumns();
            REQUIRE(listView->getColumnCount() == 0);
        }
    }

    SECTION("Items")
    {
        listView->addColumn("Col 1");
        listView->addColumn("Col 2");

        REQUIRE(listView->getItemCount() == 0);
        listView->addItem("1,1");
        listView->addItem({"2,1", "2,2", "2,3"});
        listView->addItem({"3,1", "3,2"});
        REQUIRE(listView->getItemCount() == 3);

        REQUIRE(listView->getItem(0) == "1,1");
        REQUIRE(listView->getItem(1) == "2,1");
        REQUIRE(listView->getItem(2) == "3,1");
        REQUIRE(listView->getItems() == std::vector<sf::String>{"1,1", "2,1", "3,1"});

        REQUIRE(listView->getItemRow(0) == std::vector<sf::String>{"1,1", ""});
        REQUIRE(listView->getItemRow(1) == std::vector<sf::String>{"2,1", "2,2"});
        REQUIRE(listView->getItemRow(2) == std::vector<sf::String>{"3,1", "3,2"});
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", ""}, {"2,1", "2,2"}, {"3,1", "3,2"}});

        listView->removeItem(1);
        REQUIRE(listView->getItemCount() == 2);
        REQUIRE(listView->getItems() == std::vector<sf::String>{"1,1", "3,1"});

        listView->removeAllItems();
        REQUIRE(listView->getItemCount() == 0);

        std::vector<std::vector<sf::String>> items = {{"1,1"}, {"2,1", "2,2", "2,3"}, {"3,1", "3,2"}};
        listView->addMultipleItems(items);
        REQUIRE(listView->getItemCount() == 3);
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", ""}, {"2,1", "2,2"}, {"3,1", "3,2"}});

        REQUIRE(listView->changeItem(1, {"b,1"}));
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", ""}, {"b,1", ""}, {"3,1", "3,2"}});
        REQUIRE(listView->changeItem(2, {"c,1", "c,2", "c,3"}));
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", ""}, {"b,1", ""}, {"c,1", "c,2"}});
        REQUIRE(listView->changeSubItem(0, 1, {"a,2"}));
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", "a,2"}, {"b,1", ""}, {"c,1", "c,2"}});
        REQUIRE(!listView->changeItem(3, {"d,1"}));
        REQUIRE(!listView->changeSubItem(3, 1, {"d,2"}));
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", "a,2"}, {"b,1", ""}, {"c,1", "c,2"}});
    }

    SECTION("Returned item rows depend on columns")
    {
        listView->addItem({"1,1", "1,2", "1,3"});
        listView->addItem({"2,1", "2,2"});

        REQUIRE(listView->getItemRow(0) == std::vector<sf::String>{"1,1"});
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1"}, {"2,1"}});

        listView->addColumn("Col 1");
        REQUIRE(listView->getItemRow(0) == std::vector<sf::String>{"1,1"});
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1"}, {"2,1"}});

        listView->addColumn("Col 2");
        REQUIRE(listView->getItemRow(0) == std::vector<sf::String>{"1,1", "1,2"});
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", "1,2"}, {"2,1", "2,2"}});

        listView->addColumn("Col 3");
        REQUIRE(listView->getItemRow(0) == std::vector<sf::String>{"1,1", "1,2", "1,3"});
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", "1,2", "1,3"}, {"2,1", "2,2", ""}});

        listView->addColumn("Col 4");
        REQUIRE(listView->getItemRow(0) == std::vector<sf::String>{"1,1", "1,2", "1,3", ""});
        REQUIRE(listView->getItemRows() == std::vector<std::vector<sf::String>>{{"1,1", "1,2", "1,3", ""}, {"2,1", "2,2", "", ""}});
    }

    SECTION("Selecting items")
    {
        listView->addItem("1,1");
        listView->addItem({"2,1", "2,2", "2,3"});
        listView->addItem({"3,1", "3,2"});

        REQUIRE(listView->getSelectedItemIndex() == -1);

        listView->setSelectedItem(2);
        REQUIRE(listView->getSelectedItemIndex() == 2);

        listView->deselectItem();
        REQUIRE(listView->getSelectedItemIndex() == -1);
    }

    SECTION("Header height")
    {
        listView->setHeaderHeight(50);
        REQUIRE(listView->getHeaderHeight() == 50);
    }

    SECTION("Header separator height")
    {
        listView->setHeaderSeparatorHeight(2);
        REQUIRE(listView->getHeaderSeparatorHeight() == 2);
    }

    SECTION("Header visible")
    {
        REQUIRE(listView->getHeaderVisible());
        listView->setHeaderVisible(false);
        REQUIRE(!listView->getHeaderVisible());
        listView->setHeaderVisible(true);
        REQUIRE(listView->getHeaderVisible());
    }

    SECTION("Current header height")
    {
        listView->addItem("1,1");
        listView->addItem({"2,1", "2,2", "2,3"});
        listView->addItem({"3,1", "3,2"});

        listView->setHeaderVisible(true);
        listView->setHeaderHeight(35);
        listView->setHeaderSeparatorHeight(2);
        REQUIRE(listView->getCurrentHeaderHeight() == 0);

        listView->addColumn("Col 1");
        listView->addColumn("Col 2");
        REQUIRE(listView->getCurrentHeaderHeight() == 37);

        listView->setHeaderVisible(false);
        REQUIRE(listView->getCurrentHeaderHeight() == 0);

        listView->setHeaderVisible(true);
        listView->setHeaderHeight(25);
        REQUIRE(listView->getCurrentHeaderHeight() == 27);

        listView->setHeaderSeparatorHeight(0);
        REQUIRE(listView->getCurrentHeaderHeight() == 25);

        listView->removeAllColumns();
        REQUIRE(listView->getCurrentHeaderHeight() == 0);
    }

    SECTION("ItemHeight")
    {
        listView->setItemHeight(18);
        REQUIRE(listView->getItemHeight() == 18);
    }

    SECTION("TextSize")
    {
        listView->setItemHeight(50);

        listView->setTextSize(20);
        REQUIRE(listView->getTextSize() == 20);

        listView->setTextSize(0);
        const unsigned int textSize = listView->getTextSize();
        REQUIRE(textSize > 0);
        REQUIRE(textSize != 20);
        REQUIRE(textSize < 50);

        listView->setItemHeight(60);
        REQUIRE(listView->getTextSize() > textSize);
        REQUIRE(listView->getTextSize() < 60);
    }

    SECTION("HeaderTextSize")
    {
        listView->setTextSize(22);
        REQUIRE(listView->getHeaderTextSize() == 22);

        listView->setHeaderTextSize(24);
        REQUIRE(listView->getHeaderTextSize() == 24);

        listView->setTextSize(26);
        REQUIRE(listView->getHeaderTextSize() == 24);

        listView->setHeaderTextSize(0);
        REQUIRE(listView->getHeaderTextSize() == 26);
    }

    SECTION("SeparatorWidth")
    {
        REQUIRE(listView->getSeparatorWidth() == 1);
        listView->setSeparatorWidth(4);
        REQUIRE(listView->getSeparatorWidth() == 4);
    }

    SECTION("GridLinesWidth")
    {
        REQUIRE(listView->getGridLinesWidth() == 1);
        listView->setGridLinesWidth(4);
        REQUIRE(listView->getGridLinesWidth() == 4);
    }

    SECTION("Show vertical grid lines")
    {
        REQUIRE(listView->getShowVerticalGridLines());
        listView->setShowVerticalGridLines(false);
        REQUIRE(!listView->getShowVerticalGridLines());
        listView->setShowVerticalGridLines(true);
        REQUIRE(listView->getShowVerticalGridLines());
    }

    SECTION("Show horizontal grid lines")
    {
        REQUIRE(!listView->getShowHorizontalGridLines());
        listView->setShowHorizontalGridLines(true);
        REQUIRE(listView->getShowHorizontalGridLines());
        listView->setShowHorizontalGridLines(false);
        REQUIRE(!listView->getShowHorizontalGridLines());
    }

    SECTION("Expand last column")
    {
        REQUIRE(!listView->getExpandLastColumn());
        listView->setExpandLastColumn(true);
        REQUIRE(listView->getExpandLastColumn());
        listView->setExpandLastColumn(false);
        REQUIRE(!listView->getExpandLastColumn());
    }

    SECTION("AutoScroll")
    {
        REQUIRE(listView->getAutoScroll());
        listView->setAutoScroll(false);
        REQUIRE(!listView->getAutoScroll());
        listView->setAutoScroll(true);
        REQUIRE(listView->getAutoScroll());
    }

    SECTION("VerticalScrollbarPolicy")
    {
        REQUIRE(listView->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(listView->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(listView->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(listView->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
    }

    SECTION("HorizontalScrollbarPolicy")
    {
        REQUIRE(listView->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(listView->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(listView->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(listView->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
    }

    SECTION("Events / Signals")
    {
        auto container = tgui::Group::create({400.f, 300.f});
        container->add(listView);

        auto mouseMoved = [container](sf::Vector2i pos){
            sf::Event event;
            event.type = sf::Event::MouseMoved;
            event.mouseMove.x = pos.x;
            event.mouseMove.y = pos.y;
            container->handleEvent(event);
        };
        auto mousePressed = [container](sf::Vector2i pos, sf::Mouse::Button mouseButton = sf::Mouse::Left){
            sf::Event event;
            event.type = sf::Event::MouseButtonPressed;
            event.mouseButton.button = mouseButton;
            event.mouseButton.x = pos.x;
            event.mouseButton.y = pos.y;
            container->handleEvent(event);
        };
        auto mouseReleased = [container](sf::Vector2i pos, sf::Mouse::Button mouseButton = sf::Mouse::Left){
            sf::Event event;
            event.type = sf::Event::MouseButtonReleased;
            event.mouseButton.button = mouseButton;
            event.mouseButton.x = pos.x;
            event.mouseButton.y = pos.y;
            container->handleEvent(event);
        };

        listView->setPosition(10, 20);
        listView->setSize(120, 64);
        listView->setItemHeight(20);
        listView->getRenderer()->setBorders(2);
        listView->getRenderer()->setScrollbarWidth(10);

        listView->addItem({"Item 1", "1,2"});
        listView->addItem({"Item 2", "2,2"});
        listView->addItem({"Item 3", "3,2"});

        SECTION("Widget")
        {
            testWidgetSignals(listView);
        }

        unsigned int itemSelectedCount = 0;
        unsigned int doubleClickedCount = 0;
        listView->connect("ItemSelected", &genericCallback, std::ref(itemSelectedCount));
        listView->connect("DoubleClicked", &genericCallback, std::ref(doubleClickedCount));

        SECTION("Click on item")
        {
            // Clicking the border does nothing
            mouseMoved({11, 21});
            mousePressed({11, 21});
            mouseReleased({11, 21});
            REQUIRE(itemSelectedCount == 0);
            REQUIRE(listView->getSelectedItemIndex() == -1);

            // Select the first item
            mouseMoved({12, 22});
            mousePressed({12, 22});
            mouseReleased({12, 22});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(listView->getSelectedItemIndex() == 0);

            // Select the third item
            mousePressed({40, 70});
            mouseReleased({40, 70});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(listView->getSelectedItemIndex() == 2);

            SECTION("Double click")
            {
                // If a click happens a long time after the previous click then it isn't a double click
                container->update(sf::milliseconds(10000));
                mousePressed({40, 70});
                mouseReleased({40, 70});
                REQUIRE(doubleClickedCount == 0);

                container->update(sf::milliseconds(5));
                mousePressed({40, 70});
                mouseReleased({40, 70});
                REQUIRE(doubleClickedCount == 1);
                REQUIRE(itemSelectedCount == 2);
                REQUIRE(listView->getSelectedItemIndex() == 2);

                // Third click is not registered as another double click
                mousePressed({40, 70});
                mouseReleased({40, 70});
                REQUIRE(doubleClickedCount == 1);
            }
        }

        SECTION("Click on header")
        {
            listView->setHeaderHeight(30);
            listView->addColumn("Col 1", 50);
            listView->addColumn("Col 2", 50);

            mousePressed({40, 35});
            mouseReleased({40, 35});
            REQUIRE(listView->getSelectedItemIndex() == -1);

            listView->setHeaderVisible(false);
            mousePressed({40, 35});
            mouseReleased({40, 35});
            REQUIRE(listView->getSelectedItemIndex() == 0);
        }

        SECTION("Right click")
        {
            int lastIndex = -1;
            unsigned int rightClickCount = 0;
            listView->connect("RightClicked", [&](int index){ lastIndex = index; ++rightClickCount; });

            // Right click 3th item
            mousePressed({40, 70}, sf::Mouse::Right);
            mouseReleased({40, 70}, sf::Mouse::Right);
            REQUIRE(rightClickCount == 1);
            REQUIRE(lastIndex == 2);

            // Right click behind last item
            listView->removeItem(1);
            mousePressed({40, 70}, sf::Mouse::Right);
            mouseReleased({40, 70}, sf::Mouse::Right);
            REQUIRE(rightClickCount == 2);
            REQUIRE(lastIndex == -1);

            // Right click on scrollbar
            listView->addItem("Item 5");
            listView->addItem("Item 6");
            listView->addItem("Item 7");
            mousePressed({125, 50}, sf::Mouse::Right);
            mouseReleased({125, 50}, sf::Mouse::Right);
            REQUIRE(rightClickCount == 2);
        }

        SECTION("Vertical scrollbar interaction")
        {
            // There is no scrollbar yet
            mousePressed({127, 81});
            mouseReleased({127, 81});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(listView->getSelectedItemIndex() == 2);

            listView->addItem("Item 4");

            // Clicking on scrollbar arrow down
            mousePressed({127, 81});
            mouseReleased({127, 81});

            // Dragging scrollbar thumb
            mouseMoved({125, 50});
            mousePressed({125, 50});
            mouseMoved({125, 45});
            mouseReleased({125, 45});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(listView->getSelectedItemIndex() == 2);

            // Select the second and then third item
            mouseMoved({40, 51});
            mousePressed({40, 51});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(listView->getSelectedItemIndex() == 1);
            mouseMoved({40, 52});
            REQUIRE(itemSelectedCount == 3);
            REQUIRE(listView->getSelectedItemIndex() == 2);
            mouseReleased({40, 52});

            listView->setAutoScroll(false);
            listView->addItem("Item 5");
            listView->addItem("Item 6");
            listView->addItem("Item 7");
            listView->addItem("Item 8");
            listView->addItem("Item 9");

            // Scrolling down with mouse wheel
            container->mouseWheelScrolled(-1, {40, 70});
            container->mouseWheelScrolled(-1, {40, 70});

            mousePressed({40, 52});
            REQUIRE(listView->getSelectedItemIndex() == 4);
            mouseReleased({40, 52});
        }

        SECTION("Horizontal scrollbar interaction")
        {
            listView->setHeaderHeight(30);
            listView->addColumn("Col 1", 50);
            listView->addColumn("Col 2", 50);
            listView->setSize(listView->getSize().x, listView->getSize().y + listView->getHeaderHeight());

            mousePressed({40, 110});
            mouseReleased({40, 110});
            REQUIRE(listView->getSelectedItemIndex() == 2);

            listView->deselectItem();
            listView->addColumn("Col 3", 50);

            mousePressed({40, 110});
            mouseReleased({40, 110});
            REQUIRE(listView->getSelectedItemIndex() == -1);
        }
    }

    testWidgetRenderer(listView->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = listView->getRenderer();

        SECTION("colored")
        {
            tgui::ScrollbarRenderer scrollbarRenderer;
            scrollbarRenderer.setTrackColor(sf::Color::Red);
            scrollbarRenderer.setThumbColor(sf::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColorHover", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("HeaderBackgroundColor", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("HeaderTextColor", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorColor", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("GridLinesColor", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", "{ TrackColor = Red; ThumbColor = Blue; }"));
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", "15"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColorHover", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("HeaderBackgroundColor", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("HeaderTextColor", sf::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorColor", sf::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("GridLinesColor", sf::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", scrollbarRenderer.getData()));
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", 15));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({20, 30, 40});
                renderer->setBackgroundColorHover({50, 60, 70});
                renderer->setTextColor({30, 40, 50});
                renderer->setTextColorHover({40, 50, 60});
                renderer->setSelectedBackgroundColor({60, 70, 80});
                renderer->setSelectedBackgroundColorHover({90, 100, 110});
                renderer->setSelectedTextColor({70, 80, 90});
                renderer->setSelectedTextColorHover({100, 110, 120});
                renderer->setHeaderBackgroundColor({110, 120, 130});
                renderer->setHeaderTextColor({120, 130, 140});
                renderer->setSeparatorColor({130, 140, 150});
                renderer->setGridLinesColor({140, 150, 160});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setScrollbar(scrollbarRenderer.getData());
                renderer->setScrollbarWidth(15);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("SelectedBackgroundColorHover").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("HeaderBackgroundColor").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("HeaderTextColor").getColor() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("SeparatorColor").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("GridLinesColor").getColor() == sf::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["trackcolor"].getColor() == sf::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["thumbcolor"].getColor() == sf::Color::Blue);
        }
    }

    SECTION("Saving and loading from file")
    {
        listView->addColumn("Col 1");
        listView->addColumn("Col 2", 100);
        listView->addColumn("Col 3", 75, tgui::ListView::ColumnAlignment::Right);
        listView->addColumn("Col 4", 0, tgui::ListView::ColumnAlignment::Center);
        listView->addItem("1,1");
        listView->addItem({"2,1", "2,2", "2,3"});
        listView->addItem({"3,1", "3,2"});
        listView->setItemHeight(25);
        listView->setSelectedItem(1);
        listView->setTextSize(20);
        listView->setHeaderHeight(28);
        listView->setHeaderTextSize(22);
        listView->setHeaderSeparatorHeight(3);
        listView->setSeparatorWidth(5);
        listView->setGridLinesWidth(2);
        listView->setHeaderVisible(false);
        listView->setAutoScroll(false);
        listView->setShowVerticalGridLines(false);
        listView->setShowHorizontalGridLines(true);
        listView->setExpandLastColumn(true);
        listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);

        testSavingWidget("listView", listView);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(180, 140, listView)

        listView->setEnabled(true);
        listView->setPosition(10, 5);
        listView->setSize(160, 130);
        listView->setTextSize(14);
        listView->setItemHeight(18);
        listView->setHeaderHeight(25);
        listView->setHeaderTextSize(20);
        listView->setSeparatorWidth(4);
        listView->setGridLinesWidth(2);

        tgui::ListViewRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setTextColor(sf::Color::Red);
        renderer.setSelectedBackgroundColor(sf::Color::White);
        renderer.setSelectedTextColor(sf::Color::Black);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setHeaderBackgroundColor("#ff6403");
        renderer.setHeaderTextColor("#4700a5");
        renderer.setSeparatorColor("#562c05");
        renderer.setGridLinesColor(sf::Color::Black);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setScrollbarWidth(14);
        renderer.setOpacity(0.7f);
        listView->setRenderer(renderer.getData());

        tgui::ScrollbarRenderer scrollbarRenderer = tgui::RendererData::create();
        scrollbarRenderer.setTrackColor(sf::Color::Green);
        scrollbarRenderer.setThumbColor(sf::Color::Red);
        scrollbarRenderer.setArrowBackgroundColor(sf::Color::Blue);
        scrollbarRenderer.setArrowColor(sf::Color::White);
        scrollbarRenderer.setOpacity(0.7f);
        renderer.setScrollbar(scrollbarRenderer.getData());

        auto setHoverRenderer = [&]{
                                    renderer.setBackgroundColorHover(sf::Color::Yellow);
                                    renderer.setTextColorHover(sf::Color::Magenta);
                                    renderer.setSelectedBackgroundColorHover(sf::Color::Cyan);
                                    renderer.setSelectedTextColorHover("#808080");
                                 };

        listView->addItem({"1", "1.2"});
        listView->addItem("2");
        listView->addItem({"3", "3.2"});
        listView->addItem("4");
        listView->addItem({"5", "5.2", "5.3"});
        listView->addItem("6");

        const sf::Vector2f mousePos1{30, 85};
        const sf::Vector2f mousePos2{30, 50};
        const sf::Vector2f mousePos3{30, 45};

        SECTION("No columns")
        {
            TEST_DRAW("ListView_NoColumns.png")
        }

        listView->addColumn("C1", 40);
        listView->addColumn("C2", 70);
        listView->addColumn("C3", 70);

        SECTION("No selected item")
        {
            SECTION("No hover")
            {
                TEST_DRAW("ListView_NoSelectedNoHover.png")
            }

            SECTION("Hover")
            {
                listView->mouseMoved(mousePos2);

                SECTION("No hover properties set")
                {
                    TEST_DRAW("ListView_NoSelectedHover_NoHoverSet.png")
                }
                SECTION("Hover properties set")
                {
                    setHoverRenderer();
                    TEST_DRAW("ListView_NoSelectedHover_HoverSet.png")
                }
            }
        }

        SECTION("Selected item")
        {
            listView->setSelectedItem(4);

            SECTION("No hover")
            {
                TEST_DRAW("ListView_SelectedNoHover.png")
            }

            SECTION("Hover selected")
            {
                listView->mouseMoved(mousePos1);

                SECTION("No hover properties set")
                {
                    TEST_DRAW("ListView_SelectedHoverSelected_NoHoverSet.png")
                }
                SECTION("Hover properties set")
                {
                    setHoverRenderer();
                    TEST_DRAW("ListView_SelectedHoverSelected_HoverSet.png")
                }
            }

            SECTION("Hover other")
            {
                listView->mouseMoved(mousePos3);

                SECTION("No hover properties set")
                {
                    TEST_DRAW("ListView_SelectedHoverOther_NoHoverSet.png")
                }
                SECTION("Hover properties set")
                {
                    setHoverRenderer();
                    TEST_DRAW("ListView_SelectedHoverOther_HoverSet.png")
                }
            }
        }

        SECTION("Scrollbars")
        {
            SECTION("By content")
            {
                listView->removeAllColumns();
                listView->removeAllItems();
                listView->addColumn("C1", 40);
                listView->addItem({"1", "1.2"});

                SECTION("No scrollbars")
                {
                    TEST_DRAW("ListView_NoScrollbars.png")
                }

                SECTION("Vertical scrollbar")
                {
                    for (unsigned int i = 1; i < 10; ++i)
                        listView->addItem(std::to_string(i));

                    TEST_DRAW("ListView_VerticalScrollbar.png")
                }

                SECTION("Horizontal scrollbar")
                {
                    listView->addColumn("C2", 200);
                    TEST_DRAW("ListView_HorizontalScrollbar.png")
                }

                SECTION("Both scrollbars")
                {
                    listView->addColumn("C2", 200);
                    for (unsigned int i = 1; i < 10; ++i)
                        listView->addItem(std::to_string(i));

                    TEST_DRAW("ListView_BothScrollbars.png")
                }
            }

            SECTION("By policies")
            {
                SECTION("No scrollbars")
                {
                    listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                    listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                    TEST_DRAW("ListView_NoScrollbars_Policy.png")
                }

                SECTION("Vertical scrollbar")
                {
                    listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                    listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                    TEST_DRAW("ListView_VerticalScrollbar_Policy.png")
                }

                SECTION("Horizontal scrollbar")
                {
                    listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                    listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                    TEST_DRAW("ListView_HorizontalScrollbar_Policy.png")
                }

                SECTION("Both scrollbars")
                {
                    listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                    listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                    TEST_DRAW("ListView_BothScrollbars_Policy.png")
                }

                SECTION("Forced scrollbars")
                {
                    listView->removeAllColumns();
                    listView->removeAllItems();
                    listView->addColumn("C1", 40);
                    listView->addItem({"1", "1.2"});

                    listView->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
                    listView->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
                    TEST_DRAW("ListView_ForcedScrollbars_Policy.png")
                }
            }
        }

        SECTION("Icons")
        {
            listView->setItemIcon(3, {"resources/Texture6.png", {0, 0, 20, 14}});
            listView->setItemIcon(4, {"resources/Texture7.png", {0, 0, 14, 14}});
            TEST_DRAW("ListView_Icons.png")
        }

        SECTION("Grid lines and separators")
        {
            listView->removeAllColumns();
            listView->addColumn("C1", 40);
            listView->addColumn("C2", 60);

            listView->setHeaderSeparatorHeight(2);
            listView->setSeparatorWidth(5);
            listView->setGridLinesWidth(3);
            listView->setShowVerticalGridLines(true);
            listView->setShowHorizontalGridLines(true);

            SECTION("All separators")
            {
                TEST_DRAW("ListView_GridLinesAndSeparators_All.png")
            }

            SECTION("No column separators")
            {
                listView->setSeparatorWidth(0);
                TEST_DRAW("ListView_GridLinesAndSeparators_NoColumnSeparator.png")
            }

            SECTION("No header separator")
            {
                listView->setHeaderSeparatorHeight(0);
                TEST_DRAW("ListView_GridLinesAndSeparators_NoHeaderSeparator.png")
            }

            SECTION("No vertical grid lines")
            {
                listView->setShowVerticalGridLines(false);
                TEST_DRAW("ListView_GridLinesAndSeparators_NoVerticalGridLines.png")
            }

            SECTION("No horizontal grid lines")
            {
                listView->setShowHorizontalGridLines(false);
                TEST_DRAW("ListView_GridLinesAndSeparators_NoHorizontalGridLines.png")
            }

            SECTION("No grid lines")
            {
                listView->setShowVerticalGridLines(false);
                listView->setShowHorizontalGridLines(false);
                TEST_DRAW("ListView_GridLinesAndSeparators_NoGridLines.png")
            }
        }

        SECTION("Expand last column")
        {
            listView->removeAllColumns();

            SECTION("Columns fit inside ListView")
            {
                listView->addColumn("C1", 40);
                listView->addColumn("C2", 50);

                listView->setExpandLastColumn(true);
                TEST_DRAW("ListView_ExpandLastColumnTrue_NoScrollbar.png")

                listView->setExpandLastColumn(false);
                TEST_DRAW("ListView_ExpandLastColumnFalse_NoScrollbar.png")
            }

            SECTION("Horizontal scrollbar needed")
            {
                listView->addColumn("C1", 70);
                listView->addColumn("C2", 90);

                listView->setExpandLastColumn(true);
                TEST_DRAW("ListView_ExpandLastColumnTrue_WithScrollbar.png")

                listView->setExpandLastColumn(false);
                TEST_DRAW("ListView_ExpandLastColumnFalse_WithScrollbar.png")
            }
        }
    }
}
