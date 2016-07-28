/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Gui.hpp>

TEST_CASE("[Picture]")
{
    tgui::Picture::Ptr picture = std::make_shared<tgui::Picture>();

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(picture->getWidgetType() == "Picture");
    }

    SECTION("Constructor")
    {
        sf::Texture texture;
        texture.loadFromFile("resources/image.png");

        SECTION("from file")
        {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>("resources/image.png"));
            REQUIRE(picture->getLoadedFilename() == "resources/image.png");
        }

        SECTION("from texture")
        {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>(tgui::Texture{"resources/image.png"}));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        SECTION("from sf::Texture")
        {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>(texture));
            REQUIRE(picture->getLoadedFilename() == "");
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

    SECTION("setTexture")
    {
        picture->setSize(50, 50);

        sf::Texture texture;
        texture.loadFromFile("resources/image.png");

        SECTION("from file")
        {
            REQUIRE_NOTHROW(picture->setTexture("resources/image.png"));
            REQUIRE(picture->getLoadedFilename() == "resources/image.png");
        }

        SECTION("from texture")
        {
            REQUIRE_NOTHROW(picture->setTexture(tgui::Texture{"resources/image.png"}));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        SECTION("from sf::Texture")
        {
            REQUIRE_NOTHROW(picture->setTexture(texture));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        REQUIRE(picture->getSize() == sf::Vector2f(50, 50));
    }

    SECTION("Smooth")
    {
        // Calling isSmooth has no effect when not loaded
        REQUIRE(!picture->isSmooth());
        picture->setSmooth(true);
        REQUIRE(!picture->isSmooth());

        picture = std::make_shared<tgui::Picture>("resources/image.png");
        REQUIRE(!picture->isSmooth());
        picture->setSmooth(true);
        REQUIRE(picture->isSmooth());
        picture->setSmooth(false);
        REQUIRE(!picture->isSmooth());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(picture);
        }

        SECTION("double click")
        {
            picture->setTexture("resources/image.png");
            picture->setPosition(40, 30);
            picture->setSize(150, 100);

            unsigned int doubleClickedCount = 0;
            picture->connect("DoubleClicked", genericCallback, std::ref(doubleClickedCount));

            picture->leftMousePressed(115, 80);
            picture->leftMouseReleased(115, 80);

            tgui::Gui gui;
            gui.add(picture);
            gui.updateTime(DOUBLE_CLICK_TIMEOUT);

            picture->leftMousePressed(115, 80);
            picture->leftMouseReleased(115, 80);
            REQUIRE(doubleClickedCount == 0);

            gui.updateTime(DOUBLE_CLICK_TIMEOUT / 2.f);

            picture->leftMousePressed(115, 80);
            picture->leftMouseReleased(115, 80);
            REQUIRE(doubleClickedCount == 1);
        }
    }

    SECTION("Renderer")
    {
        auto renderer = picture->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
        }

        SECTION("functions")
        {
            renderer->setOpacity(0.8f);
        }

        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
        REQUIRE(renderer->getOpacity() == 0.8f);

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }

    SECTION("Saving and loading from file")
    {
        picture->setSmooth();
        testSavingWidget("Picture", picture, false);
    }
}
