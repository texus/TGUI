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
#include <TGUI/Exception.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/TextureManager.hpp>
#include <SFML/System/Err.hpp>

TEST_CASE("[Texture]")
{
    SECTION("Loading")
    {
        SECTION("Failure")
        {
            tgui::Texture texture;

            SECTION("No image")
            {
                REQUIRE_NOTHROW(tgui::Texture());
            }
            SECTION("Image not found")
            {
                std::streambuf *oldbuf = sf::err().rdbuf(0);
                REQUIRE_THROWS_AS(tgui::Texture("NonExistent.png"), tgui::Exception);
                REQUIRE_THROWS_AS(texture.load("NonExistent.png"), tgui::Exception);
                sf::err().rdbuf(oldbuf);
            }

            REQUIRE(texture.getId() == "");
            REQUIRE(texture.getData() == nullptr);
            REQUIRE(texture.getImageSize() == sf::Vector2f(0, 0));
            REQUIRE(texture.getMiddleRect() == sf::IntRect());
        }

        SECTION("Success")
        {
            SECTION("constructor")
            {
                REQUIRE_NOTHROW(tgui::Texture("resources/image.png"));

                tgui::Texture texture{"resources/image.png"};
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->image != nullptr);
                REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(50, 50));
                REQUIRE(texture.getData()->rect == sf::IntRect());
                REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));
                REQUIRE(texture.getMiddleRect() == sf::IntRect(0, 0, 50, 50));
                REQUIRE(texture.isSmooth() == false);
            }

            SECTION("load")
            {
                tgui::Texture texture;
                texture.load("resources/image.png", {10, 5, 40, 30}, {6, 5, 28, 20});
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->image != nullptr);
                REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(40, 30));
                REQUIRE(texture.getData()->rect == sf::IntRect(10, 5, 40, 30));
                REQUIRE(texture.getImageSize() == sf::Vector2f(40, 30));
                REQUIRE(texture.getMiddleRect() == sf::IntRect(6, 5, 28, 20));
                REQUIRE(texture.isSmooth() == false);
            }
        }
    }

    SECTION("Copy and destruct")
    {
        unsigned int copyCount = 0;
        unsigned int destructCount = 0;
        {
            tgui::Texture texture{"resources/image.png", {}, {10, 0, 30, 50}};
            texture.setCopyCallback([&](std::shared_ptr<tgui::TextureData> data)
                {
                    copyCount++;
                    tgui::TextureManager::copyTexture(data);
                }
            );
            texture.setDestructCallback([&](std::shared_ptr<tgui::TextureData> data)
                {
                    destructCount++;
                    tgui::TextureManager::removeTexture(data);
                }
            );
            texture.setSmooth(true);

            REQUIRE(texture.getId() == "resources/image.png");
            REQUIRE(texture.getData() != nullptr);
            REQUIRE(texture.getData()->image != nullptr);
            REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(50, 50));
            REQUIRE(texture.getData()->rect == sf::IntRect());
            REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));
            REQUIRE(texture.getMiddleRect() == sf::IntRect(10, 0, 30, 50));
            REQUIRE(texture.isSmooth() == true);

            REQUIRE(copyCount == 0);
            REQUIRE(destructCount == 0);
            {
                tgui::Texture textureCopy{texture};
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->image != nullptr);
                REQUIRE(textureCopy.getData()->texture.getSize() == sf::Vector2u(50, 50));
                REQUIRE(textureCopy.getData()->rect == sf::IntRect());
                REQUIRE(textureCopy.getImageSize() == sf::Vector2f(50, 50));
                REQUIRE(textureCopy.getMiddleRect() == sf::IntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == true);

                REQUIRE(copyCount == 1);
                REQUIRE(destructCount == 0);
            }
            REQUIRE(copyCount == 1);
            REQUIRE(destructCount == 1);
            {
                tgui::Texture textureCopy;
                REQUIRE(textureCopy.getData() == nullptr);

                textureCopy = texture;
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->image != nullptr);
                REQUIRE(textureCopy.getData()->texture.getSize() == sf::Vector2u(50, 50));
                REQUIRE(textureCopy.getData()->rect == sf::IntRect());
                REQUIRE(textureCopy.getImageSize() == sf::Vector2f(50, 50));
                REQUIRE(textureCopy.getMiddleRect() == sf::IntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == true);

                REQUIRE(copyCount == 2);
                REQUIRE(destructCount == 1);
            }
            REQUIRE(copyCount == 2);
            REQUIRE(destructCount == 2);
        }
        REQUIRE(copyCount == 2);
        REQUIRE(destructCount == 3);
    }

    SECTION("MiddleRect")
    {
        tgui::Texture texture;
        REQUIRE(texture.getMiddleRect() == sf::IntRect());

        texture.load("resources/image.png");
        REQUIRE(texture.getMiddleRect() == sf::IntRect(0, 0, 50, 50));
    }

    SECTION("Smooth")
    {
        tgui::Texture texture{"resources/image.png"};
        REQUIRE(!texture.isSmooth());

        texture.setSmooth(true);
        REQUIRE(texture.isSmooth());

        texture.setSmooth(false);
        REQUIRE(!texture.isSmooth());
    }

    SECTION("Shader")
    {
        tgui::Texture texture{"resources/image.png"};
        REQUIRE(!texture.getShader());

        sf::Shader shader;
        texture.setShader(&shader);
        REQUIRE(texture.getShader() == &shader);

        texture.setShader(nullptr);
        REQUIRE(!texture.getShader());
    }

    SECTION("ImageLoader")
    {
        unsigned int count = 0;
        auto oldImageLoader = tgui::Texture::getImageLoader();

        auto func = [&](const sf::String&){ auto image=std::make_unique<sf::Image>(); image->create(1,1); count++; return image; };
        tgui::Texture::setImageLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setImageLoader(oldImageLoader);
    }

    SECTION("TextureLoader")
    {
        unsigned int count = 0;
        auto oldTextureLoader = tgui::Texture::getTextureLoader();

        auto func = [&](tgui::Texture&, const sf::String&, const sf::IntRect&){ count++; return std::make_shared<tgui::TextureData>(); };
        tgui::Texture::setTextureLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setTextureLoader(oldTextureLoader);
    }
}
