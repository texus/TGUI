/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifdef SFML_SYSTEM_WINDOWS
    #include <direct.h> // _getcwd
#else
    #include <unistd.h> // getcwd
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string getWorkingDirectory()
        {
            const std::string resourcePath = getResourcePath();

            std::string workingDirectory;
        #ifdef SFML_SYSTEM_WINDOWS
            if ((resourcePath[0] == '/') || (resourcePath[0] == '\\') || ((resourcePath.size() > 1) && (resourcePath[1] == ':')))
        #else
            if (resourcePath[0] == '/')
        #endif
            {
                // The resource path is already an absolute path, we don't even need to find out the current working directory
                workingDirectory = resourcePath;
            }
            else
            {
                // Get the current working directory (used for turning absolute into relative paths in saveWidget)
            #ifdef SFML_SYSTEM_WINDOWS
                char* buffer = _getcwd(nullptr, 0);
            #else
                char* buffer = getcwd(nullptr, 0);
            #endif
                if (buffer)
                {
                    workingDirectory = buffer;
                    free(buffer);

                    if (!workingDirectory.empty() && (workingDirectory[workingDirectory.size() - 1] != '/') && (workingDirectory[workingDirectory.size() - 1] != '\\'))
                        workingDirectory.push_back('/');

                    workingDirectory += resourcePath;
                }
            }

            return workingDirectory;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        sf::String tryRemoveAbsolutePath(const sf::String& value, const std::string& workingDirectory)
        {
            if (!value.isEmpty() && (value != "null") && (value != "nullptr"))
            {
                if (value[0] != '"')
                {
                #ifdef SFML_SYSTEM_WINDOWS
                    if ((value[0] == '/') || (value[0] == '\\') || ((value.getSize() > 1) && (value[1] == ':')))
                #else
                    if (value[0] == '/')
                #endif
                    {
                        if ((value.getSize() > workingDirectory.size()) && (value.substring(0, workingDirectory.size()) == workingDirectory))
                        {
                            if ((value[workingDirectory.size()] != '/') && (value[workingDirectory.size()] != '\\'))
                                return value.substring(workingDirectory.size());
                            else
                                return value.substring(workingDirectory.size() + 1);
                        }
                    }
                }
                else // The filename is between quotes
                {
                    if (value.getSize() <= 1)
                        return value;

                #ifdef SFML_SYSTEM_WINDOWS
                    if ((value[1] == '/') || (value[1] == '\\') || ((value.getSize() > 2) && (value[2] == ':')))
                #else
                    if (value[1] == '/')
                #endif
                    {
                        if ((value.getSize() + 1 > workingDirectory.size()) && (value.substring(1, workingDirectory.size()) == workingDirectory))
                            return '"' + value.substring(workingDirectory.size() + 1);
                    }
                }
            }

            return value;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void recursiveTryRemoveAbsolutePath(std::unique_ptr<DataIO::Node>& node, const std::string& workingDirectory)
        {
            for (auto& pair : node->propertyValuePairs)
            {
                if (((pair.first.size() >= 7) && (toLower(pair.first.substr(0, 7)) == "texture")) || (pair.first == "font"))
                    pair.second->value = tryRemoveAbsolutePath(pair.second->value, workingDirectory);
            }

            for (auto& child : node->children)
                recursiveTryRemoveAbsolutePath(child, workingDirectory);
        }

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

        std::unique_ptr<DataIO::Node> saveRenderer(RendererData* renderer, const std::string& name, const std::string& workingDirectory)
        {
            auto node = make_unique<DataIO::Node>();
            node->name = name;
            for (const auto& pair : renderer->propertyValuePairs)
            {
                // Skip "font = null"
                if (pair.first == "font" && ObjectConverter{pair.second}.getString() == "null")
                    continue;

                sf::String value = ObjectConverter{pair.second}.getString();

                // Turn absolute paths (which were likely caused by loading from a theme) into relative paths if the first part of the path matches the current working directory
                if (!workingDirectory.empty())
                {
                    if ((pair.second.getType() == ObjectConverter::Type::Font) || (pair.second.getType() == ObjectConverter::Type::Texture))
                        value = tryRemoveAbsolutePath(value, workingDirectory);
                }

                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{value};
                    auto rendererRootNode = DataIO::parse(ss);

                    // If there are braces around the renderer string, then the child node is the one we need
                    if (rendererRootNode->propertyValuePairs.empty() && (rendererRootNode->children.size() == 1))
                        rendererRootNode = std::move(rendererRootNode->children[0]);

                    recursiveTryRemoveAbsolutePath(rendererRootNode, workingDirectory);

                    rendererRootNode->name = pair.first;
                    node->children.push_back(std::move(rendererRootNode));
                }
                else
                    node->propertyValuePairs[pair.first] = make_unique<DataIO::ValueNode>(value);
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container()
    {
        m_containerWidget = true;
        m_allowFocus = true;
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

        if (m_fontCached)
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
            if (m_widgets[i]->m_containerWidget)
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
                    unfocusWidgets();

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

    void Container::focusWidget(const Widget::Ptr& widget)
    {
        // Loop all the widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            // Search for the widget that has to be focused
            if (m_widgets[i] == widget)
            {
                // Only continue when the widget wasn't already focused
                if (widget != m_focusedWidget)
                {
                    // Unfocus the currently focused widget
                    if (m_focusedWidget)
                    {
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();
                        m_focusedWidget = nullptr;
                    }

                    // Focus the new widget
                    if (widget->isEnabled())
                    {
                        m_focusedWidget = widget;
                        widget->m_focused = true;
                        widget->widgetFocused();
                    }
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusNextWidget()
    {
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();

        // Loop all widgets behind the focused one
        for (std::size_t i = focusedWidgetIndex; i < m_widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_widgets[i]->m_allowFocus)
            {
                // Make sure that the widget is visible and enabled
                if ((m_widgets[i]->isVisible()) && (m_widgets[i]->isEnabled()))
                {
                    if (m_focusedWidget)
                    {
                        // unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_focusedWidget = m_widgets[i];
                    m_widgets[i]->m_focused = true;
                    m_widgets[i]->widgetFocused();
                    return;
                }
            }
        }

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (m_focusedWidget)
        {
            for (std::size_t i = 0; i < focusedWidgetIndex - 1; ++i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_widgets[i]->m_allowFocus)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_widgets[i]->isVisible()) && (m_widgets[i]->isEnabled()))
                    {
                        // unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();

                        // Focus on the new widget
                        m_focusedWidget = m_widgets[i];
                        m_widgets[i]->m_focused = true;
                        m_widgets[i]->widgetFocused();

                        return;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusPreviousWidget()
    {
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();

        // Loop the widgets before the focused one
        if (m_focusedWidget)
        {
            for (std::size_t i = focusedWidgetIndex - 1; i > 0; --i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_widgets[i-1]->m_allowFocus)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_widgets[i-1]->isVisible()) && (m_widgets[i-1]->isEnabled()))
                    {
                        // unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();

                        // Focus on the new widget
                        m_focusedWidget = m_widgets[i-1];
                        m_widgets[i-1]->m_focused = true;
                        m_widgets[i-1]->widgetFocused();

                        return;
                    }
                }
            }
        }

        // None of the widgets before the focused one could be focused, so loop all widgets behind the focused one
        for (std::size_t i = m_widgets.size(); i > focusedWidgetIndex; --i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_widgets[i-1]->m_allowFocus)
            {
                // Make sure that the widget is visible and enabled
                if ((m_widgets[i-1]->isVisible()) && (m_widgets[i-1]->isEnabled()))
                {
                    if (m_focusedWidget)
                    {
                        // unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_focusedWidget = m_widgets[i-1];
                    m_widgets[i-1]->m_focused = true;
                    m_widgets[i-1]->widgetFocused();
                    return;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::unfocusWidgets()
    {
        if (m_focusedWidget)
        {
            m_focusedWidget->m_focused = false;
            m_focusedWidget->widgetUnfocused();
            m_focusedWidget = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (auto& widget : m_widgets)
        {
            if (widget->getWidgetType() == "RadioButton")
                std::static_pointer_cast<RadioButton>(widget)->uncheck();
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
        const std::string workingDirectory = getWorkingDirectory();

        auto node = make_unique<DataIO::Node>();

        std::map<RendererData*, std::vector<const Widget*>> renderers;
        getAllRenderers(renderers, this);

        unsigned int id = 0;
        SavingRenderersMap renderersMap;
        for (const auto& renderer : renderers)
        {
            // The renderer can remain inside the widget if it is not shared, so provide the node to be included inside the widget
            if (renderer.second.size() == 1)
            {
                renderersMap[renderer.second[0]] = {saveRenderer(renderer.first, "Renderer", workingDirectory), ""};
                continue;
            }

            // When the widget is shared, only provide the id instead of the node itself
            ++id;
            const std::string idStr = to_string(id);
            node->children.push_back(saveRenderer(renderer.first, "Renderer." + idStr, workingDirectory));
            for (const auto& child : renderer.second)
                renderersMap[child] = {nullptr, idStr};
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
            // Check if the widget is found
            if (m_widgets[i] == widget)
            {
                // Copy the widget
                m_widgets.push_back(m_widgets[i]);
                m_widgetNames.push_back(m_widgetNames[i]);

                // Remove the old widget
                m_widgets.erase(m_widgets.begin() + i);
                m_widgetNames.erase(m_widgetNames.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToBack(const Widget::Ptr& widget)
    {
        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            // Check if the widget is found
            if (m_widgets[i] == widget)
            {
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
                m_widgetBelowMouse->mouseNoLongerOnWidget();
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

    void Container::widgetUnfocused()
    {
        unfocusWidgets();
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
                widget->setInheritedFont(m_fontCached);
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
                    if ((widget->m_draggableWidget) || (widget->m_containerWidget))
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
            if (widget != nullptr)
            {
                // Focus the widget
                focusWidget(widget);

                // Check if the widget is a container
                if (widget->m_containerWidget)
                {
                    // If another widget was focused then unfocus it now
                    if (m_focusedWidget && (widget != m_focusedWidget))
                    {
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();
                        m_focusedWidget = nullptr;
                    }
                }

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
            else // The mouse did not went down on a widget, so unfocus the focused widget
                unfocusWidgets();

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
                if (m_focusedWidget)
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

        // Check if a key was released
        else if (event.type == sf::Event::KeyReleased)
        {
            // Change the focus to another widget when the tab key was pressed
            if (event.key.code == sf::Keyboard::Tab)
            {
                if (event.key.shift)
                    return shiftTabKeyPressed();
                else
                    return tabKeyPressed();
            }
            else
                return false;
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 32) && (event.text.unicode != 127))
            {
                // Tell the widget that the key was pressed
                if (m_focusedWidget)
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

    bool Container::focusPrevWidgetInContainer()
    {
        // Don't do anything when the tab key usage is disabled
        if (!isTabKeyUsageEnabled())
            return false;

        // If the focused widget is a container then try to focus the previous widget inside it
        if (m_focusedWidget && m_focusedWidget->m_containerWidget && std::static_pointer_cast<Container>(m_focusedWidget)->focusPrevWidgetInContainer())
            return true;

        for (std::size_t i = getFocusedWidgetIndex() - 1; i > 0; ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_widgets[i-1]->m_allowFocus)
            {
                // Make sure that the widget is visible and enabled
                if ((m_widgets[i-1]->isVisible()) && (m_widgets[i-1]->isEnabled()))
                {
                    // Container widgets can only be focused it they contain focusable widgets
                    if ((!m_widgets[i-1]->m_containerWidget) || (std::static_pointer_cast<Container>(m_widgets[i-1])->focusPrevWidgetInContainer()))
                    {
                        if (m_focusedWidget)
                        {
                            // Unfocus the current widget
                            m_focusedWidget->m_focused = false;
                            m_focusedWidget->widgetUnfocused();
                        }

                        // Focus on the new widget
                        m_focusedWidget = m_widgets[i-1];
                        m_widgets[i-1]->m_focused = true;
                        m_widgets[i-1]->widgetFocused();

                        return true;
                    }
                }
            }
        }

        // We have the lowest id
        unfocusWidgets();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusNextWidgetInContainer()
    {
        // Don't do anything when the tab key usage is disabled
        if (!isTabKeyUsageEnabled())
            return false;

        // If the focused widget is a container then try to focus the next widget inside it
        if (m_focusedWidget && m_focusedWidget->m_containerWidget && std::static_pointer_cast<Container>(m_focusedWidget)->focusNextWidgetInContainer())
            return true;

        // Loop through all widgets
        for (std::size_t i = getFocusedWidgetIndex(); i < m_widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_widgets[i]->m_allowFocus)
            {
                // Make sure that the widget is visible and enabled
                if ((m_widgets[i]->isVisible()) && (m_widgets[i]->isEnabled()))
                {
                    // Container widgets can only be focused it they contain focusable widgets
                    if ((!m_widgets[i]->m_containerWidget) || (std::static_pointer_cast<Container>(m_widgets[i])->focusNextWidgetInContainer()))
                    {
                        if (m_focusedWidget)
                        {
                            // Unfocus the current widget
                            m_focusedWidget->m_focused = false;
                            m_focusedWidget->widgetUnfocused();
                        }

                        // Focus on the new widget
                        m_focusedWidget = m_widgets[i];
                        m_widgets[i]->m_focused = true;
                        m_widgets[i]->widgetFocused();

                        return true;
                    }
                }
            }
        }

        // We have the highest id
        unfocusWidgets();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::tabKeyPressed()
    {
        // Don't do anything when the tab key usage is disabled
        if (!isTabKeyUsageEnabled())
            return false;

        // Check if a container is focused
        if (m_focusedWidget)
        {
            if (m_focusedWidget->m_containerWidget)
            {
                // Focus the next widget in container
                if (std::static_pointer_cast<Container>(m_focusedWidget)->focusNextWidgetInContainer())
                    return true;
            }
        }

        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();

        // Loop all widgets behind the focused one
        for (std::size_t i = focusedWidgetIndex; i < m_widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_widgets[i]->m_allowFocus)
            {
                // Make sure that the widget is visible and enabled
                if ((m_widgets[i]->isVisible()) && (m_widgets[i]->isEnabled()))
                {
                    if (m_focusedWidget)
                    {
                        // unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_focusedWidget = m_widgets[i];
                    m_widgets[i]->m_focused = true;
                    m_widgets[i]->widgetFocused();
                    return true;
                }
            }
        }

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (m_focusedWidget)
        {
            for (std::size_t i = 0; i < focusedWidgetIndex-1; ++i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_widgets[i]->m_allowFocus)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_widgets[i]->isVisible()) && (m_widgets[i]->isEnabled()))
                    {
                        // unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();

                        // Focus on the new widget
                        m_focusedWidget = m_widgets[i];
                        m_widgets[i]->m_focused = true;
                        m_widgets[i]->widgetFocused();
                        return true;
                    }
                }
            }
        }

        // If the currently focused container widget is the only widget to focus, then focus its next child widget
        if (m_focusedWidget && (m_focusedWidget->m_containerWidget))
        {
            std::static_pointer_cast<Container>(m_focusedWidget)->tabKeyPressed();
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::shiftTabKeyPressed()
    {
        // Don't do anything when the tab key usage is disabled
        if (!isTabKeyUsageEnabled())
            return false;

        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();

        if (m_focusedWidget)
        {
            // Check if a container is focused
            if (m_focusedWidget->m_containerWidget)
            {
                // Focus the previous widget in container
                if (std::static_pointer_cast<Container>(m_focusedWidget)->focusPrevWidgetInContainer())
                    return true;
            }

            // Loop all widgets before the focused one
            for (std::size_t i = focusedWidgetIndex - 1; i > 0; --i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_widgets[i-1]->m_allowFocus)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_widgets[i-1]->isVisible()) && (m_widgets[i-1]->isEnabled()))
                    {
                        if (m_focusedWidget)
                        {
                            // Unfocus the current widget
                            m_focusedWidget->m_focused = false;
                            m_focusedWidget->widgetUnfocused();
                        }

                        // Focus on the new widget
                        m_focusedWidget = m_widgets[i-1];
                        m_widgets[i-1]->m_focused = true;
                        m_widgets[i-1]->widgetFocused();
                        return true;
                    }
                }
            }
        }

        // None of the widgets before the focused one could be focused, so loop the ones behind it
        for (std::size_t i = m_widgets.size(); i > focusedWidgetIndex; --i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_widgets[i-1]->m_allowFocus)
            {
                // Make sure that the widget is visible and enabled
                if ((m_widgets[i-1]->isVisible()) && (m_widgets[i-1]->isEnabled()))
                {
                    if (m_focusedWidget)
                    {
                        // Unfocus the current widget
                        m_focusedWidget->m_focused = false;
                        m_focusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_focusedWidget = m_widgets[i-1];
                    m_widgets[i-1]->m_focused = true;
                    m_widgets[i-1]->widgetFocused();
                    return true;
                }
            }
        }

        // If the currently focused container widget is the only widget to focus, then focus its previous child widget
        if (m_focusedWidget && (m_focusedWidget->m_containerWidget))
        {
            std::static_pointer_cast<Container>(m_focusedWidget)->shiftTabKeyPressed();
            return true;
        }

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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiContainer::GuiContainer()
    {
        m_type = "GuiContainer";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::setSize(const Layout2d&)
    {
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
