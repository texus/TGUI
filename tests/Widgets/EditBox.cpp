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

#include <TGUI/Config.hpp>
#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/extlibs/IncludeWindows.hpp>
#endif

#include "Tests.hpp"

TEST_CASE("[EditBox]")
{
    tgui::EditBox::Ptr editBox = tgui::EditBox::create();
    editBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        editBox->onTextChange([](){});
        editBox->onTextChange([](const tgui::String&){});

        editBox->onReturnKeyPress([](){});
        editBox->onReturnKeyPress([](const tgui::String&){});

        editBox->onReturnOrUnfocus([](){});
        editBox->onReturnOrUnfocus([](const tgui::String&){});

        editBox->onCaretPositionChange([](){});
        editBox->onCaretPositionChange([](std::size_t){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(editBox)->getSignal("TextChanged").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(editBox)->getSignal("ReturnKeyPressed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(editBox)->getSignal("ReturnOrUnfocused").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(editBox)->getSignal("CaretPositionChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(editBox->getWidgetType() == "EditBox");
    }

    SECTION("Position and Size")
    {
        editBox->setPosition(40, 30);
        editBox->setSize(150, 100);
        editBox->getRenderer()->setBorders(2);

        REQUIRE(editBox->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(editBox->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(editBox->getFullSize() == editBox->getSize());
        REQUIRE(editBox->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(editBox->getText() == "");
        editBox->setText("SomeText");
        REQUIRE(editBox->getText() == "SomeText");
    }
    
    SECTION("DefaultText")
    {
        REQUIRE(editBox->getDefaultText() == "");
        editBox->setDefaultText("SomeDefaultText");
        REQUIRE(editBox->getDefaultText() == "SomeDefaultText");
    }

    SECTION("TextSize")
    {
        editBox->setTextSize(25);
        REQUIRE(editBox->getTextSize() == 25);
    }

    SECTION("PasswordCharacter")
    {
        REQUIRE(editBox->getPasswordCharacter() == U'\0');
        editBox->setPasswordCharacter(U'*');
        REQUIRE(editBox->getPasswordCharacter() == U'*');
        editBox->setPasswordCharacter(U'\x263a');
        REQUIRE(editBox->getPasswordCharacter() == U'\x263a');
    }

    SECTION("MaximumCharacters")
    {
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

    SECTION("Alignment")
    {
        REQUIRE(editBox->getAlignment() == tgui::HorizontalAlignment::Left);
        
        editBox->setAlignment(tgui::HorizontalAlignment::Center);
        REQUIRE(editBox->getAlignment() == tgui::HorizontalAlignment::Center);
        
        editBox->setAlignment(tgui::HorizontalAlignment::Right);
        REQUIRE(editBox->getAlignment() == tgui::HorizontalAlignment::Right);
        
        editBox->setAlignment(tgui::HorizontalAlignment::Left);
        REQUIRE(editBox->getAlignment() == tgui::HorizontalAlignment::Left);
    }

    SECTION("TextWidthLimited")
    {
        editBox->setTextSize(20);
        editBox->setSize(100, 25);
        editBox->setText("too long text");

        REQUIRE(!editBox->isTextWidthLimited());
        editBox->setTextWidthLimited(true);
        REQUIRE(editBox->isTextWidthLimited());
        REQUIRE(editBox->getText() == "too long ");

        // Currently the exact width of the text depends on the backend so not every string can be
        // tested here. The chosen string should lead to same behavior on different SFML versions.
        editBox->setText("yet another text");
        REQUIRE(editBox->getText() == "yet anot");

        editBox->setTextWidthLimited(false);
        REQUIRE(!editBox->isTextWidthLimited());

        editBox->setText("some other text");
        REQUIRE(editBox->getText() == "some other text");
    }

    SECTION("ReadOnly")
    {
        REQUIRE(!editBox->isReadOnly());

        editBox->setReadOnly(true);
        REQUIRE(editBox->isReadOnly());

        // The text can still be changed using the setText function
        editBox->setText("Test");
        REQUIRE(editBox->getText() == "Test");
        REQUIRE(editBox->isReadOnly());

        editBox->setReadOnly(false);
        REQUIRE(!editBox->isReadOnly());
    }

    SECTION("Suffix")
    {
        REQUIRE(editBox->getSuffix().empty());

        editBox->setSuffix("Tn");
        REQUIRE(editBox->getSuffix() == "Tn");
    }

    SECTION("Input Validator")
    {
        editBox->setText(L"++Some123 \u00CA Text456--");

        SECTION("Default")
        {
            REQUIRE(editBox->getInputValidator() == tgui::EditBox::Validator::All);
            REQUIRE(editBox->getText() == L"++Some123 \u00CA Text456--");

            REQUIRE(editBox->setInputValidator(tgui::EditBox::Validator::All));
            REQUIRE(editBox->getText() == L"++Some123 \u00CA Text456--");

            SECTION("Adding characters")
            {
                editBox->setText("Hello");

                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "Hello-");

                editBox->textEntered('0');
                REQUIRE(editBox->getText() == "Hello-0");

                editBox->textEntered('x');
                REQUIRE(editBox->getText() == "Hello-0x");

                editBox->setText("");
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "-");
            }
        }

        SECTION("Int")
        {
            REQUIRE(editBox->setInputValidator(tgui::EditBox::Validator::Int));
            REQUIRE(editBox->getInputValidator() == tgui::EditBox::Validator::Int);
            REQUIRE(editBox->getText() == "");

            editBox->setText("-5");
            REQUIRE(editBox->getText() == "-5");

            editBox->setText("642");
            REQUIRE(editBox->getText() == "642");

            editBox->setText("1.5");
            REQUIRE(editBox->getText() == "");

            editBox->setText("text");
            REQUIRE(editBox->getText() == "");

            editBox->setInputValidator(".*");
            editBox->setText("+25");
            editBox->setInputValidator(tgui::EditBox::Validator::Int);
            REQUIRE(editBox->getText() == "+25");

            SECTION("Adding characters")
            {
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "+25");

                editBox->textEntered('0');
                REQUIRE(editBox->getText() == "+250");

                editBox->textEntered('x');
                REQUIRE(editBox->getText() == "+250");

                editBox->setText("");
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "-");
            }
        }

        SECTION("UInt")
        {
            REQUIRE(editBox->setInputValidator(tgui::EditBox::Validator::UInt));
            REQUIRE(editBox->getInputValidator() == tgui::EditBox::Validator::UInt);
            REQUIRE(editBox->getText() == "");

            editBox->setText("-5");
            REQUIRE(editBox->getText() == "");

            editBox->setText("642");
            REQUIRE(editBox->getText() == "642");

            editBox->setText("1.5");
            REQUIRE(editBox->getText() == "");

            editBox->setText("text");
            REQUIRE(editBox->getText() == "");

            editBox->setInputValidator(".*");
            editBox->setText("10");
            editBox->setInputValidator(tgui::EditBox::Validator::UInt);
            REQUIRE(editBox->getText() == "10");

            SECTION("Adding characters")
            {
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "10");

                editBox->textEntered('9');
                REQUIRE(editBox->getText() == "109");

                editBox->textEntered('x');
                REQUIRE(editBox->getText() == "109");

                editBox->setText("");
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "");
            }
        }

        SECTION("Float")
        {
            REQUIRE(editBox->setInputValidator(tgui::EditBox::Validator::Float));
            REQUIRE(editBox->getInputValidator() == tgui::EditBox::Validator::Float);
            REQUIRE(editBox->getText() == "");

            editBox->setText("-5");
            REQUIRE(editBox->getText() == "-5");

            editBox->setText("642");
            REQUIRE(editBox->getText() == "642");

            editBox->setText("1.5");
            REQUIRE(editBox->getText() == "1.5");

            editBox->setText("-6.");
            REQUIRE(editBox->getText() == "-6.");

            editBox->setText(".001");
            REQUIRE(editBox->getText() == ".001");

            editBox->setText("text");
            REQUIRE(editBox->getText() == "");

            editBox->setInputValidator(".*");
            editBox->setText("-2.5");
            editBox->setInputValidator(tgui::EditBox::Validator::Float);
            REQUIRE(editBox->getText() == "-2.5");

            SECTION("Adding characters")
            {
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "-2.5");

                editBox->textEntered('0');
                REQUIRE(editBox->getText() == "-2.50");

                editBox->textEntered('x');
                REQUIRE(editBox->getText() == "-2.50");

                editBox->textEntered('.');
                REQUIRE(editBox->getText() == "-2.50");

                editBox->setText("");
                editBox->textEntered('-');
                REQUIRE(editBox->getText() == "-");
                editBox->textEntered('.');
                REQUIRE(editBox->getText() == "-.");
            }
        }

        SECTION("Invalid expressions")
        {
            REQUIRE(editBox->setInputValidator("abc[0-9]xyz"));
            REQUIRE(!editBox->setInputValidator("abc[0-"));
            REQUIRE(editBox->getInputValidator() == "abc[0-9]xyz");
        }
    }

    SECTION("Blink rate")
    {
        REQUIRE(tgui::getEditCursorBlinkRate() == std::chrono::milliseconds(500));
        tgui::setEditCursorBlinkRate(std::chrono::milliseconds(100));
        REQUIRE(tgui::getEditCursorBlinkRate() == std::chrono::milliseconds(100));
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(editBox);
        }

        editBox->setPosition(40, 30);
        editBox->setSize(150, 100);

        tgui::Event::KeyEvent keyEvent;
        keyEvent.alt = false;
        keyEvent.control = false;
        keyEvent.shift = false;
        keyEvent.system = false;

        // TODO: Actually test the events, not just the signals that fire from them

        SECTION("TextChanged signal")
        {
            unsigned int textChangedCount = 0;
            editBox->onTextChange(&genericCallback, std::ref(textChangedCount));

            editBox->textEntered('a');
            REQUIRE(textChangedCount == 1);
            editBox->textEntered('b');
            editBox->textEntered('c');
            editBox->textEntered('d');
            REQUIRE(textChangedCount == 4);

            keyEvent.code = tgui::Event::KeyboardKey::Left;
            editBox->keyPressed(keyEvent);
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 4);

            keyEvent.code = tgui::Event::KeyboardKey::Backspace;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 5);

            keyEvent.code = tgui::Event::KeyboardKey::Delete;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 6);

            editBox->selectText();

