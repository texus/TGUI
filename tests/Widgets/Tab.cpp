/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/Tab.hpp>

TEST_CASE("[Tab]") {
    tgui::Tab::Ptr tab = std::make_shared<tgui::Tab>();
    tab->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(tab->connect("TabSelected", [](){}));
        REQUIRE_NOTHROW(tab->connect("TabSelected", [](sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(tab->getWidgetType() == "Tab");
    }

    /// TODO: Test the functions in the Tab class

    SECTION("Renderer") {
        auto renderer = tab->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setSelectedBackgroundColor({40, 50, 60});
                renderer->setTextColor({70, 80, 90});
                renderer->setSelectedTextColor({100, 110, 120});
                renderer->setBorderColor({130, 140, 150});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setDistanceToSide(2);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 7);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(10, 20, 30));
                    REQUIRE(pairs["SelectedBackgroundColor"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["TextColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["SelectedTextColor"].getColor() == sf::Color(100, 110, 120));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(130, 140, 150));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["DistanceToSide"].getNumber() == 2);
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
        }

        SECTION("textured") {
            tgui::Texture textureNormal("resources/Black.png", {0, 0, 60, 32}, {16, 0, 28, 32});
            tgui::Texture textureSelected("resources/Black.png", {0, 32, 60, 32}, {16, 0, 28, 32});

            REQUIRE(!renderer->getProperty("NormalImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("SelectedImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("NormalImage", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedImage", tgui::Serializer::serialize(textureSelected)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("NormalImage", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedImage", textureSelected));
            }

            SECTION("functions") {
                renderer->setNormalTexture(textureNormal);
                renderer->setSelectedTexture(textureSelected);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 7);
                    REQUIRE(pairs["NormalImage"].getTexture().getData() == textureNormal.getData());
                    REQUIRE(pairs["SelectedImage"].getTexture().getData() == textureSelected.getData());
                }
            }

            REQUIRE(renderer->getProperty("NormalImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("SelectedImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("NormalImage").getTexture().getData() == textureNormal.getData());
            REQUIRE(renderer->getProperty("SelectedImage").getTexture().getData() == textureSelected.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(tab = std::make_shared<tgui::Theme>()->load("Tab"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(tab = theme->load("Tab"));
        REQUIRE(tab->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(tab->getSecondaryLoadingParameter() == "tab");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(tab);

        tab->setOpacity(0.8f);
        tab->add("1");
        tab->add("2");
        tab->add("3");
        tab->select("2");
        tab->setTextSize(20);
        tab->setTabHeight(26);
        tab->setMaximumTabWidth(100);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTab1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileTab1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTab2.txt"));
        REQUIRE(compareFiles("WidgetFileTab1.txt", "WidgetFileTab2.txt"));

        SECTION("Copying widget") {
            tgui::Tab temp;
            temp = *tab;

            parent->removeAllWidgets();
            parent->add(tgui::Tab::copy(std::make_shared<tgui::Tab>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTab2.txt"));
            REQUIRE(compareFiles("WidgetFileTab1.txt", "WidgetFileTab2.txt"));
        }
    }
}
