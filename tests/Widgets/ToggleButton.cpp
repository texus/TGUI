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

TEST_CASE("[ToggleButton]")
{
    tgui::ToggleButton::Ptr button = tgui::ToggleButton::create();
    button->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        button->onToggle([](){});
        button->onToggle([](bool){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(button)->getSignal("Toggled").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(button->getWidgetType() == "ToggleButton");
    }

    SECTION("Position and Size")
    {
        button->setPosition(40, 30);
        button->setSize(150, 100);
        button->getRenderer()->setBorders(2);

        REQUIRE(button->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(button->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(button->getFullSize() == button->getSize());
        REQUIRE(button->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(button->getText() == "");
        button->setText("SomeText");
        REQUIRE(button->getText() == "SomeText");
    }

    SECTION("Down")
    {
        REQUIRE(!button->isDown());
        button->setDown(true);
        REQUIRE(button->isDown());
        button->setDown(false);
        REQUIRE(!button->isDown());
    }

    SECTION("TextSize")
    {
        button->setTextSize(25);
        REQUIRE(button->getTextSize() == 25);
    }

    SECTION("IgnoreKeyEvents")
    {
        REQUIRE(!button->getIgnoreKeyEvents());
        button->setIgnoreKeyEvents(true);
        REQUIRE(button->getIgnoreKeyEvents());
        button->setIgnoreKeyEvents(false);
        REQUIRE(!button->getIgnoreKeyEvents());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(button);
        }

        SECTION("Toggled signal")
        {
            button->setPosition(40, 30);
            button->setSize(150, 100);

            unsigned int toggleCount = 0;
            button->onToggle(&genericCallback, std::ref(toggleCount));

            SECTION("mouse click")
            {
                button->leftMouseReleased({115, 80});
                REQUIRE(toggleCount == 0);

                button->leftMousePressed({115, 80});
                REQUIRE(toggleCount == 0);

                button->leftMouseReleased({115, 80});
                REQUIRE(toggleCount == 1);
            }

            SECTION("key pressed")
            {
                tgui::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = tgui::Event::KeyboardKey::Space;
                button->keyPressed(keyEvent);
                REQUIRE(toggleCount == 1);

                keyEvent.code = tgui::Event::KeyboardKey::Enter;
                button->keyPressed(keyEvent);
                REQUIRE(toggleCount == 2);

                SECTION("ignored")
                {
                    button->setIgnoreKeyEvents(true);

                    keyEvent.code = tgui::Event::KeyboardKey::Space;
                    button->keyPressed(keyEvent);
                    REQUIRE(toggleCount == 2);

                    keyEvent.code = tgui::Event::KeyboardKey::Enter;
                    button->keyPressed(keyEvent);
                    REQUIRE(toggleCount == 2);
                }
            }
        }
    }

    testWidgetRenderer(button->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = button->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDownHover", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDownDisabled", "rgb(150, 160, 170)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFocused", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDownFocused", "rgb(170, 180, 190)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDownHover", "rgb(180, 190, 200)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDownDisabled", "rgb(190, 200, 210)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorFocused", "rgb(200, 210, 220)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDownFocused", "rgb(210, 220, 230)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDown", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDownHover", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDownDisabled", "rgb(220, 230, 240)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", "rgb(230, 240, 250)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDownFocused", "rgb(240, 250, 255)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextOutlineColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextOutlineThickness", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDown", "Bold | Underlined"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleHover", "Underlined"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDownHover", "Italic | StrikeThrough"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDisabled", "StrikeThrough"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDownDisabled", "Italic | Bold"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleFocused", "Bold | StrikeThrough"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDownFocused", "Bold | Italic | Underlined | StrikeThrough"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDownHover", tgui::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", tgui::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDownDisabled", tgui::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFocused", tgui::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDownFocused", tgui::Color{170, 180, 190}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDownHover", tgui::Color{180, 190, 200}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDownDisabled", tgui::Color{190, 200, 210}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorFocused", tgui::Color{200, 210, 220}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDownFocused", tgui::Color{210, 220, 230}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDown", tgui::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", tgui::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDownHover", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDownDisabled", tgui::Color{220, 230, 240}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", tgui::Color{230, 240, 250}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDownFocused", tgui::Color{240, 250, 255}));
                REQUIRE_NOTHROW(renderer->setProperty("TextOutlineColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("TextOutlineThickness", 2));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyle::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDown", tgui::TextStyles(tgui::TextStyle::Bold | tgui::TextStyle::Underlined)));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleHover", tgui::TextStyle::Underlined));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDownHover", tgui::TextStyles(tgui::TextStyle::Italic | tgui::TextStyle::StrikeThrough)));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDisabled", tgui::TextStyle::StrikeThrough));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDownDisabled", tgui::TextStyles(tgui::TextStyle::Italic | tgui::TextStyle::Bold)));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleFocused", tgui::TextStyles(tgui::TextStyle::Bold | tgui::TextStyle::StrikeThrough)));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDownFocused", tgui::TextStyles(tgui::TextStyle::Bold |
                    tgui::TextStyles(tgui::TextStyle::Italic | tgui::TextStyle::Underlined | tgui::TextStyle::StrikeThrough))));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions")
            {
                renderer->setTextColor({20, 30, 40});
                renderer->setTextColorDown({40, 50, 60});
                renderer->setTextColorHover({30, 40, 50});
                renderer->setTextColorDownHover({140, 150, 160});
                renderer->setTextColorDisabled({90, 100, 110});
                renderer->setTextColorDownDisabled({150, 160, 170});
                renderer->setTextColorFocused({160, 170, 180});
                renderer->setTextColorDownFocused({170, 180, 190});
                renderer->setBackgroundColor({50, 60, 70});
                renderer->setBackgroundColorDown({70, 80, 90});
                renderer->setBackgroundColorHover({60, 70, 80});
                renderer->setBackgroundColorDownHover({180, 190, 200});
                renderer->setBackgroundColorDisabled({100, 110, 120});
                renderer->setBackgroundColorDownDisabled({190, 200, 210});
                renderer->setBackgroundColorFocused({200, 210, 220});
                renderer->setBackgroundColorDownFocused({210, 220, 230});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorderColorDown({120, 130, 140});
                renderer->setBorderColorHover({110, 120, 130});
                renderer->setBorderColorDownHover({10, 20, 30});
                renderer->setBorderColorDisabled({130, 140, 150});
                renderer->setBorderColorDownDisabled({220, 230, 240});
                renderer->setBorderColorFocused({230, 240, 250});
                renderer->setBorderColorDownFocused({240, 250, 255});
                renderer->setTextOutlineColor({10, 20, 30});
                renderer->setTextOutlineThickness(2);
                renderer->setTextStyle(tgui::TextStyle::Italic);
                renderer->setTextStyleDown(tgui::TextStyle::Bold | tgui::TextStyle::Underlined);
                renderer->setTextStyleHover(tgui::TextStyle::Underlined);
                renderer->setTextStyleDownHover(tgui::TextStyle::Italic | tgui::TextStyle::StrikeThrough);
                renderer->setTextStyleDisabled(tgui::TextStyle::StrikeThrough);
                renderer->setTextStyleDownDisabled(tgui::TextStyle::Italic | tgui::TextStyle::Bold);
                renderer->setTextStyleFocused(tgui::TextStyle::Bold | tgui::TextStyle::StrikeThrough);
                renderer->setTextStyleDownFocused(tgui::TextStyle::Bold |
                    tgui::TextStyles(tgui::TextStyle::Italic | tgui::TextStyle::Underlined | tgui::TextStyle::StrikeThrough));
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorDown").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorDownHover").getColor() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("TextColorDownDisabled").getColor() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("TextColorFocused").getColor() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("TextColorDownFocused").getColor() == tgui::Color(170, 180, 190));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorDownHover").getColor() == tgui::Color(180, 190, 200));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BackgroundColorDownDisabled").getColor() == tgui::Color(190, 200, 210));
            REQUIRE(renderer->getProperty("BackgroundColorFocused").getColor() == tgui::Color(200, 210, 220));
            REQUIRE(renderer->getProperty("BackgroundColorDownFocused").getColor() == tgui::Color(210, 220, 230));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BorderColorDown").getColor() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("BorderColorDownHover").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("BorderColorDownDisabled").getColor() == tgui::Color(220, 230, 240));
            REQUIRE(renderer->getProperty("BorderColorFocused").getColor() == tgui::Color(230, 240, 250));
            REQUIRE(renderer->getProperty("BorderColorDownFocused").getColor() == tgui::Color(240, 250, 255));
            REQUIRE(renderer->getProperty("TextOutlineColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TextOutlineThickness").getNumber() == 2);
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getProperty("TextStyleDown").getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Underlined));
            REQUIRE(renderer->getProperty("TextStyleHover").getTextStyle() == tgui::TextStyle::Underlined);
            REQUIRE(renderer->getProperty("TextStyleDownHover").getTextStyle() == (tgui::TextStyle::Italic | tgui::TextStyle::StrikeThrough));
            REQUIRE(renderer->getProperty("TextStyleDisabled").getTextStyle() == tgui::TextStyle::StrikeThrough);
            REQUIRE(renderer->getProperty("TextStyleDownDisabled").getTextStyle() == (tgui::TextStyle::Italic | tgui::TextStyle::Bold));
            REQUIRE(renderer->getProperty("TextStyleFocused").getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::StrikeThrough));
            REQUIRE(renderer->getProperty("TextStyleDownFocused").getTextStyle() == (tgui::TextStyle::Bold |
                    tgui::TextStyles(tgui::TextStyle::Italic | tgui::TextStyle::Underlined | tgui::TextStyle::StrikeThrough)));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getTextColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getTextColorDown() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getTextColorHover() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getTextColorDownHover() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getTextColorDisabled() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getTextColorDownDisabled() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getTextColorFocused() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getTextColorDownFocused() == tgui::Color(170, 180, 190));
            REQUIRE(renderer->getBackgroundColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getBackgroundColorDown() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorHover() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getBackgroundColorDownHover() == tgui::Color(180, 190, 200));
            REQUIRE(renderer->getBackgroundColorDisabled() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getBackgroundColorDownDisabled() == tgui::Color(190, 200, 210));
            REQUIRE(renderer->getBackgroundColorFocused() == tgui::Color(200, 210, 220));
            REQUIRE(renderer->getBackgroundColorDownFocused() == tgui::Color(210, 220, 230));
            REQUIRE(renderer->getBorderColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getBorderColorDown() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getBorderColorHover() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getBorderColorDownHover() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getBorderColorDisabled() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getBorderColorDownDisabled() == tgui::Color(220, 230, 240));
            REQUIRE(renderer->getBorderColorFocused() == tgui::Color(230, 240, 250));
            REQUIRE(renderer->getBorderColorDownFocused() == tgui::Color(240, 250, 255));
            REQUIRE(renderer->getTextOutlineColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getTextOutlineThickness() == 2);
            REQUIRE(renderer->getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getTextStyleDown() == (tgui::TextStyle::Bold | tgui::TextStyle::Underlined));
            REQUIRE(renderer->getTextStyleHover() == tgui::TextStyle::Underlined);
            REQUIRE(renderer->getTextStyleDownHover() == (tgui::TextStyle::Italic | tgui::TextStyle::StrikeThrough));
            REQUIRE(renderer->getTextStyleDisabled() == tgui::TextStyle::StrikeThrough);
            REQUIRE(renderer->getTextStyleDownDisabled() == (tgui::TextStyle::Italic | tgui::TextStyle::Bold));
            REQUIRE(renderer->getTextStyleFocused() == (tgui::TextStyle::Bold | tgui::TextStyle::StrikeThrough));
            REQUIRE(renderer->getTextStyleDownFocused() == (tgui::TextStyle::Bold |
                    tgui::TextStyles(tgui::TextStyle::Italic | tgui::TextStyle::Underlined | tgui::TextStyle::StrikeThrough)));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureNormal("resources/Texture1.png");
            tgui::Texture textureDown("resources/Texture2.png");
            tgui::Texture textureHover("resources/Texture3.png");
            tgui::Texture textureDownHover("resources/Texture4.png");
            tgui::Texture textureDisabled("resources/Texture5.png");
            tgui::Texture textureDownDisabled("resources/Texture6.png");
            tgui::Texture textureFocused("resources/Texture7.png");
            tgui::Texture textureDownFocused("resources/Texture8.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("Texture", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDown", tgui::Serializer::serialize(textureDown)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDownHover", tgui::Serializer::serialize(textureDownHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabled", tgui::Serializer::serialize(textureDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDownDisabled", tgui::Serializer::serialize(textureDownDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", tgui::Serializer::serialize(textureFocused)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDownFocused", tgui::Serializer::serialize(textureDownFocused)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("Texture", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDown", textureDown));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDownHover", textureDownHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabled", textureDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDownDisabled", textureDownDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", textureFocused));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDownFocused", textureDownFocused));
            }

            SECTION("functions")
            {
                renderer->setTexture(textureNormal);
                renderer->setTextureDown(textureDown);
                renderer->setTextureHover(textureHover);
                renderer->setTextureDownHover(textureDownHover);
                renderer->setTextureDisabled(textureDisabled);
                renderer->setTextureDownDisabled(textureDownDisabled);
                renderer->setTextureFocused(textureFocused);
                renderer->setTextureDownFocused(textureDownFocused);
            }

            REQUIRE(renderer->getProperty("Texture").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDown").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDownHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDownDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureFocused").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDownFocused").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTexture().getData() == textureNormal.getData());
            REQUIRE(renderer->getTextureDown().getData() == textureDown.getData());
            REQUIRE(renderer->getTextureHover().getData() == textureHover.getData());
            REQUIRE(renderer->getTextureDownHover().getData() == textureDownHover.getData());
            REQUIRE(renderer->getTextureDisabled().getData() == textureDisabled.getData());
            REQUIRE(renderer->getTextureDownDisabled().getData() == textureDownDisabled.getData());
            REQUIRE(renderer->getTextureFocused().getData() == textureFocused.getData());
            REQUIRE(renderer->getTextureDownFocused().getData() == textureDownFocused.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        button->setText(U"SomeText \u2190");
        button->setTextSize(25);
        button->setDown(true);

        testSavingWidget("ToggleButton", button);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 35, button)

        button->setEnabled(true);
        button->setPosition(10, 5);
        button->setSize(100, 25);
        button->setText("Click me!");
        button->setTextSize(16);

        tgui::ButtonRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(tgui::Color::Red);
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setTextStyle(tgui::TextStyle::Italic);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        button->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTextColorHover(tgui::Color::Magenta);
                                        renderer.setBackgroundColorHover(tgui::Color::Cyan);
                                        renderer.setBorderColorHover(tgui::Color::Yellow);
                                        renderer.setTextStyleHover(tgui::TextStyle::Bold);
                                        if (textured)
                                            renderer.setTextureHover("resources/Texture2.png");
                                     };

        auto setDownRenderer = [&](bool textured){
                                        renderer.setTextColorDown(tgui::Color::Black);
                                        renderer.setBackgroundColorDown(tgui::Color::White);
                                        renderer.setBorderColorDown({128, 128, 128});
                                        renderer.setTextStyleDown(tgui::TextStyle::Underlined);
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyleDisabled(tgui::TextStyle::StrikeThrough);
                                        if (textured)
                                            renderer.setTextureDown("resources/Texture3.png");
                                    };

        auto setDisabledRenderer = [&](bool textured){
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyleDisabled(tgui::TextStyle::StrikeThrough);
                                        if (textured)
                                            renderer.setTextureDisabled("resources/Texture4.png");
                                    };

        const auto mousePos = button->getPosition() + (button->getSize() / 2.f);

        SECTION("Outline")
        {
            renderer.setTextOutlineThickness(1);
            renderer.setTextOutlineColor(tgui::Color::White);
            TEST_DRAW("ToggleButton_Normal_Outline.png")
        }

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("ToggleButton_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("ToggleButton_Normal_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("ToggleButton_Normal_DownSet.png")
                    }
                }
            }

            SECTION("HoverState")
            {
                button->mouseMoved(mousePos);

                TEST_DRAW("ToggleButton_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("ToggleButton_Hover_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("ToggleButton_Hover_DownSet.png")
                    }
                }
            }

            SECTION("DownState")
            {
                button->mouseMoved(mousePos);
                button->leftMousePressed(mousePos);
                button->leftMouseReleased(mousePos);
                button->mouseMoved({0, 0});

                TEST_DRAW("ToggleButton_Down_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("ToggleButton_Down_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("ToggleButton_Down_DownSet.png")
                    }
                }
            }

            SECTION("DownHoverState")
            {
                button->mouseMoved(mousePos);
                button->leftMousePressed(mousePos);
                button->leftMouseReleased(mousePos);

                TEST_DRAW("ToggleButton_DownHover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("ToggleButton_DownHover_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("ToggleButton_DownHover_DownSet.png")
                    }
                }
            }

            SECTION("DisabledState")
            {
                button->setEnabled(false);

                TEST_DRAW("ToggleButton_Disabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(false);
                    TEST_DRAW("ToggleButton_Disabled_DisabledSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTexture("resources/Texture1.png");

            SECTION("NormalState")
            {
                TEST_DRAW("ToggleButton_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("ToggleButton_Normal_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("ToggleButton_Normal_TextureDownSet.png")
                    }
                }
            }

            SECTION("HoverState")
            {
                button->mouseMoved(mousePos);

                TEST_DRAW("ToggleButton_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("ToggleButton_Hover_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("ToggleButton_Hover_TextureDownSet.png")
                    }
                }
            }

            SECTION("DownState")
            {
                button->mouseMoved(mousePos);
                button->leftMousePressed(mousePos);
                button->leftMouseReleased(mousePos);
                button->mouseMoved({0, 0});

                TEST_DRAW("ToggleButton_Down_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("ToggleButton_Down_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("ToggleButton_Down_TextureDownSet.png")
                    }
                }
            }

            SECTION("DownHoverState")
            {
                button->mouseMoved(mousePos);
                button->leftMousePressed(mousePos);
                button->leftMouseReleased(mousePos);

                TEST_DRAW("ToggleButton_DownHover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("ToggleButton_DownHover_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("ToggleButton_DownHover_TextureDownSet.png")
                    }
                }
            }

            SECTION("DisabledState")
            {
                button->setEnabled(false);

                TEST_DRAW("ToggleButton_Disabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(true);
                    TEST_DRAW("ToggleButton_Disabled_TextureDisabledSet.png")
                }
            }
        }
    }
}
