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

#include "catch.hpp"
#include <TGUI/Texture.hpp>

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

            REQUIRE(!texture.isLoaded());
            REQUIRE(texture.getId() == "");
            REQUIRE(texture.getData() != nullptr);
            REQUIRE(texture.getData()->image == nullptr);
            REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(0, 0));
            REQUIRE(texture.getData()->rect == sf::IntRect());
            REQUIRE(texture.getSize() == sf::Vector2f(0, 0));
            REQUIRE(texture.getImageSize() == sf::Vector2f(0, 0));
            REQUIRE(texture.getColor() == sf::Color::White);
            REQUIRE(texture.getTextureRect() == sf::FloatRect());
            REQUIRE(texture.getMiddleRect() == sf::IntRect());
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Normal);
        }

        SECTION("Success")
        {
            SECTION("constructor")
            {
                REQUIRE_NOTHROW(tgui::Texture("resources/image.png"));

                tgui::Texture texture{"resources/image.png"};
                REQUIRE(texture.isLoaded());
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->image != nullptr);
                REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(50, 50));
                REQUIRE(texture.getData()->rect == sf::IntRect());
                REQUIRE(texture.getSize() == sf::Vector2f(50, 50));
                REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));
                REQUIRE(texture.getColor() == sf::Color::White);
                REQUIRE(texture.getTextureRect() == sf::FloatRect());
                REQUIRE(texture.getMiddleRect() == sf::IntRect(0, 0, 50, 50));
                REQUIRE(texture.isSmooth() == false);
                REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Normal);
            }

            SECTION("load")
            {
                tgui::Texture texture;
                texture.load("resources/image.png", {10, 5, 40, 30}, {6, 5, 28, 20});
                REQUIRE(texture.isLoaded());
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->image != nullptr);
                REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(40, 30));
                REQUIRE(texture.getData()->rect == sf::IntRect(10, 5, 40, 30));
                REQUIRE(texture.getSize() == sf::Vector2f(40, 30));
                REQUIRE(texture.getImageSize() == sf::Vector2f(40, 30));
                REQUIRE(texture.getColor() == sf::Color::White);
                REQUIRE(texture.getTextureRect() == sf::FloatRect());
                REQUIRE(texture.getMiddleRect() == sf::IntRect(6, 5, 28, 20));
                REQUIRE(texture.isSmooth() == false);
                REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::NineSlice);
            }

            SECTION("setTexture")
            {
                tgui::Texture texture1;
                texture1.load("resources/image.png", {10, 5, 40, 30});

                tgui::Texture texture2;
                REQUIRE(!texture2.isLoaded());

                texture2.setTexture(texture1.getData());
                REQUIRE(texture2.isLoaded());
                REQUIRE(texture2.getId() == "");
                REQUIRE(texture2.getData() != nullptr);
                REQUIRE(texture2.getData()->image != nullptr);
                REQUIRE(texture2.getData()->texture.getSize() == sf::Vector2u(40, 30));
                REQUIRE(texture2.getData()->rect == sf::IntRect(10, 5, 40, 30));
                REQUIRE(texture2.getSize() == sf::Vector2f(40, 30));
                REQUIRE(texture2.getImageSize() == sf::Vector2f(40, 30));
                REQUIRE(texture2.getColor() == sf::Color::White);
                REQUIRE(texture2.getTextureRect() == sf::FloatRect());
                REQUIRE(texture2.getMiddleRect() == sf::IntRect(0, 0, 40, 30));
                REQUIRE(texture2.isSmooth() == false);
                REQUIRE(texture2.getScalingType() == tgui::Texture::ScalingType::Normal);
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
            texture.setSize({200, 100});
            texture.setColor({255, 100, 100, 200});
            texture.setTextureRect({15, 10, 20, 30});
            texture.setSmooth(true);

            REQUIRE(texture.isLoaded());
            REQUIRE(texture.getId() == "resources/image.png");
            REQUIRE(texture.getData() != nullptr);
            REQUIRE(texture.getData()->image != nullptr);
            REQUIRE(texture.getData()->texture.getSize() == sf::Vector2u(50, 50));
            REQUIRE(texture.getData()->rect == sf::IntRect());
            REQUIRE(texture.getSize() == sf::Vector2f(200, 100));
            REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));
            REQUIRE(texture.getColor() == sf::Color(255, 100, 100, 200));
            REQUIRE(texture.getTextureRect() == sf::FloatRect(15, 10, 20, 30));
            REQUIRE(texture.getMiddleRect() == sf::IntRect(10, 0, 30, 50));
            REQUIRE(texture.isSmooth() == true);
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Horizontal);

            REQUIRE(copyCount == 0);
            REQUIRE(destructCount == 0);
            {
                tgui::Texture textureCopy{texture};
                REQUIRE(textureCopy.isLoaded());
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->image != nullptr);
                REQUIRE(textureCopy.getData()->texture.getSize() == sf::Vector2u(50, 50));
                REQUIRE(textureCopy.getData()->rect == sf::IntRect());
                REQUIRE(textureCopy.getSize() == sf::Vector2f(200, 100));
                REQUIRE(textureCopy.getImageSize() == sf::Vector2f(50, 50));
                REQUIRE(textureCopy.getColor() == sf::Color(255, 100, 100, 200));
                REQUIRE(textureCopy.getTextureRect() == sf::FloatRect(15, 10, 20, 30));
                REQUIRE(textureCopy.getMiddleRect() == sf::IntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == true);
                REQUIRE(textureCopy.getScalingType() == tgui::Texture::ScalingType::Horizontal);

                REQUIRE(copyCount == 1);
                REQUIRE(destructCount == 0);
            }
            REQUIRE(copyCount == 1);
            REQUIRE(destructCount == 1);
            {
                tgui::Texture textureCopy;
                REQUIRE(!textureCopy.isLoaded());

                textureCopy = texture;
                REQUIRE(textureCopy.isLoaded());
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->image != nullptr);
                REQUIRE(textureCopy.getData()->texture.getSize() == sf::Vector2u(50, 50));
                REQUIRE(textureCopy.getData()->rect == sf::IntRect());
                REQUIRE(textureCopy.getSize() == sf::Vector2f(200, 100));
                REQUIRE(textureCopy.getImageSize() == sf::Vector2f(50, 50));
                REQUIRE(textureCopy.getColor() == sf::Color(255, 100, 100, 200));
                REQUIRE(textureCopy.getTextureRect() == sf::FloatRect(15, 10, 20, 30));
                REQUIRE(textureCopy.getMiddleRect() == sf::IntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == true);
                REQUIRE(textureCopy.getScalingType() == tgui::Texture::ScalingType::Horizontal);

                REQUIRE(copyCount == 2);
                REQUIRE(destructCount == 1);
            }
            REQUIRE(copyCount == 2);
            REQUIRE(destructCount == 2);
        }
        REQUIRE(copyCount == 2);
        REQUIRE(destructCount == 3);
    }

    SECTION("Size")
    {
        tgui::Texture texture;
        REQUIRE(texture.getSize() == sf::Vector2f());
        REQUIRE(texture.getImageSize() == sf::Vector2f());

        texture.load("resources/image.png");
        REQUIRE(texture.getSize() == sf::Vector2f(50, 50));
        REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));

        texture.setSize({80, 60});
        REQUIRE(texture.getSize() == sf::Vector2f(80, 60));
        REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));
    }

    SECTION("Color")
    {
        tgui::Texture texture{"resources/image.png"};
        REQUIRE(texture.getColor() == sf::Color::White);

        texture.setColor(sf::Color::Blue);
        REQUIRE(texture.getColor() == sf::Color::Blue);
    }

    SECTION("TextureRect")
    {
        tgui::Texture texture;
        REQUIRE(texture.getTextureRect() == sf::FloatRect());

        texture.load("resources/image.png");
        REQUIRE(texture.getTextureRect() == sf::FloatRect());

        texture.setTextureRect({10, 10, 30, 30});
        REQUIRE(texture.getTextureRect() == sf::FloatRect(10, 10, 30, 30));
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

    SECTION("ImageLoader")
    {
        unsigned int count = 0;
        auto oldImageLoader = tgui::Texture::getImageLoader();

        auto func = [&](const sf::String&){ auto image=std::make_shared<sf::Image>(); image->create(1,1); count++; return image; };
        tgui::Texture::setImageLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setImageLoader(oldImageLoader);
    }

    SECTION("TextureLoader")
    {
        unsigned int count = 0;
        auto oldTextureLoader = tgui::Texture::getTextureLoader();

        auto func = [&](tgui::Texture&, const sf::String&, const sf::IntRect&){ count++; return true; };
        tgui::Texture::setTextureLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setTextureLoader(oldTextureLoader);
    }

    SECTION("isTransparentPixel")
    {
        tgui::Texture texture;
        texture.setPosition({10, 20});

        SECTION("Invalid image")
        {
            //Size==0 and not loaded yet
        }

        SECTION("Normal Scaling")
        {
            texture.load("resources/TransparentParts.png", {10, 10, 30, 30});
            texture.setSize({60, 15});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Normal);

            REQUIRE(!texture.isTransparentPixel(21, 22.5f));
            REQUIRE(texture.isTransparentPixel(22, 23));
            REQUIRE(!texture.isTransparentPixel(21, 27.5f));
            REQUIRE(texture.isTransparentPixel(22, 27.5f));
            REQUIRE(!texture.isTransparentPixel(21, 32));
            REQUIRE(texture.isTransparentPixel(22, 31.5f));
            REQUIRE(!texture.isTransparentPixel(40, 32));
            REQUIRE(texture.isTransparentPixel(40, 31.5f));
            REQUIRE(!texture.isTransparentPixel(58, 32));
            REQUIRE(texture.isTransparentPixel(57, 31.5f));
            REQUIRE(!texture.isTransparentPixel(58, 27.5f));
            REQUIRE(texture.isTransparentPixel(57, 27.5f));
            REQUIRE(!texture.isTransparentPixel(58, 22.5f));
            REQUIRE(texture.isTransparentPixel(57, 23));
            REQUIRE(!texture.isTransparentPixel(40, 22.5f));
            REQUIRE(texture.isTransparentPixel(40, 23));
            REQUIRE(texture.isTransparentPixel(40, 27.5f));

            // Corner pixels
            REQUIRE(!texture.isTransparentPixel(10, 20));
            REQUIRE(!texture.isTransparentPixel(10, 34.5f));
            REQUIRE(!texture.isTransparentPixel(69, 34.5f));
            REQUIRE(!texture.isTransparentPixel(69, 20));
        }

        SECTION("Horizontal Scaling")
        {
            texture.load("resources/TransparentParts.png", {0, 10, 50, 30}, {10, 0, 30, 30});
            texture.setSize({70, 15});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Horizontal);

            // Left part
            REQUIRE(!texture.isTransparentPixel(10.5f, 24.5f));
            REQUIRE(texture.isTransparentPixel(11, 25));
            REQUIRE(!texture.isTransparentPixel(10.5f, 27));
            REQUIRE(texture.isTransparentPixel(11, 27));
            REQUIRE(!texture.isTransparentPixel(10.5f, 30));
            REQUIRE(texture.isTransparentPixel(11, 29.5f));
            REQUIRE(!texture.isTransparentPixel(12, 30));
            REQUIRE(texture.isTransparentPixel(12, 29.5f));
            REQUIRE(!texture.isTransparentPixel(14, 30));
            REQUIRE(texture.isTransparentPixel(13.5f, 29.5f));
            REQUIRE(!texture.isTransparentPixel(14, 27));
            REQUIRE(texture.isTransparentPixel(13.5f, 27));
            REQUIRE(!texture.isTransparentPixel(14, 24.5));
            REQUIRE(texture.isTransparentPixel(13.5f, 25));
            REQUIRE(!texture.isTransparentPixel(12, 24.5f));
            REQUIRE(texture.isTransparentPixel(12, 25));
            REQUIRE(texture.isTransparentPixel(12, 27));

            // Middle part
            REQUIRE(!texture.isTransparentPixel(26, 22.5f));
            REQUIRE(texture.isTransparentPixel(27, 23));
            REQUIRE(!texture.isTransparentPixel(26, 27.5f));
            REQUIRE(texture.isTransparentPixel(27, 27.5f));
            REQUIRE(!texture.isTransparentPixel(26, 32));
            REQUIRE(texture.isTransparentPixel(27, 31.5f));
            REQUIRE(!texture.isTransparentPixel(44, 32));
            REQUIRE(texture.isTransparentPixel(44, 31.5f));
            REQUIRE(!texture.isTransparentPixel(63, 32));
            REQUIRE(texture.isTransparentPixel(62, 31.5f));
            REQUIRE(!texture.isTransparentPixel(63, 27.5f));
            REQUIRE(texture.isTransparentPixel(62, 27.5f));
            REQUIRE(!texture.isTransparentPixel(63, 22.5f));
            REQUIRE(texture.isTransparentPixel(62, 23));
            REQUIRE(!texture.isTransparentPixel(44, 22.5f));
            REQUIRE(texture.isTransparentPixel(44, 23));
            REQUIRE(texture.isTransparentPixel(44, 27.5f));

            // Right part
            REQUIRE(!texture.isTransparentPixel(75.5f, 24.5f));
            REQUIRE(texture.isTransparentPixel(76, 25));
            REQUIRE(!texture.isTransparentPixel(75.5f, 27));
            REQUIRE(texture.isTransparentPixel(76, 27));
            REQUIRE(!texture.isTransparentPixel(75.5f, 30));
            REQUIRE(texture.isTransparentPixel(76, 29.5f));
            REQUIRE(!texture.isTransparentPixel(77, 30));
            REQUIRE(texture.isTransparentPixel(77, 29.5f));
            REQUIRE(!texture.isTransparentPixel(79, 30));
            REQUIRE(texture.isTransparentPixel(78.5f, 29.5f));
            REQUIRE(!texture.isTransparentPixel(79, 27));
            REQUIRE(texture.isTransparentPixel(78.5f, 27));
            REQUIRE(!texture.isTransparentPixel(79, 24.5));
            REQUIRE(texture.isTransparentPixel(78.5f, 25));
            REQUIRE(!texture.isTransparentPixel(77, 24.5f));
            REQUIRE(texture.isTransparentPixel(77, 25));
            REQUIRE(texture.isTransparentPixel(77, 27));

            // Corner pixels
            REQUIRE(!texture.isTransparentPixel(10, 20));
            REQUIRE(!texture.isTransparentPixel(10, 34.5f));
            REQUIRE(!texture.isTransparentPixel(79.5f, 34.5f));
            REQUIRE(!texture.isTransparentPixel(79.5f, 20));
        }

        SECTION("Vertical Scaling")
        {
            texture.load("resources/TransparentParts.png", {10, 0, 30, 50}, {0, 10, 30, 30});
            texture.setSize({15, 70});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Vertical);

            // Top part
            REQUIRE(!texture.isTransparentPixel(14.5f, 20.5f));
            REQUIRE(texture.isTransparentPixel(15, 21));
            REQUIRE(!texture.isTransparentPixel(17, 20.5f));
            REQUIRE(texture.isTransparentPixel(17, 21));
            REQUIRE(!texture.isTransparentPixel(20, 20.5f));
            REQUIRE(texture.isTransparentPixel(19.5f, 21));
            REQUIRE(!texture.isTransparentPixel(20, 22));
            REQUIRE(texture.isTransparentPixel(19.5f, 22));
            REQUIRE(!texture.isTransparentPixel(20, 24));
            REQUIRE(texture.isTransparentPixel(19.5f, 23.5f));
            REQUIRE(!texture.isTransparentPixel(17, 24));
            REQUIRE(texture.isTransparentPixel(17, 23.5f));
            REQUIRE(!texture.isTransparentPixel(14.5, 24));
            REQUIRE(texture.isTransparentPixel(15, 23.5f));
            REQUIRE(!texture.isTransparentPixel(14.5f, 22));
            REQUIRE(texture.isTransparentPixel(15, 22));
            REQUIRE(texture.isTransparentPixel(17, 22));

            // Middle part
            REQUIRE(!texture.isTransparentPixel(12.5f, 36));
            REQUIRE(texture.isTransparentPixel(13, 37));
            REQUIRE(!texture.isTransparentPixel(17.5f, 36));
            REQUIRE(texture.isTransparentPixel(17.5f, 37));
            REQUIRE(!texture.isTransparentPixel(22, 36));
            REQUIRE(texture.isTransparentPixel(21.5f, 37));
            REQUIRE(!texture.isTransparentPixel(22, 54));
            REQUIRE(texture.isTransparentPixel(21.5f, 54));
            REQUIRE(!texture.isTransparentPixel(22, 73));
            REQUIRE(texture.isTransparentPixel(21.5f, 72));
            REQUIRE(!texture.isTransparentPixel(17.5f, 73));
            REQUIRE(texture.isTransparentPixel(17.5f, 72));
            REQUIRE(!texture.isTransparentPixel(12.5f, 73));
            REQUIRE(texture.isTransparentPixel(13, 72));
            REQUIRE(!texture.isTransparentPixel(12.5f, 54));
            REQUIRE(texture.isTransparentPixel(13, 54));
            REQUIRE(texture.isTransparentPixel(17.5f, 54));

            // Bottom part
            REQUIRE(!texture.isTransparentPixel(14.5f, 85.5f));
            REQUIRE(texture.isTransparentPixel(15, 86));
            REQUIRE(!texture.isTransparentPixel(17, 85.5f));
            REQUIRE(texture.isTransparentPixel(17, 86));
            REQUIRE(!texture.isTransparentPixel(20, 85.5f));
            REQUIRE(texture.isTransparentPixel(19.5f, 86));
            REQUIRE(!texture.isTransparentPixel(20, 87));
            REQUIRE(texture.isTransparentPixel(19.5f, 87));
            REQUIRE(!texture.isTransparentPixel(20, 89));
            REQUIRE(texture.isTransparentPixel(19.5f, 88.5f));
            REQUIRE(!texture.isTransparentPixel(17, 89));
            REQUIRE(texture.isTransparentPixel(17, 88.5f));
            REQUIRE(!texture.isTransparentPixel(14.5, 89));
            REQUIRE(texture.isTransparentPixel(15, 88.5f));
            REQUIRE(!texture.isTransparentPixel(14.5f, 87));
            REQUIRE(texture.isTransparentPixel(15, 87));
            REQUIRE(texture.isTransparentPixel(17, 87));

            // Corner pixels
            REQUIRE(!texture.isTransparentPixel(10, 20));
            REQUIRE(!texture.isTransparentPixel(24.5f, 20));
            REQUIRE(!texture.isTransparentPixel(24.5f, 89.5f));
            REQUIRE(!texture.isTransparentPixel(10, 89.5f));
        }

        SECTION("9-Slice Scaling")
        {
            texture.load("resources/TransparentParts.png", {}, {10, 10, 30, 30});
            texture.setSize({80, 35});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::NineSlice);

            // Top left part
            REQUIRE(!texture.isTransparentPixel(11, 21));
            REQUIRE(texture.isTransparentPixel(12, 22));
            REQUIRE(!texture.isTransparentPixel(11, 23.5f));
            REQUIRE(texture.isTransparentPixel(12, 23.5f));
            REQUIRE(!texture.isTransparentPixel(11, 26));
            REQUIRE(texture.isTransparentPixel(12, 25));
            REQUIRE(!texture.isTransparentPixel(13.5f, 26));
            REQUIRE(texture.isTransparentPixel(13.5f, 25));
            REQUIRE(!texture.isTransparentPixel(16, 26));
            REQUIRE(texture.isTransparentPixel(15, 25));
            REQUIRE(!texture.isTransparentPixel(16, 23.5f));
            REQUIRE(texture.isTransparentPixel(15, 23.5f));
            REQUIRE(!texture.isTransparentPixel(16, 21));
            REQUIRE(texture.isTransparentPixel(15, 22));
            REQUIRE(!texture.isTransparentPixel(13.5f, 21));
            REQUIRE(texture.isTransparentPixel(13.5f, 22));
            REQUIRE(texture.isTransparentPixel(13.5f, 23.5f));

            // Top right part
            REQUIRE(!texture.isTransparentPixel(88, 21));
            REQUIRE(texture.isTransparentPixel(87, 22));
            REQUIRE(!texture.isTransparentPixel(88, 23.5f));
            REQUIRE(texture.isTransparentPixel(87, 23.5f));
            REQUIRE(!texture.isTransparentPixel(88, 26));
            REQUIRE(texture.isTransparentPixel(87, 25));
            REQUIRE(!texture.isTransparentPixel(86.5f, 26));
            REQUIRE(texture.isTransparentPixel(86.5f, 25));
            REQUIRE(!texture.isTransparentPixel(83, 26));
            REQUIRE(texture.isTransparentPixel(84, 25));
            REQUIRE(!texture.isTransparentPixel(83, 23.5f));
            REQUIRE(texture.isTransparentPixel(84, 23.5f));
            REQUIRE(!texture.isTransparentPixel(83, 21));
            REQUIRE(texture.isTransparentPixel(84, 22));
            REQUIRE(!texture.isTransparentPixel(86.5f, 21));
            REQUIRE(texture.isTransparentPixel(86.5f, 22));
            REQUIRE(texture.isTransparentPixel(86.5f, 23.5f));

            // Bottom left part
            REQUIRE(!texture.isTransparentPixel(11, 53));
            REQUIRE(texture.isTransparentPixel(12, 52));
            REQUIRE(!texture.isTransparentPixel(11, 50.5f));
            REQUIRE(texture.isTransparentPixel(12, 50.5f));
            REQUIRE(!texture.isTransparentPixel(11, 48));
            REQUIRE(texture.isTransparentPixel(12, 49));
            REQUIRE(!texture.isTransparentPixel(13.5f, 48));
            REQUIRE(texture.isTransparentPixel(13.5f, 49));
            REQUIRE(!texture.isTransparentPixel(16, 48));
            REQUIRE(texture.isTransparentPixel(15, 49));
            REQUIRE(!texture.isTransparentPixel(16, 50.5f));
            REQUIRE(texture.isTransparentPixel(15, 50.5f));
            REQUIRE(!texture.isTransparentPixel(16, 53));
            REQUIRE(texture.isTransparentPixel(15, 52));
            REQUIRE(!texture.isTransparentPixel(13.5f, 53));
            REQUIRE(texture.isTransparentPixel(13.5f, 52));
            REQUIRE(texture.isTransparentPixel(13.5f, 50.5f));

            // Bottom right part
            REQUIRE(!texture.isTransparentPixel(88, 53));
            REQUIRE(texture.isTransparentPixel(87, 52));
            REQUIRE(!texture.isTransparentPixel(88, 50.5f));
            REQUIRE(texture.isTransparentPixel(87, 50.5f));
            REQUIRE(!texture.isTransparentPixel(88, 48));
            REQUIRE(texture.isTransparentPixel(87, 49));
            REQUIRE(!texture.isTransparentPixel(86.5f, 48));
            REQUIRE(texture.isTransparentPixel(86.5f, 49));
            REQUIRE(!texture.isTransparentPixel(83, 48));
            REQUIRE(texture.isTransparentPixel(84, 49));
            REQUIRE(!texture.isTransparentPixel(83, 50.5f));
            REQUIRE(texture.isTransparentPixel(84, 50.5f));
            REQUIRE(!texture.isTransparentPixel(83, 53));
            REQUIRE(texture.isTransparentPixel(84, 52));
            REQUIRE(!texture.isTransparentPixel(86.5f, 53));
            REQUIRE(texture.isTransparentPixel(86.5f, 52));
            REQUIRE(texture.isTransparentPixel(86.5f, 50.5f));

            // Left part
            REQUIRE(!texture.isTransparentPixel(11, 34.5f));
            REQUIRE(texture.isTransparentPixel(12, 35));
            REQUIRE(!texture.isTransparentPixel(11, 37));
            REQUIRE(texture.isTransparentPixel(12, 37));
            REQUIRE(!texture.isTransparentPixel(11, 40));
            REQUIRE(texture.isTransparentPixel(12, 39.5f));
            REQUIRE(!texture.isTransparentPixel(14.5f, 40));
            REQUIRE(texture.isTransparentPixel(14.5f, 39.5f));
            REQUIRE(!texture.isTransparentPixel(18, 40));
            REQUIRE(texture.isTransparentPixel(17, 39.5f));
            REQUIRE(!texture.isTransparentPixel(18, 37));
            REQUIRE(texture.isTransparentPixel(17, 37));
            REQUIRE(!texture.isTransparentPixel(18, 34.5f));
            REQUIRE(texture.isTransparentPixel(17, 35));
            REQUIRE(!texture.isTransparentPixel(14.5f, 34.5f));
            REQUIRE(texture.isTransparentPixel(14.5f, 35));
            REQUIRE(texture.isTransparentPixel(14.5f, 37));

            // Right part
            REQUIRE(!texture.isTransparentPixel(81, 34.5f));
            REQUIRE(texture.isTransparentPixel(82, 35));
            REQUIRE(!texture.isTransparentPixel(81, 37));
            REQUIRE(texture.isTransparentPixel(82, 37));
            REQUIRE(!texture.isTransparentPixel(81, 40));
            REQUIRE(texture.isTransparentPixel(82, 39.5f));
            REQUIRE(!texture.isTransparentPixel(84.5f, 40));
            REQUIRE(texture.isTransparentPixel(84.5f, 39.5f));
            REQUIRE(!texture.isTransparentPixel(88, 40));
            REQUIRE(texture.isTransparentPixel(87, 39.5f));
            REQUIRE(!texture.isTransparentPixel(88, 37));
            REQUIRE(texture.isTransparentPixel(87, 37));
            REQUIRE(!texture.isTransparentPixel(88, 34.5f));
            REQUIRE(texture.isTransparentPixel(87, 35));
            REQUIRE(!texture.isTransparentPixel(84.5f, 34.5f));
            REQUIRE(texture.isTransparentPixel(84.5f, 35));
            REQUIRE(texture.isTransparentPixel(84.5f, 37));

            // Top part
            REQUIRE(!texture.isTransparentPixel(39, 21));
            REQUIRE(texture.isTransparentPixel(40, 22));
            REQUIRE(!texture.isTransparentPixel(39, 24.5f));
            REQUIRE(texture.isTransparentPixel(40, 24.5f));
            REQUIRE(!texture.isTransparentPixel(39, 28));
            REQUIRE(texture.isTransparentPixel(40, 27));
            REQUIRE(!texture.isTransparentPixel(49, 28));
            REQUIRE(texture.isTransparentPixel(49, 27));
            REQUIRE(!texture.isTransparentPixel(60, 28));
            REQUIRE(texture.isTransparentPixel(59, 27));
            REQUIRE(!texture.isTransparentPixel(60, 24.5f));
            REQUIRE(texture.isTransparentPixel(59, 24.5f));
            REQUIRE(!texture.isTransparentPixel(60, 21));
            REQUIRE(texture.isTransparentPixel(59, 22));
            REQUIRE(!texture.isTransparentPixel(49, 21));
            REQUIRE(texture.isTransparentPixel(49, 22));
            REQUIRE(texture.isTransparentPixel(49, 24.5f));

            // Bottom part
            REQUIRE(!texture.isTransparentPixel(39, 46));
            REQUIRE(texture.isTransparentPixel(40, 47));
            REQUIRE(!texture.isTransparentPixel(39, 49.5f));
            REQUIRE(texture.isTransparentPixel(40, 49.5f));
            REQUIRE(!texture.isTransparentPixel(39, 53));
            REQUIRE(texture.isTransparentPixel(40, 52));
            REQUIRE(!texture.isTransparentPixel(49, 53));
            REQUIRE(texture.isTransparentPixel(49, 52));
            REQUIRE(!texture.isTransparentPixel(60, 53));
            REQUIRE(texture.isTransparentPixel(59, 52));
            REQUIRE(!texture.isTransparentPixel(60, 49.5f));
            REQUIRE(texture.isTransparentPixel(59, 49.5f));
            REQUIRE(!texture.isTransparentPixel(60, 46));
            REQUIRE(texture.isTransparentPixel(59, 47));
            REQUIRE(!texture.isTransparentPixel(49, 46));
            REQUIRE(texture.isTransparentPixel(49, 47));
            REQUIRE(texture.isTransparentPixel(49, 49.5f));

            // Middle part
            REQUIRE(!texture.isTransparentPixel(31, 32.5f));
            REQUIRE(texture.isTransparentPixel(32, 33));
            REQUIRE(!texture.isTransparentPixel(31, 37.5f));
            REQUIRE(texture.isTransparentPixel(32, 37.5f));
            REQUIRE(!texture.isTransparentPixel(31, 42));
            REQUIRE(texture.isTransparentPixel(32, 41.5f));
            REQUIRE(!texture.isTransparentPixel(49.5f, 42));
            REQUIRE(texture.isTransparentPixel(49.5f, 41.5f));
            REQUIRE(!texture.isTransparentPixel(68, 42));
            REQUIRE(texture.isTransparentPixel(67, 41.5f));
            REQUIRE(!texture.isTransparentPixel(68, 37.5f));
            REQUIRE(texture.isTransparentPixel(67, 37.5f));
            REQUIRE(!texture.isTransparentPixel(68, 32.5f));
            REQUIRE(texture.isTransparentPixel(67, 33));
            REQUIRE(!texture.isTransparentPixel(49.5f, 32.5f));
            REQUIRE(texture.isTransparentPixel(49.5f, 33));
            REQUIRE(texture.isTransparentPixel(49.5f, 37.5f));

            // Corner pixels
            REQUIRE(!texture.isTransparentPixel(10, 20));
            REQUIRE(!texture.isTransparentPixel(10, 54.5f));
            REQUIRE(!texture.isTransparentPixel(89, 54.5f));
            REQUIRE(!texture.isTransparentPixel(89, 20));
        }
    }

    SECTION("Downgrading scaling type")
    {
        SECTION("Horizontal")
        {
            tgui::Texture texture{"resources/image.png", {}, {10, 0, 30, 50}};
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Horizontal);

            texture.setSize({20, 60});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Normal);

            texture.setSize({100, 30});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Horizontal);
        }

        SECTION("Vertical")
        {
            tgui::Texture texture{"resources/image.png", {}, {0, 5, 50, 40}};
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Vertical);

            texture.setSize({110, 20});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Normal);

            texture.setSize({300, 100});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Vertical);
        }

        SECTION("9-Slice")
        {
            tgui::Texture texture{"resources/image.png", {}, {10, 5, 30, 40}};
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::NineSlice);

            texture.setSize({9, 1});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Normal);

            texture.setSize({20, 9});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Horizontal);

            texture.setSize({19, 10});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::Vertical);

            texture.setSize({20, 10});
            REQUIRE(texture.getScalingType() == tgui::Texture::ScalingType::NineSlice);
        }
    }
}
