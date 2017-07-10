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
#include <TGUI/Widgets/Slider.hpp>

TEST_CASE("[Slider]")
{
    tgui::Slider::Ptr slider = tgui::Slider::create();
    slider->getRenderer()->setFont("resources/DejaVuSans.ttf");
    slider->setMinimum(10);
    slider->setMaximum(20);
    slider->setValue(15);

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(slider->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(slider->connect("ValueChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(slider->onValueChange->connect([](){}));
        REQUIRE_NOTHROW(slider->onValueChange->connect([](int){}));
        REQUIRE_NOTHROW(slider->onValueChange->connect([](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(slider->onValueChange->connect([](tgui::Widget::Ptr, std::string, int){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(slider->getWidgetType() == "Slider");
    }

    SECTION("Position and Size")
    {
        slider->setPosition(40, 30);
        slider->setSize(150, 25);
        slider->getRenderer()->setBorders(2);

        REQUIRE(slider->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(slider->getSize() == sf::Vector2f(150, 25));

        // TODO: Test getFullSize and getWidgetOffset
    }

    SECTION("Minimum")
    {
        REQUIRE(slider->getMinimum() == 10);

        slider->setMinimum(12);
        REQUIRE(slider->getMinimum() == 12);
        REQUIRE(slider->getValue() == 15);
        REQUIRE(slider->getMaximum() == 20);

        slider->setMinimum(16);
        REQUIRE(slider->getMinimum() == 16);
        REQUIRE(slider->getValue() == 16);
        REQUIRE(slider->getMaximum() == 20);

        slider->setMinimum(22);
        REQUIRE(slider->getMinimum() == 22);
        REQUIRE(slider->getValue() == 22);
        REQUIRE(slider->getMaximum() == 22);
    }

    SECTION("Maximum")
    {
        REQUIRE(slider->getMaximum() == 20);

        slider->setMaximum(17);
        REQUIRE(slider->getMinimum() == 10);
        REQUIRE(slider->getValue() == 15);
        REQUIRE(slider->getMaximum() == 17);

        slider->setMaximum(11);
        REQUIRE(slider->getMinimum() == 10);
        REQUIRE(slider->getValue() == 11);
        REQUIRE(slider->getMaximum() == 11);

        slider->setMaximum(9);
        REQUIRE(slider->getMinimum() == 9);
        REQUIRE(slider->getValue() == 9);
        REQUIRE(slider->getMaximum() == 9);
    }

    SECTION("Value")
    {
        REQUIRE(slider->getValue() == 15);
        
        slider->setValue(14);
        REQUIRE(slider->getValue() == 14);
        
        slider->setValue(7);
        REQUIRE(slider->getValue() == 10);
        
        slider->setValue(23);
        REQUIRE(slider->getValue() == 20);
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(slider);
        }

        SECTION("ValueChanged")
        {
            unsigned int valueChangedCount = 0;
            slider->onValueChange->connect([&]{ genericCallback(valueChangedCount); });

            slider->setValue(14);
            REQUIRE(valueChangedCount == 1);

            slider->setValue(14);
            REQUIRE(valueChangedCount == 1);

            // TODO: Test value changes on mouse events
        }
    }

    testWidgetRenderer(slider->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = slider->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions")
            {
                renderer->setTrackColor({30, 40, 50});
                renderer->setTrackColorHover({40, 50, 60});
                renderer->setThumbColor({50, 60, 70});
                renderer->setThumbColorHover({60, 70, 80});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorderColorHover({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("TrackColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getTrackColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getTrackColorHover() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getThumbColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getThumbColorHover() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getBorderColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getBorderColorHover() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureTrack("resources/Black.png", {203, 150, 20, 45}, {0, 15, 20, 15});
            tgui::Texture textureTrackHover("resources/Black.png", {223, 150, 20, 45}, {0, 15, 20, 15});
            tgui::Texture textureThumb("resources/Black.png", {243, 150, 30, 30});
            tgui::Texture textureThumbHover("resources/Black.png", {243, 150, 30, 30});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrack", tgui::Serializer::serialize(textureTrack)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrackHover", tgui::Serializer::serialize(textureTrackHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumb", tgui::Serializer::serialize(textureThumb)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumbHover", tgui::Serializer::serialize(textureThumbHover)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrack", textureTrack));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrackHover", textureTrackHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumb", textureThumb));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumbHover", textureThumbHover));
            }

            SECTION("functions")
            {
                renderer->setTextureTrack(textureTrack);
                renderer->setTextureTrackHover(textureTrackHover);
                renderer->setTextureThumb(textureThumb);
                renderer->setTextureThumbHover(textureThumbHover);
            }

            REQUIRE(renderer->getProperty("TextureTrack").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureTrackHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureThumb").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureThumbHover").getTexture().getData() != nullptr);

            REQUIRE(renderer->getProperty("TextureTrack").getTexture().getData() == textureTrack.getData());
            REQUIRE(renderer->getProperty("TextureTrackHover").getTexture().getData() == textureTrackHover.getData());
            REQUIRE(renderer->getProperty("TextureThumb").getTexture().getData() == textureThumb.getData());
            REQUIRE(renderer->getProperty("TextureThumbHover").getTexture().getData() == textureThumbHover.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        slider->setMinimum(10);
        slider->setMaximum(50);
        slider->setValue(20);

        testSavingWidget("Slider", slider);
    }
}
