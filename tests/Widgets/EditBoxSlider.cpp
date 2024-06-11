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

TEST_CASE("[editBoxSlider]")
{
    tgui::EditBoxSlider::Ptr editBoxSlider = tgui::EditBoxSlider::create(0, 10, 5);

    SECTION("Signals")
    {
        editBoxSlider->onValueChange([](){});
        editBoxSlider->onValueChange([](float){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(editBoxSlider)->getSignal("ValueChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(editBoxSlider->getWidgetType() == "EditBoxSlider");
    }

    SECTION("Position and Size")
    {
        editBoxSlider->setPosition(40, 30);
        editBoxSlider->setSize(25, 60);

        REQUIRE(editBoxSlider->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(editBoxSlider->getSize() == tgui::Vector2f(25, 60));
        REQUIRE(editBoxSlider->getFullSize().x >= editBoxSlider->getSize().x);
        REQUIRE(editBoxSlider->getFullSize().y >= editBoxSlider->getSize().y);
        REQUIRE(editBoxSlider->getWidgetOffset().x <= 0);
        REQUIRE(editBoxSlider->getWidgetOffset().y == 0);
    }

    SECTION("Minimum")
    {
        REQUIRE(editBoxSlider->getMinimum() == 0);

        editBoxSlider->setMinimum(2);
        REQUIRE(editBoxSlider->getMinimum() == 2);
        REQUIRE(editBoxSlider->getValue() == 5);
        REQUIRE(editBoxSlider->getMaximum() == 10);

        editBoxSlider->setMinimum(6);
        REQUIRE(editBoxSlider->getMinimum() == 6);
        REQUIRE(editBoxSlider->getValue() == 6);
        REQUIRE(editBoxSlider->getMaximum() == 10);

        editBoxSlider->setMinimum(22);
        REQUIRE(editBoxSlider->getMinimum() == 22);
        REQUIRE(editBoxSlider->getValue() == 22);
        REQUIRE(editBoxSlider->getMaximum() == 22);
    }

    SECTION("Maximum")
    {
        REQUIRE(editBoxSlider->getMaximum() == 10);

        editBoxSlider->setMaximum(17);
        REQUIRE(editBoxSlider->getMinimum() == 0);
        REQUIRE(editBoxSlider->getValue() == 5);
        REQUIRE(editBoxSlider->getMaximum() == 17);

        editBoxSlider->setMaximum(11);
        REQUIRE(editBoxSlider->getMinimum() == 0);
        REQUIRE(editBoxSlider->getValue() == 5);
        REQUIRE(editBoxSlider->getMaximum() == 11);

        editBoxSlider->setMaximum(-2);
        REQUIRE(editBoxSlider->getMinimum() == -2);
        REQUIRE(editBoxSlider->getValue() == -2);
        REQUIRE(editBoxSlider->getMaximum() == -2);
    }

    SECTION("Value")
    {
        REQUIRE(editBoxSlider->getValue() == 5);
        REQUIRE(editBoxSlider->setValue(4));
        REQUIRE_FALSE(editBoxSlider->setValue(-2));
        REQUIRE_FALSE(editBoxSlider->setValue(23));
        REQUIRE(editBoxSlider->setValue(8.5));
    }

    SECTION("Step")
    {
        editBoxSlider->setStep(5);
        REQUIRE(editBoxSlider->getStep() == 5);

        editBoxSlider->setMinimum(20.5f);
        editBoxSlider->setMaximum(50.5f);
        editBoxSlider->setStep(3.0f);
        REQUIRE(editBoxSlider->setValue(26.5f));
        REQUIRE(editBoxSlider->getValue() == 26.5f);

        // If a value is set between two steps then it will be rounded to the nearest allowed value
        REQUIRE(editBoxSlider->setValue(25.8f));
        REQUIRE(editBoxSlider->getValue() == 26.5f);
    }

    SECTION("DecimalPlaces")
    {
        REQUIRE(editBoxSlider->getDecimalPlaces() == 0);

        editBoxSlider->setDecimalPlaces(3);
        REQUIRE(editBoxSlider->getDecimalPlaces() == 3);
    }

    SECTION("Events / Signals")
    {
        SECTION("ValueChanged")
        {
            editBoxSlider->setPosition(40, 30);
            editBoxSlider->setSize(25, 60);
            editBoxSlider->setMaximum(20);
            editBoxSlider->setMinimum(10);
            editBoxSlider->setValue(15);

            unsigned int valueChangedCount = 0;
            editBoxSlider->onValueChange(&genericCallback, std::ref(valueChangedCount));

            editBoxSlider->setValue(10);
            REQUIRE(valueChangedCount == 1);

            editBoxSlider->setValue(10);
            REQUIRE(valueChangedCount == 1);

            editBoxSlider->setValue(12);
            REQUIRE(valueChangedCount == 2);
        }
    }

    SECTION("Saving and loading from file")
    {
        editBoxSlider->setMinimum(10);
        editBoxSlider->setMaximum(50);
        editBoxSlider->setValue(20);
        editBoxSlider->setStep(5);

        testSavingWidget("EditBoxSlider", editBoxSlider);
    }
}
