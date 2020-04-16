/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/Widgets/BitmapButton.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Canvas.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/ListView.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/RadioButtonGroup.hpp>
#include <TGUI/Widgets/RangeSlider.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<String, std::function<Widget::Ptr()>> WidgetFactory::m_constructFunctions =
    {
        {"BitmapButton", std::make_shared<BitmapButton>},
        {"Button", std::make_shared<Button>},
        {"Canvas", std::make_shared<Canvas>},
        {"ChatBox", std::make_shared<ChatBox>},
        {"CheckBox", std::make_shared<CheckBox>},
        {"ChildWindow", std::make_shared<ChildWindow>},
        {"ClickableWidget", std::make_shared<ClickableWidget>},
        {"ComboBox", std::make_shared<ComboBox>},
        {"EditBox", std::make_shared<EditBox>},
        {"Grid", std::make_shared<Grid>},
        {"Group", std::make_shared<Group>},
        {"HorizontalLayout", std::make_shared<HorizontalLayout>},
        {"HorizontalWrap", std::make_shared<HorizontalWrap>},
        {"Knob", std::make_shared<Knob>},
        {"Label", std::make_shared<Label>},
        {"ListBox", std::make_shared<ListBox>},
        {"ListView", std::make_shared<ListView>},
        {"MenuBar", std::make_shared<MenuBar>},
        {"MessageBox", std::make_shared<MessageBox>},
        {"Panel", std::make_shared<Panel>},
        {"Picture", std::make_shared<Picture>},
        {"ProgressBar", std::make_shared<ProgressBar>},
        {"RadioButton", std::make_shared<RadioButton>},
        {"RadioButtonGroup", std::make_shared<RadioButtonGroup>},
        {"RangeSlider", std::make_shared<RangeSlider>},
        {"ScrollablePanel", std::make_shared<ScrollablePanel>},
        {"Scrollbar", std::make_shared<Scrollbar>},
        {"Slider", std::make_shared<Slider>},
        {"SpinButton", std::make_shared<SpinButton>},
        {"Tabs", std::make_shared<Tabs>},
        {"TextBox", std::make_shared<TextBox>},
        {"TreeView", std::make_shared<TreeView>},
        {"VerticalLayout", std::make_shared<VerticalLayout>}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetFactory::setConstructFunction(const String& type, const std::function<Widget::Ptr()>& constructor)
    {
        m_constructFunctions[type] = constructor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::function<Widget::Ptr()>& WidgetFactory::getConstructFunction(const String& type)
    {
        return m_constructFunctions[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
