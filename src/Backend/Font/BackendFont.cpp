/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Backend/Font/BackendFont.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFont::loadFromFile(const String& filename)
    {
        std::size_t fileSize;
        const auto fileContents = readFileToMemory(filename, fileSize);
        if (!fileContents)
            throw Exception{"Failed to load '" + filename + "'"};

        return loadFromMemory(fileContents.get(), fileSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFont::loadFromMemory(const void* data, std::size_t sizeInBytes)
    {
        auto copiedData = MakeUniqueForOverwrite<std::uint8_t[]>(sizeInBytes);
        std::memcpy(copiedData.get(), data, sizeInBytes);
        return loadFromMemory(std::move(copiedData), sizeInBytes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint64_t BackendFont::constructGlyphKey(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        // Create a unique key for every character.
        // Technically it would be possible to specify character sizes and outline thicknesses that can't uniquely be mapped
        // in the limited amount of bits, but this would require unrealistic sizes and would still unlikely cause a conflic
        // with another existing key.
        return (static_cast<std::uint64_t>(bold) << 63) // bit 64 = bold flag
             | (static_cast<std::uint64_t>(outlineThickness < 0) << 62) // bit 63 = sign of outline
             | (static_cast<std::uint64_t>(std::abs(outlineThickness) * 100) << 45) // bits 46-62 = outline
             | (static_cast<std::uint64_t>(characterSize) << 32) // bits 33-45 = character size
             | codePoint; // bits 1-32 = unicode codepoint
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFont::setSmooth(bool smooth)
    {
        m_isSmooth = smooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFont::isSmooth() const
    {
        return m_isSmooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
