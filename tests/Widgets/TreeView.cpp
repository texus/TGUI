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
#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[TreeView]")
{
    auto treeView = tgui::TreeView::create();
    treeView->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        for (const auto& signal : {"ItemSelected", "ItemSelected", "ItemSelected", "ItemSelected"})
        {
            REQUIRE_NOTHROW(treeView->connect(signal, [](){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](sf::String){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](std::string){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](std::vector<sf::String>){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](tgui::Widget::Ptr, std::string){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](tgui::Widget::Ptr, std::string, sf::String){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](tgui::Widget::Ptr, std::string, std::string){}));
            REQUIRE_NOTHROW(treeView->connect(signal, [](tgui::Widget::Ptr, std::string, std::vector<sf::String>){}));
        }
    }

    SECTION("WidgetType")
    {
        REQUIRE(treeView->getWidgetType() == "TreeView");
    }

    SECTION("Position and Size")
    {
        treeView->setPosition(40, 30);
        treeView->setSize(150, 100);
        treeView->getRenderer()->setBorders(2);

        REQUIRE(treeView->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(treeView->getSize() == sf::Vector2f(150, 100));
        REQUIRE(treeView->getFullSize() == treeView->getSize());
        REQUIRE(treeView->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Adding and removing items")
    {
        SECTION("No depth")
        {
            REQUIRE(treeView->getNodes().empty());
            REQUIRE(treeView->addItem({"One"}));
            REQUIRE(treeView->getNodes().size() == 1);
            REQUIRE(treeView->getNodes()[0].text == "One");
            REQUIRE(treeView->getNodes()[0].nodes.empty());

            REQUIRE(treeView->addItem({"Two"}, false));
            REQUIRE(treeView->getNodes().size() == 2);
        }

        SECTION("Multiple items")
        {
            REQUIRE(treeView->addItem({"Smilies", "Happy"}));
            REQUIRE(treeView->addItem({"Smilies", "Sad"}));
            REQUIRE(treeView->addItem({"Smilies", "Neither"}));
            REQUIRE(treeView->addItem({"Vehicles", "Parts", "Wheel"}));
            REQUIRE(treeView->addItem({"Vehicles", "Whole", "Truck"}));
            REQUIRE(treeView->addItem({"Vehicles", "Whole", "Car"}));

            SECTION("Verify that nodes were added")
            {
                REQUIRE(treeView->getNodes().size() == 2);
                REQUIRE(treeView->getNodes()[0].text == "Smilies");
                REQUIRE(treeView->getNodes()[0].nodes.size() == 3);
                REQUIRE(treeView->getNodes()[0].nodes[0].text == "Happy");
                REQUIRE(treeView->getNodes()[0].nodes[1].text == "Sad");
                REQUIRE(treeView->getNodes()[0].nodes[2].text == "Neither");
                REQUIRE(treeView->getNodes()[1].text == "Vehicles");
                REQUIRE(treeView->getNodes()[1].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[1].nodes[0].text == "Parts");
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes.size() == 1);
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes[0].text == "Wheel");
                REQUIRE(treeView->getNodes()[1].nodes[1].text == "Whole");
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Truck");
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Car");
            }

            SECTION("Removing items")
            {
                treeView->removeItem({"Vehicles", "Parts", "Wheel"});
                treeView->removeItem({"Smilies", "Sad"});
                treeView->removeItem({"Vehicles", "Whole", "Car"});

                REQUIRE(treeView->getNodes().size() == 2);
                REQUIRE(treeView->getNodes()[0].text == "Smilies");
                REQUIRE(treeView->getNodes()[0].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[0].nodes[0].text == "Happy");
                REQUIRE(treeView->getNodes()[0].nodes[1].text == "Neither");
                REQUIRE(treeView->getNodes()[1].text == "Vehicles");
                REQUIRE(treeView->getNodes()[1].nodes.size() == 1);
                REQUIRE(treeView->getNodes()[1].nodes[0].text == "Whole");
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes.size() == 1);
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes[0].text == "Truck");

                SECTION("Keep parent after delete")
                {
                    treeView->removeItem({"Vehicles", "Whole", "Truck"}, false);
                    REQUIRE(treeView->getNodes()[1].nodes[0].text == "Whole");
                    REQUIRE(treeView->getNodes()[1].nodes[0].nodes.size() == 0);
                }
            }

            SECTION("Removing all items")
            {
                treeView->removeAllItems();
                REQUIRE(treeView->getNodes().empty());
            }
        }

        SECTION("Invalid calls")
        {
            treeView->addItem({"One"});
            REQUIRE(!treeView->addItem({"Two", "Sub"}, false));
            REQUIRE(!treeView->addItem(std::vector<sf::String>()));
            REQUIRE(!treeView->removeItem({"One", "Sub"}));
        }
    }

    SECTION("Collapsing and expanding items")
    {
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});

        // Nodes are expanded by default
        REQUIRE(treeView->getNodes()[0].expanded);
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded);

        treeView->collapse({"Vehicles"});
        REQUIRE(!treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded); // collapse doesn't affect state of child nodes

        treeView->collapse({"Vehicles", "Parts"});
        REQUIRE(!treeView->getNodes()[1].nodes[0].expanded);

        treeView->expand({"Vehicles"});
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(!treeView->getNodes()[1].nodes[0].expanded); // expand doesn't affect state of child nodes

        treeView->collapseAll();
        REQUIRE(!treeView->getNodes()[0].expanded);
        REQUIRE(!treeView->getNodes()[1].expanded);
        REQUIRE(!treeView->getNodes()[1].nodes[0].expanded);

        treeView->expandAll();
        REQUIRE(treeView->getNodes()[0].expanded);
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded);

        // Expanding child will expand all parents
        treeView->collapseAll();
        treeView->expand({"Vehicles", "Parts", "Wheel"});
        REQUIRE(!treeView->getNodes()[0].expanded);
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].nodes[0].expanded);
    }

    SECTION("Selecting items")
    {
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});

        REQUIRE(treeView->getSelectedItem().empty());

        REQUIRE(treeView->selectItem({"Vehicles"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<sf::String>{"Vehicles"});

        REQUIRE(!treeView->selectItem({"Unexisting"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<sf::String>{"Vehicles"});

        REQUIRE(treeView->selectItem({"Smilies", "Neither"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<sf::String>{"Smilies", "Neither"});

        REQUIRE(!treeView->selectItem({"Vehicles", "Parts", "Unexisting"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<sf::String>{"Smilies", "Neither"});

        treeView->collapseAll();
        REQUIRE(treeView->selectItem({"Vehicles", "Parts", "Wheel"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<sf::String>{"Vehicles", "Parts", "Wheel"});
        REQUIRE(treeView->selectItem({"Vehicles", "Parts", "Wheel"}));

        treeView->deselectItem();
        REQUIRE(treeView->getSelectedItem().empty());
    }

    SECTION("ItemHeight")
    {
        treeView->setItemHeight(30);
        REQUIRE(treeView->getItemHeight() == 30);
    }

    SECTION("TextSize")
    {
        treeView->setTextSize(25);
        REQUIRE(treeView->getTextSize() == 25);
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(treeView);
        }

        /// TODO
    }

    testWidgetRenderer(treeView->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = treeView->getRenderer();

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
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["trackcolor"].getColor() == sf::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["thumbcolor"].getColor() == sf::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBranchExpanded("resources/TreeView/Expanded.png");
            tgui::Texture textureBranchCollapsed("resources/TreeView/Collapsed.png");
            tgui::Texture textureLeaf("resources/TreeView/Leaf.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchExpanded", tgui::Serializer::serialize(textureBranchExpanded)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchCollapsed", tgui::Serializer::serialize(textureBranchCollapsed)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureLeaf", tgui::Serializer::serialize(textureLeaf)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchExpanded", textureBranchExpanded));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchCollapsed", textureBranchCollapsed));
                REQUIRE_NOTHROW(renderer->setProperty("TextureLeaf", textureLeaf));
            }

            SECTION("functions")
            {
                renderer->setTextureBranchExpanded(textureBranchExpanded);
                renderer->setTextureBranchCollapsed(textureBranchCollapsed);
                renderer->setTextureLeaf(textureLeaf);
            }

            REQUIRE(renderer->getProperty("TextureBranchExpanded").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureBranchCollapsed").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureLeaf").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBranchExpanded().getData() == textureBranchExpanded.getData());
            REQUIRE(renderer->getTextureBranchCollapsed().getData() == textureBranchCollapsed.getData());
            REQUIRE(renderer->getTextureLeaf().getData() == textureLeaf.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});
        treeView->addItem({"Vehicles", "Whole", "Truck"});
        treeView->addItem({"Vehicles", "Whole", "Car"});
        treeView->collapse({"Vehicles", "Whole"});

        treeView->setItemHeight(30);
        treeView->setTextSize(25);

        testSavingWidget("TreeView", treeView, false);
    }

    // TODO: Draw
}
