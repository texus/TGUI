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


#ifndef TGUI_STRING_VIEW_HPP
#define TGUI_STRING_VIEW_HPP

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <string>
    #include <cctype> // tolower

    #if TGUI_COMPILED_WITH_CPP_VER >= 17
        #include <string_view>
    #else
        #include <algorithm> // min
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER >= 17
namespace tgui
{
    inline namespace literals
    {
        inline namespace string_view_literals
        {
            // Allow using operator ""sv
            // Note that this only affects code placed inside the tgui namespace.
            using namespace std::literals::string_view_literals;
        }
    }
}
#endif

TGUI_MODULE_EXPORT namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    using StringView = std::u32string_view;
    using CharStringView = std::string_view;
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

        TGUI_NODISCARD constexpr const_iterator begin() const noexcept
        {
            return m_string;
        }
        TGUI_NODISCARD constexpr const_iterator cbegin() const noexcept
        {
            return m_string;
        }

        TGUI_NODISCARD constexpr const_iterator end() const noexcept
        {
            return m_string + static_cast<std::ptrdiff_t>(m_length);
        }
        TGUI_NODISCARD constexpr const_iterator cend() const noexcept
        {
            return m_string + static_cast<std::ptrdiff_t>(m_length);
        }

        TGUI_NODISCARD constexpr const CharType& operator[](std::size_t index) const
        {
            return m_string[index];
        }

        TGUI_NODISCARD constexpr const CharType& front() const
        {
            return m_string[0];
        }

        TGUI_NODISCARD constexpr const CharType& back() const
        {
            return m_string[m_length-1];
        }

        TGUI_NODISCARD constexpr const CharType* data() const noexcept
        {
            return m_string;
        }

        TGUI_NODISCARD constexpr std::size_t size() const noexcept
        {
            return m_length;
        }
        TGUI_NODISCARD constexpr std::size_t length() const noexcept
        {
            return m_length;
        }

        TGUI_NODISCARD constexpr bool empty() const noexcept
        {
            return (m_length == 0);
        }

        TGUI_NODISCARD constexpr StringViewImpl substr(std::size_t pos = 0, std::size_t count = std::u32string::npos) const
        {
            if (count != std::u32string::npos)
                return StringViewImpl(&m_string[pos], count);
            else
                return StringViewImpl(&m_string[pos], m_length - pos);
        }

        TGUI_NODISCARD constexpr int compare(StringViewImpl strView) const noexcept
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

        TGUI_NODISCARD constexpr std::size_t find(StringViewImpl strView, std::size_t pos = 0) const noexcept
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

        TGUI_NODISCARD constexpr std::size_t find(CharType ch, std::size_t pos = 0) const noexcept
        {
            return find(StringViewImpl(&ch, 1), pos);
        }
        TGUI_NODISCARD constexpr std::size_t find(const CharType* str, std::size_t pos, std::size_t count) const
        {
            return find(StringViewImpl(str, count), pos);
        }
        TGUI_NODISCARD constexpr std::size_t find(const CharType* str, std::size_t pos = 0) const
        {
            return find(StringViewImpl(str), pos);
        }

    private:
        const CharType* m_string = nullptr;
        std::size_t m_length = 0;
    };

    template <typename CharType>
    TGUI_NODISCARD constexpr bool operator==(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) == 0;
    }

    template <typename CharType>
    TGUI_NODISCARD constexpr bool operator!=(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) != 0;
    }

    template <typename CharType>
    TGUI_NODISCARD constexpr bool operator<(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) < 0;
    }

    template <typename CharType>
    TGUI_NODISCARD constexpr bool operator<=(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) <= 0;
    }

    template <typename CharType>
    TGUI_NODISCARD constexpr bool operator>(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) > 0;
    }

    template <typename CharType>
    TGUI_NODISCARD constexpr bool operator>=(StringViewImpl<CharType> lhs, TypeIdentity_t<StringViewImpl<CharType>> rhs) noexcept
    {
       return lhs.compare(rhs) >= 0;
    }

    using StringView = StringViewImpl<char32_t>;
    using CharStringView = StringViewImpl<char>;

    // Allow using operator ""sv
    // Note that this only affects code placed inside the tgui namespace.
#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wuser-defined-literals"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wliteral-suffix"
#elif defined (_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4455) // literal suffix identifiers that do not start with an underscore are reserved
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
#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined (_MSC_VER)
#   pragma warning(pop)
#endif

#endif // TGUI_COMPILED_WITH_CPP_VER

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns whether two view are equal if letters would have been lowercase
    ///
    /// @param view1  First view to compare
    /// @param view2  Second view to compare
    ///
    /// @return Are the views equal except for the case of letters?
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEqualIgnoreCase(CharStringView view1, CharStringView view2)
    {
        return std::equal(view1.begin(), view1.end(), view2.begin(), view2.end(),
            [](char char1, char char2)
            {
                if (char1 == char2)
                    return true;
                else
                    return std::tolower(static_cast<unsigned char>(char1)) == std::tolower(static_cast<unsigned char>(char2));
            }
        );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns whether two view are equal if ASCII letters would have been lowercase
    ///
    /// @param view1  First view to compare
    /// @param view2  Second view to compare
    ///
    /// @return Are the views equal except for the case of ASCII letters?
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEqualIgnoreCase(StringView view1, StringView view2)
    {
        return std::equal(view1.begin(), view1.end(), view2.begin(), view2.end(),
            [](char32_t char1, char32_t char2)
            {
                if (char1 == char2)
                    return true;
                else if ((char1 < 128) && (char2 < 128))
                    return std::tolower(static_cast<unsigned char>(char1)) == std::tolower(static_cast<unsigned char>(char2));
                else
                    return false;
            }
        );
    }

#if TGUI_COMPILED_WITH_CPP_VER >= 17 && defined(__cpp_lib_starts_ends_with) && (__cpp_lib_starts_ends_with >= 201711L)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        return viewToLookInto.starts_with(viewToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return viewToLookInto.starts_with(charToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        return viewToLookInto.ends_with(viewToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return viewToLookInto.ends_with(charToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        return viewToLookInto.starts_with(viewToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return viewToLookInto.starts_with(charToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        return viewToLookInto.ends_with(viewToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return viewToLookInto.ends_with(charToLookFor);
    }
#else
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return viewToLookInto.substr(0, viewToLookFor.length()) == viewToLookFor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.front() == charToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return CharStringView(viewToLookInto.data() + (viewToLookInto.length() - viewToLookFor.length()), viewToLookFor.length()).compare(viewToLookFor) == 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.back() == charToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return viewToLookInto.substr(0, viewToLookFor.length()) == viewToLookFor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewStartsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.front() == charToLookFor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return StringView(viewToLookInto.data() + (viewToLookInto.length() - viewToLookFor.length()), viewToLookFor.length()).compare(viewToLookFor) == 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD inline bool viewEndsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.back() == charToLookFor);
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_STRING_VIEW_HPP
