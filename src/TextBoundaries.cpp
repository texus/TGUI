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

#include <TGUI/TextBoundaries.hpp>

#include <algorithm>
#include <utility>

#if defined(TGUI_SYSTEM_WINDOWS)
    #include <TGUI/extlibs/IncludeWindows.hpp>

    #include <cstdint>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        namespace
        {
#if defined(TGUI_SYSTEM_WINDOWS)
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // The break iterator functions of ICU. Windows contains ICU since Windows 10 version 1703, so the library is
            // loaded at runtime to still support older Windows versions.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            struct IcuFunctions
            {
                using UbrkOpenFunc = void*(__cdecl*)(int, const char*, const wchar_t*, std::int32_t, int*);
                using UbrkFirstFunc = std::int32_t(__cdecl*)(void*);
                using UbrkNextFunc = std::int32_t(__cdecl*)(void*);
                using UbrkCloseFunc = void(__cdecl*)(void*);

                UbrkOpenFunc ubrkOpen = nullptr;
                UbrkFirstFunc ubrkFirst = nullptr;
                UbrkNextFunc ubrkNext = nullptr;
                UbrkCloseFunc ubrkClose = nullptr;
            };

            // Values of the UBRK_CHARACTER and UBRK_DONE constants from ICU
            constexpr int icuCharacterBreakIterator = 0;
            constexpr std::int32_t icuBreakIteratorDone = -1;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////

            [[nodiscard]] IcuFunctions loadIcuFunctions()
            {
                // The library is only searched for in the system directory, so that no other library with that name is used.
                // Windows 10 version 1903 and newer have icu.dll, older versions only have icuuc.dll.
                HMODULE moduleHandle = LoadLibraryEx(TEXT("icu.dll"), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                if (!moduleHandle)
                    moduleHandle = LoadLibraryEx(TEXT("icuuc.dll"), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                if (!moduleHandle)
                    return {};

                IcuFunctions functions;
    #if defined(__clang__)
        #pragma clang diagnostic push
        #if defined(__clang_major__) && (__clang_major__ >= 19)
            #pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
        #endif
    #elif defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-function-type"
    #endif
                functions.ubrkOpen = reinterpret_cast<IcuFunctions::UbrkOpenFunc>(GetProcAddress(moduleHandle, "ubrk_open"));
                functions.ubrkFirst = reinterpret_cast<IcuFunctions::UbrkFirstFunc>(GetProcAddress(moduleHandle, "ubrk_first"));
                functions.ubrkNext = reinterpret_cast<IcuFunctions::UbrkNextFunc>(GetProcAddress(moduleHandle, "ubrk_next"));
                functions.ubrkClose = reinterpret_cast<IcuFunctions::UbrkCloseFunc>(GetProcAddress(moduleHandle, "ubrk_close"));
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #elif defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif

                if (!functions.ubrkOpen || !functions.ubrkFirst || !functions.ubrkNext || !functions.ubrkClose)
                {
                    FreeLibrary(moduleHandle);
                    return {};
                }

                // The library remains loaded until the process ends, as the functions can be called at any time from any thread
                return functions;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////

            [[nodiscard]] const IcuFunctions& getIcuFunctions()
            {
                static const IcuFunctions functions = loadIcuFunctions();
                return functions;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Uses ICU to find where the characters start in a part of the text. Returns false if ICU isn't available.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            [[nodiscard]] bool findIcuCharacterBoundaries(const String& text,
                                                          std::size_t startIndex,
                                                          std::size_t endIndex,
                                                          std::vector<std::size_t>& boundaries)
            {
                const IcuFunctions& icu = getIcuFunctions();
                if (!icu.ubrkOpen)
                    return false;

                // ICU works with UTF-16, so we remember at which position in our text each UTF-16 code unit is located
                std::wstring utf16;
                std::vector<std::size_t> utf16ToTextIndex;
                utf16.reserve(endIndex - startIndex);
                utf16ToTextIndex.reserve(endIndex - startIndex);
                for (std::size_t i = startIndex; i < endIndex; ++i)
                {
                    const char32_t c = text[i];
                    if (c >= 0x10000)
                    {
                        utf16.push_back(static_cast<wchar_t>(0xD800 + ((c - 0x10000) >> 10)));
                        utf16.push_back(static_cast<wchar_t>(0xDC00 + ((c - 0x10000) & 0x3FF)));
                        utf16ToTextIndex.push_back(i);
                        utf16ToTextIndex.push_back(i);
                    }
                    else
                    {
                        utf16.push_back(static_cast<wchar_t>(c));
                        utf16ToTextIndex.push_back(i);
                    }
                }

                int status = 0; // U_ZERO_ERROR
                const auto length = static_cast<std::int32_t>(utf16.length());
                void* iterator = icu.ubrkOpen(icuCharacterBreakIterator, "", utf16.c_str(), length, &status);
                if (!iterator)
                    return false;

                if (status > 0) // U_FAILURE
                {
                    icu.ubrkClose(iterator);
                    return false;
                }

                for (std::int32_t pos = icu.ubrkFirst(iterator); pos != icuBreakIteratorDone; pos = icu.ubrkNext(iterator))
                {
                    if (static_cast<std::size_t>(pos) < utf16.length())
                        boundaries.push_back(utf16ToTextIndex[static_cast<std::size_t>(pos)]);
                }

                icu.ubrkClose(iterator);
                return true;
            }
#endif

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Adds the positions where the characters start in a part of the text.
            // Lines are always separated by a boundary, so callers can limit the search to the line that they need.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void addCharacterBoundaries(const String& text,
                                        std::size_t startIndex,
                                        std::size_t endIndex,
                                        std::vector<std::size_t>& boundaries)
            {
#if defined(TGUI_SYSTEM_WINDOWS)
                const std::size_t oldSize = boundaries.size();
                if (findIcuCharacterBoundaries(text, startIndex, endIndex, boundaries))
                    return;

                boundaries.resize(oldSize);
#else
                (void)text;
#endif
                // Every code point is a separate character
                for (std::size_t i = startIndex; i < endIndex; ++i)
                    boundaries.push_back(i);
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Returns the start and end of the line that contains the character at the given position.
            // The newline character at the end of the line is part of the line.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            [[nodiscard]] std::pair<std::size_t, std::size_t> findLine(const String& text, std::size_t position)
            {
                std::size_t startIndex = position;
                while ((startIndex > 0) && (text[startIndex - 1] != U'\n'))
                    --startIndex;

                std::size_t endIndex = position;
                while ((endIndex < text.length()) && (text[endIndex] != U'\n'))
                    ++endIndex;

                if (endIndex < text.length())
                    ++endIndex;

                return {startIndex, endIndex};
            }
        } // namespace

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::size_t> findCharacterBoundaries(const String& text)
        {
            std::vector<std::size_t> boundaries;
            boundaries.reserve(text.length());

            std::size_t startIndex = 0;
            while (startIndex < text.length())
            {
                const std::size_t endIndex = findLine(text, startIndex).second;
                addCharacterBoundaries(text, startIndex, endIndex, boundaries);
                startIndex = endIndex;
            }

            return boundaries;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t findPreviousCharacterBoundary(const String& text, std::size_t position)
        {
            TGUI_ASSERT((position > 0) && (position <= text.length()), "Position must be inside the text");

            // A character never contains more than one line, so only the line in front of the position has to be searched
            const auto line = findLine(text, position - 1);
            std::vector<std::size_t> boundaries;
            addCharacterBoundaries(text, line.first, line.second, boundaries);
            for (auto it = boundaries.rbegin(); it != boundaries.rend(); ++it)
            {
                if (*it < position)
                    return *it;
            }

            return position - 1;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t findNextCharacterBoundary(const String& text, std::size_t position)
        {
            TGUI_ASSERT(position < text.length(), "Position must be inside the text");

            // A character never contains more than one line, so only the line behind the position has to be searched
            const auto line = findLine(text, position);
            std::vector<std::size_t> boundaries;
            addCharacterBoundaries(text, line.first, line.second, boundaries);
            for (const std::size_t boundary : boundaries)
            {
                if (boundary > position)
                    return boundary;
            }

            return line.second;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::size_t> findWordBoundaries(const String& text)
        {
            std::vector<std::size_t> boundaries;
            for (std::size_t i = 0; i < text.length(); ++i)
            {
                if ((i == 0) || (isWhitespace(text[i - 1]) && !isWhitespace(text[i])))
                    boundaries.push_back(i);
            }

            return boundaries;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t findPreviousWordBoundary(const String& text, std::size_t position)
        {
            position = std::min(position, text.length());

            // Skip the whitespace in front of the position, and then the word in front of that whitespace
            std::size_t i = position;
            while ((i > 0) && isWhitespace(text[i - 1]))
                --i;

            if (i == 0)
                return position;

            while ((i > 0) && !isWhitespace(text[i - 1]))
                --i;

            return i;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t findNextWordBoundary(const String& text, std::size_t position)
        {
            position = std::min(position, text.length());
            std::size_t i = position;
#if defined(TGUI_SYSTEM_WINDOWS)
            // Skip the rest of the word and the whitespace behind it, so that the caret is placed at the start of the next word.
            // Screen readers read the word behind the caret, which is why they rely on this behavior.
            while ((i < text.length()) && !isWhitespace(text[i]))
                ++i;
            while ((i < text.length()) && isWhitespace(text[i]))
                ++i;
#else
            // Skip the whitespace behind the position, and then the word behind that whitespace
            while ((i < text.length()) && isWhitespace(text[i]))
                ++i;

            if (i == text.length())
                return position;

            while ((i < text.length()) && !isWhitespace(text[i]))
                ++i;
#endif
            return i;
        }
    } // namespace priv
} // namespace tgui
