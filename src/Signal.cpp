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


#include <TGUI/Signal.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Panel.hpp>

#include <cassert>
#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <set>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::m_lastSignalId = 0;
    std::deque<const void*> Signal::m_parameters;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal::Signal(const Signal& other) :
        m_enabled {other.m_enabled},
        m_name    {other.m_name},
        m_handlers{} // signal handlers are not copied with the widget
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Signal::operator=(const Signal& other)
    {
        if (this != &other)
        {
            m_enabled = other.m_enabled;
            m_name = other.m_name;
            m_handlers.clear(); // signal handlers are not copied with the widget
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::disconnect(unsigned int id)
    {
        return (m_handlers.erase(id) != 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Signal::disconnectAll()
    {
        m_handlers.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::emit(const Widget* widget)
    {
        if (m_handlers.empty() || !m_enabled)
            return false;

        m_parameters[0] = static_cast<const void*>(&widget);

        // Copy the handlers before calling them in case the widget (and this signal) gets destroyed during the handler
        auto handlers = m_handlers;
        for (const auto& handler : handlers)
            handler.second();

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Signal::getWidget()
    {
        return dereferenceParam<Widget*>(0)->shared_from_this();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalChildWindow::emit(ChildWindow* childWindow)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&childWindow);
        return Signal::emit(childWindow);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ChildWindow> SignalChildWindow::dereferenceChildWindow()
    {
        return dereferenceParam<ChildWindow*>(1)->shared_from_this()->cast<ChildWindow>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalItem::emit(const Widget* widget, int index, const String& item, const String& id)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&index);
        m_parameters[2] = static_cast<const void*>(&item);
        m_parameters[3] = static_cast<const void*>(&id);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalPanelListBoxItem::emit(const Widget* widget, int index, const std::shared_ptr<Panel>& panel, const String& id)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&index);
        m_parameters[2] = static_cast<const void*>(&panel);
        m_parameters[3] = static_cast<const void*>(&id);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Panel> SignalPanelListBoxItem::dereferencePanel()
    {
        return dereferenceParam<Panel*>(2)->shared_from_this()->cast<Panel>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalFileDialogPaths::emit(const Widget* widget, const std::vector<Filesystem::Path>& paths)
    {
        if (m_handlers.empty())
            return false;

        assert(!paths.empty());
        const String& pathStr = paths[0].asString();
        m_parameters[1] = static_cast<const void*>(&pathStr);
        m_parameters[2] = static_cast<const void*>(&paths[0]);
        m_parameters[3] = static_cast<const void*>(&paths);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalShowEffect::emit(const Widget* widget, ShowEffectType type, bool visible)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&type);
        m_parameters[2] = static_cast<const void*>(&visible);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalAnimationType::emit(const Widget* widget, AnimationType type)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&type);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalItemHierarchy::emit(const Widget* widget, const String& item, const std::vector<String>& fullItem)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&item);
        m_parameters[2] = static_cast<const void*>(&fullItem);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
