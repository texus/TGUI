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

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto editBox = std::dynamic_pointer_cast<tgui::EditBox>(widget);
        pair.first["Text"] = {"String", editBox->getText()};
        pair.first["DefaultText"] = {"String", editBox->getDefaultText()};
        pair.first["TextSize"] = {"UInt", tgui::to_string(editBox->getTextSize())};
        pair.first["PasswordCharacter"] = {"Char", editBox->getPasswordCharacter() ? std::string(1, editBox->getPasswordCharacter()) : ""};
        pair.first["MaximumCharacters"] = {"UInt", tgui::to_string(editBox->getMaximumCharacters())};
        pair.first["Alignment"] = {"Enum{Left,Center,Right}", serializeAlignment(editBox->getAlignment())};
        pair.first["LimitTextWidth"] = {"Bool", tgui::Serializer::serialize(editBox->isTextWidthLimited())};
        pair.first["ReadOnly"] = {"Bool", tgui::Serializer::serialize(editBox->isReadOnly())};
        pair.first["InputValidator"] = {"String", editBox->getInputValidator()};

        const auto renderer = editBox->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["CaretWidth"] = {"Float", tgui::to_string(renderer->getCaretWidth())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pair.second["TextColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorFocused())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pair.second["SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextBackgroundColor())};
        pair.second["DefaultTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getDefaultTextColor())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pair.second["BackgroundColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorFocused())};
        pair.second["CaretColor"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColor())};
        pair.second["CaretColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColorHover())};
        pair.second["CaretColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColorFocused())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pair.second["BorderColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorFocused())};
        pair.second["Texture"] = {"Texture", tgui::Serializer::serialize(renderer->getTexture())};
        pair.second["TextureHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureHover())};
        pair.second["TextureDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabled())};
        pair.second["TextureFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFocused())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pair.second["DefaultTextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getDefaultTextStyle())};
        return pair;
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
