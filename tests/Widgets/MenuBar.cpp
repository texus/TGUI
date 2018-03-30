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
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[MenuBar]")
{
    tgui::MenuBar::Ptr menuBar = tgui::MenuBar::create();
    menuBar->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](sf::String){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](std::string){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](std::vector<sf::String>){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](tgui::Widget::Ptr, std::string, std::string){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](tgui::Widget::Ptr, std::string, std::vector<sf::String>){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(menuBar->getWidgetType() == "MenuBar");
    }

    SECTION("Position and Size")
    {
        SECTION("Manual size")
        {
            menuBar->setPosition(40, 30);
            menuBar->setSize(150, 100);

            REQUIRE(menuBar->getPosition() == sf::Vector2f(40, 30));
            REQUIRE(menuBar->getSize() == sf::Vector2f(150, 100));
            REQUIRE(menuBar->getFullSize() == menuBar->getSize());
            REQUIRE(menuBar->getWidgetOffset() == sf::Vector2f(0, 0));

            SECTION("Width is unchanged when adding to parent")
            {
                auto parent = tgui::Panel::create();
                parent->add(menuBar);

                REQUIRE(menuBar->getSize() == sf::Vector2f(150, 100));
            }
        }

        SECTION("Width is set when added to parent")
        {
            REQUIRE(menuBar->getSize().x == 0);

            auto parent = tgui::Panel::create();
            parent->setSize(400, 300);
            parent->add(menuBar);

            REQUIRE(menuBar->getSize().x == 400);

            SECTION("Width of parent is bound")
            {
                parent->setSize(50, 100);
                REQUIRE(menuBar->getSize().x == 50);
            }
        }
    }

    SECTION("Adding and removing menus")
    {
        SECTION("Single menu")
        {
            REQUIRE(menuBar->getMenus().empty());
            menuBar->addMenu("Help");
            REQUIRE(menuBar->getMenus().size() == 1);
            REQUIRE(menuBar->getMenus()[0].first == "Help");
            REQUIRE(menuBar->getMenus()[0].second.empty());

            menuBar->addMenuItem("About");
            REQUIRE(menuBar->getMenus().size() == 1);
            REQUIRE(menuBar->getMenus()[0].first == "Help");
            REQUIRE(menuBar->getMenus()[0].second.size() == 1);
            REQUIRE(menuBar->getMenus()[0].second[0] == "About");
        }

        SECTION("Multiple menus")
        {
            menuBar->addMenu("File");
            menuBar->addMenuItem("Load");
            menuBar->addMenuItem("Save");
            menuBar->addMenu("Edit");
            menuBar->addMenuItem("Undo");
            menuBar->addMenuItem("Redo");
            menuBar->addMenuItem("Copy");
            menuBar->addMenuItem("Paste");
            menuBar->addMenu("Help");
            menuBar->addMenuItem("About");

            SECTION("Verify that menus were added")
            {
                REQUIRE(menuBar->getMenus().size() == 3);
                REQUIRE(menuBar->getMenus()[0].first == "File");
                REQUIRE(menuBar->getMenus()[0].second.size() == 2);
                REQUIRE(menuBar->getMenus()[0].second[0] == "Load");
                REQUIRE(menuBar->getMenus()[0].second[1] == "Save");
                REQUIRE(menuBar->getMenus()[1].first == "Edit");
                REQUIRE(menuBar->getMenus()[1].second.size() == 4);
                REQUIRE(menuBar->getMenus()[1].second[0] == "Undo");
                REQUIRE(menuBar->getMenus()[1].second[1] == "Redo");
                REQUIRE(menuBar->getMenus()[1].second[2] == "Copy");
                REQUIRE(menuBar->getMenus()[1].second[3] == "Paste");
                REQUIRE(menuBar->getMenus()[2].first == "Help");
                REQUIRE(menuBar->getMenus()[2].second.size() == 1);
                REQUIRE(menuBar->getMenus()[2].second[0] == "About");
            }

            SECTION("Adding menu items to older menu")
            {
                REQUIRE(menuBar->addMenuItem("File", "Quit"));

                REQUIRE(menuBar->getMenus().size() == 3);
                REQUIRE(menuBar->getMenus()[0].first == "File");
                REQUIRE(menuBar->getMenus()[0].second.size() == 3);
                REQUIRE(menuBar->getMenus()[0].second[0] == "Load");
                REQUIRE(menuBar->getMenus()[0].second[1] == "Save");
                REQUIRE(menuBar->getMenus()[0].second[2] == "Quit");
                REQUIRE(menuBar->getMenus()[1].first == "Edit");
                REQUIRE(menuBar->getMenus()[1].second.size() == 4);
                REQUIRE(menuBar->getMenus()[2].first == "Help");
                REQUIRE(menuBar->getMenus()[2].second.size() == 1);
            }

            SECTION("Removing menu items")
            {
                menuBar->removeMenuItem("Edit", "Undo");
                menuBar->removeMenuItem("Edit", "Paste");
                menuBar->removeMenuItem("Help", "About");

                REQUIRE(menuBar->getMenus()[0].first == "File");
                REQUIRE(menuBar->getMenus()[0].second.size() == 2);
                REQUIRE(menuBar->getMenus()[1].first == "Edit");
                REQUIRE(menuBar->getMenus()[1].second.size() == 2);
                REQUIRE(menuBar->getMenus()[1].second[0] == "Redo");
                REQUIRE(menuBar->getMenus()[1].second[1] == "Copy");
                REQUIRE(menuBar->getMenus()[2].first == "Help");
                REQUIRE(menuBar->getMenus()[2].second.size() == 0);
            }

            SECTION("Removing menu")
            {
                menuBar->removeMenu("File");

                REQUIRE(menuBar->getMenus().size() == 2);
                REQUIRE(menuBar->getMenus()[0].first == "Edit");
                REQUIRE(menuBar->getMenus()[0].second.size() == 4);
                REQUIRE(menuBar->getMenus()[1].first == "Help");
                REQUIRE(menuBar->getMenus()[1].second.size() == 1);
            }

            SECTION("Removing all menus")
            {
                menuBar->removeAllMenus();
                REQUIRE(menuBar->getMenus().empty());
            }
        }

        SECTION("Invalid addMenuItem calls")
        {
            REQUIRE(!menuBar->addMenuItem("Item"));

            menuBar->addMenu("File");
            REQUIRE(!menuBar->addMenuItem("Edit", "Item"));
        }
    }

    SECTION("TextSize")
    {
        menuBar->setTextSize(25);
        REQUIRE(menuBar->getTextSize() == 25);
    }

    SECTION("MinimumSubMenuWidth")
    {
        menuBar->setMinimumSubMenuWidth(150);
        REQUIRE(menuBar->getMinimumSubMenuWidth() == 150);
    }

    SECTION("InvertedMenuDirection")
    {
        REQUIRE(!menuBar->getInvertedMenuDirection());
        menuBar->setInvertedMenuDirection(true);
        REQUIRE(menuBar->getInvertedMenuDirection());
        menuBar->setInvertedMenuDirection(false);
        REQUIRE(!menuBar->getInvertedMenuDirection());
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(menuBar);
        }

        /// TODO
    }

    testWidgetRenderer(menuBar->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = menuBar->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setSelectedBackgroundColor({40, 50, 60});
                renderer->setTextColor({70, 80, 90});
                renderer->setSelectedTextColor({100, 110, 120});
                renderer->setDistanceToSide(2);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {115, 179, 8, 6}, {2, 2, 4, 2});
            tgui::Texture textureItemBackground("resources/Black.png", {115, 181, 8, 4}, {2, 0, 4, 2});
            tgui::Texture textureSelectedItemBackground("resources/Black.png", {115, 185, 8, 6}, {2, 2, 4, 2});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureItemBackground", tgui::Serializer::serialize(textureItemBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedItemBackground", tgui::Serializer::serialize(textureSelectedItemBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureItemBackground", textureItemBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedItemBackground", textureSelectedItemBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
                renderer->setTextureItemBackground(textureItemBackground);
                renderer->setTextureSelectedItemBackground(textureSelectedItemBackground);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureItemBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureSelectedItemBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
            REQUIRE(renderer->getTextureItemBackground().getData() == textureItemBackground.getData());
            REQUIRE(renderer->getTextureSelectedItemBackground().getData() == textureSelectedItemBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        menuBar->addMenu("File");
        menuBar->addMenuItem("Quit");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenu("Help");

        menuBar->setMinimumSubMenuWidth(100);
        menuBar->setTextSize(25); // TextSize is currently reset when copying (due to setSize calling setTextSize)

        testSavingWidget("MenuBar", menuBar);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(150, 70, menuBar)

        menuBar->setEnabled(true);
        menuBar->setPosition(10, 5);
        menuBar->setSize(130, 20);
        menuBar->setTextSize(16);
        menuBar->setMinimumSubMenuWidth(60);

        menuBar->addMenu("File");
        menuBar->addMenuItem("Quit");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenu("Help");

        tgui::MenuBarRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(sf::Color::Red);
        renderer.setSelectedTextColor(sf::Color::Blue);
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setSelectedBackgroundColor(sf::Color::Yellow);
        renderer.setDistanceToSide(3);
        renderer.setOpacity(0.7f);
        menuBar->setRenderer(renderer.getData());

        SECTION("Colored")
        {
            TEST_DRAW("MenuBar.png")

            sf::Vector2f mousePos = {52, 15};
            menuBar->mouseMoved(mousePos);
            menuBar->leftMousePressed(mousePos);
            menuBar->leftMouseReleased(mousePos);

            TEST_DRAW("MenuBar_MenuOpen.png")

            mousePos = {52, 55};
            menuBar->mouseMoved(mousePos);

            TEST_DRAW("MenuBar_MenuHover.png")
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            renderer.setTextureItemBackground("resources/Texture2.png");
            renderer.setTextureSelectedItemBackground("resources/Texture3.png");

            TEST_DRAW("MenuBar_Textured.png")

            sf::Vector2f mousePos = {52, 15};
            menuBar->mouseMoved(mousePos);
            menuBar->leftMousePressed(mousePos);
            menuBar->leftMouseReleased(mousePos);

            TEST_DRAW("MenuBar_MenuOpen_Textured.png")

            mousePos = {52, 55};
            menuBar->mouseMoved(mousePos);

            TEST_DRAW("MenuBar_MenuHover_Textured.png")
        }
    }
}
