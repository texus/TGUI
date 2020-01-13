/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_SIGNAL_MANAGER_IMPL_HPP
#define TGUI_SIGNAL_MANAGER_IMPL_HPP

#include <TGUI/SignalImpl.hpp>
#include <TGUI/SignalManager.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cpp_if_constexpr) && (__cpp_if_constexpr >= 201606L)
    template <typename Func, typename... BoundArgs>
    unsigned int SignalManager::connect(std::string widgetName, std::string signalName, Func&& handler, const BoundArgs&... args)
    {
        const unsigned int id = generateUniqueId();

        if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&...)>>
                   && std::is_invocable_v<decltype(&handler), BoundArgs...>
                   && !std::is_function_v<Func>)
        {
            // Reference to function
            m_signals[id] = {widgetName, signalName, makeSignal([=, f=std::function<void(const BoundArgs&...)>(handler)]{ std::invoke(f, args...); })};
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&...)>>)
        {
            // Function pointer
            m_signals[id] = {widgetName, signalName, makeSignal([=]{ std::invoke(handler, args...); })};
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>>
                        && std::is_invocable_v<decltype(&handler), BoundArgs..., const std::shared_ptr<Widget>&, const std::string&>
                        && !std::is_function_v<Func>)
        {
            // Reference to function with caller arguments
            m_signals[id] = {widgetName, signalName, makeSignal([=, f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>& w, const std::string& s)>(handler)](const std::shared_ptr<Widget>& w, const std::string& s){ std::invoke(f, args..., w, s); })};
        }
        else
        {
            // Function pointer with caller arguments
            m_signals[id] = {widgetName, signalName, makeSignal([=](const std::shared_ptr<Widget>& w, const std::string& s){ std::invoke(handler, args..., w, s); })};
        }

        connect(id);
        return id;
    }

#else
    template <typename Func, typename... Args, typename std::enable_if<std::is_convertible<Func, std::function<void(const Args&...)>>::value>::type*>
    unsigned int SignalManager::connect(std::string widgetName, std::string signalName, Func&& handler, const Args&... args)
    {
        const unsigned int id = generateUniqueId();
        m_signals[id] = {widgetName, signalName, makeSignal([f=std::function<void(const Args&...)>(handler),args...](){ f(args...); })};

        connect(id);
        return id;
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if<!std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value // Ambigious otherwise when passing bind expression
                                                                            && std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type*>
    unsigned int SignalManager::connect(std::string widgetName, std::string signalName, Func&& handler, BoundArgs&&... args)
    {
        const unsigned int id = generateUniqueId();
        m_signals[id] = {widgetName, signalName, makeSignal(
                [f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>(handler), args...]
                        (const std::shared_ptr<Widget>& w, const std::string& s)
                { f(args..., w, s); }
        )};

        connect(id);
        return id;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename Func, typename... BoundArgs>
    unsigned int SignalManager::connect(std::string widgetName, std::initializer_list<std::string> signalNames, Func&& handler, BoundArgs&&... args)
    {
        unsigned int lastId = 0;
        for (auto& signalName : signalNames)
            lastId = connect(widgetName, signalName, handler, args...);

        return lastId;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_MANAGER_IMPL_HPP
