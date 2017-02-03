/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Gui.hpp>

TGUI_IMPORT_LAYOUT_BIND_FUNCTIONS
using tgui::Layout;
using tgui::Layout2d;

TEST_CASE("[Layouts]") {
    SECTION("constants") {
        Layout l1;
        Layout l2(-20.3f);
        Layout l3{"60"};

        Layout2d l4;
        Layout2d l5({10.f, {" 50 "}});
        Layout2d l6("{.2, -3.5}");

        REQUIRE(l1.getValue() == 0);
        REQUIRE(l2.getValue() == -20.3f);
        REQUIRE(l3.getValue() == 60);

        REQUIRE(l4.getValue() == sf::Vector2f(0, 0));
        REQUIRE(l5.getValue() == sf::Vector2f(10, 50));
        REQUIRE(l6.getValue() == sf::Vector2f(0.2f, -3.5f));
    }

    SECTION("copying layouts") {
        Layout l1;
        Layout l2{2};
        Layout l3 = l2;
        Layout l4;
        Layout l5{0};
        l5 = l2;

        REQUIRE(l1.getValue() == 0);
        REQUIRE(l2.getValue() == 2);
        REQUIRE(l3.getValue() == 2);
        REQUIRE(l4.getValue() == 0);
        REQUIRE(l5.getValue() == 2);

        REQUIRE(l1.getImpl() != l4.getImpl());
        REQUIRE(l2.getImpl() == l3.getImpl());
        REQUIRE(l2.getImpl() == l5.getImpl());

        Layout2d l6(l3, 2);
        REQUIRE(l2.getImpl() == l6.x.getImpl());
        REQUIRE(l2.getImpl() != l6.y.getImpl());
    }

    SECTION("without strings") {
        SECTION("operators") {
            Layout l1{2};
            Layout l2(3);

            REQUIRE((l1 == l2).getValue() == 0);
            REQUIRE((l1 != l2).getValue() == 1);
            REQUIRE((l1 <= l2).getValue() == 1);
            REQUIRE((l1 < l2).getValue() == 1);
            REQUIRE((l1 >= l2).getValue() == 0);
            REQUIRE((l1 > l2).getValue() == 0);

            REQUIRE((l2 == l2).getValue() == 1);
            REQUIRE((l2 != l2).getValue() == 0);
            REQUIRE((l2 <= l2).getValue() == 1);
            REQUIRE((l2 < l2).getValue() == 0);
            REQUIRE((l2 >= l2).getValue() == 1);
            REQUIRE((l2 > l2).getValue() == 0);

            REQUIRE((l1 + l2).getValue() == 5);
            REQUIRE((l1 - l2).getValue() == -1);
            REQUIRE((l1 * l2).getValue() == 6);
            REQUIRE((l1 / l2).getValue() == 2.f/3.f);
            REQUIRE((l1 % l2).getValue() == 2);

            REQUIRE((+Layout{1}).getValue() == 1);
            REQUIRE((+Layout{-2}).getValue() == -2);
            REQUIRE((-Layout{1}).getValue() == -1);
            REQUIRE((-Layout{-2}).getValue() == 2);

            l1 += l2;
            REQUIRE(l1.getValue() == 5);
            REQUIRE(l2.getValue() == 3);

            l2 -= l1;
            REQUIRE(l1.getValue() == 5);
            REQUIRE(l2.getValue() == -2);

            l1 *= l2;
            REQUIRE(l1.getValue() == -10);
            REQUIRE(l2.getValue() == -2);

            l2 /= l1;
            REQUIRE(l1.getValue() == -10);
            REQUIRE(l2.getValue() == 0.2f);

            l1 %= Layout{3};
            REQUIRE(l1.getValue() == -1);
            REQUIRE(l2.getValue() == 0.2f);

            Layout l0 = 0;
            REQUIRE((l0 && l0).getValue() == 0);
            REQUIRE((l0 && l1).getValue() == 0);
            REQUIRE((l1 && l2).getValue() == 1);
            REQUIRE((l0 || l0).getValue() == 0);
            REQUIRE((l0 || l1).getValue() == 1);
            REQUIRE((l1 || l2).getValue() == 1);

            Layout2d l3{-6, 5};
            Layout2d l4(4, 5);
            Layout2d l5(6, 7);

            REQUIRE((l3 == l3).getValue() == 1);
            REQUIRE((l3 != l3).getValue() == 0);
            REQUIRE((l3 == l4).getValue() == 0);
            REQUIRE((l3 != l4).getValue() == 1);
            REQUIRE((l3 == l5).getValue() == 0);

            REQUIRE((l3 + l4).getValue() == sf::Vector2f(-2, 10));
            REQUIRE((l3 - l4).getValue() == sf::Vector2f(-10, 0));

            REQUIRE((-3 * l3).getValue() == sf::Vector2f(18, -15));
            REQUIRE((l3 * 2).getValue() == sf::Vector2f(-12, 10));
            REQUIRE((l3 / 2).getValue() == sf::Vector2f(-3, 2.5f));
            REQUIRE((l3 % 2.5).getValue() == sf::Vector2f(-1, 0));

            REQUIRE((+l3).getValue() == sf::Vector2f(-6, 5));
            REQUIRE((-l3).getValue() == sf::Vector2f(6, -5));

            l3 += l4;
            REQUIRE(l3.getValue() == sf::Vector2f(-2, 10));
            REQUIRE(l4.getValue() == sf::Vector2f(4, 5));

            l4 -= l3;
            REQUIRE(l3.getValue() == sf::Vector2f(-2, 10));
            REQUIRE(l4.getValue() == sf::Vector2f(6, -5));

            l3 *= 2.5;
            REQUIRE(l3.getValue() == sf::Vector2f(-5, 25));

            l4 /= 2;
            REQUIRE(l4.getValue() == sf::Vector2f(3, -2.5));

            l3 %= Layout{-3};
            REQUIRE(l3.getValue() == sf::Vector2f(-2, 1));

            REQUIRE((Layout(5) + Layout(3) * Layout(2) - Layout(1)).getValue() == 10);
            REQUIRE((Layout(5) + Layout(3) * (Layout(2) - Layout(1))).getValue() == 8);
            REQUIRE(((Layout(5) + Layout(3)) * Layout(2) - Layout(1)).getValue() == 15);
            REQUIRE((Layout2d(5, 2) + Layout2d(3, -3) / Layout(2) - Layout2d(-1, 6)).getValue() == sf::Vector2f(7.5f, -5.5f));
            REQUIRE(((Layout2d(5, 2) + Layout2d(3, -3)) / Layout(2) - Layout2d(-1, 6)).getValue() == sf::Vector2f(5, -6.5f));
        }

        SECTION("bind functions") {
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
            
            REQUIRE(bindMin(10, 20).getValue() == 10);
            REQUIRE(bindMin(-50, 5).getValue() == -50);
            REQUIRE(bindMax(-10, -20).getValue() == -10);
            REQUIRE(bindMax(-50, 5).getValue() == 5);
            REQUIRE(bindRange(10, 20, 15).getValue() == 15);
            REQUIRE(bindRange(10, 20, 25).getValue() == 20);
            REQUIRE(bindRange(10, 20, 5).getValue() == 10);

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

            button3->setPosition(2 * bindRight(button1) + bindLeft(button2) / 4 + bindWidth(button1), 50 + bindBottom(button2) % 75 * bindTop(button2));
            REQUIRE(button3->getPosition() == sf::Vector2f(995, 3050));
            REQUIRE(button3->getAbsolutePosition() == sf::Vector2f(1005, 3075));

            button3->setSize(bindIf(bindWidth(button1) > bindHeight(button2), 2 * bindWidth(button1), bindWidth(button2) / 4) * 3, 0);
            REQUIRE(button3->getSize().x == 1800);
            button1->setSize(30, 10);
            REQUIRE(button3->getSize().x == 300);

            button3->setPosition(bindIf(bindLeft(button1) != bindTop(button2), bindPosition(button1), 1.5 * bindPosition(button2)));
            REQUIRE(button3->getPosition() == sf::Vector2f(40, 60));
            button1->setPosition(75, 20);
            REQUIRE(button3->getPosition() == sf::Vector2f(90, 112.5f));

            button3->setPosition(bindIf(bindPosition(button1) == bindPosition(button2), bindPosition(button1) * 1.5, bindPosition(button2)));
            REQUIRE(button3->getPosition() == sf::Vector2f(60, 75));
            button1->setPosition(60, 75);
            REQUIRE(button3->getPosition() == sf::Vector2f(90, 112.5f));
        }
    }

    SECTION("with strings") {
        SECTION("operators") {
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
            REQUIRE(Layout("5 % 3").getValue() == 2);

            REQUIRE(Layout("0 && 0").getValue() == 0);
            REQUIRE(Layout("0 && 2").getValue() == 0);
            REQUIRE(Layout("3 && 1").getValue() == 1);
            REQUIRE(Layout("0 || 0").getValue() == 0);
            REQUIRE(Layout("0 || 2").getValue() == 1);
            REQUIRE(Layout("3 || 1").getValue() == 1);
            REQUIRE(Layout("2 and 2").getValue() == 1);
            REQUIRE(Layout("2 or 0").getValue() == 1);

            REQUIRE(Layout("2 == 0").getValue() == 0);
            REQUIRE(Layout("4 == 9").getValue() == 0);
            REQUIRE(Layout("3 == 3").getValue() == 1);
            REQUIRE(Layout("2 != 0").getValue() == 1);
            REQUIRE(Layout("4 != 9").getValue() == 1);
            REQUIRE(Layout("3 != 3").getValue() == 0);
            REQUIRE(Layout("2 < 0").getValue() == 0);
            REQUIRE(Layout("4 < 9").getValue() == 1);
            REQUIRE(Layout("3 < 3").getValue() == 0);
            REQUIRE(Layout("2 > 0").getValue() == 1);
            REQUIRE(Layout("4 > 9").getValue() == 0);
            REQUIRE(Layout("3 > 3").getValue() == 0);
            REQUIRE(Layout("2 <= 0").getValue() == 0);
            REQUIRE(Layout("4 <= 9").getValue() == 1);
            REQUIRE(Layout("3 <= 3").getValue() == 1);
            REQUIRE(Layout("2 >= 0").getValue() == 1);
            REQUIRE(Layout("4 >= 9").getValue() == 0);
            REQUIRE(Layout("3 >= 3").getValue() == 1);

            REQUIRE(Layout2d("{2, -1}").getValue() == sf::Vector2f(2, -1));
            REQUIRE(Layout2d("+{2, -1}").getValue() == sf::Vector2f(2, -1));
            REQUIRE(Layout2d("-{2, -1}").getValue() == sf::Vector2f(-2, 1));
            REQUIRE(Layout2d("({2, -1})").getValue() == sf::Vector2f(2, -1));
            REQUIRE(Layout2d("+(+{2, -1})").getValue() == sf::Vector2f(2, -1));
            REQUIRE(Layout2d("-(-{2, -1})").getValue() == sf::Vector2f(2, -1));
            REQUIRE(Layout2d("{2, 3} + {-1, 5}").getValue() == sf::Vector2f(1, 8));
            REQUIRE(Layout2d("{2, 3} - {-1, 5}").getValue() == sf::Vector2f(3, -2));
            REQUIRE(Layout2d("{2, 3} + -{-1, 5}").getValue() == sf::Vector2f(3, -2));
            REQUIRE(Layout2d("{2, 3} * -2").getValue() == sf::Vector2f(-4, -6));
            REQUIRE(Layout2d("{2, 3} / 4").getValue() == sf::Vector2f(0.5f, 0.75f));
            REQUIRE(Layout2d("{2, 3} % 1.5").getValue() == sf::Vector2f(0.5f, 0));
            REQUIRE(Layout2d("2 * {2, 3}").getValue() == sf::Vector2f(4, 6));

            REQUIRE(Layout("5 + 3 * 2 - 1").getValue() == 10);
            REQUIRE(Layout("5 + 3 * (2 - 1)").getValue() == 8);
            REQUIRE(Layout("(5 + 3) * 2 - 1").getValue() == 15);
            REQUIRE(Layout2d("{5,2} + {3,-3} / 2 - {-1, 6}").getValue() == sf::Vector2f(7.5f, -5.5f));
            REQUIRE(Layout2d("({5,2} + {3,-3}) / 2 - {-1, 6}").getValue() == sf::Vector2f(5, -6.5f));

            REQUIRE(Layout("{3, 2}").getValue() == 0);
            REQUIRE(Layout("(5").getValue() == 0);
            REQUIRE(Layout("6)").getValue() == 0);
            REQUIRE(Layout("xyz").getValue() == 0);
            REQUIRE(Layout("width").getValue() == 0);
            REQUIRE(Layout2d("5").getValue() == sf::Vector2f(5, 5));
            REQUIRE(Layout2d("{1, 2} + 5").getValue() == sf::Vector2f(6, 7));
            REQUIRE(Layout2d("[1, 2]").getValue() == sf::Vector2f(0, 0));
            REQUIRE(Layout2d("{1").getValue() == sf::Vector2f(0, 0));
            REQUIRE(Layout2d("2}").getValue() == sf::Vector2f(0, 0));
            REQUIRE(Layout2d("{3}").getValue() == sf::Vector2f(0, 0));

            REQUIRE(Layout("min()").getValue() == 0);
            REQUIRE(Layout("min(2)").getValue() == 2);
            REQUIRE(Layout("min(5, 3)").getValue() == 3);
            REQUIRE(Layout("min(4, 2, 3)").getValue() == 2);
            REQUIRE(Layout("max()").getValue() == 0);
            REQUIRE(Layout("max(2)").getValue() == 2);
            REQUIRE(Layout("max(5, 3)").getValue() == 5);
            REQUIRE(Layout("max(2, 4, 3)").getValue() == 4);
            REQUIRE(Layout("range()").getValue() == 0);
            REQUIRE(Layout("range(5)").getValue() == 0);
            REQUIRE(Layout("range(1, 3)").getValue() == 0);
            REQUIRE(Layout("range(10, 20, 15)").getValue() == 15);
            REQUIRE(Layout("range(10, 20, 25)").getValue() == 20);
            REQUIRE(Layout("range(10, 20, 5)").getValue() == 10);
            REQUIRE(Layout("range(1, 4, 2, 3)").getValue() == 0);
            REQUIRE(Layout("min(5, 3) + range(10, 20, 12) * 2").getValue() == 27);

            REQUIRE(Layout("1 < 2 ? 5 : 6").getValue() == 5);
            REQUIRE(Layout("1 == 2 ? 5 : 6").getValue() == 6);
            REQUIRE(Layout("1 <= 2 ? 3 ? 5 : 6 : 7").getValue() == 5);
            REQUIRE(Layout("1 != 2 ? 0 ? 5 : 6 : 7").getValue() == 6);
            REQUIRE(Layout("1 > 2 ? 0 ? 5 : 6 : 7").getValue() == 7);
            REQUIRE(Layout("1 ? 2").getValue() == 0);
            REQUIRE(Layout("1 : 2").getValue() == 0);

            REQUIRE(Layout("if 1 < 2 then 5 else 6").getValue() == 5);
            REQUIRE(Layout("if 1 == 2 then 5 else 6").getValue() == 6);
            REQUIRE(Layout("if 1 <= 2 then if 3 then 5 else 6 else 7").getValue() == 5);
            REQUIRE(Layout("if 1 != 2 then if 0 then 5 else 6 else 7").getValue() == 6);
            REQUIRE(Layout("if 1 > 2 then if 0 then 5 else 6 else 7").getValue() == 7);
            REQUIRE(Layout("if widgetif.h then widgetif.x else widgetif.y + if 0 then widgetif.w else 2").getValue() == 2);
            REQUIRE(Layout("if ifwidget.h then ifwidget.x else ifwidget.y + if 0 then ifwidget.w else 2").getValue() == 2);
            REQUIRE(Layout("if widgetthen.h then widgetthen.x else widgetthen.y + if 0 then widgetthen.w else 2").getValue() == 2);
            REQUIRE(Layout("if thenwidget.h then thenwidget.x else thenwidget.y + if 0 then thenwidget.w else 2").getValue() == 2);
            REQUIRE(Layout("if widgetelse.h then widgetelse.x else widgetelse.y + if 0 then widgetelse.w else 2").getValue() == 2);
            REQUIRE(Layout("if elsewidget.h then elsewidget.x else elsewidget.y + if 0 then elsewidget.w else 2").getValue() == 2);
            REQUIRE(Layout("if 2 * if 1 then 2 else 3 < 5 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if if wif.x then wif.x.y + if wif.w then 5 else wif.h else 7 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if if ifw.x then ifw.x.y + if ifw.w then 5 else ifw.h else 7 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if if wthen.x then wthen.x.y + if wthen.w then 5 else wthen.h else 7 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if if thenw.x then thenw.x.y + if thenw.w then 5 else thenw.h else 7 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if if welse.x then welse.x.y + if welse.w then 5 else welse.h else 7 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if if elsew.x then elsew.x.y + if elsew.w then 5 else elsew.h else 7 then 8 else 9").getValue() == 8);
            REQUIRE(Layout("if 1 then 2").getValue() == 0);
            REQUIRE(Layout("if 0 else 2").getValue() == 0);
        }

        SECTION("bind functions") {
            auto panel = std::make_shared<tgui::Panel>();
            panel->setSize(200, 180);
            panel->setPosition(10, 25);
        
            auto button1 = std::make_shared<tgui::Button>();
            button1->setSize(300, 50);
            button1->setPosition(40, 60);
            panel->add(button1, "b1");
            
            auto button2 = std::make_shared<tgui::Button>();
            button2->setPosition(bindStr2d("b1.position"));
            button2->setSize(bindStr2d(std::string{"b1.size"}));
            REQUIRE(button2->getSize() == sf::Vector2f(0, 0));
            REQUIRE(button2->getPosition() == sf::Vector2f(0, 0));

            panel->add(button2, "b2");
            REQUIRE(button2->getSize() == sf::Vector2f(300, 50));
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            
            button2->setPosition({"b1.p"});
            REQUIRE(button2->getPosition() == sf::Vector2f(0, 0));
            
            button2->setPosition({"b1.pos"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            
            button2->setPosition(bindStr("b1.left"), bindStr(std::string{"b1.top"}));
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));
            
            button2->setPosition({"b1.x"}, {"b1.y"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));

            button2->setSize({"{b1.width, b1.height}"});
            REQUIRE(button2->getSize() == sf::Vector2f(300, 50));
            
            button2->setSize({"{b1.wi, b1.he}"});
            REQUIRE(button2->getSize() == sf::Vector2f(0, 0));
            
            button2->setSize({"{b1.w, b1.h}"});
            REQUIRE(button2->getSize() == sf::Vector2f(300, 50));
            
            button2->setSize({"{b1.right, b1.bottom}"});
            REQUIRE(button2->getSize() == sf::Vector2f(340, 110));
            
            button2->setSize({"{b1.r, b1.b}"});
            REQUIRE(button2->getSize() == sf::Vector2f(0, 0));
            
            button2->setSize({{"b1.right"}, {"b1.bottom"}});
            REQUIRE(button2->getSize() == sf::Vector2f(340, 110));
            
            button2->setSize({"{@, #}"});
            REQUIRE(button2->getSize() == sf::Vector2f(0, 0));
            
            button2->setPosition({"parent.x"}, {"&.y"});
            REQUIRE(button2->getPosition() == sf::Vector2f(10, 25));
            
            button2->setPosition({"parent.b1.parent.b1.x"}, {"&.b1.&.b1.y"});
            REQUIRE(button2->getPosition() == sf::Vector2f(40, 60));

            button2->setPosition({"b1.position"});
            button2->setSize({"b1.size"});
            button1->setSize(400, 40);
            button1->setPosition(60, 75);
            REQUIRE(button2->getSize() == sf::Vector2f(400, 40));
            REQUIRE(button2->getPosition() == sf::Vector2f(60, 75));
            
            auto button3 = std::make_shared<tgui::Button>();
            button1->setSize(300, 50);
            button1->setPosition(40, 60);
            button2->setSize(400, 40);
            button2->setPosition(60, 75);
            panel->add(button3);

            button3->setSize({"2.5 * b1.pos + b2.size / 4 + {100, 50}"});
            REQUIRE(button3->getSize() == sf::Vector2f(300, 210));

            button3->setPosition({"2 * b1.right + b2.x / 4 + b1.w"}, {"50 + b2.bottom % 75 * b2.y"});
            REQUIRE(button3->getPosition() == sf::Vector2f(995, 3050));
            REQUIRE(button3->getAbsolutePosition() == sf::Vector2f(1005, 3075));
            
            button3->setSize({"(if b1.w > b2.h then 2 * b1.w else b2.w / 4) * 3"});
            REQUIRE(button3->getSize().x == 1800);
            button1->setSize(30, 10);
            REQUIRE(button3->getSize().x == 300);

            button3->setSize({"(if b1.w <= b2.h then 2 * b1.w else b2.w / 4) * 3"});
            REQUIRE(button3->getSize().x == 180);
            button2->setSize(200, 50);
            button1->setSize(300, 50);
            REQUIRE(button3->getSize().x == 150);

            button3->setPosition(bindIf(bindLeft(button1) != bindTop(button2), bindPosition(button1), 1.5 * bindPosition(button2)));
            button3->setPosition({"b1.x != b2.y ? b1.pos : 1.5 * b2.position"});
            REQUIRE(button3->getPosition() == sf::Vector2f(40, 60));
            button1->setPosition(75, 20);
            REQUIRE(button3->getPosition() == sf::Vector2f(90, 112.5f));

            button3->setPosition({"if b1.pos == b2.pos then b1.pos * 1.5 else b2.pos"});
            REQUIRE(button3->getPosition() == sf::Vector2f(60, 75));
            button1->setPosition(60, 75);
            REQUIRE(button3->getPosition() == sf::Vector2f(90, 112.5f));

            auto button4 = std::make_shared<tgui::Button>();
            button4->setSize(200, 50);

            panel->add(button4, "xyzifxyz");
            button3->setSize({"xyzifxyz.size"});
            REQUIRE(button3->getSize() == sf::Vector2f(200, 50));

            panel->add(button4, "ifthenelse");
            button3->setSize({"ifthenelse.size"});
            REQUIRE(button3->getSize() == sf::Vector2f(200, 50));

            panel->add(button4, "then");
            button3->setSize({"then.size"});
            REQUIRE(button3->getSize() == sf::Vector2f(200, 50));

            SECTION("Gui") {
                sf::RenderTexture texture;
                texture.create(20, 15);
                tgui::Gui gui{texture};

                auto left = bindLeft(gui);
                auto top = bindTop(gui);
                auto width = bindWidth(gui);
                auto height = bindHeight(gui);
                auto right = bindRight(gui);
                auto bottom = bindBottom(gui);
                auto position = bindPosition(gui);
                auto size = bindSize(gui);

                REQUIRE(width.getValue() == 20);
                REQUIRE(height.getValue() == 15);
                REQUIRE(size.getValue() == sf::Vector2f(20, 15));

                REQUIRE(left.getValue() == 0);
                REQUIRE(top.getValue() == 0);
                REQUIRE(right.getValue() == 20);
                REQUIRE(bottom.getValue() == 15);
                REQUIRE(position.getValue() == sf::Vector2f(0, 0));

                gui.setView(sf::View{{4, 3, 40, 30}});

                REQUIRE(width.getValue() == 40);
                REQUIRE(height.getValue() == 30);
                REQUIRE(size.getValue() == sf::Vector2f(40, 30));

                // The position of the view is not set in the gui
                // This view position will be applied when rendering and is thus not allowed to be taken into account when positioning widgets
                REQUIRE(left.getValue() == 0);
                REQUIRE(top.getValue() == 0);
                REQUIRE(right.getValue() == 40);
                REQUIRE(bottom.getValue() == 30);
                REQUIRE(position.getValue() == sf::Vector2f(0, 0));
            }
        }

        SECTION("No ambiguity with 0") {
            auto widget = std::make_shared<tgui::ClickableWidget>();
            widget->setPosition({0, 0});
            widget->setPosition(0,0);
            widget->setPosition({"0","0"});
            widget->setPosition({"0"},{"0"});
            widget->setPosition("{0,0}");
            widget->setPosition("0","0");
        }
    }

    SECTION("Bug Fixes") {
        SECTION("Setting negative size and reverting back to positive (https://github.com/texus/TGUI/issues/54)") {
            tgui::Panel::Ptr panel = std::make_shared<tgui::Panel>();
            tgui::Button::Ptr button = std::make_shared<tgui::Button>();
            panel->add(button);

            // Button width becomes -10
            panel->setSize(10, 10);
            button->setSize({"{&.w - 20, &.h}"});
            REQUIRE(button->getSize() == sf::Vector2f(-10, 10));

            // Button width will become positive again
            panel->setSize(200, 100);
            REQUIRE(button->getSize() == sf::Vector2f(180, 100));
        }
    }
}