#ifdef TGUI_SYSTEM_MACOS
            keyEvent.system = true;
#else
            keyEvent.control = true;
#endif

            keyEvent.code = tgui::Event::KeyboardKey::C;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 6);

            keyEvent.code = tgui::Event::KeyboardKey::X;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 7);

#ifdef TGUI_SYSTEM_WINDOWS
            Sleep(1); // SDL on Windows seems to require some time to set the clipboard reliably
#endif
            keyEvent.code = tgui::Event::KeyboardKey::V;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 8);
        }

        SECTION("ReturnKeyPressed signal")
        {
            unsigned int count = 0;
            tgui::String expectedText = "";
            editBox->onReturnKeyPress([&](const tgui::String& text){ REQUIRE(text == expectedText); count++; });

            keyEvent.code = tgui::Event::KeyboardKey::Enter;
            editBox->keyPressed(keyEvent);
            REQUIRE(count == 1);

            editBox->textEntered('a');
            editBox->textEntered('b');
            editBox->textEntered('c');
            editBox->textEntered('d');
            expectedText = "abcd";

            keyEvent.code = tgui::Event::KeyboardKey::Enter;
            editBox->keyPressed(keyEvent);
            REQUIRE(count == 2);
        }
    }

    testWidgetRenderer(editBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = editBox->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFocused", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColorHover", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorFocused", "rgb(170, 180, 190)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", "rgb(150, 160, 170)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", "rgb(180, 190, 200)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretWidth", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextStyle", "Bold | Underlined"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", tgui::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFocused", tgui::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColorHover", tgui::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", tgui::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorFocused", tgui::Color{170, 180, 190}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", tgui::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", tgui::Color{180, 190, 200}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretWidth", 2));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyle::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextStyle", tgui::TextStyles(tgui::TextStyle::Bold | tgui::TextStyle::Underlined)));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions")
            {
                renderer->setTextColor({20, 30, 40});
                renderer->setTextColorDisabled({120, 130, 140});
                renderer->setTextColorFocused({160, 170, 180});
                renderer->setSelectedTextColor({30, 40, 50});
                renderer->setSelectedTextBackgroundColor({40, 50, 60});
                renderer->setDefaultTextColor({50, 60, 70});
                renderer->setCaretColor({60, 70, 80});
                renderer->setCaretColorHover({110, 120, 130});
                renderer->setBackgroundColor({70, 80, 90});
                renderer->setBackgroundColorHover({80, 90, 100});
                renderer->setBackgroundColorDisabled({140, 150, 160});
                renderer->setBackgroundColorFocused({170, 180, 190});
                renderer->setBorderColor({90, 100, 110});
                renderer->setBorderColorHover({100, 110, 120});
                renderer->setBorderColorDisabled({150, 160, 170});
                renderer->setBorderColorFocused({180, 190, 200});
                renderer->setCaretWidth(2);
                renderer->setTextStyle(tgui::TextStyle::Italic);
                renderer->setDefaultTextStyle(tgui::TextStyle::Bold | tgui::TextStyle::Underlined);
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("TextColorFocused").getColor() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("SelectedTextBackgroundColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("DefaultTextColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("CaretColor").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("CaretColorHover").getColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("BackgroundColorFocused").getColor() == tgui::Color(170, 180, 190));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("BorderColorFocused").getColor() == tgui::Color(180, 190, 200));
            REQUIRE(renderer->getProperty("CaretWidth").getNumber() == 2);
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getProperty("DefaultTextStyle").getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Underlined));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));

            REQUIRE(renderer->getTextColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getTextColorDisabled() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getTextColorFocused() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getSelectedTextColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getSelectedTextBackgroundColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getDefaultTextColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getCaretColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getCaretColorHover() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getBackgroundColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorHover() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getBackgroundColorDisabled() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getBackgroundColorFocused() == tgui::Color(170, 180, 190));
            REQUIRE(renderer->getBorderColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getBorderColorHover() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getBorderColorDisabled() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getBorderColorFocused() == tgui::Color(180, 190, 200));
            REQUIRE(renderer->getCaretWidth() == 2);
            REQUIRE(renderer->getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getDefaultTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Underlined));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getPadding() == tgui::Borders(5, 6, 7, 8));
        }

        SECTION("textured")
        {
            tgui::Texture textureNormal("resources/Texture1.png");
            tgui::Texture textureHover("resources/Texture2.png");
            tgui::Texture textureDisabled("resources/Texture3.png");
            tgui::Texture textureFocused("resources/Texture4.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("Texture", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabled", tgui::Serializer::serialize(textureDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", tgui::Serializer::serialize(textureFocused)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("Texture", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabled", textureDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", textureFocused));
            }

            SECTION("functions")
            {
                renderer->setTexture(textureNormal);
                renderer->setTextureHover(textureHover);
                renderer->setTextureDisabled(textureDisabled);
                renderer->setTextureFocused(textureFocused);
            }

            REQUIRE(renderer->getProperty("Texture").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureFocused").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTexture().getData() == textureNormal.getData());
            REQUIRE(renderer->getTextureHover().getData() == textureHover.getData());
            REQUIRE(renderer->getTextureDisabled().getData() == textureDisabled.getData());
            REQUIRE(renderer->getTextureFocused().getData() == textureFocused.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        editBox->setText("SomeText");
        editBox->setDefaultText("SomeDefaultText");
        editBox->setTextSize(25);
        editBox->setPasswordCharacter('*');
        editBox->setMaximumCharacters(5);
        editBox->setAlignment(tgui::HorizontalAlignment::Right);
        editBox->setTextWidthLimited(true);
        editBox->setReadOnly(true);
        editBox->setInputValidator("[0-9a-zA-Z]*");

        testSavingWidget("EditBox", editBox);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 35, editBox)

        editBox->setEnabled(true);
        editBox->setPosition(10, 5);
        editBox->setSize(100, 25);
        editBox->setText("Something");
        editBox->setTextSize(16);

        tgui::EditBoxRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(tgui::Color::Blue);
        renderer.setSelectedTextColor(tgui::Color::White);
        renderer.setSelectedTextBackgroundColor(tgui::Color::Green);
        renderer.setBackgroundColor(tgui::Color::Yellow);
        renderer.setCaretColor(tgui::Color::Cyan);
        renderer.setBorderColor(tgui::Color::Red);
        renderer.setTextStyle(tgui::TextStyle::Underlined);
        renderer.setCaretWidth(1);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setOpacity(0.7f);
        editBox->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setCaretColorHover({0, 150, 150});
                                        renderer.setBackgroundColorHover({150, 150, 0});
                                        renderer.setBorderColorHover({150, 0, 0});
                                        if (textured)
                                            renderer.setTextureHover("resources/Texture2.png");
                                    };

        auto setDisabledRenderer = [&](bool textured){
                                        renderer.setTextColorDisabled({0, 0, 80});
                                        renderer.setBackgroundColorDisabled({80, 80, 0});
                                        renderer.setBorderColorDisabled({80, 0, 0});
                                        if (textured)
                                            renderer.setTextureDisabled("resources/Texture3.png");
                                    };

        const auto mousePos = editBox->getPosition() + (editBox->getSize() / 2.f);

        editBox->selectText(2, 3);
        editBox->setFocused(true);

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("EditBox_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("EditBox_Normal_HoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                editBox->mouseMoved(mousePos);

                TEST_DRAW("EditBox_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("EditBox_Hover_HoverSet.png")
                }
            }

            SECTION("DisabledState")
            {
                editBox->setEnabled(false);

                TEST_DRAW("EditBox_Disabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(false);
                    TEST_DRAW("EditBox_Disabled_DisabledSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTexture("resources/Texture1.png");

            SECTION("NormalState")
            {
                TEST_DRAW("EditBox_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("EditBox_Normal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                editBox->mouseMoved(mousePos);

                TEST_DRAW("EditBox_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("EditBox_Hover_TextureHoverSet.png")
                }
            }

            SECTION("DisabledState")
            {
                editBox->setEnabled(false);

                TEST_DRAW("EditBox_Disabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(true);
                    TEST_DRAW("EditBox_Disabled_TextureDisabledSet.png")
                }
            }
        }

        SECTION("DefaultText")
        {
            editBox->setText("");
            editBox->setDefaultText("Hello");

            renderer.setDefaultTextColor(tgui::Color::Magenta);
            renderer.setDefaultTextStyle(tgui::TextStyle::Italic);

            TEST_DRAW("EditBox_DefaultText.png")
        }

        SECTION("Selected text")
        {
            editBox->setText("Hello");
            REQUIRE(editBox->getSelectedText() == "");

            editBox->selectText(1, 3);
            REQUIRE(editBox->getSelectedText() == "ell");

            editBox->selectText(3);
            REQUIRE(editBox->getSelectedText() == "lo");

            editBox->selectText(2, 0);
            REQUIRE(editBox->getSelectedText() == "");

            editBox->selectText();
            REQUIRE(editBox->getSelectedText() == "Hello");
        }

        SECTION("PasswordCharacter")
        {
            editBox->setPasswordCharacter('+');
            TEST_DRAW("EditBox_PasswordCharacter.png")
        }

        SECTION("Alignment")
        {
            editBox->setText("Short");
            renderer.setOpacity(1);

            SECTION("Left")
            {
                editBox->setAlignment(tgui::HorizontalAlignment::Left);
                TEST_DRAW("EditBox_Alignment_Left.png")
            }

            SECTION("Center")
            {
                editBox->setAlignment(tgui::HorizontalAlignment::Center);
                TEST_DRAW("EditBox_Alignment_Center.png")
            }

            SECTION("Right")
            {
                editBox->setAlignment(tgui::HorizontalAlignment::Right);
                TEST_DRAW("EditBox_Alignment_Right.png")
            }
        }

        SECTION("Suffix")
        {
            editBox->setTextSize(12);
            editBox->setAlignment(tgui::HorizontalAlignment::Right);
            editBox->setText("50");
            editBox->setSuffix("kg");
            TEST_DRAW("EditBox_Suffix.png")
        }
    }

    SECTION("Bug Fixes")
    {
        SECTION("ctrl+alt+A should not act as ctrl+A (https://github.com/texus/TGUI/issues/43)")
        {
            tgui::Event::KeyEvent event;
            event.code    = tgui::Event::KeyboardKey::A;
            event.alt     = false;
            event.shift   = false;
#ifdef TGUI_SYSTEM_MACOS
            event.control = false;
            event.system  = true;
#else
            event.control = true;
            event.system  = false;
#endif

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
}
