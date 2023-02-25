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


#include "GuiBuilder.hpp"
#include "Form.hpp"

// WidgetProperties are only included to allow importing 0.8 form files.
// No new widgets need to be added here and this can be removed again in the future.
#include "WidgetProperties/BitmapButtonProperties.hpp"
#include "WidgetProperties/ButtonProperties.hpp"
#include "WidgetProperties/ChatBoxProperties.hpp"
#include "WidgetProperties/ChildWindowProperties.hpp"
#include "WidgetProperties/ComboBoxProperties.hpp"
#include "WidgetProperties/EditBoxProperties.hpp"
#include "WidgetProperties/GroupProperties.hpp"
#include "WidgetProperties/KnobProperties.hpp"
#include "WidgetProperties/LabelProperties.hpp"
#include "WidgetProperties/ListBoxProperties.hpp"
#include "WidgetProperties/ListViewProperties.hpp"
#include "WidgetProperties/PanelProperties.hpp"
#include "WidgetProperties/PictureProperties.hpp"
#include "WidgetProperties/ProgressBarProperties.hpp"
#include "WidgetProperties/RadioButtonProperties.hpp"
#include "WidgetProperties/RangeSliderProperties.hpp"
#include "WidgetProperties/ScrollablePanelProperties.hpp"
#include "WidgetProperties/ScrollbarProperties.hpp"
#include "WidgetProperties/SliderProperties.hpp"
#include "WidgetProperties/SpinButtonProperties.hpp"
#include "WidgetProperties/SpinControlProperties.hpp"
#include "WidgetProperties/TabsProperties.hpp"
#include "WidgetProperties/TextAreaProperties.hpp"
#include "WidgetProperties/TreeViewProperties.hpp"

