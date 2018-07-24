/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/SignalImpl.hpp>

#include <set>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    unsigned int lastId = 0;

    unsigned int generateUniqueId()
    {
        return ++lastId;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace internal_signal
    {
        std::deque<const void*> parameters;
    }

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

    Widget::Ptr Signal::getWidget()
    {
        return internal_signal::dereference<Widget*>(internal_signal::parameters[0])->shared_from_this();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if (unboundParameters.size() == 0)
            return 0;
        else
            throw Exception{"Signal '" + m_name + "' could not provide data for unbound parameters."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(TypeName, Type) \
    unsigned int Signal##TypeName::connect(const Delegate##TypeName& handler) \
    { \
        const auto id = generateUniqueId(); \
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<Type>(internal_signal::parameters[1])); }; \
        return id; \
    } \
    \
    unsigned int Signal##TypeName::connect(const Delegate##TypeName##Ex& handler) \
    { \
        const auto id = generateUniqueId(); \
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<Type>(internal_signal::parameters[1])); }; \
        return id; \
    }

    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Int, int)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(UInt, unsigned int)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Bool, bool)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Float, float)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(String, const sf::String&)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Vector2f, Vector2f)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalInt::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(int).name()) == 0))
    #else
        if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(int)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalUInt::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(unsigned int).name()) == 0))
    #else
        if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(unsigned int)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalBool::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(bool).name()) == 0))
    #else
        if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(bool)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalFloat::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(float).name()) == 0))
    #else
        if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(float)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalString::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && ((strcmp(unboundParameters.begin()->name(), typeid(std::string).name()) == 0) || (strcmp(unboundParameters.begin()->name(), typeid(sf::String).name()) == 0)))
    #else
        if ((unboundParameters.size() == 1) && ((*unboundParameters.begin() == typeid(std::string)) || (*unboundParameters.begin() == typeid(sf::String))))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalVector2f::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && ((strcmp(unboundParameters.begin()->name(), typeid(Vector2f).name()) == 0) || (strcmp(unboundParameters.begin()->name(), typeid(sf::Vector2f).name()) == 0)))
    #else
        if ((unboundParameters.size() == 1) && ((*unboundParameters.begin() == typeid(Vector2f)) || (*unboundParameters.begin() == typeid(sf::Vector2f))))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalRange::connect(const DelegateRange& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<float>(internal_signal::parameters[1]), internal_signal::dereference<float>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalRange::connect(const DelegateRangeEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<float>(internal_signal::parameters[1]), internal_signal::dereference<float>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalRange::emit(const Widget* widget, float start, float end)
    {
        if (m_handlers.empty())
            return false;

        internal_signal::parameters[1] = static_cast<const void*>(&start);
        internal_signal::parameters[2] = static_cast<const void*>(&end);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalRange::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 2) && (strcmp(unboundParameters.begin()->name(), typeid(float).name()) == 0) && (strcmp((unboundParameters.begin()+1)->name(), typeid(float).name()) == 0))
    #else
        if ((unboundParameters.size() == 2) && (*unboundParameters.begin() == typeid(float)) && (*(unboundParameters.begin()+1) == typeid(float)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::connect(const DelegateChildWindow& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(std::static_pointer_cast<ChildWindow>(internal_signal::dereference<ChildWindow*>(internal_signal::parameters[1])->shared_from_this())); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::connect(const DelegateChildWindowEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, std::static_pointer_cast<ChildWindow>(internal_signal::dereference<ChildWindow*>(internal_signal::parameters[1])->shared_from_this())); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalChildWindow::emit(const ChildWindow* childWindow)
    {
        if (m_handlers.empty())
            return false;

        internal_signal::parameters[1] = static_cast<const void*>(&childWindow);
        return Signal::emit(childWindow);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(ChildWindow::Ptr).name()) == 0))
    #else
        if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(ChildWindow::Ptr)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItem& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemAndId& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<sf::String>(internal_signal::parameters[1]), internal_signal::dereference<sf::String>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemAndIdEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<sf::String>(internal_signal::parameters[1]), internal_signal::dereference<sf::String>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && ((strcmp(unboundParameters.begin()->name(), typeid(std::string).name()) == 0) || (strcmp(unboundParameters.begin()->name(), typeid(sf::String).name()) == 0)))
    #else
        if ((unboundParameters.size() == 1) && ((*unboundParameters.begin() == typeid(std::string)) || (*unboundParameters.begin() == typeid(sf::String))))
    #endif
            return 1;
        else if ((unboundParameters.size() == 2)
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
              && ((strcmp(unboundParameters.begin()->name(), typeid(std::string).name()) == 0) || (strcmp(unboundParameters.begin()->name(), typeid(sf::String).name()) == 0))
              && ((strcmp((unboundParameters.begin()+1)->name(), typeid(std::string).name()) == 0) || (strcmp((unboundParameters.begin()+1)->name(), typeid(sf::String).name()) == 0)))
    #else
              && ((*unboundParameters.begin() == typeid(std::string)) || (*unboundParameters.begin() == typeid(sf::String)))
              && ((*(unboundParameters.begin()+1) == typeid(std::string)) || (*(unboundParameters.begin()+1) == typeid(sf::String))))
    #endif
        {
            return 1;
        }
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItem& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItemEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItemFull& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<std::vector<sf::String>>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::connect(const DelegateMenuItemFullEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<std::vector<sf::String>>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalMenuItem::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && ((strcmp(unboundParameters.begin()->name(), typeid(std::string).name()) == 0) || (strcmp(unboundParameters.begin()->name(), typeid(sf::String).name()) == 0)))
    #else
        if ((unboundParameters.size() == 1) && ((*unboundParameters.begin() == typeid(std::string)) || (*unboundParameters.begin() == typeid(sf::String))))
    #endif
            return 1;
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        else if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(std::vector<sf::String>).name()) == 0))
    #else
        else if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(std::vector<sf::String>)))
    #endif
            return 2;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalAnimation::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(ShowAnimationType).name()) == 0))
    #else
        if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(ShowAnimationType)))
    #endif
            return 1;
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        else if ((unboundParameters.size() == 1) && (strcmp(unboundParameters.begin()->name(), typeid(bool).name()) == 0))
    #else
        else if ((unboundParameters.size() == 1) && (*unboundParameters.begin() == typeid(bool)))
    #endif
            return 2;
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        else if ((unboundParameters.size() == 2) && (strcmp(unboundParameters.begin()->name(), typeid(ShowAnimationType).name()) == 0) && (strcmp((unboundParameters.begin()+1)->name(), typeid(bool).name()) == 0))
    #else
        else if ((unboundParameters.size() == 2) && (*unboundParameters.begin() == typeid(ShowAnimationType)) && (*(unboundParameters.begin()+1) == typeid(bool)))
    #endif
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalWidgetBase::disconnect(unsigned int id)
    {
        auto it = m_connectedSignals.find(id);
        if (it != m_connectedSignals.end())
        {
            const bool ret = getSignal(it->second).disconnect(id);
            m_connectedSignals.erase(it);
            return ret;
        }
        else // The id was not found
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll(std::string signalName)
    {
        return getSignal(toLower(std::move(signalName))).disconnectAll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll()
    {
        std::set<std::string> signalNames;
        for (const auto& connection : m_connectedSignals)
            signalNames.insert(connection.second);

        for (auto& name : signalNames)
            getSignal(std::move(name)).disconnectAll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
