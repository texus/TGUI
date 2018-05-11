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
#include <TGUI/Widgets/ProgressBar.hpp>

TEST_CASE("[ProgressBar]")
{
    tgui::ProgressBar::Ptr progressBar = tgui::ProgressBar::create();
    progressBar->getRenderer()->setFont("resources/DejaVuSans.ttf");
    progressBar->setMinimum(10);
    progressBar->setMaximum(20);
    progressBar->setValue(15);

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(progressBar->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(progressBar->connect("ValueChanged", [](unsigned int){}));
        REQUIRE_NOTHROW(progressBar->connect("ValueChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(progressBar->connect("ValueChanged", [](tgui::Widget::Ptr, std::string, unsigned int){}));

        REQUIRE_NOTHROW(progressBar->connect("Full", [](){}));
        REQUIRE_NOTHROW(progressBar->connect("Full", [](tgui::Widget::Ptr, std::string){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(progressBar->getWidgetType() == "ProgressBar");
    }

    SECTION("Position and Size")
    {
        progressBar->setPosition(40, 30);
        progressBar->setSize(300, 40);
        progressBar->getRenderer()->setBorders(2);

        REQUIRE(progressBar->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(progressBar->getSize() == sf::Vector2f(300, 40));
        REQUIRE(progressBar->getFullSize() == progressBar->getSize());
        REQUIRE(progressBar->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Minimum")
    {
        REQUIRE(progressBar->getMinimum() == 10);

        progressBar->setMinimum(12);
        REQUIRE(progressBar->getMinimum() == 12);
        REQUIRE(progressBar->getValue() == 15);
        REQUIRE(progressBar->getMaximum() == 20);

        progressBar->setMinimum(16);
        REQUIRE(progressBar->getMinimum() == 16);
        REQUIRE(progressBar->getValue() == 16);
        REQUIRE(progressBar->getMaximum() == 20);

        progressBar->setMinimum(22);
        REQUIRE(progressBar->getMinimum() == 22);
        REQUIRE(progressBar->getValue() == 22);
        REQUIRE(progressBar->getMaximum() == 22);
    }

    SECTION("Maximum")
    {
        REQUIRE(progressBar->getMaximum() == 20);

        progressBar->setMaximum(17);
        REQUIRE(progressBar->getMinimum() == 10);
        REQUIRE(progressBar->getValue() == 15);
        REQUIRE(progressBar->getMaximum() == 17);

        progressBar->setMaximum(11);
        REQUIRE(progressBar->getMinimum() == 10);
        REQUIRE(progressBar->getValue() == 11);
        REQUIRE(progressBar->getMaximum() == 11);

        progressBar->setMaximum(9);
        REQUIRE(progressBar->getMinimum() == 9);
        REQUIRE(progressBar->getValue() == 9);
        REQUIRE(progressBar->getMaximum() == 9);
    }

    SECTION("Value")
    {
        REQUIRE(progressBar->getValue() == 15);

        progressBar->setValue(14);
        REQUIRE(progressBar->getValue() == 14);

        progressBar->setValue(7);
        REQUIRE(progressBar->getValue() == 10);

        progressBar->setValue(23);
        REQUIRE(progressBar->getValue() == 20);
    }

    SECTION("incrementValue")
    {
        progressBar->setValue(18);
        REQUIRE(progressBar->getValue() == 18);
        REQUIRE(progressBar->incrementValue() == 19);
        REQUIRE(progressBar->getValue() == 19);
        REQUIRE(progressBar->incrementValue() == 20);
        REQUIRE(progressBar->getValue() == 20);
        REQUIRE(progressBar->incrementValue() == 20);
        REQUIRE(progressBar->getValue() == 20);
    }

    SECTION("Text")
    {
        REQUIRE(progressBar->getText() == "");
        progressBar->setText("SomeText");
        REQUIRE(progressBar->getText() == "SomeText");
    }

    SECTION("TextSize")
    {
        progressBar->setTextSize(25);
        REQUIRE(progressBar->getTextSize() == 25);
    }

    SECTION("FillDirection")
    {
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::LeftToRight);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::RightToLeft);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::TopToBottom);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::BottomToTop);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::BottomToTop);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::LeftToRight);
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(progressBar);
        }

        SECTION("ValueChanged")
        {
            unsigned int valueChangedCount = 0;
            progressBar->connect("ValueChanged", &genericCallback, std::ref(valueChangedCount));

            progressBar->setValue(4);
            REQUIRE(valueChangedCount == 1);

            progressBar->setValue(20);
            REQUIRE(valueChangedCount == 2);

            progressBar->setValue(20);
            REQUIRE(valueChangedCount == 2);

            progressBar->setValue(18);
            progressBar->incrementValue();
            progressBar->incrementValue();
            REQUIRE(valueChangedCount == 5);

            progressBar->incrementValue();
            REQUIRE(valueChangedCount == 5);
        }

        SECTION("Full")
        {
            unsigned int fullCount = 0;
            progressBar->connect("Full", &genericCallback, std::ref(fullCount));

            progressBar->setValue(4);
            REQUIRE(fullCount == 0);

            progressBar->setValue(20);
            REQUIRE(fullCount == 1);

            progressBar->setValue(20);
            REQUIRE(fullCount == 1);

            progressBar->setValue(18);
            progressBar->incrementValue();
            REQUIRE(fullCount == 1);

            progressBar->incrementValue();
            REQUIRE(fullCount == 2);

            progressBar->incrementValue();
            REQUIRE(fullCount == 2);
        }
    }

    testWidgetRenderer(progressBar->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = progressBar->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("FillColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFilled", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("FillColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFilled", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", sf::Text::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({20, 30, 40});
                renderer->setFillColor({30, 40, 50});
                renderer->setTextColor({40, 50, 60});
                renderer->setTextColorFilled({50, 60, 70});
                renderer->setBorderColor({60, 70, 80});
                renderer->setTextStyle(sf::Text::Italic);
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("FillColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColorFilled").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getBackgroundColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getFillColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getTextColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getTextColorFilled() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getBorderColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureBack("resources/Black.png", {180, 64, 90, 40}, {20, 0, 50, 40});
            tgui::Texture textureFront("resources/Black.png", {180, 108, 82, 32}, {16, 0, 50, 32});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBack)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFill", tgui::Serializer::serialize(textureFront)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBack));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFill", textureFront));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBack);
                renderer->setTextureFill(textureFront);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureFill").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBack.getData());
            REQUIRE(renderer->getTextureFill().getData() == textureFront.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        progressBar->setText("SomeText");
        progressBar->setTextSize(25);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);

        testSavingWidget("ProgressBar", progressBar);
    }

    SECTION("Draw")
    {
        progressBar->setEnabled(true);
        progressBar->setPosition(10, 5);
        progressBar->setMinimum(1000);
        progressBar->setMaximum(2000);
        progressBar->setValue(1600);
        progressBar->setTextSize(16);

        tgui::ProgressBarRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setFillColor(sf::Color::Yellow);
        renderer.setTextColor(sf::Color::Red);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setTextStyle(sf::Text::Style::Italic);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        progressBar->setRenderer(renderer.getData());

        SECTION("Colored")
        {
            SECTION("Horizontal")
            {
                TEST_DRAW_INIT(110, 40, progressBar)

                progressBar->setSize({90, 30});
                progressBar->setText("60% done");

                SECTION("LeftToRight")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
                    TEST_DRAW("ProgressBar_LeftToRight.png")

                    renderer.setTextColorFilled(sf::Color::Magenta);
                    TEST_DRAW("ProgressBar_LeftToRight_TextFillColor.png")
                }

                SECTION("RightToLeft")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);
                    TEST_DRAW("ProgressBar_RightToLeft.png")

                    renderer.setTextColorFilled(sf::Color::Magenta);
                    TEST_DRAW("ProgressBar_RightToLeft_TextFillColor.png")
                }
            }

            SECTION("Vertical")
            {
                TEST_DRAW_INIT(50, 100, progressBar)

                progressBar->setSize({30, 90});

                SECTION("LeftToRight")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
                    TEST_DRAW("ProgressBar_TopToBottom.png")
                }

                SECTION("RightToLeft")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::BottomToTop);
                    TEST_DRAW("ProgressBar_BottomToTop.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            renderer.setTextureFill("resources/Texture2.png");

            SECTION("Horizontal")
            {
                TEST_DRAW_INIT(110, 40, progressBar)

                progressBar->setSize({90, 30});
                progressBar->setText("60% done");

                SECTION("LeftToRight")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
                    TEST_DRAW("ProgressBar_Textured_LeftToRight.png")

                    renderer.setTextColorFilled(sf::Color::Magenta);
                    TEST_DRAW("ProgressBar_Textured_LeftToRight_TextFillColor.png")
                }

                SECTION("RightToLeft")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);
                    TEST_DRAW("ProgressBar_Textured_RightToLeft.png")

                    renderer.setTextColorFilled(sf::Color::Magenta);
                    TEST_DRAW("ProgressBar_Textured_RightToLeft_TextFillColor.png")
                }
            }

            SECTION("Vertical")
            {
                TEST_DRAW_INIT(50, 100, progressBar)

                progressBar->setSize({30, 90});

                SECTION("LeftToRight")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
                    TEST_DRAW("ProgressBar_Textured_TopToBottom.png")
                }

                SECTION("RightToLeft")
                {
                    progressBar->setFillDirection(tgui::ProgressBar::FillDirection::BottomToTop);
                    TEST_DRAW("ProgressBar_Textured_BottomToTop.png")
                }
            }
        }
    }
}
