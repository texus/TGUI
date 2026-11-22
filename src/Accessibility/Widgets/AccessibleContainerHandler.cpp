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

#include <TGUI/Accessibility/AccessibleWidgetHandler.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleContainerHandler::getState(const Widget&, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            // The widgets inside the container are its children, the container itself has no meaning
            state.role = AccessibleRole::Pane;
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleRootContainerHandler::getState(const Widget&, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            // The root container represents the window, its name is provided by the window itself
            state.role = AccessibleRole::Window;
            return true;
        }
    } // namespace priv
} // namespace tgui
