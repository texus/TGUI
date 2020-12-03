/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Backends/SDL/BackendTextureSDL.hpp>
#include <TGUI/Backends/SDL/BackendSDL.hpp>
#include <TGUI/OpenGL.hpp>

#include <SDL.h>
#include <SDL_image.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureSDL::~BackendTextureSDL()
    {
        releaseResources();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDL::loadFromFile(const String& filename)
    {
        SDL_Surface* image = IMG_Load(filename.toAnsiString().c_str());
        if (!image)
            return false;

        GLenum textureFormat;
        GLenum imageFormat;
        const Uint8 bpp = image->format->BytesPerPixel * 8;
        if (bpp == 32)
        {
            textureFormat = GL_RGBA8;
            if (image->format->Rmask == 0x000000ff)
                imageFormat = GL_RGBA;
            else
            {
#if TGUI_USE_GLES
                imageFormat = GL_RGBA;
                SDL_Surface* surfaceBGRA = image;
                image = SDL_ConvertSurfaceFormat(surfaceBGRA, SDL_PIXELFORMAT_RGBA32, 0);
                SDL_FreeSurface(surfaceBGRA);
                if (!image)
                    return false;
#else
                imageFormat = GL_BGRA;
#endif
            }
        }
        else if (bpp == 24)
        {
            textureFormat = GL_RGB8;
            if (image->format->Rmask == 0x000000ff)
                imageFormat = GL_RGB;
            else
            {
#if TGUI_USE_GLES
                imageFormat = GL_RGB;
                SDL_Surface* surfaceBGR = image;
                image = SDL_ConvertSurfaceFormat(surfaceBGR, SDL_PIXELFORMAT_RGB24, 0);
                SDL_FreeSurface(surfaceBGR);
                if (!image)
                    return false;
#else
                imageFormat = GL_BGR;
#endif
            }
        }
        else
        {
            TGUI_PRINT_WARNING("Failed to load image '" + filename + "'. Only 24bpp and 32bpp images are currently supported in BackendTextureSDL.");
            return false;
        }

        releaseResources(); // Delete existing image and texture if one was previously loaded

        TGUI_GL_CHECK(glGenTextures(1, &m_textureId));

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendSDL>(getBackend()), "BackendTextureSDL::loadFromFile requires backend texture of type BackendSDL");
        std::static_pointer_cast<BackendSDL>(getBackend())->changeTexture(m_textureId, true);

        TGUI_GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, textureFormat, image->w, image->h));
        TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->w, image->h, imageFormat, GL_UNSIGNED_BYTE, image->pixels));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));

        m_image = image;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDL::loadFromPixelData(Vector2u size, const std::uint8_t* pixels)
    {
        SDL_Surface* image = SDL_CreateRGBSurfaceFrom(static_cast<void*>(const_cast<std::uint8_t*>(pixels)),
            static_cast<int>(size.x), static_cast<int>(size.y), 32, 4 * static_cast<int>(size.x),
            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        if (!image)
            return false;

        releaseResources(); // Delete existing image and texture if one was previously loaded

        TGUI_GL_CHECK(glGenTextures(1, &m_textureId));

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendSDL>(getBackend()), "BackendTextureSDL::loadFromPixelData requires backend texture of type BackendSDL");
        std::static_pointer_cast<BackendSDL>(getBackend())->changeTexture(m_textureId, true);

        TGUI_GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image->w, image->h));
        TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->w, image->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));

        m_image = image;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u BackendTextureSDL::getSize() const
    {
        if (m_image)
            return {static_cast<unsigned int>(m_image->w), static_cast<unsigned int>(m_image->h)};
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSDL::setSmooth(bool smooth)
    {
        if (m_isSmooth == smooth)
            return;

        m_isSmooth = smooth;

        if (!m_textureId)
            return;

        std::static_pointer_cast<BackendSDL>(getBackend())->changeTexture(m_textureId, false);

        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDL::isSmooth() const
    {
        return m_isSmooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDL::isTransparentPixel(Vector2u pixel) const
    {
        if (!m_image)
            return false;
        if (m_image->format->BytesPerPixel != 4)
            return false;

        const auto offset = pixel.y * m_image->pitch + pixel.x * 4;
        const std::uint32_t pixelValue = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(m_image->pixels) + offset);

        SDL_Color color;
        SDL_GetRGBA(pixelValue, m_image->format, &color.r, &color.g, &color.b, &color.a);
        return (color.a == 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GLuint BackendTextureSDL::getInternalTexture() const
    {
        return m_textureId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSDL::releaseResources()
    {
        if (m_textureId != 0)
        {
            TGUI_GL_CHECK(glDeleteTextures(1, &m_textureId));
            m_textureId = 0;
        }

        if (m_image)
        {
            SDL_FreeSurface(m_image);
            m_image = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
