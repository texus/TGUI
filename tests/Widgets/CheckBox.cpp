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

#include "../Tests.hpp"
#include <TGUI/Widgets/CheckBox.hpp>

TEST_CASE("[CheckBox]")
{
    tgui::CheckBox::Ptr checkBox = tgui::CheckBox::create();
    checkBox->getRenderer()->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(checkBox->connect("Checked", [](){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](){}));

        REQUIRE_NOTHROW(checkBox->connect("Checked", [](bool){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](bool){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(checkBox->getWidgetType() == "CheckBox");
    }

    SECTION("Position and Size")
    {
        checkBox->setPosition(40, 30);
        checkBox->setSize(36, 36);
        checkBox->getRenderer()->setBorders(2);

        SECTION("Colored")
        {
            REQUIRE(checkBox->getPosition() == sf::Vector2f(40, 30));
            REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
            REQUIRE(checkBox->getFullSize() == checkBox->getSize());
            REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));

            SECTION("With text")
            {
                checkBox->setText("Test");

                SECTION("Small text")
                {
                    checkBox->setTextSize(20);
                    REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                    REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                    REQUIRE(checkBox->getFullSize().y == checkBox->getSize().y);
                    REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));
                }

                SECTION("Large text")
                {
                    checkBox->setTextSize(50);
                    REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                    REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                    REQUIRE(checkBox->getFullSize().y > checkBox->getSize().y);
                    REQUIRE(checkBox->getWidgetOffset().x == 0);
                    REQUIRE(checkBox->getWidgetOffset().y < 0);
                }
            }
        }

        SECTION("Textured")
        {
            SECTION("Unchecked and Checked images of same size")
            {
                checkBox->getRenderer()->setTextureUnchecked({"resources/CheckBox1.png", {0, 0, 32, 32}});
                checkBox->getRenderer()->setTextureChecked({"resources/CheckBox1.png", {32, 0, 32, 32}});

                REQUIRE(checkBox->getPosition() == sf::Vector2f(40, 30));
                REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                REQUIRE(checkBox->getFullSize() == checkBox->getSize());
                REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));

                SECTION("With text")
                {
                    checkBox->setText("Test");

                    SECTION("Small text")
                    {
                        checkBox->setTextSize(20);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                        REQUIRE(checkBox->getFullSize().y == checkBox->getSize().y);
                        REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));
                    }

                    SECTION("Large text")
                    {
                        checkBox->setTextSize(50);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                        REQUIRE(checkBox->getFullSize().y > checkBox->getSize().y);
                        REQUIRE(checkBox->getWidgetOffset().x == 0);
                        REQUIRE(checkBox->getWidgetOffset().y < 0);
                    }
                }
            }

            SECTION("Unchecked and Checked images of different sizes")
            {
                checkBox->getRenderer()->setTextureUnchecked({"resources/CheckBox2.png", {0, 8, 32, 32}});
                checkBox->getRenderer()->setTextureChecked({"resources/CheckBox2.png", {32, 0, 38, 40}});

                REQUIRE(checkBox->getPosition() == sf::Vector2f(40, 30));
                REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                REQUIRE(checkBox->getFullSize() == sf::Vector2f(40, 42));
                REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, -6));

                SECTION("With text")
                {
                    checkBox->setText("Test");

                    SECTION("Small text")
                    {
                        checkBox->setTextSize(20);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > 40);
                        REQUIRE(checkBox->getFullSize().y == 42);
                        REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, -6));
                    }

                    SECTION("Large text")
                    {
                        checkBox->setTextSize(50);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > 40);
                        REQUIRE(checkBox->getFullSize().y > 42);
                        REQUIRE(checkBox->getWidgetOffset().x == 0);
                        REQUIRE(checkBox->getWidgetOffset().y < -6);
                    }
                }
            }
        }
    }

    SECTION("Checked")
    {
        REQUIRE(!checkBox->isChecked());
        checkBox->check();
        REQUIRE(checkBox->isChecked());
        checkBox->uncheck();
        REQUIRE(!checkBox->isChecked());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(checkBox);
        }

        SECTION("Check / Uncheck")
        {
            checkBox->setPosition(40, 30);
            checkBox->setSize(50, 60);

            unsigned int checkCount = 0;
            unsigned int uncheckCount = 0;
            checkBox->connect("Checked", genericCallback, std::ref(checkCount));
            checkBox->connect("Unchecked", genericCallback, std::ref(uncheckCount));

            checkBox->check();
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 0);

            checkBox->uncheck();
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 1);

            checkBox->leftMousePressed({65, 60});
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 1);

            checkBox->leftMouseReleased({65, 60});
            REQUIRE(checkCount == 2);
            REQUIRE(uncheckCount == 1);

            checkBox->leftMousePressed({65, 60});
            checkBox->leftMouseReleased({65, 60});
            REQUIRE(checkCount == 2);
            REQUIRE(uncheckCount == 2);

            SECTION("Key pressed")
            {
                sf::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = sf::Keyboard::Space;
                checkBox->keyPressed(keyEvent);
                REQUIRE(checkCount == 3);
                REQUIRE(uncheckCount == 2);

                keyEvent.code = sf::Keyboard::Return;
                checkBox->keyPressed(keyEvent);
                REQUIRE(checkCount == 3);
                REQUIRE(uncheckCount == 3);
            }
        }
    }

    SECTION("Saving and loading from file")
    {
        checkBox->check();
        checkBox->setText("SomeText");
        checkBox->setTextSize(25);
        checkBox->setTextClickable(false);

        testSavingWidget("CheckBox", checkBox);
    }
}
