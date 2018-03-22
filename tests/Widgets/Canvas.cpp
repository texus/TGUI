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
#include <TGUI/Widgets/Canvas.hpp>

TEST_CASE("[Canvas]")
{
    tgui::Canvas::Ptr canvas = tgui::Canvas::create();
    canvas->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(canvas->getWidgetType() == "Canvas");
    }

    SECTION("constructor")
    {
        canvas = tgui::Canvas::create({200, 100});
        REQUIRE(canvas->getSize() == sf::Vector2f(200, 100));
    }

    testWidgetRenderer(canvas->getRenderer());

    SECTION("Saving and loading from file")
    {
        REQUIRE_NOTHROW(canvas = tgui::Canvas::create({60, 40}));

        testSavingWidget("Canvas", canvas, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 150, canvas)

        tgui::WidgetRenderer renderer = tgui::RendererData::create();
        renderer.setOpacity(0.7f);
        canvas->setRenderer(renderer.getData());

        canvas->setSize({180, 140});
        canvas->setPosition({10, 5});

        sf::Texture texture;
        texture.loadFromFile("resources/image.png");

        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.setScale({150.f / texture.getSize().x, 100.f / texture.getSize().y});
        sprite.setPosition({15, 20});

        std::vector<sf::Vertex> vertices = {
                {{80, 90}, sf::Color::Red},
                {{80, 115}, sf::Color::Red},
                {{100, 90}, sf::Color::Red},
                {{100, 115}, sf::Color::Red}
            };

        canvas->clear(sf::Color::Yellow);
        canvas->draw(sprite);
        canvas->draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip);
        canvas->display();

        TEST_DRAW("Canvas.png")
    }
}
