/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/SignalManager.hpp>
#include <TGUI/SignalManagerImpl.hpp>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    SignalManager::Ptr SignalManager::m_manager = std::make_shared<SignalManager>();

    SignalManager::SignalManager()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SignalManager::~SignalManager()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalManager::setDefaultSignalManager(const SignalManager::Ptr &manager)
    {
        m_manager = manager;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SignalManager::Ptr SignalManager::getSignalManager()
    {
        return m_manager;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalManager::disconnect(unsigned int id)
    {
        auto it = m_signals.find(id);
        if (it != m_signals.end())
        {
            m_signals.erase(it);

            for (auto it2 = m_connectedSignals.begin(); it2 != m_connectedSignals.end();)
            {
                if (it2->m_signalId != id)
                {
                    ++it2;
                    continue;
                }

                if (auto widget = it2->m_widget.lock())
                {
                    widget->disconnect(it2->m_signalWidgetID);
                }

                it2 = m_connectedSignals.erase(it2);
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalManager::disconnectAll()
    {
        for (auto &it : m_connectedSignals)
        {
            if (auto widget = it.m_widget.lock())
            {
                widget->disconnect(it.m_signalWidgetID);
            }
        }

        m_signals.clear();
        m_connectedSignals.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalManager::add(const Widget::Ptr &widgetPtr)
    {
        m_widgets.emplace_back(widgetPtr);

        for (auto &it : m_signals)
        {
            if (it.second.m_widgetName == widgetPtr->getWidgetName())
            {
                unsigned int id;
                if (it.second.m_func.first)
                {
                    id = widgetPtr->connect(it.second.m_signalName, it.second.m_func.first);
                }
                else
                {
                    id = widgetPtr->connect(it.second.m_signalName, it.second.m_func.second);
                }

                m_connectedSignals.push_back({it.first, widgetPtr, id});
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalManager::remove(Widget *widget)
    {
        for (auto it = m_widgets.begin(); it != m_widgets.end();)
        {
            if (it->expired() || it->lock().get() == widget)
            {
                it = m_widgets.erase(it);
            }
            else
            {
                ++it;
            }
        }
        for (auto it = m_connectedSignals.begin(); it != m_connectedSignals.end();)
        {
            if (it->m_widget.expired())
            {
                it = m_connectedSignals.erase(it);
            }
            else if(it->m_widget.lock().get() == widget)
            {
                widget->disconnect(it->m_signalWidgetID);
                it = m_connectedSignals.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void SignalManager::connect(SignalID sid)
    {
        auto signalIt = m_signals.find(sid);
        if (signalIt == m_signals.end())
        { return; }
        auto &handle = signalIt->second;

        for (auto &it : m_widgets)
        {
            if (auto widget = it.lock())
            {
                if (widget->getWidgetName() == handle.m_widgetName)
                {
                    unsigned int id;
                    if (handle.m_func.first)
                    {
                        id = widget->connect(handle.m_signalName, handle.m_func.first);
                    }
                    else
                    {
                        id = widget->connect(handle.m_signalName, handle.m_func.second);
                    }

                    m_connectedSignals.push_back({sid, it, id});
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<SignalManager::Delegate, SignalManager::DelegateEx>
    SignalManager::makeSignal(const SignalManager::Delegate &handle)
    {
        return std::make_pair(handle, SignalManager::DelegateEx{});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<SignalManager::Delegate, SignalManager::DelegateEx>
    SignalManager::makeSignal(const SignalManager::DelegateEx &handle)
    {
        return std::make_pair(SignalManager::Delegate{}, handle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalManager::generateUniqueId()
    {
        return ++m_lastId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

