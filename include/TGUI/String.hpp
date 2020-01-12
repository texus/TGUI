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


#ifndef TGUI_STRING_HPP
#define TGUI_STRING_HPP

#include <TGUI/Config.hpp>
#include <string>
#include <cstring>
#include <locale>
#include <ostream>

#include <SFML/System/String.hpp>

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
#ifdef TGUI_NEXT
    TGUI_API bool isWhitespace(uint32_t character);
#else
    TGUI_API bool isWhitespace(sf::Uint32 character);
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Converts a string to an int
    ///
    /// @param str           The string to parse
    /// @param defaultValue  Value to return when the string didn't contain an int
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API int strToInt(const std::string& str, int defaultValue = 0);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Converts a string to an unsigned int
    ///
    /// @param str           The string to parse
    /// @param defaultValue  Value to return when the string didn't contain an int
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API unsigned int strToUInt(const std::string& str, unsigned int defaultValue = 0);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Converts a string to a float
    ///
    /// @param str           The string to parse
    /// @param defaultValue  Value to return when the string didn't contain a float
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API float strToFloat(const std::string& str, float defaultValue = 0);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Converts a string to lowercase
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API std::string toLower(const std::string& str);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Converts a string to uppercase
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API std::string toUpper(const std::string& str);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Trims the whitespace from a string
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_API std::string trim(const std::string& str);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper class to store strings
    ///
    /// A constructor and conversion operator are provided for sf::String to easily convert between sf::String and tgui::String.
    ///
    /// The interface is similar to std::basic_string, but functions accept char, wchar_t, char8_t, char16_t and char32_t.
    /// Some extra helper functions are also provided to e.g. convert the string to an integer or float or to lowercase.
    ///
    /// Note that when converting to std::string, an UTF-8 encoded string will be returned, even by the asAnsiString function.
    /// This is done to be able to use UTF-8 without c++20 support (the asUtf8 function always returns an std::u8string).
    /// Similarly, when passing an std::string or const char* to this class, the encoding is assumed to be UTF-8.
    ///
    /// Data is stored in UTF-32, so any parameter or operator using a different encoding will have to convert the string
    /// internally and may be slightly slower than the variants that use UTF-32.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API String
    {
    private:
        std::u32string m_string;

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
        /// @param defaultValue  Value to return if conversion fails
        ///
        /// @brief Returns the integer value or defaultValue if the string didn't contain a base 10 integer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int toInt(int defaultValue = 0) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the string to a float
        ///
        /// @param defaultValue  Value to return if conversion fails
        ///
        /// @brief Returns the float value or defaultValue if the string didn't contain an float
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float toFloat(float defaultValue = 0) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a string with the whitespace at the start and end of this string removed
        ///
        /// @return Trimmed string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String trim() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the ASCII characters in the string to lowercase
        ///
        /// @return Lowercase string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String toLower() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts the ASCII characters in the string to uppercase
        ///
        /// @return Uppercase string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String toUpper() const;

#ifdef TGUI_NEXT // Code not working on GCC 4.9
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Replaces all occurrences of a substring with a replacement string
        ///
        /// @param searchFor   The value being searched for
        /// @param replaceWith The value that replaces found searchFor values
        ///
        /// @return Reference to this object.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String& replace(const String& searchFor, const String& replaceWith);
#endif

    public:

        String() = default;

        String(const std::string& str);
        String(const std::wstring& str);
        String(const std::u16string& str);
        String(const std::u32string& str);

        String(std::u32string&& str)
            : m_string{std::move(str)}
        {
        }

        String(char ansiChar, const std::locale& locale = std::locale());
        String(wchar_t wideChar);
        String(char16_t utfChar);
        String(char32_t utfChar);

        String(const char* str);
        String(const wchar_t* str);
        String(const char16_t* str);
        String(const char32_t* str);

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

        String(std::initializer_list<char> chars);
        String(std::initializer_list<wchar_t> chars);
        String(std::initializer_list<char16_t> chars);
        String(std::initializer_list<char32_t> chars);

        String(std::string::const_iterator first, std::string::const_iterator last);
        String(std::wstring::const_iterator first, std::wstring::const_iterator last);
        String(std::u16string::const_iterator first, std::u16string::const_iterator last);
        String(std::u32string::const_iterator first, std::u32string::const_iterator last);

        explicit String(const sf::String& str)
            : m_string{reinterpret_cast<const char32_t*>(str.toUtf32().c_str())}
        {
        }

        explicit operator sf::String() const
        {
            return sf::String::fromUtf32(m_string.begin(), m_string.end());
        }

        operator std::string() const;
        operator std::wstring() const;
        operator std::u16string() const;
        operator const std::u32string&() const
        {
            return m_string;
        }

        std::string asAnsiString() const;
        std::wstring asWideString() const;
        std::u16string asUtf16() const;
        const std::u32string& asUtf32() const;

        String& assign(std::size_t count, char ch);
        String& assign(std::size_t count, wchar_t ch);
        String& assign(std::size_t count, char16_t ch);
        String& assign(std::size_t count, char32_t ch);

        String& assign(const std::string& str);
        String& assign(const std::wstring& str);
        String& assign(const std::u16string& str);
        String& assign(const std::u32string& str);
        String& assign(const String& str);

        String& assign(const std::string& str, std::size_t pos, std::size_t count = npos);
        String& assign(const std::wstring& str, std::size_t pos, std::size_t count = npos);
        String& assign(const std::u16string& str, std::size_t pos, std::size_t count = npos);
        String& assign(const std::u32string& str, std::size_t pos, std::size_t count = npos);
        String& assign(const String& str, std::size_t pos, std::size_t count = npos);

        String& assign(std::string&& str);
        String& assign(std::wstring&& str);
        String& assign(std::u16string&& str);
        String& assign(std::u32string&& str);
        String& assign(String&& str);

        String& assign(const char* str, std::size_t count);
        String& assign(const wchar_t* str, std::size_t count);
        String& assign(const char16_t* str, std::size_t count);
        String& assign(const char32_t* str, std::size_t count);

        String& assign(const char* str);
        String& assign(const wchar_t* str);
        String& assign(const char16_t* str);
        String& assign(const char32_t* str);

        String& assign(std::initializer_list<char> chars);
        String& assign(std::initializer_list<wchar_t> chars);
        String& assign(std::initializer_list<char16_t> chars);
        String& assign(std::initializer_list<char32_t> chars);

        String& assign(std::string::const_iterator first, std::string::const_iterator last);
        String& assign(std::wstring::const_iterator first, std::wstring::const_iterator last);
        String& assign(std::u16string::const_iterator first, std::u16string::const_iterator last);
        String& assign(std::u32string::const_iterator first, std::u32string::const_iterator last);

        reference       at(std::size_t pos);
        const_reference at(std::size_t pos) const;

        const_reference operator [](std::size_t index) const;
        reference operator [](std::size_t index);

        reference       front();
        const_reference front() const;

        reference       back();
        const_reference back() const;

        const char32_t* data() const noexcept;
#if __cplusplus >= 201703L
        char32_t* data() noexcept
        {
            return m_string.data();
        }
#endif
        const char32_t* c_str() const noexcept;

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

        bool empty() const noexcept;
        std::size_t size() const noexcept;
        std::size_t length() const noexcept;
        std::size_t max_size() const noexcept;

        void reserve(std::size_t newCap);
        std::size_t capacity() const noexcept;
        void shrink_to_fit();

        void clear() noexcept;
#ifdef TGUI_NEXT // Code not working on GCC 4.9
        String& insert(std::size_t index, std::size_t count, char ch);
        String& insert(std::size_t index, std::size_t count, wchar_t ch);
        String& insert(std::size_t index, std::size_t count, char16_t ch);
        String& insert(std::size_t index, std::size_t count, char32_t ch);

        String& insert(std::size_t index, const std::string& str);
        String& insert(std::size_t index, const std::wstring& str);
        String& insert(std::size_t index, const std::u16string& str);
        String& insert(std::size_t index, const std::u32string& str);
        String& insert(std::size_t index, const String& str);

        String& insert(std::size_t index, const std::string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const std::wstring& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const std::u16string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const std::u32string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const String& str, std::size_t pos, std::size_t count = npos);

        String& insert(std::size_t index, const char* str, std::size_t count);
        String& insert(std::size_t index, const wchar_t* str, std::size_t count);
        String& insert(std::size_t index, const char16_t* str, std::size_t count);
        String& insert(std::size_t index, const char32_t* str, std::size_t count);

        String& insert(std::size_t index, const char* str);
        String& insert(std::size_t index, const wchar_t* str);
        String& insert(std::size_t index, const char16_t* str);
        String& insert(std::size_t index, const char32_t* str);

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

        String& erase(std::size_t index = 0, std::size_t count = npos);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);
