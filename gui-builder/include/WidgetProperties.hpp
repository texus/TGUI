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


#ifndef TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP

#include <TGUI/Widget.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <iostream>

using PropertyValueMap = std::map<std::string, std::pair<std::string, std::string>>;
using PropertyValueMapPair = std::pair<PropertyValueMap, PropertyValueMap>;

struct WidgetProperties
{
    virtual ~WidgetProperties() = default;

    virtual void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const
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
        else // Renderer property
            widget->getRenderer()->setProperty(property, value);
    }

    virtual PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const
    {
        PropertyValueMap pairs;
        pairs["Left"] = {"Float", tgui::to_string(widget->getPosition().x)};
        pairs["Top"] = {"Float", tgui::to_string(widget->getPosition().y)};
        pairs["Width"] = {"Float", tgui::to_string(widget->getSize().x)};
        pairs["Height"] = {"Float", tgui::to_string(widget->getSize().y)};
        pairs["Visible"] = {"Bool", tgui::Serializer::serialize(widget->isVisible())};
        pairs["Enabled"] = {"Bool", tgui::Serializer::serialize(widget->isEnabled())};

        PropertyValueMap rendererPairs;
        const auto renderer = widget->getSharedRenderer();
        rendererPairs["Opacity"] = {"Byte", tgui::to_string(renderer->getOpacity())};
        rendererPairs["Font"] = {"Font", renderer->getFont().getId()};
        rendererPairs["TransparentTexture"] = {"Bool", tgui::Serializer::serialize(renderer->getTransparentTexture())};
        return {pairs, rendererPairs};
    }

protected:

    static bool parseBoolean(std::string str, bool defaultValue)
    {
        str = tgui::toLower(tgui::trim(str));
        if (str == "true" || str == "yes" || str == "on" || str == "y" || str == "t" || str == "1")
            return true;
        else if (str == "false" || str == "no" || str == "off" || str == "n" || str == "f" || str == "0")
            return false;
        else
            return defaultValue;
    }

    static std::vector<sf::String> deserializeList(const std::string& listStr)
    {
        try
        {
            std::stringstream ss{"list = " + listStr + ";"};
            auto node = tgui::DataIO::parse(ss);

            if (node->propertyValuePairs["list"])
            {
                std::vector<sf::String> list;
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

    static std::string serializeList(std::vector<sf::String> list)
    {
        if (list.empty())
            return "[]";

        std::string itemList = "[" + tgui::Serializer::serialize(list[0]);
        for (std::size_t i = 1; i < list.size(); ++i)
            itemList += ", " + tgui::Serializer::serialize(list[i]);

        itemList += "]";
        return itemList;
    }
};

#endif // TGUI_GUI_BUILDER_WIDGET_PROPERTIES_HPP
