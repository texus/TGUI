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

TEST_CASE("[spinControl]")
{
    tgui::SpinControl::Ptr spinControl = tgui::SpinControl::create(0, 10, 5);

    SECTION("Signals")
    {
        spinControl->onValueChange([](){});
        spinControl->onValueChange([](float){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(spinControl)->getSignal("ValueChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(spinControl->getWidgetType() == "SpinControl");
    }

    SECTION("Position and Size")
    {
        spinControl->setPosition(40, 30);
        spinControl->setSize(25, 60);

        REQUIRE(spinControl->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(spinControl->getSize() == tgui::Vector2f(25, 60));
        REQUIRE(spinControl->getFullSize() == spinControl->getSize());
        REQUIRE(spinControl->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Minimum")
    {
        REQUIRE(spinControl->getMinimum() == 0);

        spinControl->setMinimum(2);
        REQUIRE(spinControl->getMinimum() == 2);
        REQUIRE(spinControl->getValue() == 5);
        REQUIRE(spinControl->getMaximum() == 10);

        spinControl->setMinimum(6);
        REQUIRE(spinControl->getMinimum() == 6);
        REQUIRE(spinControl->getValue() == 6);
        REQUIRE(spinControl->getMaximum() == 10);

        spinControl->setMinimum(22);
        REQUIRE(spinControl->getMinimum() == 22);
        REQUIRE(spinControl->getValue() == 22);
        REQUIRE(spinControl->getMaximum() == 22);
    }

    SECTION("Maximum")
    {
        REQUIRE(spinControl->getMaximum() == 10);

        spinControl->setMaximum(17);
        REQUIRE(spinControl->getMinimum() == 0);
        REQUIRE(spinControl->getValue() == 5);
        REQUIRE(spinControl->getMaximum() == 17);

        spinControl->setMaximum(11);
        REQUIRE(spinControl->getMinimum() == 0);
        REQUIRE(spinControl->getValue() == 5);
        REQUIRE(spinControl->getMaximum() == 11);

        spinControl->setMaximum(-2);
        REQUIRE(spinControl->getMinimum() == -2);
        REQUIRE(spinControl->getValue() == -2);
        REQUIRE(spinControl->getMaximum() == -2);
    }

    SECTION("Value")
    {
        REQUIRE(spinControl->getValue() == 5);
        REQUIRE(spinControl->setValue(4));
        REQUIRE_FALSE(spinControl->setValue(-2));
        REQUIRE_FALSE(spinControl->setValue(23));
        REQUIRE(spinControl->setValue(8.5));
    }

    SECTION("Step")
    {
        spinControl->setStep(5);
        REQUIRE(spinControl->getStep() == 5);

        spinControl->setMinimum(20.5f);
        spinControl->setMaximum(50.5f);
        spinControl->setStep(3.0f);
        REQUIRE(spinControl->setValue(26.5f));
        REQUIRE(spinControl->getValue() == 26.5f);

        // If a value is set between two steps then it will be rounded to the nearest allowed value
        REQUIRE(spinControl->setValue(25.8f));
        REQUIRE(spinControl->getValue() == 26.5f);
    }

    SECTION("DecimalPlaces")
    {
        REQUIRE(spinControl->getDecimalPlaces() == 0);

        spinControl->setDecimalPlaces(3);
        REQUIRE(spinControl->getDecimalPlaces() == 3);
    }

    SECTION("UseWideArrows")
    {
        REQUIRE(!spinControl->getUseWideArrows());
        spinControl->setUseWideArrows(true);
        REQUIRE(spinControl->getUseWideArrows());
        spinControl->setUseWideArrows(false);
        REQUIRE(!spinControl->getUseWideArrows());
    }

    SECTION("Events / Signals")
    {
        SECTION("ValueChanged")
        {
            spinControl->setPosition(40, 30);
            spinControl->setSize(25, 60);
            spinControl->setMaximum(20);
            spinControl->setMinimum(10);
            spinControl->setValue(15);

            unsigned int valueChangedCount = 0;
            spinControl->onValueChange(&genericCallback, std::ref(valueChangedCount));

            spinControl->setValue(10);
            REQUIRE(valueChangedCount == 1);

            spinControl->setValue(10);
            REQUIRE(valueChangedCount == 1);

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition(60, 55);
            parent->add(spinControl);

            parent->leftMousePressed({110, 100});
            REQUIRE(valueChangedCount == 2);
            REQUIRE(spinControl->getValue() == 11);
            parent->leftMouseReleased({110, 100});

            parent->leftMousePressed({110, 135});
            parent->leftMouseReleased({110, 135});
            REQUIRE(valueChangedCount == 3);
            REQUIRE(spinControl->getValue() == 10);

            parent->leftMousePressed({110, 135});
            parent->leftMouseReleased({110, 135});
            REQUIRE(valueChangedCount == 3);
            REQUIRE(spinControl->getValue() == 10);
        }
    }

    SECTION("Saving and loading from file")
    {
        spinControl->setMinimum(10);
        spinControl->setMaximum(50);
        spinControl->setValue(20);
        spinControl->setStep(5);
        spinControl->setSpinButtonWidth("80%");

        testSavingWidget("SpinControl", spinControl);
    }
}
