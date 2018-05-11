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
#include <TGUI/Widgets/EditBox.hpp>

TEST_CASE("[EditBox]")
{
    tgui::EditBox::Ptr editBox = tgui::EditBox::create();
    editBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](){}));
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](sf::String){}));
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](std::string){}));
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(editBox->connect("TextChanged", [](tgui::Widget::Ptr, std::string, std::string){}));

        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](sf::String){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](std::string){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(editBox->connect("ReturnKeyPressed", [](tgui::Widget::Ptr, std::string, std::string){}));
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

        REQUIRE(editBox->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(editBox->getSize() == sf::Vector2f(150, 100));
        REQUIRE(editBox->getFullSize() == editBox->getSize());
        REQUIRE(editBox->getWidgetOffset() == sf::Vector2f(0, 0));
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
        REQUIRE(editBox->getPasswordCharacter() == '\0');
        editBox->setPasswordCharacter('*');
        REQUIRE(editBox->getPasswordCharacter() == '*');
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
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Left);
        
        editBox->setAlignment(tgui::EditBox::Alignment::Center);
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Center);
        
        editBox->setAlignment(tgui::EditBox::Alignment::Right);
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Right);
        
        editBox->setAlignment(tgui::EditBox::Alignment::Left);
        REQUIRE(editBox->getAlignment() == tgui::EditBox::Alignment::Left);
    }

    SECTION("LimitTextWidth")
    {
        editBox->setTextSize(20);
        editBox->setSize(100, 25);
        editBox->setText("too long text");

        REQUIRE(!editBox->isTextWidthLimited());
        editBox->limitTextWidth(true);
        REQUIRE(editBox->isTextWidthLimited());
        REQUIRE(editBox->getText() == "too long ");

        editBox->setText("some other text");
        REQUIRE(editBox->getText() == "some ot");

        editBox->limitTextWidth(false);
        REQUIRE(!editBox->isTextWidthLimited());

        editBox->setText("yet another text");
        REQUIRE(editBox->getText() == "yet another text");
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

    SECTION("Input Validator")
    {
        editBox->setText(L"++Some123 Ê Text456--");

        SECTION("Default")
        {
            REQUIRE(editBox->getInputValidator() == ".*");
            REQUIRE(editBox->getText() == L"++Some123 Ê Text456--");

            editBox->setInputValidator(".*");
            REQUIRE(editBox->getText() == L"++Some123 Ê Text456--");

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
            editBox->setInputValidator(tgui::EditBox::Validator::Int);
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
            editBox->setInputValidator(tgui::EditBox::Validator::UInt);
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
            editBox->setInputValidator(tgui::EditBox::Validator::Float);
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
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(editBox);
        }

        editBox->setPosition(40, 30);
        editBox->setSize(150, 100);

        sf::Event::KeyEvent keyEvent;
        keyEvent.alt = false;
        keyEvent.control = false;
        keyEvent.shift = false;
        keyEvent.system = false;

        // TODO: Actually test the events, not just the signals that fire from them

        SECTION("TextChanged signal")
        {
            unsigned int textChangedCount = 0;
            editBox->connect("TextChanged", &genericCallback, std::ref(textChangedCount));

            editBox->textEntered('a');
            REQUIRE(textChangedCount == 1);
            editBox->textEntered('b');
            editBox->textEntered('c');
            editBox->textEntered('d');
            REQUIRE(textChangedCount == 4);

            keyEvent.code = sf::Keyboard::Left;
            editBox->keyPressed(keyEvent);
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 4);

            keyEvent.code = sf::Keyboard::BackSpace;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 5);

            keyEvent.code = sf::Keyboard::Delete;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 6);

            editBox->selectText();
            keyEvent.control = true;

            keyEvent.code = sf::Keyboard::C;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 6);

            keyEvent.code = sf::Keyboard::X;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 7);

            keyEvent.code = sf::Keyboard::V;
            editBox->keyPressed(keyEvent);
            REQUIRE(textChangedCount == 8);
        }

        SECTION("ReturnKeyPressed signal")
        {
            unsigned int count = 0;
            std::string expectedText = "";
            editBox->connect("ReturnKeyPressed", [&](sf::String text){ REQUIRE(text == expectedText); count++; });

            keyEvent.code = sf::Keyboard::Return;
            editBox->keyPressed(keyEvent);
            REQUIRE(count == 1);

            editBox->textEntered('a');
            editBox->textEntered('b');
            editBox->textEntered('c');
            editBox->textEntered('d');
            expectedText = "abcd";

            keyEvent.code = sf::Keyboard::Return;
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
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColorHover", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", "rgb(150, 160, 170)"));
                REQUIRE_NOTHROW(renderer->setProperty("CaretWidth", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextStyle", "Bold | Underlined"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", sf::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextBackgroundColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColor", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretColorHover", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", sf::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", sf::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("CaretWidth", 2));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", sf::Text::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextStyle", tgui::TextStyle(sf::Text::Bold | sf::Text::Underlined)));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions")
            {
                renderer->setTextColor({20, 30, 40});
                renderer->setTextColorDisabled({120, 130, 140});
                renderer->setSelectedTextColor({30, 40, 50});
                renderer->setSelectedTextBackgroundColor({40, 50, 60});
                renderer->setDefaultTextColor({50, 60, 70});
                renderer->setCaretColor({60, 70, 80});
                renderer->setCaretColorHover({110, 120, 130});
                renderer->setBackgroundColor({70, 80, 90});
                renderer->setBackgroundColorHover({80, 90, 100});
                renderer->setBackgroundColorDisabled({140, 150, 160});
                renderer->setBorderColor({90, 100, 110});
                renderer->setBorderColorHover({100, 110, 120});
                renderer->setBorderColorDisabled({150, 160, 170});
                renderer->setCaretWidth(2);
                renderer->setTextStyle(sf::Text::Italic);
                renderer->setDefaultTextStyle(sf::Text::Bold | sf::Text::Underlined);
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("SelectedTextBackgroundColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("DefaultTextColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("CaretColor").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("CaretColorHover").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == sf::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == sf::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("CaretWidth").getNumber() == 2);
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getProperty("DefaultTextStyle").getTextStyle() == (sf::Text::Bold | sf::Text::Underlined));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));

            REQUIRE(renderer->getTextColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getTextColorDisabled() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getSelectedTextColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getSelectedTextBackgroundColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getDefaultTextColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getCaretColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getCaretColorHover() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getBackgroundColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorHover() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getBackgroundColorDisabled() == sf::Color(140, 150, 160));
            REQUIRE(renderer->getBorderColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getBorderColorHover() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getBorderColorDisabled() == sf::Color(150, 160, 170));
            REQUIRE(renderer->getCaretWidth() == 2);
            REQUIRE(renderer->getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getDefaultTextStyle() == (sf::Text::Bold | sf::Text::Underlined));
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
        editBox->setAlignment(tgui::EditBox::Alignment::Right);
        editBox->limitTextWidth();
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
        renderer.setTextColor(sf::Color::Blue);
        renderer.setSelectedTextColor(sf::Color::White);
        renderer.setSelectedTextBackgroundColor(sf::Color::Green);
        renderer.setBackgroundColor(sf::Color::Yellow);
        renderer.setCaretColor(sf::Color::Cyan);
        renderer.setBorderColor(sf::Color::Red);
        renderer.setTextStyle(sf::Text::Style::Underlined);
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

            renderer.setDefaultTextColor(sf::Color::Magenta);
            renderer.setDefaultTextStyle(sf::Text::Style::Italic);

            TEST_DRAW("EditBox_DefaultText.png")
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
                editBox->setAlignment(tgui::EditBox::Alignment::Left);
                TEST_DRAW("EditBox_Alignment_Left.png")
            }

            SECTION("Center")
            {
                editBox->setAlignment(tgui::EditBox::Alignment::Center);
                TEST_DRAW("EditBox_Alignment_Center.png")
            }

            SECTION("Right")
            {
                editBox->setAlignment(tgui::EditBox::Alignment::Right);
                TEST_DRAW("EditBox_Alignment_Right.png")
            }
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
