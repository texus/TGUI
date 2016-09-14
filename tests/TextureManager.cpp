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
#include <TGUI/TextureManager.hpp>
#include <TGUI/Texture.hpp>
#include <SFML/System/Err.hpp>

TEST_CASE("[TextureManager]")
{
    std::streambuf *oldbuf = sf::err().rdbuf(0);
    tgui::Texture texture;
    REQUIRE(!tgui::TextureManager::getTexture(texture, "NonExistent.png"));
    sf::err().rdbuf(oldbuf);

    tgui::Texture texture1;
    tgui::Texture texture2;
    REQUIRE(tgui::TextureManager::getTexture(texture1, "resources/image.png"));
    REQUIRE(tgui::TextureManager::getTexture(texture2, "resources/image.png"));
    REQUIRE(texture1.getData() == texture2.getData());

    REQUIRE_THROWS_AS(tgui::TextureManager::copyTexture(std::make_shared<tgui::TextureData>()), tgui::Exception);
    REQUIRE_NOTHROW(tgui::TextureManager::copyTexture(texture1.getData()));

    REQUIRE_THROWS_AS(tgui::TextureManager::removeTexture(std::make_shared<tgui::TextureData>()), tgui::Exception);
    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(texture1.getData()));
    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(texture1.getData()));
    REQUIRE_NOTHROW(tgui::TextureManager::removeTexture(texture1.getData()));
    REQUIRE_THROWS_AS(tgui::TextureManager::removeTexture(texture1.getData()), tgui::Exception);
}
