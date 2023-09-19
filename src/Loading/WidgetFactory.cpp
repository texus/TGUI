/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/AllWidgets.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<String, std::function<Widget::Ptr()>> WidgetFactory::m_constructFunctions =
    {
        {"BitmapButton", std::make_shared<BitmapButton>},
        {"Button", std::make_shared<Button>},
        {"ChatBox", std::make_shared<ChatBox>},
        {"CheckBox", std::make_shared<CheckBox>},
        {"ChildWindow", std::make_shared<ChildWindow>},
        {"ClickableWidget", std::make_shared<ClickableWidget>},
        {"ColorPicker", std::make_shared<ColorPicker>},
        {"ComboBox", std::make_shared<ComboBox>},
        {"EditBox", std::make_shared<EditBox>},
        {"FileDialog", std::make_shared<FileDialog>},
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
        {"PanelListBox", std::make_shared<PanelListBox>},
        {"Picture", std::make_shared<Picture>},
        {"ProgressBar", std::make_shared<ProgressBar>},
        {"RadioButton", std::make_shared<RadioButton>},
        {"RadioButtonGroup", std::make_shared<RadioButtonGroup>},
        {"RangeSlider", std::make_shared<RangeSlider>},
        {"RichTextLabel", std::make_shared<RichTextLabel>},
        {"ScrollablePanel", std::make_shared<ScrollablePanel>},
        {"Scrollbar", std::make_shared<Scrollbar>},
        {"SeparatorLine", std::make_shared<SeparatorLine>},
        {"Slider", std::make_shared<Slider>},
        {"SpinButton", std::make_shared<SpinButton>},
        {"SpinControl", std::make_shared<SpinControl>},
        {"Tabs", std::make_shared<Tabs>},
        {"TabContainer", std::make_shared<TabContainer>},
        {"TextArea", std::make_shared<TextArea>},
        {"ToggleButton", std::make_shared<ToggleButton>},
        {"TreeView", std::make_shared<TreeView>},
        {"VerticalLayout", std::make_shared<VerticalLayout>}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetFactory::setConstructFunction(const String& type, std::function<Widget::Ptr()> constructor)
    {
        m_constructFunctions[type] = std::move(constructor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::function<Widget::Ptr()> WidgetFactory::getConstructFunction(const String& type)
    {
        // If the type is not found then we will return a nullptr, without altering the list that could be returned by getWidgetTypes()
        auto it = m_constructFunctions.find(type);
        if (it != m_constructFunctions.end())
            return it->second;
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> WidgetFactory::getWidgetTypes()
    {
        std::vector<String> types;
        types.reserve(m_constructFunctions.size());
        for (const auto& pair : m_constructFunctions)
            types.push_back(pair.first);

        return types;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
