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


#ifndef TGUI_GUI_BUILDER_FORM_HPP
#define TGUI_GUI_BUILDER_FORM_HPP

#include "WidgetInfo.hpp"

#include <TGUI/Config.hpp>
#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/TGUI.hpp>
#endif

class GuiBuilder;

class Form
{
public:
    Form(GuiBuilder* guiBuilder, const tgui::String& filename, const tgui::ChildWindow::Ptr& formWindow, tgui::Vector2f formSize);
    tgui::String addWidget(const tgui::Widget::Ptr& widget, tgui::Container* parent, bool selectNewWidget = true);
    void removeWidget(const tgui::String& id);
    TGUI_NODISCARD std::shared_ptr<WidgetInfo> getWidget(const tgui::String& id) const;
    TGUI_NODISCARD std::shared_ptr<WidgetInfo> getWidgetByName(const tgui::String& name) const;
    TGUI_NODISCARD std::vector<std::shared_ptr<WidgetInfo>> getWidgets() const;
    TGUI_NODISCARD std::shared_ptr<tgui::Group> getRootWidgetsGroup() const;
    TGUI_NODISCARD std::shared_ptr<WidgetInfo> getSelectedWidget() const;
    bool setSelectedWidgetName(const tgui::String& name);
    void setSelectedWidgetRenderer(const tgui::String& renderer);
    void updateSelectionSquarePositions();
    void selectWidgetById(const tgui::String& id);
    void selectWidgetByName(const tgui::String& name);
    void selectParent();
    void mouseMoved(tgui::Vector2i pos);
    void mouseReleased();
    TGUI_NODISCARD bool rightMouseClick(tgui::Vector2i pos);
    void arrowKeyPressed(const tgui::Event::KeyEvent& keyEvent);
    void setFilename(const tgui::String& filename);
    TGUI_NODISCARD tgui::String getFilename() const;
    void setSize(tgui::Vector2f size);
    TGUI_NODISCARD tgui::Vector2f getSize() const;
    void setChanged(bool changed);
    TGUI_NODISCARD bool isChanged() const;
    void focus();
    TGUI_NODISCARD bool hasFocus() const;
    void load();
    void save();
    TGUI_NODISCARD std::stringstream saveState();
    void loadState(std::stringstream& state);
    void updateAlignmentLines();

private:
    void importLoadedWidgets(const tgui::Container::Ptr& parent);
    void onSelectionSquarePress(const tgui::Button::Ptr& square, tgui::Vector2f pos);
    TGUI_NODISCARD tgui::Widget::Ptr getWidgetBelowMouse(const tgui::Container::Ptr& parent, tgui::Vector2f pos);
    void onFormMousePress(tgui::Vector2f pos);
    void onDrag(tgui::Vector2i mousePos);
    void selectWidget(const std::shared_ptr<WidgetInfo>& widget);
    TGUI_NODISCARD std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> getAlignmentLines() const;

private:

    GuiBuilder* m_guiBuilder;
    tgui::ChildWindow::Ptr m_formWindow;
    tgui::ScrollablePanel::Ptr m_scrollablePanel;
    tgui::Group::Ptr m_widgetsContainer;
    tgui::Group::Ptr m_overlay;
    std::shared_ptr<WidgetInfo> m_selectedWidget = nullptr;
    std::array<tgui::Button::Ptr, 8> m_selectionSquares;
    std::map<tgui::String, std::shared_ptr<WidgetInfo>> m_widgets;
    bool m_changed = false;
    bool m_draggingWidget = false;
    bool m_onDragSaved = false;
    tgui::Button::Ptr m_draggingSelectionSquare;
    tgui::Vector2f m_draggingPos;
    tgui::String m_filename;
    tgui::Vector2f m_size;
    std::vector<tgui::SeparatorLine::Ptr> m_alignmentLines;
};

#endif // TGUI_GUI_BUILDER_FORM_HPP
