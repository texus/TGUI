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
#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #include <SFML/Graphics/Texture.hpp>
#endif

#include "Tests.hpp"

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Loading/ImageLoader.hpp>
#endif

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.renderer.sfml_graphics;
    #else
        #include <TGUI/Backend/Renderer/SFML-Graphics/BackendRendererSFML.hpp>
    #endif
#endif

TEST_CASE("[Picture]")
{
    tgui::Picture::Ptr picture = tgui::Picture::create();

    SECTION("Signals")
    {
        picture->onDoubleClick([](){});
        picture->onDoubleClick([](tgui::Vector2f){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(picture)->getSignal("DoubleClicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(picture->getWidgetType() == "Picture");
    }

    SECTION("Create")
    {
        tgui::Vector2u imageSize;
        (void)tgui::ImageLoader::loadFromFile("resources/image.png", imageSize);

        SECTION("from tgui::Texture")
        {
            REQUIRE_NOTHROW(picture = tgui::Picture::create("resources/image.png"));
            REQUIRE(picture->getRenderer()->getTexture().getId() == "resources/image.png");
        }

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
        if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer()))
        {
            SECTION("from sf::Texture")
            {
                sf::Texture sfTexture;
                REQUIRE(sfTexture.loadFromFile("resources/image.png"));

                tgui::Texture tguiTexture;
                tguiTexture.loadFromPixelData(sfTexture.getSize(), sfTexture.copyToImage().getPixelsPtr());

                REQUIRE_NOTHROW(picture = tgui::Picture::create(tguiTexture));
                REQUIRE(picture->getRenderer()->getTexture().getId() == "");
            }
        }
#endif

        REQUIRE(picture->getSize() == tgui::Vector2f(static_cast<float>(imageSize.x), static_cast<float>(imageSize.y)));
    }

    SECTION("Position and Size")
    {
        picture->setPosition(40, 30);
        picture->setSize(150, 100);

        REQUIRE(picture->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(picture->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(picture->getFullSize() == picture->getSize());
        REQUIRE(picture->getWidgetOffset() == tgui::Vector2f(0, 0));
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
            picture->onDoubleClick(&mouseCallback, std::ref(doubleClickedCount));

            picture->leftMousePressed({115, 80});
            picture->leftMouseReleased({115, 80});

            GuiNull gui;
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

        SECTION("Mouse events pass through picture with IgnoreMouseEvents=true")
        {
            unsigned int mousePressedCountBack = 0;
            unsigned int mousePressedCountFront = 0;

            picture->setPosition({40, 30});
            picture->setSize({150, 100});

            auto backgroundPic = tgui::Picture::copy(picture);

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(backgroundPic);
            parent->add(picture);

            backgroundPic->onMousePress(&genericCallback, std::ref(mousePressedCountBack));
            picture->onMousePress(&genericCallback, std::ref(mousePressedCountFront));

            parent->leftMousePressed({175, 135});
            parent->leftMouseReleased({175, 135});
            parent->leftMouseButtonNoLongerDown();

            REQUIRE(mousePressedCountBack == 0);
            REQUIRE(mousePressedCountFront == 1);

            picture->ignoreMouseEvents(true);

            parent->leftMousePressed({175, 135});
            parent->leftMouseReleased({175, 135});
            parent->leftMouseButtonNoLongerDown();

            REQUIRE(mousePressedCountBack == 1);
            REQUIRE(mousePressedCountFront == 1);
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

    SECTION("Bug Fixes")
    {
        SECTION("Relative size didn't work when loading from file")
        {
            std::stringstream ss(R"(Picture.Picture1 {
                Size = (100%, 100%);
                Renderer {
                    Texture = "resources/Texture1.png";
                }
            })");

            auto parent = tgui::Group::create();
            parent->setSize(280, 140);
            parent->loadWidgetsFromStream(ss);

            REQUIRE(parent->getWidgets()[0]->getSize() == tgui::Vector2f(280, 140));
        }
    }
}
