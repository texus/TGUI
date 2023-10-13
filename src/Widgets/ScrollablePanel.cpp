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


#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Vector2.hpp>
#include <TGUI/Keyboard.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ScrollablePanel::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const char* typeName, bool initRenderer) :
        Panel{typeName, false}
    {
        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ScrollablePanelRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const ScrollablePanel& other) :
        Panel                       {other},
        m_contentSize               {other.m_contentSize},
        m_mostBottomRightPosition   {other.m_mostBottomRightPosition},
        m_verticalScrollbar         {other.m_verticalScrollbar},
        m_horizontalScrollbar       {other.m_horizontalScrollbar},
        m_verticalScrollbarPolicy   {other.m_verticalScrollbarPolicy},
        m_horizontalScrollbarPolicy {other.m_horizontalScrollbarPolicy},
        m_connectedPositionCallbacks{},
        m_connectedSizeCallbacks    {}
    {
        if (m_contentSize == Vector2f{0, 0})
        {
            for (const auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(ScrollablePanel&& other) noexcept :
        Panel                       {std::move(other)},
        m_contentSize               {std::move(other.m_contentSize)},
        m_mostBottomRightPosition   {std::move(other.m_mostBottomRightPosition)},
        m_verticalScrollbar         {std::move(other.m_verticalScrollbar)},
        m_horizontalScrollbar       {std::move(other.m_horizontalScrollbar)},
        m_verticalScrollbarPolicy   {std::move(other.m_verticalScrollbarPolicy)},
        m_horizontalScrollbarPolicy {std::move(other.m_horizontalScrollbarPolicy)},
        m_connectedPositionCallbacks{std::move(other.m_connectedPositionCallbacks)},
        m_connectedSizeCallbacks    {std::move(other.m_connectedSizeCallbacks)}
    {
        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            for (const auto& widget : m_widgets)
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
                for (const auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (ScrollablePanel&& other) noexcept
    {
        if (this != &other)
        {
            m_contentSize               = std::move(other.m_contentSize);
            m_mostBottomRightPosition   = std::move(other.m_mostBottomRightPosition);
            m_verticalScrollbar         = std::move(other.m_verticalScrollbar);
            m_horizontalScrollbar       = std::move(other.m_horizontalScrollbar);
            m_verticalScrollbarPolicy   = std::move(other.m_verticalScrollbarPolicy);
            m_horizontalScrollbarPolicy = std::move(other.m_horizontalScrollbarPolicy);
            Panel::operator=(std::move(other));

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (const auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::create(const Layout2d& size, Vector2f contentSize)
    {
        auto panel = std::make_shared<ScrollablePanel>();
        panel->setSize(size);
        panel->setContentSize(contentSize);
        return panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::copy(const ScrollablePanel::ConstPtr& panel)
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

    void ScrollablePanel::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getAbsolutePosition(Vector2f offset) const
    {
        return Panel::getAbsolutePosition(offset - getContentOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::add(const Widget::Ptr& widget, const String& widgetName)
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
        const auto posCallbackIt = m_connectedPositionCallbacks.find(widget);
        if (posCallbackIt != m_connectedPositionCallbacks.end())
        {
            widget->onPositionChange.disconnect(posCallbackIt->second);
            m_connectedPositionCallbacks.erase(posCallbackIt);
        }

        const auto sizeCallbackIt = m_connectedSizeCallbacks.find(widget);
        if (sizeCallbackIt != m_connectedSizeCallbacks.end())
        {
            widget->onSizeChange.disconnect(sizeCallbackIt->second);
            m_connectedSizeCallbacks.erase(sizeCallbackIt);
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
            for (const auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getInnerSize() const
    {
        Vector2f size = Panel::getInnerSize();
        if (m_verticalScrollbarPolicy == Scrollbar::Policy::Always)
            size.x -= std::min(size.x, getScrollbarWidth());
        if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Always)
            size.y -= std::min(size.y, getScrollbarWidth());
        return size;
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

    float ScrollablePanel::getScrollbarWidth() const
    {
        return m_verticalScrollbar->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy ScrollablePanel::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy ScrollablePanel::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollAmount(unsigned int scrollAmount)
    {
        m_verticalScrollAmount = scrollAmount;

        if (scrollAmount == 0)
            m_verticalScrollbar->setScrollAmount(getGlobalTextSize() * 5);
        else
            m_verticalScrollbar->setScrollAmount(scrollAmount);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getVerticalScrollAmount() const
    {
        return m_verticalScrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollAmount(unsigned int scrollAmount)
    {
        m_horizontalScrollAmount = scrollAmount;

        if (scrollAmount == 0)
        {
            const float horizontalSpeed = 40.f * (static_cast<float>(m_horizontalScrollbar->getMaximum() - m_horizontalScrollbar->getViewportSize()) / m_horizontalScrollbar->getViewportSize());
            m_horizontalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(horizontalSpeed))));
        }
        else
            m_horizontalScrollbar->setScrollAmount(scrollAmount);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getHorizontalScrollAmount() const
    {
        return m_horizontalScrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::isVerticalScrollbarShown() const
    {
        return m_verticalScrollbar->isShown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::isHorizontalScrollbarShown() const
    {
        return m_horizontalScrollbar->isShown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ScrollablePanel::getWidgetAtPosition(Vector2f pos) const
    {
        pos.x += static_cast<float>(m_horizontalScrollbar->getValue());
        pos.y += static_cast<float>(m_verticalScrollbar->getValue());
        return Panel::getWidgetAtPosition(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        bool isDragging = false;
        if (m_verticalScrollbar->isMouseOnWidget(pos - getPosition()))
            isDragging = m_verticalScrollbar->leftMousePressed(pos - getPosition());
        else if (m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()))
            isDragging = m_horizontalScrollbar->leftMousePressed(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            isDragging = Panel::leftMousePressed({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                                  pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseReleased(Vector2f pos)
    {
        if (m_verticalScrollbar->isMouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMouseReleased(pos - getPosition());
        else if (m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()))
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
        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->isMouseOnWidget(pos - getPosition()))
        {
            m_verticalScrollbar->mouseMoved(pos - getPosition());
        }
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()))
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

    bool ScrollablePanel::scrolled(float delta, Vector2f pos, bool touch)
    {
        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));

        Vector2f innerSize = getInnerSize();
        if (verticalScrollbarVisible)
            innerSize.x -= m_verticalScrollbar->getSize().x;
        if (horizontalScrollbarVisible)
            innerSize.y -= m_horizontalScrollbar->getSize().y;

        // If we are scrolling through the panel and it causes the mouse to end up on a scrollable child widget,
        // we should continue scrolling the panel instead of passing the event to the child.
        // We will pass the event to the child if the mouse moved or the last scroll event on the panel was more than a second ago.
        const bool allowChildScrolling = ((m_lastSuccessfulScrollTime == std::chrono::steady_clock::time_point())
                                       || (pos != m_lastSuccessfulScrollPos)
                                       || (Duration{std::chrono::steady_clock::now() - m_lastSuccessfulScrollTime} > Duration{std::chrono::seconds(1)}));

        // First try to pass the scroll event to a child widget
        if (allowChildScrolling
         && FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, innerSize.x, innerSize.y}.contains(pos)
         && Container::scrolled(delta, pos + getContentOffset(), touch))
        {
            m_lastSuccessfulScrollTime = std::chrono::steady_clock::time_point(); // Reset the time as the panel didn't process this event
            return true; // A child widget swallowed the event
        }

        // If the scroll event wasn't handled by a child widget then pass them to the scrollbars in this panel
        bool scrollbarMoved = false;
        if (m_horizontalScrollbar->isShown()
         && !touch
         && (!m_verticalScrollbar->isShown() || m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()) || keyboard::isShiftPressed()))
        {
            scrollbarMoved = m_horizontalScrollbar->scrolled(delta, pos - getPosition(), touch);
        }
        else if (m_verticalScrollbar->isShown())
        {
            scrollbarMoved = m_verticalScrollbar->scrolled(delta, pos - getPosition(), touch);
        }

        if (scrollbarMoved)
        {
            mouseMoved(pos);
            m_lastSuccessfulScrollPos = pos;
            m_lastSuccessfulScrollTime = std::chrono::steady_clock::now();
        }

        return scrollbarMoved;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerOnWidget()
    {
        Panel::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseButtonNoLongerDown()
    {
        Panel::leftMouseButtonNoLongerDown();
        m_verticalScrollbar->leftMouseButtonNoLongerDown();
        m_horizontalScrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ScrollablePanel::askToolTip(Vector2f mousePos)
    {
        if (isMouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset() - getContentOffset();

            Widget::Ptr widget = updateWidgetBelowMouse(mousePos);
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

    void ScrollablePanel::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const auto oldStates = states;

        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        if ((m_roundedBorderRadius > 0) && !m_spriteBackground.isSet())
        {
            target.drawRoundedRectangle(states, getSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached),
                                        m_roundedBorderRadius, m_bordersCached, Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }
        else
        {
            // Draw the borders
            if (m_bordersCached != Borders{0})
            {
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            // Draw the background
            if (m_spriteBackground.isSet())
                target.drawSprite(states, m_spriteBackground);
            else
                target.drawFilledRect(states, innerSize, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
        }

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});
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
            target.addClippingLayer(states, {{}, contentSize});

            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()),
                                        -static_cast<float>(m_verticalScrollbar->getValue())});

            Container::draw(target, states); // NOLINT(bugprone-parent-virtual-call)
            target.removeClippingLayer();
        }

        if (m_verticalScrollbar->isVisible())
            m_verticalScrollbar->draw(target, oldStates);

        if (m_horizontalScrollbar->isVisible())
            m_horizontalScrollbar->draw(target, oldStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::rendererChanged(const String& property)
    {
        if (property == U"Scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (getSharedRenderer()->getScrollbarWidth() == 0)
            {
                const float width = m_verticalScrollbar->getDefaultWidth();
                m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
                updateScrollbars();
            }
        }
        else if (property == U"ScrollbarWidth")
        {
            const float width = (getSharedRenderer()->getScrollbarWidth() != 0) ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
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

        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_verticalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs[U"VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs[U"VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs[U"HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs[U"HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        node->propertyValuePairs[U"ContentSize"] = std::make_unique<DataIO::ValueNode>(U"(" + String::fromNumber(m_contentSize.x) + U", " + String::fromNumber(m_contentSize.y) + U")");
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Panel::load(node, renderers);

        if (node->propertyValuePairs[U"ContentSize"])
            setContentSize(Vector2f{node->propertyValuePairs[U"ContentSize"]->value});

        if (node->propertyValuePairs[U"VerticalScrollbarPolicy"])
        {
            String policy = node->propertyValuePairs[U"VerticalScrollbarPolicy"]->value.trim();
            if (policy == U"Automatic")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == U"Always")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == U"Never")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{U"Failed to parse VerticalScrollbarPolicy property, found unknown value '" + policy + U"'."};
        }

        if (node->propertyValuePairs[U"HorizontalScrollbarPolicy"])
        {
            String policy = node->propertyValuePairs[U"HorizontalScrollbarPolicy"]->value.trim();
            if (policy == U"Automatic")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == U"Always")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == U"Never")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{U"Failed to parse HorizontalScrollbarPolicy property, found unknown value '" + policy + U"'."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::updateScrollbars()
    {
        const Vector2f scrollbarSpace = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                         getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        const Vector2f visibleSize = Panel::getInnerSize();
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
            {
                m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(m_horizontalScrollbar->getViewportSize() - m_verticalScrollbar->getSize().x));
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
            }
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

        if (m_verticalScrollAmount == 0)
            setVerticalScrollAmount(0);

        if (m_horizontalScrollAmount == 0)
            setHorizontalScrollAmount(0);
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

    void ScrollablePanel::connectPositionAndSize(const Widget::Ptr& widget)
    {
        const auto updateFunc = [this]{ recalculateMostBottomRightPosition(); updateScrollbars(); };
        m_connectedPositionCallbacks[widget] = widget->onPositionChange(updateFunc);
        m_connectedSizeCallbacks[widget] = widget->onSizeChange(updateFunc);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::disconnectAllChildWidgets()
    {
        for (const auto& pair : m_connectedPositionCallbacks)
            pair.first->onPositionChange.disconnect(pair.second);
        for (const auto& pair : m_connectedSizeCallbacks)
            pair.first->onSizeChange.disconnect(pair.second);

        m_connectedPositionCallbacks.clear();
        m_connectedSizeCallbacks.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ScrollablePanel::clone() const
    {
        return std::make_shared<ScrollablePanel>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
