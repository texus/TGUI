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
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/TextBox.hpp>

TEST_CASE("[TextBox]") {
    tgui::TextBox::Ptr textBox = std::make_shared<tgui::TextBox>();
    textBox->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](){}));
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(textBox->getWidgetType() == "TextBox");
    }

    /// TODO: Tests for functions in TextBox class

    SECTION("Scrollbar") {
        tgui::Scrollbar::Ptr scrollbar = std::make_shared<tgui::Theme>()->load("scrollbar");
    
        REQUIRE(textBox->getScrollbar() != nullptr);
        REQUIRE(textBox->getScrollbar() != scrollbar);
        textBox->setScrollbar(nullptr);
        REQUIRE(textBox->getScrollbar() == nullptr);
        textBox->setScrollbar(scrollbar);
        REQUIRE(textBox->getScrollbar() != nullptr);
        REQUIRE(textBox->getScrollbar() == scrollbar);
    }

    SECTION("Renderer") {
        auto renderer = textBox->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", sf::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setTextColor({40, 50, 60});
                renderer->setSelectedTextColor({70, 80, 90});
                renderer->setSelectedTextBackgroundColor({100, 110, 120});
                renderer->setCaretColor({130, 140, 150});
                renderer->setBorderColor({160, 170, 180});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 8);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(10, 20, 30));
                    REQUIRE(pairs["TextColor"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["SelectedTextColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["SelectedTextBackgroundColor"].getColor() == sf::Color(100, 110, 120));
                    REQUIRE(pairs["CaretColor"].getColor() == sf::Color(130, 140, 150));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(160, 170, 180));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(5, 6, 7, 8));
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextBackgroundColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("CaretColor").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(160, 170, 180));
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
                    REQUIRE(pairs.size() == 8);
                    REQUIRE(pairs["BackgroundImage"].getTexture().getData() == textureBackground.getData());
                }
            }

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(textBox = std::make_shared<tgui::Theme>()->load("TextBox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(textBox = theme->load("TextBox"));
        REQUIRE(textBox->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(textBox->getSecondaryLoadingParameter() == "textbox");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(textBox);
        
        textBox->setText("This is the text in the text box!");
        textBox->setTextSize(25);
        textBox->setMaximumCharacters(16);
        textBox->setReadOnly(true);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTextBox1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileTextBox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileTextBox2.txt"));
        REQUIRE(compareFiles("WidgetFileTextBox1.txt", "WidgetFileTextBox2.txt"));
    }

    SECTION("Bug #43 (https://github.com/texus/TGUI/issues/43)") {
        sf::Event::KeyEvent event;
        event.control = true;
        event.alt     = false;
        event.shift   = false;
        event.system  = false;
        event.code    = sf::Keyboard::A;

        textBox->setText("Test");
        textBox->keyPressed(event);
        REQUIRE(textBox->getSelectedText() == "Test");

        textBox->setText("Test");
        REQUIRE(textBox->getSelectedText() == "");

        // ctrl+alt+A must not function as ctrl+A
        event.alt = true;
        textBox->keyPressed(event);
        REQUIRE(textBox->getSelectedText() == "");
    }
}
