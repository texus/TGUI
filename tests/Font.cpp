/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[Font]")
{
    tgui::Font font("resources/DejaVuSans.ttf");
    tgui::Font nullFont(nullptr);

    SECTION("Constructor")
    {
        REQUIRE(tgui::Font() == nullptr);
        REQUIRE(tgui::Font(nullptr) == nullptr);
        REQUIRE(tgui::Font("resources/DejaVuSans.ttf") != nullptr);

        REQUIRE_THROWS_AS(tgui::Font("NonExistentFile.ttf"), tgui::Exception);
    }

    SECTION("Smooth")
    {
        font.setSmooth(false);
        REQUIRE(!font.isSmooth());
        font.setSmooth(true);
        REQUIRE(font.isSmooth());
        REQUIRE(nullFont.isSmooth());
        nullFont.setSmooth(false);
        REQUIRE(nullFont.isSmooth());
        nullFont.setSmooth(true);
        REQUIRE(nullFont.isSmooth());
    }

    SECTION("Font height")
    {
        const float fontHeight = font.getFontHeight(30);
        const float ascent = font.getBackendFont()->getAscent(30);
        const float descent = font.getBackendFont()->getDescent(30);
        REQUIRE((fontHeight >= 33 && fontHeight <= 39));
        REQUIRE((ascent >= 27 && ascent <= 30));
        REQUIRE((descent >= -9 && descent <= -6));
        REQUIRE(nullFont.getFontHeight(42) == 0);
    }
}
