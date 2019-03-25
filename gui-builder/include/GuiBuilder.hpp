/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_GUI_BUILDER_GUI_BUILDER_HPP
#define TGUI_GUI_BUILDER_GUI_BUILDER_HPP

#include <TGUI/TGUI.hpp>
#include "WidgetProperties/WidgetProperties.hpp"
#include "Form.hpp"

class GuiBuilder
{
public:

    GuiBuilder();
    ~GuiBuilder();
    void mainLoop();

    void reloadProperties();
    void widgetSelected(tgui::Widget::Ptr widget);
    void closeForm(Form* form);

private:

    using PropertyValuePair = std::pair<std::string, std::pair<std::string, std::string>>;
    using OnValueChangeFunc = std::function<void(const sf::String& value)>;

    void loadStartScreen();
    void loadEditingScreen(const std::string& filename);
    void loadToolbox();
    void createNewWidget(tgui::Widget::Ptr widget);
    void recursiveCopyWidget(tgui::Container::Ptr oldContainer, tgui::Container::Ptr newContainer);
    void copyWidget(std::shared_ptr<WidgetInfo> widgetInfo);
    bool updateWidgetProperty(const std::string& property, const std::string& value);
    void initProperties();
    void addPropertyValueWidgets(float& topPosition, const PropertyValuePair& propertyValuePair, const OnValueChangeFunc& onChange);
    void changeWidgetName(const std::string& name);
    void initSelectedWidgetComboBoxAfterLoad();
    void removeSelectedWidget();
    void loadForm();
    tgui::ChildWindow::Ptr openWindowWithFocus();

    void copyWidgetToInternalClipboard(std::shared_ptr<WidgetInfo> widgetInfo);
    void pasteWidgetFromInternalClipboard();

    tgui::EditBox::Ptr addPropertyValueEditBox(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition, float rightPadding);
    tgui::Button::Ptr addPropertyValueButtonMore(const std::string& property, float topPosition);
    void addPropertyValueBool(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueColor(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueTextStyle(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueOutline(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueStringList(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueTexture(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueMultilineString(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueEditBoxInputValidator(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueChildWindowTitleButtons(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueEnum(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition, const std::vector<std::string>& enumValues);

    void menuBarCallbackNewOrLoadFile();
    void menuBarCallbackSaveFile();
    void menuBarCallbackQuit();
    void menuBarCallbackEditThemes();
    void menuBarCallbackBringWidgetToFront();
    void menuBarCallbackSendWidgetToBack();
    void menuBarCallbackCutWidget();
    void menuBarCallbackCopyWidget();
    void menuBarCallbackPasteWidget();
    void menuBarCallbackDeleteWidget();
    void menuBarCallbackKeyboardShortcuts();
    void menuBarCallbackAbout();

private:

    std::string m_lastOpenedFile;

    sf::RenderWindow m_window;
    tgui::Gui m_gui;

    tgui::ChildWindow::Ptr m_propertiesWindow;
    tgui::ScrollablePanel::Ptr m_propertiesContainer;
    tgui::ComboBox::Ptr m_selectedWidgetComboBox;
    tgui::MenuBar::Ptr m_menuBar;

    std::vector<std::unique_ptr<Form>> m_forms;
    Form* m_selectedForm = nullptr;

    std::map<std::string, std::unique_ptr<WidgetProperties>> m_widgetProperties;
    PropertyValueMapPair m_propertyValuePairs;

    std::string m_copiedWidgetType;
    std::string m_copiedWidgetTheme;
    PropertyValueMapPair m_copiedWidgetPropertyValuePairs;

    std::map<std::string, tgui::Theme> m_themes;
    std::string m_defaultTheme;
};

#endif // TGUI_GUI_BUILDER_GUI_BUILDER_HPP
