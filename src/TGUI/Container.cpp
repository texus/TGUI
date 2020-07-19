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


#include <TGUI/Container.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/SubwidgetContainer.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/Filesystem.hpp>

#include <cassert>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void getAllRenderers(std::map<RendererData*, std::vector<const Widget*>>& renderers, const Container* container)
        {
            for (const auto& child : container->getWidgets())
            {
                renderers[child->getSharedRenderer()->getData().get()].push_back(child.get());

                if (child->getToolTip())
                    renderers[child->getToolTip()->getSharedRenderer()->getData().get()].push_back(child->getToolTip().get());

                Container* childContainer = dynamic_cast<Container*>(child.get());
                if (childContainer)
                    getAllRenderers(renderers, childContainer);
                else
                {
                    SubwidgetContainer* subWidgetContainer = dynamic_cast<SubwidgetContainer*>(child.get());
                    if (subWidgetContainer)
                    {
                        renderers[subWidgetContainer->getContainer()->getSharedRenderer()->getData().get()].push_back(subWidgetContainer->getContainer());
                        getAllRenderers(renderers, subWidgetContainer->getContainer());
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
                    std::stringstream ss{ObjectConverter{pair.second}.getString().toAnsiString()};
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

        static Vector2f transformMousePos(const Widget::Ptr& widget, Vector2f mousePos)
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container()
    {
        m_containerWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(const Container& other) :
        Widget{other}
    {
        // Copy all the widgets
        for (std::size_t i = 0; i < other.m_widgets.size(); ++i)
            add(other.m_widgets[i]->clone(), other.m_widgets[i]->getWidgetName());
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

            // Copy all the widgets
            for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
            {
                // Don't allow the 'add' function of a derived class to be called, since its members are not copied yet
                Container::add(right.m_widgets[i]->clone(), right.m_widgets[i]->getWidgetName());
            }
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
        assert(widgetPtr != nullptr);

        if (widgetPtr->getParent())
            widgetPtr->getParent()->remove(widgetPtr);

        widgetPtr->setParent(this);
        m_widgets.push_back(widgetPtr);
        widgetPtr->setWidgetName(widgetName);

        if (m_fontCached != getGlobalFont())
            widgetPtr->setInheritedFont(m_fontCached);

        if (m_opacityCached < 1)
            widgetPtr->setInheritedOpacity(m_opacityCached);

        if (m_textSize != 0)
            widgetPtr->setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::get(const String& widgetName) const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i]->getWidgetName() == widgetName)
                return m_widgets[i];
        }

        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i]->isContainer())
            {
                Widget::Ptr widget = std::static_pointer_cast<Container>(m_widgets[i])->get(widgetName);
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
        // If a resource path is set then place it in front of the filename (unless the filename is an absolute path)
        String filenameInResources = filename;
        if (!getResourcePath().empty())
            filenameInResources = (Filesystem::Path(getResourcePath()) / filename).asString();

        std::ifstream in{filenameInResources.toAnsiString()};
        if (!in.is_open())
            throw Exception{"Failed to open '" + filenameInResources + "' to load the widgets from it."};

        std::stringstream stream;
        stream << in.rdbuf();
        loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToFile(const String& filename)
    {
        // If a resource path is set then place it in front of the filename (unless the filename is an absolute path)
        String filenameInResources = filename;
        if (!getResourcePath().empty())
            filenameInResources = (Filesystem::Path(getResourcePath()) / filename).asString();

        std::stringstream stream;
        saveWidgetsToStream(stream);

        std::ofstream out{filenameInResources.toAnsiString()};
        if (!out.is_open())
            throw Exception{"Failed to open '" + filenameInResources + "' for saving the widgets to it."};

        out << stream.rdbuf();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream& stream, bool replaceExisting)
    {
        auto rootNode = DataIO::parse(stream);

        // Replace the existing widgets by the ones that will be loaded if requested
        if (replaceExisting)
            removeAllWidgets();

        if (rootNode->propertyValuePairs.size() != 0)
            Widget::load(rootNode, {});

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
                    widget->load(node, availableRenderers);
                    add(widget, objectName);
                }
                else
                    throw Exception{"No construct function exists for widget type '" + widgetType + "'."};
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream&& stream, bool replaceExisting)
    {
        loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToStream(std::stringstream& stream) const
    {
        auto node = std::make_unique<DataIO::Node>();

        std::map<RendererData*, std::vector<const Widget*>> renderers;
        getAllRenderers(renderers, this);

        unsigned int id = 0;
        SavingRenderersMap renderersMap;
        for (const auto& renderer : renderers)
        {
            // The renderer can remain inside the widget if it is not shared, so provide the node to be included inside the widget
            if (renderer.second.size() == 1)
            {
                renderersMap[renderer.second[0]] = {saveRenderer(renderer.first, "Renderer"), ""};
                continue;
            }

            // When the widget is shared, only provide the id instead of the node itself
            ++id;
            const String idStr = String::fromNumber(id);
            node->children.push_back(saveRenderer(renderer.first, "Renderer." + idStr));
            for (const auto& child : renderer.second)
                renderersMap[child] = std::make_pair(nullptr, idStr); // Did not compile with VS2015 Update 2 when using braces
        }

        for (const auto& child : getWidgets())
            node->children.emplace_back(child->save(renderersMap));

        DataIO::emit(node, stream);
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
        if (!focused && m_focused && m_focusedWidget)
            m_focusedWidget->setFocused(false);

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
        processMousePressEvent(Event::MouseButton::Left, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rightMousePressed(Vector2f pos)
    {
        processMousePressEvent(Event::MouseButton::Right, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMouseReleased(Vector2f pos)
    {
        processMouseReleaseEvent(Event::MouseButton::Left, pos - getPosition() - getChildWidgetsOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rightMouseReleased(Vector2f pos)
    {
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
        if (m_mouseHover)
        {
            mouseLeftWidget();

            if (m_widgetBelowMouse)
            {
                m_widgetBelowMouse->mouseNoLongerOnWidget();
                m_widgetBelowMouse = nullptr;
            }
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
        if (isMouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset();

            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                toolTip = widget->askToolTip(transformMousePos(widget, mousePos));
                if (toolTip)
                    return toolTip;
            }

            if (m_toolTip)
                return getToolTip();
        }

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
                if (m_fontCached != getGlobalFont())
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
                childWidget->load(childNode, renderers);
                add(childWidget, className);
            }
            else
                throw Exception{"No construct function exists for widget type '" + widgetType + "'."};
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
        Widget::Ptr widget = mouseOnWhichWidget(mousePos);
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
        Widget::Ptr widget = mouseOnWhichWidget(mousePos);
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
        Widget::Ptr widgetBelowMouse = mouseOnWhichWidget(mousePos);
        if (widgetBelowMouse != nullptr)
            widgetBelowMouse->mouseReleased(button, transformMousePos(widgetBelowMouse, mousePos));

        if ((button == Event::MouseButton::Left) && m_widgetWithLeftMouseDown)
        {
            m_widgetWithLeftMouseDown->leftMouseButtonNoLongerDown();
            m_widgetWithLeftMouseDown = nullptr;
        }
        else if ((button == Event::MouseButton::Right) && m_widgetWithRightMouseDown)
        {
            m_widgetWithRightMouseDown->rightMouseButtonNoLongerDown();
            m_widgetWithRightMouseDown = nullptr;
        }

        return (widgetBelowMouse != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::processMouseWheelScrollEvent(float delta, Vector2f pos)
    {
        // Send the event to the widget below the mouse
        Widget::Ptr widget = mouseOnWhichWidget(pos);
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
        const Gui* oldParentGui = m_parentGui;

        Widget::setParent(parent);

        // If our parent was added to a gui then we need to inform the child widgets that they have been added as well
        if (oldParentGui != m_parentGui)
        {
            for (auto& widget : m_widgets)
                widget->setParent(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::mouseOnWhichWidget(Vector2f mousePos)
    {
        Widget::Ptr widgetBelowMouse = nullptr;
        for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it)
        {
            auto& widget = *it;
            if (!widget->isVisible())
                continue;

            if (!widget->isMouseOnWidget(transformMousePos(widget, mousePos)))
                continue;

            if (widget->isEnabled())
                widgetBelowMouse = widget;

            break;
        }

        // If the mouse is on a different widget, tell the old widget that the mouse has left
        if (m_widgetBelowMouse && (widgetBelowMouse != m_widgetBelowMouse))
            m_widgetBelowMouse->mouseNoLongerOnWidget();

        m_widgetBelowMouse = widgetBelowMouse;
        return widgetBelowMouse;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::draw(RenderTargetBase& target, RenderStates states) const
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

    bool Container::tryFocusWidget(const tgui::Widget::Ptr &widget, bool reverseWidgetOrder, bool recursive)
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RootContainer::RootContainer()
    {
        m_type = "RootContainer";
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

    void RootContainer::draw(RenderTargetBase& target, RenderStates states) const
    {
        // The only reason to override this function was to change the access specifier, so just call the code from the base class
        Container::draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RootContainer::setParentGui(Gui* gui)
    {
        m_parentGui = gui;

        // If widgets were already added then inform them about the gui
        for (auto& widget : m_widgets)
            widget->setParent(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