#endif
        void push_back(char ch);
        void push_back(wchar_t ch);
        void push_back(char16_t ch);
        void push_back(char32_t ch);

        void pop_back();

        String& append(std::size_t count, char ch);
        String& append(std::size_t count, wchar_t ch);
        String& append(std::size_t count, char16_t ch);
        String& append(std::size_t count, char32_t ch);

        String& append(const std::string& str);
        String& append(const std::wstring& str);
        String& append(const std::u16string& str);
        String& append(const std::u32string& str);
        String& append(const String& str);

        String& append(const std::string& str, std::size_t pos, std::size_t count = npos);
        String& append(const std::wstring& str, std::size_t pos, std::size_t count = npos);
        String& append(const std::u16string& str, std::size_t pos, std::size_t count = npos);
        String& append(const std::u32string& str, std::size_t pos, std::size_t count = npos);
        String& append(const String& str, std::size_t pos, std::size_t count = npos);

        String& append(const char* str, std::size_t count);
        String& append(const wchar_t* str, std::size_t count);
        String& append(const char16_t* str, std::size_t count);
        String& append(const char32_t* str, std::size_t count);

        String& append(const char* str);
        String& append(const wchar_t* str);
        String& append(const char16_t* str);
        String& append(const char32_t* str);

        String& append(std::string::const_iterator first, std::string::const_iterator last);
        String& append(std::wstring::const_iterator first, std::wstring::const_iterator last);
        String& append(std::u16string::const_iterator first, std::u16string::const_iterator last);
        String& append(std::u32string::const_iterator first, std::u32string::const_iterator last);

        String& append(std::initializer_list<char> chars);
        String& append(std::initializer_list<wchar_t> chars);
        String& append(std::initializer_list<char16_t> chars);
        String& append(std::initializer_list<char32_t> chars);

        String& operator+=(const String& str);

        int compare(const std::string& str) const noexcept;
        int compare(const std::wstring& str) const noexcept;
        int compare(const std::u16string& str) const noexcept;
        int compare(const std::u32string& str) const noexcept;
        int compare(const String& str) const noexcept;

        int compare(std::size_t pos1, std::size_t count1, const std::string& str) const;
        int compare(std::size_t pos1, std::size_t count1, const std::wstring& str) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u16string& str) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u32string& str) const;
        int compare(std::size_t pos1, std::size_t count1, const String& str) const;

        int compare(std::size_t pos1, std::size_t count1, const std::string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const std::wstring& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u16string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u32string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(std::size_t pos1, std::size_t count1, const String& str, std::size_t pos2, std::size_t count2 = npos) const;

        int compare(const char* s) const;
        int compare(const wchar_t* s) const;
        int compare(const char16_t* s) const;
        int compare(const char32_t* s) const;

        int compare(std::size_t pos1, std::size_t count1, const char* s) const;
        int compare(std::size_t pos1, std::size_t count1, const wchar_t* s) const;
        int compare(std::size_t pos1, std::size_t count1, const char16_t* s) const;
        int compare(std::size_t pos1, std::size_t count1, const char32_t* s) const;

        int compare(std::size_t pos1, std::size_t count1, const char* s, std::size_t count2) const;
        int compare(std::size_t pos1, std::size_t count1, const wchar_t* s, std::size_t count2) const;
        int compare(std::size_t pos1, std::size_t count1, const char16_t* s, std::size_t count2) const;
        int compare(std::size_t pos1, std::size_t count1, const char32_t* s, std::size_t count2) const;
#ifdef TGUI_NEXT // Code not working on GCC 4.9
        String& replace(std::size_t pos, std::size_t count, const std::string& str);
        String& replace(std::size_t pos, std::size_t count, const std::wstring& str);
        String& replace(std::size_t pos, std::size_t count, const std::u16string& str);
        String& replace(std::size_t pos, std::size_t count, const std::u32string& str);
        String& replace(std::size_t pos, std::size_t count, const String& str);

        String& replace(const_iterator first, const_iterator last, const std::string& str);
        String& replace(const_iterator first, const_iterator last, const std::wstring& str);
        String& replace(const_iterator first, const_iterator last, const std::u16string& str);
        String& replace(const_iterator first, const_iterator last, const std::u32string& str);
        String& replace(const_iterator first, const_iterator last, const String& str);

        String& replace(std::size_t pos, std::size_t count, const std::string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const std::wstring& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const std::u16string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const std::u32string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(std::size_t pos, std::size_t count, const String& str, std::size_t pos2, std::size_t count2 = npos);

        String& replace(const_iterator first, const_iterator last, std::string::const_iterator first2, std::string::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::wstring::const_iterator first2, std::wstring::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::u16string::const_iterator first2, std::u16string::const_iterator last2);
        String& replace(const_iterator first, const_iterator last, std::u32string::const_iterator first2, std::u32string::const_iterator last2);

        String& replace(std::size_t pos, std::size_t count, const char* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const wchar_t* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const char16_t* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const char32_t* cstr, std::size_t count2);

        String& replace(const_iterator first, const_iterator last, const char* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const wchar_t* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const char16_t* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const char32_t* cstr, std::size_t count2);

        String& replace(std::size_t pos, std::size_t count, const char* cstr);
        String& replace(std::size_t pos, std::size_t count, const wchar_t* cstr);
        String& replace(std::size_t pos, std::size_t count, const char16_t* cstr);
        String& replace(std::size_t pos, std::size_t count, const char32_t* cstr);

        String& replace(const_iterator first, const_iterator last, const char* cstr);
        String& replace(const_iterator first, const_iterator last, const wchar_t* cstr);
        String& replace(const_iterator first, const_iterator last, const char16_t* cstr);
        String& replace(const_iterator first, const_iterator last, const char32_t* cstr);

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
#endif
        String substr(std::size_t pos = 0, std::size_t count = npos) const;

        std::size_t copy(char32_t* dest, std::size_t count, std::size_t pos = 0) const;

        void resize(std::size_t count);
        void resize(std::size_t count, char ch);
        void resize(std::size_t count, wchar_t ch);
        void resize(std::size_t count, char16_t ch);
        void resize(std::size_t count, char32_t ch);

        void swap(String& other);

        std::size_t find(const std::string& str, std::size_t pos = 0) const noexcept;
        std::size_t find(const std::wstring& str, std::size_t pos = 0) const noexcept;
        std::size_t find(const std::u16string& str, std::size_t pos = 0) const noexcept;
        std::size_t find(const std::u32string& str, std::size_t pos = 0) const noexcept;
        std::size_t find(const String& str, std::size_t pos = 0) const noexcept;

        std::size_t find(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find(const char* s, std::size_t pos = 0) const;
        std::size_t find(const wchar_t* s, std::size_t pos = 0) const;
        std::size_t find(const char16_t* s, std::size_t pos = 0) const;
        std::size_t find(const char32_t* s, std::size_t pos = 0) const;

        std::size_t find(char ch, std::size_t pos = 0) const noexcept;
        std::size_t find(wchar_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find(char16_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find(char32_t ch, std::size_t pos = 0) const noexcept;

        std::size_t find_first_of(const std::string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(const std::wstring& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(const std::u16string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(const std::u32string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(const String& str, std::size_t pos = 0) const noexcept;

        std::size_t find_first_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_first_of(const char* s, std::size_t pos = 0) const;
        std::size_t find_first_of(const wchar_t* s, std::size_t pos = 0) const;
        std::size_t find_first_of(const char16_t* s, std::size_t pos = 0) const;
        std::size_t find_first_of(const char32_t* s, std::size_t pos = 0) const;

        std::size_t find_first_of(char ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(wchar_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(char16_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(char32_t ch, std::size_t pos = 0) const noexcept;

        std::size_t find_first_not_of(const std::string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(const std::wstring& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(const std::u16string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(const std::u32string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(const String& str, std::size_t pos = 0) const noexcept;

        std::size_t find_first_not_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_first_not_of(const char* s, std::size_t pos = 0) const;
        std::size_t find_first_not_of(const wchar_t* s, std::size_t pos = 0) const;
        std::size_t find_first_not_of(const char16_t* s, std::size_t pos = 0) const;
        std::size_t find_first_not_of(const char32_t* s, std::size_t pos = 0) const;

        std::size_t find_first_not_of(char ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(wchar_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(char16_t ch, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(char32_t ch, std::size_t pos = 0) const noexcept;

        std::size_t rfind(const std::string& str, std::size_t pos = npos) const noexcept;
        std::size_t rfind(const std::wstring& str, std::size_t pos = npos) const noexcept;
        std::size_t rfind(const std::u16string& str, std::size_t pos = npos) const noexcept;
        std::size_t rfind(const std::u32string& str, std::size_t pos = npos) const noexcept;
        std::size_t rfind(const String& str, std::size_t pos = npos) const noexcept;

        std::size_t rfind(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t rfind(const char* s, std::size_t pos = npos) const;
        std::size_t rfind(const wchar_t* s, std::size_t pos = npos) const;
        std::size_t rfind(const char16_t* s, std::size_t pos = npos) const;
        std::size_t rfind(const char32_t* s, std::size_t pos = npos) const;

        std::size_t rfind(char ch, std::size_t pos = npos) const noexcept;
        std::size_t rfind(wchar_t ch, std::size_t pos = npos) const noexcept;
        std::size_t rfind(char16_t ch, std::size_t pos = npos) const noexcept;
        std::size_t rfind(char32_t ch, std::size_t pos = npos) const noexcept;

        std::size_t find_last_of(const std::string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(const std::wstring& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(const std::u16string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(const std::u32string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(const String& str, std::size_t pos = npos) const noexcept;

        std::size_t find_last_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_last_of(const char* s, std::size_t pos = npos) const;
        std::size_t find_last_of(const wchar_t* s, std::size_t pos = npos) const;
        std::size_t find_last_of(const char16_t* s, std::size_t pos = npos) const;
        std::size_t find_last_of(const char32_t* s, std::size_t pos = npos) const;

        std::size_t find_last_of(char ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(wchar_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(char16_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(char32_t ch, std::size_t pos = npos) const noexcept;

        std::size_t find_last_not_of(const std::string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(const std::wstring& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(const std::u16string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(const std::u32string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(const String& str, std::size_t pos = npos) const noexcept;

        std::size_t find_last_not_of(const char* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const char16_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const char32_t* s, std::size_t pos, std::size_t count) const;

        std::size_t find_last_not_of(const char* s, std::size_t pos = npos) const;
        std::size_t find_last_not_of(const wchar_t* s, std::size_t pos = npos) const;
        std::size_t find_last_not_of(const char16_t* s, std::size_t pos = npos) const;
        std::size_t find_last_not_of(const char32_t* s, std::size_t pos = npos) const;

        std::size_t find_last_not_of(char ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(wchar_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(char16_t ch, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(char32_t ch, std::size_t pos = npos) const noexcept;

        friend bool operator==(const String& left, const String& right);
        friend bool operator!=(const String& left, const String& right);
        friend bool operator<(const String& left, const String& right);
        friend bool operator<=(const String& left, const String& right);
        friend bool operator>(const String& left, const String& right);
        friend bool operator>=(const String& left, const String& right);
        friend String operator+(const String& left, const String& right);
        friend String operator+(const String& left, String&& right);
        friend String operator+(String&& left, const String& right);
        friend String operator+(String&& left, String&& right);

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        String(const std::u8string& str);
        String(char8_t utfChar);
        String(const char8_t* str);
        String(std::size_t count, char8_t ch);
        String(const std::u8string& str, std::size_t pos);
        String(const std::u8string& str, std::size_t pos, std::size_t count);
        String(const char8_t* str, std::size_t count);
        String(std::initializer_list<char8_t> chars);
        String(std::u8string::const_iterator first, std::u8string::const_iterator last);

        operator std::u8string() const;

        std::u8string asUtf8() const;

        String& assign(std::size_t count, char8_t ch);
        String& assign(const std::u8string& str);
        String& assign(const std::u8string& str, std::size_t pos, std::size_t count = npos);
        String& assign(std::u8string&& str);
        String& assign(const char8_t* str, std::size_t count);
        String& assign(const char8_t* str);
        String& assign(std::initializer_list<char8_t> chars);
        String& assign(std::u8string::const_iterator first, std::u8string::const_iterator last);

        String& insert(std::size_t index, std::size_t count, char8_t ch);
        String& insert(std::size_t index, const std::u8string& str);
        String& insert(std::size_t index, const std::u8string& str, std::size_t pos, std::size_t count = npos);
        String& insert(std::size_t index, const char8_t* str, std::size_t count);
        String& insert(std::size_t index, const char8_t* str);
        iterator insert(const_iterator pos, char8_t ch);
        iterator insert(const_iterator pos, std::size_t count, char8_t ch);
        iterator insert(const_iterator pos, std::initializer_list<char8_t> chars);
        iterator insert(const_iterator pos, std::u8string::const_iterator first, std::u8string::const_iterator last);

        String& append(std::size_t count, char8_t ch);
        String& append(const std::u8string& str);
        String& append(const std::u8string& str, std::size_t pos, std::size_t count = npos);
        String& append(const char8_t* str, std::size_t count);
        String& append(const char8_t* str);
        String& append(std::initializer_list<char8_t> chars);
        String& append(std::u8string::const_iterator first, std::u8string::const_iterator last);

        int compare(const std::u8string& str) const noexcept;
        int compare(std::size_t pos1, std::size_t count1, const std::u8string& str) const;
        int compare(std::size_t pos1, std::size_t count1, const std::u8string& str, std::size_t pos2, std::size_t count2 = npos) const;
        int compare(const char8_t* s) const;
        int compare(std::size_t pos1, std::size_t count1, const char8_t* s) const;
        int compare(std::size_t pos1, std::size_t count1, const char8_t* s, std::size_t count2) const;

        String& replace(std::size_t pos, std::size_t count, const std::u8string& str);
        String& replace(const_iterator first, const_iterator last, const std::u8string& str);
        String& replace(std::size_t pos, std::size_t count, const std::u8string& str, std::size_t pos2, std::size_t count2 = npos);
        String& replace(const_iterator first, const_iterator last, std::u8string::const_iterator first2, std::u8string::const_iterator last2);
        String& replace(std::size_t pos, std::size_t count, const char8_t* cstr, std::size_t count2);
        String& replace(const_iterator first, const_iterator last, const char8_t* cstr, std::size_t count2);
        String& replace(std::size_t pos, std::size_t count, const char8_t* cstr);
        String& replace(const_iterator first, const_iterator last, const char8_t* cstr);
        String& replace(std::size_t pos, std::size_t count, std::size_t count2, char8_t ch);
        String& replace(const_iterator first, const_iterator last, std::size_t count2, char8_t ch);
        String& replace(const_iterator first, const_iterator last, std::initializer_list<char8_t> chars);

        void resize(std::size_t count, char8_t ch);

        std::size_t find(const std::u8string& str, std::size_t pos = 0) const noexcept;
        std::size_t find(const char8_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find(const char8_t* s, std::size_t pos = 0) const;
        std::size_t find(char8_t ch, std::size_t pos = 0) const noexcept;

        std::size_t find_first_of(const std::u8string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_of(const char8_t* s, std::size_t pos = 0) const;
        std::size_t find_first_of(char8_t ch, std::size_t pos = 0) const noexcept;

        std::size_t find_first_not_of(const std::u8string& str, std::size_t pos = 0) const noexcept;
        std::size_t find_first_not_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_first_not_of(const char8_t* s, std::size_t pos = 0) const;
        std::size_t find_first_not_of(char8_t ch, std::size_t pos = 0) const noexcept;

        std::size_t rfind(const std::u8string& str, std::size_t pos = npos) const noexcept;
        std::size_t rfind(const char8_t* s, std::size_t pos, std::size_t count) const;
        std::size_t rfind(const char8_t* s, std::size_t pos = npos) const;
        std::size_t rfind(char8_t ch, std::size_t pos = npos) const noexcept;

        std::size_t find_last_of(const std::u8string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_of(const char8_t* s, std::size_t pos = npos) const;
        std::size_t find_last_of(char8_t ch, std::size_t pos = npos) const noexcept;

        std::size_t find_last_not_of(const std::u8string& str, std::size_t pos = npos) const noexcept;
        std::size_t find_last_not_of(const char8_t* s, std::size_t pos, std::size_t count) const;
        std::size_t find_last_not_of(const char8_t* s, std::size_t pos = npos) const;
        std::size_t find_last_not_of(char8_t ch, std::size_t pos = npos) const noexcept;

        friend std::basic_ostream<char8_t>& operator<<(std::basic_ostream<char8_t>& os, const String& str);
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
        return String(left.m_string + right.m_string);
    }
    inline String operator+(String&& left, String&& right)
    {
        return String(std::move(left.m_string) + std::move(right.m_string));
    }
    inline String operator+(String&& left, const String& right)
    {
        return String(std::move(left.m_string) + right.m_string);
    }
    inline String operator+(const String& left, String&& right)
    {
        return String(left.m_string + std::move(right.m_string));
    }

    TGUI_API std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const String& str);
    TGUI_API std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& os, const String& str);
    //TGUI_API std::basic_ostream<char16_t>& operator<<(std::basic_ostream<char16_t>& os, const String& str);
    //TGUI_API std::basic_ostream<char32_t>& operator<<(std::basic_ostream<char32_t>& os, const String& str);


    // UTF-8 function are defined in the header so that they can be enabled/disabled based on
    // the compiler settings without having to recompile TGUI with a different c++ standard.
#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
    inline String::String(const std::u8string& str)
    {
        m_string.reserve(str.length()+1);
        sf::Utf8::toUtf32(str.begin(), str.end(), std::back_inserter(m_string));
    }

    inline String::String(char8_t utfChar)
        : m_string(1, static_cast<char32_t>(utfChar))
    {
    }

    inline String::String(const char8_t* str)
        : String{std::u8string{str}}
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
        return asUtf8();
    }

    inline std::u8string String::asUtf8() const
    {
        std::u8string output;
        output.reserve(m_string.length() + 1);
        sf::Utf32::toUtf8(m_string.begin(), m_string.end(), std::back_inserter(output));
        return output;
    }

    inline String& String::assign(std::size_t count, char8_t ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    inline String& String::assign(const std::u8string& str)
    {
        return *this = str;
    }

    inline String& String::assign(const std::u8string& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
    }

    inline String& String::assign(std::u8string&& str)
    {
        return *this = std::move(str);
    }

    inline String& String::assign(const char8_t* str, std::size_t count)
    {
        return *this = {str, count};
    }

    inline String& String::assign(const char8_t* str)
    {
        return *this = str;
    }

    inline String& String::assign(std::initializer_list<char8_t> chars)
    {
        return *this = chars;
    }

    inline String& String::assign(std::u8string::const_iterator first, std::u8string::const_iterator last)
    {
        return *this = {first, last};
    }

    inline String& String::insert(std::size_t index, std::size_t count, char8_t ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    inline String& String::insert(std::size_t index, const std::u8string& str)
    {
        m_string.insert(index, String{str}.m_string);
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

    inline String& String::insert(std::size_t index, const char8_t* str)
    {
        m_string.insert(index, String{str}.m_string);
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
        auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    inline String::iterator String::insert(String::const_iterator pos, std::u8string::const_iterator first, std::u8string::const_iterator last)
    {
        auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    inline String& String::append(std::size_t count, char8_t ch)
    {
        return append(String(count, ch));
    }

    inline String& String::append(const std::u8string& str)
    {
        return append(String{str});
    }

    inline String& String::append(const std::u8string& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
    }

    inline String& String::append(const char8_t* str, std::size_t count)
    {
        return append(String{str, count});
    }

    inline String& String::append(const char8_t* str)
    {
        return append(String{str});
    }

    inline String& String::append(std::initializer_list<char8_t> chars)
    {
        return append(String{chars});
    }

    inline String& String::append(std::u8string::const_iterator first, std::u8string::const_iterator last)
    {
        return append(String{first, last});
    }

    inline int String::compare(const std::u8string& str) const noexcept
    {
        return m_string.compare(String{str});
    }

    inline int String::compare(std::size_t pos1, std::size_t count1, const std::u8string& str) const
    {
        return m_string.compare(pos1, count1, String{str});
    }

    inline int String::compare(std::size_t pos1, std::size_t count1, const std::u8string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2});
    }

    inline int String::compare(const char8_t* s) const
    {
        return m_string.compare(String{s});
    }

    inline int String::compare(std::size_t pos1, std::size_t count1, const char8_t* s) const
    {
        return m_string.compare(pos1, count1, String{s});
    }

    inline int String::compare(std::size_t pos1, std::size_t count1, const char8_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2});
    }

    inline String& String::replace(std::size_t pos, std::size_t count, const std::u8string& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
        return *this;
    }

    inline String& String::replace(const_iterator first, const_iterator last, const std::u8string& str)
    {
        m_string.replace(first, last, String{str}.m_string);
        return *this;
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

    inline String& String::replace(std::size_t pos, std::size_t count, const char8_t* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    inline String& String::replace(const_iterator first, const_iterator last, const char8_t* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
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

    inline std::size_t String::find(const std::u8string& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
    }

    inline std::size_t String::find(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find(const char8_t* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    inline std::size_t String::find(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_first_of(const std::u8string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
    }

    inline std::size_t String::find_first_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_first_of(const char8_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    inline std::size_t String::find_first_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_first_not_of(const std::u8string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
    }

    inline std::size_t String::find_first_not_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_first_not_of(const char8_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    inline std::size_t String::find_first_not_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::rfind(const std::u8string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
    }

    inline std::size_t String::rfind(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    inline std::size_t String::rfind(const char8_t* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    inline std::size_t String::rfind(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_last_of(const std::u8string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
    }

    inline std::size_t String::find_last_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_last_of(const char8_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    inline std::size_t String::find_last_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    inline std::size_t String::find_last_not_of(const std::u8string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
    }

    inline std::size_t String::find_last_not_of(const char8_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    inline std::size_t String::find_last_not_of(const char8_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    inline std::size_t String::find_last_not_of(char8_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char8_t>(ch), pos);
    }

    inline std::basic_ostream<char8_t>& operator<<(std::basic_ostream<char8_t>& os, const String& str)
    {
        os << std::u8string(str);
        return os;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_STRING_HPP
