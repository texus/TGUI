/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[Picture]") {
    tgui::Picture::Ptr picture = std::make_shared<tgui::Picture>();

    SECTION("Signals") {
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](){}));
    }

    SECTION("WidgetType") {
        REQUIRE(picture->getWidgetType() == "Picture");
    }

    SECTION("constructor") {
        sf::Texture texture;
        texture.loadFromFile("resources/Black.png");

        SECTION("from file") {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>("resources/Black.png"));
            REQUIRE(picture->getLoadedFilename() == "resources/Black.png");
        }

        SECTION("from texture") {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>(texture));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        REQUIRE(picture->getSize() == sf::Vector2f(texture.getSize()));
    }

    SECTION("Smooth") {
        // No effect when not loaded
        REQUIRE(!picture->isSmooth());
        picture->setSmooth(true);
        REQUIRE(!picture->isSmooth());

        picture = std::make_shared<tgui::Picture>("resources/Black.png");
        REQUIRE(!picture->isSmooth());
        picture->setSmooth(true);
        REQUIRE(picture->isSmooth());
        picture->setSmooth(false);
        REQUIRE(!picture->isSmooth());
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>("resources/Black.png"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(picture);

        picture->setSmooth();

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePicture1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFilePicture1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePicture2.txt"));
        REQUIRE(compareFiles("WidgetFilePicture1.txt", "WidgetFilePicture2.txt"));

        SECTION("Copying widget") {
            tgui::Picture temp;
            temp = *picture;

            parent->removeAllWidgets();
            parent->add(tgui::Picture::copy(std::make_shared<tgui::Picture>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePicture2.txt"));
            REQUIRE(compareFiles("WidgetFilePicture1.txt", "WidgetFilePicture2.txt"));
        }
    }
}