const static float MOVE_STEP = 10;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void importOldFormFileExtractValidProperties(std::set<tgui::String>& possibleProperties, const PropertyValueMap& propertyMap)
{
    for (const auto& pair : propertyMap)
        possibleProperties.insert(pair.first);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void importOldFormFileFixRendererProperties(const std::unique_ptr<tgui::DataIO::Node>& parentNode, const std::set<tgui::String>& possibleProperties)
{
    for (const auto& node : parentNode->children)
    {
        if (!node->children.empty())
            importOldFormFileFixRendererProperties(node, possibleProperties);

        auto nameSeparator = node->name.find('.');
        auto widgetType = node->name.substr(0, nameSeparator);

        // Rename TextBox type to TextArea
        if (widgetType == U"TextBox")
            node->name = U"TextArea" + node->name.substr(nameSeparator);

        if ((node->name != U"Renderer") && (widgetType != U"Renderer"))
            continue;

        auto oldPropertyValuePairs = std::move(node->propertyValuePairs);
        node->propertyValuePairs.clear();
        for (auto& property : oldPropertyValuePairs)
        {
            bool matchingPropertyFound = false;
            for (const auto& validProperty : possibleProperties)
            {
                if (property.first.equalIgnoreCase(validProperty))
                {
                    node->propertyValuePairs[validProperty] = std::move(property.second);
                    matchingPropertyFound = true;
                    break;
                }
            }

            // If the property is invalid then just keep the old value. It will give an error later.
            if (!matchingPropertyFound)
                node->propertyValuePairs[property.first] = std::move(property.second);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void importOldForm(const std::unique_ptr<tgui::DataIO::Node>& rootNode)
{
    // Construct a list of all existing renderer properties for all widgets.
    // Since the renderers may be global in the form file, we can't know which widget type will use it (without some effort),
    // so instead we just get the properties for all widget types to match on.
    std::set<tgui::String> possibleProperties;
    importOldFormFileExtractValidProperties(possibleProperties, BitmapButtonProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("BitmapButton")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ButtonProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Button")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ChatBoxProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("ChatBox")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ChildWindowProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("ChildWindow")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ComboBoxProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("ComboBox")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, EditBoxProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("EditBox")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, GroupProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Group")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, KnobProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Knob")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, LabelProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Label")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ListBoxProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("ListBox")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, PanelProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Panel")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, PictureProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Picture")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ProgressBarProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("ProgressBar")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, RadioButtonProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("RadioButton")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, RangeSliderProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("RangeSlider")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ScrollablePanelProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("ScrollablePanel")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, ScrollbarProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Scrollbar")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, SliderProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Slider")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, SpinButtonProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("SpinButton")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, TabsProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("Tabs")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, TextAreaProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("TextArea")()).second);
    importOldFormFileExtractValidProperties(possibleProperties, TreeViewProperties().initProperties(
        tgui::WidgetFactory::getConstructFunction("TreeView")()).second);

    // Convert renderer properties from lowercase to the correct case-sensitive string
    importOldFormFileFixRendererProperties(rootNode, possibleProperties);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void makePathsAbsolute(const std::unique_ptr<tgui::DataIO::Node>& node, const tgui::Filesystem::Path& formPath)
{
    for (const auto& pair : node->propertyValuePairs)
    {
        if (((pair.first.size() >= 7) && (pair.first.substr(0, 7) == U"Texture")) || (pair.first == U"Font") || (pair.first == U"Image"))
        {
            if (pair.second->value.empty() || pair.second->value.equalIgnoreCase(U"none") || pair.second->value.equalIgnoreCase(U"null") || pair.second->value.equalIgnoreCase(U"nullptr"))
                continue;

            // Skip absolute paths
            if (pair.second->value[0] != '"')
            {
#ifdef TGUI_SYSTEM_WINDOWS
                if ((pair.second->value[0] == '/') || (pair.second->value[0] == '\\') || ((pair.second->value.size() > 1) && (pair.second->value[1] == ':')))
#else
                if (pair.second->value[0] == '/')
#endif
                    continue;
            }
            else // The filename is between quotes
            {
                if (pair.second->value.size() <= 1)
                    continue;

#ifdef TGUI_SYSTEM_WINDOWS
                if ((pair.second->value[1] == '/') || (pair.second->value[1] == '\\') || ((pair.second->value.size() > 2) && (pair.second->value[2] == ':')))
#else
                if (pair.second->value[1] == '/')
#endif
                    continue;
            }

            tgui::String filename;
            if (pair.second->value[0] != '"')
                filename = pair.second->value;
            else
            {
                // The filename is surrounded by quotes, with optional options behind it
                const auto endQuotePos = pair.second->value.find('"', 1);
                assert(endQuotePos != tgui::String::npos);
                filename = pair.second->value.substr(1, endQuotePos - 1);
            }

            // If the file can't be found anywhere then don't inject the file path
            const bool fileFoundRelativeToForm = tgui::Filesystem::fileExists(formPath / filename);
            const bool fileFoundRelativeToGuiBuilder = tgui::Filesystem::fileExists(tgui::getResourcePath() / filename);
            if (!fileFoundRelativeToForm && !fileFoundRelativeToGuiBuilder)
                continue;

            tgui::String pathToInject = fileFoundRelativeToForm ? formPath.asString() : tgui::getResourcePath().asString();
            assert(!pathToInject.empty());
            if (pathToInject.back() == '/')
                pathToInject.pop_back();

            // Insert the path into the filename.
            // We can't just deserialize the value to get rid of the quotes as it may contain things behind the filename.
            if (pair.second->value[0] != '"')
                pair.second->value = pathToInject + '/' + pair.second->value;
            else // The filename is between quotes
                pair.second->value = '"' + pathToInject + '/' + pair.second->value.substr(1);
        }
    }

    for (const auto& child : node->children)
        makePathsAbsolute(child, formPath);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void makePathsRelative(const std::unique_ptr<tgui::DataIO::Node>& node, const tgui::String& formPath, const tgui::String& guiBuilderPath)
{
    for (const auto& pair : node->propertyValuePairs)
    {
        if (((pair.first.size() >= 7) && (pair.first.substr(0, 7) == U"Texture")) || (pair.first == U"Font") || (pair.first == U"Image"))
        {
            if (pair.second->value.empty() || pair.second->value.equalIgnoreCase(U"none") || pair.second->value.equalIgnoreCase(U"null") || pair.second->value.equalIgnoreCase(U"nullptr"))
                continue;

            // Skip paths that are already relative (this shouldn't happen)
            if (pair.second->value[0] != '"')
            {
#ifdef TGUI_SYSTEM_WINDOWS
                if ((pair.second->value[0] != '/') && (pair.second->value[0] != '\\') && ((pair.second->value.size() <= 1) || (pair.second->value[1] != ':')))
#else
                if (pair.second->value[0] != '/')
#endif
                    continue;
            }
            else // The filename is between quotes
            {
                if (pair.second->value.size() <= 1)
                    continue;

#ifdef TGUI_SYSTEM_WINDOWS
                if ((pair.second->value[1] != '/') && (pair.second->value[1] != '\\') && ((pair.second->value.size() <= 2) || (pair.second->value[2] != ':')))
#else
                if (pair.second->value[1] != '/')
#endif
                    continue;
            }

            tgui::String filename;
            if (pair.second->value[0] != '"')
                filename = pair.second->value;
            else
            {
                // The filename is surrounded by quotes, with optional options behind it
                const auto endQuotePos = pair.second->value.find('"', 1);
                assert(endQuotePos != tgui::String::npos); // DataIO wouldn't have accepted the file if there is no close quote
                filename = pair.second->value.substr(1, endQuotePos - 1);
            }

            // Make the path relative to the form or gui builder
            if (filename.starts_with(formPath))
            {
                if (pair.second->value[0] != '"')
                    pair.second->value.erase(0, formPath.length());
                else
                    pair.second->value.erase(1, formPath.length());
            }
            else if (filename.starts_with(guiBuilderPath))
            {
                if (pair.second->value[0] != '"')
                    pair.second->value.erase(0, guiBuilderPath.length());
                else
                    pair.second->value.erase(1, guiBuilderPath.length());
            }
            else // We will need to use ".." if we want to make the path relative
            {
                // Split the form path an filename in parts (the folders that make up the path).
                tgui::String lastPath;
                tgui::Filesystem::Path path = tgui::Filesystem::Path(formPath);
                std::vector<tgui::String> formPathParts;
                while (path.asString() != lastPath)
                {
                    if (!path.getFilename().empty())
                        formPathParts.insert(formPathParts.begin(), path.getFilename());

                    lastPath = path.asString();
                    path = path.getParentPath();
                }

                lastPath = U"";
                path = tgui::Filesystem::Path(filename).getParentPath();
                std::vector<tgui::String> resourceParts;
                while (path.asString() != lastPath)
                {
                    if (!path.getFilename().empty())
                        resourceParts.insert(resourceParts.begin(), path.getFilename());

                    lastPath = path.asString();
                    path = path.getParentPath();
                }

                const std::size_t originalFormPartsLength = formPathParts.size();
                while (!formPathParts.empty() && !resourceParts.empty())
                {
                    if (formPathParts.size() > resourceParts.size())
                        formPathParts.pop_back();
                    else if (formPathParts.size() < resourceParts.size())
                        resourceParts.pop_back();
                    else
                    {
                        // Check if both subpaths are equal
                        bool pathsEqual = true;
                        for (std::size_t i = 0; pathsEqual && (i < formPathParts.size()); ++i)
                            pathsEqual &= (formPathParts[i] == resourceParts[i]);

                        if (pathsEqual)
                        {
                            assert(originalFormPartsLength > formPathParts.size());
                            const std::size_t pathsToGoUp = originalFormPartsLength - formPathParts.size();

                            tgui::String relativePath;
                            tgui::Filesystem::Path basePath = tgui::Filesystem::Path(formPath).getParentPath();
                            assert(basePath.asString() + '/' == formPath);
                            for (std::size_t i = 0; i < pathsToGoUp; ++i)
                            {
                                relativePath += U"../";
                                basePath = basePath.getParentPath();
                            }

                            tgui::String basePathStr = basePath.asString();
                            assert(!basePathStr.empty());
                            if (basePathStr.back() != '/')
                                basePathStr.push_back('/');

                            if (filename.starts_with(basePathStr))
                            {
                                if (pair.second->value[0] != '"')
                                {
                                    pair.second->value.erase(0, basePathStr.length());
                                    pair.second->value = relativePath + pair.second->value;
                                }
                                else
                                {
                                    pair.second->value.erase(0, basePathStr.length() + 1);
                                    pair.second->value = U'"' + relativePath + pair.second->value;
                                }
                            }
                            else
                                std::cerr << "Failed to make path relative. '" + filename + "' does not start with '" + basePathStr + "'." << std::endl;

                            break;
                        }
                        else
                        {
                            formPathParts.pop_back();
                            resourceParts.pop_back();
                        }
                    }
                }
            }
        }
    }

    for (const auto& child : node->children)
        makePathsRelative(child, formPath, guiBuilderPath);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Form::Form(GuiBuilder* guiBuilder, const tgui::String& filename, const tgui::ChildWindow::Ptr& formWindow, tgui::Vector2f formSize) :
    m_guiBuilder      {guiBuilder},
    m_formWindow      {formWindow},
    m_scrollablePanel {formWindow->get<tgui::ScrollablePanel>("ScrollablePanel")},
    m_widgetsContainer{m_scrollablePanel->get<tgui::Panel>("WidgetContainer")},
    m_overlay         {formWindow->get<tgui::Group>("Overlay")},
    m_filename        {filename}
{
    m_widgets["form"] = nullptr;

    m_formWindow->setTitle(filename);
    m_formWindow->onClose([this]{ m_guiBuilder->closeForm(this); });
    m_formWindow->onSizeChange([this]{ m_scrollablePanel->setSize(m_formWindow->getClientSize()); });

    auto eventHandler = tgui::ClickableWidget::create();
    eventHandler->onMousePress([this](tgui::Vector2f pos){ onFormMousePress(pos); });
    m_scrollablePanel->add(eventHandler, "EventHandler");

    setSize(formSize);

    tgui::Theme selectionSquareTheme{"resources/SelectionSquare.txt"};
    for (auto& square : m_selectionSquares)
    {
        square = tgui::Button::create();
        square->setRenderer(selectionSquareTheme.getRenderer("Square"));
        square->setSize(tgui::Vector2f{square->getRenderer()->getTexture().getImageSize()});
        square->setVisible(false);
        square->onMousePress([this,&square](tgui::Vector2f pos){ onSelectionSquarePress(square, pos); });
        m_scrollablePanel->add(square);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::String Form::addWidget(const tgui::Widget::Ptr& widget, tgui::Container* parent, bool selectNewWidget)
{
    const tgui::String id = tgui::String::fromNumber(widget.get());
    m_widgets[id] = std::make_shared<WidgetInfo>(widget);

    const tgui::String widgetType = widget->getWidgetType();
    bool foundAvailableName = false;
    unsigned int count = 0;
    tgui::String name = widget->getWidgetName();
    if (name.empty())
        name = widgetType + tgui::String::fromNumber(++count);
    while (!foundAvailableName)
    {
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

        if (!foundAvailableName)
            name = widgetType + tgui::String::fromNumber(++count);
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

void Form::removeWidget(const tgui::String& id)
{
    const auto widget = m_widgets[id];
    assert(widget != nullptr);

    // Remove the child widgets
    if (widget->ptr->isContainer())
    {
        std::vector<tgui::String> childIds;
        std::stack<tgui::Container::Ptr> parentsToSearch;
        parentsToSearch.push(widget->ptr->cast<tgui::Container>());
        while (!parentsToSearch.empty())
        {
            tgui::Container::Ptr parent = parentsToSearch.top();
            parentsToSearch.pop();
            for (const auto& childWidget : parent->getWidgets())
            {
                childIds.push_back(tgui::String::fromNumber(childWidget.get()));
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

std::shared_ptr<WidgetInfo> Form::getWidget(const tgui::String& id) const
{
    assert(m_widgets.find(id) != m_widgets.end());
    return m_widgets.at(id);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<WidgetInfo> Form::getWidgetByName(const tgui::String& name) const
{
    if (name == m_filename)
        return {};

    const auto it = std::find_if(
        m_widgets.begin(),
        m_widgets.end(),
        [&name](const auto& idAndWidgetInfo)
        {
            const auto& widgetInfo = idAndWidgetInfo.second;
            if (widgetInfo)
                return idAndWidgetInfo.second->name == name;
            else
                return false;
        }
    );

    if (it != m_widgets.end())
        return it->second;
    else
        return {};
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

bool Form::setSelectedWidgetName(const tgui::String& name)
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

void Form::selectWidgetById(const tgui::String& id)
{
    selectWidget(m_widgets[id]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectWidgetByName(const tgui::String& name)
{
    selectWidget(getWidgetByName(name));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::selectParent()
{
    if (!m_selectedWidget)
        return;

    // If we were dragging a widget then stop the drag
    m_draggingWidget = false;
    m_draggingSelectionSquare = nullptr;

    // If the widget was added directly to the form then select the form
    if (m_selectedWidget->ptr->getParent() == m_widgetsContainer.get())
    {
        selectWidget(nullptr);
        return;
    }

    selectWidget(m_widgets[tgui::String::fromNumber(m_selectedWidget->ptr->getParent())]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::mouseMoved(tgui::Vector2i pos)
{
    if (m_draggingWidget || m_draggingSelectionSquare)
        onDrag(pos);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::mouseReleased()
{
    m_draggingWidget = false;
    m_draggingSelectionSquare = nullptr;
    m_onDragSaved = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Form::rightMouseClick(tgui::Vector2i pos)
{
    tgui::Vector2f relativeWindowPos{(pos.x - m_formWindow->getAbsolutePosition().x), (pos.y - m_formWindow->getAbsolutePosition().y)};
    if (!tgui::FloatRect{m_formWindow->getChildWidgetsOffset(), m_formWindow->getClientSize()}.contains(relativeWindowPos))
        return false;

    const tgui::Vector2f relativePanelPos{
        (pos.x - m_scrollablePanel->get("EventHandler")->getAbsolutePosition().x),
        (pos.y - m_scrollablePanel->get("EventHandler")->getAbsolutePosition().y)};

    onFormMousePress(relativePanelPos);
    mouseReleased();
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::arrowKeyPressed(const tgui::Event::KeyEvent& keyEvent)
{
    if (!m_selectedWidget)
        return;

    auto selectedWidget = m_selectedWidget->ptr;

#ifdef TGUI_SYSTEM_MACOS
    const auto controlPressed = keyEvent.system;
#else
    const auto controlPressed = keyEvent.control;
#endif

    if (keyEvent.shift && controlPressed)
    {
        if (keyEvent.code == tgui::Event::KeyboardKey::Left)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPosition().x - MOVE_STEP, selectedWidget->getPositionLayout().y});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Right)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPosition().x + MOVE_STEP, selectedWidget->getPositionLayout().y});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Up)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y - MOVE_STEP});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Down)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y + MOVE_STEP});
        }

        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
    else if (keyEvent.shift)
    {
        if (keyEvent.code == tgui::Event::KeyboardKey::Left)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Resize);
            selectedWidget->setSize({selectedWidget->getSize().x - 1, selectedWidget->getSizeLayout().y});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Right)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setSize({selectedWidget->getSize().x + 1, selectedWidget->getSizeLayout().y});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Up)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y - 1});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Down)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setSize({selectedWidget->getSizeLayout().x, selectedWidget->getSize().y + 1});
        }

        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
    else if (controlPressed)
    {
        if (keyEvent.code == tgui::Event::KeyboardKey::Left)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPosition().x - 1, selectedWidget->getPositionLayout().y});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Right)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPosition().x + 1, selectedWidget->getPositionLayout().y});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Up)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y - 1});
        }
        else if (keyEvent.code == tgui::Event::KeyboardKey::Down)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            selectedWidget->setPosition({selectedWidget->getPositionLayout().x, selectedWidget->getPosition().y + 1});
        }

        setChanged(true);
        updateSelectionSquarePositions();
        m_guiBuilder->reloadProperties();
    }
    else
    {
        tgui::Vector2f selectedWidgetPoint;
        if (keyEvent.code == tgui::Event::KeyboardKey::Left)
            selectedWidgetPoint = {selectedWidget->getPosition().x, selectedWidget->getPosition().y + (selectedWidget->getSize().y / 2.f)};
        else if (keyEvent.code == tgui::Event::KeyboardKey::Right)
            selectedWidgetPoint = {selectedWidget->getPosition().x + selectedWidget->getSize().x, selectedWidget->getPosition().y + (selectedWidget->getSize().y / 2.f)};
        else if (keyEvent.code == tgui::Event::KeyboardKey::Up)
            selectedWidgetPoint = {selectedWidget->getPosition().x + (selectedWidget->getSize().x / 2.f), selectedWidget->getPosition().y};
        else if (keyEvent.code == tgui::Event::KeyboardKey::Down)
            selectedWidgetPoint = {selectedWidget->getPosition().x + (selectedWidget->getSize().x / 2.f), selectedWidget->getPosition().y + selectedWidget->getSize().y};

        float closestDistance = std::numeric_limits<float>::infinity();
        tgui::Widget::Ptr closestWidget = nullptr;
        const auto widgets = selectedWidget->getParent()->getWidgets();
        for (const auto& widget : widgets)
        {
            if (widget == selectedWidget)
                continue;

            tgui::Vector2f widgetPoint;
            if (keyEvent.code == tgui::Event::KeyboardKey::Left)
                widgetPoint = {widget->getPosition().x + widget->getSize().x, widget->getPosition().y + (widget->getSize().y / 2.f)};
            else if (keyEvent.code == tgui::Event::KeyboardKey::Right)
                widgetPoint = {widget->getPosition().x, widget->getPosition().y + (widget->getSize().y / 2.f)};
            else if (keyEvent.code == tgui::Event::KeyboardKey::Up)
                widgetPoint = {widget->getPosition().x + (widget->getSize().x / 2.f), widget->getPosition().y + widget->getSize().y};
            else if (keyEvent.code == tgui::Event::KeyboardKey::Down)
                widgetPoint = {widget->getPosition().x + (widget->getSize().x / 2.f), widget->getPosition().y};

            // Don't allow going in the opposite direction when there are no widgets on the chosen side
            if (((keyEvent.code == tgui::Event::KeyboardKey::Left) && (widgetPoint.x >= selectedWidgetPoint.x))
             || ((keyEvent.code == tgui::Event::KeyboardKey::Right) && (widgetPoint.x <= selectedWidgetPoint.x))
             || ((keyEvent.code == tgui::Event::KeyboardKey::Up) && (widgetPoint.y >= selectedWidgetPoint.y))
             || ((keyEvent.code == tgui::Event::KeyboardKey::Down) && (widgetPoint.y <= selectedWidgetPoint.y)))
                continue;

            const float distance = std::abs(widgetPoint.x - selectedWidgetPoint.x) + std::abs(widgetPoint.y - selectedWidgetPoint.y);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestWidget = widget;
            }
        }

        if (closestWidget)
            selectWidget(m_widgets[tgui::String::fromNumber(closestWidget.get())]);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::setFilename(const tgui::String& filename)
{
    m_filename = filename;
    setChanged(m_changed);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::String Form::getFilename() const
{
    return m_filename;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::setSize(tgui::Vector2f size)
{
    m_size = size;

    m_widgetsContainer->setSize(m_size);
    m_scrollablePanel->setContentSize(m_size);
    m_scrollablePanel->get("EventHandler")->setSize(m_size);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::Vector2f Form::getSize() const
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

void Form::load()
{
    // We will make some adjustments to the file before letting TGUI load it, so start by loading the file into memory.
    const tgui::String filename = (tgui::getResourcePath() / getFilename()).asString();
    std::size_t fileSize;
    auto fileContents = readFileToMemory(filename, fileSize);
    if (!fileContents)
        throw tgui::Exception("Failed to open '" + filename + "'.");

    /// TODO: Optimize this (DataIO::parse function should be able to use a string view directly on file contents)
    std::stringstream inStream{std::string{reinterpret_cast<const char*>(fileContents.get()), fileSize}};

    // Parse the file from memory
    auto rootNode = tgui::DataIO::parse(inStream);

    // If the file was created with TGUI 0.8 then convert it into a valid TGUI 0.9/0.10 form
    importOldForm(rootNode);

    // Turn all paths into absolute paths, using either the form file or the gui builder as base path
    makePathsAbsolute(rootNode, (tgui::getResourcePath() / getFilename()).getParentPath());

    // Recreate the changed file in memory
    std::stringstream outStream;
    tgui::DataIO::emit(rootNode, outStream);

    // Try to load the modified file from memory
    m_widgetsContainer->loadWidgetsFromStream(outStream);

    importLoadedWidgets(m_widgetsContainer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::save()
{
    setChanged(false);

    // Save the output file to memory so that it can be edited
    std::stringstream originalOutStream;
    m_widgetsContainer->saveWidgetsToStream(originalOutStream);
    auto rootNode = tgui::DataIO::parse(originalOutStream);

    tgui::String formPath = (tgui::getResourcePath() / getFilename()).getParentPath().asString();
    if (formPath.back() != '/')
        formPath.push_back('/');

    tgui::String guiBuilderPath = (tgui::getResourcePath()).getParentPath().asString();
    if (guiBuilderPath.back() != '/')
        guiBuilderPath.push_back('/');

    // Turn all paths into relative paths
    makePathsRelative(rootNode, formPath, guiBuilderPath);

    // Recreate the changed file in memory
    std::stringstream outStream;
    tgui::DataIO::emit(rootNode, outStream);

    // Write the file to disk
    const tgui::String filename = (tgui::getResourcePath() / getFilename()).asString();
    if (!tgui::writeFile(filename, outStream))
        throw tgui::Exception("Failed to write to '" + filename + "'.");

    m_guiBuilder->formSaved(getFilename());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Saves state to internal storage
std::stringstream Form::saveState()
{
    std::stringstream outStream;
    m_widgetsContainer->saveWidgetsToStream(outStream);
    return outStream;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Loads state from internal storage
void Form::loadState(std::stringstream& state)
{
    m_widgetsContainer->loadWidgetsFromStream(state);
    importLoadedWidgets(m_widgetsContainer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::updateAlignmentLines()
{
    static std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> lastLines;

    std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> lines = getAlignmentLines();
    if (lines == lastLines)
        return;

    // Update the position and sizes of new lines
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        if (i >= m_alignmentLines.size())
        {
            // If the line didn't exist yet then add it now
            m_alignmentLines.emplace_back(tgui::SeparatorLine::create());
            m_alignmentLines.back()->getRenderer()->setColor({0, 0, 139});
            m_overlay->add(m_alignmentLines.back());
        }

        const auto& startPoint = lines[i].first;
        const auto& endPoint = lines[i].second;
        m_alignmentLines[i]->setPosition({startPoint.x, startPoint.y});
        m_alignmentLines[i]->setSize({std::max(endPoint.x - startPoint.x, 1.f), std::max(endPoint.y - startPoint.y, 1.f)});
    }

    // Remove all lines that are no longer needed
    for (std::size_t i = m_alignmentLines.size(); i > lines.size(); --i)
    {
        m_overlay->remove(m_alignmentLines[i-1]);
        m_alignmentLines.pop_back();
    }

    lastLines = std::move(lines);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::importLoadedWidgets(const tgui::Container::Ptr& parent)
{
    const auto& widgets = parent->getWidgets();
    for (const auto& widget : widgets)
    {
        const tgui::String id = tgui::String::fromNumber(widget.get());
        m_widgets[id] = std::make_shared<WidgetInfo>(widget);
        m_widgets[id]->name = widget->getWidgetName();
        m_widgets[id]->theme = "Custom";

        if (widget->isContainer())
            importLoadedWidgets(std::static_pointer_cast<tgui::Container>(widget));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::onSelectionSquarePress(const tgui::Button::Ptr& square, tgui::Vector2f pos)
{
    m_draggingSelectionSquare = square;
    m_draggingPos = square->getPosition() + pos;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::Widget::Ptr Form::getWidgetBelowMouse(const tgui::Container::Ptr& parent, tgui::Vector2f pos)
{
    // Loop through widgets in reverse order to find the top one in case of overlapping widgets
    const auto& widgets = parent->getWidgets();
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it)
    {
        tgui::Widget::Ptr widget = *it;
        if (widget && tgui::FloatRect{widget->getPosition().x, widget->getPosition().y, widget->getFullSize().x, widget->getFullSize().y}.contains(pos))
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

void Form::onFormMousePress(tgui::Vector2f pos)
{
    auto widget = getWidgetBelowMouse(m_widgetsContainer, pos);
    if (widget)
    {
        selectWidget(m_widgets[tgui::String::fromNumber(widget.get())]);
        m_draggingWidget = true;
        m_draggingPos = pos;
    }
    else
        selectWidget(nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::onDrag(tgui::Vector2i mousePos)
{
    assert(m_selectedWidget != nullptr);

    const tgui::Vector2f pos = tgui::Vector2f{mousePos} - m_formWindow->getPosition() - m_formWindow->getChildWidgetsOffset() + m_scrollablePanel->getContentOffset();
    auto selectedWidget = m_selectedWidget->ptr;

    bool updated = false;

    if (m_draggingWidget)
    {
        // Saves state for undo recall
        if (!m_onDragSaved)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Move);
            m_onDragSaved = true;
        }

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
        if (!m_onDragSaved)
        {
            m_guiBuilder->saveUndoState(GuiBuilder::UndoType::Resize);
            m_onDragSaved = true;
        }

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

            tgui::Vector2f change;
            if (ratio <= 1)
                change = tgui::Vector2f(MOVE_STEP, MOVE_STEP * ratio);
            else
                change = tgui::Vector2f(MOVE_STEP / ratio, MOVE_STEP);

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

void Form::selectWidget(const std::shared_ptr<WidgetInfo>& widget)
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

std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> Form::getAlignmentLines() const
{
    std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> lines;

    if (!m_selectedWidget)
        return lines;

#ifdef TGUI_SYSTEM_MACOS
    const auto controlModifier = tgui::Event::KeyModifier::System;
#else
    const auto controlModifier = tgui::Event::KeyModifier::Control;
#endif

    if (!m_draggingWidget && !m_draggingSelectionSquare
     && !tgui::getBackend()->isKeyboardModifierPressed(controlModifier)
     && !tgui::getBackend()->isKeyboardModifierPressed(tgui::Event::KeyModifier::Shift))
        return lines;

    const auto selectedWidget = m_selectedWidget->ptr;
    const tgui::Vector2f selectedTopLeft = selectedWidget->getAbsolutePosition();
    const tgui::Vector2f selectedBottomRight = selectedWidget->getAbsolutePosition() + selectedWidget->getSize();
    const auto widgets = selectedWidget->getParent()->getWidgets();
    for (const auto& widget : widgets)
    {
        if (widget == selectedWidget)
            continue;

        const tgui::Vector2f topLeft = widget->getAbsolutePosition();
        const tgui::Vector2f bottomRight = widget->getAbsolutePosition() + widget->getSize();

        const float minX = std::min({selectedTopLeft.x, selectedBottomRight.x, topLeft.x, bottomRight.x});
        const float maxX = std::max({selectedTopLeft.x, selectedBottomRight.x, topLeft.x, bottomRight.x});
        const float minY = std::min({selectedTopLeft.y, selectedBottomRight.y, topLeft.y, bottomRight.y});
        const float maxY = std::max({selectedTopLeft.y, selectedBottomRight.y, topLeft.y, bottomRight.y});

        if ((topLeft.x == selectedTopLeft.x) || (topLeft.x == selectedBottomRight.x))
            lines.emplace_back(tgui::Vector2f{topLeft.x, minY}, tgui::Vector2f{topLeft.x, maxY});
        if ((topLeft.y == selectedTopLeft.y) || (topLeft.y == selectedBottomRight.y))
            lines.emplace_back(tgui::Vector2f{minX, topLeft.y}, tgui::Vector2f{maxX, topLeft.y});
        if ((bottomRight.x == selectedBottomRight.x) || (bottomRight.x == selectedTopLeft.x))
            lines.emplace_back(tgui::Vector2f{bottomRight.x, minY}, tgui::Vector2f{bottomRight.x, maxY});
        if ((bottomRight.y == selectedBottomRight.y) || (bottomRight.y == selectedTopLeft.y))
            lines.emplace_back(tgui::Vector2f{minX, bottomRight.y}, tgui::Vector2f{maxX, bottomRight.y});
    }

    // Because we used absolute positions to compare widgets inside and outside groups, the coordinates of the lines
    // still need to be adjusted to start relative to the form.
    for (auto& line : lines)
    {
        line.first -= m_overlay->getAbsolutePosition();
        line.second -= m_overlay->getAbsolutePosition();
    }

    return lines;
}
