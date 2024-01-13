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

#include "Tests.hpp"

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/TextureManager.hpp>
#endif

TEST_CASE("[TextureManager]")
{
    tgui::Texture texture;
    REQUIRE(tgui::TextureManager::getTexture(texture, "NonExistent.png", true) == nullptr);

    tgui::Texture texture1;
    tgui::Texture texture2;
    tgui::Texture texture3;
    std::shared_ptr<tgui::TextureData> textureData1 = tgui::TextureManager::getTexture(texture1, "resources/image.png", true);
    std::shared_ptr<tgui::TextureData> textureData2 = tgui::TextureManager::getTexture(texture2, "resources/image.png", true);
    std::shared_ptr<tgui::TextureData> textureData3 = tgui::TextureManager::getTexture(texture3, "resources/image.png", false);
    REQUIRE(textureData1 != nullptr);
    REQUIRE(textureData2 != nullptr);
    REQUIRE(textureData3 != nullptr);
    REQUIRE(textureData1 == textureData2);
    REQUIRE(textureData1 != textureData3);

    REQUIRE_THROWS_AS(tgui::TextureManager::copyTexture(nullptr), tgui::Exception);
    REQUIRE_THROWS_AS(tgui::TextureManager::copyTexture(std::make_shared<tgui::TextureData>()), tgui::Exception);
    REQUIRE_NOTHROW(tgui::TextureManager::copyTexture(textureData1));

    REQUIRE_THROWS_AS(tgui::TextureManager::removeTexture(std::make_shared<tgui::TextureData>()), tgui::Exception);
    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(textureData1));
    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(textureData1));
    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(textureData1));
    REQUIRE_THROWS_AS(tgui::TextureManager::removeTexture(textureData1), tgui::Exception);

    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(textureData3));
}
