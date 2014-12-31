/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Callback.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CallbackManager::bindCallback(std::function<void()> func, unsigned int trigger)
    {
        mapCallback(func, trigger);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CallbackManager::bindCallbackEx(std::function<void(const Callback&)> func, unsigned int trigger)
    {
        mapCallback(std::bind(func, std::ref(m_Callback)), trigger);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CallbackManager::bindCallback(unsigned int trigger)
    {
        mapCallback(nullptr, trigger);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CallbackManager::unbindCallback(unsigned int trigger)
    {
        unsigned int counter = 1;
        while (counter * 2 <= trigger)
            counter *= 2;

        while (counter > 0)
        {
            if (trigger >= counter)
            {
                m_CallbackFunctions.erase(counter);
                trigger -= counter;
            }

            counter /= 2;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CallbackManager::unbindAllCallback()
    {
        m_CallbackFunctions.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CallbackManager::mapCallback(const std::function<void()>& function, unsigned int trigger)
    {
        unsigned int counter = 1;
        while (counter * 2 <= trigger)
            counter *= 2;

        while (counter > 0)
        {
            if (trigger >= counter)
            {
                m_CallbackFunctions[counter].push_back(function);
                trigger -= counter;
            }

            counter /= 2;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

