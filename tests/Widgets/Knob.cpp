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
#include <TGUI/Widgets/Knob.hpp>

TEST_CASE("[Knob]")
{
    tgui::Knob::Ptr knob = tgui::Knob::create();
    knob->getRenderer()->setFont("resources/DejaVuSans.ttf");
    knob->setMinimum(10);
    knob->setMaximum(20);
    knob->setValue(15);

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(knob->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(knob->connect("ValueChanged", [](int){}));
        REQUIRE_NOTHROW(knob->connect("ValueChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(knob->connect("ValueChanged", [](tgui::Widget::Ptr, std::string, int){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(knob->getWidgetType() == "Knob");
    }

    SECTION("Position and Size")
    {
        knob->setPosition(40, 30);
        knob->setSize(100, 100);
        knob->getRenderer()->setBorders(2);

        REQUIRE(knob->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(knob->getSize() == sf::Vector2f(100, 100));
        REQUIRE(knob->getFullSize() == knob->getSize());
        REQUIRE(knob->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("StartRotation")
    {
        knob->setStartRotation(50);
        REQUIRE(knob->getStartRotation() == 50);

        knob->setStartRotation(740);
        REQUIRE(knob->getStartRotation() == 20);

        knob->setStartRotation(-30);
        REQUIRE(knob->getStartRotation() == 330);
    }
    
    SECTION("EndRotation")
    {
        knob->setEndRotation(50);
        REQUIRE(knob->getEndRotation() == 50);

        knob->setEndRotation(750);
        REQUIRE(knob->getEndRotation() == 30);

        knob->setEndRotation(-25);
        REQUIRE(knob->getEndRotation() == 335);
    }

    SECTION("Minimum")
    {
        REQUIRE(knob->getMinimum() == 10);

        knob->setMinimum(12);
        REQUIRE(knob->getMinimum() == 12);
        REQUIRE(knob->getValue() == 15);
        REQUIRE(knob->getMaximum() == 20);

        knob->setMinimum(16);
        REQUIRE(knob->getMinimum() == 16);
        REQUIRE(knob->getValue() == 16);
        REQUIRE(knob->getMaximum() == 20);

        knob->setMinimum(22);
        REQUIRE(knob->getMinimum() == 22);
        REQUIRE(knob->getValue() == 22);
        REQUIRE(knob->getMaximum() == 22);
    }

    SECTION("Maximum")
    {
        REQUIRE(knob->getMaximum() == 20);

        knob->setMaximum(17);
        REQUIRE(knob->getMinimum() == 10);
        REQUIRE(knob->getValue() == 15);
        REQUIRE(knob->getMaximum() == 17);

        knob->setMaximum(11);
        REQUIRE(knob->getMinimum() == 10);
        REQUIRE(knob->getValue() == 11);
        REQUIRE(knob->getMaximum() == 11);

        knob->setMaximum(9);
        REQUIRE(knob->getMinimum() == 9);
        REQUIRE(knob->getValue() == 9);
        REQUIRE(knob->getMaximum() == 9);
    }

    SECTION("Value")
    {
        REQUIRE(knob->getValue() == 15);

        knob->setValue(14);
        REQUIRE(knob->getValue() == 14);

        knob->setValue(7);
        REQUIRE(knob->getValue() == 10);

        knob->setValue(23);
        REQUIRE(knob->getValue() == 20);
    }

    SECTION("ClockwiseTurning")
    {
        REQUIRE(knob->getClockwiseTurning());
        knob->setClockwiseTurning(false);
        REQUIRE(!knob->getClockwiseTurning());
        knob->setClockwiseTurning(true);
        REQUIRE(knob->getClockwiseTurning());
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(knob);
        }

        /// TODO
    }

    testWidgetRenderer(knob->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = knob->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setThumbColor({40, 50, 60});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Knob/Back.png");
            tgui::Texture textureForeground("resources/Knob/Front.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureForeground", tgui::Serializer::serialize(textureForeground)));
                REQUIRE_NOTHROW(renderer->setProperty("ImageRotation", "90"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureForeground", textureForeground));
                REQUIRE_NOTHROW(renderer->setProperty("ImageRotation", 90));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
                renderer->setTextureForeground(textureForeground);
                renderer->setImageRotation(90);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureForeground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
            REQUIRE(renderer->getTextureForeground().getData() == textureForeground.getData());
            REQUIRE(renderer->getImageRotation() == 90);
        }
    }

    SECTION("Saving and loading from file")
    {
        knob->setStartRotation(-180);
        knob->setEndRotation(0);
        knob->setClockwiseTurning(false);
        knob->setMinimum(10);
        knob->setMaximum(50);
        knob->setValue(20);

        testSavingWidget("Knob", knob, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(70, 60, knob)

        knob->setEnabled(true);
        knob->setPosition({10, 5});
        knob->setSize({50, 50});
        knob->setMinimum(1000);
        knob->setMaximum(2000);
        knob->setValue(1700);
        knob->setStartRotation(-60);
        knob->setEndRotation(-120);
        knob->setClockwiseTurning(false);

        tgui::KnobRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setThumbColor(sf::Color::Red);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setImageRotation(-90);
        renderer.setOpacity(0.7f);
        knob->setRenderer(renderer.getData());

        SECTION("Colored")
        {
            renderer.setBorders({2});

            TEST_DRAW("Knob.png")
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            renderer.setTextureForeground("resources/Texture2.png");

            TEST_DRAW("Knob_Textured.png")
        }
    }
}
