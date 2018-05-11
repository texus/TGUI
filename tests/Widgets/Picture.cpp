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
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Gui.hpp>

TEST_CASE("[Picture]")
{
    tgui::Picture::Ptr picture = tgui::Picture::create();

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, sf::Vector2f){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(picture->getWidgetType() == "Picture");
    }

    SECTION("Create")
    {
        sf::Texture texture;
        texture.loadFromFile("resources/image.png");

        SECTION("from tgui::Texture")
        {
            REQUIRE_NOTHROW(picture = tgui::Picture::create("resources/image.png"));
            REQUIRE(picture->getRenderer()->getTexture().getId() == "resources/image.png");
        }

        SECTION("from sf::Texture")
        {
            REQUIRE_NOTHROW(picture = tgui::Picture::create(texture));
            REQUIRE(picture->getRenderer()->getTexture().getId() == "");
        }

        REQUIRE(picture->getSize() == sf::Vector2f(texture.getSize()));
    }

    SECTION("Position and Size")
    {
        picture->setPosition(40, 30);
        picture->setSize(150, 100);

        REQUIRE(picture->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(picture->getSize() == sf::Vector2f(150, 100));
        REQUIRE(picture->getFullSize() == picture->getSize());
        REQUIRE(picture->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("IgnoreMouseEvents")
    {
        REQUIRE(!picture->isIgnoringMouseEvents());
        picture->ignoreMouseEvents(true);
        REQUIRE(picture->isIgnoringMouseEvents());
        picture->ignoreMouseEvents(false);
        REQUIRE(!picture->isIgnoringMouseEvents());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(picture);
        }

        SECTION("double click")
        {
            picture->getRenderer()->setTexture("resources/image.png");
            picture->setPosition(40, 30);
            picture->setSize(150, 100);

            unsigned int doubleClickedCount = 0;
            picture->connect("DoubleClicked", &mouseCallback, std::ref(doubleClickedCount));

            picture->leftMousePressed({115, 80});
            picture->leftMouseReleased({115, 80});

            tgui::Gui gui;
            gui.add(picture);
            gui.updateTime(DOUBLE_CLICK_TIMEOUT);

            picture->leftMousePressed({115, 80});
            picture->leftMouseReleased({115, 80});
            REQUIRE(doubleClickedCount == 0);

            gui.updateTime(DOUBLE_CLICK_TIMEOUT / 2.f);

            picture->leftMousePressed({115, 80});
            picture->leftMouseReleased({115, 80});
            REQUIRE(doubleClickedCount == 1);
        }
    }

    SECTION("Renderer")
    {
        auto renderer = picture->getRenderer();

        tgui::Texture texture("resources/Texture1.png");

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Texture", tgui::Serializer::serialize(texture)));
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Texture", texture));
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
        }

        SECTION("functions")
        {
            renderer->setTexture(texture);
            renderer->setOpacity(0.8f);
        }

        REQUIRE(renderer->getTexture().getData() == texture.getData());
        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
        REQUIRE(renderer->getOpacity() == 0.8f);

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }

    SECTION("Saving and loading from file")
    {
        picture->getRenderer()->setTexture("resources/image.png");
        picture->setSize(80, 60);
        picture->ignoreMouseEvents(true);

        testSavingWidget("Picture", picture, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(60, 40, picture)

        picture->getRenderer()->setTexture("resources/image.png");

        picture->setPosition(10, 5);
        picture->setSize(40, 30);

        TEST_DRAW("Picture.png")
    }
}
