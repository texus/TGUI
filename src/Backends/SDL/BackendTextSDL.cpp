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


#include <TGUI/Backends/SDL/BackendTextSDL.hpp>
#include <TGUI/Backends/SDL/BackendFontSDL.hpp>
#include <TGUI/Backends/SDL/FontCacheSDL.hpp>
#include <TGUI/Backends/SDL/BackendSDL.hpp>
#include <TGUI/OpenGL.hpp>

#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    BackendTextSDL::BackendTextSDL()
    {
        // There must always be a single line of text, even when the text is empty
        m_lines.emplace_back();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextSDL::~BackendTextSDL()
    {
        for (const auto& lineTexture : m_textures)
            TGUI_GL_CHECK(glDeleteTextures(1, &lineTexture.textureId));
        for (const auto& lineTexture : m_outlineTextures)
            TGUI_GL_CHECK(glDeleteTextures(1, &lineTexture.textureId));

        if (m_font)
            FontCacheSDL::unregisterFontSize(m_font.get(), m_characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackendTextSDL::getSize()
    {
        if (!m_texturesValid)
        {
            if (!updateTextures())
                return {0, 0};
        }

        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setString(const String& string)
    {
        m_lines = string.split(U'\n');

        // Replace tab characters with 4 spaces, because SDL_ttf won't render tabs
        for (auto& line : m_lines)
            line.replace(U'\t', U"    ");

        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setCharacterSize(unsigned int characterSize)
    {
        if (m_characterSize == characterSize)
            return;

        if (m_font)
        {
            FontCacheSDL::unregisterFontSize(m_font.get(), m_characterSize);
            FontCacheSDL::registerFontSize(m_font.get(), characterSize);
        }

        m_characterSize = characterSize;
        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setFillColor(const Color& color)
    {
        m_textColor.r = color.getRed();
        m_textColor.g = color.getGreen();
        m_textColor.b = color.getBlue();
        m_textColor.a = color.getAlpha();
        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setOutlineColor(const Color& color)
    {
        m_outlineColor.r = color.getRed();
        m_outlineColor.g = color.getGreen();
        m_outlineColor.b = color.getBlue();
        m_outlineColor.a = color.getAlpha();
        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setOutlineThickness(float thickness)
    {
        m_fontOutline = static_cast<int>(thickness);
        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setStyle(TextStyles style)
    {
        m_fontStyle = static_cast<int>(static_cast<unsigned int>(style));
        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSDL::setFont(const Font& font)
    {
        // If there was already another font then unregister from it
        if (m_font)
            FontCacheSDL::unregisterFontSize(m_font.get(), m_characterSize);

        if (font)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendFontSDL>(font.getBackendFont()), "BackendTextSDL::setFont requires font of type BackendFontSDL");
            m_font = std::static_pointer_cast<BackendFontSDL>(font.getBackendFont());

            // Register the text to the new font
            FontCacheSDL::registerFontSize(m_font.get(), m_characterSize);
        }
        else
            m_font = nullptr;

        m_texturesValid = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackendTextSDL::findCharacterPos(std::size_t index) const
    {
        if (!m_font)
            return {0, 0};

        TGUI_ASSERT(!m_lines.empty(), "BackendTextSDL::m_lines can never be empty");

        std::size_t lineNumber = 0;
        while (index > m_lines[lineNumber].length())
        {
            index -= m_lines[lineNumber].length() + 1;
            ++lineNumber;
            TGUI_ASSERT(lineNumber < m_lines.size(), "Index out-of-range in BackendTextSDL::findCharacterPos");
        }

        const bool bold = ((m_fontStyle & TTF_STYLE_BOLD) == TTF_STYLE_BOLD);

        float x = 0;
        char32_t prevChar = U'\0';
        for (std::size_t i = 0; i < index; ++i)
        {
            const char32_t currentChar = m_lines[lineNumber][i];
            x += m_font->getGlyph(currentChar, m_characterSize, bold, static_cast<float>(m_fontOutline)).advance;
            x += m_font->getKerning(prevChar, currentChar, m_characterSize, bold);
            prevChar = currentChar;
        }

        if (lineNumber == 0)
            return {x, 0};
        else
            return {x, static_cast<float>(lineNumber * m_font->getLineSpacing(m_characterSize))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<BackendTextSDL::LineTexture>& BackendTextSDL::getInternalTextures()
    {
        if (!m_texturesValid)
            updateTextures();

        return m_textures;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<BackendTextSDL::LineTexture>& BackendTextSDL::getInternalOutlineTextures()
    {
        if (!m_texturesValid)
            updateTextures();

        return m_outlineTextures;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextSDL::updateTextures()
    {
        if (!m_font)
            return false;

        TTF_Font* font = m_font->getInternalFont(m_characterSize);
        if (!font)
            return false;

        const float lineSpacing = m_font->getLineSpacing(m_characterSize);

        // If textures already existed then delete them first
        for (const auto& lineTexture : m_textures)
            TGUI_GL_CHECK(glDeleteTextures(1, &lineTexture.textureId));
        for (const auto& lineTexture : m_outlineTextures)
            TGUI_GL_CHECK(glDeleteTextures(1, &lineTexture.textureId));

        m_textures.clear();
        m_outlineTextures.clear();

        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(Font{m_font, ""}, m_characterSize, m_fontStyle);

        m_size.x = 0;
        m_size.y = m_lines.size() * lineSpacing + extraVerticalSpace;

        if (m_fontStyle != TextStyle::Regular)
            TTF_SetFontStyle(font, m_fontStyle);

        std::vector<LineTexture> textures;
        for (std::size_t i = 0; i < m_lines.size(); ++i)
            textures.push_back(createLineTexture(font, static_cast<int>(i * lineSpacing), m_lines[i].toStdString(), m_textColor, 0));

        // If an outline exists then create textures for it as well
        std::vector<LineTexture> outlineTextures;
        if (m_fontOutline != 0)
        {
            TTF_SetFontOutline(font, m_fontOutline);

            for (std::size_t i = 0; i < m_lines.size(); ++i)
                outlineTextures.push_back(createLineTexture(font, static_cast<int>(i * lineSpacing), m_lines[i].toStdString(), m_outlineColor, m_fontOutline));

            // Reset the outline again so that we can always assume that it is set to 0 everywhere
            TTF_SetFontOutline(font, 0);
        }

        // Restore the font style so that we can always assume that a font from the cache has no special style
        if (m_fontStyle != TextStyle::Regular)
            TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

        // Set the new texture
        m_textures = std::move(textures);
        m_outlineTextures = std::move(outlineTextures);
        m_texturesValid = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextSDL::LineTexture BackendTextSDL::createLineTexture(TTF_Font* font, int verticalOffset, std::string line, const SDL_Color& color, int outline)
    {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); // We shouldn't render a square if the line contains a '\r'

        LineTexture lineTexture;
        if (line.empty())
        {
            lineTexture.bounding = SDL_Rect{0, 0, 0, 0};
            lineTexture.textureId = 0;
            return lineTexture;
        }

        // For OpenGL ES we must convert the surface to RGBA.
        // This isn't needed for OpenGL as long as we specify the correct image format and type,
        // but text rendering broke at some point and we now perform the conversion anyway so that
        // we know for certain what the exact surface format is.
        const GLenum imageFormat = GL_RGBA;
        SDL_Surface* surface = nullptr;
        SDL_Surface* surfaceBGRA = TTF_RenderUTF8_Blended(font, line.c_str(), color);
        if (surfaceBGRA)
        {
            surface = SDL_ConvertSurfaceFormat(surfaceBGRA, SDL_PIXELFORMAT_RGBA32, 0);
            SDL_FreeSurface(surfaceBGRA);
        }

        if (!surface)
        {
            lineTexture.bounding = SDL_Rect{0, 0, 0, 0};
            lineTexture.textureId = 0;
            return lineTexture;
        }

        const SDL_Rect lineBounding{-outline, verticalOffset - outline, surface->w, surface->h};

        m_size.x = std::max(m_size.x, static_cast<float>(surface->w));

        GLuint textureId;
        TGUI_GL_CHECK(glGenTextures(1, &textureId));

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendSDL>(getBackend()), "BackendTextSDL::createLineTexture requires backend texture of type BackendSDL");
        std::static_pointer_cast<BackendSDL>(getBackend())->changeTexture(textureId, true);

        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

#if TGUI_USE_GLES
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
#else
        if (TGUI_GLAD_GL_VERSION_4_2)
#endif
        {
            // GL 4.2 and GLES 3.0 support using glTexStorage2D instead of glTexImage2D
            TGUI_GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, surface->w, surface->h));
            TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, imageFormat, GL_UNSIGNED_BYTE, surface->pixels));
        }
#if TGUI_USE_GLES
        else if (!TGUI_GLAD_GL_ES_VERSION_3_0)
        {
            // GLES 2.0 doesn't support GL_RGBA8
            TGUI_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, imageFormat, GL_UNSIGNED_BYTE, surface->pixels));
        }
#endif
        else
        {
            // Use glTexImage2D with GL_RGBA8, which is supported by GL 3.1 core and GLES 3.0
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
            TGUI_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, imageFormat, GL_UNSIGNED_BYTE, surface->pixels));
        }

        SDL_FreeSurface(surface);

        lineTexture.bounding = lineBounding;
        lineTexture.textureId = textureId;
        return lineTexture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
