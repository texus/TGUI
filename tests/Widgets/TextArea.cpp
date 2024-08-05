/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

namespace
{
    tgui::Event::KeyEvent createKeyEvent(tgui::Event::KeyboardKey key, bool control, bool shift)
    {
        tgui::Event::KeyEvent event;
        event.alt     = false;
        event.shift   = shift;
        event.code    = key;
#ifdef TGUI_SYSTEM_MACOS
        event.control = false;
        event.system  = control;
#else
        event.control = control;
        event.system  = false;
#endif
        return event;
    }
}

TEST_CASE("[TextArea]")
{
    tgui::TextArea::Ptr textArea = tgui::TextArea::create();
    textArea->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        textArea->onTextChange([](){});
        textArea->onTextChange([](const tgui::String&){});

        textArea->onCaretPositionChange([](){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(textArea)->getSignal("TextChanged").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(textArea)->getSignal("CaretPositionChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(textArea->getWidgetType() == "TextArea");
    }

    SECTION("Position and Size")
    {
        textArea->setPosition(40, 30);
        textArea->setSize(150, 100);
        textArea->getRenderer()->setBorders(2);

        REQUIRE(textArea->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(textArea->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(textArea->getFullSize() == textArea->getSize());
        REQUIRE(textArea->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(textArea->getText() == "");

        textArea->setText("Hello");
        REQUIRE(textArea->getText() == "Hello");

        textArea->setText("World");
        REQUIRE(textArea->getText() == "World");

        textArea->addText("\n\tText");
        REQUIRE(textArea->getText() == "World\n\tText");

        // Carriage returns are removed from the text
        textArea->setText("A\r\nB\n\r\n\nC\r\rD");
        REQUIRE(textArea->getText() == U"A\nB\n\n\nCD");
    }

    SECTION("DefaultText")
    {
        REQUIRE(textArea->getDefaultText() == "");
        textArea->setDefaultText("SomeDefaultText");
        REQUIRE(textArea->getDefaultText() == "SomeDefaultText");
    }

    SECTION("Selected text")
    {
        textArea->setText("SomeText");
        REQUIRE(textArea->getSelectedText() == "");
        REQUIRE(textArea->getSelectionStart() == textArea->getSelectionEnd());

        textArea->setSelectedText(3, 6);
        REQUIRE(textArea->getSelectedText() == "eTe");
        REQUIRE(textArea->getSelectionStart() == 3);
        REQUIRE(textArea->getSelectionEnd() == 6);

        textArea->setSelectedText(7, 1);
        REQUIRE(textArea->getSelectedText() == "omeTex");
        REQUIRE(textArea->getSelectionStart() == 7);
        REQUIRE(textArea->getSelectionEnd() == 1);

        textArea->setSelectedText(2, 2);
        REQUIRE(textArea->getSelectedText() == "");
        REQUIRE(textArea->getSelectionStart() == 2);
        REQUIRE(textArea->getSelectionEnd() == 2);

        textArea->setSelectedText(6, 10);
        REQUIRE(textArea->getSelectedText() == "xt");
        REQUIRE(textArea->getSelectionStart() == 6);
        REQUIRE(textArea->getSelectionEnd() == 8);

        textArea->setSelectedText(10, 20);
        REQUIRE(textArea->getSelectedText() == "");
        REQUIRE(textArea->getSelectionStart() == 8);
        REQUIRE(textArea->getSelectionEnd() == 8);
    }

    SECTION("TextSize")
    {
        textArea->setTextSize(50);
        REQUIRE(textArea->getTextSize() == 50);
    }

    SECTION("MaximumCharacters")
    {
        SECTION("Limit is set")
        {
            textArea->setMaximumCharacters(4);
            REQUIRE(textArea->getMaximumCharacters() == 4);
        }

        SECTION("Existing text is cut")
        {
            textArea->setText("Hello World!");
            textArea->setMaximumCharacters(4);
            REQUIRE(textArea->getText() == "Hell");
        }

        SECTION("Added text is cut")
        {
            textArea->setMaximumCharacters(7);
            textArea->setText("Hello World!");
            REQUIRE(textArea->getText() == "Hello W");

            textArea->addText("\nMore Text.");
            REQUIRE(textArea->getText() == "Hello W");

            textArea->setText("Different Text");
            REQUIRE(textArea->getText() == "Differe");
        }
    }

    SECTION("ReadOnly")
    {
        SECTION("Changing the read-only state")
        {
            REQUIRE(textArea->isReadOnly() == false);

            textArea->setReadOnly(true);
            REQUIRE(textArea->isReadOnly() == true);

            textArea->setReadOnly(false);
            REQUIRE(textArea->isReadOnly() == false);
        }

        SECTION("Read-only does not affect setText calls")
        {
            textArea->setText("Hello");
            textArea->setReadOnly(true);
            textArea->setText("World");
            REQUIRE(textArea->getText() == "World");
        }

        SECTION("You can't type when the text box is read-only")
        {
            textArea->setReadOnly(true);
            textArea->textEntered('x');
            REQUIRE(textArea->getText() == "");

            textArea->setReadOnly(false);
            textArea->textEntered('x');
            REQUIRE(textArea->getText() == "x");
        }
    }

    SECTION("VerticalScrollbarPolicy")
    {
        REQUIRE(textArea->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(textArea->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(textArea->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(textArea->getVerticalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
    }

    SECTION("HorizontalScrollbarPolicy")
    {
        REQUIRE(textArea->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
        textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(textArea->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(textArea->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(textArea->getHorizontalScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
    }

    SECTION("CaretPosition")
    {
        REQUIRE(textArea->getCaretPosition() == 0);

        textArea->setText("Some\ntext");
        REQUIRE(textArea->getCaretPosition() == 9);

        textArea->setCaretPosition(7);
        REQUIRE(textArea->getCaretPosition() == 7);
        REQUIRE(textArea->getSelectionStart() == 7);
        REQUIRE(textArea->getSelectionEnd() == 7);

        textArea->setCaretPosition(25);
        REQUIRE(textArea->getCaretPosition() == 9);

        textArea->setSize({50, 50});
        textArea->setText("A long text that wouldn't fit on a single line.\nFollowed by more\nand more...");

        textArea->setCaretPosition(0);
        REQUIRE(textArea->getCaretLine() == 1);
        REQUIRE(textArea->getCaretColumn() == 1);

        textArea->setCaretPosition(10);
        REQUIRE(textArea->getCaretLine() == 1);
        REQUIRE(textArea->getCaretColumn() == 11);

        textArea->setCaretPosition(60);
        REQUIRE(textArea->getCaretLine() == 2);
        REQUIRE(textArea->getCaretColumn() == 13);

        textArea->setCaretPosition(70);
        REQUIRE(textArea->getCaretLine() == 3);
        REQUIRE(textArea->getCaretColumn() == 6);

        textArea->setCaretPosition(100);
        REQUIRE(textArea->getCaretLine() == 3);
        REQUIRE(textArea->getCaretColumn() == 12);

        textArea->setSelectedText(6, 2);
        REQUIRE(textArea->getCaretPosition() == 2);
        REQUIRE(textArea->getCaretLine() == 1);
        REQUIRE(textArea->getCaretColumn() == 3);
    }

    SECTION("LinesCount")
    {
        REQUIRE(textArea->getLinesCount() == 1);

        textArea->setText("Some text");
        REQUIRE(textArea->getLinesCount() == 1);

        textArea->setText("More\nthan\none\nline");
        REQUIRE(textArea->getLinesCount() == 4);
    }

    SECTION("TabString")
    {
        REQUIRE(textArea->getTabString() == "\t");

        textArea->setTabString("  ");
        REQUIRE(textArea->getTabString() == "  ");

        textArea->setTabString("\r\n");
        REQUIRE(textArea->getTabString() == "\n");

        textArea->setTabString("\r");
        REQUIRE(textArea->getTabString() == "");

        textArea->setTabString("a\rb\rc\u20AC\r");
        REQUIRE(textArea->getTabString() == "abc\u20AC");
    }

    SECTION("Scrollbar access")
    {
        textArea->setSize(200, 100);
        textArea->setTextSize(18);
        textArea->setText("Cupcake ipsum dolor sit amet cotton candy. Ice cream chocolate cake soufflé lollipop marzipan sesame snaps. Marzipan bear claw halvah tiramisu pie.");
        testScrollbarAccess(textArea->getVerticalScrollbar());

        REQUIRE(textArea->getHorizontalScrollbar()->getPolicy() == tgui::Scrollbar::Policy::Never);
        textArea->getHorizontalScrollbar()->setPolicy(tgui::Scrollbar::Policy::Automatic);
        textArea->setText(textArea->getText().replace(".", ".\n"));
        testScrollbarAccess(textArea->getHorizontalScrollbar());
    }

    SECTION("Events / Signals")
    {
        textArea->setSize(165, 100);
        textArea->setTextSize(24);

        SECTION("Widget")
        {
            testWidgetSignals(textArea);
        }

        SECTION("KeyPressed")
        {
            textArea->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

            SECTION("Selecting text")
            {
                REQUIRE(textArea->getSelectedText() == "");

                // Move the caret
                for (unsigned int i = 0; i < 5; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Up, false, false));
                for (unsigned int i = 0; i < 3; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, false));
                for (unsigned int i = 0; i < 1; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Down, false, false));

                REQUIRE(textArea->getSelectedText() == "");

                // Move the caret while the shift key is pressed (text will be selected)
                for (unsigned int i = 0; i < 2; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, true));
                for (unsigned int i = 0; i < 1; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Down, false, true));
                for (unsigned int i = 0; i < 3; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, true));
                for (unsigned int i = 0; i < 2; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Up, false, true));

                REQUIRE(textArea->getSelectedText() == "GHIJKLMNO");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::PageUp, false, true));
                REQUIRE(textArea->getSelectedText() == "ABCDEFGHIJKLMNO");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::PageDown, false, true));
                REQUIRE(textArea->getSelectedText() == "PQRSTUVWXYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::PageUp, false, false));
                REQUIRE(textArea->getSelectedText() == "");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::PageDown, false, false));
                REQUIRE(textArea->getSelectedText() == "");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Home, false, true));
                REQUIRE(textArea->getSelectedText() == "TUVWX");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::End, false, true));
                REQUIRE(textArea->getSelectedText() == "YZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Home, false, false));
                REQUIRE(textArea->getSelectedText() == "");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::End, false, false));
                REQUIRE(textArea->getSelectedText() == "");

                // CTRL+A selects the whole text
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::A, true, false));
                REQUIRE(textArea->getSelectedText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            }

            SECTION("Editing text")
            {
                for (unsigned int i = 0; i < 4; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Backspace, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Backspace, false, false));
                REQUIRE(textArea->getText() == "ABCDEFGHIJKLMNOPQRSTWXYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Delete, false, false));
                REQUIRE(textArea->getText() == "ABCDEFGHIJKLMNOPQRSTXYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Up, false, false));
                for (unsigned int i = 0; i < 3; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, true));

                REQUIRE(textArea->getSelectedText() == "LMN");
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Backspace, false, false));
                REQUIRE(textArea->getText() == "ABCDEFGHIJKOPQRSTXYZ");
                REQUIRE(textArea->getSelectedText() == "");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Up, false, false));
                for (unsigned int i = 0; i < 3; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, true));

                REQUIRE(textArea->getSelectedText() == "BCD");
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Delete, false, false));
                REQUIRE(textArea->getText() == "AEFGHIJKOPQRSTXYZ");
                REQUIRE(textArea->getSelectedText() == "");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Enter, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Enter, false, false));
                REQUIRE(textArea->getText() == "A\n\nEFGHIJKOPQRSTXYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Backspace, false, false));
                REQUIRE(textArea->getText() == "A\nEFGHIJKOPQRSTXYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Down, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Backspace, false, false));
                REQUIRE(textArea->getText() == "A\nEFGHIJKOPRSTXYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Enter, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Backspace, false, false));
                REQUIRE(textArea->getText() == "A\nEFGHIJKOP\n\nSTXYZ");

                // The caret should not move when adding a newline at the start of a line that only exists due to word-wrap
                textArea->setText(""); // Make sure the scrollbar is gone
                textArea->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
                for (unsigned int i = 0; i < 17; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Enter, false, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Delete, false, false));
                REQUIRE(textArea->getText() == "ABCDEFGHIJ\nLMNOPQRSTUVWXYZ");
            }

            SECTION("Copy and Paste")
            {
                for (unsigned int i = 0; i < 3; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, true));

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::C, true, false));
                REQUIRE(getClipboardContents() == "XYZ");

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Left, false, false)); // Deselect text
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::V, true, false));
                REQUIRE(textArea->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZXYZ");
                REQUIRE(getClipboardContents() == "XYZ");

                // Select some text near the beginning, because if we select text near then end then
                // it currently depends on the backend which letter is on which line.
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Home, true, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, false));
                for (unsigned int i = 0; i < 2; ++i)
                    textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::Right, false, true));

                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::X, true, false));
                REQUIRE(textArea->getText() == "ADEFGHIJKLMNOPQRSTUVWXYZXYZ");
                REQUIRE(getClipboardContents() == "BC");

                textArea->setReadOnly(true);
                textArea->setText("12345");

                // Pasting to read-only text area does nothing
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::V, true, false));
                REQUIRE(textArea->getText() == "12345");

                // Cutting from read-only text area just copies the selection
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::A, true, false));
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::X, true, false));
                REQUIRE(textArea->getText() == "12345");
                REQUIRE(getClipboardContents() == "12345");

                // Pasting too much text into a text area with a character limit will truncate the pasted text
                textArea->setReadOnly(false);
                textArea->setCaretPosition(0);
                textArea->setMaximumCharacters(7);
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::V, true, false));
                REQUIRE(textArea->getText() == "1212345");
                textArea->keyPressed(createKeyEvent(tgui::Event::KeyboardKey::V, true, false));
                REQUIRE(textArea->getText() == "1212345");
            }

            SECTION("Pressing tab")
            {
                auto sendTabEventToGui = [](GuiNull& gui) {
                    tgui::Event event;
                    event.key = createKeyEvent(tgui::Event::KeyboardKey::Tab, false, false);
                    event.type = tgui::Event::Type::KeyPressed;
                    gui.handleEvent(event);
                };

                GuiNull gui;
                gui.add(textArea);

                textArea->setText("");
                textArea->setFocused(true);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "");

                gui.setTabKeyUsageEnabled(false);
                textArea->setText("");
                textArea->setFocused(true);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "\t");

                gui.setTabKeyUsageEnabled(true);
                textArea->setText("");
                textArea->setFocused(true);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "");

                gui.setTabKeyUsageEnabled(false);
                textArea->setText("");
                textArea->setFocused(true);
                textArea->setTabString("   ");
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "   ");

                textArea->setText("");
                textArea->setTabString("\n\n\n\n\n");
                textArea->setMaximumCharacters(5);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "\n\n\n\n\n");
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "\n\n\n\n\n");

                textArea->setText("");
                textArea->setTabString("abc");
                textArea->setMaximumCharacters(5);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "abc");
                textArea->setCaretPosition(1);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "aabbc");

                textArea->setText("");
                textArea->setTabString("");
                textArea->setMaximumCharacters(0);
                sendTabEventToGui(gui);
                REQUIRE(textArea->getText() == "");
            }
        }

        SECTION("TextEntered")
        {
            textArea->setText("ABC");
            textArea->setMaximumCharacters(4);

            textArea->textEntered('D');
            REQUIRE(textArea->getText() == "ABCD");

            textArea->textEntered('E');
            REQUIRE(textArea->getText() == "ABCD");

            textArea->setMaximumCharacters(0);
            textArea->textEntered('E');
            REQUIRE(textArea->getText() == "ABCDE");

            textArea->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);

            textArea->textEntered('A');
            textArea->textEntered('B');
            REQUIRE(textArea->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZAB");

            textArea->textEntered('C');
            REQUIRE(textArea->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZAB");

            textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
            textArea->textEntered('C');
            REQUIRE(textArea->getText() == "ABCDEFGHIJKLMNOPQRSTUVWXYZABC");
        }

        // TODO: TextChanged and SelectionChanged events
    }

    testWidgetRenderer(textArea->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = textArea->getRenderer();

        SECTION("colored")
        {
            tgui::ScrollbarRenderer scrollbarRenderer;
            scrollbarRenderer.setTrackColor(tgui::Color::Red);
            scrollbarRenderer.setThumbColor(tgui::Color::Blue);

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
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{160, 170, 180}));
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

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextBackgroundColor").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("CaretColor").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("CaretWidth").getNumber() == 2);
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["TrackColor"].getColor() == tgui::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["ThumbColor"].getColor() == tgui::Color::Blue);
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
        textArea->setText("This is the text in the text box!");
        textArea->setTextSize(25);
        textArea->setMaximumCharacters(16);
        textArea->setReadOnly(true);
        textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);

        testSavingWidget("TextArea", textArea);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(270, 160, textArea)

        textArea->setEnabled(true);
        textArea->setFocused(true);
        textArea->setPosition(10, 5);
        textArea->setSize(250, 150);
        textArea->setText("Something");
        textArea->setTextSize(16);
        textArea->setText("Cupcake ipsum dolor sit.\nAmet candy canes sesame snaps cupcake apple pie lemon drops jelly apple pie candy canes.\nPowder donut sugar plum croissant gingerbread sesame snaps pie.\nCaramels cupcake icing.\nBear claw lemon drops carrot cake chocolate ice cream jelly beans gummi bears.\nMuffin ice cream cupcake chocolate cake.\nTopping oat cake bonbon.");

        tgui::TextAreaRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Yellow);
        renderer.setTextColor(tgui::Color::Blue);
        renderer.setSelectedTextColor(tgui::Color::Red);
        renderer.setSelectedTextBackgroundColor(tgui::Color::Cyan);
        renderer.setCaretColor(tgui::Color::Green);
        renderer.setBorderColor(tgui::Color::Magenta);
        renderer.setCaretWidth(2);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setOpacity(0.7f);
        textArea->setRenderer(renderer.getData());

        tgui::ScrollbarRenderer scrollbarRenderer = tgui::RendererData::create();
        scrollbarRenderer.setTrackColor(tgui::Color::Green);
        scrollbarRenderer.setThumbColor(tgui::Color::Red);
        scrollbarRenderer.setArrowBackgroundColor(tgui::Color::Blue);
        scrollbarRenderer.setArrowColor(tgui::Color::White);
        scrollbarRenderer.setOpacity(0.7f);
        renderer.setScrollbar(scrollbarRenderer.getData());

        textArea->leftMousePressed({87, 66});
        textArea->mouseMoved({136, 105});
        textArea->leftMouseReleased({136, 105});

        SECTION("Colored")
        {
            TEST_DRAW("TextArea.png")
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            TEST_DRAW("TextArea_Textured.png")
        }

        SECTION("Scrollbars")
        {
            SECTION("No scrollbars")
            {
                textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                TEST_DRAW("TextArea_NoScrollbars.png")
            }

            SECTION("Vertical scrollbar")
            {
                textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                TEST_DRAW("TextArea_VerticalScrollbar.png")
            }

            SECTION("Horizontal scrollbar")
            {
                textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
                textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                TEST_DRAW("TextArea_HorizontalScrollbar.png")
            }

            SECTION("Both scrollbars")
            {
                textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
                TEST_DRAW("TextArea_BothScrollbars.png")
            }

            SECTION("Forced scrollbars")
            {
                textArea->setText("Test");
                textArea->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
                textArea->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Always);
                TEST_DRAW("TextArea_ForcedScrollbars.png")
            }
        }
    }

    SECTION("Bug Fixes")
    {
        SECTION("ctrl+alt+A should not act as ctrl+A (https://github.com/texus/TGUI/issues/43)")
        {
            auto event = createKeyEvent(tgui::Event::KeyboardKey::A, true, false);

            textArea->setText("Test");
            textArea->keyPressed(event);
            REQUIRE(textArea->getSelectedText() == "Test");

            textArea->setText("Test");
            REQUIRE(textArea->getSelectedText() == "");

            // ctrl+alt+A must not function as ctrl+A
            event.alt = true;
            textArea->keyPressed(event);
            REQUIRE(textArea->getSelectedText() == "");
        }
    }
}
