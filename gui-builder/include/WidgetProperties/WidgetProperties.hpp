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


#ifndef TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP

#include <TGUI/Config.hpp>
#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/TGUI.hpp>
#endif

using PropertyValueMap = std::map<tgui::String, std::pair<tgui::String, tgui::String>>;
using PropertyValueMapPair = std::pair<PropertyValueMap, PropertyValueMap>;

struct WidgetProperties
{
    virtual ~WidgetProperties() = default;

    virtual void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const
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
        else if (property == "NavigationUp")
            widget->setNavigationUp(widget->getParentGui() ? widget->getParentGui()->get(value) : nullptr);
        else if (property == "NavigationDown")
            widget->setNavigationDown(widget->getParentGui() ? widget->getParentGui()->get(value) : nullptr);
        else if (property == "NavigationLeft")
            widget->setNavigationLeft(widget->getParentGui() ? widget->getParentGui()->get(value) : nullptr);
        else if (property == "NavigationRight")
            widget->setNavigationRight(widget->getParentGui() ? widget->getParentGui()->get(value) : nullptr);
        else if (property == "UserData")
            widget->setUserData(value);
        else if (property == "MouseCursor")
            widget->setMouseCursor(deserializeMouseCursor(value));
        else // Renderer property
            widget->getRenderer()->setProperty(property, value);
    }

    TGUI_NODISCARD virtual PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const
    {
        PropertyValueMap pairs;
        pairs["Left"] = {"Layout", widget->getPositionLayout().x.toString()};
        pairs["Top"] = {"Layout", widget->getPositionLayout().y.toString()};
        pairs["Width"] = {"Layout", widget->getSizeLayout().x.toString()};
        pairs["Height"] = {"Layout", widget->getSizeLayout().y.toString()};
        pairs["Visible"] = {"Bool", tgui::Serializer::serialize(widget->isVisible())};
        pairs["Enabled"] = {"Bool", tgui::Serializer::serialize(widget->isEnabled())};
        pairs["NavigationUp"] = {"String", widget->getNavigationUp() ? widget->getNavigationUp()->getWidgetName() : U""};
        pairs["NavigationDown"] = {"String", widget->getNavigationDown() ? widget->getNavigationDown()->getWidgetName() : U""};
        pairs["NavigationLeft"] = {"String", widget->getNavigationLeft() ? widget->getNavigationLeft()->getWidgetName() : U""};
        pairs["NavigationRight"] = {"String", widget->getNavigationRight() ? widget->getNavigationRight()->getWidgetName() : U""};
        pairs["MouseCursor"] = {"Enum{Arrow,Text,Hand,SizeLeft,SizeRight,SizeTop,SizeBottom,SizeBottomRight,SizeTopLeft,SizeBottomLeft,SizeTopRight,Cross,Help,NotAllowed}", serializeMouseCursor(widget->getMouseCursor())};
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


    TGUI_NODISCARD static bool parseBoolean(tgui::String str, bool defaultValue)
    {
        str = str.trim().toLower();
        if (str == "true" || str == "yes" || str == "on" || str == "y" || str == "t" || str == "1")
            return true;
        else if (str == "false" || str == "no" || str == "off" || str == "n" || str == "f" || str == "0")
            return false;
        else
            return defaultValue;
    }

    TGUI_NODISCARD static std::vector<tgui::String> deserializeList(const tgui::String& listStr)
    {
        try
        {
            std::stringstream ss{("list = " + listStr + ";").toStdString()};
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

    TGUI_NODISCARD static tgui::String serializeList(const std::vector<tgui::String>& list)
    {
        if (list.empty())
            return "[]";

        tgui::String itemList = "[" + tgui::Serializer::serialize(list[0]);
        for (std::size_t i = 1; i < list.size(); ++i)
            itemList += ", " + tgui::Serializer::serialize(list[i]);

        itemList += "]";
        return itemList;
    }

    TGUI_NODISCARD static tgui::Scrollbar::Policy deserializeScrollbarPolicy(tgui::String value)
    {
        value = value.trim().toLower();
        if (value == "always")
            return tgui::Scrollbar::Policy::Always;
        else if (value == "never")
            return tgui::Scrollbar::Policy::Never;
        else
            return tgui::Scrollbar::Policy::Automatic;
    }

    TGUI_NODISCARD static tgui::String serializeScrollbarPolicy(tgui::Scrollbar::Policy policy)
    {
        if (policy == tgui::Scrollbar::Policy::Always)
            return "Always";
        else if (policy == tgui::Scrollbar::Policy::Never)
            return "Never";
        else
            return "Automatic";
    }

    TGUI_NODISCARD static tgui::Cursor::Type deserializeMouseCursor(tgui::String value)
    {
        value = value.trim().toLower();
        if (value == "text")
            return tgui::Cursor::Type::Text;
        else if (value == "hand")
            return tgui::Cursor::Type::Hand;
        else if (value == "sizeleft")
            return tgui::Cursor::Type::SizeLeft;
        else if (value == "sizeright")
            return tgui::Cursor::Type::SizeRight;
        else if (value == "sizetop")
            return tgui::Cursor::Type::SizeTop;
        else if (value == "sizebottom")
            return tgui::Cursor::Type::SizeBottom;
        else if (value == "sizebottomright")
            return tgui::Cursor::Type::SizeBottomRight;
        else if (value == "sizetopleft")
            return tgui::Cursor::Type::SizeTopLeft;
        else if (value == "sizebottomleft")
            return tgui::Cursor::Type::SizeBottomLeft;
        else if (value == "sizetopright")
            return tgui::Cursor::Type::SizeTopRight;
        else if (value == "crosshair")
            return tgui::Cursor::Type::Crosshair;
        else if (value == "help")
            return tgui::Cursor::Type::Help;
        else if (value == "notallowed")
            return tgui::Cursor::Type::NotAllowed;
        else
            return tgui::Cursor::Type::Arrow;
    }

    TGUI_NODISCARD static tgui::String serializeMouseCursor(tgui::Cursor::Type cursor)
    {
        switch (cursor)
        {
            case tgui::Cursor::Type::Text:            return "Text";
            case tgui::Cursor::Type::Hand:            return "Hand";
            case tgui::Cursor::Type::SizeLeft:        return "SizeLeft";
            case tgui::Cursor::Type::SizeRight:       return "SizeRight";
            case tgui::Cursor::Type::SizeTop:         return "SizeTop";
            case tgui::Cursor::Type::SizeBottom:      return "SizeBottom";
            case tgui::Cursor::Type::SizeBottomRight: return "SizeBottomRight";
            case tgui::Cursor::Type::SizeTopLeft:     return "SizeTopLeft";
            case tgui::Cursor::Type::SizeBottomLeft:  return "SizeBottomLeft";
            case tgui::Cursor::Type::SizeTopRight:    return "SizeTopRight";
            case tgui::Cursor::Type::Crosshair:       return "Crosshair";
            case tgui::Cursor::Type::Help:            return "Help";
            case tgui::Cursor::Type::NotAllowed:      return "NotAllowed";
            default:                                  return "Arrow";
        }
    }
};

#endif // TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP
