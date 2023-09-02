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


#ifndef TGUI_UTF_HPP
#define TGUI_UTF_HPP

#include <TGUI/Config.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
    #include <string>
    #include <array>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    namespace utf
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Helper function that encodes a single UTF-32 character into one or more UTF-8 characters
        /// @param input       UTF-32 character to encode
        /// @param outStrUtf8  Reference to string to which the output UTF-8 characters are appended
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename CharT> // CharT is either char or char8_t
        void encodeCharUtf8(char32_t input, std::basic_string<CharT>& outStrUtf8)
        {
            if (input < 128)
            {
                outStrUtf8.push_back(static_cast<CharT>(input));
                return;
            }

            // Encode the character (if it is valid)
            if ((input > 0x0010FFFF) || ((input >= 0xD800) && (input <= 0xDBFF)))
                return;

            // Get the number of bytes to write
            std::size_t bytestoWrite;
            std::uint8_t firstByteMask;
            if (input < 0x800)
            {
                bytestoWrite = 2;
                firstByteMask = 0xC0;
            }
            else if (input < 0x10000)
            {
                bytestoWrite = 3;
                firstByteMask = 0xE0;
            }
            else
            {
                bytestoWrite = 4;
                firstByteMask = 0xF0;
            }

            // Extract the bytes to write
            std::array<CharT, 4> bytes;
            if (bytestoWrite == 4) { bytes[3] = static_cast<CharT>((input | 0x80) & 0xBF); input >>= 6; }
            if (bytestoWrite >= 3) { bytes[2] = static_cast<CharT>((input | 0x80) & 0xBF); input >>= 6; }
            bytes[1] = static_cast<CharT>((input | 0x80) & 0xBF); input >>= 6;
            bytes[0] = static_cast<CharT>(input | firstByteMask);

            // Add them to the output
            outStrUtf8.append(bytes.begin(), bytes.begin() + static_cast<std::ptrdiff_t>(bytestoWrite));
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Helper function that decodes one or more UTF-8 characters into a single UTF-32 character
        /// @param inputCharIt  Iterator to the UTF-8 character that should be decoded
        /// @param inputEndIt   Iterator to the end of the input
        /// @param outStrUtf32  Reference to string to which the output UTF-32 character is appended
        /// @return Iterator to the next UTF-8 character or inputEndIt if there are no more characters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename CharIt> // CharIt is an iterator for a string containing either char or char8_t
        CharIt decodeCharUtf8(CharIt inputCharIt, CharIt inputEndIt, std::u32string& outStrUtf32)
        {
            if (static_cast<std::uint8_t>(*inputCharIt) < 128)
            {
                outStrUtf32.push_back(static_cast<char32_t>(static_cast<std::uint8_t>(*inputCharIt)));
                return ++inputCharIt;
            }

            // Some useful precomputed data
            static const std::uint32_t offsetsMap[6] = { 0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080 };
            static const std::uint8_t trailingMap[128] =
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
            };

            // decode the character
            const std::uint8_t trailingBytes = trailingMap[static_cast<std::uint8_t>(*inputCharIt) - 128];
            const std::uint32_t offset = offsetsMap[trailingBytes];
            const auto remainingBytes = std::distance(inputCharIt, inputEndIt) - 1;
            if (remainingBytes >= static_cast<decltype(remainingBytes)>(trailingBytes))
            {
                char32_t outputChar = 0;
                for (std::uint8_t i = 0; i < trailingBytes; ++i)
                {
                    outputChar += static_cast<char32_t>(static_cast<std::uint8_t>(*inputCharIt++));
                    outputChar <<= 6;
                }

                outputChar += static_cast<char32_t>(static_cast<std::uint8_t>(*inputCharIt++));
                outputChar -= offset;
                outStrUtf32.push_back(outputChar);
            }
            else // Incomplete character
                inputCharIt = inputEndIt;

            return inputCharIt;
        }


#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an UTF-32 string to UTF-8
        /// @param strUtf32  Input UTF-32 string
        /// @return Output UTF-8 string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD inline std::u8string convertUtf32toUtf8(const std::u32string& strUtf32)
        {
            std::u8string outStrUtf8;
            outStrUtf8.reserve(strUtf32.length() + 1);
            for (const char32_t& codepoint : strUtf32)
                encodeCharUtf8(codepoint, outStrUtf8);

            return outStrUtf8;
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an UTF-8 string to UTF-32
        /// @param inputBegin  Begin iterator to input UTF-8 string
        /// @param inputBegin  End iterator to input UTF-8 string
        /// @return Output UTF-32 string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename CharIt>
        TGUI_NODISCARD std::u32string convertUtf8toUtf32(CharIt inputBegin, CharIt inputEnd)
        {
            std::u32string outStrUtf32;
            outStrUtf32.reserve(static_cast<std::size_t>((inputEnd - inputBegin) + 1));

            auto it = inputBegin;
            while (it < inputEnd)
                it = decodeCharUtf8(it, inputEnd, outStrUtf32);

            return outStrUtf32;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an UTF-16 string to UTF-32
        /// @param inputBegin Begin iterator to input UTF-16 string
        /// @param inputEnd   End iterator to input UTF-16 string
        /// @return Output UTF-32 string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename U16CharIt>
        TGUI_NODISCARD std::u32string convertUtf16toUtf32(U16CharIt inputBegin, U16CharIt inputEnd)
        {
            std::u32string outStrUtf32;
            outStrUtf32.reserve(static_cast<std::size_t>((inputEnd - inputBegin) + 1));

            auto it = inputBegin;
            while (it < inputEnd)
            {
                const char16_t first = *it++;

                // Copy the character if it isn't a surrogate pair
                if ((first < 0xD800) || (first > 0xDBFF))
                {
                    outStrUtf32.push_back(static_cast<char32_t>(first));
                    continue;
                }

                // We need to read another character
                if (it == inputEnd)
                    break;

                const char16_t second = *it++;
                if ((second >= 0xDC00) && (second <= 0xDFFF))
                    outStrUtf32.push_back(((static_cast<char32_t>(first) - 0xD800) << 10) + (static_cast<char32_t>(second) - 0xDC00) + 0x0010000);
            }


            return outStrUtf32;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an std::wstring string to UTF-32
        /// @param str  Input wstring to copy
        /// @return Output UTF-32 string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename WCharIt>
        TGUI_NODISCARD std::u32string convertWidetoUtf32(WCharIt inputBegin, WCharIt inputEnd)
        {
            std::u32string outStrUtf32;
            outStrUtf32.reserve(static_cast<std::size_t>((inputEnd - inputBegin) + 1));

            // std::wstring uses UCS-2 on Windows and UCS-4 on unix, so we can be cast directly
            for (auto it = inputBegin; it != inputEnd; ++it)
                outStrUtf32.push_back(static_cast<char32_t>(*it));


            return outStrUtf32;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an UTF-32 string to UTF-8
        /// @param strUtf32  Input UTF-32 string
        /// @return Output UTF-8 string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD inline std::string convertUtf32toStdStringUtf8(const std::u32string& strUtf32)
        {
            std::string outStrUtf8;
            outStrUtf8.reserve(strUtf32.length() + 1);
            for (const char32_t codepoint : strUtf32)
                encodeCharUtf8(codepoint, outStrUtf8);

            return outStrUtf8;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an UTF-32 string to std::wstring
        /// @param strUtf32  Input UTF-32 string
        /// @return Output wstring
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD inline std::wstring convertUtf32toWide(const std::u32string& strUtf32)
        {
            std::wstring outStr;
            outStr.reserve(strUtf32.length() + 1);

            TGUI_IF_CONSTEXPR (sizeof(wchar_t) == 4)
            {
                // On Unix, wide characters are UCS-4 and we can just copy the characters
                for (const char32_t codepoint : strUtf32)
                    outStr.push_back(static_cast<wchar_t>(codepoint));
            }
            else
            {
                // On Windows, wide characters are UCS-2. We just drop the characters that don't fit within a single wide character here.
                for (const char32_t codepoint : strUtf32)
                {
                    if ((codepoint < 0xD800) || ((codepoint > 0xDFFF) && (codepoint <= 0xFFFF)))
                        outStr.push_back(static_cast<wchar_t>(codepoint));
                }
            }

            return outStr;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Convert an UTF-32 string to UTF-16
        /// @param strUtf32  Input UTF-32 string
        /// @return Output UTF-16 string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD inline std::u16string convertUtf32toUtf16(const std::u32string& strUtf32)
        {
            std::u16string outStrUtf16;
            outStrUtf16.reserve(strUtf32.length() + 1);

            for (const char32_t codepoint : strUtf32)
            {
                // If the codepoint fitst inside 2 bytes and it would represent a valid character then just copy it
                if (codepoint <= 0xFFFF)
                {
                    if ((codepoint < 0xD800) || (codepoint > 0xDFFF))
                        outStrUtf16.push_back(static_cast<char16_t>(codepoint));

                    continue;
                }
                else if (codepoint > 0x0010FFFF)
                    continue; // Invalid character (greater than the maximum Unicode value)

                // The input character needs be converted to two UTF-16 elements
                outStrUtf16.push_back(static_cast<char16_t>(((codepoint - 0x0010000) >> 10)     + 0xD800));
                outStrUtf16.push_back(static_cast<char16_t>(((codepoint - 0x0010000) & 0x3FFUL) + 0xDC00));
            }

            return outStrUtf16;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_UTF_HPP
