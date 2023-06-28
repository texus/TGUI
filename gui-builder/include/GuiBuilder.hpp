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


#ifndef TGUI_GUI_BUILDER_GUI_BUILDER_HPP
#define TGUI_GUI_BUILDER_GUI_BUILDER_HPP

#include <TGUI/Config.hpp>

// There are still issues with including std headers after importing the TGUI module (if TGUI_BUILD_AS_CXX_MODULE is 1),
// so we include all std headers that are needed in the source files up here. This only works because this header file is
// the first one to be included in both Form.cpp and GuiBuilder.cpp
#include <cassert>
#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <vector>
    #include <memory>
    #include <string>
    #include <thread> // this_thread::sleep_for
    #include <cctype> // isdigit
    #include <cmath> // max
    #include <stack>
    #include <array>
    #include <map>
    #include <set>

    #ifdef TGUI_SYSTEM_LINUX
        #include <cstdio> // FILENAME_MAX
    #endif
#endif

#ifdef TGUI_SYSTEM_LINUX
    #include <sys/types.h> // ssize_t
    #include <unistd.h> // readlink
#endif

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
    import tgui.default_backend_window;
#else
    #include <TGUI/TGUI.hpp>
    #include <TGUI/Filesystem.hpp>
    #include <TGUI/DefaultBackendWindow.hpp>
#endif

#include "WidgetProperties/WidgetProperties.hpp"
#include "Form.hpp"

class GuiBuilder
{
public:

    enum class UndoType
    {
        Delete,
        Move,
        Resize,
        Paste,
        SendtoFront,
        SendtoBack,
        CreateNew,
        PropertyEdit
    };

    GuiBuilder(const tgui::String& programName);
    ~GuiBuilder();
    void mainLoop();

    void reloadProperties();
    void widgetSelected(const tgui::Widget::Ptr& widget);
    void formSaved(const tgui::String& filename);
    void closeForm(Form* form);
    void saveUndoState(UndoType type);

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


    TGUI_NODISCARD bool loadGuiBuilderState();
    void saveGuiBuilderState();
    void loadStartScreen();
    void loadEditingScreen(const tgui::String& filename);
    void loadToolbox();
    void showLoadFileWindow(const tgui::String& title, const tgui::String& loadButtonCaption, bool allowCreateFolder, bool fileMustExist, const tgui::String& defaultFilename, const std::function<void(const tgui::String&)>& onLoad);
    void createNewWidget(const tgui::Widget::Ptr& widget, tgui::Container* parent = nullptr, bool selectNewWidget = true);
    bool updateWidgetProperty(const tgui::String& property, const tgui::String& value);
    void initProperties();
    void addPropertyValueWidgets(float& topPosition, const PropertyValuePair& propertyValuePair, const OnValueChangeFunc& onChange);
    void changeWidgetName(const tgui::String& name);
    void initSelectedWidgetComboBoxAfterLoad();
    void removeSelectedWidget();
    void removePopupMenu();
    void createNewForm(tgui::String filename);
    bool loadForm(tgui::String filename, bool loadingFromFile = true);
    void displayErrorMessage(const tgui::String& error);
    tgui::ChildWindow::Ptr openWindowWithFocus(tgui::ChildWindow::Ptr window = tgui::ChildWindow::create());
    TGUI_NODISCARD tgui::String getDefaultFilename() const;

    void copyWidgetRecursive(std::vector<CopiedWidget>& copiedWidgetList, const std::shared_ptr<WidgetInfo>& widgetInfo);
    void pasteWidgetRecursive(const CopiedWidget& copiedWidget, tgui::Container* parent);
    void copyWidgetToInternalClipboard(const std::shared_ptr<WidgetInfo>& widgetInfo);
    void pasteWidgetFromInternalClipboard();
    void loadUndoState();

    void widgetHierarchyChanged();
    void updateSelectedWidgetHierarchy();
    void fillWidgetHierarchyTreeRecursively(std::vector<tgui::String>& hierarchy, const std::shared_ptr<tgui::Widget>& parentWidget);
    TGUI_NODISCARD bool fillWidgetHierarchy(std::vector<tgui::String>& hierarchy, tgui::Widget* widget);

    tgui::EditBox::Ptr addPropertyValueEditBox(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition, float rightPadding);
    tgui::Button::Ptr addPropertyValueButtonMore(const tgui::String& property, float topPosition);
    tgui::Button::Ptr addPropertyValueButtonLayoutRelAbs(const tgui::String& property, float topPosition, bool valueIsAbsolute);
    void addPropertyValueBool(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueColor(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueLayout(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueTextStyle(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueOutline(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueStringList(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyValueTexture(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
    void addPropertyListViewColumns(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition);
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

    std::shared_ptr<tgui::DefaultBackendWindow> m_window;
    tgui::BackendGui* m_gui;

    tgui::ChildWindow::Ptr m_propertiesWindow;
    tgui::ScrollablePanel::Ptr m_propertiesContainer;
    tgui::ComboBox::Ptr m_selectedWidgetComboBox;
    tgui::MenuBar::Ptr m_menuBar;
    tgui::TreeView::Ptr m_widgetHierarchyTree;
    tgui::ListBox::Ptr m_popupMenu;

    std::vector<std::unique_ptr<Form>> m_forms;
    Form* m_selectedForm = nullptr;
    tgui::Panel::Ptr m_foregroundPanel = nullptr;


    std::map<tgui::String, std::unique_ptr<WidgetProperties>> m_widgetProperties;
    PropertyValueMapPair m_propertyValuePairs;

    std::vector<CopiedWidget> m_copiedWidgets;

    std::map<tgui::String, tgui::Theme> m_themes;
    tgui::String m_defaultTheme;

    tgui::Vector2f m_formSize{800.f, 600.f};
    tgui::String m_defaultPath;
    tgui::Filesystem::Path m_programPath;

    std::vector<std::stringstream> m_undoSaves;
    std::vector<tgui::String> m_undoSavesDesc;
};

#endif // TGUI_GUI_BUILDER_GUI_BUILDER_HPP
