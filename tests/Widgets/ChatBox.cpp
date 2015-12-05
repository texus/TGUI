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
#include <TGUI/Widgets/ChatBox.hpp>

TEST_CASE("[ChatBox]") {
    tgui::ChatBox::Ptr chatBox = std::make_shared<tgui::ChatBox>();
    chatBox->setFont("resources/DroidSansArmenian.ttf");

    SECTION("WidgetType") {
        REQUIRE(chatBox->getWidgetType() == "ChatBox");
    }

    SECTION("adding lines") {
        std::shared_ptr<sf::Font> font1 = tgui::Font{"resources/DroidSansArmenian.ttf"}.getFont();
        std::shared_ptr<sf::Font> font2 = tgui::Font{"resources/DroidSansArmenian.ttf"}.getFont();

        chatBox->setTextColor(sf::Color::Black);
        chatBox->setTextSize(24);
        chatBox->setFont(font1);

        REQUIRE(chatBox->getLineAmount() == 0);
        chatBox->addLine("Line 1");
        REQUIRE(chatBox->getLineAmount() == 1);
        chatBox->addLine("Line 2", 16);
        chatBox->addLine("Line 3", sf::Color::Green);
        chatBox->addLine("Line 4", sf::Color::Yellow, 18);
        chatBox->addLine("Line 5", sf::Color::Blue, 20, font2);
        REQUIRE(chatBox->getLineAmount() == 5);

        REQUIRE(chatBox->getLine(0) == "Line 1");
        REQUIRE(chatBox->getLine(1) == "Line 2");
        REQUIRE(chatBox->getLine(2) == "Line 3");
        REQUIRE(chatBox->getLine(3) == "Line 4");
        REQUIRE(chatBox->getLine(4) == "Line 5");
        
        REQUIRE(chatBox->getLineColor(0) == sf::Color::Black);
        REQUIRE(chatBox->getLineColor(1) == sf::Color::Black);
        REQUIRE(chatBox->getLineColor(2) == sf::Color::Green);
        REQUIRE(chatBox->getLineColor(3) == sf::Color::Yellow);
        REQUIRE(chatBox->getLineColor(4) == sf::Color::Blue);

        REQUIRE(chatBox->getLineTextSize(0) == 24);
        REQUIRE(chatBox->getLineTextSize(1) == 16);
        REQUIRE(chatBox->getLineTextSize(2) == 24);
        REQUIRE(chatBox->getLineTextSize(3) == 18);
        REQUIRE(chatBox->getLineTextSize(4) == 20);

        REQUIRE(chatBox->getLineFont(0) == font1);
        REQUIRE(chatBox->getLineFont(1) == font1);
        REQUIRE(chatBox->getLineFont(2) == font1);
        REQUIRE(chatBox->getLineFont(3) == font1);
        REQUIRE(chatBox->getLineFont(4) == font2);
    }

    SECTION("removing lines") {
        REQUIRE(!chatBox->removeLine(0));
        chatBox->addLine("Line 1");
        chatBox->addLine("Line 2");
        chatBox->addLine("Line 3");
        REQUIRE(!chatBox->removeLine(5));

        REQUIRE(chatBox->removeLine(1));
        REQUIRE(chatBox->getLineAmount() == 2);
        REQUIRE(chatBox->getLine(0) == "Line 1");
        REQUIRE(chatBox->getLine(1) == "Line 3");

        chatBox->removeAllLines();
        REQUIRE(chatBox->getLineAmount() == 0);
    }

    SECTION("line limit") {
        chatBox->addLine("Line 1");
        chatBox->addLine("Line 2");
        chatBox->addLine("Line 3");

        chatBox->setLineLimit(2);
        REQUIRE(chatBox->getLineAmount() == 2);
        REQUIRE(chatBox->getLine(0) == "Line 2");
        REQUIRE(chatBox->getLine(1) == "Line 3");

        chatBox->addLine("Line 4");
        REQUIRE(chatBox->getLine(0) == "Line 3");
        REQUIRE(chatBox->getLine(1) == "Line 4");
    }

    SECTION("default text size") {
        chatBox->setTextSize(30);
        REQUIRE(chatBox->getTextSize() == 30);

        chatBox->addLine("Text");
        REQUIRE(chatBox->getLineTextSize(0) == 30);
    }

    SECTION("default text color") {
        chatBox->setTextColor(sf::Color::Red);
        REQUIRE(chatBox->getTextColor() == sf::Color::Red);

        chatBox->addLine("Text");
        REQUIRE(chatBox->getLineColor(0) == sf::Color::Red);
    }

    SECTION("get unexisting line") {
        std::shared_ptr<sf::Font> font1 = tgui::Font{"resources/DroidSansArmenian.ttf"}.getFont();
        std::shared_ptr<sf::Font> font2 = tgui::Font{"resources/DroidSansArmenian.ttf"}.getFont();

        chatBox->setTextColor(sf::Color::Yellow);
        chatBox->setTextSize(26);
        chatBox->setFont(font1);

        chatBox->addLine("Text", sf::Color::Blue, 20, font2);
        REQUIRE(chatBox->getLine(1) == "");
        REQUIRE(chatBox->getLineTextSize(2) == 26);
        REQUIRE(chatBox->getLineColor(3) == sf::Color::Yellow);
        REQUIRE(chatBox->getLineFont(4) == font1);
    }

    SECTION("lines start from top or bottom") {
        REQUIRE(!chatBox->getLinesStartFromTop());
        chatBox->setLinesStartFromTop(true);
        REQUIRE(chatBox->getLinesStartFromTop());
        chatBox->setLinesStartFromTop(false);
        REQUIRE(!chatBox->getLinesStartFromTop());
    }

    SECTION("Scrollbar") {
        tgui::Scrollbar::Ptr scrollbar = std::make_shared<tgui::Theme>()->load("scrollbar");
        REQUIRE(chatBox->getScrollbar() != nullptr);
        REQUIRE(chatBox->getScrollbar() != scrollbar);
        chatBox->setScrollbar(nullptr);
        REQUIRE(chatBox->getScrollbar() == nullptr);
        chatBox->setScrollbar(scrollbar);
        REQUIRE(chatBox->getScrollbar() != nullptr);
        REQUIRE(chatBox->getScrollbar() == scrollbar);
    }

    SECTION("Renderer") {
        auto renderer = chatBox->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setBorderColor({40, 50, 60});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 4);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(10, 20, 30));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(5, 6, 7, 8));
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(40, 50, 60));
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
                    REQUIRE(pairs.size() == 4);
                    REQUIRE(pairs["BackgroundImage"].getTexture().getData() == textureBackground.getData());
                }
            }

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(chatBox = std::make_shared<tgui::Theme>()->load("ChatBox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(chatBox = theme->load("ChatBox"));
        REQUIRE(chatBox->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(chatBox->getSecondaryLoadingParameter() == "chatbox");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(chatBox);

        chatBox->setTextColor(sf::Color::White);
        chatBox->setTextSize(34);
        chatBox->addLine("L4");
        chatBox->addLine("L2", 32);
        chatBox->addLine("L3", sf::Color::Magenta);
        chatBox->addLine("L1", sf::Color::Cyan, 36);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileChatBox1.txt"));

        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileChatBox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileChatBox2.txt"));
        REQUIRE(compareFiles("WidgetFileChatBox1.txt", "WidgetFileChatBox2.txt"));

        // Make sure that the lines are still in the correct order
        REQUIRE(chatBox->getLine(0) == "L4");
        REQUIRE(chatBox->getLine(1) == "L2");
        REQUIRE(chatBox->getLine(2) == "L3");
        REQUIRE(chatBox->getLine(3) == "L1");

        SECTION("Copying widget") {
            tgui::ChatBox temp;
            temp = *chatBox;

            parent->removeAllWidgets();
            parent->add(tgui::ChatBox::copy(std::make_shared<tgui::ChatBox>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileChatBox2.txt"));
            REQUIRE(compareFiles("WidgetFileChatBox1.txt", "WidgetFileChatBox2.txt"));
        }
    }
}
