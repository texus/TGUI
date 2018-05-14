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
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Clipboard.hpp>

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

    SECTION("CaretPosition")
    {
        REQUIRE(textBox->getCaretPosition() == 0);

        textBox->setText("Some\ntext");
        REQUIRE(textBox->getCaretPosition() == 9);

        textBox->setCaretPosition(7);
        REQUIRE(textBox->getCaretPosition() == 7);

        textBox->setCaretPosition(25);
        REQUIRE(textBox->getCaretPosition() == 9);
    }

    SECTION("LinesCount")
    {
        REQUIRE(textBox->getLinesCount() == 1);

        textBox->setText("Some text");
        REQUIRE(textBox->getLinesCount() == 1);

        textBox->setText("More\nthan\none\nline");
        REQUIRE(textBox->getLinesCount() == 4);
    }

    SECTION("Events / Signals")
    {
        textBox->setSize(165, 100);
        textBox->setTextSize(24);

        SECTION("Widget")
        {
            testWidgetSignals(textBox);
        }

        SECTION("KeyPressed")
        {
            auto keyEvent = [](sf::Keyboard::Key key, bool control, bool shift) {
                sf::Event::KeyEvent event;
                event.control = control;
                event.alt     = false;
                event.shift   = shift;
                event.system  = false;
                event.code    = key;
                return event;
            };

            textBox->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

            SECTION("Selecting text")
            {
                REQUIRE(textBox->getSelectedText() == "");

                // Move the caret
                for (unsigned int i = 0; i < 5; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Up, false, false));
                for (unsigned int i = 0; i < 3; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Right, false, false));
                for (unsigned int i = 0; i < 1; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Down, false, false));

                REQUIRE(textBox->getSelectedText() == "");

                // Move the caret while the shift key is pressed (text will be selected)
                for (unsigned int i = 0; i < 2; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, true));
                for (unsigned int i = 0; i < 1; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Down, false, true));
                for (unsigned int i = 0; i < 3; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Right, false, true));
                for (unsigned int i = 0; i < 2; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Up, false, true));

                REQUIRE(textBox->getSelectedText() == "GHIJKLMNO");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::PageUp, false, true));
                REQUIRE(textBox->getSelectedText() == "ABCDEFGHIJKLMNO");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::PageDown, false, true));
                REQUIRE(textBox->getSelectedText() == "PQRSTUVWXYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::PageUp, false, false));
                REQUIRE(textBox->getSelectedText() == "");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::PageDown, false, false));
                REQUIRE(textBox->getSelectedText() == "");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Home, false, true));
                REQUIRE(textBox->getSelectedText() == "TUVWX");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::End, false, true));
                REQUIRE(textBox->getSelectedText() == "YZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Home, false, false));
                REQUIRE(textBox->getSelectedText() == "");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::End, false, false));
                REQUIRE(textBox->getSelectedText() == "");

                // CTRL+A selects the whole text
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::A, true, false));
                REQUIRE(textBox->getSelectedText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            }

            SECTION("Editing text")
            {
                for (unsigned int i = 0; i < 4; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::BackSpace, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::BackSpace, false, false));
                REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTWXYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Delete, false, false));
                REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTXYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Up, false, false));
                for (unsigned int i = 0; i < 3; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Right, false, true));

                REQUIRE(textBox->getSelectedText() == "LMN");
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::BackSpace, false, false));
                REQUIRE(textBox->getText() == "ABCDEFGHIJKOPQRSTXYZ");
                REQUIRE(textBox->getSelectedText() == "");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Up, false, false));
                for (unsigned int i = 0; i < 3; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Right, false, true));

                REQUIRE(textBox->getSelectedText() == "BCD");
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Delete, false, false));
                REQUIRE(textBox->getText() == "AEFGHIJKOPQRSTXYZ");
                REQUIRE(textBox->getSelectedText() == "");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Return, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Return, false, false));
                REQUIRE(textBox->getText() == "A\n\nEFGHIJKOPQRSTXYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::BackSpace, false, false));
                REQUIRE(textBox->getText() == "A\nEFGHIJKOPQRSTXYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Right, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Down, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::BackSpace, false, false));
                REQUIRE(textBox->getText() == "A\nEFGHIJKOPRSTXYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Right, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Return, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::BackSpace, false, false));
                REQUIRE(textBox->getText() == "A\nEFGHIJKOP\n\nSTXYZ");

                // The caret should not move when adding a newline at the start of a line that only exists due to word-wrap
                textBox->setText(""); // Make sure the scrollbar is gone
                textBox->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
                for (unsigned int i = 0; i < 17; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Return, false, false));
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Delete, false, false));
                REQUIRE(textBox->getText() == "ABCDEFGHIJ\nLMNOPQRSTUVWXYZ");
            }

            SECTION("Copy and Paste")
            {
                for (unsigned int i = 0; i < 3; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, true));

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::C, true, false));
                REQUIRE(tgui::Clipboard::get() == "XYZ");

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, false)); // Deselect text
                textBox->keyPressed(keyEvent(sf::Keyboard::Key::V, true, false));
                REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZXYZ");
                REQUIRE(tgui::Clipboard::get() == "XYZ");

                for (unsigned int i = 0; i < 4; ++i)
                    textBox->keyPressed(keyEvent(sf::Keyboard::Key::Left, false, true));

                textBox->keyPressed(keyEvent(sf::Keyboard::Key::X, true, false));
                REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTUVXYZ");
                REQUIRE(tgui::Clipboard::get() == "WXYZ");
            }
        }

        SECTION("TextEntered")
        {
            textBox->setText("ABC");
            textBox->setMaximumCharacters(4);

            textBox->textEntered('D');
            REQUIRE(textBox->getText() == "ABCD");

            textBox->textEntered('E');
            REQUIRE(textBox->getText() == "ABCD");

            textBox->setMaximumCharacters(0);
            textBox->textEntered('E');
            REQUIRE(textBox->getText() == "ABCDE");

            textBox->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            textBox->setVerticalScrollbarPresent(false);

            textBox->textEntered('A');
            textBox->textEntered('B');
            REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZAB");

            textBox->textEntered('C');
            REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZAB");

            textBox->setVerticalScrollbarPresent(true);
            textBox->textEntered('C');
            REQUIRE(textBox->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZABC");
        }
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
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", "15"));
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
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", 15));
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
                renderer->setScrollbar(scrollbarRenderer.getData());
                renderer->setScrollbarWidth(15);
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
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

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

    SECTION("Draw")
    {
        TEST_DRAW_INIT(270, 160, textBox)

        textBox->setEnabled(true);
        textBox->setFocused(true);
        textBox->setPosition(10, 5);
        textBox->setSize(250, 150);
        textBox->setText("Something");
        textBox->setTextSize(16);
        textBox->setText("Cupcake ipsum dolor sit. Amet candy canes sesame snaps cupcake apple pie lemon drops jelly apple pie candy canes. Powder donut sugar plum croissant gingerbread sesame snaps pie. Caramels cupcake icing. Bear claw lemon drops carrot cake chocolate ice cream jelly beans gummi bears.");

        tgui::TextBoxRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(sf::Color::Yellow);
        renderer.setTextColor(sf::Color::Blue);
        renderer.setSelectedTextColor(sf::Color::Red);
        renderer.setSelectedTextBackgroundColor(sf::Color::Cyan);
        renderer.setCaretColor(sf::Color::Green);
        renderer.setBorderColor(sf::Color::Magenta);
        renderer.setCaretWidth(2);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setOpacity(0.7f);
        textBox->setRenderer(renderer.getData());

        tgui::ScrollbarRenderer scrollbarRenderer = tgui::RendererData::create();
        scrollbarRenderer.setTrackColor(sf::Color::Green);
        scrollbarRenderer.setThumbColor(sf::Color::Red);
        scrollbarRenderer.setArrowBackgroundColor(sf::Color::Blue);
        scrollbarRenderer.setArrowColor(sf::Color::White);
        scrollbarRenderer.setOpacity(0.7f);
        renderer.setScrollbar(scrollbarRenderer.getData());

        textBox->leftMousePressed({87, 66});
        textBox->mouseMoved({136, 105});
        textBox->leftMouseReleased({136, 105});

        SECTION("Colored")
        {
            TEST_DRAW("TextBox.png")
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            TEST_DRAW("TextBox_Textured.png")
        }
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
