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

#ifndef TGUI_SCOPE_EXIT_HPP
#define TGUI_SCOPE_EXIT_HPP

#include <TGUI/Config.hpp>

#include <type_traits>
#include <utility>

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Invokes a function when leaving scope (return or exception). Not included from TGUI.hpp; include explicitly if needed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename F>
    class ScopeExit
    {
    public:
        template <typename G, typename = typename std::enable_if<!std::is_same_v<typename std::decay<G>::type, ScopeExit>>::type>
        explicit ScopeExit(G&& func) :
            m_func(std::forward<G>(func)),
            m_active(true)
        {
        }

        ScopeExit(const ScopeExit&) = delete;

        ScopeExit& operator=(const ScopeExit&) = delete;
        ScopeExit& operator=(ScopeExit&&) = delete;

        ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<F>) :
            m_func(std::move(other.m_func)),
            m_active(other.m_active)
        {
            other.m_active = false;
        }

        ~ScopeExit()
        {
            if (m_active)
                m_func();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Skip invoking the function when the guard is destroyed
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void release() noexcept
        {
            m_active = false;
        }

    private:
        F m_func;
        bool m_active;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Helper to create a ScopeExit without naming the lambda's type
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename F>
    [[nodiscard]] ScopeExit<typename std::decay<F>::type> makeScopeExit(F&& func)
    {
        return ScopeExit<typename std::decay<F>::type>(std::forward<F>(func));
    }
} // namespace tgui

#endif // TGUI_SCOPE_EXIT_HPP
