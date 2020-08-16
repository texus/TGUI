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


#ifndef TGUI_GUI_BUILDER_GUI_BUILDER_HPP
#define TGUI_GUI_BUILDER_GUI_BUILDER_HPP

#include <TGUI/TGUI.hpp>
#include <TGUI/Filesystem.hpp>
#include "WidgetProperties/WidgetProperties.hpp"
#include "Form.hpp"

class GuiBuilder
{
public:

    GuiBuilder(const char* programName);
    ~GuiBuilder();
    void mainLoop();

    void reloadProperties();
    void widgetSelected(tgui::Widget::Ptr widget);
    void formSaved(const tgui::String& filename);
    void closeForm(Form* form);

private:

    using PropertyValuePair = std::pair<tgui::String, std::pair<tgui::String, tgui::String>>;
    using OnValueChangeFunc = std::function<void(const tgui::String& value)>;

    struct CopiedWidget
    {
        tgui::String theme;
        tgui::Widget::Ptr widget;
        tgui::Widget::Ptr originalWidget;
        std::vector<CopiedWidget> childWidgets;
    };

    bool loadGuiBuilderState();
    void saveGuiBuilderState();
    void loadStartScreen();
    void loadEditingScreen(const tgui::String& filename);
    void loadToolbox();
    void showLoadFileWindow(const tgui::String& title, const tgui::String& loadButtonCaption, const tgui::String& defaultFilename, const std::function<void(const tgui::String&)>& onLoad);
    void createNewWidget(tgui::Widget::Ptr widget, tgui::Container* parent = nullptr, bool selectNewWidget = true);
    bool updateWidgetProperty(const tgui::String& property, const tgui::String& value);
    void initProperties();
    void addPropertyValueWidgets(float& topPosition, const PropertyValuePair& propertyValuePair, const OnValueChangeFunc& onChange);
    void changeWidgetName(const tgui::String& name);
    void initSelectedWidgetComboBoxAfterLoad();
    void removeSelectedWidget();
    void removePopupMenu();
    void createNewForm(const tgui::String& filename);
    bool loadForm(const tgui::String& filename);
    tgui::ChildWindow::Ptr openWindowWithFocus();
    tgui::String getDefaultFilename() const;

    void copyWidgetRecursive(std::vector<CopiedWidget>& copiedWidgetList, std::shared_ptr<WidgetInfo> widgetInfo);
    void pasteWidgetRecursive(const CopiedWidget& copiedWidget, tgui::Container* parent);
    void copyWidgetToInternalClipboard(std::shared_ptr<WidgetInfo> widgetInfo);
    void pasteWidgetFromInternalClipboard();

    void widgetHierarchyChanged();
    void updateSelectedWidgetHierarchy();
    void fillWidgetHierarchyTreeRecursively(std::vector<tgui::String>& hierarchy, std::shared_ptr<tgui::Widget> parentWidget);
    bool fillWidgetHierarchy(std::vector<tgui::String>& hierarchy, tgui::Widget* widget);

    tgui::EditBox::Ptr addPropertyValueEditBox(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition, float rightPadding);
    tgui::Button::Ptr addPropertyValueButtonMore(const tgui::String& property, float topPosition);
    void addPropertyValueBool(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueColor(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueTextStyle(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueOutline(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueStringList(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueTexture(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueMultilineString(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueEditBoxInputValidator(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueChildWindowTitleButtons(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueEnum(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition, const std::vector<tgui::String>& enumValues);

    void menuBarCallbackNewForm();
    void menuBarCallbackLoadForm();
    void menuBarCallbackLoadRecent(const tgui::String& filename);
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

    std::vector<tgui::String> m_recentFiles;

    sf::RenderWindow m_window;
    tgui::GuiSFML m_gui;

    tgui::ChildWindow::Ptr m_propertiesWindow;
    tgui::ScrollablePanel::Ptr m_propertiesContainer;
    tgui::ComboBox::Ptr m_selectedWidgetComboBox;
    tgui::MenuBar::Ptr m_menuBar;
    tgui::TreeView::Ptr m_widgetHierarchyTree;
    tgui::ListBox::Ptr m_popupMenu;

    std::vector<std::unique_ptr<Form>> m_forms;
    Form* m_selectedForm = nullptr;

    std::map<tgui::String, std::unique_ptr<WidgetProperties>> m_widgetProperties;
    PropertyValueMapPair m_propertyValuePairs;

    std::vector<CopiedWidget> m_copiedWidgets;

    std::map<tgui::String, tgui::Theme> m_themes;
    tgui::String m_defaultTheme;

    tgui::Vector2f m_formSize{800.f, 600.f};
    tgui::String m_defaultPath;
    tgui::Filesystem::Path m_programPath;
};

#endif // TGUI_GUI_BUILDER_GUI_BUILDER_HPP
