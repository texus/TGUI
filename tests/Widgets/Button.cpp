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
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[Button]") {
    tgui::Button::Ptr button = std::make_shared<tgui::Button>();

    SECTION("Signals") {
        REQUIRE_NOTHROW(button->connect("Pressed", [](){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(button->getWidgetType() == "Button");
    }

    SECTION("Text") {
        REQUIRE(button->getText() == "");
        button->setText("SomeText");
        REQUIRE(button->getText() == "SomeText");
    }

    SECTION("TextSize") {
        button->setTextSize(25);
        REQUIRE(button->getTextSize() == 25);
    }

    SECTION("Renderer") {
        auto renderer = button->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorDown").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorDown").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions") {
                renderer->setTextColor({10, 20, 30});
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorDown").getColor() == sf::Color(10, 20, 30));

                renderer->setBackgroundColor({20, 30, 40});
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == sf::Color(20, 30, 40));

                renderer->setTextColorNormal({20, 30, 40});
                renderer->setTextColorHover({30, 40, 50});
                renderer->setTextColorDown({40, 50, 60});
                renderer->setBackgroundColorNormal({50, 60, 70});
                renderer->setBackgroundColorHover({60, 70, 80});
                renderer->setBackgroundColorDown({70, 80, 90});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 8);
                    REQUIRE(pairs["TextColorNormal"].getColor() == sf::Color(20, 30, 40));
                    REQUIRE(pairs["TextColorHover"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["TextColorDown"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["BackgroundColorNormal"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["BackgroundColorHover"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["BackgroundColorDown"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(80, 90, 100));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                }
            }

            REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorDown").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured") {
            tgui::Texture textureNormal("resources/Black.png", {0, 64, 45, 50}, {10, 0, 25, 50});
            tgui::Texture textureHover("resources/Black.png", {45, 64, 45, 50}, {10, 0, 25, 50});
            tgui::Texture textureDown("resources/Black.png", {90, 64, 45, 50}, {10, 0, 25, 50});
            tgui::Texture textureFocused("resources/Black.png", {0, 64, 45, 50}, {10, 0, 25, 50});

            REQUIRE(!renderer->getProperty("NormalImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("HoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("DownImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("FocusedImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("NormalImage", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("HoverImage", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("DownImage", tgui::Serializer::serialize(textureDown)));
                REQUIRE_NOTHROW(renderer->setProperty("FocusedImage", tgui::Serializer::serialize(textureFocused)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("NormalImage", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("HoverImage", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("DownImage", textureDown));
                REQUIRE_NOTHROW(renderer->setProperty("FocusedImage", textureFocused));
            }

            SECTION("functions") {
                renderer->setNormalTexture(textureNormal);
                renderer->setHoverTexture(textureHover);
                renderer->setDownTexture(textureDown);
                renderer->setFocusTexture(textureFocused);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 9);
                    REQUIRE(pairs["NormalImage"].getTexture().getData() == textureNormal.getData());
                    REQUIRE(pairs["HoverImage"].getTexture().getData() == textureHover.getData());
                    REQUIRE(pairs["DownImage"].getTexture().getData() == textureDown.getData());
                    REQUIRE(pairs["FocusedImage"].getTexture().getData() == textureFocused.getData());
                }
            }

            REQUIRE(renderer->getProperty("NormalImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("HoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("DownImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("FocusedImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("NormalImage").getTexture().getData() == textureNormal.getData());
            REQUIRE(renderer->getProperty("HoverImage").getTexture().getData() == textureHover.getData());
            REQUIRE(renderer->getProperty("DownImage").getTexture().getData() == textureDown.getData());
            REQUIRE(renderer->getProperty("FocusedImage").getTexture().getData() == textureFocused.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(button = std::make_shared<tgui::Theme>()->load("Button"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(button = theme->load("Button"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(button);
        
        button->setText("SomeText");
        button->setTextSize(25);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileButton1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileButton1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileButton2.txt"));
        REQUIRE(compareFiles("WidgetFileButton1.txt", "WidgetFileButton2.txt"));
    }
}
