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

#include <TGUI/Backend/Window/BackendGui.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Backend/Renderer/BackendRenderTarget.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Timer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGui::BackendGui()
    {
        m_container->setParentGui(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGui::~BackendGui()
    {
        m_container->setParentGui(nullptr);

        // Destroy the container and its widgets before destroying the backend.
        // This solves issues such as TextArea still accessing the backend when it gets unfocused (by removing it from its parent).
        // We first explictly destroy the widgets before destroying the container, to handle the case where an onUnfocus signal
        // is still being triggered that attempts to access the gui (and it's container).
        m_container->removeAllWidgets();
        m_container = nullptr;

        if (isBackendSet())
            getBackend()->detatchGui(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setAbsoluteViewport(const FloatRect& viewport)
    {
        m_viewport = {viewport.left, viewport.top, viewport.width, viewport.height};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setRelativeViewport(const FloatRect& viewport)
    {
        m_viewport = {RelativeValue(viewport.left), RelativeValue(viewport.top), RelativeValue(viewport.width), RelativeValue(viewport.height)};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RelFloatRect BackendGui::getViewport() const
    {
        return m_viewport;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setAbsoluteView(const FloatRect& view)
    {
        m_view = {view.left, view.top, view.width, view.height};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setRelativeView(const FloatRect& view)
    {
        m_view = {RelativeValue(view.left), RelativeValue(view.top), RelativeValue(view.width), RelativeValue(view.height)};
        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RelFloatRect BackendGui::getView() const
    {
        return m_view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::handleEvent(Event event)
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
                {
                    m_lastMousePos = {event.mouseMove.x, event.mouseMove.y};
                    mouseCoords = mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
                }
                else if (event.type == Event::Type::MouseWheelScrolled)
                {
                    m_lastMousePos = {event.mouseWheel.x, event.mouseWheel.y};
                    mouseCoords = mapPixelToCoords({event.mouseWheel.x, event.mouseWheel.y});
                }
                else // if ((event.type == Event::Type::MouseButtonPressed) || (event.type == Event::Type::MouseButtonReleased))
                {
                    m_lastMousePos = {event.mouseButton.x, event.mouseButton.y};
                    mouseCoords = mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                }

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

                if (event.type == Event::Type::MouseMoved)
                    return m_container->processMouseMoveEvent(mouseCoords);
                else if (event.type == Event::Type::MouseWheelScrolled)
                {
                    if (m_container->processScrollEvent(event.mouseWheel.delta, mouseCoords, false))
                        return true;

                    // Even if no scrollbar moved, we will still absorb the scroll event when the mouse is on top of a widget
                    return m_container->getWidgetAtPosition(mouseCoords) != nullptr;
                }
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
            case Event::Type::MouseLeft:
            {
                m_container->mouseNoLongerOnWidget();
                break;
            }
            case Event::Type::MouseEntered:
            case Event::Type::Closed:
            {
                // We don't do anything with these events
                break;
            }
        }

        // The event wasn't absorbed by the gui
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setTabKeyUsageEnabled(bool enabled)
    {
        m_tabKeyUsageEnabled = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::isTabKeyUsageEnabled() const
    {
        return m_tabKeyUsageEnabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::draw()
    {
        if (m_drawUpdatesTime)
            updateTime();

        TGUI_ASSERT(m_backendRenderTarget != nullptr, "Gui must be given a window (or render target) before calling draw()");
        m_backendRenderTarget->drawGui(m_container);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RootContainer::Ptr BackendGui::getContainer() const
    {
        return m_container;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendRenderTarget> BackendGui::getBackendRenderTarget() const
    {
        return m_backendRenderTarget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setFont(const Font& font)
    {
        m_container->setInheritedFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font BackendGui::getFont() const
    {
       if (m_container->getInheritedFont())
          return m_container->getInheritedFont();
        else
            return Font::getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Widget::Ptr>& BackendGui::getWidgets() const
    {
        return m_container->getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::add(const Widget::Ptr& widgetPtr, const String& widgetName)
    {
        m_container->add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BackendGui::get(const String& widgetName) const
    {
        return m_container->get(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::remove(const Widget::Ptr& widget)
    {
        return m_container->remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::removeAllWidgets()
    {
        m_container->removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BackendGui::getFocusedChild() const
    {
        return m_container->getFocusedChild();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BackendGui::getFocusedLeaf() const
    {
        return m_container->getFocusedLeaf();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BackendGui::getWidgetAtPosition(Vector2f pos) const
    {
        return m_container->getWidgetAtPosition(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BackendGui::getWidgetBelowMouseCursor(Vector2i mousePos) const
    {
        return getWidgetAtPosition(mapPixelToCoords(mousePos));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::focusNextWidget(bool recursive)
    {
        return m_container->focusNextWidget(recursive);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::focusPreviousWidget(bool recursive)
    {
        return m_container->focusPreviousWidget(recursive);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::unfocusAllWidgets()
    {
        m_container->setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::moveWidgetToFront(const Widget::Ptr& widget)
    {
        m_container->moveWidgetToFront(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::moveWidgetToBack(const Widget::Ptr& widget)
    {
        m_container->moveWidgetToBack(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t BackendGui::moveWidgetForward(const Widget::Ptr& widget)
    {
        return m_container->moveWidgetForward(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t BackendGui::moveWidgetBackward(const Widget::Ptr& widget)
    {
        return m_container->moveWidgetBackward(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::setWidgetIndex(const Widget::Ptr& widget, std::size_t index)
    {
        return m_container->setWidgetIndex(widget, index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int BackendGui::getWidgetIndex(const Widget::Ptr& widget) const
    {
        return m_container->getWidgetIndex(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setOpacity(float opacity)
    {
        m_container->setInheritedOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendGui::getOpacity() const
    {
        return m_container->getInheritedOpacity();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setTextSize(unsigned int size)
    {
        m_container->setTextSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendGui::getTextSize() const
    {
        return m_container->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::loadWidgetsFromFile(const String& filename, bool replaceExisting)
    {
        m_container->loadWidgetsFromFile(filename, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::saveWidgetsToFile(const String& filename)
    {
        m_container->saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::loadWidgetsFromStream(std::stringstream& stream, bool replaceExisting)
    {
        m_container->loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::loadWidgetsFromStream(std::stringstream&& stream, bool replaceExisting)
    {
        loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::saveWidgetsToStream(std::stringstream& stream) const
    {
        m_container->saveWidgetsToStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setOverrideMouseCursor(Cursor::Type type)
    {
        m_overrideMouseCursors.push(type);
        getBackend()->setMouseCursor(this, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::restoreOverrideMouseCursor()
    {
        if (m_overrideMouseCursors.empty())
            return;

        m_overrideMouseCursors.pop();

        const Cursor::Type newCursor = m_overrideMouseCursors.empty() ? m_requestedMouseCursor : m_overrideMouseCursors.top();
        getBackend()->setMouseCursor(this, newCursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::requestMouseCursor(Cursor::Type type)
    {
        if (type == m_requestedMouseCursor)
            return;

        m_requestedMouseCursor = type;
        if (m_overrideMouseCursors.empty())
            getBackend()->setMouseCursor(this, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setDrawingUpdatesTime(bool drawUpdatesTime)
    {
        m_drawUpdatesTime = drawUpdatesTime;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::updateTime()
    {
        const auto timePointNow = std::chrono::steady_clock::now();

        bool screenRefreshRequired = false;
        if (m_lastUpdateTime > std::chrono::steady_clock::time_point())
            screenRefreshRequired = updateTime(timePointNow - m_lastUpdateTime);

        m_lastUpdateTime = timePointNow;
        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::updateTime(Duration elapsedTime)
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
                const Vector2f lastMousePos = mapPixelToCoords(m_lastMousePos);
                Widget::Ptr tooltip = m_container->askToolTip(lastMousePos);
                if (tooltip)
                {
                    m_visibleToolTip = tooltip;
                    add(tooltip, "#TGUI_INTERNAL$ToolTip#");

                    // Remember the position of the tool tip, since we need to restore it when hiding it
                    m_toolTipRelativePos = tooltip->getPosition();

                    // Convert the relative tool tip position in an absolute one
                    Vector2f pos = lastMousePos + ToolTip::getDistanceToMouse() + tooltip->getPosition();

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

    Vector2f BackendGui::mapPixelToCoords(Vector2i pixel) const
    {
        return {((pixel.x - m_viewport.getLeft()) * (m_view.getWidth() / m_viewport.getWidth())) + m_view.getLeft(),
                ((pixel.y - m_viewport.getTop()) * (m_view.getHeight() / m_viewport.getHeight())) + m_view.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackendGui::mapCoordsToPixel(Vector2f coord) const
    {
        return {m_viewport.getLeft() + ((coord.x - m_view.getLeft()) / (m_view.getWidth() / m_viewport.getWidth())),
                m_viewport.getTop() + ((coord.y - m_view.getTop()) / (m_view.getHeight() / m_viewport.getHeight()))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::handleTwoFingerScroll(bool wasAlreadyScrolling)
    {
        TGUI_ASSERT(m_twoFingerScroll.isScrolling(), "m_twoFingerScroll.isScrolling() must return true when BackendGui::handleTwoFingerScroll is called");

        if (!wasAlreadyScrolling)
        {
            // If a tooltip is visible then hide it now
            if (m_visibleToolTip != nullptr)
            {
                // Correct the position of the tool tip so that it is relative again
                m_visibleToolTip->setPosition(m_toolTipRelativePos);

                remove(m_visibleToolTip);
                m_visibleToolTip = nullptr;
            }
            m_tooltipPossible = false;

            // In case the touch of the first finger caused a widget to respond, we tell widgets the mouse isn't down on them
            m_container->mouseNoLongerOnWidget();
            m_container->leftMouseButtonNoLongerDown();
        }

        const Vector2f touchPos = mapPixelToCoords(Vector2i{m_twoFingerScroll.getTouchPosition()});
        const float touchDelta = m_twoFingerScroll.getDelta(m_view.getHeight() / m_viewport.getHeight());
        if ((touchDelta != 0) && (m_container->processScrollEvent(touchDelta, touchPos, true)))
            return true;

        // Even if no widget was scrolled, we will still absorb the event when the scrolling bagan on top of a widget
        return m_container->getWidgetAtPosition(touchPos) != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::updateContainerSize()
    {
        m_viewport.updateParentSize({static_cast<float>(m_framebufferSize.x), static_cast<float>(m_framebufferSize.y)});
        m_view.updateParentSize({m_viewport.getWidth(), m_viewport.getHeight()});

        // If a render target was created already then inform it about the size change
        if (m_backendRenderTarget)
            m_backendRenderTarget->setView(m_view.getRect(), m_viewport.getRect(), {static_cast<float>(m_framebufferSize.x), static_cast<float>(m_framebufferSize.y)});

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

    void BackendGui::startTextInput(FloatRect)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::stopTextInput()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::updateTextCursorPosition(FloatRect, Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGui::setKeyboardNavigationEnabled(bool enabled)
    {
        m_keyboardNavigationEnabled = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGui::isKeyboardNavigationEnabled() const
    {
        return m_keyboardNavigationEnabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
