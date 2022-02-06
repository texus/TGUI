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

#include <TGUI/GuiBase.hpp>
#include <TGUI/Backend.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Timer.hpp>
#include <TGUI/BackendRenderTarget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiBase::~GuiBase()
    {
        if (m_container) // Always the case, unless derived class throws exception in constructor while creating backend
        {
            m_container->setParentGui(nullptr);
            getBackend()->detatchGui(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setAbsoluteViewport(const FloatRect& viewport)
    {
        m_viewport = {viewport.left, viewport.top, viewport.width, viewport.height};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setRelativeViewport(const FloatRect& viewport)
    {
        m_viewport = {RelativeValue(viewport.left), RelativeValue(viewport.top), RelativeValue(viewport.width), RelativeValue(viewport.height)};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RelFloatRect GuiBase::getViewport() const
    {
        return m_viewport;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setAbsoluteView(const FloatRect& view)
    {
        m_view = {view.left, view.top, view.width, view.height};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setRelativeView(const FloatRect& view)
    {
        m_view = {RelativeValue(view.left), RelativeValue(view.top), RelativeValue(view.width), RelativeValue(view.height)};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RelFloatRect GuiBase::getView() const
    {
        return m_view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::handleEvent(Event event)
    {
        switch (event.type)
        {
            case Event::Type::MouseMoved:
            case Event::Type::MouseButtonPressed:
            case Event::Type::MouseButtonReleased:
            case Event::Type::MouseWheelScrolled:
            {
                Vector2f mouseCoords;
                if (event.type == Event::Type::MouseMoved)
                    mouseCoords = mapPixelToView(event.mouseMove.x, event.mouseMove.y);
                else if (event.type == Event::Type::MouseWheelScrolled)
                    mouseCoords = mapPixelToView(event.mouseWheel.x, event.mouseWheel.y);
                else // if ((event.type == Event::Type::MouseButtonPressed) || (event.type == Event::Type::MouseButtonReleased))
                    mouseCoords = mapPixelToView(event.mouseButton.x, event.mouseButton.y);

                // If a tooltip is visible then hide it now
                if (m_visibleToolTip != nullptr)
                {
                    // Correct the position of the tool tip so that it is relative again
                    m_visibleToolTip->setPosition(m_toolTipRelativePos);

                    remove(m_visibleToolTip);
                    m_visibleToolTip = nullptr;
                }

                // Reset the data for the tooltip since the mouse has moved
                m_tooltipTime = {};
                m_tooltipPossible = true;
                m_lastMousePos = mouseCoords;

                if (event.type == Event::Type::MouseMoved)
                    return m_container->processMouseMoveEvent(mouseCoords);
                else if (event.type == Event::Type::MouseWheelScrolled)
                    return m_container->processMouseWheelScrollEvent(event.mouseWheel.delta, mouseCoords);
                else if (event.type == Event::Type::MouseButtonPressed)
                    return m_container->processMousePressEvent(event.mouseButton.button, mouseCoords);
                else // if (event.type == Event::Type::MouseButtonReleased)
                {
                    const bool eventHandled = m_container->processMouseReleaseEvent(event.mouseButton.button, mouseCoords);
                    if (event.mouseButton.button == Event::MouseButton::Left)
                        m_container->leftMouseButtonNoLongerDown();
                    else if (event.mouseButton.button == Event::MouseButton::Right)
                        m_container->rightMouseButtonNoLongerDown();
                    return eventHandled;
                }
            }
            case Event::Type::KeyPressed:
            {
                if (isTabKeyUsageEnabled() && (event.key.code == Event::KeyboardKey::Tab))
                {
                    if (event.key.shift)
                        focusPreviousWidget(true);
                    else
                        focusNextWidget(true);

                    return true;
                }
                else
                    return m_container->processKeyPressEvent(event.key);
            }
            case Event::Type::TextEntered:
            {
                return m_container->processTextEnteredEvent(event.text.unicode);
            }
            case Event::Type::LostFocus:
            {
                m_windowFocused = false;
                break;
            }
            case Event::Type::GainedFocus:
            {
                m_windowFocused = true;
                break;
            }
            case Event::Type::Resized:
            {
                updateContainerSize();
                break;
            }
            case Event::Type::Closed:
            {
                break;
            }
        }

        // The event wasn't absorbed by the gui
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setTabKeyUsageEnabled(bool enabled)
    {
        m_tabKeyUsageEnabled = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::isTabKeyUsageEnabled() const
    {
        return m_tabKeyUsageEnabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RootContainer::Ptr GuiBase::getContainer() const
    {
        return m_container;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setFont(const Font& font)
    {
        m_container->setInheritedFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font GuiBase::getFont() const
    {
       if (m_container->getInheritedFont())
          return m_container->getInheritedFont();
        else
            return Font::getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Widget::Ptr>& GuiBase::getWidgets() const
    {
        return m_container->getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::add(const Widget::Ptr& widgetPtr, const String& widgetName)
    {
        m_container->add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GuiBase::get(const String& widgetName) const
    {
        return m_container->get(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::remove(const Widget::Ptr& widget)
    {
        return m_container->remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::removeAllWidgets()
    {
        m_container->removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GuiBase::getFocusedChild() const
    {
        return m_container->getFocusedChild();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GuiBase::getFocusedLeaf() const
    {
        return m_container->getFocusedLeaf();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GuiBase::getWidgetAtPosition(Vector2f pos) const
    {
        return m_container->getWidgetAtPosition(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GuiBase::getWidgetBelowMouseCursor(Vector2i mousePos) const
    {
        return getWidgetAtPosition(mapPixelToView(mousePos.x, mousePos.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::focusNextWidget(bool recursive)
    {
        return m_container->focusNextWidget(recursive);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::focusPreviousWidget(bool recursive)
    {
        return m_container->focusPreviousWidget(recursive);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::unfocusAllWidgets()
    {
        m_container->setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::moveWidgetToFront(const Widget::Ptr& widget)
    {
        m_container->moveWidgetToFront(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::moveWidgetToBack(const Widget::Ptr& widget)
    {
        m_container->moveWidgetToBack(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t GuiBase::moveWidgetForward(const Widget::Ptr& widget)
    {
        return m_container->moveWidgetForward(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t GuiBase::moveWidgetBackward(const Widget::Ptr& widget)
    {
        return m_container->moveWidgetBackward(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::setWidgetIndex(const Widget::Ptr& widget, std::size_t index)
    {
        return m_container->setWidgetIndex(widget, index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int GuiBase::getWidgetIndex(const Widget::Ptr& widget) const
    {
        return m_container->getWidgetIndex(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setOpacity(float opacity)
    {
        m_container->setInheritedOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float GuiBase::getOpacity() const
    {
        return m_container->getInheritedOpacity();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setTextSize(unsigned int size)
    {
        m_container->setTextSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int GuiBase::getTextSize() const
    {
        return m_container->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::loadWidgetsFromFile(const String& filename, bool replaceExisting)
    {
        m_container->loadWidgetsFromFile(filename, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::saveWidgetsToFile(const String& filename)
    {
        m_container->saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::loadWidgetsFromStream(std::stringstream& stream, bool replaceExisting)
    {
        m_container->loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::loadWidgetsFromStream(std::stringstream&& stream, bool replaceExisting)
    {
        loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::saveWidgetsToStream(std::stringstream& stream) const
    {
        m_container->saveWidgetsToStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setOverrideMouseCursor(Cursor::Type type)
    {
        m_overrideMouseCursors.push(type);
        getBackend()->setMouseCursor(this, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::restoreOverrideMouseCursor()
    {
        if (m_overrideMouseCursors.empty())
            return;

        m_overrideMouseCursors.pop();

        const Cursor::Type newCursor = m_overrideMouseCursors.empty() ? m_requestedMouseCursor : m_overrideMouseCursors.top();
        getBackend()->setMouseCursor(this, newCursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::requestMouseCursor(Cursor::Type type)
    {
        if (type == m_requestedMouseCursor)
            return;

        m_requestedMouseCursor = type;
        if (m_overrideMouseCursors.empty())
            getBackend()->setMouseCursor(this, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::setDrawingUpdatesTime(bool drawUpdatesTime)
    {
        m_drawUpdatesTime = drawUpdatesTime;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::updateTime()
    {
        const auto timePointNow = std::chrono::steady_clock::now();

        bool screenRefreshRequired = false;
        if (m_lastUpdateTime > decltype(m_lastUpdateTime){})
            screenRefreshRequired = updateTime(timePointNow - m_lastUpdateTime);

        m_lastUpdateTime = timePointNow;
        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiBase::updateTime(Duration elapsedTime)
    {
        bool screenRefreshRequired = Timer::updateTime(elapsedTime);

        if (!m_windowFocused)
            return screenRefreshRequired;

        screenRefreshRequired |= m_container->updateTime(elapsedTime);

        if (m_tooltipPossible)
        {
            m_tooltipTime += elapsedTime;
            if (m_tooltipTime >= ToolTip::getInitialDelay())
            {
                Widget::Ptr tooltip = m_container->askToolTip(m_lastMousePos);
                if (tooltip)
                {
                    m_visibleToolTip = tooltip;
                    add(tooltip, "#TGUI_INTERNAL$ToolTip#");

                    // Remember the position of the tool tip, since we need to restore it when hiding it
                    m_toolTipRelativePos = tooltip->getPosition();

                    // Convert the relative tool tip position in an absolute one
                    Vector2f pos = m_lastMousePos + ToolTip::getDistanceToMouse() + tooltip->getPosition();

                    // Don't display the tool tip outside the window, unless it doesn't fit at all
                    if (pos.x + tooltip->getSize().x > m_view.getLeft() + m_view.getWidth())
                        pos.x = std::max(m_view.getLeft(), m_view.getLeft() + m_view.getWidth() - tooltip->getSize().x);
                    if (pos.y + tooltip->getSize().y > m_view.getTop() + m_view.getHeight())
                        pos.y = std::max(m_view.getTop(), m_view.getTop() + m_view.getHeight() - tooltip->getSize().y);

                    tooltip->setPosition(pos);
                    screenRefreshRequired = true;
                }

                m_tooltipPossible = false;
            }
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f GuiBase::mapPixelToView(int x, int y) const
    {
        return {((x - m_viewport.getLeft()) * (m_view.getWidth() / m_viewport.getWidth())) + m_view.getLeft(),
                ((y - m_viewport.getTop()) * (m_view.getHeight() / m_viewport.getHeight())) + m_view.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::init()
    {
        getBackend()->attachGui(this);

        m_container = std::make_shared<RootContainer>();
        m_container->setParentGui(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiBase::updateContainerSize()
    {
        m_container->setSize(Vector2f{m_view.getWidth(), m_view.getHeight()});

        // Derived classes should update m_view in their updateContainerSize() function before calling this function
        const FloatRect& viewRect = m_view.getRect();
        if (viewRect != m_lastView)
        {
            m_lastView = viewRect;
            onViewChange.emit(m_container.get(), viewRect);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
