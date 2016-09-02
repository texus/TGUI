/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::deque<const void*> priv::data; // deque because vector could reallocate when resizing after parameters are already bound
    unsigned int SignalWidgetBase::m_lastId = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal::Signal(std::vector<std::vector<std::string>>&& types)
    {
        std::size_t maxSize = 0;
        for (const auto& typeList : types)
            maxSize += typeList.size();

        m_allowedTypes = types;
        if (maxSize > priv::data.size())
            priv::data.resize(maxSize, nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::disconnect(unsigned int id)
    {
        return !!m_functions.erase(id) || !!m_functionsEx.erase(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Signal::disconnectAll()
    {
        m_functions.clear();
        m_functionsEx.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::isEmpty()
    {
        return m_functions.empty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Signal::operator()(unsigned int)
    {
        for (const auto& function : m_functions)
            function.second();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnect(unsigned int id)
    {
        for (auto& signal : m_signals)
        {
            if (signal.second.disconnect(id))
                return;
        }

        throw Exception{"Failed to disconnect signal handler. There is no function bound to the given id " + tgui::to_string(id) + "."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll(const std::string& signalName)
    {
        for (const auto& name : extractSignalNames(signalName))
            m_signals.at(toLower(name)).disconnectAll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll()
    {
        for (auto& signal : m_signals)
            signal.second.disconnectAll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalWidgetBase::isSignalBound(std::string&& name)
    {
        assert(m_signals.find(toLower(name)) != m_signals.end());
        return !m_signals[toLower(name)].isEmpty() || !m_signals[toLower(name)].m_functionsEx.empty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> SignalWidgetBase::extractSignalNames(std::string input)
    {
        // A space is used for binding multiple signals at once
        std::vector<std::string> names;
        std::string::size_type pos = 0;
        while ((pos = input.find(" ")) != std::string::npos)
        {
            std::string token = input.substr(0, pos);
            if (token != "")
                names.push_back(token);

            input.erase(0, pos+1);
        }

        if (input != "")
            names.push_back(input);

        return names;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
