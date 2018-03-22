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
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Gui.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

using namespace tgui::bind_functions;
using tgui::Layout;
using tgui::Layout2d;

TEST_CASE("[Layouts]")
{
    SECTION("constants")
    {
        Layout l1;
        Layout l2(-20.3f);
        Layout l3{"60"};

        Layout2d l4;
        Layout2d l5({10.f, {" 50 "}});
        Layout2d l6(".2", "-3.5");

        REQUIRE(l1.getValue() == 0);
        REQUIRE(l2.getValue() == -20.3f);
        REQUIRE(l3.getValue() == 60);

        REQUIRE(l4.getValue() == sf::Vector2f(0, 0));
        REQUIRE(l5.getValue() == sf::Vector2f(10, 50));
        REQUIRE(l6.getValue() == sf::Vector2f(0.2f, -3.5f));
    }

    SECTION("copying layouts")
    {
        Layout l1;
        Layout l2{2};
        Layout l3 = l2;
        Layout l4;
        Layout l5{0};
        l5 = std::move(l2);

        REQUIRE(l1.getValue() == 0);
        REQUIRE(l3.getValue() == 2);
        REQUIRE(l4.getValue() == 0);
        REQUIRE(l5.getValue() == 2);

        REQUIRE(l1.toString() == "0");
        REQUIRE(l3.toString() == "2");
        REQUIRE(l4.toString() == "0");
        REQUIRE(l5.toString() == "2");
    }

    SECTION("without strings")
    {
        SECTION("operators")
        {
            Layout l1{2};
            Layout l2(3);

            REQUIRE((l1 + l2).getValue() == 5);
            REQUIRE((l1 - l2).getValue() == -1);
            REQUIRE((l1 * l2).getValue() == 6);
            REQUIRE((l1 / l2).getValue() == 2.f/3.f);
            REQUIRE((-Layout{1}).getValue() == -1);
            REQUIRE((-Layout{-2}).getValue() == 2);

            REQUIRE((l1 + l2).toString() == "2 + 3");
            REQUIRE((l1 - l2).toString() == "2 - 3");
            REQUIRE((l1 * l2).toString() == "2 * 3");
            REQUIRE((l1 / l2).toString() == "2 / 3");
            REQUIRE((-Layout{1}).toString() == "0 - 1");
            REQUIRE((-Layout{-2}).toString() == "0 - -2");

            Layout2d l3{-6, 5};
            Layout2d l4(4, 5);
            Layout2d l5(6, 7);

            REQUIRE((l3 + l4).getValue() == sf::Vector2f(-2, 10));
            REQUIRE((l3 - l4).getValue() == sf::Vector2f(-10, 0));
            REQUIRE((-3 * l3).getValue() == sf::Vector2f(18, -15));
            REQUIRE((l3 * 2).getValue() == sf::Vector2f(-12, 10));
            REQUIRE((l3 / 2).getValue() == sf::Vector2f(-3, 2.5f));
            REQUIRE((-l3).getValue() == sf::Vector2f(6, -5));

            REQUIRE((l3 + l4).toString() == "(-6 + 4, 5 + 5)");
            REQUIRE((l3 - l4).toString() == "(-6 - 4, 5 - 5)");
            REQUIRE((-3 * l3).toString() == "(-3 * -6, -3 * 5)");
            REQUIRE((l3 * 2).toString() == "(-6 * 2, 5 * 2)");
            REQUIRE((l3 / 2).toString() == "(-6 / 2, 5 / 2)");
            REQUIRE((-l3).toString() == "(0 - -6, 0 - 5)");

            REQUIRE((Layout(5) + Layout(3) * Layout(2) - Layout(1)).getValue() == 10);
            REQUIRE((Layout(5) + Layout(3) * (Layout(2) - Layout(1))).getValue() == 8);
            REQUIRE(((Layout(5) + Layout(3)) * Layout(2) - Layout(1)).getValue() == 15);
            REQUIRE((Layout2d(5, 2) + Layout2d(3, -3) / Layout(2) - Layout2d(-1, 6)).getValue() == sf::Vector2f(7.5f, -5.5f));
            REQUIRE(((Layout2d(5, 2) + Layout2d(3, -3)) / Layout(2) - Layout2d(-1, 6)).getValue() == sf::Vector2f(5, -6.5f));

            REQUIRE((Layout(5) + Layout(3) * Layout(2) - Layout(1)).toString() == "(5 + (3 * 2)) - 1");
            REQUIRE((Layout(5) + Layout(3) * (Layout(2) - Layout(1))).toString() == "5 + (3 * (2 - 1))");
            REQUIRE(((Layout(5) + Layout(3)) * Layout(2) - Layout(1)).toString() == "((5 + 3) * 2) - 1");
            REQUIRE((Layout2d(5, 2) + Layout2d(3, -3) / Layout(2) - Layout2d(-1, 6)).toString() == "((5 + (3 / 2)) - -1, (2 + (-3 / 2)) - 6)");
            REQUIRE(((Layout2d(5, 2) + Layout2d(3, -3)) / Layout(2) - Layout2d(-1, 6)).toString() == "(((5 + 3) / 2) - -1, ((2 + -3) / 2) - 6)");
        }

        SECTION("bind functions")
        {
            auto button1 = std::make_shared<tgui::Button>();
            button1->setSize(300, 50);
            button1->setPosition(40, 60);

            auto button2 = std::make_shared<tgui::Button>();
            button2->setPosition(bindLeft(button1), bindTop(button1));
            button2->setSize(bindWidth(button1), bindHeight(button1));
            REQUIRE(button2->getSize() == sf::Vector2f(300, 50));
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));

            button2->setPosition(bindRight(button1), bindBottom(button1));
            REQUIRE(button2->getPosition() == sf::Vector2f(340, 110));

            button2->setPosition(bindPosition(button1));
            button2->setSize(bindSize(button1));
            REQUIRE(button2->getSize() == sf::Vector2f(300, 50));
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));

            button1->setSize(400, 40);
            button1->setPosition(60, 75);
            REQUIRE(button2->getSize() == sf::Vector2f(400, 40));
            REQUIRE(button2->getPosition() == sf::Vector2f(60, 75));

            button1->setSize(bindSize(button2)); // Binding each other only works when value is cached
            REQUIRE(button1->getSize() == sf::Vector2f(400, 40));
            REQUIRE(button2->getSize() == sf::Vector2f(400, 40));

            auto button3 = std::make_shared<tgui::Button>();
            auto panel = std::make_shared<tgui::Panel>();
            panel->setSize(200, 180);
            panel->setPosition(10, 25);
            button1->setSize(300, 50);
            button1->setPosition(40, 60);
            button2->setSize(400, 40);
            button2->setPosition(60, 75);
            panel->add(button1);
            panel->add(button2);
            panel->add(button3);

            button3->setSize(2.5 * bindPosition(button1) + bindSize(button2) / 4 + sf::Vector2f(100, 50));
            REQUIRE(button3->getSize() == sf::Vector2f(300, 210));

            button3->setPosition(2 * bindRight(button1) + bindLeft(button2) / 4 + bindWidth(button1), 50 - bindBottom(button2) + 75 * bindTop(button2));
            REQUIRE(button3->getPosition() == sf::Vector2f(995, 5560));
            REQUIRE(button3->getAbsolutePosition() == sf::Vector2f(1005, 5585));
        }
    }

    SECTION("with strings")
    {
        SECTION("percentages")
        {
            Layout layout{"50%"};
            REQUIRE(layout.toString() == "0.5 * &.size");

            auto button = std::make_shared<tgui::Button>();
            button->setPosition({"40%", "30%"});
            button->setSize({"20%", "10%"});

            REQUIRE(button->getSize() == sf::Vector2f(0, 0));
            REQUIRE(button->getPosition() == sf::Vector2f(0, 0));

            auto panel = std::make_shared<tgui::Panel>();
            panel->setSize(400, 300);
            panel->add(button);

            REQUIRE(button->getSize() == sf::Vector2f(80, 30));
            REQUIRE(button->getPosition() == sf::Vector2f(160, 90));
        }

        SECTION("operators")
        {
            REQUIRE(Layout("2").getValue() == 2);
            REQUIRE(Layout("+2").getValue() == 2);
            REQUIRE(Layout("-2").getValue() == -2);
            REQUIRE(Layout("(2)").getValue() == 2);
            REQUIRE(Layout("-(-2)").getValue() == 2);
            REQUIRE(Layout("+(+2)").getValue() == 2);
            REQUIRE(Layout("2 + 3").getValue() == 5);
            REQUIRE(Layout("2 - 3").getValue() == -1);
            REQUIRE(Layout("2 + -3").getValue() == -1);
            REQUIRE(Layout("2 * 3").getValue() == 6);
            REQUIRE(Layout("5 / 3").getValue() == 5.f/3.f);

            REQUIRE(Layout("2").toString() == "2");
            REQUIRE(Layout("+2").toString() == "0 + 2");
            REQUIRE(Layout("-2").toString() == "0 - 2");
            REQUIRE(Layout("(2)").toString() == "2");
            REQUIRE(Layout("-(-2)").toString() == "0 - (0 - 2)");
            REQUIRE(Layout("+(+2)").toString() == "0 + (0 + 2)");
            REQUIRE(Layout("2 + 3").toString() == "2 + 3");
            REQUIRE(Layout("2 - 3").toString() == "2 - 3");
            REQUIRE(Layout("2 + -3").toString() == "(2 + 0) - 3");
            REQUIRE(Layout("2 * 3").toString() == "2 * 3");
            REQUIRE(Layout("5 / 3").toString() == "5 / 3");

            REQUIRE(Layout("5 + 3 * 2 - 1").getValue() == 10);
            REQUIRE(Layout("5 + 3 * (2 - 1)").getValue() == 8);
            REQUIRE(Layout("(5 + 3) * 2 - 1").getValue() == 15);

            REQUIRE(Layout("5 + 3 * 2 - 1").toString() == "(5 + (3 * 2)) - 1");
            REQUIRE(Layout("5 + 3 * (2 - 1)").toString() == "5 + (3 * (2 - 1))");
            REQUIRE(Layout("(5 + 3) * 2 - 1").toString() == "((5 + 3) * 2) - 1");

            REQUIRE(Layout("xyz").getValue() == 0);
            REQUIRE(Layout("width").getValue() == 0);
        }

        SECTION("bind functions")
        {
            auto panel = std::make_shared<tgui::Panel>();
            panel->setSize(200, 180);
            panel->setPosition(10, 25);

            auto button1 = std::make_shared<tgui::Button>();
            button1->setSize(300, 50);
            button1->setPosition(40, 60);
            panel->add(button1, "b1");

            auto button2 = std::make_shared<tgui::Button>();
            button2->setPosition("b1.position");
            button2->setSize("b1.size");
            REQUIRE(button2->getSize() == sf::Vector2f(0, 0));
            REQUIRE(button2->getPosition() == sf::Vector2f(0, 0));
            REQUIRE(button2->getSizeLayout().toString() == "(b1.size, b1.size)");
            REQUIRE(button2->getPositionLayout().toString() == "(b1.position, b1.position)");

            panel->add(button2, "b2");
            REQUIRE(button2->getSize() == sf::Vector2f(300, 50));
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            REQUIRE(button2->getSizeLayout().toString() == "(b1.size, b1.size)");
            REQUIRE(button2->getPositionLayout().toString() == "(b1.position, b1.position)");

            button2->setPosition({"b1.p"});
            REQUIRE(button2->getPosition() == sf::Vector2f(0, 0));

            button2->setPosition({"b1.pos"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(b1.pos, b1.pos)");

            button2->setPosition("b1.left", std::string{"b1.top"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(b1.left, b1.top)");

            button2->setPosition({"b1.x"}, {"b1.y"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(b1.x, b1.y)");

            button2->setSize({{"b1.right"}, {"b1.bottom"}});
            REQUIRE(button2->getSize() == sf::Vector2f(340, 110));
            REQUIRE(button2->getSizeLayout().toString() == "(b1.left + b1.width, b1.top + b1.height)");

            button2->setSize({"{@, #}"});
            REQUIRE(button2->getSize() == sf::Vector2f(0, 0));

            button2->setPosition({"parent.x"}, {"&.y"});
            REQUIRE(button2->getPosition() == sf::Vector2f(10, 25));
            REQUIRE(button2->getPositionLayout().toString() == "(parent.x, &.y)");

            button2->setPosition({"parent.b1.parent.b1.x"}, {"&.b1.&.b1.y"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(parent.b1.parent.b1.x, &.b1.&.b1.y)");

            button2->setPosition({"b1.position"});
            button2->setSize({"b1.size"});
            button1->setSize(400, 40);
            button1->setPosition(60, 75);
            REQUIRE(button1->getSizeLayout().toString() == "(400, 40)");
            REQUIRE(button1->getPositionLayout().toString() == "(60, 75)");
            REQUIRE(button2->getSize() == sf::Vector2f(400, 40));
            REQUIRE(button2->getPosition() == sf::Vector2f(60, 75));

            auto button3 = std::make_shared<tgui::Button>();
            button1->setSize(300, 50);
            button1->setPosition(40, 60);
            button2->setSize(400, 40);
            button2->setPosition(60, 75);
            panel->add(button3);

            button3->setSize({"2.5 * b1.pos + b2.size / 4 + 50"});
            REQUIRE(button3->getSize() == sf::Vector2f(250, 210));
            REQUIRE(button3->getSizeLayout().toString() == "(((2.5 * b1.pos) + (b2.size / 4)) + 50, ((2.5 * b1.pos) + (b2.size / 4)) + 50)");

            button3->setPosition({"2 * b1.right + b2.x / 4 + b1.w"}, {"50 - b2.bottom + 75 * b2.y"});
            REQUIRE(button3->getPosition() == sf::Vector2f(995, 5560));
            REQUIRE(button3->getAbsolutePosition() == sf::Vector2f(1005, 5585));
            REQUIRE(button3->getPositionLayout().toString() == "(((2 * (b1.left + b1.width)) + (b2.x / 4)) + b1.w, (50 - (b2.top + b2.height)) + (75 * b2.y))");

            auto button4 = std::make_shared<tgui::Button>();
            button4->setSize(200, 50);

            SECTION("Gui")
            {
                sf::RenderTexture texture;
                texture.create(20, 15);
                tgui::Gui gui{texture};

                auto width = bindWidth(gui);
                auto height = bindHeight(gui);
                auto size = bindSize(gui);

                REQUIRE(width.getValue() == 20);
                REQUIRE(height.getValue() == 15);
                REQUIRE(size.getValue() == sf::Vector2f(20, 15));

                gui.setView(sf::View{{4, 3, 40, 30}});

                REQUIRE(width.getValue() == 40);
                REQUIRE(height.getValue() == 30);
                REQUIRE(size.getValue() == sf::Vector2f(40, 30));
            }
        }

        SECTION("No ambiguity with 0")
        {
            auto widget = std::make_shared<tgui::ClickableWidget>();
            widget->setPosition({0, 0});
            widget->setPosition(0,0);
            widget->setPosition({"0","0"});
            widget->setPosition({"0"},{"0"});
            widget->setPosition("0","0");
        }
    }

    SECTION("Bug Fixes")
    {
        SECTION("Setting negative size and reverting back to positive (https://github.com/texus/TGUI/issues/54)")
        {
            tgui::Panel::Ptr panel = std::make_shared<tgui::Panel>();
            tgui::Button::Ptr button = std::make_shared<tgui::Button>();
            panel->add(button);

            // Button width becomes -10
            panel->setSize(10, 10);
            button->setSize({"&.w - 20", "&.h"});
            REQUIRE(button->getSize() == sf::Vector2f(-10, 10));

            // Button width will become positive again
            panel->setSize(200, 100);
            REQUIRE(button->getSize() == sf::Vector2f(180, 100));
        }
    }
}
