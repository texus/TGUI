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


#include <TGUI/Container.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>

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
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveRenderer(RendererData* renderer, const std::string& name)
        {
            auto node = std::make_unique<DataIO::Node>();
            node->name = name;
            for (const auto& pair : renderer->propertyValuePairs)
            {
                // Skip "font = null"
                if (pair.first == "font" && ObjectConverter{pair.second}.getString() == "null")
                    continue;

                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{ObjectConverter{pair.second}.getString()};
                    auto rendererRootNode = DataIO::parse(ss);

                    // If there are braces around the renderer string, then the child node is the one we need
                    if (rendererRootNode->propertyValuePairs.empty() && (rendererRootNode->children.size() == 1))
                        rendererRootNode = std::move(rendererRootNode->children[0]);

                    rendererRootNode->name = pair.first;
                    node->children.push_back(std::move(rendererRootNode));
                }
                else
                    node->propertyValuePairs[pair.first] = std::make_unique<DataIO::ValueNode>(ObjectConverter{pair.second}.getString());
            }

            return node;
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
            add(other.m_widgets[i]->clone(), other.m_widgetNames[i]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(Container&& other) :
        Widget                {std::move(other)},
        m_widgets             {std::move(other.m_widgets)},
        m_widgetNames         {std::move(other.m_widgetNames)},
        m_widgetBelowMouse    {std::move(other.m_widgetBelowMouse)},
        m_focusedWidget       {std::move(other.m_focusedWidget)},
        m_handingMouseReleased{std::move(other.m_handingMouseReleased)}
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
            m_focusedWidget = nullptr;

            // Remove all the old widgets
            Container::removeAllWidgets();

            // Copy all the widgets
            for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
            {
                // Don't allow the 'add' function of a derived class to be called, since its members are not copied yet
                Container::add(right.m_widgets[i]->clone(), right.m_widgetNames[i]);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Container::operator= (Container&& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Widget::operator=(std::move(right));
            m_widgets              = std::move(right.m_widgets);
            m_widgetNames          = std::move(right.m_widgetNames);
            m_widgetBelowMouse     = std::move(right.m_widgetBelowMouse);
            m_focusedWidget        = std::move(right.m_focusedWidget);
            m_handingMouseReleased = std::move(right.m_handingMouseReleased);

            for (auto& widget : m_widgets)
                widget->setParent(this);

            right.m_widgets = {};
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        assert(widgetPtr != nullptr);

        widgetPtr->setParent(this);
        m_widgets.push_back(widgetPtr);
        m_widgetNames.push_back(widgetName);

        if (m_fontCached != getGlobalFont())
            widgetPtr->setInheritedFont(m_fontCached);

        if (m_opacityCached < 1)
            widgetPtr->setInheritedOpacity(m_opacityCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::get(const sf::String& widgetName) const
    {
        for (std::size_t i = 0; i < m_widgetNames.size(); ++i)
        {
            if (m_widgetNames[i] == widgetName)
                return m_widgets[i];
        }

        for (std::size_t i = 0; i < m_widgetNames.size(); ++i)
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
            // Check if the pointer matches
            if (m_widgets[i] == widget)
            {
                if (m_widgetBelowMouse == widget)
                {
                    widget->mouseNoLongerOnWidget();
                    m_widgetBelowMouse = nullptr;
                }

                if (widget == m_focusedWidget)
                {
                    m_focusedWidget = nullptr;
                    widget->setFocused(false);
                }

                // Remove the widget
                widget->setParent(nullptr);
                m_widgets.erase(m_widgets.begin() + i);
                m_widgetNames.erase(m_widgetNames.begin() + i);
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::removeAllWidgets()
    {
        for (const auto& widget : m_widgets)
            widget->setParent(nullptr);

        m_widgets.clear();
        m_widgetNames.clear();

        m_widgetBelowMouse = nullptr;
        m_focusedWidget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
            {
                m_widgetNames[i] = name;
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Container::getWidgetName(const Widget::ConstPtr& widget) const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return m_widgetNames[i];
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (auto& widget : m_widgets)
        {
            if (widget->getWidgetType() == "RadioButton")
                std::static_pointer_cast<RadioButton>(widget)->setChecked(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromFile(const std::string& filename)
    {
        std::ifstream in{filename};
        if (!in.is_open())
            throw Exception{"Failed to open '" + filename + "' to load the widgets from it."};

        std::stringstream stream;
        stream << in.rdbuf();
        loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToFile(const std::string& filename)
    {
        std::stringstream stream;
        saveWidgetsToStream(stream);

        std::ofstream out{filename};
        if (!out.is_open())
            throw Exception{"Failed to open '" + filename + "' for saving the widgets to it."};

        out << stream.rdbuf();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream& stream)
    {
        auto rootNode = DataIO::parse(stream);

        removeAllWidgets(); // The existing widgets will be replaced by the ones that will be loaded

        if (rootNode->propertyValuePairs.size() != 0)
            Widget::load(rootNode, {});

        std::map<std::string, std::shared_ptr<RendererData>> availableRenderers;
        for (const auto& node : rootNode->children)
        {
            auto nameSeparator = node->name.find('.');
            auto widgetType = node->name.substr(0, nameSeparator);

            std::string objectName;
            if (nameSeparator != std::string::npos)
                objectName = Deserializer::deserialize(ObjectConverter::Type::String, node->name.substr(nameSeparator + 1)).getString();

            if (toLower(widgetType) == "renderer")
            {
                if (!objectName.empty())
                    availableRenderers[toLower(objectName)] = RendererData::createFromDataIONode(node.get());
            }
            else // Section describes a widget
            {
                const auto& constructor = WidgetFactory::getConstructFunction(toLower(widgetType));
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

    void Container::loadWidgetsFromStream(std::stringstream&& stream)
    {
        loadWidgetsFromStream(stream);
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
            const std::string idStr = to_string(id);
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
            m_widgetNames.push_back(m_widgetNames[i]);

            // Remove the old widget
            m_widgets.erase(m_widgets.begin() + i);
            m_widgetNames.erase(m_widgetNames.begin() + i);
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
            const std::string name = m_widgetNames[i];
            m_widgets.insert(m_widgets.begin(), obj);
            m_widgetNames.insert(m_widgetNames.begin(), name);

            // Remove the old widget
            m_widgets.erase(m_widgets.begin() + i + 1);
            m_widgetNames.erase(m_widgetNames.begin() + i + 1);
            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusNextWidget()
    {
        // If the focused widget is a container then try to focus the next widget inside it
        if (m_focusedWidget && m_focusedWidget->isContainer())
        {
            auto focusedContainer = std::static_pointer_cast<Container>(m_focusedWidget);
            if (focusedContainer->focusNextWidget())
                return true;
        }

        // Loop all widgets behind the focused one
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();
        for (std::size_t i = focusedWidgetIndex; i < m_widgets.size(); ++i)
        {
            if (tryFocusWidget(m_widgets[i], false))
                return true;
        }

        // If we are not an isolated focus group then the focus will be given to the group behind us
        if (!m_isolatedFocus)
            return false;

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (!m_focusedWidget)
            return false;

        // Also include the focused widget since it may be a container that didn't have its first widget focused
        for (std::size_t i = 0; i < focusedWidgetIndex; ++i)
        {
            if (tryFocusWidget(m_widgets[i], false))
                return true;
        }

        // No other widget could be focused
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusPreviousWidget()
    {
        // If the focused widget is a container then try to focus the previous widget inside it
        if (m_focusedWidget && m_focusedWidget->isContainer())
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
                if (tryFocusWidget(m_widgets[i-1], true))
                    return true;
            }

            // If we are not an isolated focus group then the focus will be given to the group before us
            if (!m_isolatedFocus)
                return false;
        }

        // None of the widgets before the focused one could be focused, so loop the ones after it.
        for (std::size_t i = m_widgets.size(); i > focusedWidgetIndex; --i)
        {
            if (tryFocusWidget(m_widgets[i-1], true))
                return true;
        }

        // Also include the focused widget since it may be a container that didn't have its last widget focused.
        if (focusedWidgetIndex > 0)
        {
            if (tryFocusWidget(m_widgets[focusedWidgetIndex-1], true))
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
        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseButton.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMouseReleased(Vector2f pos)
    {
        sf::Event event;
        event.type = sf::Event::MouseButtonReleased;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseButton.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);

        // Let the event manager handle the event, but don't let it call mouseNoLongerDown on all widgets (it will be done later)
        m_handingMouseReleased = true;
        handleEvent(event);
        m_handingMouseReleased = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);

        sf::Event event;
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseMove.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::keyPressed(const sf::Event::KeyEvent& event)
    {
        sf::Event newEvent;
        newEvent.type = sf::Event::KeyPressed;
        newEvent.key = event;

        // Let the event manager handle the event
        handleEvent(newEvent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::textEntered(std::uint32_t key)
    {
        sf::Event event;
        event.type = sf::Event::TextEntered;
        event.text.unicode = key;

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseWheelScrolled(float delta, Vector2f pos)
    {
        sf::Event event;
        event.type = sf::Event::MouseWheelScrolled;
        event.mouseWheelScroll.delta = delta;
        event.mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel;
        event.mouseWheelScroll.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseWheelScroll.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);

        // Let the event manager handle the event
        handleEvent(event);
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

    void Container::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        for (auto& widget : m_widgets)
            widget->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::askToolTip(Vector2f mousePos)
    {
        if (mouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset();

            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                toolTip = widget->askToolTip(mousePos);
                if (toolTip)
                    return toolTip;
            }

            if (m_toolTip)
                return getToolTip();
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rendererChanged(const std::string& property)
    {
        Widget::rendererChanged(property);

        if (property == "opacity")
        {
            for (std::size_t i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->setInheritedOpacity(m_opacityCached);
        }
        else if (property == "font")
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

            const auto& constructor = WidgetFactory::getConstructFunction(toLower(widgetType));
            if (constructor)
            {
                std::string className;
                if (nameSeparator != std::string::npos)
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

    void Container::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            // Update the elapsed time in widgets that need it
            if (m_widgets[i]->isVisible())
                m_widgets[i]->update(elapsedTime);
        }

        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::handleEvent(sf::Event& event)
    {
        // Check if a mouse button has moved
        if ((event.type == sf::Event::MouseMoved) || ((event.type == sf::Event::TouchMoved) && (event.touch.finger == 0)))
        {
            Vector2f mousePos;
            if (event.type == sf::Event::MouseMoved)
                mousePos = {static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)};
            else
                mousePos = {static_cast<float>(event.touch.x), static_cast<float>(event.touch.y)};

            // Loop through all widgets
            for (auto& widget : m_widgets)
            {
                // Check if the mouse went down on the widget
                if (widget->m_mouseDown)
                {
                    // Some widgets should always receive mouse move events while dragging them, even if the mouse is no longer on top of them.
                    if (widget->m_draggableWidget || widget->isContainer())
                    {
                        widget->mouseMoved(mousePos);
                        return true;
                    }
                }
            }

            // Check if the mouse is on top of a widget
            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseMoved(mousePos);
                return true;
            }

            return false;
        }

        // Check if a mouse button was pressed or a touch event occurred
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::TouchBegan))
        {
            Vector2f mousePos;
            if (event.type == sf::Event::MouseButtonPressed)
                mousePos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};
            else
                mousePos = {static_cast<float>(event.touch.x), static_cast<float>(event.touch.y)};

            // Check if the mouse is on top of a widget
            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                // Unfocus the previously focused widget
                if (m_focusedWidget && (m_focusedWidget != widget))
                    m_focusedWidget->setFocused(false);

                // Focus the widget unless it is a container, in which case it will get focused when the event is handled by the bottom widget
                m_focusedWidget = widget;
                if (!widget->isContainer())
                    widget->setFocused(true);

                if (((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Left))
                 || ((event.type == sf::Event::TouchBegan) && (event.touch.finger == 0)))
                {
                    widget->leftMousePressed(mousePos);
                    return true;
                }
                else if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Right))
                {
                    widget->rightMousePressed(mousePos);
                    return true;
                }
            }
            else // The mouse did not went down on a widget, so unfocus the focused child widget, but keep ourselves focused
            {
                if (m_focusedWidget)
                    m_focusedWidget->setFocused(false);

                m_focusedWidget = nullptr;
                setFocused(true);
            }

            return false;
        }

        // Check if a mouse button was released
        else if (((event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Left))
              || ((event.type == sf::Event::TouchEnded) && (event.touch.finger == 0)))
        {
            Vector2f mousePos;
            if (event.type == sf::Event::MouseButtonReleased)
                mousePos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};
            else
                mousePos = {static_cast<float>(event.touch.x), static_cast<float>(event.touch.y)};

            // Check if the mouse is on top of a widget
            Widget::Ptr widgetBelowMouse = mouseOnWhichWidget(mousePos);
            if (widgetBelowMouse != nullptr)
                widgetBelowMouse->leftMouseReleased(mousePos);

            // Tell all widgets that the mouse has gone up
            // But don't do this when leftMouseReleased was called on this container because
            // it will happen afterwards when mouseNoLongerDown is called on it
            if (!m_handingMouseReleased)
            {
                for (auto& widget : m_widgets)
                    widget->mouseNoLongerDown();
            }

            if (widgetBelowMouse != nullptr)
                return true;

            return false;
        }

        // Check if a key was pressed
        else if (event.type == sf::Event::KeyPressed)
        {
            // Only continue when the character was recognised
            if (event.key.code != sf::Keyboard::Unknown)
            {
                // Check if there is a focused widget
                if (m_focusedWidget && m_focusedWidget->isFocused())
                {
                #ifdef SFML_SYSTEM_ANDROID
                    // Map delete to backspace on android
                    if (event.key.code == sf::Keyboard::Delete)
                        event.key.code = sf::Keyboard::BackSpace;
                #endif

                    // Tell the widget that the key was pressed
                    m_focusedWidget->keyPressed(event.key);
                    return true;
                }
            }

            return false;
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 32) && (event.text.unicode != 127))
            {
                // Tell the widget that the key was pressed
                if (m_focusedWidget && m_focusedWidget->isFocused())
                {
                    m_focusedWidget->textEntered(event.text.unicode);
                    return true;
                }
            }

            return false;
        }

        // Check for mouse wheel scrolling
        else if ((event.type == sf::Event::MouseWheelScrolled) && (event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel))
        {
            // Find the widget under the mouse
            Widget::Ptr widget = mouseOnWhichWidget({static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y)});
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseWheelScrolled(event.mouseWheelScroll.delta, {static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y)});
                return true;
            }

            return false;
        }
        else // Event is ignored
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::mouseOnWhichWidget(Vector2f mousePos)
    {
        Widget::Ptr widget = nullptr;
        for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it)
        {
            if ((*it)->isVisible())
            {
                if ((*it)->mouseOnWidget(mousePos))
                {
                    if ((*it)->isEnabled())
                        widget = *it;

                    break;
                }
            }
        }

        // If the mouse is on a different widget, tell the old widget that the mouse has left
        if (m_widgetBelowMouse && (widget != m_widgetBelowMouse))
            m_widgetBelowMouse->mouseNoLongerOnWidget();

        m_widgetBelowMouse = widget;
        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::drawWidgetContainer(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        // Draw all widgets when they are visible
        for (const auto& widget : m_widgets)
        {
            if (widget->isVisible())
                widget->draw(*target, states);
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

    bool Container::tryFocusWidget(const tgui::Widget::Ptr &widget, bool reverseWidgetOrder)
    {
        // If you are not allowed to focus the widget, then skip it
        if (!widget->canGainFocus() || !widget->isVisible() || !widget->isEnabled())
            return false;

        if (widget->isContainer())
        {
            auto container = std::static_pointer_cast<Container>(widget);

            // Also skip isolated containers (e.g. ChildWindow)
            if (container->m_isolatedFocus)
                return false;

            // Try to focus the first focusable widget in the container
            auto oldUnfocusedWidget = container->m_focusedWidget;
            container->m_focusedWidget = nullptr;
            bool childFocused = reverseWidgetOrder ? container->focusPreviousWidget() : container->focusNextWidget();

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

    GuiContainer::GuiContainer()
    {
        m_type = "GuiContainer";
        m_focused = true;
        m_isolatedFocus = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::setSize(const Layout2d&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::setFocused(bool focused)
    {
        Container::setFocused(focused);
        m_focused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiContainer::mouseOnWidget(Vector2f) const
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::draw(sf::RenderTarget&, sf::RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
