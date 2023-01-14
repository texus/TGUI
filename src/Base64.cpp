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

// Encode and decode implementations are based on https://stackoverflow.com/a/34571089

#include <TGUI/Base64.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* base64CharactersTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// The following table maps each character from base64CharactersTable to its index in the table.
// All characters that don't appear in base64CharactersTable are mapped to 255. Valid indices are in range [0,63]
static const std::uint8_t base64InverseCharactersTable[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [  0 -  15]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [ 16 -  31]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  // [ 32 -  47]
     52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,  // [ 48 -  63]
    255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  // [ 64 -  79]
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  // [ 80 -  95]
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  // [ 96 - 111]
     41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,  // [112 - 128]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [128 - 143]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [144 - 159]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [160 - 175]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [176 - 191]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [192 - 207]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [208 - 223]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  // [224 - 239]
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}; // [240 - 255]

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string base64Encode(const std::uint8_t* data, std::size_t nrBytes)
    {
        std::string result;

        const std::size_t nrBytesWithPadding = ((nrBytes + 2) / 3) * 3;
        result.reserve((nrBytesWithPadding * 8) / 6);

        unsigned int val = 0;
        int valShift = -6;
        for (std::size_t i = 0; i < nrBytes; ++i)
        {
            val = (val << 8) + data[i];
            valShift += 8;
            while (valShift >= 0)
            {
                result.push_back(base64CharactersTable[(val >> valShift) & 0b00111111]);
                valShift -= 6;
            }
        }

        if (valShift > -6)
            result.push_back(base64CharactersTable[((val << 8) >> (valShift + 8)) & 0b00111111]);

        // Add padding bytes if needed
        while (result.size() % 4)
            result.push_back('=');

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::uint8_t> base64Decode(CharStringView data)
    {
        // Reserve space for the result. This might reserve up to 2 bytes more than needed if data is padded.
        std::vector<std::uint8_t> result;
        result.reserve(data.length() * 6 / 8);

        unsigned int val = 0;
        int valShift = -8;
        for (char c : data)
        {
            const std::uint8_t tableIndex = base64InverseCharactersTable[static_cast<std::uint8_t>(c)]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)

            // Stop the decoding when an invalid character is encountered.
            // Since we don't check for it explicitly, this case will also happen when we reach a padding byte at the end.
            if (tableIndex == 255)
                break;

            val = (val << 6) + tableIndex;
            valShift += 6;
            if (valShift >= 0)
            {
                result.push_back(static_cast<std::uint8_t>((val >> valShift) & 0xFF));
                valShift -= 8;
            }
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
