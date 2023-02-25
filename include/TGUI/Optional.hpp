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

#ifndef TGUI_OPTIONAL_HPP
#define TGUI_OPTIONAL_HPP

#include <TGUI/Config.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #if TGUI_COMPILED_WITH_CPP_VER >= 17
        #include <optional>
    #else
        #include <memory>
    #endif
#endif

TGUI_MODULE_EXPORT namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    template<typename T>
    using Optional = std::optional<T>;
#else
    template<typename T>
    class Optional
    {
    public:
        Optional() noexcept = default;

        Optional(const T& val) noexcept :
            m_ptr(std::make_unique<T>(val))
        {
        }

        Optional(T&& val) noexcept :
            m_ptr(std::make_unique<T>(std::move(val)))
        {
        }

        Optional(const Optional& other) :
            m_ptr(other.m_ptr ? std::make_unique<T>(*other.m_ptr) : nullptr)
        {
        }

        Optional(Optional&& other) noexcept = default;

        template<typename... Args>
        void emplace(Args&&... args)
        {
            m_ptr = std::make_unique<T>(args...);
        }

        void reset() noexcept
        {
            m_ptr = nullptr;
        }

        T& operator*()
        {
            return *m_ptr;
        }

        const T& operator*() const
        {
            return *m_ptr;
        }

        T* operator->() noexcept
        {
            return m_ptr.get();
        }

        const T* operator->() const noexcept
        {
            return m_ptr.get();
        }

        Optional& operator=(std::nullptr_t) noexcept
        {
            m_ptr = nullptr;
            return *this;
        }

        Optional& operator=(const T& val) noexcept
        {
            m_ptr = std::make_unique<T>(val);
            return *this;
        }

        Optional& operator=(T&& val) noexcept
        {
            m_ptr = std::make_unique<T>(std::move(val));
            return *this;
        }

        Optional& operator=(const Optional& other) noexcept
        {
            if (this == &other)
                return *this;

            m_ptr = other.m_ptr ? std::make_unique<T>(*other.m_ptr) : nullptr;
            return *this;
        }

        Optional& operator=(Optional&& val) noexcept = default;

        TGUI_NODISCARD bool operator==(std::nullptr_t) const noexcept
        {
            return m_ptr == nullptr;
        }

        TGUI_NODISCARD bool operator!=(std::nullptr_t) const noexcept
        {
            return m_ptr != nullptr;
        }

        explicit operator bool() const noexcept
        {
            return m_ptr != nullptr;
        }

        TGUI_NODISCARD bool has_value() const noexcept
        {
            return m_ptr != nullptr;
        }

        TGUI_NODISCARD const T& value() const
        {
            return *m_ptr;
        }

        TGUI_NODISCARD T& value()
        {
            return *m_ptr;
        }

    private:
        std::unique_ptr<T> m_ptr;
    };
#endif
}

#endif // TGUI_OPTIONAL_HPP
