/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
#include <TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp>
#include <TGUI/Backend/Renderer/SFML-Graphics/BackendRendererSFML.hpp>

#include <SFML/Graphics/View.hpp>
namespace sf  // Anonymous namespace didn't work for Clang on macOS
{
    bool operator==(const sf::View& left, const sf::View& right)
    {
        return left.getCenter() == right.getCenter()
            && left.getSize() == right.getSize()
            && left.getRotation() == right.getRotation()
            && left.getViewport() == right.getViewport();
    }
}

TEST_CASE("[CanvasSFML]")
{
    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer()))
    {
        tgui::CanvasSFML::Ptr canvas = tgui::CanvasSFML::create();
        canvas->getRenderer()->setFont("resources/DejaVuSans.ttf");

        SECTION("WidgetType")
        {
            REQUIRE(canvas->getWidgetType() == "CanvasSFML");
        }

        SECTION("constructor")
        {
            canvas = tgui::CanvasSFML::create({200, 100});
            REQUIRE(canvas->getSize() == tgui::Vector2f(200, 100));
        }

        SECTION("view")
        {
            canvas = tgui::CanvasSFML::create({200, 100});

            REQUIRE(canvas->getView() == sf::View({0, 0, 200, 100}));
            REQUIRE(canvas->getDefaultView() == sf::View({0, 0, 200, 100}));

            canvas->setView(sf::View({20, 10, 100, 50}));
            REQUIRE(canvas->getView() == sf::View({20, 10, 100, 50}));
            REQUIRE(canvas->getDefaultView() == sf::View({0, 0, 200, 100}));

            REQUIRE(canvas->getViewport() == tgui::IntRect(0, 0, 200, 100));

            sf::View view({20, 10, 100, 50});
            view.setViewport({0.1f, 0.2f, 0.5f, 0.6f});
            canvas->setView(view);
            REQUIRE(canvas->getViewport() == tgui::IntRect(20, 20, 100, 60));
        }

        SECTION("internal render texture")
        {
            canvas = tgui::CanvasSFML::create({50, 50});
            sf::RenderTexture *internalRenderTexture = &canvas->getRenderTexture();

            canvas->setSize({70, 80});
            canvas->setView(sf::View({20, 10, 100, 50}));
            canvas->setPosition({10, 5});

            // The address of the internal render texture never changes
            REQUIRE(internalRenderTexture == &canvas->getRenderTexture());
        }

        testWidgetRenderer(canvas->getRenderer());

        SECTION("Saving and loading from file")
        {
            REQUIRE_NOTHROW(canvas = tgui::CanvasSFML::create({60, 40}));

            testSavingWidget("CanvasSFML", canvas, false);
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
            REQUIRE(texture.loadFromFile("resources/image.png"));

            sf::Sprite sprite;
            sprite.setTexture(texture);
            sprite.setScale({150.f / texture.getSize().x, 100.f / texture.getSize().y});
            sprite.setPosition({15, 20});

            std::vector<sf::Vertex> vertices = {
                    {{80, 90}, tgui::Color::Red},
                    {{80, 115}, tgui::Color::Red},
                    {{100, 90}, tgui::Color::Red},
                    {{100, 115}, tgui::Color::Red}
                };

            canvas->clear(tgui::Color::Yellow);
            canvas->draw(sprite);
            canvas->draw(vertices.data(), vertices.size(), sf::PrimitiveType::TriangleStrip);
            canvas->display();

            TEST_DRAW("Canvas.png")
        }
    }
}
#endif

#if TGUI_HAS_RENDERER_BACKEND_SDL_RENDERER
#include <TGUI/Backend/Renderer/SDL_Renderer/CanvasSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_Renderer/BackendRendererSDL.hpp>

TEST_CASE("[CanvasSDL]")
{
    if (std::dynamic_pointer_cast<tgui::BackendRendererSDL>(tgui::getBackend()->getRenderer()))
    {
        tgui::CanvasSDL::Ptr canvas = tgui::CanvasSDL::create();
        canvas->getRenderer()->setFont("resources/DejaVuSans.ttf");

        SECTION("WidgetType")
        {
            REQUIRE(canvas->getWidgetType() == "CanvasSDL");
        }

        SECTION("constructor")
        {
            canvas = tgui::CanvasSDL::create({200, 100});
            REQUIRE(canvas->getSize() == tgui::Vector2f(200, 100));
        }

        SECTION("internal texture target")
        {
            canvas = tgui::CanvasSDL::create({50, 50});
            SDL_Texture* internalTextureTarget = canvas->getTextureTarget();

            canvas->setSize({70, 80});

            // The address of the texture target has changed with the resize
            REQUIRE(internalTextureTarget != canvas->getTextureTarget());
            internalTextureTarget = canvas->getTextureTarget();

            // Changing the position has no impact on the internal texture
            canvas->setPosition({10, 5});
            REQUIRE(internalTextureTarget == canvas->getTextureTarget());
        }

        testWidgetRenderer(canvas->getRenderer());

        SECTION("Saving and loading from file")
        {
            REQUIRE_NOTHROW(canvas = tgui::CanvasSDL::create({60, 40}));

            testSavingWidget("CanvasSDL", canvas, false);
        }
    }
}
#endif

#if TGUI_HAS_BACKEND_OPENGL3
#include <TGUI/Backend/Renderer/OpenGL3/CanvasOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL3/BackendRendererOpenGL3.hpp>

TEST_CASE("[CanvasOpenGL3]")
{
    if (std::dynamic_pointer_cast<tgui::BackendRendererOpenGL3>(tgui::getBackend()->getRenderer()))
    {
        tgui::CanvasOpenGL3::Ptr canvas = tgui::CanvasOpenGL3::create();
        canvas->getRenderer()->setFont("resources/DejaVuSans.ttf");

        SECTION("WidgetType")
        {
            REQUIRE(canvas->getWidgetType() == "CanvasOpenGL3");
        }

        SECTION("constructor")
        {
            canvas = tgui::CanvasOpenGL3::create({200, 100});
            REQUIRE(canvas->getSize() == tgui::Vector2f(200, 100));
        }

        testWidgetRenderer(canvas->getRenderer());

        SECTION("Saving and loading from file")
        {
            REQUIRE_NOTHROW(canvas = tgui::CanvasOpenGL3::create({60, 40}));

            testSavingWidget("CanvasOpenGL3", canvas, false);
        }
    }
}
#endif

#if TGUI_HAS_BACKEND_GLES2
#include <TGUI/Backend/Renderer/GLES2/CanvasGLES2.hpp>
#include <TGUI/Backend/Renderer/GLES2/BackendRendererGLES2.hpp>

TEST_CASE("[CanvasGLES2]")
{
    if (std::dynamic_pointer_cast<tgui::BackendRendererGLES2>(tgui::getBackend()->getRenderer()))
    {
        tgui::CanvasGLES2::Ptr canvas = tgui::CanvasGLES2::create();
        canvas->getRenderer()->setFont("resources/DejaVuSans.ttf");

        SECTION("WidgetType")
        {
            REQUIRE(canvas->getWidgetType() == "CanvasGLES2");
        }

        SECTION("constructor")
        {
            canvas = tgui::CanvasGLES2::create({200, 100});
            REQUIRE(canvas->getSize() == tgui::Vector2f(200, 100));
        }

        testWidgetRenderer(canvas->getRenderer());

        SECTION("Saving and loading from file")
        {
            REQUIRE_NOTHROW(canvas = tgui::CanvasGLES2::create({60, 40}));

            testSavingWidget("CanvasGLES2", canvas, false);
        }
    }
}
#endif
