/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[Button]")
{
    tgui::Button::Ptr button = std::make_shared<tgui::Button>();
    button->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(button->connect("Pressed", [](){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](sf::String){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(button->getWidgetType() == "Button");
    }

    SECTION("Position and Size")
    {
        button->setPosition(40, 30);
        button->setSize(150, 100);
        button->getRenderer()->setBorders(2);

        REQUIRE(button->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(button->getSize() == sf::Vector2f(150, 100));
        REQUIRE(button->getFullSize() == button->getSize());
        REQUIRE(button->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(button->getText() == "");
        button->setText("SomeText");
        REQUIRE(button->getText() == "SomeText");
    }

    SECTION("TextSize")
    {
        button->setTextSize(25);
        REQUIRE(button->getTextSize() == 25);
    }

    SECTION("Events")
    {
        testClickableWidgetSignals(button);

        button->setPosition(40, 30);
        button->setSize(150, 100);

        SECTION("Pressed signal")
        {
            unsigned int pressedCount = 0;

            button->connect("Pressed", genericCallback, std::ref(pressedCount));

            SECTION("mouse click")
            {
                button->leftMouseReleased(115, 80);
                REQUIRE(pressedCount == 0);

                SECTION("mouse press")
                {
                    button->leftMousePressed(115, 80);
                    REQUIRE(pressedCount == 0);
                }

                button->leftMouseReleased(115, 80);
                REQUIRE(pressedCount == 1);
            }

            SECTION("key pressed")
            {
                sf::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = sf::Keyboard::Space;
                button->keyPressed(keyEvent);
                REQUIRE(pressedCount == 1);

                keyEvent.code = sf::Keyboard::Return;
                button->keyPressed(keyEvent);
                REQUIRE(pressedCount == 2);
            }
        }
    }

    SECTION("Renderer")
    {
        auto renderer = button->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
            }
            
            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
            }

            SECTION("functions")
            {
                renderer->setTextColor({10, 20, 30});
                REQUIRE(renderer->getTextColorNormal() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getTextColorHover() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getTextColorDown() == sf::Color(10, 20, 30));

                renderer->setBackgroundColor({20, 30, 40});
                REQUIRE(renderer->getBackgroundColorNormal() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getBackgroundColorHover() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getBackgroundColorDown() == sf::Color(20, 30, 40));

                renderer->setTextColorNormal({20, 30, 40});
                renderer->setTextColorHover({30, 40, 50});
                renderer->setTextColorDown({40, 50, 60});
                renderer->setBackgroundColorNormal({50, 60, 70});
                renderer->setBackgroundColorHover({60, 70, 80});
                renderer->setBackgroundColorDown({70, 80, 90});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setOpacity(0.8f);
            }

            REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorDown").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);

            REQUIRE(renderer->getTextColorNormal() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getTextColorHover() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getTextColorDown() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getBackgroundColorNormal() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getBackgroundColorHover() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getBackgroundColorDown() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getBorderColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getOpacity() == 0.8f);
        }

        SECTION("textured")
        {
            tgui::Texture textureNormal("resources/Black.png", {0, 64, 45, 50}, {10, 0, 25, 50});
            tgui::Texture textureHover("resources/Black.png", {45, 64, 45, 50}, {10, 0, 25, 50});
            tgui::Texture textureDown("resources/Black.png", {90, 64, 45, 50}, {10, 0, 25, 50});
            tgui::Texture textureFocused("resources/Black.png", {0, 64, 45, 50}, {10, 0, 25, 50});

            REQUIRE(!renderer->getProperty("TextureNormal").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("TextureHover").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("TextureDown").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("TextureFocused").getTexture().isLoaded());

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureNormal", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDown", tgui::Serializer::serialize(textureDown)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", tgui::Serializer::serialize(textureFocused)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureNormal", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDown", textureDown));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", textureFocused));
            }

            SECTION("functions")
            {
                renderer->setTextureNormal(textureNormal);
                renderer->setTextureHover(textureHover);
                renderer->setTextureDown(textureDown);
                renderer->setTextureFocused(textureFocused);
            }

            REQUIRE(renderer->getProperty("TextureNormal").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("TextureHover").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("TextureDown").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("TextureFocused").getTexture().isLoaded());

            REQUIRE(renderer->getTextureNormal().getData() == textureNormal.getData());
            REQUIRE(renderer->getTextureHover().getData() == textureHover.getData());
            REQUIRE(renderer->getTextureDown().getData() == textureDown.getData());
            REQUIRE(renderer->getTextureFocused().getData() == textureFocused.getData());
        }

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }

    SECTION("Saving and loading from file")
    {
        tgui::Theme theme{"resources/Black.txt"};
        button->setRenderer(theme.getRenderer("button"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(button);

        button->setText("SomeText");
        button->setTextSize(25);
        button->getRenderer()->setTextColorNormal({20, 30, 40});
        button->getRenderer()->setTextColorHover({30, 40, 50});
        button->getRenderer()->setTextColorDown({40, 50, 60});
        button->getRenderer()->setBackgroundColorNormal({50, 60, 70});
        button->getRenderer()->setBackgroundColorHover({60, 70, 80});
        button->getRenderer()->setBackgroundColorDown({70, 80, 90});
        button->getRenderer()->setBorderColor({80, 90, 100});
        button->getRenderer()->setBorders({1, 2, 3, 4});
        button->getRenderer()->setOpacity(0.8f);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileButton1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileButton1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileButton2.txt"));
        REQUIRE(compareFiles("WidgetFileButton1.txt", "WidgetFileButton2.txt"));

        SECTION("Copying widget")
        {
            copy(parent, button);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileButton2.txt"));
            REQUIRE(compareFiles("WidgetFileButton1.txt", "WidgetFileButton2.txt"));
        }
    }
}
