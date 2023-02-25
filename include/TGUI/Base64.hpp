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


#ifndef TGUI_BASE64_HPP
#define TGUI_BASE64_HPP

#include <TGUI/String.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
    #include <vector>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Encodes binary data to a base64 string
    ///
    /// @param data    Pointer to the start of the binary data
    /// @param nrBytes Length of the binary data in bytes
    ///
    /// @return String with base64-encoded data. The string is padded with '=' at the end if needed, and contains no prefix.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD TGUI_API std::string base64Encode(const std::uint8_t* data, std::size_t nrBytes);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Decodes a base64 string to binary data
    ///
    /// @param data  View on the base64 string
    ///
    /// @return Binary data contained in the base64-encoded string
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TGUI_NODISCARD TGUI_API std::vector<std::uint8_t> base64Decode(CharStringView data);

} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BASE64_HPP
