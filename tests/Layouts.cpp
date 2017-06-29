/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Gui.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

using tgui::Layout;
using tgui::Layout2d;

TEST_CASE("[Layouts]")
{
    SECTION("constants")
    {
        Layout l1;
        REQUIRE(l1.isConstant());
        REQUIRE(l1.getValue() == 0);

        Layout l2(-20.3f);
        REQUIRE(l2.isConstant());
        REQUIRE(l2.getValue() == -20.3f);

        Layout l3{"60"};
        REQUIRE(l3.isConstant());
        REQUIRE(l3.getValue() == 60);

        Layout2d l4;
        REQUIRE(l4.x.isConstant());
        REQUIRE(l4.y.isConstant());
        REQUIRE(l4.getValue() == sf::Vector2f(0, 0));

        Layout2d l5{10.f, "-50"};
        REQUIRE(l5.x.isConstant());
        REQUIRE(l5.y.isConstant());
        REQUIRE(l5.getValue() == sf::Vector2f(10, -50));

        SECTION("updating parent size")
        {
            l1.updateParentSize(110);
            l2.updateParentSize(120);
            l3.updateParentSize(130);
            l4.updateParentSize({141, 142});
            l5.updateParentSize({153, 154});

            REQUIRE(l1.getValue() == 0);
            REQUIRE(l2.getValue() == -20.3f);
            REQUIRE(l3.getValue() == 60);
            REQUIRE(l4.getValue() == sf::Vector2f(0, 0));
            REQUIRE(l5.getValue() == sf::Vector2f(10, -50));
        }
    }

    SECTION("relative values")
    {
        Layout l1{"60%"};
        REQUIRE(!l1.isConstant());
        REQUIRE(l1.getRatio() == 60.f/100.f);

        Layout2d l2{{"40%"}, {"5%"}};
        REQUIRE(!l2.x.isConstant());
        REQUIRE(!l2.y.isConstant());
        REQUIRE(l2.x.getRatio() == 40.f/100.f);
        REQUIRE(l2.y.getRatio() == 5.f/100.f);

        Layout2d l3{40, "25.3%"};
        REQUIRE(l3.x.isConstant());
        REQUIRE(!l3.y.isConstant());
        REQUIRE(l3.x.getValue() == 40);
        REQUIRE(l3.y.getRatio() == 25.3f/100.f);

        SECTION("updating parent size")
        {
            REQUIRE(l1.getValue() == 0);
            l1.updateParentSize(250);
            REQUIRE(l1.getValue() == 250*0.6f);
            l1.updateParentSize(100);
            REQUIRE(l1.getValue() == 100*0.6f);

            l2.updateParentSize({150, 120});
            REQUIRE(l2.x.getValue() == 150*0.4f);
            REQUIRE(l2.y.getValue() == 120*0.05f);

            l3.updateParentSize({300, 506});
            REQUIRE(l3.x.getValue() == 40);
            REQUIRE(l3.y.getValue() == 506*0.253f);
        }
    }

    SECTION("empty string")
    {
        Layout l{""};
        REQUIRE(l.isConstant());
        REQUIRE(l.getValue() == 0);
    }
}
