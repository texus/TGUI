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

#ifndef TGUI_ACCESSIBILITY_TREE_HPP
#define TGUI_ACCESSIBILITY_TREE_HPP

#include <TGUI/Accessibility/AccessibleObject.hpp>

#include <map>
#include <utility>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class BackendGui;

    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Interface that the platform-specific code implements to be informed about changes in the gui
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibilityEventListener
        {
        public:
            // Virtual destructor
            virtual ~AccessibilityEventListener() = default;

            // Called before events are reported, with the objects of which the state may have changed. Platforms that cache
            // the state of objects (e.g. to answer requests from other threads) have to update their cache here, as clients
            // will query the objects as soon as they receive the events.
            virtual void accessibleObjectsChanged(const std::vector<AccessibleObject::Ptr>& objects) = 0;

            // Called when something changed in the gui. The property is only meaningful for the PropertyChanged event.
            virtual void accessibleEventOccurred(AccessibleEvent event,
                                                 const AccessibleObject::Ptr& object,
                                                 AccessibleProperty property) = 0;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Backend-independent representation of the gui as it is presented to assistive technology.
        //
        // The tree asks widgets to create their accessible objects on demand and caches them, so that each widget (or part of
        // a widget) is always represented by the same object. Widgets report their changes to the tree, which queues them
        // until flushEvents is called so that the listener only receives the events once the widgets are in a stable state.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API AccessibilityTree
        {
        public:
            // Constructor
            AccessibilityTree(BackendGui& gui, AccessibilityEventListener& listener);

            // Destructor
            ~AccessibilityTree();

            AccessibilityTree(const AccessibilityTree&) = delete;
            AccessibilityTree& operator=(const AccessibilityTree&) = delete;

            // Returns the gui that is represented by this tree
            [[nodiscard]] BackendGui& getGui() const
            {
                return m_gui;
            }

            // Returns the object that represents the gui itself
            [[nodiscard]] AccessibleObject::Ptr getRoot();

            // Returns the object that represents a widget, creating it if it didn't exist yet
            [[nodiscard]] AccessibleObject::Ptr getWidgetObject(const Widget::Ptr& widget, const AccessibleObject::Ptr& parent);

            // Returns the object that represents a part of a widget (e.g. a list box item), creating it if it didn't exist yet
            [[nodiscard]] AccessibleObject::Ptr getPartObject(const Widget::Ptr& widget, const String& part, const AccessibleObject::Ptr& parent);

            // Searches the object that represents the widget, creating it and its parents if they didn't exist yet
            [[nodiscard]] AccessibleObject::Ptr findWidgetObject(const Widget::Ptr& widget);

            // Returns the object that represents a part of a widget, but only if it was already created before
            [[nodiscard]] AccessibleObject::Ptr findExistingPartObject(const Widget* widget, const String& key) const;

            // Returns whether the widget only exists to display a part of another widget (e.g. the list of an open combo box)
            [[nodiscard]] static bool isInternalPopupWidget(const Widget& widget);

            // Returns the object that currently has keyboard focus
            [[nodiscard]] AccessibleObject::Ptr getFocusedObject();

            // Returns the object that was last reported to have focus. Unlike getFocusedObject, this always matches the focus
            // events that the listener received, so it should be used when describing the state of the objects.
            [[nodiscard]] AccessibleObject::Ptr getReportedFocusedObject() const
            {
                return m_lastFocusedObject.lock();
            }

            // Queues an event that was reported by a widget. If the part is empty then the event is about the widget itself.
            void queueEvent(const Widget& widget, AccessibleEvent event, AccessibleProperty property, const String& part);

            // Returns whether widgets reported changes that haven't been send to the listener yet
            [[nodiscard]] bool hasQueuedEvents() const
            {
                return !m_queuedEvents.empty();
            }

            // Makes certain that the focused object is reported again during the next flush, even if it didn't change
            void invalidateFocus()
            {
                m_lastFocusedObject.reset();
            }

            // Indicates that the gui was changed via the accessibility api, so the screen needs to be redrawn
            void requestRedraw()
            {
                m_redrawRequested = true;
            }

            // Sends all queued events to the listener. If reportEvents is false then the events are discarded.
            // Returns true when the gui was changed via the accessibility api since the last call.
            bool flushEvents(bool reportEvents);

            // Removes all objects from the cache. They will all be reported as removed.
            void clear();

        private:
            using MapKey = std::pair<const void*, String>;

            struct QueuedEvent
            {
                std::weak_ptr<const Widget> widget;
                AccessibleEvent event;
                AccessibleProperty property;
                String part;
            };

            // Adds a newly created object to the cache
            AccessibleObject::Ptr registerObject(const MapKey& key, const AccessibleObject::Ptr& object, const AccessibleObject::Ptr& parent);

            // Removes the objects that represent parts of the gui that no longer exist
            void removeDeadObjects(bool reportEvents);

            // Removes the accessibility handlers from the signals of the widget, if the object represents a widget that still exists
            static void disconnectSignals(const AccessibleObject& object);

        private:
            BackendGui& m_gui;
            AccessibilityEventListener& m_listener;
            AccessibleObject::Ptr m_root;
            std::map<MapKey, AccessibleObject::Ptr> m_objects;
            std::vector<QueuedEvent> m_queuedEvents;
            std::weak_ptr<AccessibleObject> m_lastFocusedObject;
            std::weak_ptr<Widget> m_lastFocusedWidget; // Deepest focused widget during the last flush, to notice focus changes
            std::weak_ptr<AccessibleObject> m_lastSelectedObject; // Prevents reporting the same selection twice in a row
            std::weak_ptr<const Widget> m_widgetWithOpenPopup; // Widget with an open menu or list, in which the focus is located
            bool m_redrawRequested = false;
            bool m_flushing = false;
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBILITY_TREE_HPP
