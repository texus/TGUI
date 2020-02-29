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


#ifndef TGUI_GUI_BUILDER_CHILD_WINDOW_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_CHILD_WINDOW_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/ChildWindow.hpp>

struct ChildWindowProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const sf::String& value) const override
    {
        auto childWindow = std::dynamic_pointer_cast<tgui::ChildWindow>(widget);
        if (property == "TitleAlignment")
            childWindow->setTitleAlignment(deserializeTitleAlignment(value));
        else if (property == "TitleButtons")
            childWindow->setTitleButtons(deserializeTitleButtons(value));
        else if (property == "Title")
            childWindow->setTitle(value);
        else if (property == "KeepInParent")
            childWindow->setKeepInParent(parseBoolean(value, false));
        else if (property == "Resizable")
            childWindow->setResizable(parseBoolean(value, false));
        else if (property == "PositionLocked")
            childWindow->setPositionLocked(parseBoolean(value, false));
        else if (property == "MinimumWidth")
            childWindow->setMinimumSize({tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber(), childWindow->getMinimumSize().y});
        else if (property == "MinimumHeight")
            childWindow->setMinimumSize({childWindow->getMinimumSize().x, tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber()});
        else if (property == "MaximumWidth")
            childWindow->setMaximumSize({tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber(), childWindow->getMaximumSize().y});
        else if (property == "MaximumHeight")
            childWindow->setMaximumSize({childWindow->getMaximumSize().x, tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber()});
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto childWindow = std::dynamic_pointer_cast<tgui::ChildWindow>(widget);
        pair.first["TitleAlignment"] = {"Enum{Left,Center,Right}", serializeTitleAlignment(childWindow->getTitleAlignment())};
        pair.first["TitleButtons"] = {"ChildWindowTitleButtons", serializeTitleButtons(childWindow->getTitleButtons())};
        pair.first["Title"] = {"String", childWindow->getTitle()};
        pair.first["KeepInParent"] = {"Bool", tgui::Serializer::serialize(childWindow->isKeptInParent())};
        pair.first["Resizable"] = {"Bool", tgui::Serializer::serialize(childWindow->isResizable())};
        pair.first["PositionLocked"] = {"Bool", tgui::Serializer::serialize(childWindow->isPositionLocked())};
        pair.first["MinimumWidth"] = {"Float", tgui::to_string(childWindow->getMinimumSize().x)};
        pair.first["MinimumHeight"] = {"Float", tgui::to_string(childWindow->getMinimumSize().y)};
        pair.first["MaximumWidth"] = {"Float", tgui::to_string(childWindow->getMaximumSize().x)};
        pair.first["MaximumHeight"] = {"Float", tgui::to_string(childWindow->getMaximumSize().y)};

        const auto renderer = childWindow->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["TitleColor"] = {"Color", tgui::Serializer::serialize(renderer->getTitleColor())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["TitleBarColor"] = {"Color", tgui::Serializer::serialize(renderer->getTitleBarColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorFocused())};
        pair.second["TitleBarHeight"] = {"Float", tgui::to_string(renderer->getTitleBarHeight())};
        pair.second["BorderBelowTitleBar"] = {"Float", tgui::to_string(renderer->getBorderBelowTitleBar())};
        pair.second["DistanceToSide"] = {"Float", tgui::to_string(renderer->getDistanceToSide())};
        pair.second["PaddingBetweenButtons"] = {"Float", tgui::to_string(renderer->getPaddingBetweenButtons())};
        pair.second["MinimumResizableBorderWidth"] = {"Float", tgui::to_string(renderer->getMinimumResizableBorderWidth())};
        pair.second["ShowTextOnTitleButtons"] = {"Bool", tgui::Serializer::serialize(renderer->getTransparentTexture())};
        pair.second["TextureTitleBar"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTitleBar())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        return pair;
    }


private:

    static tgui::ChildWindow::TitleAlignment deserializeTitleAlignment(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "right")
            return tgui::ChildWindow::TitleAlignment::Right;
        else if (value == "center")
            return tgui::ChildWindow::TitleAlignment::Center;
        else
            return tgui::ChildWindow::TitleAlignment::Left;
    }

    static int deserializeTitleButtons(std::string value)
    {
        int decodedTitleButtons = tgui::ChildWindow::TitleButton::None;
        std::vector<std::string> titleButtons = tgui::Deserializer::split(value, '|');
        for (const auto& elem : titleButtons)
        {
            std::string requestedTitleButton = tgui::toLower(tgui::trim(elem));
            if (requestedTitleButton == "close")
                decodedTitleButtons |= sf::Text::Bold;
            else if (requestedTitleButton == "maximize")
                decodedTitleButtons |= sf::Text::Italic;
            else if (requestedTitleButton == "minimize")
                decodedTitleButtons |= sf::Text::Underlined;
        }

        return decodedTitleButtons;
    }

    static std::string serializeTitleAlignment(tgui::ChildWindow::TitleAlignment alignment)
    {
        if (alignment == tgui::ChildWindow::TitleAlignment::Center)
            return "Center";
        else if (alignment == tgui::ChildWindow::TitleAlignment::Right)
            return "Right";
        else
            return "Left";
    }

    static std::string serializeTitleButtons(unsigned int titleButtons)
    {
        std::string serializedTitleButtons;
        if (titleButtons & tgui::ChildWindow::TitleButton::Minimize)
            serializedTitleButtons += " | Minimize";
        if (titleButtons & tgui::ChildWindow::TitleButton::Maximize)
            serializedTitleButtons += " | Maximize";
        if (titleButtons & tgui::ChildWindow::TitleButton::Close)
            serializedTitleButtons += " | Close";

        if (!serializedTitleButtons.empty())
            serializedTitleButtons.erase(0, 3);
        else
            serializedTitleButtons = "None";

        return serializedTitleButtons;
    }
};

#endif // TGUI_GUI_BUILDER_CHILD_WINDOW_PROPERTIES_HPP
