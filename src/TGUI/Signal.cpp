/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/to_string.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    unsigned int lastId = 0;

    unsigned int generateUniqueId()
    {
        return ++lastId;
    }

    template <typename Type>
    const Type& dereference(const void* obj)
    {
        return *static_cast<const Type*>(obj);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::vector<const void*> Signal::m_parameters(3, nullptr);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal::Signal(const Signal& other) :
        m_name    {other.m_name},
        m_handlers{} // signal handlers are not copied with the widget
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Signal::operator=(const Signal& other)
    {
        if (this != &other)
        {
            m_name = other.m_name;
            m_handlers.clear(); // signal handlers are not copied with the widget
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::connect(const Delegate& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = handler;
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::connect(const DelegateEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::disconnect(unsigned int id)
    {
        return m_handlers.erase(id) == 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Signal::disconnectAll()
    {
        m_handlers.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Widget::Ptr Signal::getWidget()
    {
        return dereference<Widget*>(m_parameters[0])->shared_from_this();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(TypeName, Type) \
    unsigned int Signal##TypeName::connect(const Delegate##TypeName& handler) \
    { \
        const auto id = generateUniqueId(); \
        m_handlers[id] = [handler](){ handler(dereference<Type>(m_parameters[1])); }; \
        return id; \
    } \
    \
    unsigned int Signal##TypeName::connect(const Delegate##TypeName##Ex& handler) \
    { \
        const auto id = generateUniqueId(); \
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, dereference<Type>(m_parameters[1])); }; \
        return id; \
    }

    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Int, int)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(UInt, unsigned int)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Bool, bool)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Float, float)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(String, sf::String)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Vector2f, sf::Vector2f)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::connect(const DelegateChildWindow& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(std::static_pointer_cast<ChildWindow>(dereference<ChildWindow*>(m_parameters[1])->shared_from_this())); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::connect(const DelegateChildWindowEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, std::static_pointer_cast<ChildWindow>(dereference<ChildWindow*>(m_parameters[1])->shared_from_this())); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalChildWindow::emit(const ChildWindow* childWindow)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&childWindow);
        return Signal::emit(childWindow);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItem& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(dereference<sf::String>(m_parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, dereference<sf::String>(m_parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemAndId& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(dereference<sf::String>(m_parameters[1]), dereference<sf::String>(m_parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemAndIdEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, dereference<sf::String>(m_parameters[1]), dereference<sf::String>(m_parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItem& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(dereference<sf::String>(m_parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItemEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, dereference<sf::String>(m_parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItemFull& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(dereference<std::vector<sf::String>>(m_parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItemFullEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, dereference<std::vector<sf::String>>(m_parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
