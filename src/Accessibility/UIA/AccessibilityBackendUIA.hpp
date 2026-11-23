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

#ifndef TGUI_ACCESSIBILITY_BACKEND_UIA_HPP
#define TGUI_ACCESSIBILITY_BACKEND_UIA_HPP

#include <TGUI/Accessibility/AccessibilityBackend.hpp>
#include "UiaProviders.hpp"

#include <chrono>
#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Exposes the gui to screen readers on Windows via UI Automation.
        //
        // The window is subclassed so that we receive the WM_GETOBJECT message with which UI Automation asks for the provider
        // of the window. Providers are created for the objects that UI Automation requests, and they remain connected until
        // their object is removed from the tree or until the window is destroyed.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibilityBackendUIA : public AccessibilityBackend
        {
        public:
            // Creates the backend for a window. Returns a nullptr if the window couldn't be subclassed.
            [[nodiscard]] static std::unique_ptr<AccessibilityBackendUIA> create(BackendGui& gui, HWND window);

            // Constructor, the window will be subclassed by the create function
            AccessibilityBackendUIA(BackendGui& gui, HWND window);

            // Destructor
            ~AccessibilityBackendUIA() override;

            // Returns the window that contains the gui, or a nullptr when the window was destroyed
            [[nodiscard]] HWND getWindowHandle() const
            {
                return m_window;
            }

            // Returns the queue in which the providers place the actions that have to be executed on the gui thread
            [[nodiscard]] const std::shared_ptr<UiaActionQueue>& getActionQueue() const
            {
                return m_actionQueue;
            }

            // Returns the provider of an object with an extra reference, the caller must call Release on it.
            // Returns a nullptr if the object is a nullptr.
            [[nodiscard]] UiaElementProvider* getProvider(const AccessibleObject::Ptr& object);

            // Converts a rectangle from gui coordinates to screen coordinates
            [[nodiscard]] UiaRect mapGuiToScreen(const FloatRect& rect) const;

            // Converts a point from screen coordinates to gui coordinates
            [[nodiscard]] Vector2f mapScreenToGui(double x, double y) const;

            // Returns whether the window currently has keyboard focus
            [[nodiscard]] bool hasWindowFocus() const;

            // Returns the title of the window. This function may only be called from the thread of the window.
            [[nodiscard]] String getWindowTitle() const;

            // Sends the queued events to UI Automation
            bool update() override;

            // Remembers that the snapshots have to be checked for changes that weren't reported by the widgets
            void guiDrawn() override
            {
                m_snapshotsOutdated = true;
            }

            // Waits until the timeout expires or until a message arrives, which includes the requests from UI Automation
            bool waitForEvents(Duration timeout) override;

            // Called by the tree before it reports events, to update the snapshots of the objects that the events are about
            void accessibleObjectsChanged(const std::vector<AccessibleObject::Ptr>& objects) override;

            // Called by the tree to report the changes that have to be send to UI Automation
            void accessibleEventOccurred(AccessibleEvent event, const AccessibleObject::Ptr& object, AccessibleProperty property) override;

        private:
            // Handles the messages that are send to the window before passing them to the original window procedure
            static LRESULT CALLBACK subclassProc(HWND hWnd,
                                                 UINT message,
                                                 WPARAM wParam,
                                                 LPARAM lParam,
                                                 UINT_PTR subclassId,
                                                 DWORD_PTR refData);

            // Disconnects all providers, after which clients that still have a reference to them get an error when using them
            void disconnectProviders();

            // Removes the subclass from the window and tells UI Automation that the window no longer has providers
            void detachFromWindow();

            // Updates the snapshots of all providers, which UI Automation reads from its own threads
            void updateSnapshots();

            // Raises a property changed event on the provider of the object
            void raisePropertyChangedEvent(UiaElementProvider& provider, AccessibleProperty property);

        private:
            HWND m_window = nullptr;
            bool m_subclassed = false;
            bool m_windowFocused = false; // Updated with the WM_SETFOCUS and WM_KILLFOCUS messages
            std::shared_ptr<UiaActionQueue> m_actionQueue;
            std::chrono::steady_clock::time_point m_lastSnapshotUpdateTime;
            bool m_snapshotsOutdated = true; // Set when the gui was drawn, as something might have changed without a signal
            std::chrono::steady_clock::time_point m_focusReannounceTime; // When to report the focus again after attaching
            bool m_focusReannouncePending = false;
            std::unordered_map<const AccessibleObject*, UiaElementProvider*> m_providers; // Each provider holds one reference
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBILITY_BACKEND_UIA_HPP
