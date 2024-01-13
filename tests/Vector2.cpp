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

#include <iostream>

#include "Tests.hpp"

TEST_CASE("[Vector2]")
{
    SECTION("Constructor")
    {
        tgui::Vector2f vecFloat{1.4f, 2.5f};
        REQUIRE(vecFloat.x == 1.4f);
        REQUIRE(vecFloat.y == 2.5f);

        tgui::Vector2u vecUint{10, 4};
        REQUIRE(vecUint.x == 10);
        REQUIRE(vecUint.y == 4);

        tgui::Vector2i vecInt{-5, 7};
        REQUIRE(vecInt.x == -5);
        REQUIRE(vecInt.y == 7);

        tgui::Vector2<double> vecDouble{3.1, -0.8};
        REQUIRE(vecDouble.x == 3.1);
        REQUIRE(vecDouble.y == -0.8);

        tgui::Vector2i vecStr1{"(1,2)"};
        REQUIRE(vecStr1.x == 1);
        REQUIRE(vecStr1.y == 2);

        tgui::Vector2i vecStr2{"{3, 4}"};
        REQUIRE(vecStr2.x == 3);
        REQUIRE(vecStr2.y == 4);

        tgui::Vector2f vecStr3{"5.6, 7.8"};
        REQUIRE(vecStr3.x == 5.6f);
        REQUIRE(vecStr3.y == 7.8f);

        std::streambuf *oldbuf = std::cerr.rdbuf(nullptr);

        tgui::Vector2i vecStrInvalid1{""};
        REQUIRE(vecStrInvalid1.x == 0);
        REQUIRE(vecStrInvalid1.y == 0);

        tgui::Vector2i vecStrInvalid2{"()"};
        REQUIRE(vecStrInvalid2.x == 0);
        REQUIRE(vecStrInvalid2.y == 0);

        tgui::Vector2i vecStrInvalid3{"{9}"};
        REQUIRE(vecStrInvalid3.x == 0);
        REQUIRE(vecStrInvalid3.y == 0);

        tgui::Vector2i vecStrInvalid4{"(1,2"};
        REQUIRE(vecStrInvalid4.x == 0); // "(1" to int
        REQUIRE(vecStrInvalid4.y == 2); // "2" to int

        tgui::Vector2i vecStrInvalid5{"3,4}"};
        REQUIRE(vecStrInvalid5.x == 3); // "3" to int
        REQUIRE(vecStrInvalid5.y == 4); // "4}" to int

        std::cerr.rdbuf(oldbuf);
    }

    SECTION("Copy")
    {
        tgui::Vector2u vecUint{1, 2};

        tgui::Vector2u vecUint2{vecUint};
        REQUIRE(vecUint2.x == 1);
        REQUIRE(vecUint2.y == 2);

        tgui::Vector2u vecInt{vecUint};
        REQUIRE(vecInt.x == 1);
        REQUIRE(vecInt.y == 2);
    }

#if TGUI_HAS_WINDOW_BACKEND_SFML
    SECTION("Construct from sf::Vector")
    {
        sf::Vector2i sfmlVec{1, 2};
        tgui::Vector2i tguiVec{sfmlVec};
        REQUIRE(tguiVec.x == 1);
        REQUIRE(tguiVec.y == 2);
    }

    SECTION("Convert to sf::Vector")
    {
        tgui::Vector2u tguiVec{1, 2};
        sf::Vector2u sfmlVec{tguiVec};
        REQUIRE(sfmlVec.x == 1);
        REQUIRE(sfmlVec.y == 2);
    }
#endif

    SECTION("operators")
    {
        REQUIRE(tgui::Vector2i{1, 2} == tgui::Vector2i{1, 2});
        REQUIRE(tgui::Vector2i{1, 2} != tgui::Vector2i{2, 2});
        REQUIRE(tgui::Vector2i{1, 2} != tgui::Vector2i{1, 3});
        REQUIRE(tgui::Vector2i{2, 4} != tgui::Vector2i{1, 2});

        REQUIRE(-tgui::Vector2i{1, 2} == tgui::Vector2i{-1, -2});

        REQUIRE(tgui::Vector2i{1, 2} + tgui::Vector2i{3, 4} == tgui::Vector2i{4, 6});
        REQUIRE(tgui::Vector2i{3, 4} - tgui::Vector2i{1, 2} == tgui::Vector2i{2, 2});

        REQUIRE(3.f * tgui::Vector2i{2, 3} == tgui::Vector2i{6, 9});
        REQUIRE(tgui::Vector2i{2, 3} * 2.f == tgui::Vector2i{4, 6});
        REQUIRE(tgui::Vector2i{6, 4} / 2.f == tgui::Vector2i{3, 2});

        tgui::Vector2u vec{5, 4};

        vec += tgui::Vector2u{2, 4};
        REQUIRE(vec == tgui::Vector2u{7, 8});

        vec -= tgui::Vector2u{4, 6};
        REQUIRE(vec == tgui::Vector2u{3, 2});

        vec *= 4.f;
        REQUIRE(vec == tgui::Vector2u{12, 8});

        vec /= 2.f;
        REQUIRE(vec == tgui::Vector2u{6, 4});
    }
}
