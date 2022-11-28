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


#ifndef TGUI_STRING_HPP
#define TGUI_STRING_HPP

#include <TGUI/Config.hpp>
#include <TGUI/Utf.hpp>
#include <string>
#include <vector>
#include <cstring>
#include <locale>
#include <iomanip>
#include <ostream>
#include <sstream>

#if TGUI_HAS_WINDOW_BACKEND_SFML
    #include <SFML/System/String.hpp>
#endif

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #include <type_traits>
    #include <string_view>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks if a character is a whitespace character (space, tab, carriage return or line feed)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API bool isWhitespace(char character);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks if a character is a whitespace character (space, tab, carriage return or line feed)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API bool isWhitespace(char32_t character);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper class to store strings
    ///
    /// A constructor and conversion operator are provided for sf::String to easily convert between sf::String and tgui::String.
    ///
    /// The interface is similar to std::basic_string, but functions accept char, wchar_t, char8_t, char16_t and char32_t.
    /// Some extra helper functions are also provided to e.g. convert the string to an integer or float or to lowercase.
    ///
    /// Note that when converting to std::string, an UTF-8 encoded string will be returned.
    /// Similarly, when passing an std::string or const char* to this class, the encoding is assumed to be UTF-8.
    ///
    /// Data is stored in UTF-32, so any parameter or operator using a different encoding will have to convert the string
    /// internally and may be slightly slower than the variants that use UTF-32.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API String
    {
    private:
        std::u32string m_string;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        // Helper to check if template parameter is a string_view
        template <typename StringViewType>
        using IsStringViewType = std::enable_if_t<
            std::is_same_v<StringViewType, std::string_view>
#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
            || std::is_same_v<StringViewType, std::u8string_view>
#endif
            || std::is_same_v<StringViewType, std::wstring_view>
            || std::is_same_v<StringViewType, std::u16string_view>
            || std::is_same_v<StringViewType, std::u32string_view>,
            void>;
#endif

    public:

        static const decltype(std::u32string::npos) npos;

        using iterator = std::u32string::iterator;
        using const_iterator = std::u32string::const_iterator;
        using reverse_iterator = std::u32string::reverse_iterator;
        using const_reverse_iterator = std::u32string::const_reverse_iterator;

        using value_type = char32_t;
        using reference = char32_t&;
        using const_reference = const char32_t&;


    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to an integer
        ///
        /// @param result  Integer value if the string contains a base 10 integer. Unmodified if string is invalid.
        ///
        /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool attemptToInt(int& result) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to an unsigned int
        ///
        /// @param result  Unsigned integer value if the string contains a base 10 unsigned int. Unmodified if string is invalid.
        ///
        /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool attemptToUInt(unsigned int& result) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to a float
        ///
        /// @param result  Float value if the string contains a float. Unmodified if string is invalid.
        ///
        /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool attemptToFloat(float& result) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to an integer
        ///
        /// @param defaultValue  Value to return if conversion fails
        ///
        /// @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int toInt(int defaultValue = 0) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to an unsigned int
        ///
        /// @param defaultValue  Value to return if conversion fails
        ///
        /// @return Returns the unsigned integer value or defaultValue if the string didn't contain a base 10 unsigned integer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int toUInt(unsigned int defaultValue = 0) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to a float
        ///
        /// @param defaultValue  Value to return if conversion fails
        ///
        /// @return Returns the float value or defaultValue if the string didn't contain an float
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float toFloat(float defaultValue = 0) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a string with the whitespace at the start and end of this string removed
        ///
        /// @return Trimmed string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String trim() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the ASCII characters in the string to lowercase
        ///
        /// @return Lowercase string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String toLower() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the ASCII characters in the string to uppercase
        ///
        /// @return Uppercase string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String toUpper() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares this string to another and checks if they are equal if ASCII letters would have been lowercase
        ///
        /// @param other  The other string to compare this one with
        ///
        /// @return Are the strings equal except for the case of ASCII letters?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool equalIgnoreCase(const String& other) const;

#ifndef TGUI_REMOVE_DEPRECATED_CODE
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks whether the first part of the string matches the given substring
        ///
        /// @param substring  Characters to compare against the first part of the string
        ///
        /// @return Does the first part of the string match the given substring?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("Use starts_with instead") bool startsWith(const String& substring) const;
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks whether the first part of the string matches the given substring, case-insensitive
        ///
        /// @param substring  Characters to compare against the first part of the string
        ///
        /// @return Does the first part of the string match the given substring if we ignore the case?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool startsWithIgnoreCase(const String& substring) const;

#ifndef TGUI_REMOVE_DEPRECATED_CODE
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks whether the last part of the string matches the given substring
        ///
        /// @param substring  Characters to compare against the final part of the string
        ///
        /// @return Does the back of the string match the given substring?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("Use ends_with instead") bool endsWith(const String& substring) const;
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks whether the last part of the string matches the given substring, case-insensitive
        ///
        /// @param substring  Characters to compare against the final part of the string
        ///
        /// @return Does the back of the string match the given substring if we ignore the case?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool endsWithIgnoreCase(const String& substring) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Replaces all occurrences of a substring with a replacement string
        ///
        /// @param searchFor   The value being searched for
        /// @param replaceWith The value that replaces found searchFor values
        ///
        /// @return Reference to this object.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String& replace(const String& searchFor, const String& replaceWith);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Splits the string into multiple substrings given the delimiter that separates them
        ///
        /// @param delimiter  Character that separates the substrings
        /// @param trim       Should whitespace be removed at the start and end of each part?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<String> split(char32_t delimiter, bool trim = false) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Joins multiple string segments into a single string
        ///
        /// @param segments   Substrings that need to concatenated behind each other (with optional separators inbetween)
        /// @param separator  Character that separates the substrings
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static String join(const std::vector<String>& segments, const String& separator);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the string from a number
        ///
        /// @param value  Number to convert to string
        ///
        /// @return String representing given number
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        static String fromNumber(T value)
        {
            std::ostringstream oss;
            oss.imbue(std::locale::classic());
            oss << value;
            return {oss.str()};
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the string from a floating point number, keeping only a certain amount of decimals behind the comma
        ///
        /// @param value     Number to convert to string
        /// @param decimals  Digits to keep behind the comma
        ///
        /// @return String representing given number, rounded to the given decimals
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        static String fromNumberRounded(T value, unsigned int decimals)
        {
            std::ostringstream oss;
            oss.imbue(std::locale::classic());
            oss << std::fixed << std::setprecision(static_cast<int>(decimals));
            oss << value;
            return {oss.str()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        String() = default;

        String(const std::string& str);
        String(const std::wstring& str);
        String(const std::u16string& str);
        String(const std::u32string& str);

        String(std::u32string&& str) :
            m_string{std::move(str)}
        {
        }

        String(char ansiChar);
        String(wchar_t wideChar);
        String(char16_t utfChar);
        String(char32_t utfChar);

        String(const char* str);
        String(const wchar_t* str);
        String(const char16_t* str);
        String(const char32_t* str) :
            m_string{str}
        {
        }

        // Constructor to initialize the string from a number (integer or float)
        template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
        explicit String(T number) :
            String{fromNumber(number)}
        {
        }

        String(std::size_t count, char ch);
        String(std::size_t count, wchar_t ch);
        String(std::size_t count, char16_t ch);
        String(std::size_t count, char32_t ch);

        String(const std::string& str, std::size_t pos);
        String(const std::wstring& str, std::size_t pos);
        String(const std::u16string& str, std::size_t pos);
        String(const std::u32string& str, std::size_t pos);

        String(const std::string& str, std::size_t pos, std::size_t count);
        String(const std::wstring& str, std::size_t pos, std::size_t count);
        String(const std::u16string& str, std::size_t pos, std::size_t count);
        String(const std::u32string& str, std::size_t pos, std::size_t count);

        String(const char* str, std::size_t count);
        String(const wchar_t* str, std::size_t count);
        String(const char16_t* str, std::size_t count);
        String(const char32_t* str, std::size_t count);

        explicit String(std::initializer_list<char> chars);
        explicit String(std::initializer_list<wchar_t> chars);
        explicit String(std::initializer_list<char16_t> chars);
        explicit String(std::initializer_list<char32_t> chars);

        // Constructors using iterators have to be explicit to prevent {"1", "2"} to be ambiguous between String and std::vector<String>.
        // The reason these constructors were considered a candicate to clang is due to a private constructor in the iterator class.
        // We can't accept string_view::const_iterator iterators, because it would lead to the same kind of ambiguity even when the
        // constructor is marked explicit (in GCC but not MSVC if we define the function normally, in MSVC but not GCC if we use a template).
        explicit String(std::string::const_iterator first, std::string::const_iterator last);
        explicit String(std::wstring::const_iterator first, std::wstring::const_iterator last);
        explicit String(std::u16string::const_iterator first, std::u16string::const_iterator last);
        explicit String(std::u32string::const_iterator first, std::u32string::const_iterator last);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        template <typename StringViewType, typename = IsStringViewType<StringViewType>>
        explicit String(const StringViewType& stringView) :
            String(stringView.data(), stringView.size())
        {
        }

        template <typename StringViewType, typename = IsStringViewType<StringViewType>>
        explicit String(const StringViewType& stringView, std::size_t pos, std::size_t count) :
            String(stringView.data(), pos, count)
        {
        }
#endif

#if TGUI_HAS_WINDOW_BACKEND_SFML
        // This constructor has to be explicit or it will cause MSVC to no longer compile code that performs sf::String + std::string
        explicit String(const sf::String& str)
            : m_string{reinterpret_cast<const char32_t*>(str.toUtf32().c_str())}
        {
        }

        explicit operator sf::String() const
        {
            return sf::String::fromUtf32(m_string.begin(), m_string.end());
        }
#endif

        explicit operator std::string() const;
        explicit operator std::wstring() const;
        explicit operator std::u16string() const;
        explicit operator const std::u32string&() const
        {
            return m_string;
        }

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        operator std::u32string_view() const noexcept
        {
            return m_string;
        }
#endif

        std::string toStdString() const;
        std::wstring toWideString() const;
        std::u16string toUtf16() const;
        const std::u32string& toUtf32() const
        {
            return m_string;
        }

        String& assign(std::size_t count, char ch);
        String& assign(std::size_t count, wchar_t ch);
        String& assign(std::size_t count, char16_t ch);
        String& assign(std::size_t count, char32_t ch);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& assign(std::u32string_view sv);
#endif
        String& assign(const char32_t* str);
        String& assign(const std::u32string& str);
        String& assign(const String& str);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& assign(std::u32string_view sv, std::size_t pos, std::size_t count = npos);
#endif
        String& assign(const std::string& str, std::size_t pos, std::size_t count = npos);
        String& assign(const std::wstring& str, std::size_t pos, std::size_t count = npos);
        String& assign(const std::u16string& str, std::size_t pos, std::size_t count = npos);
        String& assign(const std::u32string& str, std::size_t pos, std::size_t count = npos);
        String& assign(const String& str, std::size_t pos, std::size_t count = npos);

        String& assign(std::u32string&& str);
        String& assign(String&& str);

        String& assign(const char* str, std::size_t count);
        String& assign(const wchar_t* str, std::size_t count);
        String& assign(const char16_t* str, std::size_t count);
        String& assign(const char32_t* str, std::size_t count);

        String& assign(std::initializer_list<char> chars);
        String& assign(std::initializer_list<wchar_t> chars);
        String& assign(std::initializer_list<char16_t> chars);
        String& assign(std::initializer_list<char32_t> chars);

        String& assign(std::string::const_iterator first, std::string::const_iterator last);
        String& assign(std::wstring::const_iterator first, std::wstring::const_iterator last);
        String& assign(std::u16string::const_iterator first, std::u16string::const_iterator last);
        String& assign(std::u32string::const_iterator first, std::u32string::const_iterator last);
        String& assign(std::u32string_view::const_iterator first, std::u32string_view::const_iterator last);

        reference       at(std::size_t pos);
        const_reference at(std::size_t pos) const;

        const_reference operator [](std::size_t index) const;
        reference operator [](std::size_t index);

        reference       front();
        const_reference front() const;

        reference       back();
        const_reference back() const;

        const char32_t* data() const noexcept
        {
            return m_string.data();
        }

#if __cplusplus >= 201703L
        char32_t* data() noexcept
        {
            return m_string.data();
        }
#endif
        const char32_t* c_str() const noexcept
        {
            return m_string.c_str();
        }

        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;

        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

        reverse_iterator rbegin() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator crbegin() const noexcept;

        reverse_iterator rend() noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crend() const noexcept;

        bool empty() const noexcept
        {
            return m_string.empty();
        }

        std::size_t size() const noexcept
        {
            return m_string.size();
        }

        std::size_t length() const noexcept
        {
            return m_string.length();
        }

        std::size_t max_size() const noexcept;

        void reserve(std::size_t newCap);
        std::size_t capacity() const noexcept;
        void shrink_to_fit();

        void clear() noexcept;

        String& insert(std::size_t index, std::size_t count, char ch);
        String& insert(std::size_t index, std::size_t count, wchar_t ch);
        String& insert(std::size_t index, std::size_t count, char16_t ch);
        String& insert(std::size_t index, std::size_t count, char32_t ch);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& insert(std::size_t index, std::u32string_view sv);
#endif
        String& insert(std::size_t index, const char32_t* str);
        String& insert(std::size_t index, const std::u32string& str);
        String& insert(std::size_t index, const String& str);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& insert(std::size_t index, std::u32string_view sv, std::size_t pos, std::size_t count = npos);
#endif
        String& insert(std::size_t index, const std::string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const std::wstring& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const std::u16string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const std::u32string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const String& str, std::size_t pos, std::size_t count = npos);

        String& insert(std::size_t index, const char* str, std::size_t count);
        String& insert(std::size_t index, const wchar_t* str, std::size_t count);
        String& insert(std::size_t index, const char16_t* str, std::size_t count);
        String& insert(std::size_t index, const char32_t* str, std::size_t count);

        iterator insert(const_iterator pos, char ch);
        iterator insert(const_iterator pos, wchar_t ch);
        iterator insert(const_iterator pos, char16_t ch);
        iterator insert(const_iterator pos, char32_t ch);

        iterator insert(const_iterator pos, std::size_t count, char ch);
        iterator insert(const_iterator pos, std::size_t count, wchar_t ch);
        iterator insert(const_iterator pos, std::size_t count, char16_t ch);
        iterator insert(const_iterator pos, std::size_t count, char32_t ch);

        iterator insert(const_iterator pos, std::initializer_list<char> chars);
        iterator insert(const_iterator pos, std::initializer_list<wchar_t> chars);
        iterator insert(const_iterator pos, std::initializer_list<char16_t> chars);
        iterator insert(const_iterator pos, std::initializer_list<char32_t> chars);

        iterator insert(const_iterator pos, std::string::const_iterator first, std::string::const_iterator last);
        iterator insert(const_iterator pos, std::wstring::const_iterator first, std::wstring::const_iterator last);
        iterator insert(const_iterator pos, std::u16string::const_iterator first, std::u16string::const_iterator last);
        iterator insert(const_iterator pos, std::u32string::const_iterator first, std::u32string::const_iterator last);
        iterator insert(const_iterator pos, std::u32string_view::const_iterator first, std::u32string_view::const_iterator last);

        String& erase(std::size_t index = 0, std::size_t count = npos);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);

        void push_back(char ch);
        void push_back(wchar_t ch);
        void push_back(char16_t ch);
        void push_back(char32_t ch);

        void pop_back();

        String& append(std::size_t count, char ch);
        String& append(std::size_t count, wchar_t ch);
        String& append(std::size_t count, char16_t ch);
        String& append(std::size_t count, char32_t ch);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& append(std::u32string_view sv);
#endif
        String& append(const char32_t* str);
        String& append(const std::u32string& str);
        String& append(const String& str);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& append(std::u32string_view sv, std::size_t pos, std::size_t count = npos);
#endif
        String& append(const std::string& str, std::size_t pos, std::size_t count = npos);
        String& append(const std::wstring& str, std::size_t pos, std::size_t count = npos);
        String& append(const std::u16string& str, std::size_t pos, std::size_t count = npos);
        String& append(const std::u32string& str, std::size_t pos, std::size_t count = npos);
        String& append(const String& str, std::size_t pos, std::size_t count = npos);

        String& append(const char* str, std::size_t count);
        String& append(const wchar_t* str, std::size_t count);
        String& append(const char16_t* str, std::size_t count);
        String& append(const char32_t* str, std::size_t count);

        String& append(std::string::const_iterator first, std::string::const_iterator last);
        String& append(std::wstring::const_iterator first, std::wstring::const_iterator last);
        String& append(std::u16string::const_iterator first, std::u16string::const_iterator last);
        String& append(std::u32string::const_iterator first, std::u32string::const_iterator last);
        String& append(std::u32string_view::const_iterator first, std::u32string_view::const_iterator last);

        String& append(std::initializer_list<char> chars);
        String& append(std::initializer_list<wchar_t> chars);
        String& append(std::initializer_list<char16_t> chars);
        String& append(std::initializer_list<char32_t> chars);

        String& operator+=(const String& str);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        int compare(std::u32string_view sv) const noexcept;
#endif
        int compare(const char32_t* s) const;
        int compare(const std::u32string& str) const noexcept;
        int compare(const String& str) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        int compare(std::size_t pos1, std::size_t count1, std::u32string_view sv) const;
#endif
        int compare(std::size_t pos1, std::size_t count1, const char32_t* s) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u32string& str) const;
        int compare(std::size_t pos1, std::size_t count1, const String& str) const;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        int compare(std::size_t pos1, std::size_t count1, std::u32string_view sv, std::size_t pos2, std::size_t count2 = npos) const;
#endif
        int compare(std::size_t pos1, std::size_t count1, const std::string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const std::wstring& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u16string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u32string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const String& str, std::size_t pos2, std::size_t count2 = npos) const;

        int compare(std::size_t pos1, std::size_t count1, const char* s, std::size_t count2) const;
        int compare(std::size_t pos1, std::size_t count1, const wchar_t* s, std::size_t count2) const;
        int compare(std::size_t pos1, std::size_t count1, const char16_t* s, std::size_t count2) const;
        int compare(std::size_t pos1, std::size_t count1, const char32_t* s, std::size_t count2) const;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& replace(std::size_t pos, std::size_t count, std::u32string_view sv);
#endif
        String& replace(std::size_t pos, std::size_t count, const char32_t* cstr);
        String& replace(std::size_t pos, std::size_t count, const std::u32string& str);
        String& replace(std::size_t pos, std::size_t count, const String& str);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& replace(const_iterator first, const_iterator last, std::u32string_view sv);
#endif
        String& replace(const_iterator first, const_iterator last, const char32_t* cstr);
        String& replace(const_iterator first, const_iterator last, const std::u32string& str);
        String& replace(const_iterator first, const_iterator last, const String& str);

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        String& replace(std::size_t pos, std::size_t count, std::u32string_view sv, std::size_t pos2, std::size_t count2 = npos);
#endif
        String& replace(std::size_t pos, std::size_t count, const std::string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const std::wstring& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const std::u16string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const std::u32string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const String& str, std::size_t pos2, std::size_t count2 = npos);

        String& replace(const_iterator first, const_iterator last, std::string::const_iterator first2, std::string::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::wstring::const_iterator first2, std::wstring::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::u16string::const_iterator first2, std::u16string::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::u32string::const_iterator first2, std::u32string::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::u32string_view::const_iterator first2, std::u32string_view::const_iterator last2);

        String& replace(std::size_t pos, std::size_t count, const char* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const wchar_t* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const char16_t* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const char32_t* cstr, std::size_t count2);

        String& replace(const_iterator first, const_iterator last, const char* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const wchar_t* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const char16_t* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const char32_t* cstr, std::size_t count2);

        String& replace(std::size_t pos, std::size_t count, std::size_t count2, char ch);
        String& replace(std::size_t pos, std::size_t count, std::size_t count2, wchar_t ch);
        String& replace(std::size_t pos, std::size_t count, std::size_t count2, char16_t ch);
        String& replace(std::size_t pos, std::size_t count, std::size_t count2, char32_t ch);

        String& replace(const_iterator first, const_iterator last, std::size_t count2, char ch);
        String& replace(const_iterator first, const_iterator last, std::size_t count2, wchar_t ch);
        String& replace(const_iterator first, const_iterator last, std::size_t count2, char16_t ch);
        String& replace(const_iterator first, const_iterator last, std::size_t count2, char32_t ch);

        String& replace(const_iterator first, const_iterator last, std::initializer_list<char> chars);
        String& replace(const_iterator first, const_iterator last, std::initializer_list<wchar_t> chars);
        String& replace(const_iterator first, const_iterator last, std::initializer_list<char16_t> chars);
        String& replace(const_iterator first, const_iterator last, std::initializer_list<char32_t> chars);

        String substr(std::size_t pos = 0, std::size_t count = npos) const;

        std::size_t copy(char32_t* dest, std::size_t count, std::size_t pos = 0) const;

        void resize(std::size_t count);
        void resize(std::size_t count, char ch);
        void resize(std::size_t count, wchar_t ch);
        void resize(std::size_t count, char16_t ch);
        void resize(std::size_t count, char32_t ch);

        void swap(String& other);

        bool contains(char c) const noexcept;
        bool contains(wchar_t c) const noexcept;
        bool contains(char16_t c) const noexcept;
        bool contains(char32_t c) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        bool contains(std::u32string_view sv) const noexcept;
#endif
        bool contains(const char32_t* s) const;
        bool contains(const std::u32string& s) const;
        bool contains(const String& s) const;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::size_t find(std::u32string_view sv, std::size_t pos = 0) const noexcept;
#endif
        std::size_t find(const char32_t* s, std::size_t pos = 0) const;
        std::size_t find(const std::u32string& str, std::size_t pos = 0) const noexcept;
        std::size_t find(const String& str, std::size_t pos = 0) const noexcept;

        std::size_t find(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find(char ch, std::size_t pos = 0) const noexcept;
        std::size_t find(wchar_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find(char16_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find(char32_t ch, std::size_t pos = 0) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::size_t find_first_of(std::u32string_view sv, std::size_t pos = 0) const noexcept;
#endif
        std::size_t find_first_of(const char32_t* s, std::size_t pos = 0) const;
        std::size_t find_first_of(const std::u32string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(const String& str, std::size_t pos = 0) const noexcept;

        std::size_t find_first_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_first_of(char ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(wchar_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(char16_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(char32_t ch, std::size_t pos = 0) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::size_t find_first_not_of(std::u32string_view sv, std::size_t pos = 0) const noexcept;
#endif
        std::size_t find_first_not_of(const char32_t* s, std::size_t pos = 0) const;
        std::size_t find_first_not_of(const std::u32string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(const String& str, std::size_t pos = 0) const noexcept;

        std::size_t find_first_not_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_first_not_of(char ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(wchar_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(char16_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(char32_t ch, std::size_t pos = 0) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::size_t rfind(std::u32string_view sv, std::size_t pos = npos) const noexcept;
#endif
        std::size_t rfind(const char32_t* s, std::size_t pos = npos) const;
        std::size_t rfind(const std::u32string& str, std::size_t pos = npos) const noexcept;
        std::size_t rfind(const String& str, std::size_t pos = npos) const noexcept;

        std::size_t rfind(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t rfind(char ch, std::size_t pos = npos) const noexcept;
        std::size_t rfind(wchar_t ch, std::size_t pos = npos) const noexcept;
        std::size_t rfind(char16_t ch, std::size_t pos = npos) const noexcept;
        std::size_t rfind(char32_t ch, std::size_t pos = npos) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::size_t find_last_of(std::u32string_view sv, std::size_t pos = npos) const noexcept;
#endif
        std::size_t find_last_of(const char32_t* s, std::size_t pos = npos) const;
        std::size_t find_last_of(const std::u32string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(const String& str, std::size_t pos = npos) const noexcept;

        std::size_t find_last_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_last_of(char ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(wchar_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(char16_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(char32_t ch, std::size_t pos = npos) const noexcept;

#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::size_t find_last_not_of(std::u32string_view sv, std::size_t pos = npos) const noexcept;
#endif
        std::size_t find_last_not_of(const char32_t* s, std::size_t pos = npos) const;
        std::size_t find_last_not_of(const std::u32string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(const String& str, std::size_t pos = npos) const noexcept;

        std::size_t find_last_not_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_last_not_of(char ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(wchar_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(char16_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(char32_t ch, std::size_t pos = npos) const noexcept;

        inline friend bool operator==(const String& left, const String& right);
        inline friend bool operator!=(const String& left, const String& right);
        inline friend bool operator<(const String& left, const String& right);
        inline friend bool operator<=(const String& left, const String& right);
        inline friend bool operator>(const String& left, const String& right);
        inline friend bool operator>=(const String& left, const String& right);
        inline friend String operator+(const String& left, const String& right);
        inline friend String operator+(const String& left, String&& right);
        inline friend String operator+(String&& left, const String& right);
        inline friend String operator+(String&& left, String&& right);

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        inline String(const std::u8string& str);
        inline String(char8_t utfChar);
        inline String(const char8_t* str);
        inline String(std::size_t count, char8_t ch);
        inline String(const std::u8string& str, std::size_t pos);
        inline String(const std::u8string& str, std::size_t pos, std::size_t count);
        inline String(const char8_t* str, std::size_t count);
        inline explicit String(std::initializer_list<char8_t> chars);
        inline explicit String(std::u8string::const_iterator first, std::u8string::const_iterator last);

        inline explicit operator std::u8string() const;

        inline std::u8string toUtf8() const;

        inline String& assign(std::size_t count, char8_t ch);
        inline String& assign(const std::u8string& str, std::size_t pos, std::size_t count = npos);
        inline String& assign(const char8_t* str, std::size_t count);
        inline String& assign(std::initializer_list<char8_t> chars);
        inline String& assign(std::u8string::const_iterator first, std::u8string::const_iterator last);

        inline String& insert(std::size_t index, std::size_t count, char8_t ch);
        inline String& insert(std::size_t index, const std::u8string& str, std::size_t pos, std::size_t count = npos);
        inline String& insert(std::size_t index, const char8_t* str, std::size_t count);
        inline iterator insert(const_iterator pos, char8_t ch);
        inline iterator insert(const_iterator pos, std::size_t count, char8_t ch);
        inline iterator insert(const_iterator pos, std::initializer_list<char8_t> chars);
        inline iterator insert(const_iterator pos, std::u8string::const_iterator first, std::u8string::const_iterator last);

        inline String& append(std::size_t count, char8_t ch);
        inline String& append(const std::u8string& str, std::size_t pos, std::size_t count = npos);
        inline String& append(const char8_t* str, std::size_t count);
        inline String& append(std::initializer_list<char8_t> chars);
        inline String& append(std::u8string::const_iterator first, std::u8string::const_iterator last);

        inline int compare(std::size_t pos1, std::size_t count1, const std::u8string& str, std::size_t pos2, std::size_t count2 = npos) const;
        inline int compare(std::size_t pos1, std::size_t count1, const char8_t* s, std::size_t count2) const;

        inline String& replace(std::size_t pos, std::size_t count, const std::u8string& str, std::size_t pos2, std::size_t count2 = npos);
        inline String& replace(const_iterator first, const_iterator last, std::u8string::const_iterator first2, std::u8string::const_iterator last2);
        inline String& replace(std::size_t pos, std::size_t count, const char8_t* cstr, std::size_t count2);
        inline String& replace(const_iterator first, const_iterator last, const char8_t* cstr, std::size_t count2);
        inline String& replace(std::size_t pos, std::size_t count, std::size_t count2, char8_t ch);
        inline String& replace(const_iterator first, const_iterator last, std::size_t count2, char8_t ch);
        inline String& replace(const_iterator first, const_iterator last, std::initializer_list<char8_t> chars);

        inline void resize(std::size_t count, char8_t ch);

        inline bool contains(char8_t c) const noexcept;

        inline std::size_t find(const char8_t* s, std::size_t pos, std::size_t count) const;
        inline std::size_t find(char8_t ch, std::size_t pos = 0) const noexcept;

        inline std::size_t find_first_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        inline std::size_t find_first_of(char8_t ch, std::size_t pos = 0) const noexcept;

        inline std::size_t find_first_not_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        inline std::size_t find_first_not_of(char8_t ch, std::size_t pos = 0) const noexcept;

        inline std::size_t rfind(const char8_t* s, std::size_t pos, std::size_t count) const;
        inline std::size_t rfind(char8_t ch, std::size_t pos = npos) const noexcept;

        inline std::size_t find_last_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        inline std::size_t find_last_of(char8_t ch, std::size_t pos = npos) const noexcept;

        inline std::size_t find_last_not_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        inline std::size_t find_last_not_of(char8_t ch, std::size_t pos = npos) const noexcept;
#endif

#if TGUI_COMPILED_WITH_CPP_VER >= 17 && defined(__cpp_lib_starts_ends_with) && defined(__cpp_lib_char8_t) && (__cpp_lib_starts_ends_with >= 201711L) && (__cpp_lib_char8_t >= 201811L)
        inline bool starts_with(std::u32string_view sv) const noexcept;
        inline bool starts_with(const char32_t* s) const;
        inline bool starts_with(const std::u32string& s) const;
        inline bool starts_with(const String& s) const;

        inline bool starts_with(char ch) const noexcept;
        inline bool starts_with(wchar_t ch) const noexcept;
        inline bool starts_with(char8_t ch) const noexcept;
        inline bool starts_with(char16_t ch) const noexcept;
        inline bool starts_with(char32_t ch) const noexcept;

        inline bool ends_with(std::u32string_view sv) const noexcept;
        inline bool ends_with(const char32_t* s) const;
        inline bool ends_with(const std::u32string& s) const;
        inline bool ends_with(const String& s) const;

        inline bool ends_with(char ch) const noexcept;
        inline bool ends_with(wchar_t ch) const noexcept;
        inline bool ends_with(char8_t ch) const noexcept;
        inline bool ends_with(char16_t ch) const noexcept;
        inline bool ends_with(char32_t ch) const noexcept;
#else
        inline bool starts_with(const String& s) const;
        inline bool ends_with(const String& s) const;
#endif
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline bool operator==(const String& left, const String& right)
    {
        return left.m_string == right.m_string;
    }

    inline bool operator!=(const String& left, const String& right)
    {
        return left.m_string != right.m_string;
    }

    inline bool operator<(const String& left, const String& right)
    {
        return left.m_string < right.m_string;
    }

    inline bool operator<=(const String& left, const String& right)
    {
        return left.m_string <= right.m_string;
    }

    inline bool operator>(const String& left, const String& right)
    {
        return left.m_string > right.m_string;
    }

    inline bool operator>=(const String& left, const String& right)
    {
        return left.m_string >= right.m_string;
    }

    inline String operator+(const String& left, const String& right)
    {
        return {left.m_string + right.m_string};
    }
    inline String operator+(String&& left, String&& right)
    {
        return {std::move(left.m_string) + std::move(right.m_string)};
    }
    inline String operator+(String&& left, const String& right)
    {
        return {std::move(left.m_string) + right.m_string};
    }
    inline String operator+(const String& left, String&& right)
    {
        return {left.m_string + std::move(right.m_string)};
    }

    // We don't provide operator<< implmentations for basic_ostream<charX_t> streams because
    // even clang 15 can't compile them when using libc++.
    // We could define them for VS, GCC and for clang with libstdc++, but there is no real use for them.
    TGUI_API std::ostream& operator<<(std::ostream& os, const String& str);
    TGUI_API std::wostream& operator<<(std::wostream& os, const String& str);


    // UTF-8 function are defined in the header so that they can be enabled/disabled based on
    // the compiler settings without having to recompile TGUI with a different c++ standard.
#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
    inline String::String(const std::u8string& str) :
        m_string(utf::convertUtf8toUtf32(str.begin(), str.end()))
    {
    }

    inline String::String(char8_t utfChar)
        : m_string(1, static_cast<char32_t>(utfChar))
    {
    }

    inline String::String(const char8_t* str)
        : String{utf::convertUtf8toUtf32(str, str + std::char_traits<char8_t>::length(str))}
    {
    }

    inline String::String(std::size_t count, char8_t ch)
        : m_string(count, static_cast<char32_t>(ch))
    {
    }

    inline String::String(const std::u8string& str, std::size_t pos)
        : String{std::u8string(str, pos)}
    {
    }

    inline String::String(const std::u8string& str, std::size_t pos, std::size_t count)
        : String{std::u8string(str, pos, count)}
    {
    }

    inline String::String(const char8_t* str, std::size_t count)
        : String{std::u8string{str, count}}
    {
    }

    inline String::String(std::initializer_list<char8_t> chars)
        : String(std::u8string(chars.begin(), chars.end()))
    {
    }

    inline String::String(std::u8string::const_iterator first, std::u8string::const_iterator last)
        : String{std::u8string(first, last)}
    {
    }

    inline String::operator std::u8string() const
    {
        return utf::convertUtf32toUtf8(m_string);
    }

    inline std::u8string String::toUtf8() const
    {
        return utf::convertUtf32toUtf8(m_string);
    }

    inline String& String::assign(const std::u8string& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(String{str, pos, count}.m_string);
        return *this;
    }

    inline String& String::assign(const char8_t* str, std::size_t count)
    {
        m_string.assign(String{str, count}.m_string);
        return *this;
    }

    inline String& String::assign(std::size_t count, char8_t ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    inline String& String::assign(std::initializer_list<char8_t> chars)
    {
        m_string.assign(String{chars}.m_string);
        return *this;
    }

    inline String& String::assign(std::u8string::const_iterator first, std::u8string::const_iterator last)
    {
        m_string.assign(String{first, last}.m_string);
        return *this;
    }

    inline String& String::insert(std::size_t index, std::size_t count, char8_t ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    inline String& String::insert(std::size_t index, const std::u8string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }

    inline String& String::insert(std::size_t index, const char8_t* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    inline String::iterator String::insert(String::const_iterator pos, char8_t ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(ch));
    }

    inline String::iterator String::insert(String::const_iterator pos, std::size_t count, char8_t ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(ch));
    }

    inline String::iterator String::insert(String::const_iterator pos, std::initializer_list<char8_t> chars)
    {
        const std::u32string tmpStr(utf::convertUtf8toUtf32(chars.begin(), chars.end()));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    inline String::iterator String::insert(String::const_iterator pos, std::u8string::const_iterator first, std::u8string::const_iterator last)
    {
        const std::u32string tmpStr(utf::convertUtf8toUtf32(first, last));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    inline String& String::append(std::size_t count, char8_t ch)
    {
        m_string.append(count, static_cast<char32_t>(ch));
        return *this;
    }

    inline String& String::append(const std::u8string& str, std::size_t pos, std::size_t count)
    {
        m_string.append(String{str, pos, count}.m_string);
        return *this;
    }

    inline String& String::append(const char8_t* str, std::size_t count)
    {
        m_string.append(String{str, count}.m_string);
        return *this;
    }

    inline String& String::append(std::initializer_list<char8_t> chars)
    {
        m_string.append(String{chars}.m_string);
        return *this;
    }

    inline String& String::append(std::u8string::const_iterator first, std::u8string::const_iterator last)
    {
        m_string.append(String{first, last}.m_string);
        return *this;
    }

    inline int String::compare(std::size_t pos1, std::size_t count1, const std::u8string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2}.m_string);
    }

    inline int String::compare(std::size_t pos1, std::size_t count1, const char8_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2}.m_string);
    }

    inline String& String::replace(std::size_t pos, std::size_t count, const std::u8string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    inline String& String::replace(const_iterator first, const_iterator last, std::u8string::const_iterator first2, std::u8string::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    inline String& String::replace(std::size_t pos, std::size_t count, const char8_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    inline String& String::replace(const_iterator first, const_iterator last, const char8_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    inline String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char8_t ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    inline String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char8_t ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    inline String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char8_t> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    inline void String::resize(std::size_t count, char8_t ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    inline bool String::contains(char8_t c) const noexcept
    {
        return contains(static_cast<char32_t>(c));
    }

    inline std::size_t String::find(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_first_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_first_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_first_not_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_first_not_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::rfind(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    inline std::size_t String::rfind(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_last_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_last_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_last_not_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_last_not_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char8_t>(ch), pos);
    }
#endif

#if TGUI_COMPILED_WITH_CPP_VER >= 17 && defined(__cpp_lib_starts_ends_with) && defined(__cpp_lib_char8_t) && (__cpp_lib_starts_ends_with >= 201711L) && (__cpp_lib_char8_t >= 201811L)
    inline bool String::starts_with(std::u32string_view sv) const noexcept
    {
        return m_string.starts_with(sv);
    }

    inline bool String::starts_with(const char32_t* s) const
    {
        return m_string.starts_with(s);
    }

    inline bool String::starts_with(const std::u32string& s) const
    {
        return m_string.starts_with(s);
    }

    inline bool String::starts_with(const String& s) const
    {
        return m_string.starts_with(s.m_string);
    }

    inline bool String::starts_with(char ch) const noexcept
    {
        return m_string.starts_with(static_cast<char32_t>(ch));
    }

    inline bool String::starts_with(wchar_t ch) const noexcept
    {
        return m_string.starts_with(static_cast<char32_t>(ch));
    }

    inline bool String::starts_with(char8_t ch) const noexcept
    {
        return m_string.starts_with(static_cast<char32_t>(ch));
    }

    inline bool String::starts_with(char16_t ch) const noexcept
    {
        return m_string.starts_with(static_cast<char32_t>(ch));
    }

    inline bool String::starts_with(char32_t ch) const noexcept
    {
        return m_string.starts_with(ch);
    }

    inline bool String::ends_with(std::u32string_view sv) const noexcept
    {
        return m_string.ends_with(sv);
    }

    inline bool String::ends_with(const char32_t* s) const
    {
        return m_string.ends_with(s);
    }

    inline bool String::ends_with(const std::u32string& s) const
    {
        return m_string.ends_with(s);
    }

    inline bool String::ends_with(const String& s) const
    {
        return m_string.ends_with(s.m_string);
    }

    inline bool String::ends_with(char ch) const noexcept
    {
        return m_string.ends_with(static_cast<char32_t>(ch));
    }

    inline bool String::ends_with(wchar_t ch) const noexcept
    {
        return m_string.ends_with(static_cast<char32_t>(ch));
    }

    inline bool String::ends_with(char8_t ch) const noexcept
    {
        return m_string.ends_with(static_cast<char32_t>(ch));
    }

    inline bool String::ends_with(char16_t ch) const noexcept
    {
        return m_string.ends_with(static_cast<char32_t>(ch));
    }

    inline bool String::ends_with(char32_t ch) const noexcept
    {
        return m_string.ends_with(ch);
    }
#else
    inline bool String::starts_with(const String& s) const
    {
        if (s.length() <= length())
            return compare(0, s.length(), s) == 0;
        else
            return false;
    }

    inline bool String::ends_with(const String& s) const
    {
        if (s.length() <= length())
            return compare(length() - s.length(), s.length(), s) == 0;
        else
            return false;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    using StringView = std::u32string_view;
    using CharStringView = std::string_view;

    // Allow using operator ""sv
    // Note that this only affects code placed inside the tgui namespace.
    using namespace std::literals::string_view_literals;
#else
    using StringView = const String&;
    using CharStringView = const std::string&;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_STRING_HPP
