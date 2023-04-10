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


#include <TGUI/Widgets/RadioButtonGroup.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char RadioButtonGroup::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::RadioButtonGroup(const char* typeName, bool initRenderer) :
        Container{typeName, initRenderer}
    {
        setSize({"100%", "100%"});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::Ptr RadioButtonGroup::create()
    {
        return std::make_shared<RadioButtonGroup>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::Ptr RadioButtonGroup::copy(const RadioButtonGroup::ConstPtr& group)
    {
        if (group)
            return std::static_pointer_cast<RadioButtonGroup>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonGroup::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (auto& widget : m_widgets)
        {
            if (widget->getWidgetType() == U"RadioButton")
                std::static_pointer_cast<RadioButton>(widget)->setChecked(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButtonGroup::getCheckedRadioButton() const
    {
        for (const auto& widget : m_widgets)
        {
            if (widget->getWidgetType() != U"RadioButton")
                continue;

            RadioButton::Ptr radioButton = std::static_pointer_cast<RadioButton>(widget);
            if (radioButton->isChecked())
                return radioButton;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButtonGroup::isMouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (!FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            return false;

        const Vector2f offset = getChildWidgetsOffset();
        for (const auto& widget : m_widgets)
        {
            if (widget->isVisible() && widget->isMouseOnWidget(transformMousePos(widget, pos - offset)))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr RadioButtonGroup::clone() const
    {
        return std::make_shared<RadioButtonGroup>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
