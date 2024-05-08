/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Config.hpp>
#if TGUI_HAS_WINDOW_BACKEND_SFML
    #include <SFML/System/String.hpp>
#endif

#include "Tests.hpp"

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    using namespace std::literals::string_view_literals;
#else
    using namespace tgui::literals::string_view_literals;
#endif

TEST_CASE("[String]")
{
    tgui::String str;

    std::string s("abcde");
    std::wstring ws(L"\u03b1\u03b2\u03b3\u03b4\u03b5");
    std::u16string u16s(u"\u03b1\u03b2\u03b3\u03b4\u03b5");
    std::u32string u32s(U"\u03b1\u03b2\u03b3\u03b4\u03b5");

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    std::string_view view_s(s);
    std::wstring_view view_ws(ws);
    std::u16string_view view_u16s(u16s);
    std::u32string_view view_u32s(u32s);
#else
    tgui::CharStringView view_s(s);
    tgui::StringViewImpl<wchar_t> view_ws(ws);
    tgui::StringViewImpl<char16_t> view_u16s(u16s);
    tgui::StringView view_u32s(u32s);
#endif

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
    std::u8string u8s(u8"\u03b1\u03b2\u03b3\u03b4\u03b5");

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    std::u8string_view view_u8s(u8s);
#else
    tgui::StringViewImpl<char8_t> view_u8s(u8s);
#endif
#endif

    SECTION("Constructor")
    {
        REQUIRE(tgui::String().empty());

        REQUIRE(tgui::String("xyz") == "xyz");
        REQUIRE(tgui::String(u8"\U00010348") == u8"\U00010348");
        REQUIRE(tgui::String(L"\U00010348") == L"\U00010348");
        REQUIRE(tgui::String(u"\U00010348") == u"\U00010348");
        REQUIRE(tgui::String(U"\U00010348") == U"\U00010348");

        REQUIRE(tgui::String("abcde", 3) == "abc");
        REQUIRE(tgui::String(u8"\u03b1\u03b2\u03b3\u03b4\u03b5", 6) == u8"\u03b1\u03b2\u03b3");
        REQUIRE(tgui::String(L"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == L"\u03b1\u03b2\u03b3");
        REQUIRE(tgui::String(u"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == u"\u03b1\u03b2\u03b3");
        REQUIRE(tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"\u03b1\u03b2\u03b3");

        REQUIRE(tgui::String(std::string("xyz")) == "xyz");
        REQUIRE(tgui::String(std::wstring(L"\U00010348")) == L"\U00010348");
        REQUIRE(tgui::String(std::u16string(u"\U00010348")) == u"\U00010348");
        REQUIRE(tgui::String(std::u32string(U"\U00010348")) == U"\U00010348");

        REQUIRE(tgui::String(std::string("abcde"), 3) == "de");
        REQUIRE(tgui::String(std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == L"\u03b4\u03b5");
        REQUIRE(tgui::String(std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == u"\u03b4\u03b5");
        REQUIRE(tgui::String(std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"\u03b4\u03b5");

        REQUIRE(tgui::String(std::string("abcde"), 1, 3) == "bcd");
        REQUIRE(tgui::String(std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == L"\u03b2\u03b3\u03b4");
        REQUIRE(tgui::String(std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == u"\u03b2\u03b3\u03b4");
        REQUIRE(tgui::String(std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"\u03b2\u03b3\u03b4");

        REQUIRE(tgui::String('x') == 'x');
        REQUIRE(tgui::String(L'\x20AC') == L'\x20AC');
        REQUIRE(tgui::String(u'\x20AC') == u'\x20AC');
        REQUIRE(tgui::String(U'\x10348') == U'\x10348');

        REQUIRE(tgui::String(3, 'x') == "xxx");
        REQUIRE(tgui::String(3, L'\x20AC') == L"\u20AC\u20AC\u20AC");
        REQUIRE(tgui::String(3, u'\x20AC') == u"\u20AC\u20AC\u20AC");
        REQUIRE(tgui::String(3, U'\x10348') == U"\U00010348\U00010348\U00010348");

        REQUIRE(tgui::String({'a', 'b', 'c'}) == "abc");
        REQUIRE(tgui::String({L'\x03b1', L'\x03b2', L'\x03b3'}) == L"\u03b1\u03b2\u03b3");
        REQUIRE(tgui::String({u'\x03b1', u'\x03b2', u'\x03b3'}) == u"\u03b1\u03b2\u03b3");
        REQUIRE(tgui::String({U'\x03b1', U'\x03b2', U'\x03b3'}) == U"\u03b1\u03b2\u03b3");

        REQUIRE(tgui::String(s.begin(), s.end()) == "abcde");
        REQUIRE(tgui::String(ws.begin(), ws.end()) == L"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String(u16s.begin(), u16s.end()) == u"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String(u32s.begin(), u32s.end()) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");

        REQUIRE(tgui::String("xyz"sv) == "xyz");
        REQUIRE(tgui::String(L"\U00010348"sv) == L"\U00010348");
        REQUIRE(tgui::String(u"\U00010348"sv) == u"\U00010348");
        REQUIRE(tgui::String(U"\U00010348"sv) == U"\U00010348");

        REQUIRE(tgui::String("abcde"sv, 1, 3) == "bcd");
        REQUIRE(tgui::String(L"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == L"\u03b2\u03b3\u03b4");
        REQUIRE(tgui::String(u"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == u"\u03b2\u03b3\u03b4");
        REQUIRE(tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == U"\u03b2\u03b3\u03b4");

        REQUIRE(tgui::String(view_s) == U"abcde");
        REQUIRE(tgui::String(view_ws) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String(view_u16s) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String(view_u32s) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");

#if TGUI_HAS_WINDOW_BACKEND_SFML
        REQUIRE(tgui::String(sf::String("xyz")) == "xyz");
#endif

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(tgui::String(std::u8string(u8"\U00010348")) == u8"\U00010348");
        REQUIRE(tgui::String(std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 6) == u8"\u03b4\u03b5");
        REQUIRE(tgui::String(std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 2, 6) == u8"\u03b2\u03b3\u03b4");
        REQUIRE(tgui::String(u8s.begin(), u8s.end()) == u8"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String(3, u8'x') == "xxx");

    #if TGUI_COMPILED_WITH_CPP_VER >= 17
        REQUIRE(tgui::String(u8"\U00010348"sv) == u8"\U00010348");
        REQUIRE(tgui::String(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 2, 6) == u8"\u03b2\u03b3\u03b4");
    #endif

        REQUIRE(tgui::String(view_u8s) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
#endif

        REQUIRE(tgui::String(15) == "15");
        REQUIRE(tgui::String(-3) == "-3");
        REQUIRE(tgui::String(0.5) == "0.5");
    }

    SECTION("Conversions")
    {
        REQUIRE(std::string(tgui::String(U"xyz")) == "xyz");
        REQUIRE(std::wstring(tgui::String(U"\u20AC")) == L"\u20AC");
        REQUIRE(std::u16string(tgui::String(U"\U00010348")) == u"\U00010348");
        REQUIRE(std::u32string(tgui::String(U"\U00010348")) == U"\U00010348");

        REQUIRE(tgui::String(U"xyz").toStdString() == "xyz");
        REQUIRE(tgui::String(U"\u20AC").toWideString() == L"\u20AC");
        REQUIRE(tgui::String(U"\u03b1").toUtf16() == u"\u03b1");
        REQUIRE(tgui::String(U"\uFB21").toUtf16() == u"\uFB21");
        REQUIRE(tgui::String(U"\U00010348").toUtf16() == u"\U00010348");
        REQUIRE(tgui::String(U"\U00010348").toUtf32() == U"\U00010348");

        REQUIRE(tgui::StringView(tgui::String(U"\U00010348")) == U"\U00010348"sv);

#if TGUI_HAS_WINDOW_BACKEND_SFML
        REQUIRE(sf::String(tgui::String(U"test")) == sf::String("test"));
#endif

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(tgui::String(u8"\U00010348").toUtf8() == u8"\U00010348");
        REQUIRE(std::u8string(tgui::String(U"\U00010348")) == u8"\U00010348");
        REQUIRE(tgui::String(U"\U00010348").toUtf8() == u8"\U00010348");
        REQUIRE(tgui::String(U"\u20AC").toUtf8() == u8"\u20AC");
        REQUIRE(tgui::String(U"\u03b1").toUtf8() == u8"\u03b1");
#endif
    }

    SECTION("assign")
    {
        REQUIRE(str.assign("xyz") == "xyz");
        REQUIRE(str.assign(L"\U00010348") == L"\U00010348");
        REQUIRE(str.assign(u"\U00010348") == u"\U00010348");
        REQUIRE(str.assign(U"\U00010348") == U"\U00010348");

        REQUIRE(str.assign("abcde", 3) == "abc");
        REQUIRE(str.assign(L"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == L"\u03b1\u03b2\u03b3");
        REQUIRE(str.assign(u"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == u"\u03b1\u03b2\u03b3");
        REQUIRE(str.assign(U"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"\u03b1\u03b2\u03b3");

        REQUIRE(str.assign(std::string("xyz")) == "xyz");
        REQUIRE(str.assign(std::wstring(L"\U00010348")) == L"\U00010348");
        REQUIRE(str.assign(std::u16string(u"\U00010348")) == u"\U00010348");
        REQUIRE(str.assign(std::u32string(U"\U00010348")) == U"\U00010348");
        REQUIRE(str.assign(tgui::String(U"\U00010348")) == U"\U00010348");
        REQUIRE(str.assign(U"\U00010348"sv) == U"\U00010348");

        REQUIRE(str.assign(std::string("abcde"), 3) == "de");
        REQUIRE(str.assign(std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == L"\u03b4\u03b5");
        REQUIRE(str.assign(std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == u"\u03b4\u03b5");
        REQUIRE(str.assign(std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"\u03b4\u03b5");
        REQUIRE(str.assign(tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"\u03b4\u03b5");
        REQUIRE(str.assign(U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 3) == U"\u03b4\u03b5");

        REQUIRE(str.assign(std::string("abcde"), 1, 3) == "bcd");
        REQUIRE(str.assign(std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == L"\u03b2\u03b3\u03b4");
        REQUIRE(str.assign(std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == u"\u03b2\u03b3\u03b4");
        REQUIRE(str.assign(std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"\u03b2\u03b3\u03b4");
        REQUIRE(str.assign(tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"\u03b2\u03b3\u03b4");
        REQUIRE(str.assign(U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == U"\u03b2\u03b3\u03b4");

        REQUIRE(str.assign('x') == 'x');
        REQUIRE(str.assign(L'\x20AC') == L'\x20AC');
        REQUIRE(str.assign(u'\x20AC') == u'\x20AC');
        REQUIRE(str.assign(U'\x10348') == U'\x10348');

        REQUIRE(str.assign(3, 'x') == "xxx");
        REQUIRE(str.assign(3, L'\x20AC') == L"\u20AC\u20AC\u20AC");
        REQUIRE(str.assign(3, u'\x20AC') == u"\u20AC\u20AC\u20AC");
        REQUIRE(str.assign(3, U'\x10348') == U"\U00010348\U00010348\U00010348");

        REQUIRE(str.assign({'a', 'b', 'c'}) == "abc");
        REQUIRE(str.assign({L'\x03b1', L'\x03b2', L'\x03b3'}) == L"\u03b1\u03b2\u03b3");
        REQUIRE(str.assign({u'\x03b1', u'\x03b2', u'\x03b3'}) == u"\u03b1\u03b2\u03b3");
        REQUIRE(str.assign({U'\x03b1', U'\x03b2', U'\x03b3'}) == U"\u03b1\u03b2\u03b3");

        REQUIRE(str.assign(s.begin(), s.end()) == "abcde");
        REQUIRE(str.assign(ws.begin(), ws.end()) == L"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(str.assign(u16s.begin(), u16s.end()) == u"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(str.assign(u32s.begin(), u32s.end()) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(str.assign(view_u32s.begin(), view_u32s.end()) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");

        REQUIRE(str.assign(s) == "abcde");
        REQUIRE(str.assign(ws) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(str.assign(u16s) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(str.assign(u32s) == U"\u03b1\u03b2\u03b3\u03b4\u03b5");

        tgui::String str2 = "abc";
        REQUIRE(str.assign(str2) == "abc");
        REQUIRE(str.assign(tgui::String("xyz")) == "xyz");
        REQUIRE(str.assign(tgui::String("abcde"), 2, 2) == "cd");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(str.assign(u8"\U00010348") == u8"\U00010348");
        REQUIRE(str.assign(u8"\u03b1\u03b2\u03b3\u03b4\u03b5", 6) == u8"\u03b1\u03b2\u03b3");
        REQUIRE(str.assign(std::u8string(u8"\U00010348")) == u8"\U00010348");
        REQUIRE(str.assign(std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 6) == u8"\u03b4\u03b5");
        REQUIRE(str.assign(std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 2, 6) == u8"\u03b2\u03b3\u03b4");
        REQUIRE(str.assign(3, u8'x') == u8"xxx");
        REQUIRE(str.assign({u8'a', u8'b', u8'c'}) == u8"abc");
        REQUIRE(str.assign(u8s.begin(), u8s.end()) == u8"\u03b1\u03b2\u03b3\u03b4\u03b5");
#endif
    }

    SECTION("Accessing characters")
    {
        tgui::String str1 = "abc";
        const tgui::String str2(u8"\u03b1\u03b2\u03b3", 6);

        SECTION("at")
        {
            REQUIRE(str1.at(0) == U'a');
            REQUIRE(str2.at(0) == U'\x03b1');

            str1.at(0) = U'd';
            REQUIRE(str1.at(0) == U'd');
        }

        SECTION("operator[]")
        {
            REQUIRE(str1[1] == U'b');
            REQUIRE(str2[1] == U'\x03b2');

            str1[1] = U'e';
            REQUIRE(str1[1] == U'e');
        }

        SECTION("front")
        {
            REQUIRE(str1.front() == U'a');
            REQUIRE(str2.front() == U'\x03b1');

            str1.front() = U'd';
            REQUIRE(str1.front() == U'd');
        }

        SECTION("back")
        {
            REQUIRE(str1.back() == U'c');
            REQUIRE(str2.back() == U'\x03b3');

            str1.back() = U'f';
            REQUIRE(str1.back() == U'f');
        }

        SECTION("data")
        {
            REQUIRE(str1.data()[1] == U'b');
            REQUIRE(str2.data()[1] == U'\x03b2');

            str1.data()[1] = U'e';
            REQUIRE(str1.data()[1] == U'e');
        }

        SECTION("c_str")
        {
            REQUIRE(str1.c_str()[1] == U'b');
            REQUIRE(str2.c_str()[1] == U'\x03b2');
        }

        SECTION("begin / end")
        {
            REQUIRE(*str1.begin() == U'a');
            REQUIRE(*(str2.begin() + 2) == U'\x03b3');

            *(str1.begin() + 1) = U'e';
            REQUIRE(*(str1.begin() + 1) == U'e');

            REQUIRE(str1.begin() + 3 == str1.end());
            REQUIRE(str2.begin() + 3 == str2.end());
        }

        SECTION("cbegin / cend")
        {
            REQUIRE(*str1.cbegin() == U'a');
            REQUIRE(*(str2.cbegin() + 2) == U'\x03b3');

            REQUIRE(str1.cbegin() + 3 == str1.cend());
            REQUIRE(str2.cbegin() + 3 == str2.cend());
        }

        SECTION("rbegin / rend")
        {
            REQUIRE(*str1.rbegin() == U'c');
            REQUIRE(*(str2.rbegin() + 2) == U'\x03b1');

            *(str1.rbegin() + 1) = U'e';
            REQUIRE(*(str1.rbegin() + 1) == U'e');

            REQUIRE(str1.rbegin() + 3 == str1.rend());
            REQUIRE(str2.rbegin() + 3 == str2.rend());
        }

        SECTION("crbegin / crend")
        {
            REQUIRE(*str1.crbegin() == U'c');
            REQUIRE(*(str2.crbegin() + 2) == U'\x03b1');

            REQUIRE(str1.crbegin() + 3 == str1.crend());
            REQUIRE(str2.crbegin() + 3 == str2.crend());
        }

        SECTION("substr")
        {
            REQUIRE(str1.substr(1, 1) == U"b");
            REQUIRE(str2.substr(1, 1) == U"\u03b2");
        }
    }

    SECTION("Size")
    {
        REQUIRE(tgui::String().max_size() <= tgui::String::npos);

        REQUIRE(str.empty());

        str = "abc";
        REQUIRE(!str.empty());
        REQUIRE(str.size() == 3);
        REQUIRE(str.length() == 3);

        str.shrink_to_fit();
        str.reserve(200);
        REQUIRE(str.capacity() >= 200);

        str.clear();
        REQUIRE(str.empty());
    }

    SECTION("insert")
    {
        str = "^$"; REQUIRE(str.insert(1, "xyz") == "^xyz$");
        str = "^$"; REQUIRE(str.insert(1, L"\U00010348") == L"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, u"\U00010348") == u"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, U"\U00010348") == U"^\U00010348$");

        str = "^$"; REQUIRE(str.insert(1, "abcde", 3) == "^abc$");
        str = "^$"; REQUIRE(str.insert(1, L"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == L"^\u03b1\u03b2\u03b3$");
        str = "^$"; REQUIRE(str.insert(1, u"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == u"^\u03b1\u03b2\u03b3$");
        str = "^$"; REQUIRE(str.insert(1, U"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");

        str = "^$"; REQUIRE(str.insert(1, std::string("xyz")) == "^xyz$");
        str = "^$"; REQUIRE(str.insert(1, std::wstring(L"\U00010348")) == L"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, std::u16string(u"\U00010348")) == u"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, std::u32string(U"\U00010348")) == U"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, tgui::String(U"\U00010348")) == U"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, U"\U00010348"sv) == U"^\U00010348$");

        str = "^$"; REQUIRE(str.insert(1, std::string("abcde"), 3) == "^de$");
        str = "^$"; REQUIRE(str.insert(1, std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == L"^\u03b4\u03b5$");
        str = "^$"; REQUIRE(str.insert(1, std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == u"^\u03b4\u03b5$");
        str = "^$"; REQUIRE(str.insert(1, std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"^\u03b4\u03b5$");
        str = "^$"; REQUIRE(str.insert(1, tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"^\u03b4\u03b5$");
        str = "^$"; REQUIRE(str.insert(1, U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 3) == U"^\u03b4\u03b5$");

        str = "^$"; REQUIRE(str.insert(1, std::string("abcde"), 1, 3) == "^bcd$");
        str = "^$"; REQUIRE(str.insert(1, std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == L"^\u03b2\u03b3\u03b4$");
        str = "^$"; REQUIRE(str.insert(1, std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == u"^\u03b2\u03b3\u03b4$");
        str = "^$"; REQUIRE(str.insert(1, std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"^\u03b2\u03b3\u03b4$");
        str = "^$"; REQUIRE(str.insert(1, tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"^\u03b2\u03b3\u03b4$");
        str = "^$"; REQUIRE(str.insert(1, U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == U"^\u03b2\u03b3\u03b4$");

        str = "^$"; REQUIRE(str.insert(1, 3, 'x') == "^xxx$");
        str = "^$"; REQUIRE(str.insert(1, 3, L'\x20AC') == L"^\u20AC\u20AC\u20AC$");
        str = "^$"; REQUIRE(str.insert(1, 3, u'\x20AC') == u"^\u20AC\u20AC\u20AC$");
        str = "^$"; REQUIRE(str.insert(1, 3, U'\x10348') == U"^\U00010348\U00010348\U00010348$");

        str = "^$"; str.insert(1, tgui::String("xyz")); REQUIRE(str == "^xyz$");
        str = "^$"; str.insert(1, tgui::String("abcde"), 2, 2); REQUIRE(str == "^cd$");

        str = "^$"; str.insert(str.begin() + 1, 3, 'x'); REQUIRE(str == "^xxx$");
        str = "^$"; str.insert(str.begin() + 1, 3, L'\x20AC'); REQUIRE(str == L"^\u20AC\u20AC\u20AC$");
        str = "^$"; str.insert(str.begin() + 1, 3, u'\x20AC'); REQUIRE(str == u"^\u20AC\u20AC\u20AC$");
        str = "^$"; str.insert(str.begin() + 1, 3, U'\x10348'); REQUIRE(str == U"^\U00010348\U00010348\U00010348$");

        str = "^$"; str.insert(str.begin() + 1, 'x'); REQUIRE(str == "^x$");
        str = "^$"; str.insert(str.begin() + 1, L'\x20AC'); REQUIRE(str == L"^\u20AC$");
        str = "^$"; str.insert(str.begin() + 1, u'\x20AC'); REQUIRE(str == u"^\u20AC$");
        str = "^$"; str.insert(str.begin() + 1, U'\x10348'); REQUIRE(str == U"^\U00010348$");

        str = "^$"; str.insert(str.begin() + 1, {'a', 'b', 'c'}); REQUIRE(str == "^abc$");
        str = "^$"; str.insert(str.begin() + 1, {L'\x03b1', L'\x03b2', L'\x03b3'}); REQUIRE(str == L"^\u03b1\u03b2\u03b3$");
        str = "^$"; str.insert(str.begin() + 1, {u'\x03b1', u'\x03b2', u'\x03b3'}); REQUIRE(str == u"^\u03b1\u03b2\u03b3$");
        str = "^$"; str.insert(str.begin() + 1, {U'\x03b1', U'\x03b2', U'\x03b3'}); REQUIRE(str == U"^\u03b1\u03b2\u03b3$");

        str = "^$"; str.insert(str.begin() + 1, s.begin(), s.end()); REQUIRE(str == "^abcde$");
        str = "^$"; str.insert(str.begin() + 1, ws.begin(), ws.end()); REQUIRE(str == L"^\u03b1\u03b2\u03b3\u03b4\u03b5$");
        str = "^$"; str.insert(str.begin() + 1, u16s.begin(), u16s.end()); REQUIRE(str == u"^\u03b1\u03b2\u03b3\u03b4\u03b5$");
        str = "^$"; str.insert(str.begin() + 1, u32s.begin(), u32s.end()); REQUIRE(str == U"^\u03b1\u03b2\u03b3\u03b4\u03b5$");
        str = "^$"; str.insert(str.begin() + 1, view_u32s.begin(), view_u32s.end()); REQUIRE(str == U"^\u03b1\u03b2\u03b3\u03b4\u03b5$");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        str = "^$"; REQUIRE(str.insert(1, u8"\U00010348") == u8"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, u8"\u03b1\u03b2\u03b3\u03b4\u03b5", 6) == u8"^\u03b1\u03b2\u03b3$");
        str = "^$"; REQUIRE(str.insert(1, std::u8string(u8"\U00010348")) == u8"^\U00010348$");
        str = "^$"; REQUIRE(str.insert(1, std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 6) == u8"^\u03b4\u03b5$");
        str = "^$"; REQUIRE(str.insert(1, std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 2, 6) == u8"^\u03b2\u03b3\u03b4$");
        str = "^$"; REQUIRE(str.insert(1, 3, u8'\x13') == u8"^\u0013\u0013\u0013$");
        str = "^$"; str.insert(str.begin() + 1, 3, u8'\x13'); REQUIRE(str == u8"^\u0013\u0013\u0013$");
        str = "^$"; str.insert(str.begin() + 1, u8'\x13'); REQUIRE(str == u8"^\u0013$");
        str = "^$"; str.insert(str.begin() + 1, {u8'\x41', u8'\x42', u8'\x43'}); REQUIRE(str == u8"^\u0041\u0042\u0043$");
        str = "^$"; str.insert(str.begin() + 1, u8s.begin(), u8s.end()); REQUIRE(str == u8"^\u03b1\u03b2\u03b3\u03b4\u03b5$");
#endif
    }

    SECTION("erase")
    {
        str = "ABCDEFGHIJK";
        REQUIRE(str.erase(2, 3) == "ABFGHIJK");

        const auto it = str.erase(str.begin() + 3);
        REQUIRE(str == "ABFHIJK");

        str.erase(it, it + 2);
        REQUIRE(str == "ABFJK");

        str.pop_back();
        REQUIRE(str == "ABFJ");
    }

    SECTION("push_back")
    {
        str.push_back('_');
        REQUIRE(str == "_");

        str.push_back(L'\x03b1');
        REQUIRE(str == U"_\u03b1");

        str.push_back(u'\x03b2');
        REQUIRE(str == U"_\u03b1\u03b2");

        str.push_back(U'\x03b3');
        REQUIRE(str == U"_\u03b1\u03b2\u03b3");
    }

    SECTION("append")
    {
        str = "@"; REQUIRE(str.append("xyz") == "@xyz");
        str = "@"; REQUIRE(str.append(L"\U00010348") == L"@\U00010348");
        str = "@"; REQUIRE(str.append(u"\U00010348") == u"@\U00010348");
        str = "@"; REQUIRE(str.append(U"\U00010348") == U"@\U00010348");

        str = "@"; REQUIRE(str.append("abcde", 3) == "@abc");
        str = "@"; REQUIRE(str.append(L"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == L"@\u03b1\u03b2\u03b3");
        str = "@"; REQUIRE(str.append(u"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == u"@\u03b1\u03b2\u03b3");
        str = "@"; REQUIRE(str.append(U"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"@\u03b1\u03b2\u03b3");

        str = "@"; REQUIRE(str.append(std::string("xyz")) == "@xyz");
        str = "@"; REQUIRE(str.append(std::wstring(L"\U00010348")) == L"@\U00010348");
        str = "@"; REQUIRE(str.append(std::u16string(u"\U00010348")) == u"@\U00010348");
        str = "@"; REQUIRE(str.append(std::u32string(U"\U00010348")) == U"@\U00010348");
        str = "@"; REQUIRE(str.append(tgui::String(U"\U00010348")) == U"@\U00010348");
        str = "@"; REQUIRE(str.append(U"\U00010348"sv) == U"@\U00010348");

        str = "@"; REQUIRE(str.append(std::string("abcde"), 3) == "@de");
        str = "@"; REQUIRE(str.append(std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == L"@\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == u"@\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"@\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 3) == U"@\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 3) == U"@\u03b4\u03b5");

        str = "@"; REQUIRE(str.append(std::string("abcde"), 1, 3) == "@bcd");
        str = "@"; REQUIRE(str.append(std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == L"@\u03b2\u03b3\u03b4");
        str = "@"; REQUIRE(str.append(std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == u"@\u03b2\u03b3\u03b4");
        str = "@"; REQUIRE(str.append(std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"@\u03b2\u03b3\u03b4");
        str = "@"; REQUIRE(str.append(tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"@\u03b2\u03b3\u03b4");
        str = "@"; REQUIRE(str.append(U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == U"@\u03b2\u03b3\u03b4");

        str = "@"; REQUIRE(str.append(3, 'x') == "@xxx");
        str = "@"; REQUIRE(str.append(3, L'\x20AC') == L"@\u20AC\u20AC\u20AC");
        str = "@"; REQUIRE(str.append(3, u'\x20AC') == u"@\u20AC\u20AC\u20AC");
        str = "@"; REQUIRE(str.append(3, U'\x10348') == U"@\U00010348\U00010348\U00010348");

        str = "@"; REQUIRE(str.append({'a', 'b', 'c'}) == "@abc");
        str = "@"; REQUIRE(str.append({L'\x03b1', L'\x03b2', L'\x03b3'}) == L"@\u03b1\u03b2\u03b3");
        str = "@"; REQUIRE(str.append({u'\x03b1', u'\x03b2', u'\x03b3'}) == u"@\u03b1\u03b2\u03b3");
        str = "@"; REQUIRE(str.append({U'\x03b1', U'\x03b2', U'\x03b3'}) == U"@\u03b1\u03b2\u03b3");

        str = "@"; REQUIRE(str.append(s.begin(), s.end()) == "@abcde");
        str = "@"; REQUIRE(str.append(ws.begin(), ws.end()) == L"@\u03b1\u03b2\u03b3\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(u16s.begin(), u16s.end()) == u"@\u03b1\u03b2\u03b3\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(u32s.begin(), u32s.end()) == U"@\u03b1\u03b2\u03b3\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(view_u32s.begin(), view_u32s.end()) == U"@\u03b1\u03b2\u03b3\u03b4\u03b5");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        str = "@"; REQUIRE(str.append(u8"\U00010348") == u8"@\U00010348");
        str = "@"; REQUIRE(str.append(u8"\u03b1\u03b2\u03b3\u03b4\u03b5", 6) == u8"@\u03b1\u03b2\u03b3");
        str = "@"; REQUIRE(str.append(std::u8string(u8"\U00010348")) == u8"@\U00010348");
        str = "@"; REQUIRE(str.append(std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 6) == u8"@\u03b4\u03b5");
        str = "@"; REQUIRE(str.append(std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 2, 6) == u8"@\u03b2\u03b3\u03b4");
        str = "@"; REQUIRE(str.append(3, u8'\x13') == u8"@\u0013\u0013\u0013");
        str = "@"; REQUIRE(str.append({u8'\x41', u8'\x42', u8'\x43'}) == u8"@\u0041\u0042\u0043");
        str = "@"; REQUIRE(str.append(u8s.begin(), u8s.end()) == u8"@\u03b1\u03b2\u03b3\u03b4\u03b5");
#endif
    }

    SECTION("operator+=")
    {
        str = "@"; REQUIRE((str += "xyz") == "@xyz");
        str = "@"; REQUIRE((str += L"\U00010348") == L"@\U00010348");
        str = "@"; REQUIRE((str += u"\U00010348") == u"@\U00010348");
        str = "@"; REQUIRE((str += U"\U00010348") == U"@\U00010348");

        str = "@"; REQUIRE((str += std::string("xyz")) == "@xyz");
        str = "@"; REQUIRE((str += std::wstring(L"\U00010348")) == L"@\U00010348");
        str = "@"; REQUIRE((str += std::u16string(u"\U00010348")) == u"@\U00010348");
        str = "@"; REQUIRE((str += std::u32string(U"\U00010348")) == U"@\U00010348");
        str = "@"; REQUIRE((str += tgui::String("xyz")) == "@xyz");

        str = "@"; REQUIRE((str += 'x') == "@x");
        str = "@"; REQUIRE((str += L'\x20AC') == L"@\u20AC");
        str = "@"; REQUIRE((str += u'\x20AC') == u"@\u20AC");
        str = "@"; REQUIRE((str += U'\x10348') == U"@\U00010348");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        str = "@"; REQUIRE((str += u8"\U00010348") == u8"@\U00010348");
        str = "@"; REQUIRE((str += std::u8string(u8"\U00010348")) == u8"@\U00010348");
        str = "@"; REQUIRE((str += u8'\x13') == u8"@\u0013");
        str = "@"; REQUIRE((str += tgui::String{u8'\x41', u8'\x42', u8'\x43'}) == u8"@\u0041\u0042\u0043");
#endif
    }

    SECTION("compare")
    {
        str = U"\u03b1\u03b2\u03b3";

        REQUIRE(str.compare("xyz") > 0);
        REQUIRE(str.compare(L"\u03b1\u03b2\u03b3") == 0);
        REQUIRE(str.compare(u"\u03b1\u03b2") > 0);
        REQUIRE(str.compare(U"\u03b1\u03b2\u03b3\u03b4") < 0);

        REQUIRE(str.compare(1, 2, "yz") > 0);
        REQUIRE(str.compare(1, 2, L"\u03b2\u03b3") == 0);
        REQUIRE(str.compare(1, 2, u"\u03b2") > 0);
        REQUIRE(str.compare(1, 2, U"\u03b2\u03b3\u03b4") < 0);

        REQUIRE(str.compare(0, 2, "xyz", 2) > 0);
        REQUIRE(str.compare(0, 2, L"\u03b1\u03b2\u03b3", 2) == 0);
        REQUIRE(str.compare(0, 2, u"\u03b1\u03b2\u03b3", 1) > 0);
        REQUIRE(str.compare(0, 2, U"\u03b1\u03b2\u03b3", 3) < 0);

        REQUIRE(str.compare(std::string("xyz")) > 0);
        REQUIRE(str.compare(std::wstring(L"\u03b1\u03b2\u03b3")) == 0);
        REQUIRE(str.compare(std::u16string(u"\u03b1\u03b2")) > 0);
        REQUIRE(str.compare(std::u32string(U"\u03b1\u03b2\u03b3\u03b4")) < 0);
        REQUIRE(str.compare(tgui::String(U"\u03b1\u03b2\u03b3\u03b4")) < 0);
        REQUIRE(str.compare(U"\u03b1\u03b2\u03b3\u03b4"sv) < 0);

        REQUIRE(str.compare(1, 2, std::string("yz")) > 0);
        REQUIRE(str.compare(1, 2, std::wstring(L"\u03b2\u03b3")) == 0);
        REQUIRE(str.compare(1, 2, std::u16string(u"\u03b2")) > 0);
        REQUIRE(str.compare(1, 2, std::u32string(U"\u03b2\u03b3\u03b4")) < 0);
        REQUIRE(str.compare(1, 2, tgui::String(U"\u03b2\u03b3\u03b4")) < 0);
        REQUIRE(str.compare(1, 2, U"\u03b2\u03b3\u03b4"sv) < 0);

        REQUIRE(str.compare(0, 2, std::string("yz"), 1, 2) > 0);
        REQUIRE(str.compare(0, 2, std::wstring(L"\u03b4\u03b1\u03b2\u03b3"), 1, 2) == 0);
        REQUIRE(str.compare(0, 2, std::u16string(u"\u03b4\u03b1\u03b2\u03b3"), 1, 1) > 0);
        REQUIRE(str.compare(0, 2, std::u32string(U"\u03b4\u03b1\u03b2\u03b3"), 1, 3) < 0);
        REQUIRE(str.compare(0, 2, tgui::String(U"\u03b4\u03b1\u03b2\u03b3"), 1, 3) < 0);
        REQUIRE(str.compare(0, 2, U"\u03b4\u03b1\u03b2\u03b3"sv, 1, 3) < 0);

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(str.compare(u8"\u03b1\u03b2\u03b3") == 0);
        REQUIRE(str.compare(1, 2, u8"\u03b2\u03b3") == 0);
        REQUIRE(str.compare(0, 2, u8"\u03b1\u03b2\u03b3", 4) == 0);
        REQUIRE(str.compare(std::u8string(u8"\u03b1\u03b2\u03b3")) == 0);
        REQUIRE(str.compare(1, 2, std::u8string(u8"\u03b2\u03b3")) == 0);
        REQUIRE(str.compare(0, 2, std::u8string(u8"\u03b4\u03b1\u03b2\u03b3"), 2, 4) == 0);
#endif
    }

    SECTION("replace")
    {
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, "abc") == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, L"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, u"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, U"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, "abcde", 3) == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, L"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, u"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, U"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, "abc") == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, L"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, U"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, "abcde", 3) == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, L"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, U"\u03b1\u03b2\u03b3\u03b4\u03b5", 3) == U"^\u03b1\u03b2\u03b3$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::string("abc")) == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::wstring(L"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::u16string(u"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::u32string(U"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, tgui::String(U"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, U"\u03b1\u03b2\u03b3"sv) == U"^\u03b1\u03b2\u03b3$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::string("abcde"), 1, 3) == U"^bcd$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::wstring(L"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::u16string(u"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::u32string(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, tgui::String(U"\u03b1\u03b2\u03b3\u03b4\u03b5"), 1, 3) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, U"\u03b1\u03b2\u03b3\u03b4\u03b5"sv, 1, 3) == U"^\u03b2\u03b3\u03b4$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, std::string("abc")) == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, std::wstring(L"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, std::u16string(u"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, std::u32string(U"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, tgui::String(U"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, U"\u03b1\u03b2\u03b3"sv) == U"^\u03b1\u03b2\u03b3$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, s.begin() + 1, s.begin() + 4) == U"^bcd$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, ws.begin() + 1, ws.begin() + 4) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u16s.begin() + 1, u16s.begin() + 4) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u32s.begin() + 1, u32s.begin() + 4) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, view_u32s.begin() + 1, view_u32s.begin() + 4) == U"^\u03b2\u03b3\u03b4$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, 3, 'b') == U"^bbb$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, 3, L'\x03b2') == U"^\u03b2\u03b2\u03b2$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, 3, u'\x03b2') == U"^\u03b2\u03b2\u03b2$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, 3, U'\x03b2') == U"^\u03b2\u03b2\u03b2$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, 3, 'b') == U"^bbb$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, 3, L'\x03b2') == U"^\u03b2\u03b2\u03b2$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, 3, u'\x03b2') == U"^\u03b2\u03b2\u03b2$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, 3, U'\x03b2') == U"^\u03b2\u03b2\u03b2$");

        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, {'a', 'b', 'c'}) == U"^abc$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, {L'\x03b1', L'\x03b2', L'\x03b3'}) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, {u'\x03b1', u'\x03b2', u'\x03b3'}) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, {U'\x03b1', U'\x03b2', U'\x03b3'}) == U"^\u03b1\u03b2\u03b3$");

        str = "^ab:abc:abc:bc$"; REQUIRE(str.replace("abc", "xyz") == "^ab:xyz:xyz:bc$");
        str = "123"; REQUIRE(str.replace("", "xyz") == "xyz1xyz2xyz3xyz");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, u8"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, u8"\u03b1\u03b2\u03b3\u03b4\u03b5", 6) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u8"\u03b1\u03b2\u03b3") == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u8"\u03b1\u03b2\u03b3\u03b4\u03b5", 6) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::u8string(u8"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, std::u8string(u8"\u03b1\u03b2\u03b3\u03b4\u03b5"), 2, 6) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, std::u8string(u8"\u03b1\u03b2\u03b3")) == U"^\u03b1\u03b2\u03b3$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, u8s.begin() + 2, u8s.begin() + 8) == U"^\u03b2\u03b3\u03b4$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(1, 2, 3, u8'b') == U"^bbb$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, 3, u8'b') == U"^bbb$");
        str = U"^a\u03b5$"; REQUIRE(str.replace(str.begin() + 1, str.begin() + 3, {u8'a', u8'b', u8'c'}) == U"^abc$");
#endif
    }

    SECTION("remove")
    {
        str = "^ab:abc:abc:bc$";
        str.remove("abc");
        REQUIRE(str == "^ab:::bc$");

        str.remove("0123456789");
        REQUIRE(str == "^ab:::bc$");

        // Removing an empty string does nothing
        str.remove("");
        REQUIRE(str == "^ab:::bc$");
    }

    SECTION("copy")
    {
        char32_t chars[4];
        chars[3] = U'$';
        str = U"\u03b1\u03b2\u03b3\u03b4\u03b5";
        REQUIRE(str.copy(static_cast<char32_t*>(chars), 3, 1) == 3);
        REQUIRE(std::u32string(static_cast<const char32_t*>(chars), 4) == U"\u03b2\u03b3\u03b4$");
    }

    SECTION("resize")
    {
        str = "@"; str.resize(3); REQUIRE(str == std::u32string(U"@\u0000\u0000", 3));
        str = "@"; str.resize(3, '_'); REQUIRE(str == "@__");
        str = "@"; str.resize(3, L'\x03b1'); REQUIRE(str == U"@\u03b1\u03b1");
        str = "@"; str.resize(3, u'\x03b1'); REQUIRE(str == U"@\u03b1\u03b1");
        str = "@"; str.resize(3, U'\x03b1'); REQUIRE(str == U"@\u03b1\u03b1");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        str = "@"; str.resize(3, u8'_'); REQUIRE(str == "@__");
#endif
    }

    SECTION("swap")
    {
        tgui::String str1 = "abc";
        tgui::String str2 = U"\u03b1\u03b2\u03b3\u03b4\u03b5";
        str1.swap(str2);
        REQUIRE(str1 == U"\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(str2 == "abc");
    }

    SECTION("contains")
    {
        str = U"abc\u03b4\u03b5\u03b6\u03b7";

        REQUIRE(str.contains("bc"));
        REQUIRE(str.contains(L"c\u03b4"));
        REQUIRE(str.contains(u"c\u03b4"));
        REQUIRE(str.contains(U"c\u03b4"));
        REQUIRE(str.contains(std::u32string(U"c\u03b4")));
        REQUIRE(str.contains(tgui::String(U"c\u03b4")));
        REQUIRE(str.contains(U"c\u03b4"sv));

        REQUIRE(str.contains('c'));
        REQUIRE(str.contains(L'\x03b4'));
        REQUIRE(str.contains(u'\x03b4'));
        REQUIRE(str.contains(U'\x03b4'));

        REQUIRE(!str.contains("abcd"));
        REQUIRE(!str.contains(L"c\u03b3"));
        REQUIRE(!str.contains(u"c\u03b3"));
        REQUIRE(!str.contains(U"c\u03b3"));

        REQUIRE(!str.contains('d'));
        REQUIRE(!str.contains(L'\x03b3'));
        REQUIRE(!str.contains(u'\x03b3'));
        REQUIRE(!str.contains(U'\x03b3'));

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(str.contains(u8'a'));
#endif
    }

    SECTION("find")
    {
        str = U"abc\u03b4\u03b5\u03b6\u03b7";

        REQUIRE(str.find("bc") == 1);
        REQUIRE(str.find(L"c\u03b4") == 2);
        REQUIRE(str.find(u"c\u03b4") == 2);
        REQUIRE(str.find(U"c\u03b4") == 2);

        REQUIRE(str.find("bcd", 1, 2) == 1);
        REQUIRE(str.find(L"c\u03b4\u03b5", 1, 2) == 2);
        REQUIRE(str.find(u"c\u03b4\u03b5", 1, 2) == 2);
        REQUIRE(str.find(U"c\u03b4\u03b5", 1, 2) == 2);

        REQUIRE(str.find(std::string("bc")) == 1);
        REQUIRE(str.find(std::wstring(L"c\u03b4")) == 2);
        REQUIRE(str.find(std::u16string(u"c\u03b4")) == 2);
        REQUIRE(str.find(std::u32string(U"c\u03b4")) == 2);
        REQUIRE(str.find(tgui::String(U"c\u03b4")) == 2);
        REQUIRE(str.find(U"c\u03b4"sv) == 2);

        REQUIRE(str.find('c') == 2);
        REQUIRE(str.find(L'\x03b4') == 3);
        REQUIRE(str.find(u'\x03b4') == 3);
        REQUIRE(str.find(U'\x03b4') == 3);

        REQUIRE(str.find_first_of("bc") == 1);
        REQUIRE(str.find_first_of(L"c\u03b4") == 2);
        REQUIRE(str.find_first_of(u"c\u03b4") == 2);
        REQUIRE(str.find_first_of(U"c\u03b4") == 2);

        REQUIRE(str.find_first_of("bcd", 1, 2) == 1);
        REQUIRE(str.find_first_of(L"c\u03b4\u03b5", 1, 2) == 2);
        REQUIRE(str.find_first_of(u"c\u03b4\u03b5", 1, 2) == 2);
        REQUIRE(str.find_first_of(U"c\u03b4\u03b5", 1, 2) == 2);

        REQUIRE(str.find_first_of(std::string("bc")) == 1);
        REQUIRE(str.find_first_of(std::wstring(L"c\u03b4")) == 2);
        REQUIRE(str.find_first_of(std::u16string(u"c\u03b4")) == 2);
        REQUIRE(str.find_first_of(std::u32string(U"c\u03b4")) == 2);
        REQUIRE(str.find_first_of(tgui::String(U"c\u03b4")) == 2);
        REQUIRE(str.find_first_of(U"c\u03b4"sv) == 2);

        REQUIRE(str.find_first_of('c') == 2);
        REQUIRE(str.find_first_of(L'\x03b4') == 3);
        REQUIRE(str.find_first_of(u'\x03b4') == 3);
        REQUIRE(str.find_first_of(U'\x03b4') == 3);

        REQUIRE(str.find_first_not_of("bc") == 0);
        REQUIRE(str.find_first_not_of(L"c\u03b4") == 0);
        REQUIRE(str.find_first_not_of(u"c\u03b4") == 0);
        REQUIRE(str.find_first_not_of(U"c\u03b4") == 0);

        REQUIRE(str.find_first_not_of("bcd", 1, 2) == 3);
        REQUIRE(str.find_first_not_of(L"c\u03b4\u03b5", 1, 2) == 1);
        REQUIRE(str.find_first_not_of(u"c\u03b4\u03b5", 1, 2) == 1);
        REQUIRE(str.find_first_not_of(U"c\u03b4\u03b5", 1, 2) == 1);

        REQUIRE(str.find_first_not_of(std::string("bc")) == 0);
        REQUIRE(str.find_first_not_of(std::wstring(L"c\u03b4")) == 0);
        REQUIRE(str.find_first_not_of(std::u16string(u"c\u03b4")) == 0);
        REQUIRE(str.find_first_not_of(std::u32string(U"c\u03b4")) == 0);
        REQUIRE(str.find_first_not_of(tgui::String(U"c\u03b4")) == 0);
        REQUIRE(str.find_first_not_of(U"c\u03b4"sv) == 0);

        REQUIRE(str.find_first_not_of('c') == 0);
        REQUIRE(str.find_first_not_of(L'\x03b4') == 0);
        REQUIRE(str.find_first_not_of(u'\x03b4') == 0);
        REQUIRE(str.find_first_not_of(U'\x03b4') == 0);

        REQUIRE(str.find_last_of("bc") == 2);
        REQUIRE(str.find_last_of(L"c\u03b4") == 3);
        REQUIRE(str.find_last_of(u"c\u03b4") == 3);
        REQUIRE(str.find_last_of(U"c\u03b4") == 3);

        REQUIRE(str.find_last_of("bcd", 5, 2) == 2);
        REQUIRE(str.find_last_of(L"c\u03b4\u03b5", 5, 2) == 3);
        REQUIRE(str.find_last_of(u"c\u03b4\u03b5", 5, 2) == 3);
        REQUIRE(str.find_last_of(U"c\u03b4\u03b5", 5, 2) == 3);

        REQUIRE(str.find_last_of(std::string("bc")) == 2);
        REQUIRE(str.find_last_of(std::wstring(L"c\u03b4")) == 3);
        REQUIRE(str.find_last_of(std::u16string(u"c\u03b4")) == 3);
        REQUIRE(str.find_last_of(std::u32string(U"c\u03b4")) == 3);
        REQUIRE(str.find_last_of(tgui::String(U"c\u03b4")) == 3);
        REQUIRE(str.find_last_of(U"c\u03b4"sv) == 3);

        REQUIRE(str.find_last_of('c') == 2);
        REQUIRE(str.find_last_of(L'\x03b4') == 3);
        REQUIRE(str.find_last_of(u'\x03b4') == 3);
        REQUIRE(str.find_last_of(U'\x03b4') == 3);

        REQUIRE(str.find_last_not_of("bc") == 6);
        REQUIRE(str.find_last_not_of(L"c\u03b4") == 6);
        REQUIRE(str.find_last_not_of(u"c\u03b4") == 6);
        REQUIRE(str.find_last_not_of(U"c\u03b4") == 6);

        REQUIRE(str.find_last_not_of("bcd", 5, 2) == 5);
        REQUIRE(str.find_last_not_of(L"c\u03b4\u03b5", 5, 2) == 5);
        REQUIRE(str.find_last_not_of(u"c\u03b4\u03b5", 5, 2) == 5);
        REQUIRE(str.find_last_not_of(U"c\u03b4\u03b5", 5, 2) == 5);

        REQUIRE(str.find_last_not_of(std::string("bc")) == 6);
        REQUIRE(str.find_last_not_of(std::wstring(L"c\u03b4")) == 6);
        REQUIRE(str.find_last_not_of(std::u16string(u"c\u03b4")) == 6);
        REQUIRE(str.find_last_not_of(std::u32string(U"c\u03b4")) == 6);
        REQUIRE(str.find_last_not_of(tgui::String(U"c\u03b4")) == 6);
        REQUIRE(str.find_last_not_of(U"c\u03b4"sv) == 6);

        REQUIRE(str.find_last_not_of('c') == 6);
        REQUIRE(str.find_last_not_of(L'\x03b4') == 6);
        REQUIRE(str.find_last_not_of(u'\x03b4') == 6);
        REQUIRE(str.find_last_not_of(U'\x03b4') == 6);

        REQUIRE(str.rfind("bc") == 1);
        REQUIRE(str.rfind(L"c\u03b4") == 2);
        REQUIRE(str.rfind(u"c\u03b4") == 2);
        REQUIRE(str.rfind(U"c\u03b4") == 2);

        REQUIRE(str.rfind("bcd", 5, 2) == 1);
        REQUIRE(str.rfind(L"c\u03b4\u03b5", 5, 2) == 2);
        REQUIRE(str.rfind(u"c\u03b4\u03b5", 5, 2) == 2);
        REQUIRE(str.rfind(U"c\u03b4\u03b5", 5, 2) == 2);

        REQUIRE(str.rfind(std::string("bc")) == 1);
        REQUIRE(str.rfind(std::wstring(L"c\u03b4")) == 2);
        REQUIRE(str.rfind(std::u16string(u"c\u03b4")) == 2);
        REQUIRE(str.rfind(std::u32string(U"c\u03b4")) == 2);
        REQUIRE(str.rfind(tgui::String(U"c\u03b4")) == 2);
        REQUIRE(str.rfind(U"c\u03b4"sv) == 2);

        REQUIRE(str.rfind('c') == 2);
        REQUIRE(str.rfind(L'\x03b4') == 3);
        REQUIRE(str.rfind(u'\x03b4') == 3);
        REQUIRE(str.rfind(U'\x03b4') == 3);

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(str.find(u8"c\u03b4") == 2);
        REQUIRE(str.find(u8"c\u03b4\u03b5", 1, 3) == 2);
        REQUIRE(str.find(std::u8string(u8"c\u03b4")) == 2);
        REQUIRE(str.find(u8'b') == 1);
        REQUIRE(str.find_first_of(u8"c\u03b4") == 2);
        REQUIRE(str.find_first_of(u8"c\u03b4\u03b5", 1, 3) == 2);
        REQUIRE(str.find_first_of(std::u8string(u8"c\u03b4")) == 2);
        REQUIRE(str.find_first_of(u8'b') == 1);
        REQUIRE(str.find_first_not_of(u8"c\u03b4") == 0);
        REQUIRE(str.find_first_not_of(u8"c\u03b4\u03b5", 1, 3) == 1);
        REQUIRE(str.find_first_not_of(std::u8string(u8"c\u03b4")) == 0);
        REQUIRE(str.find_first_not_of(u8'b') == 0);
        REQUIRE(str.find_last_of(u8"c\u03b4") == 3);
        REQUIRE(str.find_last_of(u8"c\u03b4\u03b5", 5, 3) == 3);
        REQUIRE(str.find_last_of(std::u8string(u8"c\u03b4")) == 3);
        REQUIRE(str.find_last_of(u8'b') == 1);
        REQUIRE(str.find_last_not_of(u8"c\u03b4") == 6);
        REQUIRE(str.find_last_not_of(u8"c\u03b4\u03b5", 5, 3) == 5);
        REQUIRE(str.find_last_not_of(std::u8string(u8"c\u03b4")) == 6);
        REQUIRE(str.find_last_not_of(u8'b') == 6);
        REQUIRE(str.rfind(u8"c\u03b4") == 2);
        REQUIRE(str.rfind(u8"c\u03b4\u03b5", 5, 3) == 2);
        REQUIRE(str.rfind(std::u8string(u8"c\u03b4")) == 2);
        REQUIRE(str.rfind(u8'b') == 1);
#endif
    }

    SECTION("Comparison operators")
    {
        SECTION("ASCII strings")
        {
            tgui::String str1 = "abc";
            tgui::String str2 = "xyz";

            SECTION("True")
            {
                REQUIRE(str1 == str1);
                REQUIRE(str1 == "abc");
                REQUIRE("abc" == str1);
                REQUIRE(str1 == std::string("abc"));
                REQUIRE(std::string("abc") == str1);

                REQUIRE(str1 != str2);
                REQUIRE(str1 != "xyz");
                REQUIRE("xyz" != str1);
                REQUIRE(str1 != std::string("xyz"));
                REQUIRE(std::string("xyz") != str1);

                REQUIRE(str1 < str2);
                REQUIRE(str1 < "xyz");
                REQUIRE("abc" < str2);
                REQUIRE(str1 < std::string("xyz"));
                REQUIRE(std::string("abc") < str2);

                REQUIRE(str1 <= str2);
                REQUIRE(str1 <= "xyz");
                REQUIRE("abc" <= str2);
                REQUIRE(str1 <= std::string("xyz"));
                REQUIRE(std::string("abc") <= str2);

                REQUIRE(str2 > str1);
                REQUIRE(str2 > "abc");
                REQUIRE("xyz" > str1);
                REQUIRE(str2 > std::string("abc"));
                REQUIRE(std::string("xyz") > str1);

                REQUIRE(str2 >= str1);
                REQUIRE(str2 >= "abc");
                REQUIRE("xyz" >= str1);
                REQUIRE(str2 >= std::string("abc"));
                REQUIRE(std::string("xyz") >= str1);
            }

            SECTION("False")
            {
                REQUIRE(!(str1 == str2));
                REQUIRE(!(str1 == "xyz"));
                REQUIRE(!("xyz" == str1));
                REQUIRE(!(str1 == std::string("xyz")));
                REQUIRE(!(std::string("xyz") == str1));

                REQUIRE(!(str1 != str1));
                REQUIRE(!(str1 != "abc"));
                REQUIRE(!("abc" != str1));
                REQUIRE(!(str1 != std::string("abc")));
                REQUIRE(!(std::string("abc") != str1));

                REQUIRE(!(str2 < str1));
                REQUIRE(!(str2 < "abc"));
                REQUIRE(!("xyz" < str1));
                REQUIRE(!(str2 < std::string("abc")));
                REQUIRE(!(std::string("xyz") < str1));

                REQUIRE(!(str2 <= str1));
                REQUIRE(!(str2 <= "abc"));
                REQUIRE(!("xyz" <= str1));
                REQUIRE(!(str2 <= std::string("abc")));
                REQUIRE(!(std::string("xyz") <= str1));

                REQUIRE(!(str1 > str2));
                REQUIRE(!(str1 > "xyz"));
                REQUIRE(!("abc" > str2));
                REQUIRE(!(str1 > std::string("xyz")));
                REQUIRE(!(std::string("abc") > str2));

                REQUIRE(!(str1 >= str2));
                REQUIRE(!(str1 >= "xyz"));
                REQUIRE(!("abc" >= str2));
                REQUIRE(!(str1 >= std::string("xyz")));
                REQUIRE(!(std::string("abc") >= str2));
            }
        }

        SECTION("Unicode strings")
        {
            tgui::String str1 = U"\u03b1\u03b2";
            tgui::String str2 = U"\u03b3\u03b4";

            SECTION("True")
            {
                REQUIRE(str1 == str1);
                REQUIRE(str1 == L"\u03b1\u03b2");
                REQUIRE(str1 == u"\u03b1\u03b2");
                REQUIRE(str1 == U"\u03b1\u03b2");
                REQUIRE(str1 == U"\u03b1\u03b2"sv);
                REQUIRE(L"\u03b1\u03b2" == str1);
                REQUIRE(u"\u03b1\u03b2" == str1);
                REQUIRE(U"\u03b1\u03b2" == str1);
                REQUIRE(str1 == std::wstring(L"\u03b1\u03b2"));
                REQUIRE(str1 == std::u16string(u"\u03b1\u03b2"));
                REQUIRE(str1 == std::u32string(U"\u03b1\u03b2"));
                REQUIRE(std::wstring(L"\u03b1\u03b2") == str1);
                REQUIRE(std::u16string(u"\u03b1\u03b2") == str1);
                REQUIRE(std::u32string(U"\u03b1\u03b2") == str1);

                REQUIRE(str1 != str2);
                REQUIRE(str1 != L"\u03b3\u03b4");
                REQUIRE(str1 != u"\u03b3\u03b4");
                REQUIRE(str1 != U"\u03b3\u03b4");
                REQUIRE(str1 != U"\u03b3\u03b4"sv);
                REQUIRE(L"\u03b3\u03b4" != str1);
                REQUIRE(u"\u03b3\u03b4" != str1);
                REQUIRE(U"\u03b3\u03b4" != str1);
                REQUIRE(str1 != std::wstring(L"\u03b3\u03b4"));
                REQUIRE(str1 != std::u16string(u"\u03b3\u03b4"));
                REQUIRE(str1 != std::u32string(U"\u03b3\u03b4"));
                REQUIRE(std::wstring(L"\u03b3\u03b4") != str1);
                REQUIRE(std::u16string(u"\u03b3\u03b4") != str1);
                REQUIRE(std::u32string(U"\u03b3\u03b4") != str1);

                REQUIRE(str1 < str2);
                REQUIRE(str1 < L"\u03b3\u03b4");
                REQUIRE(str1 < u"\u03b3\u03b4");
                REQUIRE(str1 < U"\u03b3\u03b4");
                REQUIRE(str1 < U"\u03b3\u03b4"sv);
                REQUIRE(L"\u03b1\u03b2" < str2);
                REQUIRE(u"\u03b1\u03b2" < str2);
                REQUIRE(U"\u03b1\u03b2" < str2);
                REQUIRE(str1 < std::wstring(L"\u03b3\u03b4"));
                REQUIRE(str1 < std::u16string(u"\u03b3\u03b4"));
                REQUIRE(str1 < std::u32string(U"\u03b3\u03b4"));
                REQUIRE(std::wstring(L"\u03b1\u03b2") < str2);
                REQUIRE(std::u16string(u"\u03b1\u03b2") < str2);
                REQUIRE(std::u32string(U"\u03b1\u03b2") < str2);

                REQUIRE(str1 <= str2);
                REQUIRE(str1 <= L"\u03b3\u03b4");
                REQUIRE(str1 <= u"\u03b3\u03b4");
                REQUIRE(str1 <= U"\u03b3\u03b4");
                REQUIRE(str1 <= U"\u03b3\u03b4"sv);
                REQUIRE(L"\u03b1\u03b2" <= str2);
                REQUIRE(u"\u03b1\u03b2" <= str2);
                REQUIRE(U"\u03b1\u03b2" <= str2);
                REQUIRE(str1 <= std::wstring(L"\u03b3\u03b4"));
                REQUIRE(str1 <= std::u16string(u"\u03b3\u03b4"));
                REQUIRE(str1 <= std::u32string(U"\u03b3\u03b4"));
                REQUIRE(std::wstring(L"\u03b1\u03b2") <= str2);
                REQUIRE(std::u16string(u"\u03b1\u03b2") <= str2);
                REQUIRE(std::u32string(U"\u03b1\u03b2") <= str2);

                REQUIRE(str2 > str1);
                REQUIRE(str2 > L"\u03b1\u03b2");
                REQUIRE(str2 > u"\u03b1\u03b2");
                REQUIRE(str2 > U"\u03b1\u03b2");
                REQUIRE(str2 > U"\u03b1\u03b2"sv);
                REQUIRE(L"\u03b3\u03b4" > str1);
                REQUIRE(u"\u03b3\u03b4" > str1);
                REQUIRE(U"\u03b3\u03b4" > str1);
                REQUIRE(str2 > std::wstring(L"\u03b1\u03b2"));
                REQUIRE(str2 > std::u16string(u"\u03b1\u03b2"));
                REQUIRE(str2 > std::u32string(U"\u03b1\u03b2"));
                REQUIRE(std::wstring(L"\u03b3\u03b4") > str1);
                REQUIRE(std::u16string(u"\u03b3\u03b4") > str1);
                REQUIRE(std::u32string(U"\u03b3\u03b4") > str1);

                REQUIRE(str2 >= str1);
                REQUIRE(str2 >= L"\u03b1\u03b2");
                REQUIRE(str2 >= u"\u03b1\u03b2");
                REQUIRE(str2 >= U"\u03b1\u03b2");
                REQUIRE(str2 >= U"\u03b1\u03b2"sv);
                REQUIRE(L"\u03b3\u03b4" >= str1);
                REQUIRE(u"\u03b3\u03b4" >= str1);
                REQUIRE(U"\u03b3\u03b4" >= str1);
                REQUIRE(str2 >= std::wstring(L"\u03b1\u03b2"));
                REQUIRE(str2 >= std::u16string(u"\u03b1\u03b2"));
                REQUIRE(str2 >= std::u32string(U"\u03b1\u03b2"));
                REQUIRE(std::wstring(L"\u03b3\u03b4") >= str1);
                REQUIRE(std::u16string(u"\u03b3\u03b4") >= str1);
                REQUIRE(std::u32string(U"\u03b3\u03b4") >= str1);

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
                REQUIRE(str1 == u8"\u03b1\u03b2");
                REQUIRE(u8"\u03b1\u03b2" == str1);
                REQUIRE(str1 == std::u8string(u8"\u03b1\u03b2"));
                REQUIRE(std::u8string(u8"\u03b1\u03b2") == str1);
                REQUIRE(str1 != u8"\u03b3\u03b4");
                REQUIRE(u8"\u03b3\u03b4" != str1);
                REQUIRE(str1 != std::u8string(u8"\u03b3\u03b4"));
                REQUIRE(std::u8string(u8"\u03b3\u03b4") != str1);
                REQUIRE(str1 < u8"\u03b3\u03b4");
                REQUIRE(u8"\u03b1\u03b2" < str2);
                REQUIRE(str1 < std::u8string(u8"\u03b3\u03b4"));
                REQUIRE(std::u8string(u8"\u03b1\u03b2") < str2);
                REQUIRE(str1 <= u8"\u03b3\u03b4");
                REQUIRE(u8"\u03b1\u03b2" <= str2);
                REQUIRE(str1 <= std::u8string(u8"\u03b3\u03b4"));
                REQUIRE(std::u8string(u8"\u03b1\u03b2") <= str2);
                REQUIRE(str2 > u8"\u03b1\u03b2");
                REQUIRE(u8"\u03b3\u03b4" > str1);
                REQUIRE(str2 > std::u8string(u8"\u03b1\u03b2"));
                REQUIRE(std::u8string(u8"\u03b3\u03b4") > str1);
                REQUIRE(str2 >= u8"\u03b1\u03b2");
                REQUIRE(u8"\u03b3\u03b4" >= str1);
                REQUIRE(str2 >= std::u8string(u8"\u03b1\u03b2"));
                REQUIRE(std::u8string(u8"\u03b3\u03b4") >= str1);
#endif
            }

            SECTION("False")
            {
                REQUIRE(!(str1 == str2));
                REQUIRE(!(str1 == L"\u03b3\u03b4"));
                REQUIRE(!(str1 == u"\u03b3\u03b4"));
                REQUIRE(!(str1 == U"\u03b3\u03b4"));
                REQUIRE(!(L"\u03b3\u03b4" == str1));
                REQUIRE(!(u"\u03b3\u03b4" == str1));
                REQUIRE(!(U"\u03b3\u03b4" == str1));
                REQUIRE(!(str1 == std::wstring(L"\u03b3\u03b4")));
                REQUIRE(!(str1 == std::u16string(u"\u03b3\u03b4")));
                REQUIRE(!(str1 == std::u32string(U"\u03b3\u03b4")));
                REQUIRE(!(std::wstring(L"\u03b3\u03b4") == str1));
                REQUIRE(!(std::u16string(u"\u03b3\u03b4") == str1));
                REQUIRE(!(std::u32string(U"\u03b3\u03b4") == str1));

                REQUIRE(!(str1 != str1));
                REQUIRE(!(str1 != L"\u03b1\u03b2"));
                REQUIRE(!(str1 != u"\u03b1\u03b2"));
                REQUIRE(!(str1 != U"\u03b1\u03b2"));
                REQUIRE(!(L"\u03b1\u03b2" != str1));
                REQUIRE(!(u"\u03b1\u03b2" != str1));
                REQUIRE(!(U"\u03b1\u03b2" != str1));
                REQUIRE(!(str1 != std::wstring(L"\u03b1\u03b2")));
                REQUIRE(!(str1 != std::u16string(u"\u03b1\u03b2")));
                REQUIRE(!(str1 != std::u32string(U"\u03b1\u03b2")));
                REQUIRE(!(std::wstring(L"\u03b1\u03b2") != str1));
                REQUIRE(!(std::u16string(u"\u03b1\u03b2") != str1));
                REQUIRE(!(std::u32string(U"\u03b1\u03b2") != str1));

                REQUIRE(!(str2 < str1));
                REQUIRE(!(str2 < L"\u03b1\u03b2"));
                REQUIRE(!(str2 < u"\u03b1\u03b2"));
                REQUIRE(!(str2 < U"\u03b1\u03b2"));
                REQUIRE(!(L"\u03b3\u03b4" < str1));
                REQUIRE(!(u"\u03b3\u03b4" < str1));
                REQUIRE(!(U"\u03b3\u03b4" < str1));
                REQUIRE(!(str2 < std::wstring(L"\u03b1\u03b2")));
                REQUIRE(!(str2 < std::u16string(u"\u03b1\u03b2")));
                REQUIRE(!(str2 < std::u32string(U"\u03b1\u03b2")));
                REQUIRE(!(std::wstring(L"\u03b3\u03b4") < str1));
                REQUIRE(!(std::u16string(u"\u03b3\u03b4") < str1));
                REQUIRE(!(std::u32string(U"\u03b3\u03b4") < str1));

                REQUIRE(!(str2 <= str1));
                REQUIRE(!(str2 <= L"\u03b1\u03b2"));
                REQUIRE(!(str2 <= u"\u03b1\u03b2"));
                REQUIRE(!(str2 <= U"\u03b1\u03b2"));
                REQUIRE(!(L"\u03b3\u03b4" <= str1));
                REQUIRE(!(u"\u03b3\u03b4" <= str1));
                REQUIRE(!(U"\u03b3\u03b4" <= str1));
                REQUIRE(!(str2 <= std::wstring(L"\u03b1\u03b2")));
                REQUIRE(!(str2 <= std::u16string(u"\u03b1\u03b2")));
                REQUIRE(!(str2 <= std::u32string(U"\u03b1\u03b2")));
                REQUIRE(!(std::wstring(L"\u03b3\u03b4") <= str1));
                REQUIRE(!(std::u16string(u"\u03b3\u03b4") <= str1));
                REQUIRE(!(std::u32string(U"\u03b3\u03b4") <= str1));

                REQUIRE(!(str1 > str2));
                REQUIRE(!(str1 > L"\u03b3\u03b4"));
                REQUIRE(!(str1 > u"\u03b3\u03b4"));
                REQUIRE(!(str1 > U"\u03b3\u03b4"));
                REQUIRE(!(L"\u03b1\u03b2" > str2));
                REQUIRE(!(u"\u03b1\u03b2" > str2));
                REQUIRE(!(U"\u03b1\u03b2" > str2));
                REQUIRE(!(str1 > std::wstring(L"\u03b3\u03b4")));
                REQUIRE(!(str1 > std::u16string(u"\u03b3\u03b4")));
                REQUIRE(!(str1 > std::u32string(U"\u03b3\u03b4")));
                REQUIRE(!(std::wstring(L"\u03b1\u03b2") > str2));
                REQUIRE(!(std::u16string(u"\u03b1\u03b2") > str2));
                REQUIRE(!(std::u32string(U"\u03b1\u03b2") > str2));

                REQUIRE(!(str1 >= str2));
                REQUIRE(!(str1 >= L"\u03b3\u03b4"));
                REQUIRE(!(str1 >= u"\u03b3\u03b4"));
                REQUIRE(!(str1 >= U"\u03b3\u03b4"));
                REQUIRE(!(L"\u03b1\u03b2" >= str2));
                REQUIRE(!(u"\u03b1\u03b2" >= str2));
                REQUIRE(!(U"\u03b1\u03b2" >= str2));
                REQUIRE(!(str1 >= std::wstring(L"\u03b3\u03b4")));
                REQUIRE(!(str1 >= std::u16string(u"\u03b3\u03b4")));
                REQUIRE(!(str1 >= std::u32string(U"\u03b3\u03b4")));
                REQUIRE(!(std::wstring(L"\u03b1\u03b2") >= str2));
                REQUIRE(!(std::u16string(u"\u03b1\u03b2") >= str2));
                REQUIRE(!(std::u32string(U"\u03b1\u03b2") >= str2));

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
                REQUIRE(!(str1 == u8"\u03b3\u03b4"));
                REQUIRE(!(u8"\u03b3\u03b4" == str1));
                REQUIRE(!(str1 == std::u8string(u8"\u03b3\u03b4")));
                REQUIRE(!(std::u8string(u8"\u03b3\u03b4") == str1));
                REQUIRE(!(str1 != u8"\u03b1\u03b2"));
                REQUIRE(!(u8"\u03b1\u03b2" != str1));
                REQUIRE(!(str1 != std::u8string(u8"\u03b1\u03b2")));
                REQUIRE(!(std::u8string(u8"\u03b1\u03b2") != str1));
                REQUIRE(!(str2 < u8"\u03b1\u03b2"));
                REQUIRE(!(u8"\u03b3\u03b4" < str1));
                REQUIRE(!(str2 < std::u8string(u8"\u03b1\u03b2")));
                REQUIRE(!(std::u8string(u8"\u03b3\u03b4") < str1));
                REQUIRE(!(str2 <= u8"\u03b1\u03b2"));
                REQUIRE(!(u8"\u03b3\u03b4" <= str1));
                REQUIRE(!(str2 <= std::u8string(u8"\u03b1\u03b2")));
                REQUIRE(!(std::u8string(u8"\u03b3\u03b4") <= str1));
                REQUIRE(!(str1 > u8"\u03b3\u03b4"));
                REQUIRE(!(u8"\u03b1\u03b2" > str2));
                REQUIRE(!(str1 > std::u8string(u8"\u03b3\u03b4")));
                REQUIRE(!(std::u8string(u8"\u03b1\u03b2") > str2));
                REQUIRE(!(str1 >= u8"\u03b3\u03b4"));
                REQUIRE(!(u8"\u03b1\u03b2" >= str2));
                REQUIRE(!(str1 >= std::u8string(u8"\u03b3\u03b4")));
                REQUIRE(!(std::u8string(u8"\u03b1\u03b2") >= str2));
#endif
            }
        }
    }

    SECTION("operator+")
    {
        tgui::String str2 = U"\u03b4";

        str = "@"; REQUIRE(str + "xyz" == "@xyz");
        str = "@"; REQUIRE(str + L"\U00010348" == L"@\U00010348");
        str = "@"; REQUIRE(str + u"\U00010348" == u"@\U00010348");
        str = "@"; REQUIRE(str + U"\U00010348" == U"@\U00010348");

        str = "@"; REQUIRE("xyz" + str == "xyz@");
        str = "@"; REQUIRE(L"\U00010348" + str == L"\U00010348@");
        str = "@"; REQUIRE(u"\U00010348" + str == u"\U00010348@");
        str = "@"; REQUIRE(U"\U00010348" + str == U"\U00010348@");

        str = "@"; REQUIRE(str + std::string("xyz") == "@xyz");
        str = "@"; REQUIRE(str + std::wstring(L"\U00010348") == L"@\U00010348");
        str = "@"; REQUIRE(str + std::u16string(u"\U00010348") == u"@\U00010348");
        str = "@"; REQUIRE(str + std::u32string(U"\U00010348") == U"@\U00010348");

        str = "@"; REQUIRE(std::string("xyz") + str == "xyz@");
        str = "@"; REQUIRE(std::wstring(L"\U00010348") + str == L"\U00010348@");
        str = "@"; REQUIRE(std::u16string(u"\U00010348") + str == u"\U00010348@");
        str = "@"; REQUIRE(std::u32string(U"\U00010348") + str == U"\U00010348@");

        str = "@"; REQUIRE(str + 'x' == "@x");
        str = "@"; REQUIRE(str + L'\x20AC' == L"@\u20AC");
        str = "@"; REQUIRE(str + u'\x20AC' == u"@\u20AC");
        str = "@"; REQUIRE(str + U'\x10348' == U"@\U00010348");

        str = "@"; REQUIRE('x' + str == "x@");
        str = "@"; REQUIRE(L'\x20AC' + str == L"\u20AC@");
        str = "@"; REQUIRE(u'\x20AC' + str == u"\u20AC@");
        str = "@"; REQUIRE(U'\x10348' + str == U"\U00010348@");

        str = "@"; REQUIRE(str + str2 == U"@\u03b4");
        str = "@"; REQUIRE(str2 + str == U"\u03b4@");
        str = "@"; REQUIRE(str + tgui::String(U"\U00010348") == U"@\U00010348");
        str = "@"; REQUIRE(tgui::String(U"\U00010348") + str == U"\U00010348@");
        str = "@"; REQUIRE(tgui::String(U"\U00010348") + tgui::String(U"\u03b4") == U"\U00010348\u03b4");

        REQUIRE(tgui::String("@") + "xyz" == "@xyz");
        REQUIRE(tgui::String("@") + L"\U00010348" == L"@\U00010348");
        REQUIRE(tgui::String("@") + u"\U00010348" == u"@\U00010348");
        REQUIRE(tgui::String("@") + U"\U00010348" == U"@\U00010348");

        REQUIRE("xyz" + tgui::String("@") == "xyz@");
        REQUIRE(L"\U00010348" + tgui::String("@") == L"\U00010348@");
        REQUIRE(u"\U00010348" + tgui::String("@") == u"\U00010348@");
        REQUIRE(U"\U00010348" + tgui::String("@") == U"\U00010348@");

        REQUIRE(tgui::String("@") + s == "@abcde");
        REQUIRE(tgui::String("@") + ws == L"@\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String("@") + u16s == u"@\u03b1\u03b2\u03b3\u03b4\u03b5");
        REQUIRE(tgui::String("@") + u32s == U"@\u03b1\u03b2\u03b3\u03b4\u03b5");

        REQUIRE(s + tgui::String("@") == "abcde@");
        REQUIRE(ws + tgui::String("@") == L"\u03b1\u03b2\u03b3\u03b4\u03b5@");
        REQUIRE(u16s + tgui::String("@") == u"\u03b1\u03b2\u03b3\u03b4\u03b5@");
        REQUIRE(u32s + tgui::String("@") == U"\u03b1\u03b2\u03b3\u03b4\u03b5@");

        REQUIRE(tgui::String("@") + std::string("xyz") == "@xyz");
        REQUIRE(tgui::String("@") + std::wstring(L"\U00010348") == L"@\U00010348");
        REQUIRE(tgui::String("@") + std::u16string(u"\U00010348") == u"@\U00010348");
        REQUIRE(tgui::String("@") + std::u32string(U"\U00010348") == U"@\U00010348");

        REQUIRE(std::string("xyz") + tgui::String("@") == "xyz@");
        REQUIRE(std::wstring(L"\U00010348") + tgui::String("@") == L"\U00010348@");
        REQUIRE(std::u16string(u"\U00010348") + tgui::String("@") == u"\U00010348@");
        REQUIRE(std::u32string(U"\U00010348") + tgui::String("@") == U"\U00010348@");

        REQUIRE(tgui::String("@") + 'x' == "@x");
        REQUIRE(tgui::String("@") + L'\x20AC' == L"@\u20AC");
        REQUIRE(tgui::String("@") + u'\x20AC' == u"@\u20AC");
        REQUIRE(tgui::String("@") + U'\x10348' == U"@\U00010348");

        REQUIRE('x' + tgui::String("@") == "x@");
        REQUIRE(L'\x20AC' + tgui::String("@") == L"\u20AC@");
        REQUIRE(u'\x20AC' + tgui::String("@") == u"\u20AC@");
        REQUIRE(U'\x10348' + tgui::String("@") == U"\U00010348@");

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        str = "@"; REQUIRE(str + u8"\U00010348" == u8"@\U00010348");
        str = "@"; REQUIRE(u8"\U00010348" + str == u8"\U00010348@");
        str = "@"; REQUIRE(str + std::u8string(u8"\U00010348") == u8"@\U00010348");
        str = "@"; REQUIRE(std::u8string(u8"\U00010348") + str == u8"\U00010348@");
        str = "@"; REQUIRE(str + u8'\x13' == u"@\u0013");
        str = "@"; REQUIRE(u8'\x13' + str == u"\u0013@");
        REQUIRE(tgui::String("@") + u8"\U00010348" == u8"@\U00010348");
        REQUIRE(u8"\U00010348" + tgui::String("@") == u8"\U00010348@");
        REQUIRE(tgui::String("@") + std::u8string(u8"\U00010348") == u8"@\U00010348");
        REQUIRE(std::u8string(u8"\U00010348") + tgui::String("@") == u8"\U00010348@");
        REQUIRE(tgui::String("@") + u8'\x13' == u8"@\u0013");
        REQUIRE(u8'\x13' + tgui::String("@") == u8"\u0013@");
#endif
    }

    SECTION("operator<< and operator>>")
    {
        tgui::String str1;
        tgui::String str2;
        tgui::String str3;

        std::stringbuf streambuf;
        std::ostream ostream(&streambuf);
        ostream << tgui::String("abc");
        REQUIRE(streambuf.str() == "abc");
        ostream << "1 2";

        std::istream istream(&streambuf);
        istream >> str1 >> str2 >> str3;
        REQUIRE(str1 == "abc1");
        REQUIRE(str2 == "2");
        REQUIRE(str3 == "");

        std::wstringbuf wstreambuf;
        std::wostream wostream(&wstreambuf);
        wostream << (tgui::String(L"\u03b1\u03b2\u03b3") + tgui::String(U"\u03b4\u03b5"));
        REQUIRE(wstreambuf.str() == L"\u03b1\u03b2\u03b3\u03b4\u03b5");
        wostream << "3 4";

        std::wistream wistream(&wstreambuf);
        wistream >> str1 >> str2 >> str3;
        REQUIRE(str1 == L"\u03b1\u03b2\u03b3\u03b4\u03b53");
        REQUIRE(str2 == "4");
        REQUIRE(str3 == "");
    }

    SECTION("attemptToInt")
    {
        int val = 0;
        str = "123";
        REQUIRE(str.attemptToInt(val));
        REQUIRE(val == 123);

        str = "text";
        REQUIRE(!str.attemptToInt(val));
        REQUIRE(val == 123);
    }

    SECTION("attemptToUInt")
    {
        unsigned int val = 0;
        str = "123";
        REQUIRE(str.attemptToUInt(val));
        REQUIRE(val == 123);

        str = "text";
        REQUIRE(!str.attemptToUInt(val));
        REQUIRE(val == 123);
    }

    SECTION("attemptToFloat")
    {
        float val = 0;
        str = "0.5";
        REQUIRE(str.attemptToFloat(val));
        REQUIRE(val == 0.5f);

        str = "text";
        REQUIRE(!str.attemptToFloat(val));
        REQUIRE(val == 0.5f);
    }

    SECTION("toInt")
    {
        str = "123";
        REQUIRE(str.toInt() == 123);

        str = "text";
        REQUIRE(str.toInt(-5) == -5);
    }

    SECTION("toUInt")
    {
        str = "123";
        REQUIRE(str.toUInt() == 123);

        str = "text";
        REQUIRE(str.toUInt() == 0);
        REQUIRE(str.toUInt(5) == 5);
    }

    SECTION("toFloat")
    {
        str = "0.5";
        REQUIRE(str.toFloat() == 0.5f);

        str = "text";
        REQUIRE(str.toFloat(-3.5f) == -3.5f);
    }

    SECTION("fromNumber")
    {
        REQUIRE(tgui::String::fromNumber(15) == "15");
        REQUIRE(tgui::String::fromNumber(-3) == "-3");
        REQUIRE(tgui::String::fromNumber(0.5) == "0.5");
        REQUIRE(tgui::String::fromNumber(uint8_t(5)) == "5");
    }

    SECTION("fromNumberRounded")
    {
        REQUIRE(tgui::String::fromNumberRounded(15.001f, 0) == "15");
        REQUIRE(tgui::String::fromNumberRounded(-3.0015f, 3) == "-3.001");
        REQUIRE(tgui::String::fromNumberRounded(0.5f, 2) == "0.50");
        REQUIRE(tgui::String::fromNumberRounded(15, 2) == "15");
        REQUIRE(tgui::String::fromNumberRounded(uint8_t(5), 2) == "5");
    }

    SECTION("trim")
    {
        str = "\t xyz\r\n";
        REQUIRE(str.trim() == "xyz");

        str = "a";
        REQUIRE(str.trim() == "a");

        str = "";
        REQUIRE(str.trim() == "");
    }

    SECTION("toLower")
    {
        str = "aBCdEfgHIJ \u20AC";
        REQUIRE(str.toLower() == "abcdefghij \u20AC");
    }

    SECTION("toUpper")
    {
        str = "aBCdEfgHIJ \u20AC";
        REQUIRE(str.toUpper() == "ABCDEFGHIJ \u20AC");
    }

    SECTION("equalIgnoreCase")
    {
        str = "aBc";
        REQUIRE(str.equalIgnoreCase("aBc"));
        REQUIRE(str.equalIgnoreCase("Abc"));

        REQUIRE(!str.equalIgnoreCase(""));
        REQUIRE(!str.equalIgnoreCase("xyz"));
        REQUIRE(!str.equalIgnoreCase("abcde"));
        REQUIRE(!str.equalIgnoreCase(U"\u03b1\u03b2\u03b3"));

        REQUIRE(tgui::viewEqualIgnoreCase("aBc", "Abc"));
        REQUIRE(tgui::viewEqualIgnoreCase(U"aBc", U"Abc"));
    }

    SECTION("split")
    {
        const tgui::String listStr("alpha, bravo, charlie");
        std::vector<tgui::String> parts = listStr.split(U',');
        REQUIRE(parts.size() == 3);
        REQUIRE(parts[0] == "alpha");
        REQUIRE(parts[1] == " bravo");
        REQUIRE(parts[2] == " charlie");

        parts = listStr.split(',', true);
        REQUIRE(parts.size() == 3);
        REQUIRE(parts[0] == "alpha");
        REQUIRE(parts[1] == "bravo");
        REQUIRE(parts[2] == "charlie");

        parts = tgui::String("xyz").split(',');
        REQUIRE(parts.size() == 1);
        REQUIRE(parts[0] == "xyz");

        parts = tgui::String("").split(',');
        REQUIRE(parts.size() == 1);
        REQUIRE(parts[0] == "");

        parts = tgui::String("a|b").split('|');
        REQUIRE(parts.size() == 2);
        REQUIRE(parts[0] == "a");
        REQUIRE(parts[1] == "b");

        parts = tgui::String("a<br>b").split("<br>");
        REQUIRE(parts.size() == 2);
        REQUIRE(parts[0] == "a");
        REQUIRE(parts[1] == "b");

        parts = tgui::String("abc").split("");
        REQUIRE(parts.size() == 3);
        REQUIRE(parts[0] == "a");
        REQUIRE(parts[1] == "b");
        REQUIRE(parts[2] == "c");
    }

    SECTION("join")
    {
        REQUIRE(tgui::String::join({"alpha", "bravo", "charlie"}, ", ") == "alpha, bravo, charlie");
        REQUIRE(tgui::String::join({"xyz"}, ", ") == "xyz");
        REQUIRE(tgui::String::join({""}, ", ") == "");
        REQUIRE(tgui::String::join({}, ", ") == "");
    }

    SECTION("starts_with")
    {
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(tgui::String{"abc"}));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with("ab"));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(U"ab"sv));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(U"abc\u20ACxyz"));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(""));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").starts_with(U"abc\u20ACxyz123"));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").starts_with(U"123abc\u20ACxyz"));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").starts_with("xyz"));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").starts_with("o"));

        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with('a'));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(L'a'));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(u'a'));
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(U'a'));

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(tgui::String(U"abc\u20ACxyz").starts_with(u8'a'));
#endif

        REQUIRE(tgui::viewStartsWith("abcxyz", "abc"));
        REQUIRE(tgui::viewStartsWith("abcxyz", 'a'));
        REQUIRE(tgui::viewStartsWith(U"abc\u20ACxyz", U"abc"));
        REQUIRE(tgui::viewStartsWith(U"abc\u20ACxyz", U'a'));
    }

    SECTION("ends_with")
    {
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(tgui::String{"xyz"}));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with("yz"));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(U"yz"sv));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(U"abc\u20ACxyz"));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(""));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").ends_with(U"abc\u20ACxyz123"));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").ends_with(U"123abc\u20ACxyz"));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").ends_with("abc"));
        REQUIRE(!tgui::String(U"abc\u20ACxyz").ends_with("o"));

        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with('z'));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(L'z'));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(u'z'));
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(U'z'));

#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        REQUIRE(tgui::String(U"abc\u20ACxyz").ends_with(u8'z'));
#endif

        REQUIRE(tgui::viewEndsWith("abcxyz", "xyz"));
        REQUIRE(tgui::viewEndsWith("abcxyz", 'z'));
        REQUIRE(tgui::viewEndsWith(U"abc\u20ACxyz", U"xyz"));
        REQUIRE(tgui::viewEndsWith(U"abc\u20ACxyz", U'z'));
    }

    SECTION("count")
    {
        REQUIRE(tgui::String(U"abc\u20ACxyz").count('a') == 1);
        REQUIRE(tgui::String(U"abc\u20ACxyz").count(L'a') == 1);
        REQUIRE(tgui::String(U"abc\u20ACxyz").count(u'a') == 1);
        REQUIRE(tgui::String(U"abc\u20ACxyz").count(U'a') == 1);

        REQUIRE(tgui::String(U"\n\n\n\n\n").count(U'\n') == 5);
        REQUIRE(tgui::String(U"a\nb\nc\u20AC\nx\ny\nz").count(U'\n') == 5);
        REQUIRE(tgui::String(U"").count(U'\n') == 0);
        REQUIRE(tgui::String(U"\n").count(U'\n') == 1);
        REQUIRE(tgui::String(U"abc\u20ACxyz").count(U'\u20AC') == 1);

        REQUIRE(tgui::String(U"a\nb\nc\u20AC\nx\ny\nz").count(U'\n', 1) == 5);
        REQUIRE(tgui::String(U"a\nb\nc\u20AC\nx\ny\nz").count(U'\n', 6) == 3);
        REQUIRE(tgui::String(U"a\nb\nc\u20AC\nx\ny\nz").count(U'a', 0) == 1);
        REQUIRE(tgui::String(U"a\nb\nc\u20AC\nx\ny\nz").count(U'a', 1) == 0);
        REQUIRE(tgui::String(U"a\nb\nc\u20AC\nx\ny\nz").count(U'a', 40) == 0);
    }

    SECTION("isWhitespace")
    {
        REQUIRE(tgui::isWhitespace(' '));
        REQUIRE(tgui::isWhitespace('\t'));
        REQUIRE(tgui::isWhitespace('\r'));
        REQUIRE(tgui::isWhitespace('\n'));
        REQUIRE(!tgui::isWhitespace('x'));

        REQUIRE(tgui::isWhitespace(U' '));
        REQUIRE(tgui::isWhitespace(U'\t'));
        REQUIRE(tgui::isWhitespace(U'\r'));
        REQUIRE(tgui::isWhitespace(U'\n'));
        REQUIRE(!tgui::isWhitespace(U'x'));
    }

    SECTION("Invalid characters")
    {
        REQUIRE(tgui::String(U"\x200000").toUtf16() == u"");
        REQUIRE(tgui::String(U"\xDBFF").toUtf16() == u"");

        TGUI_IF_CONSTEXPR (sizeof(wchar_t) == 2)
        {
            REQUIRE(tgui::String(U"\x200000").toWideString() == L"");
            REQUIRE(tgui::String(U"\xDBFF").toWideString() == L"");
        }

        REQUIRE(tgui::String(U"\x200000").toStdString() == "");
        REQUIRE(tgui::String(U"\xDBFF").toStdString() == "");

        REQUIRE(tgui::String(u"\xDBFF") == u"");
        REQUIRE(tgui::String(u"\xDBFF\x1234") == u"");

        REQUIRE(tgui::String("\xE2\x82") == "");
    }
}
