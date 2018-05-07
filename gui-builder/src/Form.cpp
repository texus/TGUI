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


#include "Form.hpp"
#include "GuiBuilder.hpp"

#include <cassert>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Form::Form(GuiBuilder* guiBuilder, const std::string& filename, tgui::ChildWindow::Ptr formWindow) :
    m_guiBuilder      {guiBuilder},
    m_formWindow      {formWindow},
    m_scrollablePanel {formWindow->get<tgui::ScrollablePanel>("ScrollablePanel")},
    m_widgetsContainer{m_scrollablePanel->get<tgui::Group>("WidgetContainer")},
    m_filename        {filename}
{
    m_widgets["form"] = nullptr;

    m_formWindow->setTitle(filename);
    m_formWindow->connect("Closed", [this]{ m_guiBuilder->closeForm(this); });
    m_formWindow->connect("SizeChanged", [this]{ m_scrollablePanel->setSize(m_formWindow->getSize()); });

    auto eventHandler = tgui::ClickableWidget::create();
    eventHandler->connect("MousePressed", [=](sf::Vector2f pos){ onFormMousePress(pos); });
    m_scrollablePanel->add(eventHandler, "EventHandler");

    m_scrollablePanel->setSize(m_formWindow->getSize());
    setSize(sf::Vector2i{sf::Vector2f{m_formWindow->getSize()}});

    tgui::Theme selectionSquareTheme{"resources/SelectionSquare.txt"};
    for (auto& square : m_selectionSquares)
    {
        square = tgui::Button::create();
        square->setRenderer(selectionSquareTheme.getRenderer("Square"));
        square->setSize(square->getRenderer()->getTexture().getImageSize());
        square->setVisible(false);
        square->connect("MousePressed", [=](sf::Vector2f pos){ onSelectionSquarePress(square, pos); });
        m_scrollablePanel->add(square);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Form::addWidget(tgui::Widget::Ptr widget, tgui::Container* parent)
{
    const std::string id = tgui::to_string(widget.get());
    m_widgets[id] = std::make_shared<WidgetInfo>(widget);

    const std::string name = "Widget" + tgui::to_string(++m_idCounter);
    m_widgets[id]->name = name;

    if (parent)
        parent->add(widget, name);
    else
        m_widgetsContainer->add(widget, name);

    selectWidget(m_widgets[id]);

    setChanged(true);
    return name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Form::addExistingWidget(tgui::Widget::Ptr widget)
{
    const std::string id = tgui::to_string(widget.get());
    m_widgets[id] = std::make_shared<WidgetInfo>(widget);

    const std::string name = "Widget" + tgui::to_string(++m_idCounter);
    m_widgets[id]->name = name;

    setChanged(true);
    return name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::removeWidget(const std::string& id)
{
    const auto widget = m_widgets[id];
    assert(widget != nullptr);
    widget->ptr->getParent()->remove(widget->ptr);

    m_widgets.erase(id);
    setChanged(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<WidgetInfo> Form::getWidget(const std::string& id) const
{
    assert(m_widgets.find(id) != m_widgets.end());
    return m_widgets.at(id);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<WidgetInfo>> Form::getWidgets() const
{
    std::vector<std::shared_ptr<WidgetInfo>> widgets;

    for (auto& pair : m_widgets)
    {
        if (pair.second != nullptr)
            widgets.push_back(pair.second);
    }

    return widgets;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<WidgetInfo> Form::getSelectedWidget() const
{
    return m_selectedWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::setSelectedWidgetName(const std::string& name)
{
    assert(m_selectedWidget != nullptr);

    bool widgetFound = false;
    auto widgets = m_widgetsContainer->getWidgets();
    for (auto& widget : widgets)
    {
        if (widgetFound)
        {
            // These widgets appeared after the selected widgets and thus have to be displayed in front of it.
            // By removing and re-adding the selected widget we however changed the order, which we are correcting here.
            widget->moveToFront();
        }
        else
        {
            if (widget == m_selectedWidget->ptr)
            {
                // Remove the selected widget and add it again with a different name
                tgui::Container* parent = widget->getParent();
                parent->remove(m_selectedWidget->ptr);
                parent->add(m_selectedWidget->ptr, name);

                m_selectedWidget->name = name;
                widgetFound = true;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::updateSelectionSquarePositions()
{
    assert(m_selectedWidget != nullptr);

    const auto& widget = m_selectedWidget->ptr;

    auto position = widget->getPosition();
    tgui::Container* parentWidget = widget->getParent();
    while (parentWidget != m_widgetsContainer.get())
    {
        position += parentWidget->getPosition() + parentWidget->getChildWidgetsOffset();
        parentWidget = parentWidget->getParent();
    }

    m_selectionSquares[0]->setPosition({position.x,                               position.y});
    m_selectionSquares[1]->setPosition({position.x + (widget->getSize().x / 2.f), position.y});
    m_selectionSquares[2]->setPosition({position.x + widget->getSize().x,         position.y});
    m_selectionSquares[3]->setPosition({position.x + widget->getSize().x,         position.y + (widget->getSize().y / 2.f)});
    m_selectionSquares[4]->setPosition({position.x + widget->getSize().x,         position.y + widget->getSize().y});
    m_selectionSquares[5]->setPosition({position.x + (widget->getSize().x / 2.f), position.y + widget->getSize().y});
    m_selectionSquares[6]->setPosition({position.x,                               position.y + widget->getSize().y});
    m_selectionSquares[7]->setPosition({position.x,                               position.y + (widget->getSize().y / 2.f)});

    // The positions given to the squares where those of its center
    for (auto& square : m_selectionSquares)
        square->setPosition({std::round(square->getPosition().x - (square->getSize().y / 2.f)),
                             std::round(square->getPosition().y - (square->getSize().x / 2.f))});
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectWidgetById(const std::string& id)
{
    selectWidget(m_widgets[id]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::mouseMoved(sf::Vector2i pos)
{
    if (m_draggingWidget || m_draggingSelectionSquare)
        onDrag(pos);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::mouseReleased()
{
    m_draggingWidget = false;
    m_draggingSelectionSquare = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::setFilename(const sf::String& filename)
{
    m_filename = filename;
    setChanged(m_changed);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sf::String Form::getFilename() const
{
    return m_filename;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::setSize(sf::Vector2i size)
{
    m_size = size;

    m_scrollablePanel->setContentSize(sf::Vector2f{m_size});
    m_widgetsContainer->setSize(sf::Vector2f{m_size});
    m_scrollablePanel->get("EventHandler")->setSize(sf::Vector2f{m_size});
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sf::Vector2i Form::getSize() const
{
    return m_size;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::setChanged(bool changed)
{
    if (changed)
        m_formWindow->setTitle("*" + m_filename);
    else
        m_formWindow->setTitle(m_filename);

    m_changed = changed;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Form::isChanged() const
{
    return m_changed;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Form::load()
{
    try
    {
        m_widgetsContainer->loadWidgetsFromFile(getFilename());
    }
    catch (const tgui::Exception& e)
    {
        // Failed to open file
        std::cout << "Failed to load '" << getFilename().toAnsiString() << "', reason: " << e.what() << std::endl;
        return false;
    }

    importLoadedWidgets(m_widgetsContainer);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::save()
{
    setChanged(false);
    m_widgetsContainer->saveWidgetsToFile(getFilename());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::importLoadedWidgets(tgui::Container::Ptr parent)
{
    const auto& widgets = parent->getWidgets();
    const auto& widgetNames = parent->getWidgetNames();
    for (std::size_t i = 0; i < widgets.size(); ++i)
    {
        const std::string id = tgui::to_string(widgets[i].get());
        m_widgets[id] = std::make_shared<WidgetInfo>(widgets[i]);
        m_widgets[id]->name = widgetNames[i];
        m_widgets[id]->theme = "Custom";

        // Keep track of the highest id found in widgets with default names, to avoid creating new widgets with confusing names
        if ((widgetNames[i].getSize() >= 7) && (widgetNames[i].substring(0, 6) == "Widget"))
        {
            const std::string potentialNumber = widgetNames[i].substring(6);
            if (std::all_of(potentialNumber.begin(), potentialNumber.end(), ::isdigit))
                m_idCounter = std::max(m_idCounter, static_cast<unsigned int>(tgui::stoi(potentialNumber)));
        }

        if (widgets[i]->isContainer())
            importLoadedWidgets(std::static_pointer_cast<tgui::Container>(widgets[i]));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::onSelectionSquarePress(tgui::Button::Ptr square, sf::Vector2f pos)
{
    m_draggingSelectionSquare = square;
    m_draggingPos = square->getPosition() + pos;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::Widget::Ptr Form::getWidgetBelowMouse(tgui::Container::Ptr parent, sf::Vector2f pos)
{
    // Loop through widgets in reverse order to find the top one in case of overlapping widgets
    const auto& widgets = parent->getWidgets();
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it)
    {
        tgui::Widget::Ptr widget = *it;
        if (widget && sf::FloatRect{widget->getPosition().x, widget->getPosition().y, widget->getSize().x, widget->getSize().y}.contains(pos))
        {
            if (widget->isContainer())
            {
                tgui::Container::Ptr container = std::static_pointer_cast<tgui::Container>(widget);
                tgui::Widget::Ptr child = getWidgetBelowMouse(container, pos - container->getPosition() - container->getChildWidgetsOffset());
                if (child)
                    return child;
            }

            return widget;
        }
    }

    return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::onFormMousePress(sf::Vector2f pos)
{
    auto widget = getWidgetBelowMouse(m_widgetsContainer, pos);
    if (widget)
    {
        selectWidget(m_widgets[tgui::to_string(widget.get())]);
        m_draggingWidget = true;
        m_draggingPos = pos;
    }
    else
        selectWidget(nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::onDrag(sf::Vector2i mousePos)
{
    assert(m_selectedWidget != nullptr);

    const sf::Vector2f pos = sf::Vector2f{mousePos} - m_formWindow->getPosition() - m_formWindow->getChildWidgetsOffset() + m_scrollablePanel->getContentOffset();
    auto selectedWidget = m_selectedWidget->ptr;
    const float step = 10;

    bool updated = false;

    if (m_draggingWidget)
    {
        while (pos.x - m_draggingPos.x >= step)
        {
            selectedWidget->setPosition({selectedWidget->getPosition().x + step, selectedWidget->getPosition().y});
            m_draggingPos.x += step;
            updated = true;
        }

        while (m_draggingPos.x - pos.x >= step)
        {
            selectedWidget->setPosition({selectedWidget->getPosition().x - step, selectedWidget->getPosition().y});
            m_draggingPos.x -= step;
            updated = true;
        }

        while (pos.y - m_draggingPos.y >= step)
        {
            selectedWidget->setPosition({selectedWidget->getPosition().x, selectedWidget->getPosition().y + step});
            m_draggingPos.y += step;
            updated = true;
        }

        while (m_draggingPos.y - pos.y >= step)
        {
            selectedWidget->setPosition({selectedWidget->getPosition().x, selectedWidget->getPosition().y - step});
            m_draggingPos.y -= step;
            updated = true;
        }
    }

    if (m_draggingSelectionSquare)
    {
        if (m_draggingSelectionSquare == m_selectionSquares[1]) // Top
        {
            while (pos.y - m_draggingPos.y >= step)
            {
                selectedWidget->setPosition({selectedWidget->getPosition().x, selectedWidget->getPosition().y + step});
                selectedWidget->setSize({selectedWidget->getSize().x, selectedWidget->getSize().y - step});
                m_draggingPos.y += step;
                updated = true;
            }

            while (m_draggingPos.y - pos.y >= step)
            {
                selectedWidget->setPosition({selectedWidget->getPosition().x, selectedWidget->getPosition().y - step});
                selectedWidget->setSize({selectedWidget->getSize().x, selectedWidget->getSize().y + step});
                m_draggingPos.y -= step;
                updated = true;
            }
        }
        else if (m_draggingSelectionSquare == m_selectionSquares[3]) // Right
        {
            while (pos.x - m_draggingPos.x >= step)
            {
                selectedWidget->setSize({selectedWidget->getSize().x + step, selectedWidget->getSize().y});
                m_draggingPos.x += step;
                updated = true;
            }

            while (m_draggingPos.x - pos.x >= step)
            {
                selectedWidget->setSize({selectedWidget->getSize().x - step, selectedWidget->getSize().y});
                m_draggingPos.x -= step;
                updated = true;
            }
        }
        else if (m_draggingSelectionSquare == m_selectionSquares[5]) // Bottom
        {
            while (pos.y - m_draggingPos.y >= step)
            {
                selectedWidget->setSize({selectedWidget->getSize().x, selectedWidget->getSize().y + step});
                m_draggingPos.y += step;
                updated = true;
            }

            while (m_draggingPos.y - pos.y >= step)
            {
                selectedWidget->setSize({selectedWidget->getSize().x, selectedWidget->getSize().y - step});
                m_draggingPos.y -= step;
                updated = true;
            }
        }
        else if (m_draggingSelectionSquare == m_selectionSquares[7]) // Left
        {
            while (pos.x - m_draggingPos.x >= step)
            {
                selectedWidget->setPosition({selectedWidget->getPosition().x + step, selectedWidget->getPosition().y});
                selectedWidget->setSize({selectedWidget->getSize().x - step, selectedWidget->getSize().y});
                m_draggingPos.x += step;
                updated = true;
            }

            while (m_draggingPos.x - pos.x >= step)
            {
                selectedWidget->setPosition({selectedWidget->getPosition().x - step, selectedWidget->getPosition().y});
                selectedWidget->setSize({selectedWidget->getSize().x + step, selectedWidget->getSize().y});
                m_draggingPos.x -= step;
                updated = true;
            }
        }
        else // Corner
        {
            const float ratio = selectedWidget->getSize().y / selectedWidget->getSize().x;

            sf::Vector2f change;
            if (ratio <= 1)
                change = sf::Vector2f(step, step * ratio);
            else
                change = sf::Vector2f(step / ratio, step);

            if (m_draggingSelectionSquare == m_selectionSquares[0]) // Top left
            {
                while ((pos.x - m_draggingPos.x >= change.x) && (pos.y - m_draggingPos.y >= change.y))
                {
                    selectedWidget->setPosition({selectedWidget->getPosition().x + change.x, selectedWidget->getPosition().y + change.y});
                    selectedWidget->setSize({selectedWidget->getSize().x - change.x, selectedWidget->getSize().y - change.y});
                    m_draggingPos.x += change.x;
                    m_draggingPos.y += change.y;
                    updated = true;
                }

                while ((m_draggingPos.x - pos.x >= change.x) && (m_draggingPos.y - pos.y >= change.y))
                {
                    selectedWidget->setPosition({selectedWidget->getPosition().x - change.x, selectedWidget->getPosition().y - change.y});
                    selectedWidget->setSize({selectedWidget->getSize().x + change.x, selectedWidget->getSize().y + change.y});
                    m_draggingPos.x -= change.x;
                    m_draggingPos.y -= change.y;
                    updated = true;
                }
            }
            else if (m_draggingSelectionSquare == m_selectionSquares[2]) // Top right
            {
                while ((m_draggingPos.x - pos.x >= change.x) && (pos.y - m_draggingPos.y >= change.y))
                {
                    selectedWidget->setPosition({selectedWidget->getPosition().x, selectedWidget->getPosition().y + change.y});
                    selectedWidget->setSize({selectedWidget->getSize().x - change.x, selectedWidget->getSize().y - change.y});
                    m_draggingPos.x -= change.x;
                    m_draggingPos.y += change.y;
                    updated = true;
                }

                while ((pos.x - m_draggingPos.x >= change.x) && (m_draggingPos.y - pos.y >= change.y))
                {
                    selectedWidget->setPosition({selectedWidget->getPosition().x, selectedWidget->getPosition().y - change.y});
                    selectedWidget->setSize({selectedWidget->getSize().x + change.x, selectedWidget->getSize().y + change.y});
                    m_draggingPos.x += change.x;
                    m_draggingPos.y -= change.y;
                    updated = true;
                }
            }
            else if (m_draggingSelectionSquare == m_selectionSquares[4]) // Bottom right
            {
                while ((m_draggingPos.x - pos.x >= change.x) && (m_draggingPos.y - pos.y >= change.y))
                {
                    selectedWidget->setSize({selectedWidget->getSize().x - change.x, selectedWidget->getSize().y - change.y});
                    m_draggingPos.x -= change.x;
                    m_draggingPos.y -= change.y;
                    updated = true;
                }

                while ((pos.x - m_draggingPos.x >= change.x) && (pos.y - m_draggingPos.y >= change.y))
                {
                    selectedWidget->setSize({selectedWidget->getSize().x + change.x, selectedWidget->getSize().y + change.y});
                    m_draggingPos.x += change.x;
                    m_draggingPos.y += change.y;
                    updated = true;
                }
            }
            else if (m_draggingSelectionSquare == m_selectionSquares[6]) // Bottom left
            {
                while ((pos.x - m_draggingPos.x >= change.x) && (m_draggingPos.y - pos.y >= change.y))
                {
                    selectedWidget->setPosition({selectedWidget->getPosition().x + change.x, selectedWidget->getPosition().y});
                    selectedWidget->setSize({selectedWidget->getSize().x - change.x, selectedWidget->getSize().y - change.y});
                    m_draggingPos.x += change.x;
                    m_draggingPos.y -= change.y;
                    updated = true;
                }

                while ((m_draggingPos.x - pos.x >= change.x) && (pos.y - m_draggingPos.y >= change.y))
                {
                    selectedWidget->setPosition({selectedWidget->getPosition().x - change.x, selectedWidget->getPosition().y});
                    selectedWidget->setSize({selectedWidget->getSize().x + change.x, selectedWidget->getSize().y + change.y});
                    m_draggingPos.x -= change.x;
                    m_draggingPos.y += change.y;
                    updated = true;
                }
            }
        }
    }

    if (updated)
    {
        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectWidget(std::shared_ptr<WidgetInfo> widget)
{
    if (m_selectedWidget != widget)
    {
        m_selectedWidget = widget;

        if (widget)
        {
            updateSelectionSquarePositions();
            for (auto& square : m_selectionSquares)
                square->setVisible(true);
        }
        else // No widget selected
        {
            for (auto& square : m_selectionSquares)
                square->setVisible(false);
        }
    }

    m_guiBuilder->widgetSelected(widget ? widget->ptr : nullptr);
}
