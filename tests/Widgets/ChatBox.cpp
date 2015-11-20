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
        auto font = std::make_shared<sf::Font>();
        font->loadFromFile("resources/DroidSansArmenian.ttf");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->setFont("resources/DroidSansArmenian.ttf");
        parent->add(chatBox);

        REQUIRE(chatBox->getLineAmount() == 0);
        chatBox->addLine("Line 1");
        REQUIRE(chatBox->getLineAmount() == 1);
        chatBox->addLine("Line 2", 16);
        chatBox->addLine("Line 3", sf::Color::Green);
        chatBox->addLine("Line 4", sf::Color::Yellow, 18);
        chatBox->addLine("Line 5", sf::Color::Blue, 20, font);
        REQUIRE(chatBox->getLineAmount() == 5);

        REQUIRE(chatBox->getLine(0) == "Line 1");
        REQUIRE(chatBox->getLine(1) == "Line 2");
        REQUIRE(chatBox->getLine(2) == "Line 3");
        REQUIRE(chatBox->getLine(3) == "Line 4");
        REQUIRE(chatBox->getLine(4) == "Line 5");
        
        /// TODO: Also check colors, text size and font
    }

    /// TODO: Test other functions

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

    /// TODO: Saving to and loading from file
}
