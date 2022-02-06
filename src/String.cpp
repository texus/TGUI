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


#include <TGUI/String.hpp>

#include <algorithm>
#include <cctype> // tolower, toupper, isspace

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #include <charconv>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    const decltype(std::u32string::npos) String::npos = std::u32string::npos;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char character)
    {
        if (character == ' ' || character == '\t' || character == '\r' || character == '\n')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char32_t character)
    {
        if (character >= 128)
            return false;
        else
            return isWhitespace(static_cast<char>(character));
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
        const std::string& ansiStr = trim().toStdString();

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
        const std::string& ansiStr = trim().toStdString();

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
        return std::equal(m_string.begin(), m_string.end(), other.begin(), other.end(), [](char32_t char1, char32_t char2){
            if (char1 == char2)
                return true;
            else if ((char1 < 128) && (char2 < 128))
                return std::tolower(static_cast<unsigned char>(char1)) == std::tolower(static_cast<unsigned char>(char2));
            else
                return false;
        });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::startsWith(const String& substring) const
    {
        if (substring.length() <= length())
            return compare(0, substring.length(), substring) == 0;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::endsWith(const String& substring) const
    {
        if (substring.length() <= length())
            return compare(length() - substring.length(), substring.length(), substring) == 0;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::replace(const String& searchFor, const String& replaceWith)
    {
        std::size_t step = replaceWith.length();
        std::size_t len = searchFor.length();
        std::size_t pos = find(searchFor);

        // Replace each occurrence of search
        while (pos != npos)
        {
            replace(pos, len, replaceWith);
            pos = find(searchFor, pos + step);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> String::split(char32_t delimiter, bool trim) const
    {
        std::vector<String> substrings;

        std::size_t startPos = 0;
        std::size_t endPos = find(delimiter);
        while (endPos != npos)
        {
            substrings.push_back(substr(startPos, endPos - startPos));
            startPos = endPos + 1;
            endPos = find(delimiter, startPos);
        }

        if (startPos == 0)
            substrings.push_back(*this);
        else
            substrings.push_back(substr(startPos, length() - startPos));

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

    String::String(const std::u32string& str)
        : m_string{str}
    {
    }

    String::String(char ansiChar)
        : m_string(1, static_cast<char32_t>(ansiChar))
    {
    }

    String::String(wchar_t wideChar)
        : m_string(1, static_cast<char32_t>(wideChar))
    {
    }

    String::String(char16_t utfChar)
        : m_string(1, static_cast<char32_t>(utfChar))
    {
    }

    String::String(char32_t utfChar)
        : m_string(1, utfChar)
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

    String::String(std::size_t count, char ch)
        : m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, wchar_t ch)
        : m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, char16_t ch)
        : m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, char32_t ch)
        : m_string(count, ch)
    {
    }

    String::String(const std::string& str, std::size_t pos)
        : String{std::string(str, pos)}
    {
    }

    String::String(const std::wstring& str, std::size_t pos)
        : String{std::wstring(str, pos)}
    {
    }

    String::String(const std::u16string& str, std::size_t pos)
        : String{std::u16string(str, pos)}
    {
    }

    String::String(const std::u32string& str, std::size_t pos)
        : m_string(str, pos)
    {
    }

    String::String(const std::string& str, std::size_t pos, std::size_t count)
        : String{std::string(str, pos, count)}
    {
    }

    String::String(const std::wstring& str, std::size_t pos, std::size_t count)
        : String{std::wstring(str, pos, count)}
    {
    }

    String::String(const std::u16string& str, std::size_t pos, std::size_t count)
        : String{std::u16string(str, pos, count)}
    {
    }

    String::String(const std::u32string& str, std::size_t pos, std::size_t count)
        : m_string(str, pos, count)
    {
    }

    String::String(const char* str, std::size_t count)
        : String{std::string{str, count}}
    {
    }

    String::String(const wchar_t* str, std::size_t count)
        : String{std::wstring{str, count}}
    {
    }

    String::String(const char16_t* str, std::size_t count)
        : String{std::u16string{str, count}}
    {
    }

    String::String(const char32_t* str, std::size_t count)
        : m_string{str, count}
    {
    }

    String::String(std::initializer_list<char> chars)
        : String(std::string(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<wchar_t> chars)
        : String(std::wstring(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<char16_t> chars)
        : String(std::u16string(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<char32_t> chars)
        : m_string(chars)
    {
    }

    String::String(std::string::const_iterator first, std::string::const_iterator last)
        : String{std::string(first, last)}
    {
    }

    String::String(std::wstring::const_iterator first, std::wstring::const_iterator last)
        : String{std::wstring(first, last)}
    {
    }

    String::String(std::u16string::const_iterator first, std::u16string::const_iterator last)
        : String{std::u16string(first, last)}
    {
    }

    String::String(std::u32string::const_iterator first, std::u32string::const_iterator last)
        : m_string(first, last)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::operator std::string() const
    {
        return toStdString();
    }

    String::operator std::wstring() const
    {
        return toWideString();
    }

    String::operator std::u16string() const
    {
        return toUtf16();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    std::string String::toAnsiString() const
    {
        return toStdString();
    }
#endif

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

    String& String::assign(const std::string& str)
    {
        return *this = str;
    }

    String& String::assign(const std::wstring& str)
    {
        return *this = str;
    }

    String& String::assign(const std::u16string& str)
    {
        return *this = str;
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

    String& String::assign(const std::string& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
    }

    String& String::assign(const std::wstring& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
    }

    String& String::assign(const std::u16string& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
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

    String& String::assign(std::string&& str)
    {
        return *this = std::move(str);
    }

    String& String::assign(std::wstring&& str)
    {
        return *this = std::move(str);
    }

    String& String::assign(std::u16string&& str)
    {
        return *this = std::move(str);
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
        return *this = {str, count};
    }

    String& String::assign(const wchar_t* str, std::size_t count)
    {
        return *this = {str, count};
    }

    String& String::assign(const char16_t* str, std::size_t count)
    {
        return *this = {str, count};
    }

    String& String::assign(const char32_t* str, std::size_t count)
    {
        m_string.assign(str, count);
        return *this;
    }

    String& String::assign(const char* str)
    {
        return *this = str;
    }

    String& String::assign(const wchar_t* str)
    {
        return *this = str;
    }

    String& String::assign(const char16_t* str)
    {
        return *this = str;
    }

    String& String::assign(const char32_t* str)
    {
        m_string.assign(str);
        return *this;
    }

    String& String::assign(std::initializer_list<char> chars)
    {
        return *this = String(chars);
    }

    String& String::assign(std::initializer_list<wchar_t> chars)
    {
        return *this = String(chars);
    }

    String& String::assign(std::initializer_list<char16_t> chars)
    {
        return *this = String(chars);
    }

    String& String::assign(std::initializer_list<char32_t> chars)
    {
        m_string.assign(chars);
        return *this;
    }

    String& String::assign(std::string::const_iterator first, std::string::const_iterator last)
    {
        return *this = String(first, last);
    }

    String& String::assign(std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        return *this = String(first, last);
    }

    String& String::assign(std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        return *this = String(first, last);
    }

    String& String::assign(std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        m_string.assign(first, last);
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

    String& String::insert(std::size_t index, const std::string& str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::wstring& str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::u16string& str)
    {
        m_string.insert(index, String{str}.m_string);
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

    String& String::insert(std::size_t index, const char* str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const wchar_t* str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char16_t* str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char32_t* str)
    {
        m_string.insert(index, str);
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
        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<wchar_t> chars)
    {
        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char16_t> chars)
    {
        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char32_t> chars)
    {
        // This function was missing in libstdc++ with GCC 8.2 on Arch Linux. It is present now with GCC 9.1.
        //return m_string.insert(pos, chars);

        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::string::const_iterator first, std::string::const_iterator last)
    {
        const auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        const auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        const auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        return m_string.insert(pos, first, last);
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

    String& String::append(const std::string& str)
    {
        return append(String{str});
    }

    String& String::append(const std::wstring& str)
    {
        return append(String{str});
    }

    String& String::append(const std::u16string& str)
    {
        return append(String{str});
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

    String& String::append(const std::string& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
    }

    String& String::append(const std::wstring& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
    }

    String& String::append(const std::u16string& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
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
        return append(String{str, count});
    }

    String& String::append(const wchar_t* str, std::size_t count)
    {
        return append(String{str, count});
    }

    String& String::append(const char16_t* str, std::size_t count)
    {
        return append(String{str, count});
    }

    String& String::append(const char32_t* str, std::size_t count)
    {
        m_string.append(str, count);
        return *this;
    }

    String& String::append(const char* str)
    {
        return append(String{str});
    }

    String& String::append(const wchar_t* str)
    {
        return append(String{str});
    }

    String& String::append(const char16_t* str)
    {
        return append(String{str});
    }

    String& String::append(const char32_t* str)
    {
        m_string.append(str);
        return *this;
    }

    String& String::append(std::initializer_list<char> chars)
    {
        return append(String{chars});
    }

    String& String::append(std::initializer_list<wchar_t> chars)
    {
        return append(String{chars});
    }

    String& String::append(std::initializer_list<char16_t> chars)
    {
        return append(String{chars});
    }

    String& String::append(std::initializer_list<char32_t> chars)
    {
        m_string.append(chars);
        return *this;
    }

    String& String::append(std::string::const_iterator first, std::string::const_iterator last)
    {
        return append(String{first, last});
    }

    String& String::append(std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        return append(String{first, last});
    }

    String& String::append(std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        return append(String{first, last});
    }

    String& String::append(std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        m_string.append(first, last);
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::operator+=(const String& str)
    {
        m_string += str.m_string;
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int String::compare(const std::string& str) const noexcept
    {
        return m_string.compare(String{str}.m_string);
    }

    int String::compare(const std::wstring& str) const noexcept
    {
        return m_string.compare(String{str}.m_string);
    }

    int String::compare(const std::u16string& str) const noexcept
    {
        return m_string.compare(String{str}.m_string);
    }

    int String::compare(const std::u32string& str) const noexcept
    {
        return m_string.compare(str);
    }

    int String::compare(const String& str) const noexcept
    {
        return m_string.compare(str.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::string& str) const
    {
        return m_string.compare(pos1, count1, String{str}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::wstring& str) const
    {
        return m_string.compare(pos1, count1, String{str}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u16string& str) const
    {
        return m_string.compare(pos1, count1, String{str}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u32string& str) const
    {
        return m_string.compare(pos1, count1, str);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const String& str) const
    {
        return m_string.compare(pos1, count1, str.m_string);
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

    int String::compare(const char* s) const
    {
        return m_string.compare(String{s}.m_string);
    }

    int String::compare(const wchar_t* s) const
    {
        return m_string.compare(String{s}.m_string);
    }

    int String::compare(const char16_t* s) const
    {
        return m_string.compare(String{s}.m_string);
    }

    int String::compare(const char32_t* s) const
    {
        return m_string.compare(s);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char* s) const
    {
        return m_string.compare(pos1, count1, String{s}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const wchar_t* s) const
    {
        return m_string.compare(pos1, count1, String{s}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char16_t* s) const
    {
        return m_string.compare(pos1, count1, String{s}.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char32_t* s) const
    {
        return m_string.compare(pos1, count1, s);
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

    String& String::replace(std::size_t pos, std::size_t count, const std::string& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::wstring& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u16string& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
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

    String& String::replace(const_iterator first, const_iterator last, const std::string& str)
    {
        m_string.replace(first, last, String{str}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::wstring& str)
    {
        m_string.replace(first, last, String{str}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::u16string& str)
    {
        m_string.replace(first, last, String{str}.m_string);
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

    String& String::replace(std::size_t pos, std::size_t count, const char* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const wchar_t* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char16_t* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char32_t* cstr)
    {
        m_string.replace(pos, count, cstr);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const wchar_t* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char16_t* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char32_t* cstr)
    {
        m_string.replace(first, last, cstr);
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

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    bool String::contains(std::u32string_view sv) const noexcept
    {
#if defined(__cpp_lib_string_contains) && (__cpp_lib_string_contains >= 202011L)
        return m_string.contains(sv);
#else
        return find(sv) != npos;
#endif
    }
#endif

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

    bool String::contains(char32_t c) const noexcept
    {
#if defined(__cpp_lib_string_contains) && (__cpp_lib_string_contains >= 202011L)
        return m_string.contains(c);
#else
        return find(c) != npos;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::contains(const char* s) const
    {
        return find(s) != npos;
    }

    bool String::contains(const wchar_t* s) const
    {
        return find(s) != npos;
    }

    bool String::contains(const char16_t* s) const
    {
        return find(s) != npos;
    }

    bool String::contains(const char32_t* s) const
    {
#if defined(__cpp_lib_string_contains) && (__cpp_lib_string_contains >= 202011L)
        return m_string.contains(s);
#else
        return find(s) != npos;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
    }

    std::size_t String::find(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
    }

    std::size_t String::find(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
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

    std::size_t String::find(const char* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    std::size_t String::find(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    std::size_t String::find(const char16_t* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    std::size_t String::find(const char32_t* s, std::size_t pos) const
    {
        return m_string.find(s, pos);
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

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    std::size_t String::find(std::u32string_view sv, std::size_t pos) const noexcept
    {
        return m_string.find(sv, pos);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_first_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
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

    std::size_t String::find_first_of(const char* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(s, pos);
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

    std::size_t String::find_first_not_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
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

    std::size_t String::find_first_not_of(const char* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(s, pos);
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

    std::size_t String::rfind(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
    }

    std::size_t String::rfind(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
    }

    std::size_t String::rfind(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
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

    std::size_t String::rfind(const char* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    std::size_t String::rfind(const wchar_t* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    std::size_t String::rfind(const char16_t* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    std::size_t String::rfind(const char32_t* s, std::size_t pos) const
    {
        return m_string.rfind(s, pos);
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

    std::size_t String::find_last_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
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

    std::size_t String::find_last_of(const char* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(s, pos);
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

    std::size_t String::find_last_not_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
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

    std::size_t String::find_last_not_of(const char* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(s, pos);
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

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const String& str)
    {
        os << std::string(str);
        return os;
    }

    std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& os, const String& str)
    {
        os << std::wstring(str);
        return os;
    }

    // Doesn't compile with libc++
    //std::basic_ostream<char16_t>& operator<<(std::basic_ostream<char16_t>& os, const String& str)
    //{
    //    os << std::u16string(str);
    //    return os;
    //}

    //std::basic_ostream<char32_t>& operator<<(std::basic_ostream<char32_t>& os, const String& str)
    //{
    //    os << std::u32string(str);
    //    return os;
    //}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
