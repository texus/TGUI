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

        REQUIRE(l4.getValue() == tgui::Vector2f(0, 0));
        REQUIRE(l5.getValue() == tgui::Vector2f(10, 50));
        REQUIRE(l6.getValue() == tgui::Vector2f(0.2f, -3.5f));

        REQUIRE(l1.isConstant());
        REQUIRE(l2.isConstant());
        REQUIRE(l3.isConstant());

        REQUIRE(l4.x.isConstant());
        REQUIRE(l4.y.isConstant());
        REQUIRE(l5.x.isConstant());
        REQUIRE(l5.y.isConstant());
        REQUIRE(l6.x.isConstant());
        REQUIRE(l6.y.isConstant());
    }

    SECTION("copying layouts")
    {
        Layout l1;
        Layout l2{2};
        Layout l3 = l2; // NOLINT(performance-unnecessary-copy-initialization)
        Layout l4;
        Layout l5{0};
        Layout l6{"max(2,3)+1"};
        l5 = std::move(l6);

        REQUIRE(l1.getValue() == 0);
        REQUIRE(l3.getValue() == 2);
        REQUIRE(l4.getValue() == 0);
        REQUIRE(l5.getValue() == 4);

        REQUIRE(l1.toString() == "0");
        REQUIRE(l3.toString() == "2");
        REQUIRE(l4.toString() == "0");
        REQUIRE(l5.toString() == "max(2, 3) + 1");
    }

    SECTION("without strings")
    {
        SECTION("relative values")
        {
            Layout layout{tgui::RelativeValue(1)};
            REQUIRE(layout.toString() == "100%");

            Layout layout2{tgui::RelativeValue(0.5f)};
            REQUIRE(layout2.toString() == "50%");

            REQUIRE(!layout.isConstant());

            auto button = std::make_shared<tgui::Button>();
            button->setPosition({tgui::RelativeValue(0.4f), tgui::RelativeValue(0.3f)});
            button->setSize({tgui::RelativeValue(0.2f), tgui::RelativeValue(0.1f)});

            REQUIRE(button->getSize() == tgui::Vector2f(0, 0));
            REQUIRE(button->getPosition() == tgui::Vector2f(0, 0));

            auto panel = std::make_shared<tgui::Panel>();
            panel->setSize(400, 300);
            panel->add(button);

            REQUIRE(button->getSize() == tgui::Vector2f(80, 30));
            REQUIRE(button->getPosition() == tgui::Vector2f(160, 90));

            SECTION("Inner size is used")
            {
                panel->getRenderer()->setBorders({5, 10, 15, 20});
                panel->getRenderer()->setPadding({25, 30, 35, 40});

                REQUIRE(button->getSize() == tgui::Vector2f(0.2f * (400 - 5 - 15 - 25 - 35), 0.1f * (300 - 10 - 20 - 30 - 40)));
                REQUIRE(button->getPosition() == tgui::Vector2f(0.4f * (400 - 5 - 15 - 25 - 35), 0.3f * (300 - 10 - 20 - 30 - 40)));
            }
        }

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

            REQUIRE((l3 + l4).getValue() == tgui::Vector2f(-2, 10));
            REQUIRE((l3 - l4).getValue() == tgui::Vector2f(-10, 0));
            REQUIRE((-3 * l3).getValue() == tgui::Vector2f(18, -15));
            REQUIRE((l3 * 2).getValue() == tgui::Vector2f(-12, 10));
            REQUIRE((l3 / 2).getValue() == tgui::Vector2f(-3, 2.5f));
            REQUIRE((-l3).getValue() == tgui::Vector2f(6, -5));

            REQUIRE((l3 + l4).toString() == "(-6 + 4, 5 + 5)");
            REQUIRE((l3 - l4).toString() == "(-6 - 4, 5 - 5)");
            REQUIRE((-3 * l3).toString() == "(-3 * -6, -3 * 5)");
            REQUIRE((l3 * 2).toString() == "(-6 * 2, 5 * 2)");
            REQUIRE((l3 / 2).toString() == "(-6 / 2, 5 / 2)");
            REQUIRE((-l3).toString() == "(0 - -6, 0 - 5)");

            REQUIRE((Layout(5) + Layout(3) * Layout(2) - Layout(1)).getValue() == 10);
            REQUIRE((Layout(5) + Layout(3) * (Layout(2) - Layout(1))).getValue() == 8);
            REQUIRE(((Layout(5) + Layout(3)) * Layout(2) - Layout(1)).getValue() == 15);
            REQUIRE((Layout2d(5, 2) + Layout2d(3, -3) / Layout(2) - Layout2d(-1, 6)).getValue() == tgui::Vector2f(7.5f, -5.5f));
            REQUIRE(((Layout2d(5, 2) + Layout2d(3, -3)) / Layout(2) - Layout2d(-1, 6)).getValue() == tgui::Vector2f(5, -6.5f));

            REQUIRE((Layout(5) + Layout(3) * Layout(2) - Layout(1)).toString() == "(5 + (3 * 2)) - 1");
            REQUIRE((Layout(5) + Layout(3) * (Layout(2) - Layout(1))).toString() == "5 + (3 * (2 - 1))");
            REQUIRE(((Layout(5) + Layout(3)) * Layout(2) - Layout(1)).toString() == "((5 + 3) * 2) - 1");
            REQUIRE((Layout2d(5, 2) + Layout2d(3, -3) / Layout(2) - Layout2d(-1, 6)).toString() == "((5 + (3 / 2)) - -1, (2 + (-3 / 2)) - 6)");
            REQUIRE(((Layout2d(5, 2) + Layout2d(3, -3)) / Layout(2) - Layout2d(-1, 6)).toString() == "(((5 + 3) / 2) - -1, ((2 + -3) / 2) - 6)");
        }

        SECTION("bind functions")
        {
            SECTION("Widgets")
            {
                auto button1 = std::make_shared<tgui::Button>();
                button1->setSize(300, 50);
                button1->setPosition(40, 60);

                REQUIRE(!bindLeft(button1).isConstant());

                auto button2 = std::make_shared<tgui::Button>();
                button2->setPosition(bindLeft(button1), bindTop(button1));
                button2->setSize(bindWidth(button1), bindHeight(button1));
                REQUIRE(button2->getSize() == tgui::Vector2f(300, 50));
                REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));

                button2->setPosition(bindRight(button1), bindBottom(button1));
                REQUIRE(button2->getPosition() == tgui::Vector2f(340, 110));

                button2->setPosition(bindPosition(button1));
                button2->setSize(bindSize(button1));
                REQUIRE(button2->getSize() == tgui::Vector2f(300, 50));
                REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));

                button1->setSize(400, 40);
                button1->setPosition(60, 75);
                REQUIRE(button2->getSize() == tgui::Vector2f(400, 40));
                REQUIRE(button2->getPosition() == tgui::Vector2f(60, 75));

                button1->setSize(bindSize(button2)); // Binding each other only works when value is cached
                REQUIRE(button1->getSize() == tgui::Vector2f(400, 40));
                REQUIRE(button2->getSize() == tgui::Vector2f(400, 40));

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

                button3->setSize(2.5 * bindPosition(button1) + bindSize(button2) / 4 + tgui::Vector2f(100, 50));
                REQUIRE(button3->getSize() == tgui::Vector2f(300, 210));

                button3->setPosition(2 * bindRight(button1) + bindLeft(button2) / 4 + bindWidth(button1), 50 - bindBottom(button2) + 75 * bindTop(button2));
                REQUIRE(button3->getPosition() == tgui::Vector2f(995, 5560));
                REQUIRE(button3->getAbsolutePosition() == tgui::Vector2f(1005, 5585));

                auto button4 = std::make_shared<tgui::Button>();
                button4->setSize(200, 50);

                auto button5 = std::make_shared<tgui::Button>();
                button5->setSize(bindMax(bindWidth(button4), bindHeight(button4)), bindMin(bindWidth(button4), bindHeight(button4)));
                REQUIRE(button5->getSize() == tgui::Vector2f(200, 50));

                button4->setSize(80, 120);
                REQUIRE(button5->getSize() == tgui::Vector2f(120, 80));

                button5->setPosition(bindSize(button5));
                REQUIRE(button5->getPosition() == tgui::Vector2f(120, 80));
            }

            SECTION("Container inner size")
            {
                auto panel = tgui::Panel::create({400, 300});

                auto width = bindInnerWidth(panel);
                auto height = bindInnerHeight(panel);
                auto size = bindInnerSize(panel);

                REQUIRE(width.getValue() == 400);
                REQUIRE(height.getValue() == 300);
                REQUIRE(size.getValue() == tgui::Vector2f(400, 300));

                panel->getRenderer()->setBorders({10, 20, 15, 25});

                REQUIRE(width.getValue() == 375);
                REQUIRE(height.getValue() == 255);
                REQUIRE(size.getValue() == tgui::Vector2f(375, 255));
            }

            SECTION("Gui")
            {
                GuiNull gui;
                gui.setAbsoluteView({0, 0, 20, 15});

                auto width = bindWidth(gui);
                auto height = bindHeight(gui);
                auto size = bindSize(gui);

                REQUIRE(width.getValue() == 20);
                REQUIRE(height.getValue() == 15);
                REQUIRE(size.getValue() == tgui::Vector2f(20, 15));

                gui.setAbsoluteView({4, 3, 40, 30});

                REQUIRE(width.getValue() == 40);
                REQUIRE(height.getValue() == 30);
                REQUIRE(size.getValue() == tgui::Vector2f(40, 30));
            }

            SECTION("Bound widgets to string")
            {
                auto root = tgui::Panel::create({400, 300});

                auto panel = tgui::Panel::create({300, 200});
                root->add(panel, "Panel");

                auto button1 = std::make_shared<tgui::Button>();
                auto button2 = std::make_shared<tgui::Button>();
                auto button3 = std::make_shared<tgui::Button>();
                panel->add(button1, "b1");
                panel->add(button2, "b2");

                auto layout = bindRight(button1) + bindBottom(button2) + bindInnerWidth(panel) + bindInnerHeight(panel) + 10;
                REQUIRE(layout.toString() == "((((b1.left + b1.width) + (b2.top + b2.height)) + Panel.innerwidth) + Panel.innerheight) + 10");
            }
        }
    }

    SECTION("with strings")
    {
        SECTION("percentages")
        {
            Layout layout{"100%"};
            REQUIRE(layout.toString() == "100%");

            Layout layout2{"50%"};
            REQUIRE(layout2.toString() == "50%");

            REQUIRE(!layout.isConstant());

            auto button = std::make_shared<tgui::Button>();
            button->setPosition({"40%", "30%"});
            button->setSize({"20%", "10%"});

            REQUIRE(button->getSize() == tgui::Vector2f(0, 0));
            REQUIRE(button->getPosition() == tgui::Vector2f(0, 0));

            auto panel = std::make_shared<tgui::Panel>();
            panel->setSize(400, 300);
            panel->add(button);

            REQUIRE(button->getSize() == tgui::Vector2f(80, 30));
            REQUIRE(button->getPosition() == tgui::Vector2f(160, 90));

            SECTION("Inner size is used")
            {
                panel->getRenderer()->setBorders({5, 10, 15, 20});
                panel->getRenderer()->setPadding({25, 30, 35, 40});

                REQUIRE(button->getSize() == tgui::Vector2f(0.2f * (400 - 5 - 15 - 25 - 35), 0.1f * (300 - 10 - 20 - 30 - 40)));
                REQUIRE(button->getPosition() == tgui::Vector2f(0.4f * (400 - 5 - 15 - 25 - 35), 0.3f * (300 - 10 - 20 - 30 - 40)));
            }
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
            REQUIRE(Layout("min(1,2)").getValue() == 1);
            REQUIRE(Layout("min(2,1)").getValue() == 1);
            REQUIRE(Layout("max(1,2)").getValue() == 2);
            REQUIRE(Layout("max(2,1)").getValue() == 2);

            REQUIRE(Layout("2").toString() == "2");
            REQUIRE(Layout("+2").toString() == "2");
            REQUIRE(Layout("-2").toString() == "-2");
            REQUIRE(Layout("(2)").toString() == "2");
            REQUIRE(Layout("-(-2)").toString() == "2");
            REQUIRE(Layout("+(+2)").toString() == "2");
            REQUIRE(Layout("2 + 3").toString() == "2 + 3");
            REQUIRE(Layout("2 - 3").toString() == "2 - 3");
            REQUIRE(Layout("2 + -3").toString() == "(2 + 0) - 3");
            REQUIRE(Layout("2 * 3").toString() == "2 * 3");
            REQUIRE(Layout("5 / 3").toString() == "5 / 3");
            REQUIRE(Layout("min(1,2)").toString() == "min(1, 2)");
            REQUIRE(Layout("max(2,1)").toString() == "max(2, 1)");

            REQUIRE(!Layout("2 + 3").isConstant());
            REQUIRE(!Layout("2 - 3").isConstant());
            REQUIRE(!Layout("2 + -3").isConstant());
            REQUIRE(!Layout("2 * 3").isConstant());
            REQUIRE(!Layout("5 / 3").isConstant());
            REQUIRE(!Layout("min(1,2)").isConstant());
            REQUIRE(!Layout("max(2,1)").isConstant());

            REQUIRE(Layout("5 + 3 * 2 - 1").getValue() == 10);
            REQUIRE(Layout("5 + 3 * (2 - 1)").getValue() == 8);
            REQUIRE(Layout("(5 + 3) * 2 - 1").getValue() == 15);
            REQUIRE(Layout("min(5 + min(2, 3), max(2, 1) * 3)").getValue() == 6);
            REQUIRE(Layout("min(5 + min(2, 3), max(2, 1) * 4)").getValue() == 7);
            REQUIRE(Layout("6 * (max(2,1) + (1 - (5))) / (-3)").getValue() == 4);

            REQUIRE(Layout("5 + 3 * 2 - 1").toString() == "(5 + (3 * 2)) - 1");
            REQUIRE(Layout("5 + 3 * (2 - 1)").toString() == "5 + (3 * (2 - 1))");
            REQUIRE(Layout("(5 + 3) * 2 - 1").toString() == "((5 + 3) * 2) - 1");
            REQUIRE(Layout("min(5 + min(2, 3), max(2, 1) * 3)").toString() == "min(5 + min(2, 3), max(2, 1) * 3)");
            REQUIRE(Layout("6 * (max(2,1) + (1 - (5))) / (-3)").toString() == "(6 * (max(2, 1) + (1 - 5))) / -3");

            REQUIRE(Layout("(5 + 3) * 2 - 1").toString() == "((5 + 3) * 2) - 1");

            REQUIRE(Layout("width").getValue() == 0);

            std::streambuf *oldbuf = std::cerr.rdbuf(nullptr);
            REQUIRE(Layout("xyz").getValue() == 0);
            std::cerr.rdbuf(oldbuf);
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
            REQUIRE(button2->getSize() == tgui::Vector2f(0, 0));
            REQUIRE(button2->getPosition() == tgui::Vector2f(0, 0));
            REQUIRE(button2->getSizeLayout().toString() == "(b1.size, b1.size)");
            REQUIRE(button2->getPositionLayout().toString() == "(b1.position, b1.position)");

            panel->add(button2, "b2");
            REQUIRE(button2->getSize() == tgui::Vector2f(300, 50));
            REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));
            REQUIRE(button2->getSizeLayout().toString() == "(b1.size, b1.size)");
            REQUIRE(button2->getPositionLayout().toString() == "(b1.position, b1.position)");

            std::streambuf *oldbuf = std::cerr.rdbuf(nullptr);
            button2->setPosition({"b1.p"});
            REQUIRE(button2->getPosition() == tgui::Vector2f(0, 0));
            std::cerr.rdbuf(oldbuf);

            button2->setPosition({"b1.pos"});
            REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(b1.pos, b1.pos)");

            button2->setPosition("b1.left", tgui::String{"b1.top"});
            REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(b1.left, b1.top)");

            button2->setPosition({"b1.x"}, {"b1.y"});
            REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(b1.x, b1.y)");

            button2->setSize({{"b1.right"}, {"b1.bottom"}});
            REQUIRE(button2->getSize() == tgui::Vector2f(340, 110));
            REQUIRE(button2->getSizeLayout().toString() == "(b1.left + b1.width, b1.top + b1.height)");

            oldbuf = std::cerr.rdbuf(nullptr);
            button2->setSize({"{@, #}"});
            REQUIRE(button2->getSize() == tgui::Vector2f(0, 0));
            std::cerr.rdbuf(oldbuf);

            button2->setPosition({"parent.x"}, {"&.y"});
            REQUIRE(button2->getPosition() == tgui::Vector2f(10, 25));
            REQUIRE(button2->getPositionLayout().toString() == "(parent.x, &.y)");

            button2->setPosition({"parent.b1.parent.b1.x"}, {"&.b1.&.b1.y"});
            REQUIRE(button2->getPosition() == tgui::Vector2f(40, 60));
            REQUIRE(button2->getPositionLayout().toString() == "(parent.b1.parent.b1.x, &.b1.&.b1.y)");

            button2->setPosition({"b1.position"});
            button2->setSize({"b1.size"});
            button1->setSize(400, 40);
            button1->setPosition(60, 75);
            REQUIRE(button1->getSizeLayout().toString() == "(400, 40)");
            REQUIRE(button1->getPositionLayout().toString() == "(60, 75)");
            REQUIRE(button2->getSize() == tgui::Vector2f(400, 40));
            REQUIRE(button2->getPosition() == tgui::Vector2f(60, 75));

            auto button3 = std::make_shared<tgui::Button>();
            button1->setSize(300, 50);
            button1->setPosition(40, 60);
            button2->setSize(400, 40);
            button2->setPosition(60, 75);
            panel->add(button3);

            button3->setSize({"2.5 * b1.pos + b2.size / 4 + 50"});
            REQUIRE(button3->getSize() == tgui::Vector2f(250, 210));
            REQUIRE(button3->getSizeLayout().toString() == "(((2.5 * b1.pos) + (b2.size / 4)) + 50, ((2.5 * b1.pos) + (b2.size / 4)) + 50)");

            button3->setPosition({"2 * b1.right + b2.x / 4 + b1.w"}, {"50 - b2.bottom + 75 * b2.y"});
            REQUIRE(button3->getPosition() == tgui::Vector2f(995, 5560));
            REQUIRE(button3->getAbsolutePosition() == tgui::Vector2f(1005, 5585));
            REQUIRE(button3->getPositionLayout().toString() == "(((2 * (b1.left + b1.width)) + (b2.x / 4)) + b1.w, (50 - (b2.top + b2.height)) + (75 * b2.y))");

            auto button4 = std::make_shared<tgui::Button>();
            button4->setSize(200, 50);
            panel->add(button4, "b4");

            auto button5 = std::make_shared<tgui::Button>();
            panel->add(button5);
            button5->setSize({"max(b4.w, b4.h)", "min(b4.w, b4.h)"});
            REQUIRE(button5->getSize() == tgui::Vector2f(200, 50));

            button4->setSize(80, 120);
            REQUIRE(button5->getSize() == tgui::Vector2f(120, 80));

            button5->setPosition("size");
            REQUIRE(button5->getPosition() == tgui::Vector2f(120, 80));
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

    SECTION("AutoLayout")
    {
        auto parent = tgui::Panel::create({100, 100});

        auto compareVector2f = [](tgui::Vector2f left, tgui::Vector2f right) {
            return (std::fabs(left.x - right.x) < 0.00001f) && (std::fabs(left.y - right.y) < 0.00001f);
        };

        SECTION("All layouts")
        {
            auto leftmost1 = tgui::Panel::create({"2%", 0});
            leftmost1->setAutoLayout(tgui::AutoLayout::Leftmost);
            parent->add(leftmost1);

            auto left1 = tgui::Panel::create({"4%", 0});
            left1->setAutoLayout(tgui::AutoLayout::Left);
            parent->add(left1);

            auto top1 = tgui::Panel::create({0, "6%"});
            top1->setAutoLayout(tgui::AutoLayout::Top);
            parent->add(top1);

            auto bottom1 = tgui::Panel::create({0, "8%"});
            bottom1->setAutoLayout(tgui::AutoLayout::Bottom);
            parent->add(bottom1);

            auto rightmost1 = tgui::Panel::create({"10%", 0});
            rightmost1->setAutoLayout(tgui::AutoLayout::Rightmost);
            parent->add(rightmost1);

            auto right1 = tgui::Panel::create({"12%", 0});
            right1->setAutoLayout(tgui::AutoLayout::Right);
            parent->add(right1);

            auto right2 = tgui::Panel::create({"13%", 0});
            right2->setAutoLayout(tgui::AutoLayout::Right);
            parent->add(right2);

            auto rightmost2 = tgui::Panel::create({"14%", 0});
            rightmost2->setAutoLayout(tgui::AutoLayout::Rightmost);
            parent->add(rightmost2);

            auto bottom2 = tgui::Panel::create({0, "15%"});
            bottom2->setAutoLayout(tgui::AutoLayout::Bottom);
            parent->add(bottom2);

            auto top2 = tgui::Panel::create({0, "16%"});
            top2->setAutoLayout(tgui::AutoLayout::Top);
            parent->add(top2);

            auto left2 = tgui::Panel::create({"17%", 0});
            left2->setAutoLayout(tgui::AutoLayout::Left);
            parent->add(left2);

            auto leftmost2 = tgui::Panel::create({"18%", 0});
            leftmost2->setAutoLayout(tgui::AutoLayout::Leftmost);
            parent->add(leftmost2);

            auto fill = tgui::Panel::create({0, 0});
            fill->setAutoLayout(tgui::AutoLayout::Fill);
            parent->add(fill);

            REQUIRE(compareVector2f(leftmost1->getSize(), {2, 100}));
            REQUIRE(compareVector2f(left1->getSize(), {4, 55}));
            REQUIRE(compareVector2f(top1->getSize(), {56, 6}));
            REQUIRE(compareVector2f(bottom1->getSize(), {56, 8}));
            REQUIRE(compareVector2f(rightmost1->getSize(), {10, 100}));
            REQUIRE(compareVector2f(right1->getSize(), {12, 55}));
            REQUIRE(compareVector2f(leftmost2->getSize(), {18, 100}));
            REQUIRE(compareVector2f(left2->getSize(), {17, 55}));
            REQUIRE(compareVector2f(top2->getSize(), {56, 16}));
            REQUIRE(compareVector2f(bottom2->getSize(), {56, 15}));
            REQUIRE(compareVector2f(rightmost2->getSize(), {14, 100}));
            REQUIRE(compareVector2f(right2->getSize(), {13, 55}));
            REQUIRE(compareVector2f(fill->getSize(), {10, 55}));

            REQUIRE(compareVector2f(leftmost1->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(left1->getPosition(), {20, 22}));
            REQUIRE(compareVector2f(top1->getPosition(), {20, 0}));
            REQUIRE(compareVector2f(bottom1->getPosition(), {20, 92}));
            REQUIRE(compareVector2f(rightmost1->getPosition(), {90, 0}));
            REQUIRE(compareVector2f(right1->getPosition(), {64, 22}));
            REQUIRE(compareVector2f(leftmost2->getPosition(), {2, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {24, 22}));
            REQUIRE(compareVector2f(top2->getPosition(), {20, 6}));
            REQUIRE(compareVector2f(bottom2->getPosition(), {20, 77}));
            REQUIRE(compareVector2f(rightmost2->getPosition(), {76, 0}));
            REQUIRE(compareVector2f(right2->getPosition(), {51, 22}));
            REQUIRE(compareVector2f(fill->getPosition(), {41, 22}));
        }

        SECTION("z-order")
        {
            auto left1 = tgui::Panel::create({20, 0});
            left1->setAutoLayout(tgui::AutoLayout::Leftmost);
            parent->add(left1);

            auto left2 = tgui::Panel::create({40, 0});
            left2->setAutoLayout(tgui::AutoLayout::Leftmost);
            parent->add(left2);

            REQUIRE(compareVector2f(left1->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {20, 0}));

            left2->moveToBack();
            REQUIRE(compareVector2f(left1->getPosition(), {40, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {0, 0}));

            left2->moveToFront();
            REQUIRE(compareVector2f(left1->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {20, 0}));

            parent->moveWidgetForward(left1);
            REQUIRE(compareVector2f(left1->getPosition(), {40, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {0, 0}));

            parent->moveWidgetBackward(left1);
            REQUIRE(compareVector2f(left1->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {20, 0}));

            parent->setWidgetIndex(left2, 0);
            REQUIRE(compareVector2f(left1->getPosition(), {40, 0}));
            REQUIRE(compareVector2f(left2->getPosition(), {0, 0}));
        }

        SECTION("Removing widget")
        {
            auto left1 = tgui::Panel::create({20, 0});
            left1->setAutoLayout(tgui::AutoLayout::Left);
            parent->add(left1);

            auto left2 = tgui::Panel::create({40, 0});
            left2->setAutoLayout(tgui::AutoLayout::Left);
            parent->add(left2);

            REQUIRE(compareVector2f(left2->getPosition(), {20, 0}));
            parent->remove(left1);
            REQUIRE(compareVector2f(left2->getPosition(), {0, 0}));
        }

        SECTION("Resizing widget")
        {
            auto top1 = tgui::Panel::create({0, 30});
            top1->setAutoLayout(tgui::AutoLayout::Top);
            parent->add(top1);

            auto top2 = tgui::Panel::create({0, 20});
            top2->setAutoLayout(tgui::AutoLayout::Top);
            parent->add(top2);

            REQUIRE(compareVector2f(top2->getPosition(), {0, 30}));
            top1->setHeight(40);
            REQUIRE(compareVector2f(top2->getPosition(), {0, 40}));
        }

        SECTION("Resizing parent")
        {
            auto panel1 = tgui::Panel::create({"20%", 0});
            panel1->setAutoLayout(tgui::AutoLayout::Right);
            parent->add(panel1);

            auto panel2 = tgui::Panel::create({0, "30%"});
            panel2->setAutoLayout(tgui::AutoLayout::Bottom);
            parent->add(panel2);

            REQUIRE(compareVector2f(panel1->getPosition(), {80, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 70}));
            REQUIRE(compareVector2f(panel1->getSize(), {20, 70}));
            REQUIRE(compareVector2f(panel2->getSize(), {100, 30}));

            parent->setSize({50, 150});
            REQUIRE(compareVector2f(panel1->getPosition(), {40, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 105}));
            REQUIRE(compareVector2f(panel1->getSize(), {10, 105}));
            REQUIRE(compareVector2f(panel2->getSize(), {50, 45}));

            parent->getRenderer()->setBorders({5, 15});
            REQUIRE(compareVector2f(panel1->getPosition(), {32, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 84}));
            REQUIRE(compareVector2f(panel1->getSize(), {8, 84}));
            REQUIRE(compareVector2f(panel2->getSize(), {40, 36}));

            parent->getRenderer()->setPadding({5, 15});
            REQUIRE(compareVector2f(panel1->getPosition(), {24, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 63}));
            REQUIRE(compareVector2f(panel1->getSize(), {6, 63}));
            REQUIRE(compareVector2f(panel2->getSize(), {30, 27}));
        }

        SECTION("Changing layout")
        {
            auto panel1 = tgui::Panel::create({20, 0});
            parent->add(panel1);

            auto panel2 = tgui::Panel::create({30, 0});
            parent->add(panel2);

            panel1->setAutoLayout(tgui::AutoLayout::Left);
            panel2->setAutoLayout(tgui::AutoLayout::Left);
            REQUIRE(compareVector2f(panel1->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {20, 0}));

            panel1->setAutoLayout(tgui::AutoLayout::Right);
            REQUIRE(compareVector2f(panel1->getPosition(), {80, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 0}));

            // Height can't be changed because it is still controlled by the AutoLayout
            panel1->setHeight(50);
            REQUIRE(compareVector2f(panel1->getPosition(), {80, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 0}));

            panel1->setAutoLayout(tgui::AutoLayout::Top);
            panel1->setHeight(40);
            REQUIRE(compareVector2f(panel1->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 40}));
        }

        SECTION("Copying widget")
        {
            auto panel1 = tgui::Panel::create({20, 0});
            panel1->setAutoLayout(tgui::AutoLayout::Rightmost);
            parent->add(panel1);

            auto panel2 = tgui::Panel::copy(panel1);
            parent->add(panel2);

            REQUIRE(compareVector2f(panel2->getPosition(), {60, 0}));
            REQUIRE(compareVector2f(panel2->getSize(), {20, 100}));
        }

        SECTION("Copying parent")
        {
            auto panel1 = tgui::Panel::create();
            panel1->setAutoLayout(tgui::AutoLayout::Fill);
            parent->add(panel1, "Pnl1");

            auto panel2 = tgui::Panel::create({0, "30%"});
            panel2->setAutoLayout(tgui::AutoLayout::Top);
            parent->add(panel2, "Pnl2");

            tgui::Panel::Ptr parentCopy;
            SECTION("Copy constructor")
            {
                parentCopy = tgui::Panel::copy(parent);
            }
            SECTION("Assignment operator")
            {
                parentCopy = tgui::Panel::create();
                *parentCopy = *parent;
            }

            auto panelCopy1 = parentCopy->get<tgui::Panel>("Pnl1");
            auto panelCopy2 = parentCopy->get<tgui::Panel>("Pnl2");

            REQUIRE(compareVector2f(panel1->getPosition(), {0, 30}));
            REQUIRE(compareVector2f(panel1->getSize(), {100, 70}));
            REQUIRE(compareVector2f(panel2->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(panel2->getSize(), {100, 30}));
            REQUIRE(compareVector2f(panelCopy1->getPosition(), {0, 30}));
            REQUIRE(compareVector2f(panelCopy1->getSize(), {100, 70}));
            REQUIRE(compareVector2f(panelCopy2->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(panelCopy2->getSize(), {100, 30}));

            // The copies are independent of each other
            auto panel3 = tgui::Panel::create({40, 0});
            panel3->setAutoLayout(tgui::AutoLayout::Leftmost);
            parent->add(panel3);

            panel2->setHeight(20);
            panelCopy2->setHeight(50);

            REQUIRE(compareVector2f(panel1->getPosition(), {40, 20}));
            REQUIRE(compareVector2f(panel1->getSize(), {60, 80}));
            REQUIRE(compareVector2f(panel2->getPosition(), {40, 0}));
            REQUIRE(compareVector2f(panel2->getSize(), {60, 20}));
            REQUIRE(compareVector2f(panelCopy1->getPosition(), {0, 50}));
            REQUIRE(compareVector2f(panelCopy1->getSize(), {100, 50}));
            REQUIRE(compareVector2f(panelCopy2->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(panelCopy2->getSize(), {100, 50}));
        }

        SECTION("Dependent layouts")
        {
            auto panel1 = tgui::Panel::create({40, 30});
            panel1->setPosition({35, 5});
            parent->add(panel1);

            auto panel2 = tgui::Panel::create({25, 20});
            panel2->setPosition({45, 50});
            parent->add(panel2);

            auto panel3 = tgui::Panel::create({10, 10});
            panel3->setPosition(bindPosition(panel1) + (bindSize(panel1) - bindSize(panel3)) / 2.f);
            parent->add(panel3);

            auto panel4 = tgui::Panel::create({15, bindHeight(panel2) / 2.f});
            panel4->setPosition({bindRight(panel2) + 5, bindTop(panel2) + (bindHeight(panel2) - bindHeight(panel4)) / 2.f});
            parent->add(panel4);

            REQUIRE(compareVector2f(panel3->getPosition(), {50, 15}));
            REQUIRE(compareVector2f(panel3->getSize(), {10, 10}));
            REQUIRE(compareVector2f(panel4->getPosition(), {75, 55}));
            REQUIRE(compareVector2f(panel4->getSize(), {15, 10}));

            panel1->setAutoLayout(tgui::AutoLayout::Fill);
            panel2->setAutoLayout(tgui::AutoLayout::Left);

            REQUIRE(compareVector2f(panel3->getPosition(), {57.5f, 45}));
            REQUIRE(compareVector2f(panel3->getSize(), {10, 10}));
            REQUIRE(compareVector2f(panel4->getPosition(), {30, 25}));
            REQUIRE(compareVector2f(panel4->getSize(), {15, 50}));

            panel4->setWidth(bindWidth(panel1));
            REQUIRE(compareVector2f(panel4->getSize(), {75, 50}));
        }

        SECTION("Nested layouts")
        {
            auto outerPanelLeft = tgui::Panel::create({"10%", 0});
            outerPanelLeft->setAutoLayout(tgui::AutoLayout::Left);
            parent->add(outerPanelLeft);

            auto outerPanelTop = tgui::Panel::create({0, "30%"});
            outerPanelTop->setAutoLayout(tgui::AutoLayout::Top);
            parent->add(outerPanelTop);

            auto container = tgui::Panel::create({0, 0});
            container->setAutoLayout(tgui::AutoLayout::Fill);
            parent->add(container);

            auto innerPanelRight = tgui::Panel::create({"20%", 0});
            innerPanelRight->setAutoLayout(tgui::AutoLayout::Right);
            container->add(innerPanelRight);

            auto innerPanelBottom = tgui::Panel::create({0, "25%"});
            innerPanelBottom->setAutoLayout(tgui::AutoLayout::Bottom);
            container->add(innerPanelBottom);

            auto innerPanelFill = tgui::Panel::create();
            innerPanelFill->setAutoLayout(tgui::AutoLayout::Fill);
            container->add(innerPanelFill);

            REQUIRE(compareVector2f(container->getPosition(), {10, 30}));
            REQUIRE(compareVector2f(container->getSize(), {90, 70}));
            REQUIRE(compareVector2f(innerPanelFill->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(innerPanelFill->getSize(), {72, 52.5f}));

            outerPanelLeft->setWidth("20%");
            outerPanelTop->setHeight("40%");

            REQUIRE(compareVector2f(container->getPosition(), {20, 40}));
            REQUIRE(compareVector2f(container->getSize(), {80, 60}));
            REQUIRE(compareVector2f(innerPanelFill->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(innerPanelFill->getSize(), {64, 45}));

            auto outerPanelRight = tgui::Panel::create({"15%", 0});
            outerPanelRight->setAutoLayout(tgui::AutoLayout::Right);
            parent->add(outerPanelRight);

            REQUIRE(compareVector2f(container->getPosition(), {20, 40}));
            REQUIRE(compareVector2f(container->getSize(), {65, 60}));
            REQUIRE(compareVector2f(innerPanelFill->getPosition(), {0, 0}));
            REQUIRE(compareVector2f(innerPanelFill->getSize(), {52, 45}));
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
            button->setSize({"&.iw - 20", "&.ih"});
            REQUIRE(button->getSize() == tgui::Vector2f(-10, 10));

            // Button width will become positive again
            panel->setSize(200, 100);
            REQUIRE(button->getSize() == tgui::Vector2f(180, 100));
        }

        SECTION("Copying complex layout during layout update (https://forum.tgui.eu/index.php?topic=989)")
        {
            class CustomWidget : public tgui::ClickableWidget
            {
            public:
                using Ptr = std::shared_ptr<CustomWidget>;

                void setSize(const tgui::Layout2d& size) override
                {
                    tgui::Layout2d newSize = size; // NOLINT(performance-unnecessary-copy-initialization)
                    ClickableWidget::setSize(newSize);
                }
            };

            tgui::Panel::Ptr panel = std::make_shared<tgui::Panel>();
            panel->setSize({400, 300});

            CustomWidget::Ptr widget = std::make_shared<CustomWidget>();
            widget->setSize({bindWidth(panel) + 50, bindHeight(panel) * 2});

            // The line below causes CustomWidget::setSize to be executed while looping over the
            // layouts that are connected to the panel.
            panel->setSize({800, 600});
        }

        SECTION("Recursive layouts could crash (https://github.com/texus/TGUI/issues/198)")
        {
            std::streambuf *oldbuf = std::cerr.rdbuf(nullptr);

            auto widget = tgui::ClickableWidget::create();
            widget->setSize({100, 100});
            widget->setSize({100, "5 + height"});
            REQUIRE(widget->getSize().y == 5);

            auto widget2 = tgui::ClickableWidget::create();
            widget2->setSize(bindSize(widget) * 2);

            widget->setSize({100, 100});
            REQUIRE(widget2->getSize() == tgui::Vector2f{200, 200});

            widget->setSize(bindSize(widget2) * 200);
            REQUIRE(widget->getSize() == tgui::Vector2f{0, 0});
            REQUIRE(widget2->getSize() == tgui::Vector2f{0, 0});

            std::cerr.rdbuf(oldbuf);
        }
    }
}
