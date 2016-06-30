/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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

    std::deque<const void*> Signal::data;

    SignalWidgetBase* Signal::m_currentWidget;
    std::size_t       Signal::m_currentId;
    std::string       Signal::m_currentSignalName;

    unsigned int SignalWidgetBase::m_lastId = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnect(unsigned int id)
    {
        for (auto& signal : m_signals)
        {
            if (signal.second.erase(id) != 0)
                return;
        }

        throw Exception{"Failed to disconnect signal handler. There is no function bound to the given id " + tgui::to_string(id) + "."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll(const std::string& signalNames)
    {
        for (auto& name : extractSignalNames(signalNames))
            m_signals.at(toLower(name)).clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll()
    {
        for (auto& signal : m_signals)
            signal.second.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t SignalWidgetBase::findBoundParametersLocation(const std::string& signal, const std::initializer_list<std::type_index>&)
    {
        throw Exception{"TGUI Error: widget does not support signal '" + signal + "'"};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalWidgetBase::isSignalBound(std::string&& name)
    {
        return !m_signals[toLower(name)].empty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> SignalWidgetBase::extractSignalNames(std::string input)
    {
        // A space is used as separator when binding multiple signals at once
        std::vector<std::string> names;
        std::string::size_type pos = 0;
        while ((pos = input.find(" ")) != std::string::npos)
        {
            std::string token = input.substr(0, pos);
            if (token != "")
                names.push_back(toLower(token));

            input.erase(0, pos+1);
        }

        if (input != "")
            names.push_back(toLower(input));

        return names;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
