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


#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/SignalImpl.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const Layout2d& size, Vector2f contentSize) :
        Panel{size}
    {
        m_type = "ScrollablePanel";

        m_renderer = aurora::makeCopied<ScrollablePanelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        setContentSize(contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const ScrollablePanel& other) :
        Panel                      {other},
        m_contentSize              {other.m_contentSize},
        m_mostBottomRightPosition  {other.m_mostBottomRightPosition},
        m_verticalScrollbar        {other.m_verticalScrollbar},
        m_horizontalScrollbar      {other.m_horizontalScrollbar},
        m_verticalScrollbarPolicy  {other.m_verticalScrollbarPolicy},
        m_horizontalScrollbarPolicy{other.m_horizontalScrollbarPolicy},
        m_connectedCallbacks       {}
    {
        if (m_contentSize == Vector2f{0, 0})
        {
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(ScrollablePanel&& other) :
        Panel                      {std::move(other)},
        m_contentSize              {std::move(other.m_contentSize)},
        m_mostBottomRightPosition  {std::move(other.m_mostBottomRightPosition)},
        m_verticalScrollbar        {std::move(other.m_verticalScrollbar)},
        m_horizontalScrollbar      {std::move(other.m_horizontalScrollbar)},
        m_verticalScrollbarPolicy  {std::move(other.m_verticalScrollbarPolicy)},
        m_horizontalScrollbarPolicy{std::move(other.m_horizontalScrollbarPolicy)},
        m_connectedCallbacks       {std::move(other.m_connectedCallbacks)}
    {
        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (const ScrollablePanel& other)
    {
        if (this != &other)
        {
            Panel::operator=(other);
            m_contentSize               = other.m_contentSize;
            m_mostBottomRightPosition   = other.m_mostBottomRightPosition;
            m_verticalScrollbar         = other.m_verticalScrollbar;
            m_horizontalScrollbar       = other.m_horizontalScrollbar;
            m_verticalScrollbarPolicy   = other.m_verticalScrollbarPolicy;
            m_horizontalScrollbarPolicy = other.m_horizontalScrollbarPolicy;

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (ScrollablePanel&& other)
    {
        if (this != &other)
        {
            Panel::operator=(std::move(other));
            m_contentSize               = std::move(other.m_contentSize);
            m_mostBottomRightPosition   = std::move(other.m_mostBottomRightPosition);
            m_verticalScrollbar         = std::move(other.m_verticalScrollbar);
            m_horizontalScrollbar       = std::move(other.m_horizontalScrollbar);
            m_verticalScrollbarPolicy   = std::move(other.m_verticalScrollbarPolicy);
            m_horizontalScrollbarPolicy = std::move(other.m_horizontalScrollbarPolicy);

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::create(Layout2d size, Vector2f contentSize)
    {
        return std::make_shared<ScrollablePanel>(size, contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::copy(ScrollablePanel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<ScrollablePanel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanelRenderer* ScrollablePanel::getSharedRenderer()
    {
        return aurora::downcast<ScrollablePanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollablePanelRenderer* ScrollablePanel::getSharedRenderer() const
    {
        return aurora::downcast<const ScrollablePanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanelRenderer* ScrollablePanel::getRenderer()
    {
        return aurora::downcast<ScrollablePanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollablePanelRenderer* ScrollablePanel::getRenderer() const
    {
        return aurora::downcast<const ScrollablePanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getAbsolutePosition() const
    {
        return Panel::getAbsolutePosition() - getContentOffset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::add(const Widget::Ptr& widget, const sf::String& widgetName)
    {
        Panel::add(widget, widgetName);

        if (m_contentSize == Vector2f{0, 0})
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;

            updateScrollbars();

            connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::remove(const Widget::Ptr& widget)
    {
        const auto callbackIt = m_connectedCallbacks.find(widget);
        if (callbackIt != m_connectedCallbacks.end())
        {
            widget->disconnect(callbackIt->second-1);
            widget->disconnect(callbackIt->second);
            m_connectedCallbacks.erase(callbackIt);
        }

        const bool ret = Panel::remove(widget);

        if (m_contentSize == Vector2f{0, 0})
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if ((bottomRight.x == m_mostBottomRightPosition.x) || (bottomRight.y == m_mostBottomRightPosition.y))
            {
                recalculateMostBottomRightPosition();
                updateScrollbars();
            }
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::removeAllWidgets()
    {
        disconnectAllChildWidgets();

        Panel::removeAllWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            recalculateMostBottomRightPosition();
            updateScrollbars();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setContentSize(Vector2f size)
    {
        m_contentSize = size;

        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            recalculateMostBottomRightPosition();

            // Automatically recalculate the bottom right position when the position or size of a widget changes
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getContentSize() const
    {
        if (m_contentSize != Vector2f{0, 0})
            return m_contentSize;
        else if (m_widgets.empty())
            return getInnerSize();
        else
            return m_mostBottomRightPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getContentOffset() const
    {
        return {static_cast<float>(m_horizontalScrollbar->getValue()), static_cast<float>(m_verticalScrollbar->getValue())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setScrollbarWidth(float width)
    {
        m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
        m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ScrollablePanel::getScrollbarWidth() const
    {
        return m_verticalScrollbar->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollbarPolicy(ScrollbarPolicy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == ScrollbarPolicy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == ScrollbarPolicy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // ScrollbarPolicy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollbarPolicy ScrollablePanel::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollbarPolicy(ScrollbarPolicy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == ScrollbarPolicy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == ScrollbarPolicy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // ScrollbarPolicy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollbarPolicy ScrollablePanel::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        if (m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMousePressed(pos - getPosition());
        else if (m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar->leftMousePressed(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMousePressed({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                     pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseReleased(Vector2f pos)
    {
        if (m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMouseReleased(pos - getPosition());
        else if (m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMouseReleased({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                      pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseMoved(Vector2f pos)
    {
        // Check if the mouse event should go to the scrollbar
        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_verticalScrollbar->mouseMoved(pos - getPosition());
        }
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar->mouseMoved(pos - getPosition());
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
            {
                Panel::mouseMoved({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                   pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
            }

            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::mouseWheelScrolled(float delta, Vector2f pos)
    {
        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));

        sf::Vector2f innerSize = getInnerSize();
        if (verticalScrollbarVisible)
            innerSize.x -= m_verticalScrollbar->getSize().x;
        if (horizontalScrollbarVisible)
            innerSize.y -= m_horizontalScrollbar->getSize().y;

        if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, innerSize.x, innerSize.y}.contains(pos))
        {
            if (Container::mouseWheelScrolled(delta, pos + getContentOffset()))
                return true; // A child widget swallowed the event
        }

        if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }

        return true; // We swallowed the event
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerOnWidget()
    {
        Panel::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerDown()
    {
        Panel::mouseNoLongerDown();
        m_verticalScrollbar->mouseNoLongerDown();
        m_horizontalScrollbar->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ScrollablePanel::askToolTip(Vector2f mousePos)
    {
        if (mouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset() - getContentOffset();

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

    void ScrollablePanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const auto oldStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
        drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
        Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        if (m_verticalScrollbar->isVisible() && (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()))
            contentSize.x -= m_verticalScrollbar->getSize().x;
        if (m_horizontalScrollbar->isVisible() && (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()))
            contentSize.y -= m_horizontalScrollbar->getSize().y;

        // If the content size is manually specified and smaller than the panel itself, then use it for clipping
        if ((m_contentSize.x > 0) && (contentSize.x > m_contentSize.x))
            contentSize.x = m_contentSize.x;
        if ((m_contentSize.y > 0) && (contentSize.y > m_contentSize.y))
            contentSize.y = m_contentSize.y;

        // Draw the child widgets
        {
            const Clipping clipping{target, states, {}, contentSize};

            states.transform.translate(-static_cast<float>(m_horizontalScrollbar->getValue()),
                                       -static_cast<float>(m_verticalScrollbar->getValue()));

            drawWidgetContainer(&target, states);
        }

        if (m_verticalScrollbar->isVisible())
            m_verticalScrollbar->draw(target, oldStates);

        if (m_horizontalScrollbar->isVisible())
            m_horizontalScrollbar->draw(target, oldStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::rendererChanged(const std::string& property)
    {
        if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            updateScrollbars();
        }
        else
            Panel::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ScrollablePanel::save(SavingRenderersMap& renderers) const
    {
        auto node = Panel::save(renderers);

        if (m_verticalScrollbarPolicy != ScrollbarPolicy::Automatic)
        {
            if (m_verticalScrollbarPolicy == ScrollbarPolicy::Always)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == ScrollbarPolicy::Never)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != ScrollbarPolicy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == ScrollbarPolicy::Always)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == ScrollbarPolicy::Never)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        node->propertyValuePairs["ContentSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_contentSize.x) + ", " + to_string(m_contentSize.y) + ")");
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Panel::load(node, renderers);

        if (node->propertyValuePairs["contentsize"])
            setContentSize(Vector2f{node->propertyValuePairs["contentsize"]->value});

        if (node->propertyValuePairs["verticalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["verticalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setVerticalScrollbarPolicy(ScrollbarPolicy::Automatic);
            else if (policy == "always")
                setVerticalScrollbarPolicy(ScrollbarPolicy::Always);
            else if (policy == "never")
                setVerticalScrollbarPolicy(ScrollbarPolicy::Never);
            else
                throw Exception{"Failed to parse VerticalScrollbarPolicy property, found unknown value."};
        }

        if (node->propertyValuePairs["horizontalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["horizontalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setHorizontalScrollbarPolicy(ScrollbarPolicy::Automatic);
            else if (policy == "always")
                setHorizontalScrollbarPolicy(ScrollbarPolicy::Always);
            else if (policy == "never")
                setHorizontalScrollbarPolicy(ScrollbarPolicy::Never);
            else
                throw Exception{"Failed to parse HorizontalScrollbarPolicy property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::updateScrollbars()
    {
        const Vector2f scrollbarSpace = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                         getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        const Vector2f visibleSize = getInnerSize();
        m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(visibleSize.x));
        m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(visibleSize.y));

        const Vector2f contentSize = getContentSize();
        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(contentSize.x));
        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(contentSize.y));

        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        if (horizontalScrollbarVisible)
        {
            m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y - m_horizontalScrollbar->getSize().y);
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(m_verticalScrollbar->getViewportSize() - m_horizontalScrollbar->getSize().y));

            const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));
            if (verticalScrollbarVisible)
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
            else
                m_horizontalScrollbar->setSize(scrollbarSpace.x, m_horizontalScrollbar->getSize().y);
        }
        else
        {
            m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y);

            const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));
            if (verticalScrollbarVisible)
            {
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
                m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(m_horizontalScrollbar->getViewportSize() - m_verticalScrollbar->getSize().x));

                if (m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize())))
                    m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y - m_horizontalScrollbar->getSize().y);
            }
            else
                m_horizontalScrollbar->setSize(scrollbarSpace.x, m_horizontalScrollbar->getSize().y);
        }

        m_verticalScrollbar->setPosition(m_bordersCached.getLeft() + scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), m_bordersCached.getTop() + scrollbarSpace.y - m_horizontalScrollbar->getSize().y);

        const float verticalSpeed = 40.f * (static_cast<float>(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize()) / m_verticalScrollbar->getViewportSize());
        m_verticalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(verticalSpeed))));

        const float horizontalSpeed = 40.f * (static_cast<float>(m_horizontalScrollbar->getMaximum() - m_horizontalScrollbar->getViewportSize()) / m_horizontalScrollbar->getViewportSize());
        m_horizontalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(horizontalSpeed))));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::recalculateMostBottomRightPosition()
    {
        m_mostBottomRightPosition = {0, 0};

        for (const auto& widget : m_widgets)
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::connectPositionAndSize(Widget::Ptr widget)
    {
        m_connectedCallbacks[widget] = widget->connect({"PositionChanged", "SizeChanged"}, [this](){ recalculateMostBottomRightPosition(); updateScrollbars(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::disconnectAllChildWidgets()
    {
        for (const auto& pair : m_connectedCallbacks)
        {
            pair.first->disconnect(pair.second-1);
            pair.first->disconnect(pair.second);
        }

        m_connectedCallbacks.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
