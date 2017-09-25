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


#ifndef TGUI_GUI_BUILDER_EDIT_BOX_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_EDIT_BOX_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/EditBox.hpp>

struct EditBoxProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto editBox = std::dynamic_pointer_cast<tgui::EditBox>(widget);
        if (property == "Text")
            editBox->setText(value);
        else if (property == "DefaultText")
            editBox->setDefaultText(value);
        else if (property == "TextSize")
            editBox->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "PasswordCharacter")
            editBox->setPasswordCharacter(value.empty() ? '\0' : value[0]);
        else if (property == "MaximumCharacters")
            editBox->setMaximumCharacters(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "Alignment")
            editBox->setAlignment(deserializeAlignment(value));
        else if (property == "LimitTextWidth")
            editBox->limitTextWidth(parseBoolean(value, false));
        else if (property == "ReadOnly")
            editBox->setReadOnly(parseBoolean(value, false));
        else if (property == "InputValidator")
            editBox->setInputValidator(value);
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto editBox = std::dynamic_pointer_cast<tgui::EditBox>(widget);
        pairs["Text"] = {"String", editBox->getText()};
        pairs["DefaultText"] = {"String", editBox->getDefaultText()};
        pairs["TextSize"] = {"UInt", tgui::to_string(editBox->getTextSize())};
        pairs["PasswordCharacter"] = {"Char", editBox->getPasswordCharacter() ? std::string(1, editBox->getPasswordCharacter()) : ""};
        pairs["MaximumCharacters"] = {"UInt", tgui::to_string(editBox->getMaximumCharacters())};
        pairs["Alignment"] = {"Enum{Left,Center,Right}", serializeAlignment(editBox->getAlignment())};
        pairs["LimitTextWidth"] = {"Bool", tgui::Serializer::serialize(editBox->isTextWidthLimited())};
        pairs["ReadOnly"] = {"Bool", tgui::Serializer::serialize(editBox->isReadOnly())};
        pairs["InputValidator"] = {"String", editBox->getInputValidator()};

        const auto renderer = editBox->getRenderer();
        pairs["Borders"] = {"Outline", renderer->getBorders().toString()};
        pairs["Padding"] = {"Outline", renderer->getPadding().toString()};
        pairs["CaretWidth"] = {"Float", tgui::to_string(renderer->getCaretWidth())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pairs["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pairs["SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextBackgroundColor())};
        pairs["DefaultTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getDefaultTextColor())};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pairs["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pairs["CaretColor"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColor())};
        pairs["CaretColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColorHover())};
        pairs["CaretColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColorDisabled())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pairs["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pairs["Texture"] = {"Texture", tgui::Serializer::serialize(renderer->getTexture())};
        pairs["TextureHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureHover())};
        pairs["TextureDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabled())};
        pairs["TextureFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFocused())};
        pairs["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pairs["DefaultTextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getDefaultTextStyle())};
        return pairs;
    }

private:

    static tgui::EditBox::Alignment deserializeAlignment(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "right")
            return tgui::EditBox::Alignment::Right;
        else if (value == "center")
            return tgui::EditBox::Alignment::Center;
        else
            return tgui::EditBox::Alignment::Left;
    }

    static std::string serializeAlignment(tgui::EditBox::Alignment alignment)
    {
        if (alignment == tgui::EditBox::Alignment::Center)
            return "Center";
        else if (alignment == tgui::EditBox::Alignment::Right)
            return "Right";
        else
            return "Left";
    }
};

#endif // TGUI_GUI_BUILDER_EDIT_BOX_PROPERTIES_HPP
