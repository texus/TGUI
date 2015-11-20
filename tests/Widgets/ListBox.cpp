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

#include "../Tests.hpp"
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>

TEST_CASE("[ListBox]") {
    tgui::ListBox::Ptr listBox = std::make_shared<tgui::ListBox>();
    listBox->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](){}));
        
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](sf::String){}));
        
        REQUIRE_NOTHROW(listBox->connect("ItemSelected", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MousePressed", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("MouseReleased", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(listBox->connect("DoubleClicked", [](sf::String, sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(listBox->getWidgetType() == "ListBox");
    }

    SECTION("adding items") {
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
    
    SECTION("removing items") {
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
    
    SECTION("changing items") {
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

    SECTION("selecting items") {
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
    
    SECTION("ItemHeight") {
        listBox->setItemHeight(20);
        REQUIRE(listBox->getItemHeight() == 20);
    }

    SECTION("MaximumItems") {
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
    
    SECTION("Scrollbar") {
        tgui::Scrollbar::Ptr scrollbar = std::make_shared<tgui::Theme>()->load("scrollbar");
    
        REQUIRE(listBox->getScrollbar() != nullptr);
        REQUIRE(listBox->getScrollbar() != scrollbar);
        listBox->setScrollbar(nullptr);
        REQUIRE(listBox->getScrollbar() == nullptr);
        listBox->setScrollbar(scrollbar);
        REQUIRE(listBox->getScrollbar() != nullptr);
        REQUIRE(listBox->getScrollbar() == scrollbar);
    }

    SECTION("Renderer") {
        auto renderer = listBox->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("HoverBackgroundColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("HoverBackgroundColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions") {
                renderer->setTextColor({10, 20, 30});
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));

                renderer->setBackgroundColor({20, 30, 40});
                renderer->setTextColorNormal({30, 40, 50});
                renderer->setTextColorHover({40, 50, 60});
                renderer->setHoverBackgroundColor({50, 60, 70});
                renderer->setSelectedBackgroundColor({60, 70, 80});
                renderer->setSelectedTextColor({70, 80, 90});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 9);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(20, 30, 40));
                    REQUIRE(pairs["TextColorNormal"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["TextColorHover"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["HoverBackgroundColor"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["SelectedBackgroundColor"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["SelectedTextColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(80, 90, 100));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(5, 6, 7, 8));
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("HoverBackgroundColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getBorders() == tgui::Borders(5, 6, 7, 8));
        }

        SECTION("textured") {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

            REQUIRE(!renderer->getProperty("BackgroundImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", tgui::Serializer::serialize(textureBackground)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", textureBackground));
            }

            SECTION("functions") {
                renderer->setBackgroundTexture(textureBackground);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 9);
                    REQUIRE(pairs["BackgroundImage"].getTexture().getData() == textureBackground.getData());
                }
            }

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(listBox = std::make_shared<tgui::Theme>()->load("ListBox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(listBox = theme->load("ListBox"));
        REQUIRE(listBox->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(listBox->getSecondaryLoadingParameter() == "listbox");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(listBox);
        
        listBox->addItem("Item 1", "1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3", "3");
        listBox->setItemHeight(25);
        listBox->setMaximumItems(5);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileListBox1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileListBox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileListBox2.txt"));
        REQUIRE(compareFiles("WidgetFileListBox1.txt", "WidgetFileListBox2.txt"));

        SECTION("Copying widget") {
            tgui::ListBox temp;
            temp = *listBox;

            parent->removeAllWidgets();
            parent->add(tgui::ListBox::copy(std::make_shared<tgui::ListBox>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileListBox2.txt"));
            REQUIRE(compareFiles("WidgetFileListBox1.txt", "WidgetFileListBox2.txt"));
        }
    }
}
