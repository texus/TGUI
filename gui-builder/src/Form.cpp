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


#include "Form.hpp"
#include "GuiBuilder.hpp"

#include <stack>
#include <cassert>
#include <cmath>

const static float MOVE_STEP = 10;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Form::Form(GuiBuilder* guiBuilder, const std::string& filename, tgui::ChildWindow::Ptr formWindow, sf::Vector2f formSize) :
    m_guiBuilder      {guiBuilder},
    m_formWindow      {formWindow},
    m_scrollablePanel {formWindow->get<tgui::ScrollablePanel>("ScrollablePanel")},
    m_widgetsContainer{m_scrollablePanel->get<tgui::Panel>("WidgetContainer")},
    m_filename        {filename}
{
    m_widgets["form"] = nullptr;

    m_formWindow->setTitle(filename);
    m_formWindow->connect("Closed", [this]{ m_guiBuilder->closeForm(this); });
    m_formWindow->connect("SizeChanged", [this] { m_scrollablePanel->setSize(m_formWindow->getSize()); });

    auto eventHandler = tgui::ClickableWidget::create();
    eventHandler->connect("MousePressed", [=](sf::Vector2f pos){ onFormMousePress(pos); });
    m_scrollablePanel->add(eventHandler, "EventHandler");

    m_scrollablePanel->setSize(m_formWindow->getSize());
    setSize(formSize);

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

std::string Form::addWidget(tgui::Widget::Ptr widget, tgui::Container* parent, bool selectNewWidget)
{
    const std::string id = tgui::to_string(widget.get());
    m_widgets[id] = std::make_shared<WidgetInfo>(widget);

    const std::string widgetType = widget->getWidgetType();
    bool foundAvailableName = false;
    unsigned int count = 0;
    std::string name;
    while (!foundAvailableName)
    {
        name = widgetType + tgui::to_string(++count);

        foundAvailableName = true;
        for (const auto& pair : m_widgets)
        {
            const auto& widgetInfo = pair.second;
            if (widgetInfo && (widgetInfo->name == name))
            {
                foundAvailableName = false;
                break;
            }
        }
    }

    m_widgets[id]->name = name;

    if (parent)
        parent->add(widget, name);
    else
        m_widgetsContainer->add(widget, name);

    if (selectNewWidget)
        selectWidget(m_widgets[id]);

    setChanged(true);
    return name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::removeWidget(const std::string& id)
{
    const auto widget = m_widgets[id];
    assert(widget != nullptr);

    // Remove the child widgets
    if (widget->ptr->isContainer())
    {
        std::vector<std::string> childIds;
        std::stack<tgui::Container::Ptr> parentsToSearch;
        parentsToSearch.push(widget->ptr->cast<tgui::Container>());
        while (!parentsToSearch.empty())
        {
            tgui::Container::Ptr parent = parentsToSearch.top();
            parentsToSearch.pop();
            for (const auto& childWidget : parent->getWidgets())
            {
                childIds.push_back(tgui::to_string(childWidget.get()));
                if (childWidget->isContainer())
                    parentsToSearch.push(childWidget->cast<tgui::Container>());
            }
        }

        for (const auto& childId : childIds)
            m_widgets.erase(childId);
    }

    // Now remove the widget itself
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

std::shared_ptr<WidgetInfo> Form::getWidgetByName(const std::string& name) const
{
    if (name == m_filename)
        return std::shared_ptr<WidgetInfo>();

    const auto it = std::find_if(
        m_widgets.begin(),
        m_widgets.end(),
        [&name](const auto& idAndWidgetInfo)
        {
            const auto& widgetIndo = idAndWidgetInfo.second;
            if (widgetIndo)
                return idAndWidgetInfo.second->name == name;
            else
                return false;
        }
    );

    assert(it != m_widgets.end());
    return it->second;
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

std::shared_ptr<tgui::Group> Form::getRootWidgetsGroup() const
{
    return m_widgetsContainer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<WidgetInfo> Form::getSelectedWidget() const
{
    return m_selectedWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Form::setSelectedWidgetName(const std::string& name)
{
    assert(m_selectedWidget != nullptr);

    // Don't allow setting a name that is already used by a different widget
    for (const auto& pair : m_widgets)
    {
        const auto& widgetInfo = pair.second;
        if (widgetInfo && (widgetInfo->name == name))
        {
            if (widgetInfo->ptr != m_selectedWidget->ptr)
                return false;
        }
    }

    m_selectedWidget->ptr->setWidgetName(name);
    m_selectedWidget->name = name;
    return true;
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

    // Exception for ChildWindow as its size is currently its client size instead of the full size
    if (widget->getWidgetType() == "ChildWindow")
    {
        auto renderer = widget->cast<tgui::ChildWindow>()->getSharedRenderer();
        position.x += renderer->getBorders().getLeft();
        position.y += renderer->getBorders().getTop() + renderer->getTitleBarHeight() + renderer->getBorderBelowTitleBar();
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
    {
        square->setPosition({std::round(square->getPosition().x - (square->getSize().y / 2.f)),
                             std::round(square->getPosition().y - (square->getSize().x / 2.f))});
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectWidgetById(const std::string& id)
{
    selectWidget(m_widgets[id]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectWidgetByName(const std::string& name)
{
    selectWidget(getWidgetByName(name));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectParent()
{
    if (!m_selectedWidget)
        return;

    // If the widget was added directly to the form then select the form
    if (m_selectedWidget->ptr->getParent() == m_widgetsContainer.get())
    {
        selectWidget(nullptr);
        return;
    }

    selectWidget(m_widgets[tgui::to_string(m_selectedWidget->ptr->getParent())]);
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

bool Form::rightMouseClick(sf::Vector2i pos)
{
    sf::Vector2f relativeWindowPos{(pos.x - m_formWindow->getAbsolutePosition().x), (pos.y - m_formWindow->getAbsolutePosition().y)};
    if (!tgui::FloatRect{m_formWindow->getChildWidgetsOffset(), m_formWindow->getSize()}.contains(relativeWindowPos))
        return false;

    const sf::Vector2f relativePanelPos{
        (pos.x - m_scrollablePanel->get("EventHandler")->getAbsolutePosition().x),
        (pos.y - m_scrollablePanel->get("EventHandler")->getAbsolutePosition().y)};

    onFormMousePress(relativePanelPos);
    mouseReleased();
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::arrowKeyPressed(const sf::Event::KeyEvent& keyEvent)
{
    if (!m_selectedWidget)
        return;

    auto selectedWidget = m_selectedWidget->ptr;

    if (keyEvent.shift && keyEvent.control)
    {
        if (keyEvent.code == sf::Keyboard::Left)
            selectedWidget->setPosition({selectedWidget->getPosition().x - MOVE_STEP, selectedWidget->getPositionLayout().y});
        else if (keyEvent.code == sf::Keyboard::Right)
            selectedWidget->setPosition({selectedWidget->getPosition().x + MOVE_STEP, selectedWidget->getPositionLayout().y});
        else if (keyEvent.code == sf::Keyboard::Up)
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y - MOVE_STEP});
        else if (keyEvent.code == sf::Keyboard::Down)
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y + MOVE_STEP});

        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
    else if (keyEvent.shift)
    {
        if (keyEvent.code == sf::Keyboard::Left)
            selectedWidget->setSize({selectedWidget->getSize().x - 1, selectedWidget->getSizeLayout().y});
        else if (keyEvent.code == sf::Keyboard::Right)
            selectedWidget->setSize({selectedWidget->getSize().x + 1, selectedWidget->getSizeLayout().y});
        else if (keyEvent.code == sf::Keyboard::Up)
            selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y - 1});
        else if (keyEvent.code == sf::Keyboard::Down)
            selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y + 1});

        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
    else if (keyEvent.control)
    {
        if (keyEvent.code == sf::Keyboard::Left)
            selectedWidget->setPosition({selectedWidget->getPosition().x - 1, selectedWidget->getPositionLayout().y});
        else if (keyEvent.code == sf::Keyboard::Right)
            selectedWidget->setPosition({selectedWidget->getPosition().x + 1, selectedWidget->getPositionLayout().y});
        else if (keyEvent.code == sf::Keyboard::Up)
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y - 1});
        else if (keyEvent.code == sf::Keyboard::Down)
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y + 1});

        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
    else
    {
        sf::Vector2f selectedWidgetPoint;
        if (keyEvent.code == sf::Keyboard::Left)
            selectedWidgetPoint = {selectedWidget->getPosition().x, selectedWidget->getPosition().y + (selectedWidget->getSize().y / 2.f)};
        else if (keyEvent.code == sf::Keyboard::Right)
            selectedWidgetPoint = {selectedWidget->getPosition().x + selectedWidget->getSize().x, selectedWidget->getPosition().y + (selectedWidget->getSize().y / 2.f)};
        else if (keyEvent.code == sf::Keyboard::Up)
            selectedWidgetPoint = {selectedWidget->getPosition().x + (selectedWidget->getSize().x / 2.f), selectedWidget->getPosition().y};
        else if (keyEvent.code == sf::Keyboard::Down)
            selectedWidgetPoint = {selectedWidget->getPosition().x + (selectedWidget->getSize().x / 2.f), selectedWidget->getPosition().y + selectedWidget->getSize().y};

        float closestDistance = std::numeric_limits<float>::infinity();
        tgui::Widget::Ptr closestWidget = nullptr;
        const auto widgets = selectedWidget->getParent()->getWidgets();
        for (const auto& widget : widgets)
        {
            if (widget == selectedWidget)
                continue;

            sf::Vector2f widgetPoint;
            if (keyEvent.code == sf::Keyboard::Left)
                widgetPoint = {widget->getPosition().x + widget->getSize().x, widget->getPosition().y + (widget->getSize().y / 2.f)};
            else if (keyEvent.code == sf::Keyboard::Right)
                widgetPoint = {widget->getPosition().x, widget->getPosition().y + (widget->getSize().y / 2.f)};
            else if (keyEvent.code == sf::Keyboard::Up)
                widgetPoint = {widget->getPosition().x + (widget->getSize().x / 2.f), widget->getPosition().y + widget->getSize().y};
            else if (keyEvent.code == sf::Keyboard::Down)
                widgetPoint = {widget->getPosition().x + (widget->getSize().x / 2.f), widget->getPosition().y};

            // Don't allow going in the opposite direction when there are no widgets on the chosen side
            if (((keyEvent.code == sf::Keyboard::Left) && (widgetPoint.x >= selectedWidgetPoint.x))
             || ((keyEvent.code == sf::Keyboard::Right) && (widgetPoint.x <= selectedWidgetPoint.x))
             || ((keyEvent.code == sf::Keyboard::Up) && (widgetPoint.y >= selectedWidgetPoint.y))
             || ((keyEvent.code == sf::Keyboard::Down) && (widgetPoint.y <= selectedWidgetPoint.y)))
                continue;

            const float distance = std::abs(widgetPoint.x - selectedWidgetPoint.x) + std::abs(widgetPoint.y - selectedWidgetPoint.y);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestWidget = widget;
            }
        }

        if (closestWidget)
            selectWidget(m_widgets[tgui::to_string(closestWidget.get())]);
    }
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

void Form::setSize(sf::Vector2f size)
{
    m_size = size;

    m_widgetsContainer->setSize(m_size);
    m_scrollablePanel->setContentSize(m_size);
    m_scrollablePanel->get("EventHandler")->setSize(m_size);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sf::Vector2f Form::getSize() const
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

void Form::focus()
{
    m_formWindow->setFocused(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Form::hasFocus() const
{
    return m_formWindow->isFocused();
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

    m_guiBuilder->formSaved(getFilename());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::drawExtra(sf::RenderWindow& window) const
{
    if (!m_selectedWidget)
        return;

    if (!m_draggingWidget && !m_draggingSelectionSquare
     && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)
     && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
        return;

    const auto selectedWidget = m_selectedWidget->ptr;
    const sf::Vector2f selectedTopLeft = selectedWidget->getAbsolutePosition();
    const sf::Vector2f selectedBottomRight = selectedWidget->getAbsolutePosition() + selectedWidget->getSize();
    const auto widgets = selectedWidget->getParent()->getWidgets();
    for (const auto& widget : widgets)
    {
        if (widget == selectedWidget)
            continue;

        const sf::Vector2f topLeft = widget->getAbsolutePosition();
        const sf::Vector2f bottomRight = widget->getAbsolutePosition() + widget->getSize();

        const float minX = std::min({selectedTopLeft.x, selectedBottomRight.x, topLeft.x, bottomRight.x});
        const float maxX = std::max({selectedTopLeft.x, selectedBottomRight.x, topLeft.x, bottomRight.x});
        const float minY = std::min({selectedTopLeft.y, selectedBottomRight.y, topLeft.y, bottomRight.y});
        const float maxY = std::max({selectedTopLeft.y, selectedBottomRight.y, topLeft.y, bottomRight.y});

        if ((topLeft.x == selectedTopLeft.x) || (topLeft.x == selectedBottomRight.x))
            drawLine(window, {topLeft.x, minY}, {topLeft.x, maxY});
        if ((topLeft.y == selectedTopLeft.y) || (topLeft.y == selectedBottomRight.y))
            drawLine(window, {minX, topLeft.y}, {maxX, topLeft.y});
        if ((bottomRight.x == selectedBottomRight.x) || (bottomRight.x == selectedTopLeft.x))
            drawLine(window, {bottomRight.x, minY}, {bottomRight.x, maxY});
        if ((bottomRight.y == selectedBottomRight.y) || (bottomRight.y == selectedTopLeft.y))
            drawLine(window, {minX, bottomRight.y}, {maxX, bottomRight.y});
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::importLoadedWidgets(tgui::Container::Ptr parent)
{
    const auto& widgets = parent->getWidgets();
    for (std::size_t i = 0; i < widgets.size(); ++i)
    {
        const std::string id = tgui::to_string(widgets[i].get());
        m_widgets[id] = std::make_shared<WidgetInfo>(widgets[i]);
        m_widgets[id]->name = widgets[i]->getWidgetName();
        m_widgets[id]->theme = "Custom";

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
        if (widget && sf::FloatRect{widget->getPosition().x, widget->getPosition().y, widget->getFullSize().x, widget->getFullSize().y}.contains(pos))
        {
            // Skip invisible widgets, those have to be selected using the combo box in the properties window.
            // This prevents clicking on stuff you don't see instead of the thing you are trying to click on.
            if (!widget->isVisible())
                continue;

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

    bool updated = false;

    if (m_draggingWidget)
    {
        while (pos.x - m_draggingPos.x >= MOVE_STEP)
        {
            selectedWidget->setPosition({selectedWidget->getPosition().x + MOVE_STEP, selectedWidget->getPositionLayout().y});
            m_draggingPos.x += MOVE_STEP;
            updated = true;
        }

        while (m_draggingPos.x - pos.x >= MOVE_STEP)
        {
            selectedWidget->setPosition({selectedWidget->getPosition().x - MOVE_STEP, selectedWidget->getPositionLayout().y});
            m_draggingPos.x -= MOVE_STEP;
            updated = true;
        }

        while (pos.y - m_draggingPos.y >= MOVE_STEP)
        {
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y + MOVE_STEP});
            m_draggingPos.y += MOVE_STEP;
            updated = true;
        }

        while (m_draggingPos.y - pos.y >= MOVE_STEP)
        {
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y - MOVE_STEP});
            m_draggingPos.y -= MOVE_STEP;
            updated = true;
        }
    }

    if (m_draggingSelectionSquare)
    {
        if (m_draggingSelectionSquare == m_selectionSquares[1]) // Top
        {
            while (pos.y - m_draggingPos.y >= MOVE_STEP)
            {
                selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y + MOVE_STEP});
                selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y - MOVE_STEP});
                m_draggingPos.y += MOVE_STEP;
                updated = true;
            }

            while (m_draggingPos.y - pos.y >= MOVE_STEP)
            {
                selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y - MOVE_STEP});
                selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y + MOVE_STEP});
                m_draggingPos.y -= MOVE_STEP;
                updated = true;
            }
        }
        else if (m_draggingSelectionSquare == m_selectionSquares[3]) // Right
        {
            while (pos.x - m_draggingPos.x >= MOVE_STEP)
            {
                selectedWidget->setSize({selectedWidget->getSize().x + MOVE_STEP, selectedWidget->getSizeLayout().y});
                m_draggingPos.x += MOVE_STEP;
                updated = true;
            }

            while (m_draggingPos.x - pos.x >= MOVE_STEP)
            {
                selectedWidget->setSize({selectedWidget->getSize().x - MOVE_STEP, selectedWidget->getSizeLayout().y});
                m_draggingPos.x -= MOVE_STEP;
                updated = true;
            }
        }
        else if (m_draggingSelectionSquare == m_selectionSquares[5]) // Bottom
        {
            while (pos.y - m_draggingPos.y >= MOVE_STEP)
            {
                selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y + MOVE_STEP});
                m_draggingPos.y += MOVE_STEP;
                updated = true;
            }

            while (m_draggingPos.y - pos.y >= MOVE_STEP)
            {
                selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y - MOVE_STEP});
                m_draggingPos.y -= MOVE_STEP;
                updated = true;
            }
        }
        else if (m_draggingSelectionSquare == m_selectionSquares[7]) // Left
        {
            while (pos.x - m_draggingPos.x >= MOVE_STEP)
            {
                selectedWidget->setPosition({selectedWidget->getPosition().x + MOVE_STEP, selectedWidget->getPositionLayout().y});
                selectedWidget->setSize({selectedWidget->getSize().x - MOVE_STEP, selectedWidget->getSizeLayout().y});
                m_draggingPos.x += MOVE_STEP;
                updated = true;
            }

            while (m_draggingPos.x - pos.x >= MOVE_STEP)
            {
                selectedWidget->setPosition({selectedWidget->getPosition().x - MOVE_STEP, selectedWidget->getPositionLayout().y});
                selectedWidget->setSize({selectedWidget->getSize().x + MOVE_STEP, selectedWidget->getSizeLayout().y});
                m_draggingPos.x -= MOVE_STEP;
                updated = true;
            }
        }
        else // Corner
        {
            const float ratio = selectedWidget->getSize().y / selectedWidget->getSize().x;

            sf::Vector2f change;
            if (ratio <= 1)
                change = sf::Vector2f(MOVE_STEP, MOVE_STEP * ratio);
            else
                change = sf::Vector2f(MOVE_STEP / ratio, MOVE_STEP);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::drawLine(sf::RenderWindow& window, sf::Vector2f startPoint, sf::Vector2f endPoint) const
{
    sf::Vertex line[2] = {
        {startPoint + sf::Vector2f{0.5f, 0.5f}, sf::Color{0, 0, 139}},
        {endPoint + sf::Vector2f{0.5f, 0.5f}, sf::Color{0, 0, 139}}
    };
    window.draw(line, 2, sf::Lines);
}
