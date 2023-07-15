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


#ifndef TGUI_DURATION_HPP
#define TGUI_DURATION_HPP

#include <TGUI/Config.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
    #include <chrono>
    #include <type_traits>
#endif

#if TGUI_HAS_WINDOW_BACKEND_SFML
    #include <SFML/System/Time.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper for durations
    ///
    /// You do not have to use class directly in your code unless you want to store the duration.
    /// Functions taking tgui::Duration as parameter can be directly passed arguments like:
    /// - sf::microseconds(500)
    /// - std::chrono::nanoseconds(20000)
    /// - std::chrono::milliseconds(10) / 2.0f
    /// - 200  // unit is milliseconds
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class Duration
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates an zero-length duration
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Duration() :
            m_duration{std::chrono::nanoseconds::zero()}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the duration from any kind of std::chrono::duration
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Rep, typename Period>
        constexpr Duration(std::chrono::duration<Rep, Period> duration) :
            m_duration{std::chrono::duration_cast<std::chrono::nanoseconds>(duration)}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the duration from a given amount of milliseconds
        /// @param milliseconds  Amount of milliseconds to store in the duration
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Duration(int milliseconds) :
            Duration{std::chrono::milliseconds(milliseconds)}
        {
        }


#if TGUI_HAS_WINDOW_BACKEND_SFML
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the duration from an sf::Time instance
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Duration(sf::Time duration) :
            m_duration{std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::microseconds(duration.asMicroseconds()))}
        {
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the duration in seconds
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr float asSeconds() const
        {
            return static_cast<float>(static_cast<double>(m_duration.count()) / 1000000000.0);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert the duration to std::chrono::nanoseconds
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr operator std::chrono::nanoseconds() const
        {
            return m_duration;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert the duration to std::chrono::duration
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Rep, typename Period>
        constexpr operator std::chrono::duration<Rep, Period>() const
        {
            return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(m_duration);
        }

#if TGUI_HAS_WINDOW_BACKEND_SFML
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert the duration to sf::Time
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        operator sf::Time() const
        {
            return sf::microseconds(m_duration.count() / 1000);
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::chrono::nanoseconds m_duration;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD constexpr bool operator==(const Duration& lhs, const Duration& rhs)
    {
        return std::chrono::nanoseconds(lhs) == std::chrono::nanoseconds(rhs);
    }

    TGUI_NODISCARD constexpr bool operator!=(const Duration& lhs, const Duration& rhs)
    {
        return std::chrono::nanoseconds(lhs) != std::chrono::nanoseconds(rhs);
    }

    TGUI_NODISCARD constexpr bool operator>(const Duration& lhs, const Duration& rhs)
    {
        return std::chrono::nanoseconds(lhs) > std::chrono::nanoseconds(rhs);
    }

    TGUI_NODISCARD constexpr bool operator>=(const Duration& lhs, const Duration& rhs)
    {
        return std::chrono::nanoseconds(lhs) >= std::chrono::nanoseconds(rhs);
    }

    TGUI_NODISCARD constexpr bool operator<(const Duration& lhs, const Duration& rhs)
    {
        return std::chrono::nanoseconds(lhs) < std::chrono::nanoseconds(rhs);
    }

    TGUI_NODISCARD constexpr bool operator<=(const Duration& lhs, const Duration& rhs)
    {
        return std::chrono::nanoseconds(lhs) <= std::chrono::nanoseconds(rhs);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD constexpr Duration operator+(const Duration& lhs, const Duration& rhs)
    {
        return {std::chrono::nanoseconds(lhs) + std::chrono::nanoseconds(rhs)};
    }

    TGUI_NODISCARD constexpr Duration operator-(const Duration& lhs, const Duration& rhs)
    {
        return {std::chrono::nanoseconds(lhs) - std::chrono::nanoseconds(rhs)};
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    TGUI_NODISCARD constexpr Duration operator*(const Duration& lhs, T rhs)
    {
        return {std::chrono::nanoseconds(lhs) * rhs};
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    TGUI_NODISCARD constexpr Duration operator*(T lhs, const Duration& rhs)
    {
        return {lhs * std::chrono::nanoseconds(rhs)};
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    TGUI_NODISCARD constexpr Duration operator/(const Duration& lhs, T rhs)
    {
        return {std::chrono::nanoseconds(lhs) / rhs};
    }

    TGUI_NODISCARD constexpr float operator/(const Duration& lhs, const Duration& rhs)
    {
        return lhs.asSeconds() / rhs.asSeconds();
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    TGUI_NODISCARD constexpr Duration operator%(const Duration& lhs, T rhs)
    {
        return {std::chrono::nanoseconds(lhs) % rhs};
    }

    TGUI_NODISCARD constexpr Duration operator%(const Duration& lhs, const Duration& rhs)
    {
        return {std::chrono::nanoseconds(lhs) % std::chrono::nanoseconds(rhs)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    constexpr Duration& operator+=(Duration& lhs, const Duration& rhs)
    {
        return lhs = lhs + rhs;
    }

    constexpr Duration& operator-=(Duration& lhs, const Duration& rhs)
    {

        return lhs = lhs - rhs;
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    constexpr Duration& operator*=(Duration& lhs, T rhs)
    {
        return lhs = lhs * rhs;
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    constexpr Duration& operator/=(Duration& lhs, T rhs)
    {
        return lhs = lhs / rhs;
    }

    template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
    constexpr Duration& operator%=(Duration& lhs, T rhs)
    {
        return lhs = lhs % rhs;
    }

    constexpr Duration& operator%=(Duration& lhs, const Duration& rhs)
    {
        return lhs = lhs % rhs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_DURATION_HPP


