////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Accessibility/AccessibilityTree.hpp>
#include <TGUI/Accessibility/AccessibleWidgetHandler.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <TGUI/SubwidgetContainer.hpp>

#include <algorithm>
#include <atomic>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibilityTree::AccessibilityTree(BackendGui& gui, AccessibilityEventListener& listener) :
            m_gui{gui},
            m_listener{listener}
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibilityTree::~AccessibilityTree()
        {
            // The listener is being destroyed together with the tree, so we don't report the removal of the objects here
            for (auto& pair : m_objects)
            {
                disconnectSignals(*pair.second);
                pair.second->invalidate();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::getRoot()
        {
            const Widget::Ptr rootContainer = m_gui.getContainer();
            if (!rootContainer)
                return nullptr;

            if (m_root && m_root->isAlive() && (m_root->getWidget() == rootContainer))
                return m_root;

            m_root = registerObject({rootContainer.get(), String{}},
                                    std::make_shared<AccessibleObject>(*this, rootContainer, String{}),
                                    nullptr);
            return m_root;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::getWidgetObject(const Widget::Ptr& widget, const AccessibleObject::Ptr& parent)
        {
            if (!widget)
                return nullptr;

            const MapKey mapKey{widget.get(), String{}};
            const auto it = m_objects.find(mapKey);
            if ((it != m_objects.end()) && it->second->isAlive() && (it->second->getWidget() == widget))
            {
                it->second->m_parent = parent;
                return it->second;
            }

            return registerObject(mapKey, std::make_shared<AccessibleObject>(*this, widget, String{}), parent);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::getPartObject(const Widget::Ptr& widget,
                                                               const String& part,
                                                               const AccessibleObject::Ptr& parent)
        {
            const MapKey mapKey{widget.get(), part};
            const auto it = m_objects.find(mapKey);
            if ((it != m_objects.end()) && it->second->isAlive() && (it->second->getWidget() == widget))
            {
                it->second->m_parent = parent;
                return it->second;
            }

            return registerObject(mapKey, std::make_shared<AccessibleObject>(*this, widget, part), parent);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::findWidgetObject(const Widget::Ptr& widget)
        {
            if (!widget)
                return nullptr;

            const auto rootContainer = m_gui.getContainer();
            if (widget == rootContainer)
                return getRoot();

            // Find all parents of the widget, so that we can create the objects starting from the root
            std::vector<Widget::Ptr> ancestors;
            bool rootFound = false;
            for (Widget* ancestor = widget->getParent(); ancestor != nullptr; ancestor = ancestor->getParent())
            {
                if (ancestor == rootContainer.get())
                {
                    rootFound = true;
                    break;
                }

                ancestors.push_back(ancestor->shared_from_this());
            }

            // Widgets inside a SubwidgetContainer have a parent that isn't connected to the gui. If the object was already
            // created while navigating through the tree then it already knows its parent, otherwise we can't determine it.
            if (!rootFound)
            {
                const auto it = m_objects.find({widget.get(), String{}});
                if ((it != m_objects.end()) && it->second->isAlive() && (it->second->getWidget() == widget))
                    return it->second;

                return getWidgetObject(widget, nullptr);
            }

            AccessibleObject::Ptr parent = getRoot();
            for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it)
            {
                parent = getWidgetObject(*it, parent);
                if (!parent)
                    return nullptr;
            }

            return getWidgetObject(widget, parent);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::findExistingPartObject(const Widget* widget, const String& key) const
        {
            const auto it = m_objects.find({widget, key});
            if ((it == m_objects.end()) || !it->second->isAlive())
                return nullptr;

            return it->second;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::getFocusedObject()
        {
            const auto rootContainer = m_gui.getContainer();
            if (!rootContainer)
                return nullptr;

            // When a menu is open then the keyboard is used to navigate inside the menu, so the selected menu item has focus
            if (const auto menuWidget = m_widgetWithOpenPopup.lock())
            {
                if (const auto menuObject = findWidgetObject(std::const_pointer_cast<Widget>(menuWidget)))
                {
                    if (const auto menuItem = menuObject->getFocusedDescendant())
                        return menuItem;
                }
            }

            // Follow the focused widgets from the root container down to the deepest focused widget
            AccessibleObject::Ptr object = getRoot();
            Widget::Ptr focusedWidget = rootContainer->getFocusedChild();
            while (focusedWidget && focusedWidget->isVisible())
            {
                // The list of an open combo box gets focus, but the combo box itself already passes the focus to its items
                if (isInternalPopupWidget(*focusedWidget))
                    break;

                const auto childObject = getWidgetObject(focusedWidget, object);
                if (!childObject)
                    break;

                object = childObject;
                if (focusedWidget->isContainer())
                    focusedWidget = std::static_pointer_cast<Container>(focusedWidget)->getFocusedChild();
                else if (const auto subwidgetContainer = std::dynamic_pointer_cast<SubwidgetContainer>(focusedWidget))
                    focusedWidget = subwidgetContainer->getContainer()->getFocusedChild();
                else
                    break;
            }

            // Some widgets pass the focus to one of their parts (e.g. the selected item of a list box).
            // The amount of iterations is limited to protect against objects that would keep returning new descendants.
            for (unsigned int i = 0; i < 8; ++i)
            {
                const auto descendant = object->getFocusedDescendant();
                if (!descendant || (descendant == object))
                    break;

                object = descendant;
            }

            return object;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityTree::queueEvent(const Widget& widget, AccessibleEvent event, AccessibleProperty property, const String& part)
        {
            // The same event is only reported once per flush
            for (const auto& queuedEvent : m_queuedEvents)
            {
                if ((queuedEvent.event == event) && (queuedEvent.property == property) && (queuedEvent.part == part)
                    && (queuedEvent.widget.lock().get() == &widget))
                {
                    return;
                }
            }

            m_queuedEvents.push_back({widget.weak_from_this(), event, property, part});
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibilityTree::flushEvents(bool reportEvents)
        {
            const bool redrawRequested = m_redrawRequested;
            m_redrawRequested = false;

            // Signal handlers could be triggered while we are querying the widgets, which might cause a recursive call
            if (m_flushing)
                return redrawRequested;

            m_flushing = true;
            removeDeadObjects(reportEvents);

            auto queuedEvents = std::move(m_queuedEvents);
            m_queuedEvents.clear();

            if (!reportEvents)
            {
                // Keep track of which widget has an open menu, as the focus is located inside the menu in that case
                for (const auto& queuedEvent : queuedEvents)
                {
                    if (queuedEvent.event == AccessibleEvent::MenuOpened)
                        m_widgetWithOpenPopup = queuedEvent.widget;
                    else if ((queuedEvent.event == AccessibleEvent::MenuClosed)
                             && (m_widgetWithOpenPopup.lock() == queuedEvent.widget.lock()))
                        m_widgetWithOpenPopup.reset();
                }

                m_lastFocusedObject.reset();
                m_flushing = false;
                return redrawRequested;
            }

            // The events are first collected and the state of the tree is updated, so that the listener can be told which
            // objects changed before it reports the events. Clients query the objects as soon as they receive an event.
            struct ReportedEvent
            {
                AccessibleEvent event;
                AccessibleObject::Ptr object;
                AccessibleProperty property;
            };
            std::vector<ReportedEvent> reportedEvents;
            std::vector<AccessibleObject::Ptr> changedObjects;

            // Adds the object to the changed objects, together with the objects of which the state depends on it
            const auto addChangedObject = [&changedObjects](const AccessibleObject::Ptr& object, bool includeSiblings)
            {
                changedObjects.push_back(object);
                const auto parent = object->getParent();
                if (parent)
                    changedObjects.push_back(parent);

                // Selecting an item changes the state of the other items as well
                const auto& children = (includeSiblings && parent) ? parent->getChildren() : object->getChildren();
                changedObjects.insert(changedObjects.end(), children.begin(), children.end());
            };

            for (const auto& queuedEvent : queuedEvents)
            {
                const auto widget = std::const_pointer_cast<Widget>(queuedEvent.widget.lock());
                if (!widget)
                    continue;

                // Keep track of which widget has an open menu, as the focus is located inside the menu in that case
                if (queuedEvent.event == AccessibleEvent::MenuOpened)
                    m_widgetWithOpenPopup = queuedEvent.widget;
                else if ((queuedEvent.event == AccessibleEvent::MenuClosed) && (m_widgetWithOpenPopup.lock() == widget))
                    m_widgetWithOpenPopup.reset();

                // The focus is calculated below, after all other events have been processed
                if (queuedEvent.event == AccessibleEvent::FocusChanged)
                    continue;

                AccessibleObject::Ptr object;
                if (queuedEvent.part.empty())
                    object = findWidgetObject(widget);
                else // Parts that were never requested by assistive technology don't need to report changes
                    object = findExistingPartObject(widget.get(), queuedEvent.part);

                // For a menu that was opened, the event is reported on the menu instead of on the widget that owns the menu
                if (object && (queuedEvent.event == AccessibleEvent::MenuOpened))
                {
                    if (const auto menuItem = object->getFocusedDescendant())
                    {
                        if (const auto menu = menuItem->getParent())
                            object = menu;
                    }
                }

                if (!object)
                    continue;

                // A combo box reports its selection again when its list closes, which shouldn't be announced twice
                if (queuedEvent.event == AccessibleEvent::ElementSelected)
                {
                    if (object == m_lastSelectedObject.lock())
                        continue;

                    m_lastSelectedObject = object;
                }

                addChangedObject(object, queuedEvent.event == AccessibleEvent::ElementSelected);
                reportedEvents.push_back({queuedEvent.event, object, queuedEvent.property});

                if ((queuedEvent.event == AccessibleEvent::PropertyChanged) && (queuedEvent.property == AccessibleProperty::Offscreen))
                {
                    // Showing or hiding a window is also reported as opening or closing it
                    if (object->getRole() == AccessibleRole::Window)
                        reportedEvents.push_back(
                            {object->isOffscreen() ? AccessibleEvent::WindowClosed : AccessibleEvent::WindowOpened, object, {}});

                    // Hidden widgets aren't part of the tree, so showing or hiding a widget changes the children of its parent
                    if (queuedEvent.part.empty())
                    {
                        if (const auto parent = object->getParent())
                            reportedEvents.push_back({AccessibleEvent::ChildrenChanged, parent, {}});
                    }
                }

                // While a widget is expanded (e.g. a combo box with an open list), the focus is located inside it
                if ((queuedEvent.event == AccessibleEvent::PropertyChanged)
                    && (queuedEvent.property == AccessibleProperty::ExpandState) && queuedEvent.part.empty())
                {
                    const auto expandState = object->getState().expandState;
                    if (expandState && (*expandState == AccessibleExpandState::Expanded))
                        m_widgetWithOpenPopup = queuedEvent.widget;
                    else if (m_widgetWithOpenPopup.lock() == widget)
                        m_widgetWithOpenPopup.reset();
                }
            }

            // Widgets only report changes once their object was requested, so the focus can move to a widget that never reported
            // anything (e.g. when no widget had focus yet). The focused widget is therefore compared on every flush as well.
            const auto rootContainer = m_gui.getContainer();
            const Widget::Ptr focusedWidget = rootContainer ? rootContainer->getFocusedLeaf() : nullptr;
            const bool focusedWidgetChanged = (focusedWidget != m_lastFocusedWidget.lock());
            m_lastFocusedWidget = focusedWidget;

            // The focus event is send last, so that the new state of the focused object was already reported.
            // Both the old and new focused object changed state, and the root knows which descendant has focus.
            if (!queuedEvents.empty() || m_lastFocusedObject.expired() || focusedWidgetChanged)
            {
                const auto focusedObject = getFocusedObject();
                const auto previousFocusedObject = m_lastFocusedObject.lock();
                if (focusedObject && (focusedObject != previousFocusedObject))
                {
                    m_lastFocusedObject = focusedObject;
                    changedObjects.push_back(focusedObject);
                    if (previousFocusedObject)
                        changedObjects.push_back(previousFocusedObject);
                    if (m_root)
                        changedObjects.push_back(m_root);

                    // When no widget has focus then the window itself has focus, which the platform already reports.
                    // This is often only an intermediate state while the focus moves from one widget to another.
                    if (focusedObject != m_root)
                        reportedEvents.push_back({AccessibleEvent::FocusChanged, focusedObject, {}});
                }
            }

            if (!changedObjects.empty())
            {
                std::sort(changedObjects.begin(), changedObjects.end());
                changedObjects.erase(std::unique(changedObjects.begin(), changedObjects.end()), changedObjects.end());
                m_listener.accessibleObjectsChanged(changedObjects);
            }

            for (const auto& reportedEvent : reportedEvents)
                m_listener.accessibleEventOccurred(reportedEvent.event, reportedEvent.object, reportedEvent.property);

            m_flushing = false;
            return redrawRequested;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityTree::clear()
        {
            auto objects = std::move(m_objects);
            m_objects.clear();
            m_queuedEvents.clear();
            m_root = nullptr;
            m_lastFocusedObject.reset();

            for (auto& pair : objects)
            {
                m_listener.accessibleEventOccurred(AccessibleEvent::ObjectRemoved, pair.second, {});
                disconnectSignals(*pair.second);
                pair.second->invalidate();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibilityTree::registerObject(const MapKey& key,
                                                                const AccessibleObject::Ptr& object,
                                                                const AccessibleObject::Ptr& parent)
        {
            // If there was an object for a widget that no longer exists at the same address then remove the old object
            const auto it = m_objects.find(key);
            if (it != m_objects.end())
            {
                const auto oldObject = it->second;
                m_objects.erase(it);
                m_listener.accessibleEventOccurred(AccessibleEvent::ObjectRemoved, oldObject, {});
                disconnectSignals(*oldObject);
                oldObject->invalidate();
            }

            // Unique ids are never reused within the process. A gui that is created again for the same window would otherwise
            // give its objects the runtime ids of the objects of the previous gui, which screen readers may still remember.
            static std::atomic<unsigned int> lastUniqueId{0};
            object->m_uniqueId = ++lastUniqueId;
            object->m_parent = parent;
            m_objects[key] = object;

            // Widgets inform the tree about their changes via their signals
            if (object->getPart().empty())
            {
                if (const auto widget = object->getWidget())
                    AccessibleWidgetHandler::getHandler(*widget).connectSignals(*widget, *this);
            }

            return object;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityTree::disconnectSignals(const AccessibleObject& object)
        {
            if (!object.getPart().empty())
                return;

            if (const auto widget = object.getWidget())
                AccessibleWidgetHandler::getHandler(*widget).disconnectSignals(*widget);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibilityTree::isInternalPopupWidget(const Widget& widget)
        {
            const String& name = widget.getWidgetName();
            return (name == U"#TGUI_INTERNAL$ComboBoxListBox#") || (name == U"#TGUI_INTERNAL$OpenMenuPlaceholder#");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibilityTree::removeDeadObjects(bool reportEvents)
        {
            for (auto it = m_objects.begin(); it != m_objects.end();)
            {
                if (it->second->isAlive())
                {
                    ++it;
                    continue;
                }

                const auto object = it->second;
                it = m_objects.erase(it);
                if (reportEvents)
                    m_listener.accessibleEventOccurred(AccessibleEvent::ObjectRemoved, object, {});
                object->invalidate();
            }
        }
    } // namespace priv
} // namespace tgui
