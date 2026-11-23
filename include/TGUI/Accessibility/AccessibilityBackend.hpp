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

#ifndef TGUI_ACCESSIBILITY_BACKEND_HPP
#define TGUI_ACCESSIBILITY_BACKEND_HPP

#include <TGUI/Accessibility/AccessibilityTree.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Base class for the code that exposes the gui of a window to the accessibility api of the operating system.
        //
        // Widgets report their changes via their signals to the tree of the backend, which passes them to the
        // platform-specific code once per frame. The platform-specific code answers the requests from assistive
        // technology by querying the accessible objects in the tree.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibilityBackend : public AccessibilityEventListener
        {
        public:
            // Creates the backend for the accessibility api of the current platform.
            // Returns a nullptr when accessibility isn't supported on this platform or when TGUI was build without it.
            [[nodiscard]] static std::unique_ptr<AccessibilityBackend> create(BackendGui& gui, void* nativeWindowHandle);

            // Constructor
            explicit AccessibilityBackend(BackendGui& gui);

            // Virtual destructor
            ~AccessibilityBackend() override = default;

            AccessibilityBackend(const AccessibilityBackend&) = delete;
            AccessibilityBackend& operator=(const AccessibilityBackend&) = delete;

            // Returns the tree that contains the accessible objects
            [[nodiscard]] AccessibilityTree& getTree()
            {
                return m_tree;
            }

            // Called each time the gui is updated to send the queued events.
            // Returns true when the gui was changed by assistive technology and thus needs to be redrawn.
            virtual bool update() = 0;

            // Called each time the gui is drawn. Changes that widgets don't report (e.g. scrolling) are always followed by a
            // redraw, so platforms that cache the state of objects only need to check for such changes after the gui was drawn.
            virtual void guiDrawn()
            {
            }

            // Waits until the timeout expires or until a message arrives for the window.
            // Returns false when the platform can't wait for messages, in which case the caller has to sleep itself.
            virtual bool waitForEvents(Duration)
            {
                return false;
            }

        protected:
            AccessibilityTree m_tree;
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBILITY_BACKEND_HPP
