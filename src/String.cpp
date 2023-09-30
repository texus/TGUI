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


#include <TGUI/String.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <algorithm>
    #include <cctype> // tolower, toupper, isspace
    #include <cwctype> // iswspace, iswalpha
    #include <iterator> // distance

    #if TGUI_COMPILED_WITH_CPP_VER >= 17
        #include <charconv> // from_chars
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    const decltype(std::u32string::npos) String::npos = std::u32string::npos;
#endif

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD static inline bool compareCharIgnoreCase(char32_t char1, char32_t char2) {
        if (char1 == char2)
            return true;
        else if ((char1 < 128) && (char2 < 128))
            return std::tolower(static_cast<unsigned char>(char1)) == std::tolower(static_cast<unsigned char>(char2));
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char character)
    {
        return std::isspace(static_cast<unsigned char>(character));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char32_t character)
    {
        // wchar_t is only 2 bytes on Windows
        TGUI_IF_CONSTEXPR (sizeof(wchar_t) < 4)
        {
            if (character > 0xFFFF)
                return false;
        }

        return std::iswspace(static_cast<wint_t>(character));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isAlpha(char32_t character)
    {
        // wchar_t is only 2 bytes on Windows
        TGUI_IF_CONSTEXPR (sizeof(wchar_t) < 4)
        {
            if (character > 0xFFFF)
                return false;
        }

        return std::iswalpha(static_cast<wint_t>(character));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isDigit(char32_t character)
    {
        return ((character >= U'0') && (character <= U'9'));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::attemptToInt(int& result) const
    {
        const std::string& ansiStr = trim().toStdString();

#if (TGUI_COMPILED_WITH_CPP_VER >= 17) && defined(__cpp_lib_to_chars) && (__cpp_lib_to_chars >= 201611L)
        return std::from_chars(ansiStr.data(), ansiStr.data() + ansiStr.length(), result).ec == std::errc{};
#else
        try
        {
            result = std::stoi(ansiStr);
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::attemptToUInt(unsigned int& result) const
    {
        const std::string ansiStr = trim().toStdString();

#if (TGUI_COMPILED_WITH_CPP_VER >= 17) && defined(__cpp_lib_to_chars) && (__cpp_lib_to_chars >= 201611L)
        return std::from_chars(ansiStr.data(), ansiStr.data() + ansiStr.length(), result).ec == std::errc{};
#else
        try
        {
            result = static_cast<unsigned int>(std::stoi(ansiStr));
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::attemptToFloat(float& result) const
    {
        const std::string ansiStr = trim().toStdString();

#if (TGUI_COMPILED_WITH_CPP_VER >= 17) && defined(__cpp_lib_to_chars) && (__cpp_lib_to_chars >= 201611L)
        return std::from_chars(ansiStr.data(), ansiStr.data() + ansiStr.length(), result).ec == std::errc{};
#else
        // We can't use std::stof because it always depends on the global locale
        std::istringstream iss(ansiStr);
        iss.imbue(std::locale::classic());

        float value = 0;
        iss >> value;

        if (iss.fail())
            return false;

        result = value;
        return true;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int String::toInt(int defaultValue) const
    {
        int result;
        if (attemptToInt(result))
            return result;
        else
            return defaultValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int String::toUInt(unsigned int defaultValue) const
    {
        unsigned int result;
        if (attemptToUInt(result))
            return result;
        else
            return defaultValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float String::toFloat(float defaultValue) const
    {
        float result;
        if (attemptToFloat(result))
            return result;
        else
            return defaultValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::trim() const
    {
        const auto firstIt = std::find_if(m_string.begin(), m_string.end(), [](char32_t c) { return (c >= 256) || !std::isspace(static_cast<unsigned char>(c)); });
        if (firstIt == m_string.end())
            return {};

        const auto lastIt = std::find_if(m_string.rbegin(), m_string.rend(), [](char32_t c) { return (c >= 256) || !std::isspace(static_cast<unsigned char>(c)); }).base();
        return String(firstIt, lastIt);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::toLower() const
    {
        String str;
        str.reserve(m_string.length() + 1);
        for (const char32_t& c : m_string)
        {
            if (c < 256)
                str.push_back(static_cast<char32_t>(std::tolower(static_cast<unsigned char>(c))));
            else
                str.push_back(c);
        }

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::toUpper() const
    {
        String str;
        str.reserve(m_string.length() + 1);
        for (const char32_t& c : m_string)
        {
            if (c < 256)
                str.push_back(static_cast<char32_t>(std::toupper(static_cast<unsigned char>(c))));
            else
                str.push_back(c);
        }

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::equalIgnoreCase(const String& other) const
    {
        return viewEqualIgnoreCase(*this, other);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    bool String::startsWith(const String& substring) const
    {
        return starts_with(substring);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::startsWithIgnoreCase(const String& substring) const
    {
        if (substring.length() > length())
            return false;

        return std::equal(m_string.begin(), m_string.begin() + static_cast<std::ptrdiff_t>(substring.length()), substring.begin(), substring.end(), &compareCharIgnoreCase);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    bool String::endsWith(const String& substring) const
    {
        return ends_with(substring);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::endsWithIgnoreCase(const String& substring) const
    {
        if (substring.length() > length())
            return false;

        return std::equal(m_string.begin() + static_cast<std::ptrdiff_t>(length() - substring.length()), m_string.end(),
                          substring.begin(), substring.end(), &compareCharIgnoreCase);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::replace(const String& searchFor, const String& replaceWith)
    {
        const std::size_t len = searchFor.length();
        std::size_t step = replaceWith.length();

        // When searching for an empty string, insert the replacement between each character and at the front and back
        if (len == 0)
            step += 1;

        std::size_t pos = find(searchFor);
        while (pos != npos)
        {
            replace(pos, len, replaceWith);
            pos = find(searchFor, pos + step);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::remove(const String& substring)
    {
        const std::size_t len = substring.length();
        if (len == 0)
            return;

        std::size_t pos = find(substring);

        while (pos != npos)
        {
            erase(pos, len);
            pos = find(substring, pos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> String::split(const String& delimiter, bool trim) const
    {
        std::vector<String> substrings;

        const std::size_t delimiterLength = delimiter.length();
        if (delimiterLength > 0)
        {
            std::size_t startPos = 0;
            std::size_t endPos = find(delimiter);
            while (endPos != npos)
            {
                substrings.push_back(substr(startPos, endPos - startPos));
                startPos = endPos + delimiterLength;
                endPos = find(delimiter, startPos);
            }

            if (startPos == 0)
                substrings.push_back(*this);
            else
                substrings.push_back(substr(startPos, length() - startPos));
        }
        else // When the delimeter is empty, each character is put in its own part
        {
            for (char32_t c : m_string)
                substrings.push_back(c);
        }

        if (trim)
        {
            for (auto& part : substrings)
                part = part.trim();
        }

        return substrings;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::join(const std::vector<String>& segments, const String& separator)
    {
        String result;

        auto it = segments.begin();
        if (it != segments.end())
        {
            result += *it;
            ++it;
        }

        for (; it != segments.end(); ++it)
        {
            result += separator;
            result += *it;
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::String(const std::string& str) :
        m_string(utf::convertUtf8toUtf32(str.begin(), str.end()))
    {
    }

    String::String(const std::wstring& str) :
        m_string(utf::convertWidetoUtf32(str.begin(), str.end()))
    {
    }

    String::String(const std::u16string& str) :
        m_string(utf::convertUtf16toUtf32(str.begin(), str.end()))
    {
    }

    String::String(const std::u32string& str) :
        m_string{str}
    {
    }

    String::String(char ansiChar) :
        m_string(1, static_cast<char32_t>(ansiChar))
    {
    }

    String::String(wchar_t wideChar) :
        m_string(1, static_cast<char32_t>(wideChar))
    {
    }

    String::String(char16_t utfChar) :
        m_string(1, static_cast<char32_t>(utfChar))
    {
    }

    String::String(char32_t utfChar) :
        m_string(1, utfChar)
    {
    }

    String::String(const char* str) :
        m_string(utf::convertUtf8toUtf32(str, str + std::char_traits<char>::length(str)))
    {
    }

    String::String(const wchar_t* str) :
        m_string(utf::convertWidetoUtf32(str, str + std::char_traits<wchar_t>::length(str)))
    {
    }

    String::String(const char16_t* str) :
        m_string(utf::convertUtf16toUtf32(str, str + std::char_traits<char16_t>::length(str)))
    {
    }

    String::String(std::size_t count, char ch) :
        m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, wchar_t ch) :
        m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, char16_t ch) :
        m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, char32_t ch) :
        m_string(count, ch)
    {
    }

    String::String(const std::string& str, std::size_t pos) :
        m_string(utf::convertUtf8toUtf32(str.begin() + static_cast<std::ptrdiff_t>(pos), str.end()))
    {
    }

    String::String(const std::wstring& str, std::size_t pos) :
        m_string(utf::convertWidetoUtf32(str.begin() + static_cast<std::ptrdiff_t>(pos), str.end()))
    {
    }

    String::String(const std::u16string& str, std::size_t pos) :
        m_string(utf::convertUtf16toUtf32(str.begin() + static_cast<std::ptrdiff_t>(pos), str.end()))
    {
    }

    String::String(const std::u32string& str, std::size_t pos) :
        m_string(str, pos)
    {
    }

    String::String(const std::string& str, std::size_t pos, std::size_t count) :
        m_string(utf::convertUtf8toUtf32(str.begin() + static_cast<std::ptrdiff_t>(pos),
                                         ((count != npos) && (pos + count < str.length())) ? (str.begin() + static_cast<std::ptrdiff_t>(pos + count)) : str.end()))
    {
    }

    String::String(const std::wstring& str, std::size_t pos, std::size_t count) :
        m_string(utf::convertWidetoUtf32(str.begin() + static_cast<std::ptrdiff_t>(pos),
                                         ((count != npos) && (pos + count < str.length())) ? (str.begin() + static_cast<std::ptrdiff_t>(pos + count)) : str.end()))
    {
    }

    String::String(const std::u16string& str, std::size_t pos, std::size_t count) :
        m_string(utf::convertUtf16toUtf32(str.begin() + static_cast<std::ptrdiff_t>(pos),
                                          ((count != npos) && (pos + count < str.length())) ? (str.begin() + static_cast<std::ptrdiff_t>(pos + count)) : str.end()))
    {
    }

    String::String(const std::u32string& str, std::size_t pos, std::size_t count) :
        m_string(str, pos, count)
    {
    }

    String::String(const char* str, std::size_t count) :
        m_string(utf::convertUtf8toUtf32(str, str + count))
    {
    }

    String::String(const wchar_t* str, std::size_t count) :
        m_string(utf::convertWidetoUtf32(str, str + count))
    {
    }

    String::String(const char16_t* str, std::size_t count) :
        m_string(utf::convertUtf16toUtf32(str, str + count))
    {
    }

    String::String(const char32_t* str, std::size_t count) :
        m_string{str, count}
    {
    }

    String::String(std::initializer_list<char> chars) :
        m_string(utf::convertUtf8toUtf32(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<wchar_t> chars) :
        m_string(utf::convertWidetoUtf32(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<char16_t> chars) :
        m_string(utf::convertUtf16toUtf32(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<char32_t> chars) :
        m_string(chars)
    {
    }

    String::String(std::string::const_iterator first, std::string::const_iterator last) :
        m_string(utf::convertUtf8toUtf32(first, last))
    {
    }

    String::String(std::wstring::const_iterator first, std::wstring::const_iterator last) :
        m_string(utf::convertWidetoUtf32(first, last))
    {
    }

    String::String(std::u16string::const_iterator first, std::u16string::const_iterator last) :
        m_string(utf::convertUtf16toUtf32(first, last))
    {
    }

    String::String(std::u32string::const_iterator first, std::u32string::const_iterator last) :
        m_string(first, last)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::operator std::string() const
    {
        return utf::convertUtf32toStdStringUtf8(m_string);
    }

    String::operator std::wstring() const
    {
        return utf::convertUtf32toWide(m_string);
    }

    String::operator std::u16string() const
    {
        return utf::convertUtf32toUtf16(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string String::toStdString() const
    {
        return utf::convertUtf32toStdStringUtf8(m_string);
    }

    std::wstring String::toWideString() const
    {
        return utf::convertUtf32toWide(m_string);
    }

    std::u16string String::toUtf16() const
    {
        return utf::convertUtf32toUtf16(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::assign(std::size_t count, char ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::assign(std::size_t count, wchar_t ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::assign(std::size_t count, char16_t ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::assign(std::size_t count, char32_t ch)
    {
        m_string.assign(count, ch);
        return *this;
    }

    String& String::assign(StringView sv)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.assign(sv);
#else
        m_string.assign(sv.data(), sv.length());
#endif
        return *this;
    }

    String& String::assign(const char32_t* str)
    {
        m_string.assign(str);
        return *this;
    }

    String& String::assign(const std::u32string& str)
    {
        m_string.assign(str);
        return *this;
    }

    String& String::assign(const String& str)
    {
        m_string.assign(str.m_string);
        return *this;
    }

    String& String::assign(StringView sv, std::size_t pos, std::size_t count)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.assign(sv, pos, count);
#else
        if (count != npos)
            m_string.assign(sv.data() + pos, count);
        else if (pos <= sv.length())
            m_string.assign(sv.data() + pos, sv.length() - pos);
        else
            throw std::out_of_range("Invalid arguments for String::assign");
#endif
        return *this;
    }

    String& String::assign(const std::string& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(String{str, pos, count}.m_string);
        return *this;
    }

    String& String::assign(const std::wstring& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(String{str, pos, count}.m_string);
        return *this;
    }

    String& String::assign(const std::u16string& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(String{str, pos, count}.m_string);
        return *this;
    }

    String& String::assign(const std::u32string& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(str, pos, count);
        return *this;
    }

    String& String::assign(const String& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(str.m_string, pos, count);
        return *this;
    }

    String& String::assign(std::u32string&& str)
    {
        m_string.assign(std::move(str));
        return *this;
    }

    String& String::assign(String&& str)
    {
        m_string.assign(std::move(str.m_string));
        return *this;
    }

    String& String::assign(const char* str, std::size_t count)
    {
        m_string.assign(String{str, count}.m_string);
        return *this;
    }

    String& String::assign(const wchar_t* str, std::size_t count)
    {
        m_string.assign(String{str, count}.m_string);
        return *this;
    }

    String& String::assign(const char16_t* str, std::size_t count)
    {
        m_string.assign(String{str, count}.m_string);
        return *this;
    }

    String& String::assign(const char32_t* str, std::size_t count)
    {
        m_string.assign(str, count);
        return *this;
    }

    String& String::assign(std::initializer_list<char> chars)
    {
        m_string.assign(String{chars}.m_string);
        return *this;
    }

    String& String::assign(std::initializer_list<wchar_t> chars)
    {
        m_string.assign(String{chars}.m_string);
        return *this;
    }

    String& String::assign(std::initializer_list<char16_t> chars)
    {
        m_string.assign(String{chars}.m_string);
        return *this;
    }

    String& String::assign(std::initializer_list<char32_t> chars)
    {
        m_string.assign(chars);
        return *this;
    }

    String& String::assign(std::string::const_iterator first, std::string::const_iterator last)
    {
        m_string.assign(String{first, last}.m_string);
        return *this;
    }

    String& String::assign(std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        m_string.assign(String{first, last}.m_string);
        return *this;
    }

    String& String::assign(std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        m_string.assign(String{first, last}.m_string);
        return *this;
    }

    String& String::assign(std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        m_string.assign(first, last);
        return *this;
    }

    String& String::assign(StringView::const_iterator first, StringView::const_iterator last)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.assign(first, last);
#else
        if (last >= first)
            m_string.assign(first, static_cast<std::size_t>(last - first));
#endif
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reference String::at(std::size_t pos)
    {
        return m_string.at(pos);
    }

    String::const_reference String::at(std::size_t pos) const
    {
        return m_string.at(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::const_reference String::operator [](std::size_t index) const
    {
        return m_string[index];
    }

    String::reference String::operator [](std::size_t index)
    {
        return m_string[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reference String::front()
    {
        return m_string.front();
    }

    String::const_reference String::front() const
    {
        return m_string.front();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reference String::back()
    {
        return m_string.back();
    }

    String::const_reference String::back() const
    {
        return m_string.back();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::iterator String::begin() noexcept
    {
        return m_string.begin();
    }

    String::const_iterator String::begin() const noexcept
    {
        return m_string.begin();
    }

    String::const_iterator String::cbegin() const noexcept
    {
        return m_string.cbegin();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::iterator String::end() noexcept
    {
        return m_string.end();
    }

    String::const_iterator String::end() const noexcept
    {
        return m_string.end();
    }

    String::const_iterator String::cend() const noexcept
    {
        return m_string.cend();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reverse_iterator String::rbegin() noexcept
    {
        return m_string.rbegin();
    }

    String::const_reverse_iterator String::rbegin() const noexcept
    {
        return m_string.rbegin();
    }

    String::const_reverse_iterator String::crbegin() const noexcept
    {
        return m_string.crbegin();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reverse_iterator String::rend() noexcept
    {
        return m_string.rend();
    }

    String::const_reverse_iterator String::rend() const noexcept
    {
        return m_string.rend();
    }

    String::const_reverse_iterator String::crend() const noexcept
    {
        return m_string.crend();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::clear() noexcept
    {
        m_string.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::max_size() const noexcept
    {
        return m_string.max_size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::reserve(std::size_t newCap)
    {
        m_string.reserve(newCap);
    }

    std::size_t String::capacity() const noexcept
    {
        return m_string.capacity();
    }

    void String::shrink_to_fit()
    {
        m_string.shrink_to_fit();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::insert(std::size_t index, std::size_t count, char ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::insert(std::size_t index, std::size_t count, wchar_t ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::insert(std::size_t index, std::size_t count, char16_t ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::insert(std::size_t index, std::size_t count, char32_t ch)
    {
        m_string.insert(index, count, ch);
        return *this;
    }

    String& String::insert(std::size_t index, StringView sv)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.insert(index, sv);
#else
        m_string.insert(index, sv.data(), sv.length());
#endif
        return *this;
    }

    String& String::insert(std::size_t index, const char32_t* str)
    {
        m_string.insert(index, str);
        return *this;
    }

    String& String::insert(std::size_t index, const std::u32string& str)
    {
        m_string.insert(index, str);
        return *this;
    }

    String& String::insert(std::size_t index, const String& str)
    {
        m_string.insert(index, str.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, StringView sv, std::size_t pos, std::size_t count)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.insert(index, sv, pos, count);
#else
        if (index > length())
            throw std::out_of_range("Invalid arguments for String::insert");

        if (count != npos)
            m_string.insert(index, sv.data() + pos, count);
        else if (pos <= sv.length())
            m_string.insert(index, sv.data() + pos, sv.length() - pos);
        else
            throw std::out_of_range("Invalid arguments for String::insert");
#endif
        return *this;
    }

    String& String::insert(std::size_t index, const std::string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::wstring& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }
    String& String::insert(std::size_t index, const std::u16string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::u32string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, str, pos, count);
        return *this;
    }

    String& String::insert(std::size_t index, const String& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, str.m_string, pos, count);
        return *this;
    }

    String& String::insert(std::size_t index, const char* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const wchar_t* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char16_t* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char32_t* str, std::size_t count)
    {
        m_string.insert(index, str, count);
        return *this;
    }

    String::iterator String::insert(const_iterator pos, char ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, wchar_t ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, char16_t ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, char32_t ch)
    {
        return m_string.insert(pos, ch);
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, char ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, wchar_t ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, char16_t ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, char32_t ch)
    {
        return m_string.insert(pos, count, ch);
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char> chars)
    {
        const std::u32string tmpStr(utf::convertUtf8toUtf32(chars.begin(), chars.end()));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<wchar_t> chars)
    {
        const std::u32string tmpStr(utf::convertWidetoUtf32(chars.begin(), chars.end()));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char16_t> chars)
    {
        const std::u32string tmpStr(utf::convertUtf16toUtf32(chars.begin(), chars.end()));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char32_t> chars)
    {
        // This function was missing in libstdc++ with GCC 8.2 on Arch Linux. It is present now with GCC 9.1.
        //return m_string.insert(pos, chars);

        return m_string.insert(pos, chars.begin(), chars.end());
    }

    String::iterator String::insert(const_iterator pos, std::string::const_iterator first, std::string::const_iterator last)
    {
        const std::u32string tmpStr(utf::convertUtf8toUtf32(first, last));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        const std::u32string tmpStr(utf::convertWidetoUtf32(first, last));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        const std::u32string tmpStr(utf::convertUtf16toUtf32(first, last));
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        return m_string.insert(pos, first, last);
    }

    String::iterator String::insert(const_iterator pos, StringView::const_iterator first, StringView::const_iterator last)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.insert(pos, first, last);
#else
        const auto index = std::distance(m_string.cbegin(), pos);

        if ((last > first) && (index >= 0))
            m_string.insert(static_cast<std::size_t>(index), first, static_cast<std::size_t>(last - first));

        return m_string.begin() + index;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::erase(std::size_t index, std::size_t count)
    {
        m_string.erase(index, count);
        return *this;
    }

    String::iterator String::erase(const_iterator position)
    {
        return m_string.erase(position);
    }

    String::iterator String::erase(const_iterator first, const_iterator last)
    {
        return m_string.erase(first, last);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::push_back(char ch)
    {
        m_string.push_back(static_cast<char32_t>(ch));
    }

    void String::push_back(wchar_t ch)
    {
        m_string.push_back(static_cast<char32_t>(ch));
    }

    void String::push_back(char16_t ch)
    {
        m_string.push_back(static_cast<char32_t>(ch));
    }

    void String::push_back(char32_t ch)
    {
        m_string.push_back(ch);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::pop_back()
    {
        m_string.pop_back();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::append(std::size_t count, char ch)
    {
        m_string.append(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::append(std::size_t count, wchar_t ch)
    {
        m_string.append(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::append(std::size_t count, char16_t ch)
    {
        m_string.append(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::append(std::size_t count, char32_t ch)
    {
        m_string.append(count, ch);
        return *this;
    }

    String& String::append(StringView sv)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.append(sv);
#else
        m_string.append(sv.data(), sv.length());
#endif
        return *this;
    }

    String& String::append(const char32_t* str)
    {
        m_string.append(str);
        return *this;
    }

    String& String::append(const std::u32string& str)
    {
        m_string.append(str);
        return *this;
    }

    String& String::append(const String& str)
    {
        m_string.append(str.m_string);
        return *this;
    }

    String& String::append(StringView sv, std::size_t pos, std::size_t count)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.append(sv, pos, count);
#else
        if (count != npos)
            m_string.append(sv.data() + pos, count);
        else if (pos < sv.length())
            m_string.append(sv.data() + pos, sv.length() - pos);
        else
            throw std::out_of_range("Invalid arguments for String::append");
#endif
        return *this;
    }

    String& String::append(const std::string& str, std::size_t pos, std::size_t count)
    {
        m_string.append(String{str, pos, count}.m_string);
        return *this;
    }

    String& String::append(const std::wstring& str, std::size_t pos, std::size_t count)
    {
        m_string.append(String{str, pos, count}.m_string);
        return *this;
    }

    String& String::append(const std::u16string& str, std::size_t pos, std::size_t count)
    {
        m_string.append(String{str, pos, count}.m_string);
        return *this;
    }

    String& String::append(const std::u32string& str, std::size_t pos, std::size_t count)
    {
        m_string.append(str, pos, count);
        return *this;
    }

    String& String::append(const String& str, std::size_t pos, std::size_t count)
    {
        m_string.append(str.m_string, pos, count);
        return *this;
    }

    String& String::append(const char* str, std::size_t count)
    {
        m_string.append(String{str, count}.m_string);
        return *this;
    }

    String& String::append(const wchar_t* str, std::size_t count)
    {
        m_string.append(String{str, count}.m_string);
        return *this;
    }

    String& String::append(const char16_t* str, std::size_t count)
    {
        m_string.append(String{str, count}.m_string);
        return *this;
    }

    String& String::append(const char32_t* str, std::size_t count)
    {
        m_string.append(str, count);
        return *this;
    }

    String& String::append(std::initializer_list<char> chars)
    {
        m_string.append(String{chars}.m_string);
        return *this;
    }

    String& String::append(std::initializer_list<wchar_t> chars)
    {
        m_string.append(String{chars}.m_string);
        return *this;
    }

    String& String::append(std::initializer_list<char16_t> chars)
    {
        m_string.append(String{chars}.m_string);
        return *this;
    }

    String& String::append(std::initializer_list<char32_t> chars)
    {
        m_string.append(chars);
        return *this;
    }

    String& String::append(std::string::const_iterator first, std::string::const_iterator last)
    {
        m_string.append(String{first, last}.m_string);
        return *this;
    }

    String& String::append(std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        m_string.append(String{first, last}.m_string);
        return *this;
    }

    String& String::append(std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        m_string.append(String{first, last}.m_string);
        return *this;
    }

    String& String::append(std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        m_string.append(first, last);
        return *this;
    }

    String& String::append(StringView::const_iterator first, StringView::const_iterator last)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.append(first, last);
#else
        if (last > first)
            m_string.append(first, static_cast<std::size_t>(last - first));
#endif
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::operator+=(const String& str)
    {
        m_string += str.m_string;
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int String::compare(StringView sv) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.compare(sv);
#else
        return m_string.compare(0, length(), sv.data(), sv.length());
#endif
    }

    int String::compare(const char32_t* s) const
    {
        return m_string.compare(s);
    }

    int String::compare(const std::u32string& str) const noexcept
    {
        return m_string.compare(str);
    }

    int String::compare(const String& str) const noexcept
    {
        return m_string.compare(str.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, StringView sv) const
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.compare(pos1, count1, sv);
#else
        return m_string.compare(pos1, count1, sv.data(), sv.length());
#endif
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char32_t* s) const
    {
        return m_string.compare(pos1, count1, s);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u32string& str) const
    {
        return m_string.compare(pos1, count1, str);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const String& str) const
    {
        return m_string.compare(pos1, count1, str.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, StringView sv, std::size_t pos2, std::size_t count2) const
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.compare(pos1, count1, sv, pos2, count2);
#else
        return m_string.compare(pos1, count1, sv.data() + pos2, count2);
#endif
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::wstring& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u16string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u32string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, str, pos2, count2);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const String& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, str.m_string, pos2, count2);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const wchar_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char16_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char32_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, s, count2);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::replace(std::size_t pos, std::size_t count, StringView sv)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.replace(pos, count, sv);
#else
        m_string.replace(pos, count, sv.data(), sv.length());
#endif
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char32_t* cstr)
    {
        m_string.replace(pos, count, cstr);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u32string& str)
    {
        m_string.replace(pos, count, str);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const String& str)
    {
        m_string.replace(pos, count, str.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, StringView sv)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.replace(first, last, sv);
#else
        m_string.replace(first, last, sv.data(), sv.length());
#endif
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char32_t* cstr)
    {
        m_string.replace(first, last, cstr);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::u32string& str)
    {
        m_string.replace(first, last, str);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const String& str)
    {
        m_string.replace(first, last, str.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, StringView sv, std::size_t pos2, std::size_t count2)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.replace(pos, count, sv, pos2, count2);
#else
        if ((count2 != npos) && (pos2 + count2 <= sv.length()))
            m_string.replace(pos, count, sv.data() + pos2, count2);
        else if (pos2 <= sv.length())
            m_string.replace(pos, count, sv.data() + pos2, sv.length() - pos2);
        else
            throw std::out_of_range("Invalid arguments for String::replace");
#endif
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::wstring& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u16string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u32string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, str, pos2, count2);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const String& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, str.m_string, pos2, count2);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::string::const_iterator first2, std::string::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::wstring::const_iterator first2, std::wstring::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::u16string::const_iterator first2, std::u16string::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::u32string::const_iterator first2, std::u32string::const_iterator last2)
    {
        m_string.replace(first, last, first2, last2);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, StringView::const_iterator first2, StringView::const_iterator last2)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        m_string.replace(first, last, first2, last2);
#else
        if (last2 > first2)
            m_string.replace(first, last, first2, static_cast<std::size_t>(last2 - first2));
#endif
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const wchar_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char16_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char32_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, cstr, count2);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const wchar_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char16_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char32_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, cstr, count2);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, wchar_t ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char16_t ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char32_t ch)
    {
        m_string.replace(pos, count, count2, ch);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, wchar_t ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char16_t ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char32_t ch)
    {
        m_string.replace(first, last, count2, ch);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<wchar_t> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char16_t> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char32_t> chars)
    {
        m_string.replace(first, last, chars);
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::substr(std::size_t pos, std::size_t count) const
    {
        return {m_string.substr(pos, count)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::copy(char32_t* dest, std::size_t count, std::size_t pos) const
    {
        return m_string.copy(dest, count, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::resize(std::size_t count)
    {
        m_string.resize(count);
    }

    void String::resize(std::size_t count, char ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    void String::resize(std::size_t count, wchar_t ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    void String::resize(std::size_t count, char16_t ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    void String::resize(std::size_t count, char32_t ch)
    {
        m_string.resize(count, ch);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::swap(String& other)
    {
        m_string.swap(other.m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::contains(char c) const noexcept
    {
        return contains(static_cast<char32_t>(c));
    }

    bool String::contains(wchar_t c) const noexcept
    {
        return contains(static_cast<char32_t>(c));
    }

    bool String::contains(char16_t c) const noexcept
    {
        return contains(static_cast<char32_t>(c));
    }

#if TGUI_COMPILED_WITH_CPP_VER >= 17 && defined(__cpp_lib_string_contains) && (__cpp_lib_string_contains >= 202011L)
    bool String::contains(char32_t c) const noexcept
    {
        return m_string.contains(c);
    }

    bool String::contains(StringView sv) const noexcept
    {
        return m_string.contains(sv);
    }

    bool String::contains(const char32_t* s) const
    {
        return m_string.contains(s);
    }

    bool String::contains(const std::u32string& s) const
    {
        return m_string.contains(s);
    }

    bool String::contains(const String& s) const
    {
        return m_string.contains(s.m_string);
    }
#else
    bool String::contains(char32_t c) const noexcept
    {
        return find(c) != npos;
    }

    bool String::contains(StringView sv) const noexcept
    {
        return find(sv) != npos;
    }

    bool String::contains(const char32_t* s) const
    {
        return find(s) != npos;
    }

    bool String::contains(const std::u32string& s) const
    {
        return find(s) != npos;
    }

    bool String::contains(const String& s) const
    {
        return find(s.m_string) != npos;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find(StringView sv, std::size_t pos) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.find(sv, pos);
#else
        return m_string.find(sv.data(), pos, sv.length());
#endif
    }

    std::size_t String::find(const char32_t* s, std::size_t pos) const
    {
        return m_string.find(s, pos);
    }

    std::size_t String::find(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find(str, pos);
    }

    std::size_t String::find(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find(str.m_string, pos);
    }

    std::size_t String::find(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    std::size_t String::find(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    std::size_t String::find(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    std::size_t String::find(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(s, pos, count);
    }

    std::size_t String::find(char ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_first_of(StringView sv, std::size_t pos) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.find_first_of(sv, pos);
#else
        return m_string.find_first_of(sv.data(), pos, sv.length());
#endif
    }

    std::size_t String::find_first_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(s, pos);
    }

    std::size_t String::find_first_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(str, pos);
    }

    std::size_t String::find_first_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(str.m_string, pos);
    }

    std::size_t String::find_first_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(s, pos, count);
    }

    std::size_t String::find_first_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_first_not_of(StringView sv, std::size_t pos) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.find_first_not_of(sv, pos);
#else
        return m_string.find_first_not_of(sv.data(), pos, sv.length());
#endif
    }

    std::size_t String::find_first_not_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(s, pos);
    }

    std::size_t String::find_first_not_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(str, pos);
    }

    std::size_t String::find_first_not_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(str.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(s, pos, count);
    }

    std::size_t String::find_first_not_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_not_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_not_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_not_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::rfind(StringView sv, std::size_t pos) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.rfind(sv, pos);
#else
        return m_string.rfind(sv.data(), pos, sv.length());
#endif
    }

    std::size_t String::rfind(const char32_t* s, std::size_t pos) const
    {
        return m_string.rfind(s, pos);
    }

    std::size_t String::rfind(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(str, pos);
    }

    std::size_t String::rfind(const String& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(str.m_string, pos);
    }

    std::size_t String::rfind(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    std::size_t String::rfind(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    std::size_t String::rfind(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    std::size_t String::rfind(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(s, pos, count);
    }

    std::size_t String::rfind(char ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::rfind(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::rfind(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::rfind(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_last_of(StringView sv, std::size_t pos) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.find_last_of(sv, pos);
#else
        return m_string.find_last_of(sv.data(), pos, sv.length());
#endif
    }

    std::size_t String::find_last_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(s, pos);
    }

    std::size_t String::find_last_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(str, pos);
    }

    std::size_t String::find_last_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(str.m_string, pos);
    }

    std::size_t String::find_last_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(s, pos, count);
    }

    std::size_t String::find_last_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_last_not_of(StringView sv, std::size_t pos) const noexcept
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        return m_string.find_last_not_of(sv, pos);
#else
        return m_string.find_last_not_of(sv.data(), pos, sv.length());
#endif
    }

    std::size_t String::find_last_not_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(s, pos);
    }

    std::size_t String::find_last_not_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(str, pos);
    }

    std::size_t String::find_last_not_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(str.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(s, pos, count);
    }

    std::size_t String::find_last_not_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_not_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_not_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_not_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::count(char ch, std::size_t pos) const noexcept
    {
        return count(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::count(wchar_t ch, std::size_t pos) const noexcept
    {
        return count(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::count(char16_t ch, std::size_t pos) const noexcept
    {
        return count(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::count(char32_t ch, std::size_t pos) const noexcept
    {
        std::size_t counter = 0;
        const std::size_t end = m_string.size();
        for (std::size_t c = pos; c < end; ++c)
        {
            if (m_string[c] == ch)
                ++counter;
        }

        return counter;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::ostream& operator<<(std::ostream& os, const String& str)
    {
        os << std::string(str);
        return os;
    }

    std::wostream& operator<<(std::wostream& os, const String& str)
    {
        os << std::wstring(str);
        return os;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::istream& operator>>(std::istream& is, String& str)
    {
        std::string strVal;
        is >> strVal;
        str = std::move(strVal);
        return is;
    }

    std::wistream& operator>>(std::wistream& is, String& str)
    {
        std::wstring strVal;
        is >> strVal;
        str = std::move(strVal);
        return is;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
