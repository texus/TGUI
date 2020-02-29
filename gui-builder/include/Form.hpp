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


#ifndef TGUI_GUI_BUILDER_FORM_HPP
#define TGUI_GUI_BUILDER_FORM_HPP

#include "WidgetInfo.hpp"
#include <TGUI/TGUI.hpp>
#include <array>

class GuiBuilder;

class Form
{
public:
    Form(GuiBuilder* guiBuilder, const std::string& filename, tgui::ChildWindow::Ptr formWindow, sf::Vector2f formSize);
    std::string addWidget(tgui::Widget::Ptr widget, tgui::Container* parent, bool selectNewWidget = true);
    void removeWidget(const std::string& id);
    std::shared_ptr<WidgetInfo> getWidget(const std::string& id) const;
    std::shared_ptr<WidgetInfo> getWidgetByName(const std::string& name) const;
    std::vector<std::shared_ptr<WidgetInfo>> getWidgets() const;
    std::shared_ptr<tgui::Group> getRootWidgetsGroup() const;
    std::shared_ptr<WidgetInfo> getSelectedWidget() const;
    bool setSelectedWidgetName(const std::string& name);
    void setSelectedWidgetRenderer(const std::string& renderer);
    void updateSelectionSquarePositions();
    void selectWidgetById(const std::string& id);
    void selectWidgetByName(const std::string& name);
    void selectParent();
    void mouseMoved(sf::Vector2i pos);
    void mouseReleased();
    bool rightMouseClick(sf::Vector2i pos);
    void arrowKeyPressed(const sf::Event::KeyEvent& keyEvent);
    void setFilename(const sf::String& filename);
    sf::String getFilename() const;
    void setSize(sf::Vector2f size);
    sf::Vector2f getSize() const;
    void setChanged(bool changed);
    bool isChanged() const;
    void focus();
    bool hasFocus() const;
    bool load();
    void save();
    void drawExtra(sf::RenderWindow& window) const;

private:
    void importLoadedWidgets(tgui::Container::Ptr parent);
    void onSelectionSquarePress(tgui::Button::Ptr square, sf::Vector2f pos);
    tgui::Widget::Ptr getWidgetBelowMouse(tgui::Container::Ptr parent, sf::Vector2f pos);
    void onFormMousePress(sf::Vector2f pos);
    void onDrag(sf::Vector2i mousePos);
    void selectWidget(std::shared_ptr<WidgetInfo> widget);
    void drawLine(sf::RenderWindow& window, sf::Vector2f startPoint, sf::Vector2f endPoint) const;

private:

    GuiBuilder* m_guiBuilder;
    tgui::ChildWindow::Ptr m_formWindow;
    tgui::ScrollablePanel::Ptr m_scrollablePanel;
    tgui::Group::Ptr m_widgetsContainer;
    std::shared_ptr<WidgetInfo> m_selectedWidget = nullptr;
    std::array<tgui::Button::Ptr, 8> m_selectionSquares;
    std::map<std::string, std::shared_ptr<WidgetInfo>> m_widgets;
    bool m_changed = false;
    bool m_draggingWidget = false;
    tgui::Button::Ptr m_draggingSelectionSquare;
    sf::Vector2f m_draggingPos;
    sf::String m_filename;
    sf::Vector2f m_size;
};

#endif // TGUI_GUI_BUILDER_FORM_HPP
