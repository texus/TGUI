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


#ifndef TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP

#include <TGUI/Widget.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <iostream>

using PropertyValueMap = std::map<tgui::String, std::pair<tgui::String, tgui::String>>;
using PropertyValueMapPair = std::pair<PropertyValueMap, PropertyValueMap>;

struct WidgetProperties
{
    virtual ~WidgetProperties() = default;

    virtual void updateProperty(tgui::Widget::Ptr widget, const tgui::String& property, const tgui::String& value) const
    {
        if (property == "Left")
            widget->setPosition(value, widget->getPositionLayout().y);
        else if (property == "Top")
            widget->setPosition(widget->getPositionLayout().x, value);
        else if (property == "Width")
            widget->setSize(value, widget->getSizeLayout().y);
        else if (property == "Height")
            widget->setSize(widget->getSizeLayout().x, value);
        else if (property == "Visible")
            widget->setVisible(parseBoolean(value, true));
        else if (property == "Enabled")
            widget->setEnabled(parseBoolean(value, true));
        else if (property == "UserData")
            widget->setUserData(value);
        else // Renderer property
            widget->getRenderer()->setProperty(property, value);
    }

    virtual PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const
    {
        PropertyValueMap pairs;
        pairs["Left"] = {"String", widget->getPositionLayout().x.toString()};
        pairs["Top"] = {"String", widget->getPositionLayout().y.toString()};
        pairs["Width"] = {"String", widget->getSizeLayout().x.toString()};
        pairs["Height"] = {"String", widget->getSizeLayout().y.toString()};
        pairs["Visible"] = {"Bool", tgui::Serializer::serialize(widget->isVisible())};
        pairs["Enabled"] = {"Bool", tgui::Serializer::serialize(widget->isEnabled())};
        try
        {
            pairs["UserData"] = {"String", widget->getUserData<tgui::String>()};
        }
        catch(const std::bad_cast&)
        {
            pairs["UserData"] = {"String", ""};
        }


        PropertyValueMap rendererPairs;
        const auto renderer = widget->getSharedRenderer();
        rendererPairs["Opacity"] = {"Float", tgui::String::fromNumber(renderer->getOpacity())};
        rendererPairs["OpacityDisabled"] = {"Float", tgui::String::fromNumber(renderer->getOpacityDisabled())};
        rendererPairs["Font"] = {"Font", tgui::Serializer::serialize(renderer->getFont())};
        rendererPairs["TransparentTexture"] = {"Bool", tgui::Serializer::serialize(renderer->getTransparentTexture())};
        return {pairs, rendererPairs};
    }


    static bool parseBoolean(tgui::String str, bool defaultValue)
    {
        str = str.trim().toLower();
        if (str == "true" || str == "yes" || str == "on" || str == "y" || str == "t" || str == "1")
            return true;
        else if (str == "false" || str == "no" || str == "off" || str == "n" || str == "f" || str == "0")
            return false;
        else
            return defaultValue;
    }

    static std::vector<tgui::String> deserializeList(const tgui::String& listStr)
    {
        try
        {
            std::stringstream ss{("list = " + listStr + ";").toAnsiString()};
            auto node = tgui::DataIO::parse(ss);

            if (node->propertyValuePairs["list"])
            {
                std::vector<tgui::String> list;
                for (const auto& value : node->propertyValuePairs["list"]->valueList)
                    list.push_back(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());

                return list;
            }
        }
        catch (const tgui::Exception&)
        {
            std::cout << "Failed to deserialize list '" + listStr + "'" << std::endl;
        }

        return {};
    }

    static tgui::String serializeList(std::vector<tgui::String> list)
    {
        if (list.empty())
            return "[]";

        tgui::String itemList = "[" + tgui::Serializer::serialize(list[0]);
        for (std::size_t i = 1; i < list.size(); ++i)
            itemList += ", " + tgui::Serializer::serialize(list[i]);

        itemList += "]";
        return itemList;
    }

    static tgui::Scrollbar::Policy deserializeScrollbarPolicy(tgui::String value)
    {
        value = value.trim().toLower();
        if (value == "always")
            return tgui::Scrollbar::Policy::Always;
        else if (value == "never")
            return tgui::Scrollbar::Policy::Never;
        else
            return tgui::Scrollbar::Policy::Automatic;
    }

    static tgui::String serializeScrollbarPolicy(tgui::Scrollbar::Policy policy)
    {
        if (policy == tgui::Scrollbar::Policy::Always)
            return "Always";
        else if (policy == tgui::Scrollbar::Policy::Never)
            return "Never";
        else
            return "Automatic";
    }
};

#endif // TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP
