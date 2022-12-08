/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/TGUI.hpp>
#include <array>

class GuiBuilder;

class Form
{
public:
    Form(GuiBuilder* guiBuilder, const tgui::String& filename, tgui::ChildWindow::Ptr formWindow, tgui::Vector2f formSize);
    tgui::String addWidget(tgui::Widget::Ptr widget, tgui::Container* parent, bool selectNewWidget = true);
    void removeWidget(const tgui::String& id);
    std::shared_ptr<WidgetInfo> getWidget(const tgui::String& id) const;
    std::shared_ptr<WidgetInfo> getWidgetByName(const tgui::String& name) const;
    std::vector<std::shared_ptr<WidgetInfo>> getWidgets() const;
    std::shared_ptr<tgui::Group> getRootWidgetsGroup() const;
    std::shared_ptr<WidgetInfo> getSelectedWidget() const;
    bool setSelectedWidgetName(const tgui::String& name);
    void setSelectedWidgetRenderer(const tgui::String& renderer);
    void updateSelectionSquarePositions();
    void selectWidgetById(const tgui::String& id);
    void selectWidgetByName(const tgui::String& name);
    void selectParent();
    void mouseMoved(tgui::Vector2i pos);
    void mouseReleased();
    bool rightMouseClick(tgui::Vector2i pos);
    void arrowKeyPressed(const tgui::Event::KeyEvent& keyEvent);
    void setFilename(const tgui::String& filename);
    tgui::String getFilename() const;
    void setSize(tgui::Vector2f size);
    tgui::Vector2f getSize() const;
    void setChanged(bool changed);
    bool isChanged() const;
    void focus();
    bool hasFocus() const;
    void load();
    void save();
    tgui::String saveState();
    bool loadState(tgui::String);
    void updateAlignmentLines();

private:
    void importLoadedWidgets(tgui::Container::Ptr parent);
    void onSelectionSquarePress(tgui::Button::Ptr square, tgui::Vector2f pos);
    tgui::Widget::Ptr getWidgetBelowMouse(tgui::Container::Ptr parent, tgui::Vector2f pos);
    void onFormMousePress(tgui::Vector2f pos);
    void onDrag(tgui::Vector2i mousePos);
    void selectWidget(std::shared_ptr<WidgetInfo> widget);
    std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> getAlignmentLines() const;
    bool onDragSaved;

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
    tgui::Button::Ptr m_draggingSelectionSquare;
    tgui::Vector2f m_draggingPos;
    tgui::String m_filename;
    tgui::Vector2f m_size;
    std::vector<tgui::SeparatorLine::Ptr> m_alignmentLines;
};

#endif // TGUI_GUI_BUILDER_FORM_HPP
