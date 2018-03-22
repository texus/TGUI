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
#include <TGUI/Widgets/HorizontalWrap.hpp>
#include <TGUI/Widgets/Picture.hpp>

TEST_CASE("[HorizontalWrap]")
{
    auto wrap = tgui::HorizontalWrap::create({140, "100%"});
    wrap->setPosition(20, 10);

    SECTION("Positioning widgets")
    {
        auto pic = tgui::Picture::create("resources/image.png");
        pic->setSize({50, 40});

        auto pic1 = tgui::Picture::copy(pic);
        wrap->add(pic1);
        REQUIRE(pic1->getPosition() == sf::Vector2f(0, 0));

        auto pic2 = tgui::Picture::copy(pic);
        wrap->add(pic2);
        REQUIRE(pic2->getPosition() == sf::Vector2f(50, 0));

        auto pic3 = tgui::Picture::copy(pic);
        wrap->add(pic3);
        REQUIRE(pic3->getPosition() == sf::Vector2f(0, 40));

        auto pic4 = tgui::Picture::copy(pic);
        wrap->add(pic4);
        REQUIRE(pic4->getPosition() == sf::Vector2f(50, 40));

        wrap->getRenderer()->setPadding({10});
        REQUIRE(pic1->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(pic2->getPosition() == sf::Vector2f(60, 0));
        REQUIRE(pic3->getPosition() == sf::Vector2f(0, 50));
        REQUIRE(pic4->getPosition() == sf::Vector2f(60, 50));

        wrap->getRenderer()->setSpaceBetweenWidgets(20);
        REQUIRE(pic1->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(pic2->getPosition() == sf::Vector2f(70, 0));
        REQUIRE(pic3->getPosition() == sf::Vector2f(0, 60));
        REQUIRE(pic4->getPosition() == sf::Vector2f(70, 60));

        wrap->getRenderer()->setSpaceBetweenWidgets(21);
        REQUIRE(pic1->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(pic2->getPosition() == sf::Vector2f(0, 61));
        REQUIRE(pic3->getPosition() == sf::Vector2f(0, 122));
        REQUIRE(pic4->getPosition() == sf::Vector2f(0, 183));

        wrap->getRenderer()->setSpaceBetweenWidgets(20);
        wrap->removeAllWidgets();

        auto pic5 = tgui::Picture::copy(pic);
        pic5->setSize({80, 60});
        wrap->add(pic5);

        auto pic6 = tgui::Picture::copy(pic);
        pic6->setSize({80, 60});
        wrap->add(pic6);

        auto pic7 = tgui::Picture::copy(pic);
        pic7->setSize({20, 20});
        wrap->add(pic7);

        auto pic8 = tgui::Picture::copy(pic);
        wrap->add(pic8);

        REQUIRE(pic5->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(pic6->getPosition() == sf::Vector2f(0, 80));
        REQUIRE(pic7->getPosition() == sf::Vector2f(100, 80));
        REQUIRE(pic8->getPosition() == sf::Vector2f(0, 160));
    }

    testWidgetRenderer(wrap->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = wrap->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("SpaceBetweenWidgets", "15"));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", "(1, 2, 3, 4)"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("SpaceBetweenWidgets", 15));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Padding{1, 2, 3, 4}));
        }

        SECTION("functions")
        {
            renderer->setSpaceBetweenWidgets(15);
            renderer->setPadding({1, 2, 3, 4});
        }

        REQUIRE(renderer->getProperty("SpaceBetweenWidgets").getNumber() == 15);
        REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(1, 2, 3, 4));

        REQUIRE(renderer->getSpaceBetweenWidgets() == 15);
        REQUIRE(renderer->getPadding() == tgui::Padding(1, 2, 3, 4));
    }

    SECTION("Saving and loading from file")
    {
        wrap->getRenderer()->setSpaceBetweenWidgets(20);

        auto pic = tgui::Picture::create("resources/image.png");
        pic->setSize({50, 40});

        auto pic5 = tgui::Picture::copy(pic);
        pic5->setSize({80, 60});
        wrap->add(pic5);

        auto pic6 = tgui::Picture::copy(pic);
        pic6->setSize({80, 60});
        wrap->add(pic6);

        auto pic7 = tgui::Picture::copy(pic);
        pic7->setSize({20, 20});
        wrap->add(pic7);

        auto pic8 = tgui::Picture::copy(pic);
        wrap->add(pic8);

        testSavingWidget("HorizontalWrap", wrap, false);
    }
}
