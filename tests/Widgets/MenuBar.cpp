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
#include <TGUI/Widgets/MenuBar.hpp>

TEST_CASE("[MenuBar]") {
    tgui::MenuBar::Ptr menuBar = std::make_shared<tgui::MenuBar>();
    menuBar->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](sf::String){}));
        REQUIRE_NOTHROW(menuBar->connect("MenuItemClicked", [](std::vector<sf::String>){}));
    }

    SECTION("WidgetType") {
        REQUIRE(menuBar->getWidgetType() == "MenuBar");
    }

    /// TODO: Test the functions in the MenuBar class

    SECTION("Renderer") {
        auto renderer = menuBar->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setSelectedBackgroundColor({40, 50, 60});
                renderer->setTextColor({70, 80, 90});
                renderer->setSelectedTextColor({100, 110, 120});
                renderer->setDistanceToSide(2);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 5);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(10, 20, 30));
                    REQUIRE(pairs["SelectedBackgroundColor"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["TextColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["SelectedTextColor"].getColor() == sf::Color(100, 110, 120));
                    REQUIRE(pairs["DistanceToSide"].getNumber() == 2);
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
        }

        SECTION("textured") {
            tgui::Texture textureBackground("resources/Black.png", {115, 179, 8, 6}, {2, 2, 4, 2});
            tgui::Texture textureItemBackground("resources/Black.png", {115, 181, 8, 4}, {2, 0, 4, 2});
            tgui::Texture textureSelectedItemBackground("resources/Black.png", {115, 185, 8, 6}, {2, 2, 4, 2});

            REQUIRE(!renderer->getProperty("BackgroundImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ItemBackgroundImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("SelectedItemBackgroundImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("ItemBackgroundImage", tgui::Serializer::serialize(textureItemBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedItemBackgroundImage", tgui::Serializer::serialize(textureSelectedItemBackground)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("ItemBackgroundImage", textureItemBackground));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedItemBackgroundImage", textureSelectedItemBackground));
            }

            SECTION("functions") {
                renderer->setBackgroundTexture(textureBackground);
                renderer->setItemBackgroundTexture(textureItemBackground);
                renderer->setSelectedItemBackgroundTexture(textureSelectedItemBackground);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 8);
                    REQUIRE(pairs["BackgroundImage"].getTexture().getData() == textureBackground.getData());
                    REQUIRE(pairs["ItemBackgroundImage"].getTexture().getData() == textureItemBackground.getData());
                    REQUIRE(pairs["SelectedItemBackgroundImage"].getTexture().getData() == textureSelectedItemBackground.getData());
                }
            }

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ItemBackgroundImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("SelectedItemBackgroundImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().getData() == textureBackground.getData());
            REQUIRE(renderer->getProperty("ItemBackgroundImage").getTexture().getData() == textureItemBackground.getData());
            REQUIRE(renderer->getProperty("SelectedItemBackgroundImage").getTexture().getData() == textureSelectedItemBackground.getData());
        }
    }

    // TODO: Saving and loading from file test
}
