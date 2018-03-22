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


#ifndef TGUI_GUI_BUILDER_GUI_BUILDER_HPP
#define TGUI_GUI_BUILDER_GUI_BUILDER_HPP

#include <TGUI/TGUI.hpp>
#include "WidgetProperties.hpp"
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

    void loadStartScreen();
    void loadEditingScreen(const std::string& filename);
    void loadToolbox();
    void createNewWidget(tgui::Widget::Ptr widget);
    void recursiveCopyWidget(tgui::Container::Ptr oldContainer, tgui::Container::Ptr newContainer);
    void copyWidget(std::shared_ptr<WidgetInfo> widgetInfo);
    void updateWidgetProperty(const std::string& property, const std::string& value);
    void initProperties();
    tgui::EditBox::Ptr addPropertyValueEditBoxes(float& topPosition, const std::pair<std::string, std::pair<std::string, std::string>>& propertyValuePair);
    void changeWidgetName(const std::string& name);
    void initSelectedWidgetComboBoxAfterLoad();
    void removeSelectedWidget();
    void loadForm();
    void menuBarItemClicked(const std::string& item);

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

    std::map<std::string, tgui::Theme> m_themes;
    std::string m_defaultTheme;

    sf::String m_previousValue;
};

#endif // TGUI_GUI_BUILDER_GUI_BUILDER_HPP
