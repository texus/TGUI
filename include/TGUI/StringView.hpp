/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_STRING_VIEW_HPP
#define TGUI_STRING_VIEW_HPP

#include <string>

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #include <string_view>
#else
    #include <algorithm> // min
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    using StringView = std::u32string_view;
    using CharStringView = std::string_view;

    inline namespace literals
    {
        inline namespace string_view_literals
        {
            // Allow using operator ""sv
            // Note that this only affects code placed inside the tgui namespace.
            using namespace std::literals::string_view_literals;
        }
    }
#else
    template <typename Type>
    struct TypeIdentity
    {
        using type = Type;
    };

    template<typename Type>
    using TypeIdentity_t = typename TypeIdentity<Type>::type;

    template <typename CharType>
    class StringViewImpl
    {
    public:
        using const_iterator = const CharType*;

        constexpr StringViewImpl() = default;
        constexpr StringViewImpl(const StringViewImpl& other) = default;

        constexpr StringViewImpl(const CharType* str, std::size_t strLength) :
            m_string(str),
            m_length(strLength)
        {
        }

        constexpr StringViewImpl(const CharType* str) :
            m_string(str),
            m_length(std::char_traits<CharType>::length(str))
        {
        }

        constexpr StringViewImpl(const std::basic_string<CharType>& str) :
            m_string(str.data()),
            m_length(str.length())
        {
        }

        constexpr const_iterator begin() const noexcept
        {
            return m_string;
        }
        constexpr const_iterator cbegin() const noexcept
        {
            return m_string;
        }

        constexpr const_iterator end() const noexcept
        {
            return m_string + static_cast<std::ptrdiff_t>(m_length);
        }
        constexpr const_iterator cend() const noexcept
        {
            return m_string + static_cast<std::ptrdiff_t>(m_length);
        }

        constexpr const CharType& operator[](std::size_t index) const
        {
            return m_string[index];
        }

        constexpr const CharType& front() const
        {
            return m_string[0];
        }

        constexpr const CharType& back() const
        {
            return m_string[m_length-1];
        }

        constexpr const CharType* data() const noexcept
        {
            return m_string;
        }

        constexpr std::size_t size() const noexcept
        {
            return m_length;
        }
        constexpr std::size_t length() const noexcept
        {
            return m_length;
        }

        constexpr bool empty() const noexcept
        {
            return (m_length == 0);
        }

        constexpr StringViewImpl substr(std::size_t pos = 0, std::size_t count = std::u32string::npos) const
        {
            if (count != std::u32string::npos)
                return StringViewImpl(&m_string[pos], count);
            else
                return StringViewImpl(&m_string[pos], m_length - pos);
        }

        constexpr int compare(StringViewImpl strView) const noexcept
        {
            const std::size_t rlen = std::min(length(), strView.length());
            const int ret = std::char_traits<CharType>::compare(data(), strView.data(), rlen);
            if (ret != 0)
                return ret;

            if (length() < strView.length())
                return -1;
            else if (length() > strView.length())
                return 1;
            else
                return 0;
        }

        constexpr std::size_t find(StringViewImpl strView, std::size_t pos = 0) const noexcept
        {
            if (empty() || (strView.length() > m_length))
                return std::u32string::npos;

            if (strView.empty())
                return pos;

            for (std::size_t i = pos; i <= m_length - strView.length(); ++i)
            {
                if (m_string[i] != strView[0])
                    continue;

                bool found = true;
                for (std::size_t j = 1; j < strView.length(); ++j)
                {
                    if (m_string[i+j] != strView[j])
                    {
                        found = false;
                        break;
                    }
                }

                if (found)
                    return i;
            }

            return std::u32string::npos;
        }

        constexpr std::size_t find(CharType ch, std::size_t pos = 0) const noexcept
        {
            return find(StringViewImpl(&ch, 1), pos);
        }
        constexpr std::size_t find(const CharType* str, std::size_t pos, std::size_t count) const
        {
            return find(StringViewImpl(str, count), pos);
        }
        constexpr std::size_t find(const CharType* str, std::size_t pos = 0) const
        {
            return find(StringViewImpl(str), pos);
        }

    private:
        const CharType* m_string = nullptr;
        std::size_t m_length = 0;
    };

    template <typename CharType>
    constexpr bool operator==(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) == 0;
    }

    template <typename CharType>
    constexpr bool operator!=(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) != 0;
    }

    template <typename CharType>
    constexpr bool operator<(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) < 0;
    }

    template <typename CharType>
    constexpr bool operator<=(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) <= 0;
    }

    template <typename CharType>
    constexpr bool operator>(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) > 0;
    }

    template <typename CharType>
    constexpr bool operator>=(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) >= 0;
    }

    using StringView = StringViewImpl<char32_t>;
    using CharStringView = StringViewImpl<char>;

    // Allow using operator ""sv
    // Note that this only affects code placed inside the tgui namespace.
#if defined(__GNUC__) && !defined(__clang__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif
    inline namespace literals
    {
        inline namespace string_view_literals
        {
            inline constexpr StringViewImpl<char> operator""sv(const char* str, size_t len) noexcept
            {
                return StringViewImpl<char>{str, len};
            }

            inline constexpr StringViewImpl<wchar_t> operator""sv(const wchar_t* str, size_t len) noexcept
            {
                return StringViewImpl<wchar_t>{str, len};
            }

            inline constexpr StringViewImpl<char16_t> operator""sv(const char16_t* str, size_t len) noexcept
            {
                return StringViewImpl<char16_t>{str, len};
            }

            inline constexpr StringViewImpl<char32_t> operator""sv(const char32_t* str, size_t len) noexcept
            {
                return StringViewImpl<char32_t>{str, len};
            }
        }
    }
#if defined(__GNUC__) && !defined(__clang__)
#   pragma GCC diagnostic pop
#endif

#endif

#if TGUI_COMPILED_WITH_CPP_VER >= 17 && defined(__cpp_lib_starts_ends_with) && (__cpp_lib_starts_ends_with >= 201711L)
    inline bool viewStartsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        return viewToLookInto.starts_with(viewToLookFor);
    }

    inline bool viewStartsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return viewToLookInto.starts_with(charToLookFor);
    }

    inline bool viewEndsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        return viewToLookInto.ends_with(viewToLookFor);
    }

    inline bool viewEndsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return viewToLookInto.ends_with(charToLookFor);
    }
#else
    inline bool viewStartsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return viewToLookInto.substr(0, viewToLookFor.length()) == viewToLookFor;
    }

    inline bool viewStartsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.front() == charToLookFor);
    }

    inline bool viewEndsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return StringView(viewToLookInto.data() + (viewToLookInto.length() - viewToLookFor.length()), viewToLookFor.length()).compare(viewToLookFor) == 0;
    }

    inline bool viewEndsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.back() == charToLookFor);
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_STRING_VIEW_HPP
