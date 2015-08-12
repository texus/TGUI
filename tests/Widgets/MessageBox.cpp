/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "../Tests.hpp"
#include <TGUI/Widgets/MessageBox.hpp>

TEST_CASE("[MessageBox]") {
    tgui::MessageBox::Ptr childWindow = std::make_shared<tgui::MessageBox>();

    SECTION("Signals") {
        REQUIRE_NOTHROW(childWindow->connect("ButtonPressed", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("ButtonPressed", [](sf::String){}));

    }

    SECTION("WidgetType") {
        REQUIRE(childWindow->getWidgetType() == "MessageBox");
    }

    /// TODO: Test the functions in the MessageBox class

    SECTION("Renderer") {
        auto renderer = childWindow->getRenderer();

        SECTION("set serialized property") {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
        }

        SECTION("set object property") {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{10, 20, 30}));
        }

        SECTION("functions") {
            renderer->setTextColor({10, 20, 30});

            SECTION("getPropertyValuePairs") {
                auto pairs = renderer->getPropertyValuePairs();
                REQUIRE(pairs["TextColor"].getColor() == sf::Color(10, 20, 30));
            }
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
    }

    /// TODO: Saving to and loading from file
}
