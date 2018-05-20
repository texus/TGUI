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
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/Group.hpp>

TEST_CASE("[ListBox]")
{
    tgui::ListBox::Ptr listBox = tgui::ListBox::create();
    listBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](std::string){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, std::string, std::string){}));

        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](tgui::Widget::Ptr, std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](tgui::Widget::Ptr, std::string, sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](tgui::Widget::Ptr, std::string, std::string, std::string){}));

        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](tgui::Widget::Ptr, std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](tgui::Widget::Ptr, std::string, sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](tgui::Widget::Ptr, std::string, std::string, std::string){}));

        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](std::string){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, std::string){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, std::string, std::string){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(listBox->getWidgetType() == "ListBox");
    }

    SECTION("Position and Size")
    {
        listBox->setPosition(40, 30);
        listBox->setSize(150, 100);
        listBox->getRenderer()->setBorders(2);

        REQUIRE(listBox->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(listBox->getSize() == sf::Vector2f(150, 100));
        REQUIRE(listBox->getFullSize() == listBox->getSize());
        REQUIRE(listBox->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Adding items")
    {
        REQUIRE(listBox->getItemCount() == 0);
        listBox->addItem("Item 1", "1");
        REQUIRE(listBox->getItemCount() == 1);
        listBox->addItem("Item 2", "2");
        listBox->addItem("Item 3");
        REQUIRE(listBox->getItemCount() == 3);

        REQUIRE(listBox->getItems()[0] == "Item 1");
        REQUIRE(listBox->getItems()[1] == "Item 2");
        REQUIRE(listBox->getItems()[2] == "Item 3");

        REQUIRE(listBox->getItemById("1") == "Item 1");
        REQUIRE(listBox->getItemById("2") == "Item 2");
        REQUIRE(listBox->getItemById("3") == "");
    }

    SECTION("Removing items")
    {
        listBox->addItem("Item 1", "1");
        listBox->addItem("Item 2", "2");
        listBox->addItem("Item 3", "3");
        REQUIRE(listBox->getItemCount() == 3);

        REQUIRE(!listBox->removeItem("Item 0"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->removeItem("Item 2"));
        REQUIRE(listBox->getItemCount() == 2);

        listBox->addItem("Item 4", "4");
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 1");
        REQUIRE(listBox->getItems()[1] == "Item 3");
        REQUIRE(listBox->getItems()[2] == "Item 4");

        REQUIRE(!listBox->removeItemByIndex(3));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->removeItemByIndex(0));
        REQUIRE(listBox->getItemCount() == 2);
        REQUIRE(listBox->getItems()[0] == "Item 3");
        REQUIRE(listBox->getItems()[1] == "Item 4");

        REQUIRE(!listBox->removeItemById("2"));
        REQUIRE(listBox->getItemCount() == 2);
        REQUIRE(listBox->removeItemById("4"));
        REQUIRE(listBox->getItemCount() == 1);
        REQUIRE(listBox->getItems()[0] == "Item 3");

        listBox->addItem("Item 5");
        listBox->addItem("Item 5");
        listBox->addItem("Item 6");
        REQUIRE(listBox->getItemCount() == 4);
        listBox->removeAllItems();
        REQUIRE(listBox->getItemCount() == 0);
    }

    SECTION("Changing items")
    {
        listBox->addItem("Item 1", "1");
        listBox->addItem("Item 2", "2");
        listBox->addItem("Item 3", "3");

        REQUIRE(!listBox->changeItem("Item 0", "Item 00"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 1");
        REQUIRE(listBox->getItems()[1] == "Item 2");
        REQUIRE(listBox->getItems()[2] == "Item 3");
        REQUIRE(listBox->changeItem("Item 1", "Item 10"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 10");
        REQUIRE(listBox->getItems()[1] == "Item 2");
        REQUIRE(listBox->getItems()[2] == "Item 3");

        REQUIRE(!listBox->changeItemById("0", "Item 00"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 10");
        REQUIRE(listBox->getItems()[1] == "Item 2");
        REQUIRE(listBox->getItems()[2] == "Item 3");
        REQUIRE(listBox->changeItemById("3", "Item 30"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 10");
        REQUIRE(listBox->getItems()[1] == "Item 2");
        REQUIRE(listBox->getItems()[2] == "Item 30");

        REQUIRE(!listBox->changeItemByIndex(3, "Item 00"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 10");
        REQUIRE(listBox->getItems()[1] == "Item 2");
        REQUIRE(listBox->getItems()[2] == "Item 30");
        REQUIRE(listBox->changeItemByIndex(1, "Item 20"));
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 10");
        REQUIRE(listBox->getItems()[1] == "Item 20");
        REQUIRE(listBox->getItems()[2] == "Item 30");
    }

    SECTION("Selecting items")
    {
        listBox->addItem("Item 1", "1");
        listBox->addItem("Item 2", "2");
        listBox->addItem("Item 3", "3");

        REQUIRE(listBox->getSelectedItem() == "");
        REQUIRE(listBox->getSelectedItemId() == "");
        REQUIRE(listBox->getSelectedItemIndex() == -1);

        REQUIRE(!listBox->setSelectedItem("Item 0"));
        REQUIRE(listBox->getSelectedItemIndex() == -1);
        REQUIRE(listBox->setSelectedItem("Item 1"));
        REQUIRE(listBox->getSelectedItem() == "Item 1");
        REQUIRE(listBox->getSelectedItemId() == "1");
        REQUIRE(listBox->getSelectedItemIndex() == 0);

        REQUIRE(!listBox->setSelectedItemById("0"));
        REQUIRE(listBox->getSelectedItemIndex() == -1);
        REQUIRE(listBox->setSelectedItemById("2"));
        REQUIRE(listBox->getSelectedItem() == "Item 2");
        REQUIRE(listBox->getSelectedItemId() == "2");
        REQUIRE(listBox->getSelectedItemIndex() == 1);

        REQUIRE(!listBox->setSelectedItemByIndex(3));
        REQUIRE(listBox->getSelectedItemIndex() == -1);
        REQUIRE(listBox->setSelectedItemByIndex(2));
        REQUIRE(listBox->getSelectedItem() == "Item 3");
        REQUIRE(listBox->getSelectedItemId() == "3");
        REQUIRE(listBox->getSelectedItemIndex() == 2);

        listBox->deselectItem();
        REQUIRE(listBox->getSelectedItem() == "");
        REQUIRE(listBox->getSelectedItemId() == "");
        REQUIRE(listBox->getSelectedItemIndex() == -1);
    }

    SECTION("Contains")
    {
        REQUIRE(!listBox->contains("Item 1"));
        REQUIRE(!listBox->containsId("1"));

        listBox->addItem("Item 1", "1");
        REQUIRE(listBox->contains("Item 1"));
        REQUIRE(listBox->containsId("1"));

        listBox->addItem("Item 2");
        REQUIRE(listBox->contains("Item 2"));
        REQUIRE(!listBox->containsId("2"));

        listBox->removeItem("Item 1");
        REQUIRE(!listBox->contains("Item 1"));
        REQUIRE(!listBox->containsId("1"));
    }

    SECTION("ItemHeight")
    {
        listBox->setItemHeight(20);
        REQUIRE(listBox->getItemHeight() == 20);
    }

    SECTION("MaximumItems")
    {
        listBox->addItem("Item 1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3");
        listBox->addItem("Item 4");
        listBox->addItem("Item 5");
        REQUIRE(listBox->getItemCount() == 5);
        listBox->setMaximumItems(3);
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 1");
        REQUIRE(listBox->getItems()[2] == "Item 3");

        listBox->addItem("Item 6");
        REQUIRE(listBox->getItemCount() == 3);
        REQUIRE(listBox->getItems()[0] == "Item 1");
        REQUIRE(listBox->getItems()[2] == "Item 3");
    }

    SECTION("AutoScroll")
    {
        REQUIRE(listBox->getAutoScroll());
        listBox->setAutoScroll(false);
        REQUIRE(!listBox->getAutoScroll());
        listBox->setAutoScroll(true);
        REQUIRE(listBox->getAutoScroll());
    }

    SECTION("Events / Signals")
    {
        auto container = tgui::Group::create({400.f, 300.f});
        container->add(listBox);

        auto mouseMoved = [container](sf::Vector2i pos){
            sf::Event event;
            event.type = sf::Event::MouseMoved;
            event.mouseMove.x = pos.x;
            event.mouseMove.y = pos.y;
            container->handleEvent(event);
        };
        auto mousePressed = [container](sf::Vector2i pos){
            sf::Event event;
            event.type = sf::Event::MouseButtonPressed;
            event.mouseButton.button = sf::Mouse::Left;
            event.mouseButton.x = pos.x;
            event.mouseButton.y = pos.y;
            container->handleEvent(event);
        };
        auto mouseReleased = [container](sf::Vector2i pos){
            sf::Event event;
            event.type = sf::Event::MouseButtonReleased;
            event.mouseButton.button = sf::Mouse::Left;
            event.mouseButton.x = pos.x;
            event.mouseButton.y = pos.y;
            container->handleEvent(event);
        };

        listBox->setPosition(10, 20);
        listBox->setSize(120, 64);
        listBox->setItemHeight(20);
        listBox->getRenderer()->setBorders(2);
        listBox->getRenderer()->setScrollbarWidth(10);

        listBox->addItem("Item 1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3");

        SECTION("Widget")
        {
            testWidgetSignals(listBox);
        }

        unsigned int itemSelectedCount = 0;
        unsigned int doubleClickedCount = 0;
        listBox->connect("ItemSelected", &genericCallback, std::ref(itemSelectedCount));
        listBox->connect("DoubleClicked", &genericCallback, std::ref(doubleClickedCount));

        SECTION("Click on item")
        {
            // Clicking the border does nothing
            mouseMoved({11, 21});
            mousePressed({11, 21});
            mouseReleased({11, 21});
            REQUIRE(itemSelectedCount == 0);
            REQUIRE(listBox->getSelectedItemIndex() == -1);

            // Select the first item
            mouseMoved({12, 22});
            mousePressed({12, 22});
            mouseReleased({12, 22});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(listBox->getSelectedItemIndex() == 0);

            // Select the third item
            mousePressed({40, 70});
            mouseReleased({40, 70});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(listBox->getSelectedItemIndex() == 2);

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
                REQUIRE(listBox->getSelectedItemIndex() == 2);

                // Third click is not registered as another double click
                mousePressed({40, 70});
                mouseReleased({40, 70});
                REQUIRE(doubleClickedCount == 1);
            }
        }

        SECTION("Scrollbar interaction")
        {
            // There is no scrollbar yet
            mousePressed({127, 81});
            mouseReleased({127, 81});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(listBox->getSelectedItemIndex() == 2);

            listBox->addItem("Item 4");

            // Clicking on scrollbar arrow down
            mousePressed({127, 81});
            mouseReleased({127, 81});

            // Dragging scrollbar thumb
            mouseMoved({125, 50});
            mousePressed({125, 50});
            mouseMoved({125, 45});
            mouseReleased({125, 45});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(listBox->getSelectedItemIndex() == 2);

            // Select the second and then third item
            mouseMoved({40, 51});
            mousePressed({40, 51});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(listBox->getSelectedItemIndex() == 1);
            mouseMoved({40, 52});
            REQUIRE(itemSelectedCount == 3);
            REQUIRE(listBox->getSelectedItemIndex() == 2);

            listBox->setAutoScroll(false);
            listBox->addItem("Item 5");
            listBox->addItem("Item 6");
            listBox->addItem("Item 7");
            listBox->addItem("Item 8");
            listBox->addItem("Item 9");

            // Scrolling down with mouse wheel
            container->mouseWheelScrolled(-1, {40, 70});
            container->mouseWheelScrolled(-1, {40, 70});

            mousePressed({40, 52});
            REQUIRE(listBox->getSelectedItemIndex() == 4);
        }
    }

    testWidgetRenderer(listBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = listBox->getRenderer();

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
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Bold"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextStyle", "Italic"));
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
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", sf::Text::Bold));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextStyle", sf::Text::Italic));
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
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setTextStyle(sf::Text::Bold);
                renderer->setSelectedTextStyle(sf::Text::Italic);
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
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == sf::Text::Bold);
            REQUIRE(renderer->getProperty("SelectedTextStyle").getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["trackcolor"].getColor() == sf::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["thumbcolor"].getColor() == sf::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        listBox->addItem("Item 1", "1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3", "3");
        listBox->setItemHeight(25);
        listBox->setMaximumItems(5);
        listBox->setSelectedItem("Item 2");
        listBox->setTextSize(20);
        listBox->setMaximumItems(5);
        listBox->setAutoScroll(false);

        testSavingWidget("ListBox", listBox);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(80, 70, listBox)

        listBox->setEnabled(true);
        listBox->setPosition(10, 5);
        listBox->setSize(60, 60);
        listBox->setTextSize(14);
        listBox->setItemHeight(18);

        tgui::ListBoxRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setTextColor(sf::Color::Red);
        renderer.setSelectedBackgroundColor(sf::Color::White);
        renderer.setSelectedTextColor(sf::Color::Black);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setTextStyle(sf::Text::Italic);
        renderer.setSelectedTextStyle(sf::Text::Bold);
        renderer.setScrollbarWidth(14);
        renderer.setOpacity(0.7f);
        listBox->setRenderer(renderer.getData());

        auto setHoverRenderer = [&]{
                                    renderer.setBackgroundColorHover(sf::Color::Yellow);
                                    renderer.setTextColorHover(sf::Color::Magenta);
                                    renderer.setSelectedBackgroundColorHover(sf::Color::Cyan);
                                    renderer.setSelectedTextColorHover("#808080");
                                 };

        listBox->addItem("1");
        listBox->addItem("2");
        listBox->addItem("3");
        listBox->addItem("4");
        listBox->addItem("5");
        listBox->addItem("6");

        const sf::Vector2f mousePos1{30, 20};
        const sf::Vector2f mousePos2{30, 35};
        const sf::Vector2f mousePos3{30, 50};

        SECTION("Colored")
        {
            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("ListBox_NoSelectedNoHover.png")
                }

                SECTION("Hover")
                {
                    listBox->mouseMoved(mousePos2);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ListBox_NoSelectedHover_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("ListBox_NoSelectedHover_HoverSet.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                listBox->setSelectedItem("4");

                SECTION("No hover")
                {
                    TEST_DRAW("ListBox_SelectedNoHover.png")
                }

                SECTION("Hover selected")
                {
                    listBox->mouseMoved(mousePos1);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ListBox_SelectedHoverSelected_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("ListBox_SelectedHoverSelected_HoverSet.png")
                    }
                }

                SECTION("Hover other")
                {
                    listBox->mouseMoved(mousePos3);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ListBox_SelectedHoverOther_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("ListBox_SelectedHoverOther_HoverSet.png")
                    }
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");

            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("ListBox_NoSelectedNoHover_Texture.png")
                }

                SECTION("Hover")
                {
                    listBox->mouseMoved(mousePos2);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ListBox_NoSelectedHover_NoHoverSet_Texture.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("ListBox_NoSelectedHover_HoverSet_Texture.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                listBox->setSelectedItem("4");

                SECTION("No hover")
                {
                    TEST_DRAW("ListBox_SelectedNoHover_Texture.png")
                }

                SECTION("Hover selected")
                {
                    listBox->mouseMoved(mousePos1);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ListBox_SelectedHoverSelected_NoHoverSet_Texture.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("ListBox_SelectedHoverSelected_HoverSet_Texture.png")
                    }
                }

                SECTION("Hover other")
                {
                    listBox->mouseMoved(mousePos3);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ListBox_SelectedHoverOther_NoHoverSet_Texture.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("ListBox_SelectedHoverOther_HoverSet_Texture.png")
                    }
                }
            }
        }
    }
}
