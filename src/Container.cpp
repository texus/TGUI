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


#include <TGUI/Container.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/GuiBase.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/SubwidgetContainer.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/Filesystem.hpp>

#include <fstream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void getAllRenderers(std::vector<RendererData*>& orderedRenderers, std::map<RendererData*, std::vector<const Widget*>>& rendererToWidgetsMap, const Container* container)
        {
            const auto addRenderer = [&](RendererData* rendererData, const Widget* widget){
                auto it = rendererToWidgetsMap.find(rendererData);
                if (it != rendererToWidgetsMap.end())
                    it->second.push_back(widget);
                else
                {
                    // Add the renderer to the orderedRenderers list when it occurs the first time.
                    // This allows renderers to be saved in order of appearance instead of in a random order.
                    rendererToWidgetsMap[rendererData].push_back(widget);
                    orderedRenderers.push_back(rendererData);
                }
            };

            for (const auto& child : container->getWidgets())
            {
                addRenderer(child->getSharedRenderer()->getData().get(), child.get());

                if (child->getToolTip())
                    addRenderer(child->getToolTip()->getSharedRenderer()->getData().get(), child->getToolTip().get());

                Container* childContainer = dynamic_cast<Container*>(child.get());
                if (childContainer)
                    getAllRenderers(orderedRenderers, rendererToWidgetsMap, childContainer);
                else
                {
                    SubwidgetContainer* subWidgetContainer = dynamic_cast<SubwidgetContainer*>(child.get());
                    if (subWidgetContainer)
                    {
                        addRenderer(subWidgetContainer->getContainer()->getSharedRenderer()->getData().get(), subWidgetContainer->getContainer());
                        getAllRenderers(orderedRenderers, rendererToWidgetsMap, subWidgetContainer->getContainer());
                    }
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveRenderer(RendererData* renderer, const String& name)
        {
            auto node = std::make_unique<DataIO::Node>();
            node->name = name;
            for (const auto& pair : renderer->propertyValuePairs)
            {
                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{ObjectConverter{pair.second}.getString().toStdString()};
                    auto rendererRootNode = DataIO::parse(ss);

                    // If there are braces around the renderer string, then the child node is the one we need
                    if (rendererRootNode->propertyValuePairs.empty() && (rendererRootNode->children.size() == 1))
                        rendererRootNode = std::move(rendererRootNode->children[0]);

                    rendererRootNode->name = pair.first;
                    node->children.push_back(std::move(rendererRootNode));
                }
                else
                {
                    String value = ObjectConverter{pair.second}.getString();

                    // Skip empty values
                    if (value.empty())
                        continue;

                    // Skip "Font = null"
                    if (pair.first == "Font" && value == "null")
                        continue;

                    node->propertyValuePairs[pair.first] = std::make_unique<DataIO::ValueNode>(value);
                }
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void makePathsRelativeToForm(const std::unique_ptr<DataIO::Node>& node, const String& formPath)
        {
            for (const auto& pair : node->propertyValuePairs)
            {
                if (!pair.first.startsWith(U"Texture") && (pair.first != U"Font") && (pair.first != U"Image"))
                    continue;

                if (pair.second->value.empty() || pair.second->value.equalIgnoreCase(U"none") || pair.second->value.equalIgnoreCase(U"null") || pair.second->value.equalIgnoreCase(U"nullptr"))
                    continue;

                String filename;
                if (pair.second->value[0] != '"')
                    filename = pair.second->value;
                else
                {
                    // The filename is surrounded by quotes, with optional options behind it
                    const auto endQuotePos = pair.second->value.find('"', 1);
                    assert(endQuotePos != String::npos); // DataIO wouldn't have accepted the file if there is no close quote
                    filename = pair.second->value.substr(1, endQuotePos - 1);
                }

                // Make the path relative to the form file
                if (filename.startsWith(formPath))
                {
                    if (pair.second->value[0] != '"')
                        pair.second->value.erase(0, formPath.length());
                    else
                        pair.second->value.erase(1, formPath.length());
                }
            }

            for (const auto& childNode : node->children)
                makePathsRelativeToForm(childNode, formPath);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(const char* typeName, bool initRenderer) :
        Widget{typeName, initRenderer}
    {
        m_containerWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(const Container& other) :
        Widget{other}
    {
        // Widgets with layouts that refer to each other need to be added simultaneously.
        // They all need to be in m_widgets before setParent is called on the first widget,
        // which is why we can't just use call add(widget) for each widget.
        m_widgets.reserve(other.m_widgets.size());
        for (std::size_t i = 0; i < other.m_widgets.size(); ++i)
            m_widgets.push_back(other.m_widgets[i]->clone());

        for (std::size_t i = 0; i < other.m_widgets.size(); ++i)
            widgetAdded(m_widgets[i]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(Container&& other) noexcept :
        Widget                    {std::move(other)},
        m_widgets                 {std::move(other.m_widgets)},
        m_widgetBelowMouse        {std::move(other.m_widgetBelowMouse)},
        m_widgetWithLeftMouseDown {std::move(other.m_widgetWithLeftMouseDown)},
        m_widgetWithRightMouseDown{std::move(other.m_widgetWithRightMouseDown)},
        m_focusedWidget           {std::move(other.m_focusedWidget)}
    {
        // Parent of all widgets should be set to nullptr first, in case widgets have layouts depending on each other.
        // Otherwise calling setParent on one widget could cause another widget's position to be recalculated which could
        // give a warning if it still has its old parent where it won't find any siblings.
        for (auto& widget : m_widgets)
            widget->setParent(nullptr);

        for (auto& widget : m_widgets)
            widget->setParent(this);

        other.m_widgets = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::~Container()
    {
        for (const auto& widget : m_widgets)
        {
            if (widget->getParent() == this)
                widget->setParent(nullptr);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Container::operator= (const Container& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Widget::operator=(right);

            m_widgetBelowMouse = nullptr;
            m_widgetWithLeftMouseDown = nullptr;
            m_widgetWithRightMouseDown = nullptr;
            m_focusedWidget = nullptr;

            // Remove all the old widgets
            Container::removeAllWidgets();

            // Widgets with layouts that refer to each other need to be added simultaneously.
            // They all need to be in m_widgets before setParent is called on the first widget,
            // which is why we can't just use call add(widget) for each widget.
            m_widgets.reserve(right.m_widgets.size());
            for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
                m_widgets.push_back(right.m_widgets[i]->clone());

            for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
                widgetAdded(m_widgets[i]);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Container::operator= (Container&& right) noexcept
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Widget::operator=(std::move(right));
            m_widgets                  = std::move(right.m_widgets);
            m_widgetBelowMouse         = std::move(right.m_widgetBelowMouse);
            m_widgetWithLeftMouseDown  = std::move(right.m_widgetWithLeftMouseDown);
            m_widgetWithRightMouseDown = std::move(right.m_widgetWithRightMouseDown);
            m_focusedWidget            = std::move(right.m_focusedWidget);

            // Parent of all widgets should be set to nullptr first, in case widgets have layouts depending on each other.
            // Otherwise calling setParent on one widget could cause another widget's position to be recalculated which could
            // give a warning if it still has its old parent where it won't find any siblings.
            for (auto& widget : m_widgets)
                widget->setParent(nullptr);

            for (auto& widget : m_widgets)
                widget->setParent(this);

            right.m_widgets = {};
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setSize(const Layout2d& size)
    {
        if (size.getValue() != m_prevSize)
        {
            Widget::setSize(size);
            m_prevInnerSize = getInnerSize();
        }
        else // Size didn't change, but also check the inner size in case the borders or padding changed
        {
            Widget::setSize(size);
            if (getInnerSize() != m_prevInnerSize)
            {
                m_prevInnerSize = getInnerSize();
                for (auto& layout : m_boundSizeLayouts)
                    layout->recalculateValue();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::add(const Widget::Ptr& widgetPtr, const String& widgetName)
    {
        TGUI_ASSERT(widgetPtr != nullptr, "Can't add nullptr to container");

        m_widgets.push_back(widgetPtr);
        if (!widgetName.empty())
            widgetPtr->setWidgetName(widgetName);

        widgetAdded(widgetPtr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::get(const String& widgetName) const
    {
        // First search for direct children
        for (const auto& child : m_widgets)
        {
            if (child->getWidgetName() == widgetName)
                return child;
        }

        // If no widget was found then search recursively
        for (const auto& child : m_widgets)
        {
            if (child->isContainer())
            {
                Widget::Ptr widget = std::static_pointer_cast<Container>(child)->get(widgetName);
                if (widget != nullptr)
                    return widget;
            }
        }

        // If we still couldn't find it then check if there are any SubwidgetContainer widgets and search their subwidgets
        for (const auto& child : m_widgets)
        {
            auto subWidgetContainer = dynamic_cast<const SubwidgetContainer*>(child.get());
            if (subWidgetContainer)
            {
                Widget::Ptr widget = subWidgetContainer->getContainer()->get(widgetName);
                if (widget != nullptr)
                    return widget;
            }
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::remove(const Widget::Ptr& widget)
    {
        // Loop through every widget
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] != widget)
                continue;

            if (widget == m_widgetBelowMouse && m_parentGui && (widget->getMouseCursor() != m_mouseCursor))
                m_parentGui->requestMouseCursor(m_mouseCursor);

            if (m_widgetBelowMouse == widget)
                m_widgetBelowMouse = nullptr;
            if (m_widgetWithLeftMouseDown == widget)
                m_widgetWithLeftMouseDown = nullptr;
            if (m_widgetWithRightMouseDown == widget)
                m_widgetWithRightMouseDown = nullptr;

            if (widget == m_focusedWidget)
            {
                m_focusedWidget = nullptr;
                widget->setFocused(false);
            }

            // Remove the widget
            widget->setParent(nullptr);
            m_widgets.erase(m_widgets.begin() + i);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::removeAllWidgets()
    {
        if (m_mouseHover && m_parentGui && (m_mouseCursor != Cursor::Type::Arrow))
            m_parentGui->requestMouseCursor(m_mouseCursor);

        for (const auto& widget : m_widgets)
            widget->setParent(nullptr);

        m_widgets.clear();

        m_widgetBelowMouse = nullptr;
        m_widgetWithLeftMouseDown = nullptr;
        m_widgetWithRightMouseDown = nullptr;
        m_focusedWidget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Container::getInnerSize() const
    {
        return getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setTextSize(unsigned int size)
    {
        Widget::setTextSize(size);

        if (size != 0)
        {
            for (const auto& widget : m_widgets)
                widget->setTextSize(size);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromFile(const String& filename, bool replaceExisting)
    {
        auto oldTheme = tgui::Theme::getDefault();
        tgui::Theme::setDefault(nullptr);

        // If a resource path is set then place it in front of the filename (unless the filename is an absolute path)
        String filenameInResources = filename;
        if (!getResourcePath().isEmpty())
            filenameInResources = (getResourcePath() / filename).asString();

        std::size_t fileSize;
        auto fileContents = readFileToMemory(filenameInResources, fileSize);
        if (!fileContents)
            throw Exception{"Failed to open '" + filenameInResources + "' to load the widgets from it."};

        /// TODO: Optimize this (parse function should be able to use a string view directly on file contents)
        std::stringstream stream{std::string{reinterpret_cast<const char*>(fileContents.get()), fileSize}};
        const auto rootNode = DataIO::parse(stream);

        // All files need to be loaded relative to the form file
        const auto& parentPath = Filesystem::Path(filename).getParentPath();
        if (!parentPath.isEmpty())
        {
            std::map<String, bool> checkedFilenames;
            injectFormFilePath(rootNode, parentPath.asString(), checkedFilenames);
        }

        loadWidgetsFromNodeTree(rootNode, replaceExisting);

        tgui::Theme::setDefault(oldTheme);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToFile(const String& filename)
    {
        // If a resource path is set then place it in front of the filename (unless the filename is an absolute path)
        String filenameInResources = filename;
        if (!getResourcePath().isEmpty())
            filenameInResources = (getResourcePath() / filename).asString();

        const String formFileDir = Filesystem::Path(filename).getParentPath().asString();

        std::stringstream stream;
        saveWidgetsToStream(stream, formFileDir);

        if (!writeFile(filenameInResources, stream))
            throw Exception{"Failed to write '" + filenameInResources + "' while trying to save widgets in it."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream& stream, bool replaceExisting)
    {
        const auto rootNode = DataIO::parse(stream);
        loadWidgetsFromNodeTree(rootNode, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream&& stream, bool replaceExisting)
    {
        loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToStream(std::stringstream& stream, const String& rootDirectory) const
    {
        auto rootNode = saveWidgetsToNodeTree(rootDirectory);
        DataIO::emit(rootNode, stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromNodeTree(const std::unique_ptr<DataIO::Node>& rootNode, bool replaceExisting)
    {
        // Replace the existing widgets by the ones that will be loaded if requested
        if (replaceExisting)
            removeAllWidgets();

        if (rootNode->propertyValuePairs.size() != 0)
            Widget::load(rootNode, {});

        std::vector<std::pair<Widget::Ptr, std::reference_wrapper<const std::unique_ptr<DataIO::Node>>>> widgetsToLoad;
        std::map<String, std::shared_ptr<RendererData>> availableRenderers;
        for (const auto& node : rootNode->children)
        {
            auto nameSeparator = node->name.find('.');
            auto widgetType = node->name.substr(0, nameSeparator);

            String objectName;
            if (nameSeparator != String::npos)
                objectName = Deserializer::deserialize(ObjectConverter::Type::String, node->name.substr(nameSeparator + 1)).getString();

            if (widgetType == "Renderer")
            {
                if (!objectName.empty())
                    availableRenderers[objectName] = RendererData::createFromDataIONode(node.get());
            }
            else // Section describes a widget
            {
                const auto& constructor = WidgetFactory::getConstructFunction(widgetType);
                if (constructor)
                {
                    Widget::Ptr widget = constructor();
                    add(widget, objectName);

                    // We delay loading of widgets until they have all been added to the container.
                    // Otherwise there would be issues if their position and size layouts refer to
                    // widgets that have not yet been loaded.
                    widgetsToLoad.push_back(std::make_pair(widget, std::cref(node)));
                }
                else
                    throw Exception{"No construct function exists for widget type '" + widgetType + "'."};
            }
        }

        for (auto& pair : widgetsToLoad)
        {
            Widget::Ptr& widget = pair.first;
            const auto& node = pair.second.get();
            widget->load(node, availableRenderers);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Container::saveWidgetsToNodeTree(const String& rootDirectory) const
    {
        auto rootNode = std::make_unique<DataIO::Node>();

        std::vector<RendererData*> orderedRenderers;
        std::map<RendererData*, std::vector<const Widget*>> rendererToWidgetsMap;
        getAllRenderers(orderedRenderers, rendererToWidgetsMap, this);

        unsigned int id = 0;
        SavingRenderersMap renderersMap;
        for (const auto& renderer : orderedRenderers)
        {
            assert(rendererToWidgetsMap.find(renderer) != rendererToWidgetsMap.end());
            const auto& widgetsUsingRenderer = rendererToWidgetsMap[renderer];

            // The renderer can remain inside the widget if it is not shared, so provide the node to be included inside the widget
            if (widgetsUsingRenderer.size() == 1)
            {
                renderersMap[widgetsUsingRenderer[0]] = {saveRenderer(renderer, "Renderer"), ""};
                continue;
            }

            // When the widget is shared, only provide the id instead of the node itself
            ++id;
            const String idStr = String::fromNumber(id);
            rootNode->children.push_back(saveRenderer(renderer, "Renderer." + idStr));
            for (const auto& child : widgetsUsingRenderer)
                renderersMap[child] = std::make_pair(nullptr, idStr); // Did not compile with VS2015 Update 2 when using braces
        }

        for (const auto& child : getWidgets())
            rootNode->children.emplace_back(child->save(renderersMap));

        if (!rootDirectory.empty())
        {
            if ((rootDirectory.back() != '/') && (rootDirectory.back() != '\\'))
                makePathsRelativeToForm(rootNode, rootDirectory + U'/');
            else
                makePathsRelativeToForm(rootNode, rootDirectory);
        }

        return rootNode;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToFront(const Widget::Ptr& widget)
    {
        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] != widget)
                continue;

            // Copy the widget
            m_widgets.push_back(m_widgets[i]);

            // Remove the old widget
            m_widgets.erase(m_widgets.begin() + i);
            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToBack(const Widget::Ptr& widget)
    {
        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] != widget)
                continue;

            // Copy the widget
            const Widget::Ptr obj = m_widgets[i];
            m_widgets.insert(m_widgets.begin(), obj);

            // Remove the old widget
            m_widgets.erase(m_widgets.begin() + i + 1);
            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Container::moveWidgetForward(const Widget::Ptr& widget)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] != widget)
                continue;

            // If the widget is already at the front then we can't move it further forward
            if (i == m_widgets.size() - 1)
                return m_widgets.size() - 1;

            std::swap(m_widgets[i], m_widgets[i+1]);
            return i + 1;
        }

        // The widget wasn't found in this container
        return m_widgets.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Container::moveWidgetBackward(const Widget::Ptr& widget)
    {
        for (std::size_t i = m_widgets.size(); i > 0; --i)
        {
            if (m_widgets[i-1] != widget)
                continue;

            // If the widget is already at the back then we can't move it further backward
            if (i-1 == 0)
                return 0;

            std::swap(m_widgets[i-2], m_widgets[i-1]);
            return i-2;
        }

        // The widget wasn't found in this container
        return m_widgets.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::setWidgetIndex(const Widget::Ptr& widget, std::size_t index)
    {
        if (index >= m_widgets.size())
            return false;

        std::size_t currentWidgetIndex = m_widgets.size();
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
            {
                currentWidgetIndex = i;
                break;
            }
        }

        if (currentWidgetIndex == m_widgets.size())
            return false;

        if (index == currentWidgetIndex)
            return true;

        // Move the widget to the new index
        m_widgets.erase(m_widgets.begin() + currentWidgetIndex);
        m_widgets.insert(m_widgets.begin() + index, widget);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Container::getWidgetIndex(const Widget::Ptr& widget) const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::getFocusedChild() const
    {
        return m_focusedWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::getFocusedLeaf() const
    {
        if (!m_focusedWidget || !m_focusedWidget->isContainer())
            return m_focusedWidget;

        const auto leafWidget = std::static_pointer_cast<Container>(m_focusedWidget)->getFocusedLeaf();

        // If the container has no focused child then the container itself is the leaf
        if (!leafWidget)
            return m_focusedWidget;

        return leafWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::getWidgetAtPosition(Vector2f pos) const
    {
        pos -= getPosition() + getChildWidgetsOffset();

        for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it)
        {
            auto& widget = *it;

            // Look for a visible widget below the mouse
            if (!widget->isVisible())
                continue;
            if (!widget->isMouseOnWidget(transformMousePos(widget, pos)))
                continue;

            // If the widget is a container then look inside it
            if (widget->isContainer())
            {
                Container::Ptr container = std::static_pointer_cast<Container>(widget);
                auto childWidget = container->getWidgetAtPosition(transformMousePos(widget, pos));
                if (childWidget)
                    return childWidget;
            }

            // If the widget isn't a container, or there were no child widgets inside it, then return this widget
            return widget;
        }

        // No visible widgets were found at the queried position
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusNextWidget(bool recursive)
    {
        // If the focused widget is a container then try to focus the next widget inside it
        if (recursive && m_focusedWidget && m_focusedWidget->isContainer())
        {
            auto focusedContainer = std::static_pointer_cast<Container>(m_focusedWidget);
            if (focusedContainer->focusNextWidget(true))
                return true;
        }

        // Loop all widgets behind the focused one
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();
        for (std::size_t i = focusedWidgetIndex; i < m_widgets.size(); ++i)
        {
            if (tryFocusWidget(m_widgets[i], false, recursive))
                return true;
        }

        // If we are not an isolated focus group then the focus will be given to the group behind us
        if (recursive && !m_isolatedFocus)
            return false;

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (!m_focusedWidget)
            return false;

        // Also include the focused widget since it may be a container that didn't have its first widget focused
        for (std::size_t i = 0; i < focusedWidgetIndex; ++i)
        {
            if (tryFocusWidget(m_widgets[i], false, recursive))
                return true;
        }

        // No other widget could be focused
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusPreviousWidget(bool recursive)
    {
        // If the focused widget is a container then try to focus the previous widget inside it
        if (recursive && m_focusedWidget && m_focusedWidget->isContainer())
        {
            auto focusedContainer = std::static_pointer_cast<Container>(m_focusedWidget);
            if (focusedContainer->focusPreviousWidget())
                return true;
        }

        // Loop all widgets before the focused one
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();
        if (focusedWidgetIndex > 0)
        {
            for (std::size_t i = focusedWidgetIndex - 1; i > 0; --i)
            {
                if (tryFocusWidget(m_widgets[i-1], true, recursive))
                    return true;
            }

            // If we are not an isolated focus group then the focus will be given to the group before us
            if (recursive && !m_isolatedFocus)
                return false;
        }

        // None of the widgets before the focused one could be focused, so loop the ones after it.
        for (std::size_t i = m_widgets.size(); i > focusedWidgetIndex; --i)
        {
            if (tryFocusWidget(m_widgets[i-1], true, recursive))
                return true;
        }

        // Also include the focused widget since it may be a container that didn't have its last widget focused.
        if (focusedWidgetIndex > 0)
        {
            if (tryFocusWidget(m_widgets[focusedWidgetIndex-1], true, recursive))
                return true;
        }

        // No other widget could be focused
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setFocused(bool focused)
    {
        if (m_focusedWidget && (focused != m_focusedWidget->isFocused()))
            m_focusedWidget->setFocused(focused);

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::childWidgetFocused(const Widget::Ptr& child)
    {
        if (m_focusedWidget != child)
        {
            if (m_focusedWidget)
                m_focusedWidget->setFocused(false);

            m_focusedWidget = child;
        }

        if (!isFocused())
            setFocused(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);
        processMousePressEvent(Event::MouseButton::Left, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rightMousePressed(Vector2f pos)
    {
        Widget::rightMousePressed(pos);
        processMousePressEvent(Event::MouseButton::Right, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMouseReleased(Vector2f pos)
    {
        Widget::leftMouseReleased(pos);
        processMouseReleaseEvent(Event::MouseButton::Left, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rightMouseReleased(Vector2f pos)
    {
        Widget::rightMouseReleased(pos);
        processMouseReleaseEvent(Event::MouseButton::Right, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);
        processMouseMoveEvent(pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::keyPressed(const Event::KeyEvent& event)
    {
        processKeyPressEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::textEntered(char32_t key)
    {
        processTextEnteredEvent(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::mouseWheelScrolled(float delta, Vector2f pos)
    {
        return processMouseWheelScrollEvent(delta, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseNoLongerOnWidget()
    {
        if (!m_mouseHover)
            return;

        mouseLeftWidget();

        if (m_widgetBelowMouse)
        {
            m_widgetBelowMouse->mouseNoLongerOnWidget();
            m_widgetBelowMouse = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();

        if (m_widgetWithLeftMouseDown)
        {
            m_widgetWithLeftMouseDown->leftMouseButtonNoLongerDown();
            m_widgetWithLeftMouseDown = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rightMouseButtonNoLongerDown()
    {
        Widget::rightMouseButtonNoLongerDown();

        if (m_widgetWithRightMouseDown)
        {
            m_widgetWithRightMouseDown->rightMouseButtonNoLongerDown();
            m_widgetWithRightMouseDown = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::askToolTip(Vector2f mousePos)
    {
        if (!isMouseOnWidget(mousePos))
            return nullptr;

        // We shouldn't show tooltips when dragging something
        if (m_widgetWithLeftMouseDown && (m_widgetWithLeftMouseDown->isDraggableWidget() || m_widgetWithLeftMouseDown->isContainer()))
            return nullptr;

        Widget::Ptr toolTip = nullptr;

        mousePos -= getPosition() + getChildWidgetsOffset();

        Widget::Ptr widget = getWidgetBelowMouse(mousePos);
        if (widget && (widget->isEnabled() || ToolTip::getShowOnDisabledWidget()))
        {
            toolTip = widget->askToolTip(transformMousePos(widget, mousePos));
            if (toolTip)
                return toolTip;
        }

        if (m_toolTip)
            return getToolTip();

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rendererChanged(const String& property)
    {
        Widget::rendererChanged(property);

        if ((property == "Opacity") || (property == "OpacityDisabled"))
        {
            for (std::size_t i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->setInheritedOpacity(m_opacityCached);
        }
        else if (property == "Font")
        {
            for (const auto& widget : m_widgets)
            {
                if (m_fontCached != Font::getGlobalFont())
                    widget->setInheritedFont(m_fontCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Container::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        for (const auto& child : getWidgets())
            node->children.emplace_back(child->save(renderers));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        std::vector<std::pair<Widget::Ptr, std::reference_wrapper<const std::unique_ptr<DataIO::Node>>>> widgetsToLoad;
        for (const auto& childNode : node->children)
        {
            const auto nameSeparator = childNode->name.find('.');
            const auto widgetType = childNode->name.substr(0, nameSeparator);

            const auto& constructor = WidgetFactory::getConstructFunction(widgetType);
            if (constructor)
            {
                String className;
                if (nameSeparator != String::npos)
                    className = Deserializer::deserialize(ObjectConverter::Type::String, childNode->name.substr(nameSeparator + 1)).getString();

                Widget::Ptr childWidget = constructor();
                add(childWidget, className);

                // We delay loading of widgets until they have all been added to the container.
                // Otherwise there would be issues if their position and size layouts refer to
                // widgets that have not yet been loaded.
                widgetsToLoad.push_back(std::make_pair(childWidget, std::cref(childNode)));
            }
            else
                throw Exception{"No construct function exists for widget type '" + widgetType + "'."};
        }

        for (auto& pair : widgetsToLoad)
        {
            Widget::Ptr& childWidget = pair.first;
            const auto& childNode = pair.second.get();
            childWidget->load(childNode, renderers);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processMouseMoveEvent(Vector2f mousePos)
    {
        // Some widgets should always receive mouse move events while dragging them, even if the mouse is no longer on top of them
        if (m_widgetWithLeftMouseDown)
        {
            if (m_widgetWithLeftMouseDown->isDraggableWidget() || m_widgetWithLeftMouseDown->isContainer())
            {
                m_widgetWithLeftMouseDown->mouseMoved(transformMousePos(m_widgetWithLeftMouseDown, mousePos));
                return true;
            }
        }

        // Check if the mouse is on top of a widget
        Widget::Ptr widget = updateWidgetBelowMouse(mousePos);
        if (widget != nullptr)
        {
            // Send the event to the widget
            widget->mouseMoved(transformMousePos(widget, mousePos));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processMousePressEvent(Event::MouseButton button, Vector2f mousePos)
    {
        // Check if the mouse is on top of a widget
        Widget::Ptr widget = updateWidgetBelowMouse(mousePos);
        if (widget)
        {
            if (button == Event::MouseButton::Left)
                m_widgetWithLeftMouseDown = widget;
            else if (button == Event::MouseButton::Right)
                m_widgetWithRightMouseDown = widget;

            // Unfocus the previously focused widget
            if (m_focusedWidget && (m_focusedWidget != widget))
                m_focusedWidget->setFocused(false);

            // Focus the widget unless it is a container, in which case it will get focused when the event is handled by the bottom widget
            m_focusedWidget = widget;
            if (!widget->isContainer())
                widget->setFocused(true);

            widget->mousePressed(button, transformMousePos(widget, mousePos));
            return true;
        }
        else // The mouse did not went down on a widget, so unfocus the focused child widget, but keep ourselves focused
        {
            if (button == Event::MouseButton::Left)
                m_widgetWithLeftMouseDown = nullptr;
            else if (button == Event::MouseButton::Right)
                m_widgetWithRightMouseDown = nullptr;

            if (m_focusedWidget)
                m_focusedWidget->setFocused(false);

            m_focusedWidget = nullptr;
            setFocused(true);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processMouseReleaseEvent(Event::MouseButton button, Vector2f mousePos)
    {
        Widget::Ptr widgetBelowMouse = updateWidgetBelowMouse(mousePos);
        if (widgetBelowMouse != nullptr)
            widgetBelowMouse->mouseReleased(button, transformMousePos(widgetBelowMouse, mousePos));

        if ((button == Event::MouseButton::Left) && m_widgetWithLeftMouseDown)
        {
            m_widgetWithLeftMouseDown->leftMouseButtonNoLongerDown();
            m_widgetWithLeftMouseDown = nullptr;
            return true;
        }
        else if ((button == Event::MouseButton::Right) && m_widgetWithRightMouseDown)
        {
            m_widgetWithRightMouseDown->rightMouseButtonNoLongerDown();
            m_widgetWithRightMouseDown = nullptr;
            return true;
        }

        return (widgetBelowMouse != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processMouseWheelScrollEvent(float delta, Vector2f pos)
    {
        // Send the event to the widget below the mouse
        Widget::Ptr widget = updateWidgetBelowMouse(pos);
        if (widget != nullptr)
            return widget->mouseWheelScrolled(delta, transformMousePos(widget, pos));

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processKeyPressEvent(Event::KeyEvent event)
    {
        // Only continue when the character was recognised
        if (event.code == Event::KeyboardKey::Unknown)
            return false;

        // Check if there is a focused widget
        if (m_focusedWidget && m_focusedWidget->isFocused())
        {
            // Tell the widget that the key was pressed
            m_focusedWidget->keyPressed(event);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processTextEnteredEvent(char32_t key)
    {
        // Check if the character that we pressed is allowed
        if ((key < 32) || (key == 127))
            return false;

        // Tell the widget that the key was pressed
        if (m_focusedWidget && m_focusedWidget->isFocused())
        {
            m_focusedWidget->textEntered(key);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::updateTime(Duration elapsedTime)
    {
        bool screenRefreshRequired = Widget::updateTime(elapsedTime);

        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            // Update the elapsed time in widgets that need it
            if (m_widgets[i]->isVisible())
                screenRefreshRequired |= m_widgets[i]->updateTime(elapsedTime);
        }

        m_animationTimeElapsed = {};
        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setParent(Container* parent)
    {
        const GuiBase* oldParentGui = m_parentGui;

        Widget::setParent(parent);

        // If our parent was added to a gui then we need to inform the child widgets that they have been added as well
        if (oldParentGui != m_parentGui)
        {
            for (auto& widget : m_widgets)
                widget->setParent(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setParentGui(GuiBase* gui)
    {
        m_parentGui = gui;

        // If widgets were already added then inform them about the gui
        for (auto& widget : m_widgets)
            widget->setParent(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::getWidgetBelowMouse(Vector2f mousePos) const
    {
        for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it)
        {
            auto& widget = *it;
            if (!widget->isVisible())
                continue;

            if (!widget->isMouseOnWidget(transformMousePos(widget, mousePos)))
                continue;

            return widget;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::updateWidgetBelowMouse(Vector2f mousePos)
    {
        Widget::Ptr widgetBelowMouse = getWidgetBelowMouse(mousePos);
        if (widgetBelowMouse && !widgetBelowMouse->isEnabled())
            widgetBelowMouse = nullptr;

        // If the mouse is on a different widget, tell the old widget that the mouse has left
        if (m_widgetBelowMouse && (widgetBelowMouse != m_widgetBelowMouse))
            m_widgetBelowMouse->mouseNoLongerOnWidget();

        m_widgetBelowMouse = widgetBelowMouse;
        return widgetBelowMouse;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::draw(BackendRenderTargetBase& target, RenderStates states) const
    {
        for (const auto& widget : m_widgets)
        {
            if (!widget->isVisible())
                continue;

            const Vector2f origin{widget->getOrigin().x * widget->getSize().x, widget->getOrigin().y * widget->getSize().y};

            RenderStates widgetStates = states;
            widgetStates.transform.translate(widget->getPosition() - origin);
            if (widget->getRotation() != 0)
            {
                const Vector2f rotOrigin{widget->getRotationOrigin().x * widget->getSize().x, widget->getRotationOrigin().y * widget->getSize().y};
                widgetStates.transform.rotate(widget->getRotation(), rotOrigin);
            }
            if ((widget->getScale().x != 1) || (widget->getScale().y != 1))
            {
                const Vector2f scaleOrigin{widget->getScaleOrigin().x * widget->getSize().x, widget->getScaleOrigin().y * widget->getSize().y};
                widgetStates.transform.scale(widget->getScale(), scaleOrigin);
            }

            target.drawWidget(widgetStates, widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Container::getFocusedWidgetIndex() const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_focusedWidget == m_widgets[i])
                return i+1;
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::tryFocusWidget(const Widget::Ptr &widget, bool reverseWidgetOrder, bool recursive)
    {
        // If you are not allowed to focus the widget, then skip it
        if (!widget->canGainFocus() || !widget->isVisible() || !widget->isEnabled())
            return false;

        if (recursive && widget->isContainer())
        {
            auto container = std::static_pointer_cast<Container>(widget);

            // Also skip isolated containers (e.g. ChildWindow)
            if (container->m_isolatedFocus)
                return false;

            // Try to focus the first focusable widget in the container
            auto oldUnfocusedWidget = container->m_focusedWidget;
            container->m_focusedWidget = nullptr;
            bool childFocused = reverseWidgetOrder ? container->focusPreviousWidget(true) : container->focusNextWidget(true);

            if (oldUnfocusedWidget && (oldUnfocusedWidget != container->m_focusedWidget))
                oldUnfocusedWidget->setFocused(false);

            if (!childFocused)
                return false;
        }

        if (m_focusedWidget == widget)
            return true;

        if (m_focusedWidget)
            m_focusedWidget->setFocused(false);

        m_focusedWidget = widget;
        m_focusedWidget->setFocused(true);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Container::transformMousePos(const Widget::Ptr& widget, Vector2f mousePos) const
    {
        const bool defaultOrigin = (widget->getOrigin().x == 0) && (widget->getOrigin().y == 0);
        const bool scaledOrRotated = (widget->getScale().x != 1) || (widget->getScale().y != 1) || (widget->getRotation() != 0);
        if (defaultOrigin && !scaledOrRotated)
            return mousePos;

        const Vector2f origin{widget->getOrigin().x * widget->getSize().x, widget->getOrigin().y * widget->getSize().y};
        if (!scaledOrRotated)
            return mousePos + origin;

        const Vector2f rotOrigin{widget->getRotationOrigin().x * widget->getSize().x, widget->getRotationOrigin().y * widget->getSize().y};
        const Vector2f scaleOrigin{widget->getScaleOrigin().x * widget->getSize().x, widget->getScaleOrigin().y * widget->getSize().y};

        Transform transform;
        transform.translate(widget->getPosition() - origin);
        transform.rotate(widget->getRotation(), rotOrigin);
        transform.scale(widget->getScale(), scaleOrigin);
        mousePos = transform.getInverse().transformPoint(mousePos);
        return mousePos + widget->getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::widgetAdded(Widget::Ptr widgetPtr)
    {
        if (widgetPtr->getParent())
        {
            widgetPtr->getParent()->remove(widgetPtr);
            TGUI_PRINT_WARNING("widget was already connected to a parent while being added to parent");
        }

        widgetPtr->setParent(this);

        if (m_fontCached != Font::getGlobalFont())
            widgetPtr->setInheritedFont(m_fontCached);

        if (m_opacityCached < 1)
            widgetPtr->setInheritedOpacity(m_opacityCached);

        if (m_textSize != 0)
            widgetPtr->setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::injectFormFilePath(const std::unique_ptr<DataIO::Node>& node, const String& path, std::map<String, bool>& checkedFilenames) const
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

                String filename;
                if (pair.second->value[0] != '"')
                    filename = pair.second->value;
                else
                {
                    // The filename is surrounded by quotes, with optional options behind it
                    const auto endQuotePos = pair.second->value.find('"', 1);
                    TGUI_ASSERT(endQuotePos != String::npos, "End quote must exist in Container::injectFormFilePath, DataIO could not accept the value otherwise");
                    filename = pair.second->value.substr(1, endQuotePos - 1);
                }

                // If this image already appeared in the form file, then we already know whether it exists or not,
                // and we would have already warned if it is located in the wrong folder.
                auto checkedFileIt = checkedFilenames.find(filename);
                if ((checkedFileIt != checkedFilenames.end()) && !checkedFileIt->second)
                    continue; // File found on wrong location, don't inject path to new directory

                // Check if the file is located where it should, and warn if it is located on the old search location
                if (checkedFileIt == checkedFilenames.end())
                {
                    bool canInjectPath = true;
                    if (!Filesystem::fileExists(getResourcePath() / path / filename)
                     && Filesystem::fileExists(getResourcePath() / filename))
                    {
                        canInjectPath = false;
                        TGUI_PRINT_WARNING(U"Form file contained '" + filename
                            + U"', which TGUI now interprets as '" + (getResourcePath() / path / filename).asString()
                            + U"'. File was however found at '" + (getResourcePath() / filename).asString()
                            + U"', which is the deprecated search location. Loading will fail in future TGUI versions.");
                    }

                    checkedFilenames[filename] = canInjectPath;
                    if (!canInjectPath)
                        continue;
                }

                // Insert the path into the filename.
                // We can't just deserialize the value to get rid of the quotes as it may contain things behind the filename.
                if (pair.second->value[0] != '"')
                    pair.second->value = path + '/' + pair.second->value;
                else // The filename is between quotes
                    pair.second->value = '"' + path + '/' + pair.second->value.substr(1);
            }
        }

        for (const auto& child : node->children)
            injectFormFilePath(child, path, checkedFilenames);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RootContainer::RootContainer(const char* typeName, bool initRenderer) :
        Container{typeName, initRenderer}
    {
        m_focused = true;
        m_isolatedFocus = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RootContainer::setSize(const Layout2d& size)
    {
        if (m_size.getValue() == size.getValue())
            return;

        m_size = size;
        onSizeChange.emit(this, size.getValue());
        for (auto& layout : m_boundSizeLayouts)
            layout->recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RootContainer::setFocused(bool focused)
    {
        Container::setFocused(focused);
        m_focused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RootContainer::isMouseOnWidget(Vector2f) const
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RootContainer::draw(BackendRenderTargetBase& target, RenderStates states) const
    {
        // The only reason to override this function was to change the access specifier, so just call the code from the base class
        Container::draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
