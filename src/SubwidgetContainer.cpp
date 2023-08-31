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


#include <TGUI/SubwidgetContainer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SubwidgetContainer::SubwidgetContainer(const char* typeName, bool initRenderer) :
        Widget{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        m_container->setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::setFocused(bool focused)
    {
        m_container->setFocused(focused);
        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);

        for (auto& widget : m_container->getWidgets())
            widget->setEnabled(enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::updateTextSize()
    {
        Widget::updateTextSize();
        m_container->setTextSize(m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container* SubwidgetContainer::getContainer()
    {
        return m_container.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Container* SubwidgetContainer::getContainer() const
    {
        return m_container.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SubwidgetContainer::leftMousePressed(Vector2f pos)
    {
        const bool isDragging = m_container->leftMousePressed(pos - getPosition());
        Widget::leftMousePressed(pos);
        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::leftMouseReleased(Vector2f pos)
    {
        m_container->leftMouseReleased(pos - getPosition());
        Widget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::rightMousePressed(Vector2f pos)
    {
        m_container->rightMousePressed(pos - getPosition());
        Widget::rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::rightMouseReleased(Vector2f pos)
    {
        m_container->rightMouseReleased(pos - getPosition());
        Widget::rightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::mouseMoved(Vector2f pos)
    {
        m_container->mouseMoved(pos - getPosition());
        Widget::mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::keyPressed(const Event::KeyEvent& event)
    {
        m_container->keyPressed(event);
        Widget::keyPressed(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::textEntered(char32_t key)
    {
        m_container->textEntered(key);
        Widget::textEntered(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SubwidgetContainer::scrolled(float delta, Vector2f pos, bool touch)
    {
        m_container->scrolled(delta, pos - getPosition(), touch);
        return Widget::scrolled(delta, pos, touch);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::mouseNoLongerOnWidget()
    {
        m_container->mouseNoLongerOnWidget();
        Widget::mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::leftMouseButtonNoLongerDown()
    {
        m_container->leftMouseButtonNoLongerDown();
        Widget::leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::rightMouseButtonNoLongerDown()
    {
        m_container->rightMouseButtonNoLongerDown();
        Widget::rightMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr SubwidgetContainer::askToolTip(Vector2f mousePos)
    {
        auto toolTip = m_container->askToolTip(mousePos - getPosition());
        if (toolTip)
            return toolTip;

        if (m_toolTip)
            return getToolTip();

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::rendererChanged(const String& property)
    {
        // If the property matches the name of a child widget then the value should be a renderer object to be passed to that widget
        for (const auto& widget : m_container->getWidgets())
        {
            const String& name = widget->getWidgetName();
            if (!name.empty() && (name == property))
            {
                auto propertyValue = getSharedRenderer()->getProperty(property);
                if (propertyValue.getType() != ObjectConverter::Type::None)
                    widget->setRenderer(propertyValue.getRenderer());
                else
                    widget->setRenderer(Theme::getDefault()->getRendererNoThrow(widget->getWidgetType()));

                return;
            }
        }

        // If the property starts with "WidgetName." then the part behind the dot is the property name for that widget
        const auto dotPos = property.find(U'.');
        if (dotPos != String::npos)
        {
            const String& nameToSearch = property.substr(0, dotPos);
            const String& propertyForChild = property.substr(dotPos + 1);
            for (const auto& widget : m_container->getWidgets())
            {
                const String& name = widget->getWidgetName();
                if (!name.empty() && (name == nameToSearch))
                {
                    widget->getRenderer()->setProperty(propertyForChild, getSharedRenderer()->getProperty(property));
                    return;
                }
            }
        }

        Widget::rendererChanged(property);

        // Make sure to pass on the SubwidgetContainer's font to the internal container
        if (property == U"Font")
        {
            m_container->setInheritedFont(m_fontCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> SubwidgetContainer::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        // Find the renderer node
        std::vector<std::unique_ptr<DataIO::Node>>::iterator rendererNodeIt;
        const String& rendererName = renderers.at(this).second; // Empty if the renderer isn't shared
        if (rendererName.empty()) // We can't check renderers.at(this).first as Widget::save moved the value
        {
            // If the renderer has no name then it isn't shared and we should find the renderer among the child nodes
            rendererNodeIt = std::find_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
                return child->name == U"Renderer";
            });
            TGUI_ASSERT(rendererNodeIt != node->children.end(), "SubwidgetContainer relies on Widget::save saving the renderer");
        }
        else // The renderer is shared with other widgets, we need to look for it in global scope
        {
            DataIO::Node* rootNode = node.get();
            while (rootNode->parent)
                rootNode = node->parent;

            rendererNodeIt = std::find_if(rootNode->children.begin(), rootNode->children.end(), [&](const std::unique_ptr<DataIO::Node>& child){
                return child->name == U"Renderer" + rendererName;
            });
            TGUI_ASSERT(rendererNodeIt != rootNode->children.end(), "SubwidgetContainer relies on the renderer being saved already");
        }

        // Remove all renderer properties that were specified for subwidgets, as they will be saved as part of the child widget
        for (const auto& widget : m_container->getWidgets())
        {
            const String& widgetName = widget->getWidgetName();
            if (widgetName.empty())
                continue;

            auto& propertyValuePairs = (*rendererNodeIt)->propertyValuePairs;
            auto propertyIt = propertyValuePairs.begin();
            while (propertyIt != propertyValuePairs.end())
            {
                // Search for properties that either match the widget name or start with the name followed by a dot
                if ((!propertyIt->first.starts_with(widgetName))
                 || ((propertyIt->first.length() > widgetName.length()) && (propertyIt->first[widgetName.length()] != U'.'))
                 || (propertyIt->first != widgetName))
                {
                    ++propertyIt;
                    continue;
                }

                propertyIt = propertyValuePairs.erase(propertyIt);
            }

            auto& children = (*rendererNodeIt)->children;
            auto childIt = children.begin();
            while (childIt != children.end())
            {
                // Search for children that either match the widget name or start with the name followed by a dot
                const auto& childName = (*childIt)->name;
                if ((!childName.starts_with(widgetName))
                 || ((childName.length() > widgetName.length()) && (childName[widgetName.length()] != U'.'))
                 || (childName != widgetName))
                {
                    ++childIt;
                    continue;
                }

                childIt = children.erase(childIt);
            }
        }

        node->children.emplace_back(m_container->save(renderers));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        m_container->removeAllWidgets();

        Widget::load(node, renderers);
        if (node->children.size() == 1)
        {
            m_container->load(node->children[0], renderers);
            m_container->setSize(getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SubwidgetContainer::updateTime(Duration elapsedTime)
    {
        return m_container->updateTime(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SubwidgetContainer::isMouseOnWidget(Vector2f pos) const
    {
        return m_container->isMouseOnWidget(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::draw(BackendRenderTarget& target, RenderStates states) const
    {
        m_container->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubwidgetContainer::setParent(Container* parent)
    {
        Widget::setParent(parent);
        m_container->setParentGui(m_parentGui);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
