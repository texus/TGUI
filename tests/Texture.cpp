/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Backend/Window/Backend.hpp>

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #include <SFML/Graphics/Shader.hpp>
    #include <TGUI/Backend/Renderer/SFML-Graphics/BackendRendererSFML.hpp>
#endif

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
                REQUIRE_THROWS_AS(tgui::Texture("NonExistent.png"), tgui::Exception);
                REQUIRE_THROWS_AS(texture.load("NonExistent.png"), tgui::Exception);
            }
            SECTION("loadFromBase64")
            {
                REQUIRE_THROWS_AS(texture.loadFromBase64("aW52YWxpZA=="), tgui::Exception);
            }

            REQUIRE(texture.getId() == "");
            REQUIRE(texture.getData() == nullptr);
            REQUIRE(texture.getImageSize() == tgui::Vector2u(0, 0));
            REQUIRE(texture.getMiddleRect() == tgui::UIntRect());
        }

        SECTION("Success")
        {
            SECTION("constructor")
            {
                REQUIRE_NOTHROW(tgui::Texture("resources/image.png"));
                REQUIRE_NOTHROW(tgui::Texture("resources/image.bmp"));
                REQUIRE_NOTHROW(tgui::Texture("resources/image.jpg"));

                tgui::Texture texture{"resources/image.png"};
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->backendTexture != nullptr);
                REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(texture.getImageSize() == tgui::Vector2u(50, 50));
                REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(texture.isSmooth() == true);
            }

            SECTION("load")
            {
                tgui::Texture texture;
                REQUIRE_NOTHROW(texture.load("resources/image.png", {10, 5, 40, 30}, {6, 5, 28, 20}));
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->backendTexture != nullptr);
                REQUIRE(texture.getPartRect() == tgui::UIntRect(10, 5, 40, 30));
                REQUIRE(texture.getImageSize() == tgui::Vector2u(40, 30));
                REQUIRE(texture.getMiddleRect() == tgui::UIntRect(6, 5, 28, 20));
                REQUIRE(texture.isSmooth() == true);
            }
        }

        SECTION("loadFromBase64")
        {
            tgui::Texture texture;
            texture.loadFromBase64("iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAABhWlDQ1BJQ0MgcHJvZmlsZQAAKJF9kT1Iw0AcxV9bpSI"
                "VB+sH4pChumhBVMRRq1CECqFWaNXB5NIvaNKQpLg4Cq4FBz8Wqw4uzro6uAqC4AeIm5uToouU+L+k0CLGg+N+vLv3uHsH+Gslpppt44CqWUYyH"
                "hPSmVUh+IoQ+tGLPoxKzNTnRDEBz/F1Dx9f76I8y/vcn6NLyZoM8AnEs0w3LOIN4ulNS+e8TxxmBUkhPiceM+iCxI9cl11+45x32M8zw0YqOU8"
                "cJhbyLSy3MCsYKvEUcURRNcr3p11WOG9xVksV1rgnf2Eoq60sc53mEOJYxBJECJBRQRElWIjSqpFiIkn7MQ//oOMXySWTqwhGjgWUoUJy/OB/8"
                "LtbMzc54SaFYkD7i21/DAPBXaBete3vY9uunwCBZ+BKa/rLNWDmk/RqU4scAd3bwMV1U5P3gMsdYOBJlwzJkQI0/bkc8H5G35QBem6BzjW3t8Y"
                "+Th+AFHWVuAEODoGRPGWve7y7o7W3f880+vsBmyVyt7MJ+VMAAAAGYktHRACTALMA7AFBCKUAAAAJcEhZcwAALiMAAC4jAXilP3YAAAAHdElNR"
                "QflDBcVMRvUtg1zAAACG0lEQVQ4y63VzY8MYRDH8U/3zOxkmMFgDYkQEsGGuIoDN/4B4eJPEyd/ghMHR3GSiJcsDoi16exKs73LvOxuO2zNpnX"
                "Gy0EllZ48/dS363l+VTWJGdbrDxIcQR89zMWrEdbxBatFnpX12GQG7ChO4DzS2t4y/Cs+4l2RZ2szgb3+oIlTAbtA0qdszQBOkGMZq3iDpWm2z"
                "UrAKVzB4R0v57EnIFMY/EAbDXRibRhwjcoxz+A0jmGAeRzEAewL34tWJNLAFjbRnut0v46HG6NmCBDHnGZnPgR5jAd4Fplcwg1crhx/E0nCt15"
                "/8KIZai4EYB6H4ve9Is/u1jR7iqe9/uAObkWGI6xhP9ppBKdxlD3o4vEM2K4VeXYfT2J/q1INrTQA0wsv4+UDf7eHtThopqFY3Z79A/DljPJrp"
                "P6PTQteinGtY7ZDzb/ZQi3DBNspioBMomjXcf0fgFdj/yTiYdJod7pJFPK0+hs42+500/Fw4/lvhsfNAGbRIdv4jOXmzkLygXIuYK24j9u9/uA"
                "4HlUEWCC5RnkpQCuR5SusF3k2bBZ5Vvb6g3fRYlPFx/HVi+GVuyp/xGBYCeingL7Z7eXxcGPU7nQnkd1Wpa2KGFVVzwP0He9jbbHIs1F92izF1"
                "DgX82Wtol69PFp4HdDFeP52wO7DyejNXUryK3Qc1fC2yLPNP07syl9AO0Z/M+q1lNhW2sSkyLPhrNifn2O2xjCyiUUAAAAASUVORK5CYII=",
                {2, 1, 6, 5}, {1, 2, 4, 1}, false);
            REQUIRE(texture.getId() == "");
            REQUIRE(texture.getData() != nullptr);
            REQUIRE(texture.getData()->backendTexture != nullptr);
            REQUIRE(texture.getPartRect() == tgui::UIntRect(2, 1, 6, 5));
            REQUIRE(texture.getImageSize() == tgui::Vector2u(6, 5));
            REQUIRE(texture.getMiddleRect() == tgui::UIntRect(1, 2, 4, 1));
            REQUIRE(texture.isSmooth() == false);
        }
    }

    SECTION("Copy and destruct")
    {
        unsigned int copyCount = 0;
        unsigned int destructCount = 0;
        {
            tgui::Texture texture{"resources/image.png", {}, {10, 0, 30, 50}, false};
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

            REQUIRE(texture.getId() == "resources/image.png");
            REQUIRE(texture.getData() != nullptr);
            REQUIRE(texture.getData()->backendTexture != nullptr);
            REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
            REQUIRE(texture.getImageSize() == tgui::Vector2u(50, 50));
            REQUIRE(texture.getMiddleRect() == tgui::UIntRect(10, 0, 30, 50));
            REQUIRE(texture.isSmooth() == false);

            REQUIRE(copyCount == 0);
            REQUIRE(destructCount == 0);
            {
                tgui::Texture textureCopy{texture};
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->backendTexture != nullptr);
                REQUIRE(textureCopy.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(textureCopy.getImageSize() == tgui::Vector2u(50, 50));
                REQUIRE(textureCopy.getMiddleRect() == tgui::UIntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == false);

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
                REQUIRE(textureCopy.getData()->backendTexture != nullptr);
                REQUIRE(textureCopy.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(textureCopy.getImageSize() == tgui::Vector2u(50, 50));
                REQUIRE(textureCopy.getMiddleRect() == tgui::UIntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == false);

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
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect());

        texture.load("resources/image.png");
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 50, 50));
    }

    SECTION("Smooth")
    {
        // Textures are smoothed by default
        REQUIRE(tgui::Texture::getDefaultSmooth());
        REQUIRE(tgui::Texture("resources/image.png").isSmooth());

        tgui::Texture::setDefaultSmooth(false);
        REQUIRE(!tgui::Texture::getDefaultSmooth());
        REQUIRE(!tgui::Texture("resources/image.png").isSmooth());

        tgui::Texture::setDefaultSmooth(true);
        REQUIRE(tgui::Texture::getDefaultSmooth());
        REQUIRE(tgui::Texture("resources/image.png").isSmooth());
    }

    SECTION("Color")
    {
        tgui::Texture texture{"resources/image.png"};
        REQUIRE(texture.getColor() == tgui::Color::White);

        texture.setColor("red");
        REQUIRE(texture.getColor() == tgui::Color::Red);
    }

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer()))
    {
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
    }
#endif

    SECTION("BackendTextureLoader")
    {
        unsigned int count = 0;
        auto oldBackendTextureLoader = tgui::Texture::getBackendTextureLoader();

        auto func = [&](tgui::BackendTexture&, const tgui::String& filename, bool) {
            REQUIRE(filename == "resources/image.png");
            count++;
            return true;
        };

        tgui::Texture::setBackendTextureLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setBackendTextureLoader(oldBackendTextureLoader);
    }

    SECTION("TextureLoader")
    {
        unsigned int count = 0;
        auto oldTextureLoader = tgui::Texture::getTextureLoader();

        auto func = [&](tgui::Texture& texture, const tgui::String& filename, bool smooth) {
            REQUIRE(filename == "resources/image.png");
            auto data = std::make_shared<tgui::TextureData>();
            data->backendTexture = std::make_shared<tgui::BackendTexture>();
            texture.getBackendTextureLoader()(*data->backendTexture, filename, smooth);
            count++;
            return data;
        };

        tgui::Texture::setTextureLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setTextureLoader(oldTextureLoader);
    }
}
