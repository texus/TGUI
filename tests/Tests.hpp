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

#include "catch.hpp"
#include <TGUI/Container.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/Serializer.hpp>

static const sf::Time DOUBLE_CLICK_TIMEOUT = sf::milliseconds(500);

bool compareFiles(const std::string& leftFileName, const std::string& rightFileName);

void mouseCallback(unsigned int& count, sf::Vector2f pos);
void genericCallback(unsigned int& count);

void testWidgetSignals(tgui::Widget::Ptr widget);
void testClickableWidgetSignals(tgui::ClickableWidget::Ptr widget);

template <typename WidgetType>
void copy(std::shared_ptr<tgui::Container> parent, std::shared_ptr<WidgetType> widget)
{
    REQUIRE(WidgetType::copy(nullptr) == nullptr);

    parent->removeAllWidgets();

    // Copy constructor
    WidgetType temp1(*widget);

    // Assignment operator
    WidgetType temp2;
    temp2 = temp1;

    // Move constructor
    WidgetType temp3 = std::move(temp2);

    // Move assignment operator
    WidgetType temp4;
    temp4 = std::move(temp3);

    parent->add(WidgetType::copy(std::make_shared<WidgetType>(temp4)));
}
