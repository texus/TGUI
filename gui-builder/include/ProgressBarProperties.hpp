/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_GUI_BUILDER_PROGRESS_BAR_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_PROGRESS_BAR_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/ProgressBar.hpp>

struct ProgressBarProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto progressBar = std::dynamic_pointer_cast<tgui::ProgressBar>(widget);
        if (property == "Minimum")
            progressBar->setMinimum(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "Maximum")
            progressBar->setMaximum(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "Value")
            progressBar->setValue(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "Text")
            progressBar->setText(value);
        else if (property == "TextSize")
            progressBar->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "FillDirection")
            progressBar->setFillDirection(deserializeFillDirection(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto progressBar = std::dynamic_pointer_cast<tgui::ProgressBar>(widget);
        pairs["Minimum"] = {"UInt", tgui::to_string(progressBar->getMinimum())};
        pairs["Maximum"] = {"UInt", tgui::to_string(progressBar->getMaximum())};
        pairs["Value"] = {"UInt", tgui::to_string(progressBar->getValue())};
        pairs["Text"] = {"String", progressBar->getText()};
        pairs["TextSize"] = {"UInt", tgui::to_string(progressBar->getTextSize())};
        pairs["FillDirection"] = {"Enum{LeftToRight,RightToLeft,TopToBottom,BottomToTop}", serializeFillDirection(progressBar->getFillDirection())};

        const auto renderer = progressBar->getRenderer();
        pairs["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["TextColorFilled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorFilled())};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["FillColor"] = {"Color", tgui::Serializer::serialize(renderer->getFillColor())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pairs["TextureFill"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFill())};
        pairs["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        return pairs;
    }

private:

    static tgui::ProgressBar::FillDirection deserializeFillDirection(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "bottomtotop")
            return tgui::ProgressBar::FillDirection::BottomToTop;
        else if (value == "toptobottom")
            return tgui::ProgressBar::FillDirection::TopToBottom;
        else if (value == "righttoleft")
            return tgui::ProgressBar::FillDirection::RightToLeft;
        else
            return tgui::ProgressBar::FillDirection::LeftToRight;
    }

    static std::string serializeFillDirection(tgui::ProgressBar::FillDirection direction)
    {
        if (direction == tgui::ProgressBar::FillDirection::BottomToTop)
            return "BottomToTop";
        else if (direction == tgui::ProgressBar::FillDirection::TopToBottom)
            return "TopToBottom";
        else if (direction == tgui::ProgressBar::FillDirection::RightToLeft)
            return "RightToLeft";
        else
            return "LeftToRight";
    }
};

#endif // TGUI_GUI_BUILDER_PROGRESS_BAR_PROPERTIES_HPP
