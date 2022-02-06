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


#include <TGUI/Backends/SDL/BackendTextureSDL.hpp>
#include <TGUI/Backends/SDL/BackendSDL.hpp>
#include <TGUI/Loading/ImageLoader.hpp>
#include <TGUI/OpenGL.hpp>

#include <SDL.h>

#include <cstring> // memcpy

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureSDL::~BackendTextureSDL()
    {
        releaseResources();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDL::load(Vector2u size, std::unique_ptr<std::uint8_t[]> pixels)
    {
        TGUI_ASSERT(pixels.get() && (size.x > 0) && (size.y > 0), "BackendTextureSDL::loadFromPixelDataImpl needs valid parameters");

        releaseResources(); // Delete existing texture if one was previously loaded

        TGUI_GL_CHECK(glGenTextures(1, &m_textureId));

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendSDL>(getBackend()), "BackendTextureSDL requires backend texture of type BackendSDL");
        std::static_pointer_cast<BackendSDL>(getBackend())->changeTexture(m_textureId, true);

        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

#if TGUI_USE_GLES
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
#else
        if (TGUI_GLAD_GL_VERSION_4_2)
#endif
        {
            // GL 4.2 and GLES 3.0 support using glTexStorage2D instead of glTexImage2D
            TGUI_GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)));
            TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), GL_RGBA, GL_UNSIGNED_BYTE, pixels.get()));
        }
#if TGUI_USE_GLES
        else if (!TGUI_GLAD_GL_ES_VERSION_3_0)
        {
            // GLES 2.0 doesn't support GL_RGBA8
            TGUI_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get()));
        }
#endif
        else
        {
            // Use glTexImage2D with GL_RGBA8, which is supported by GL 3.1 core and GLES 3.0
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
            TGUI_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get()));
        }

        return BackendTextureBase::load(size, std::move(pixels));
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
