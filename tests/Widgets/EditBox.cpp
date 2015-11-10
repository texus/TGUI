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
#include <TGUI/Widgets/EditBox.hpp>

TEST_CASE("[EditBox]") {
    tgui::EditBox::Ptr editBox = std::make_shared<tgui::EditBox>();
    editBox->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](){}));

        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](sf::String){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(editBox->getWidgetType() == "EditBox");
    }

    SECTION("Text") {
        REQUIRE(editBox->getText() == "");
        editBox->setText("SomeText");
        REQUIRE(editBox->getText() == "SomeText");
    }
    
    SECTION("DefaultText") {
        REQUIRE(editBox->getDefaultText() == "");
        editBox->setDefaultText("SomeDefaultText");
        REQUIRE(editBox->getDefaultText() == "SomeDefaultText");
    }

    SECTION("TextSize") {
        editBox->setTextSize(25);
        REQUIRE(editBox->getTextSize() == 25);
    }

    SECTION("PasswordCharacter") {
        REQUIRE(editBox->getPasswordCharacter() == '\0');
        editBox->setPasswordCharacter('*');
        REQUIRE(editBox->getPasswordCharacter() == '*');
    }

    SECTION("MaximumCharacters") {
        REQUIRE(editBox->getMaximumCharacters() == 0);
        editBox->setText("1234567890");

        editBox->setMaximumCharacters(5);
        REQUIRE(editBox->getMaximumCharacters() == 5);
        REQUIRE(editBox->getText() == "12345");

        editBox->setText("0987654321");
        REQUIRE(editBox->getText() == "09876");

        editBox->setMaximumCharacters(0);
        REQUIRE(editBox->getText() == "09876");
        
        editBox->setText("1234567890");
        REQUIRE(editBox->getText() == "1234567890");
    }

    SECTION("Alignment") {
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Left);
        
        editBox->setAlignment(tgui::EditBox::Alignment::Center);
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Center);
        
        editBox->setAlignment(tgui::EditBox::Alignment::Right);
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Right);
        
        editBox->setAlignment(tgui::EditBox::Alignment::Left);
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Left);
    }

    SECTION("LimitTextWidth") {
        REQUIRE(!editBox->isTextWidthLimited());
        editBox->limitTextWidth(true);
        REQUIRE(editBox->isTextWidthLimited());
        editBox->limitTextWidth(false);
        REQUIRE(!editBox->isTextWidthLimited());
    }

    SECTION("CaretWidth") {
        editBox->setCaretWidth(3);
        REQUIRE(editBox->getCaretWidth() == 3);
    }

    SECTION("NumbersOnly") {
        REQUIRE(!editBox->isNumbersOnly());
        editBox->setText("Some123Text456");
        REQUIRE(editBox->getText() == "Some123Text456");

        editBox->setNumbersOnly(true);
        REQUIRE(editBox->isNumbersOnly());
        REQUIRE(editBox->getText() == "123456");

        editBox->setText("098Some765Text");
        REQUIRE(editBox->getText() == "098765");

        editBox->setNumbersOnly(false);
        REQUIRE(!editBox->isNumbersOnly());
    }

    SECTION("Renderer") {
        auto renderer = editBox->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(10, 20, 30));

                renderer->setTextColor({20, 30, 40});
                renderer->setSelectedTextColor({30, 40, 50});
                renderer->setSelectedTextBackgroundColor({40, 50, 60});
                renderer->setDefaultTextColor({50, 60, 70});
                renderer->setCaretColor({60, 70, 80});
                renderer->setBackgroundColorNormal({70, 80, 90});
                renderer->setBackgroundColorHover({80, 90, 100});
                renderer->setBorderColor({90, 100, 110});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 10);
                    REQUIRE(pairs["TextColor"].getColor() == sf::Color(20, 30, 40));
                    REQUIRE(pairs["SelectedTextColor"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["SelectedTextBackgroundColor"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["DefaultTextColor"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["CaretColor"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["BackgroundColorNormal"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["BackgroundColorHover"].getColor() == sf::Color(80, 90, 100));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(90, 100, 110));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(5, 6, 7, 8));
                }
            }
 
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("SelectedTextBackgroundColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("DefaultTextColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("CaretColor").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getBorders() == tgui::Borders(5, 6, 7, 8));
        }

        SECTION("textured") {
            tgui::Texture textureNormal("resources/Black.png", {0, 114, 60, 40}, {15, 0, 30, 40});
            tgui::Texture textureHover("resources/Black.png", {0, 114, 60, 40}, {15, 0, 30, 40});
            tgui::Texture textureFocused("resources/Black.png", {0, 114, 60, 40}, {15, 0, 30, 40});

            REQUIRE(!renderer->getProperty("NormalImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("HoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("FocusedImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("NormalImage", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("HoverImage", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("FocusedImage", tgui::Serializer::serialize(textureFocused)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("NormalImage", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("HoverImage", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("FocusedImage", textureFocused));
            }

            SECTION("functions") {
                renderer->setNormalTexture(textureNormal);
                renderer->setHoverTexture(textureHover);
                renderer->setFocusTexture(textureFocused);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 11);
                    REQUIRE(pairs["NormalImage"].getTexture().getData() == textureNormal.getData());
                    REQUIRE(pairs["HoverImage"].getTexture().getData() == textureHover.getData());
                    REQUIRE(pairs["FocusedImage"].getTexture().getData() == textureFocused.getData());
                }
            }

            REQUIRE(renderer->getProperty("NormalImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("HoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("FocusedImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("NormalImage").getTexture().getData() == textureNormal.getData());
            REQUIRE(renderer->getProperty("HoverImage").getTexture().getData() == textureHover.getData());
            REQUIRE(renderer->getProperty("FocusedImage").getTexture().getData() == textureFocused.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(editBox = std::make_shared<tgui::Theme>()->load("EditBox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(editBox = theme->load("EditBox"));
        REQUIRE(editBox->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(editBox->getSecondaryLoadingParameter() == "editbox");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(editBox);
        
        editBox->setText("SomeText");
        editBox->setDefaultText("SomeDefaultText");
        editBox->setTextSize(25);
        editBox->setPasswordCharacter('*');
        editBox->setMaximumCharacters(5);
        editBox->setAlignment(tgui::EditBox::Alignment::Right);
        editBox->limitTextWidth();
        editBox->setCaretWidth(3);
        editBox->setNumbersOnly();

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileEditBox1.txt"));

        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileEditBox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileEditBox2.txt"));
        REQUIRE(compareFiles("WidgetFileEditBox1.txt", "WidgetFileEditBox2.txt"));
    }

    SECTION("Bug #43 (https://github.com/texus/TGUI/issues/43)") {
        sf::Event::KeyEvent event;
        event.control = true;
        event.code = sf::Keyboard::A;

        editBox->setText("Test");
        editBox->keyPressed(event);
        REQUIRE(editBox->getSelectedText() == "Test");

        editBox->setText("Test");
        REQUIRE(editBox->getSelectedText() == "");

        // ctrl+alt+A must not function as ctrl+A
        event.alt = true;
        editBox->keyPressed(event);
        REQUIRE(editBox->getSelectedText() == "");
    }
}
