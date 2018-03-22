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
#include <TGUI/Widgets/VerticalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::string, std::function<Widget::Ptr()>> WidgetFactory::m_constructFunctions =
    {
        {"bitmapbutton", std::make_shared<BitmapButton>},
        {"button", std::make_shared<Button>},
        {"canvas", std::make_shared<Canvas>},
        {"chatbox", std::make_shared<ChatBox>},
        {"checkbox", std::make_shared<CheckBox>},
        {"childwindow", std::make_shared<ChildWindow>},
        {"clickablewidget", std::make_shared<ClickableWidget>},
        {"combobox", std::make_shared<ComboBox>},
        {"editbox", std::make_shared<EditBox>},
        {"grid", std::make_shared<Grid>},
        {"group", std::make_shared<Group>},
        {"horizontallayout", std::make_shared<HorizontalLayout>},
        {"horizontalwrap", std::make_shared<HorizontalWrap>},
        {"knob", std::make_shared<Knob>},
        {"label", std::make_shared<Label>},
        {"listbox", std::make_shared<ListBox>},
        {"menubar", std::make_shared<MenuBar>},
        {"messagebox", std::make_shared<MessageBox>},
        {"panel", std::make_shared<Panel>},
        {"picture", std::make_shared<Picture>},
        {"progressbar", std::make_shared<ProgressBar>},
        {"radiobutton", std::make_shared<RadioButton>},
        {"radiobuttongroup", std::make_shared<RadioButtonGroup>},
        {"rangeslider", std::make_shared<RangeSlider>},
        {"scrollablepanel", std::make_shared<ScrollablePanel>},
        {"scrollbar", std::make_shared<Scrollbar>},
        {"slider", std::make_shared<Slider>},
        {"spinbutton", std::make_shared<SpinButton>},
        {"tabs", std::make_shared<Tabs>},
        {"textbox", std::make_shared<TextBox>},
        {"verticallayout", std::make_shared<VerticalLayout>}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetFactory::setConstructFunction(const std::string& type, const std::function<Widget::Ptr()>& constructor)
    {
        m_constructFunctions[toLower(type)] = constructor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::function<Widget::Ptr()>& WidgetFactory::getConstructFunction(const std::string& type)
    {
        return m_constructFunctions[toLower(type)];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
