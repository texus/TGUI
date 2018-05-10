/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Font.hpp>
#include <TGUI/Loading/Deserializer.hpp>

#include <cassert>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(std::nullptr_t)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::string& id) :
        m_font(Deserializer::deserialize(ObjectConverter::Type::Font, id).getFont()), // Did not compile with clang 3.6 when using braces
        m_id  (Deserializer::deserialize(ObjectConverter::Type::String, id).getString()) // Did not compile with clang 3.6 when using braces
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const char* id) :
        Font(std::string{id})
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::shared_ptr<sf::Font>& font) :
        m_font{font}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const sf::Font& font) :
        m_font{std::make_shared<sf::Font>(font)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const void* data, std::size_t sizeInBytes) :
        m_font{std::make_shared<sf::Font>()}
    {
        m_font->loadFromMemory(data, sizeInBytes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font& Font::operator=(const Font& other)
    {
        if (this != &other)
        {
            // If this was the last instance that shares the global font then also destroy the global font
            if (m_font && (m_font == getInternalGlobalFont()) && (m_font.use_count() == 2))
                setGlobalFont(nullptr);

            m_font = other.m_font;
            m_id = other.m_id;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font& Font::operator=(Font&& other)
    {
        if (this != &other)
        {
            // If this was the last instance that shares the global font then also destroy the global font
            if (m_font && (m_font == getInternalGlobalFont()) && (m_font.use_count() == 2))
                setGlobalFont(nullptr);

            m_font = std::move(other.m_font);
            m_id = std::move(other.m_id);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::~Font()
    {
        // If this is the last instance that shares the global font then also destroy the global font
        if (m_font && (m_font == getInternalGlobalFont()) && (m_font.use_count() == 2))
            setGlobalFont(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Font::getId() const
    {
        return m_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> Font::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::operator std::shared_ptr<sf::Font>() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::operator bool() const
    {
        return m_font != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator==(std::nullptr_t) const
    {
        return m_font == nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator!=(std::nullptr_t) const
    {
        return m_font != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator==(const Font& right) const
    {
        return m_font == right.m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator!=(const Font& right) const
    {
        return m_font != right.m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Glyph& Font::getGlyph(std::uint32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness) const
    {
        assert(m_font != nullptr);

    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        return m_font->getGlyph(codePoint, characterSize, bold, outlineThickness);
    #else
        (void)outlineThickness;
        return m_font->getGlyph(codePoint, characterSize, bold);
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Font::getKerning(std::uint32_t first, std::uint32_t second, unsigned int characterSize) const
    {
        if (m_font)
            return m_font->getKerning(first, second, characterSize);
        else
            return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Font::getLineSpacing(unsigned int characterSize) const
    {
        if (m_font)
            return m_font->getLineSpacing(characterSize);
        else
            return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
