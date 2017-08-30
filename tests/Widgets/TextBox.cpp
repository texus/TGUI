/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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
#include <TGUI/Widgets/TextBox.hpp>

TEST_CASE("[TextBox]")
{
    tgui::TextBox::Ptr textBox = tgui::TextBox::create();
    textBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](){}));
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](sf::String){}));
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](std::string){}));
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(textBox->connect("TextChanged", [](tgui::Widget::Ptr, std::string, std::string){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(textBox->getWidgetType() == "TextBox");
    }

    SECTION("Position and Size")
    {
        textBox->setPosition(40, 30);
        textBox->setSize(150, 100);
        textBox->getRenderer()->setBorders(2);

        REQUIRE(textBox->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(textBox->getSize() == sf::Vector2f(150, 100));
        REQUIRE(textBox->getFullSize() == textBox->getSize());
        REQUIRE(textBox->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(textBox->getText() == "");

        textBox->setText("Hello");
        REQUIRE(textBox->getText() == "Hello");

        textBox->setText("World");
        REQUIRE(textBox->getText() == "World");

        textBox->addText("\n\tText");
        REQUIRE(textBox->getText() == "World\n\tText");
    }

    SECTION("TextSize")
    {
        textBox->setTextSize(50);
        REQUIRE(textBox->getTextSize() == 50);
    }

    SECTION("MaximumCharacters")
    {
        SECTION("Limit is set")
        {
            textBox->setMaximumCharacters(4);
            REQUIRE(textBox->getMaximumCharacters() == 4);
        }

        SECTION("Existing text is cut")
        {
            textBox->setText("Hello World!");
            textBox->setMaximumCharacters(4);
            REQUIRE(textBox->getText() == "Hell");
        }

        SECTION("Added text is cut")
        {
            textBox->setMaximumCharacters(7);
            textBox->setText("Hello World!");
            REQUIRE(textBox->getText() == "Hello W");

            textBox->addText("\nMore Text.");
            REQUIRE(textBox->getText() == "Hello W");

            textBox->setText("Different Text");
            REQUIRE(textBox->getText() == "Differe");
        }
    }

    SECTION("ReadOnly")
    {
        SECTION("Changing the read-only state")
        {
            REQUIRE(textBox->isReadOnly() == false);

            textBox->setReadOnly(true);
            REQUIRE(textBox->isReadOnly() == true);

            textBox->setReadOnly(false);
            REQUIRE(textBox->isReadOnly() == false);
        }

        SECTION("Read-only does not affect setText calls")
        {
            textBox->setText("Hello");
            textBox->setReadOnly(true);
            textBox->setText("World");
            REQUIRE(textBox->getText() == "World");
        }

        SECTION("You can't type when the text box is read-only")
        {
            textBox->setReadOnly(true);
            textBox->textEntered('x');
            REQUIRE(textBox->getText() == "");

            textBox->setReadOnly(false);
            textBox->textEntered('x');
            REQUIRE(textBox->getText() == "x");
        }
    }

    SECTION("VerticalScrollbarPresent")
    {
        REQUIRE(textBox->isVerticalScrollbarPresent() == true);

        textBox->setVerticalScrollbarPresent(false);
        REQUIRE(textBox->isVerticalScrollbarPresent() == false);

        textBox->setVerticalScrollbarPresent(true);
        REQUIRE(textBox->isVerticalScrollbarPresent() == true);
    }

    // TODO: setCaretPosition
    // TODO: getLinesCount

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(textBox);
        }

        // TODO
    }

    testWidgetRenderer(textBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = textBox->getRenderer();

        SECTION("colored")
        {
            tgui::ScrollbarRenderer scrollbarRenderer;
            scrollbarRenderer.setTrackColor(sf::Color::Red);
            scrollbarRenderer.setThumbColor(sf::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretWidth", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", "{ TrackColor = Red; ThumbColor = Blue; }"));
            }
            
            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", sf::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretWidth", 2));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", scrollbarRenderer.getData()));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setTextColor({40, 50, 60});
                renderer->setSelectedTextColor({70, 80, 90});
                renderer->setSelectedTextBackgroundColor({100, 110, 120});
                renderer->setCaretColor({130, 140, 150});
                renderer->setBorderColor({160, 170, 180});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setCaretWidth(2);

                REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 0);
                renderer->setScrollbar(scrollbarRenderer.getData());
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextBackgroundColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("CaretColor").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("CaretWidth").getNumber() == 2);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["trackcolor"].getColor() == sf::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["thumbcolor"].getColor() == sf::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        textBox->setText("This is the text in the text box!");
        textBox->setTextSize(25);
        textBox->setMaximumCharacters(16);
        textBox->setReadOnly(true);
        textBox->setVerticalScrollbarPresent(false);

        testSavingWidget("TextBox", textBox);
    }

    SECTION("Bug Fixes")
    {
        SECTION("ctrl+alt+A should not act as ctrl+A (https://github.com/texus/TGUI/issues/43)")
        {
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
}
