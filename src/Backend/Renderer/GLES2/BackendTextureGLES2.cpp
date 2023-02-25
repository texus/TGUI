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


#include <TGUI/Backend/Renderer/GLES2/BackendTextureGLES2.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureGLES2::~BackendTextureGLES2()
    {
        if (m_textureId != 0)
            TGUI_GL_CHECK(glDeleteTextures(1, &m_textureId));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureGLES2::loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth)
    {
        TGUI_ASSERT(isBackendSet(), "BackendTextureGLES2 can't be created when there is no system backend initialized (was a gui created yet?)");
        TGUI_ASSERT(getBackend()->getRenderer(), "BackendTextureGLES2 can't be created when there is no backend renderer (was a gui attached to a window yet?)");

        const bool reuseTexture = ((m_textureId != 0) && (size.x == m_imageSize.x) && (size.y == m_imageSize.y) && (smooth == m_isSmooth));

        BackendTexture::loadTextureOnly(size, pixels, smooth);

        if (!reuseTexture)
        {
            if (m_textureId != 0)
                TGUI_GL_CHECK(glDeleteTextures(1, &m_textureId));

            TGUI_GL_CHECK(glGenTextures(1, &m_textureId));
        }

        GLint oldBoundTexture;
        TGUI_GL_CHECK(glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBoundTexture));

        TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));

        if (reuseTexture)
        {
            if (pixels)
                TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), GL_RGBA, GL_UNSIGNED_BYTE, pixels));
        }
        else
        {
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST));
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST));

            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

            if (TGUI_GLAD_GL_ES_VERSION_3_0)
            {
                TGUI_GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y)));
                if (pixels)
                    TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), GL_RGBA, GL_UNSIGNED_BYTE, pixels));
            }
            else
            {
                // GLES 2.0 doesn't support GL_RGBA8
                TGUI_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
            }
        }

        // Restore the texture that was bound when this function was called
        TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(oldBoundTexture)));
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureGLES2::setSmooth(bool smooth)
    {
        if (m_isSmooth == smooth)
            return;

        BackendTexture::setSmooth(smooth);

        if (!m_textureId)
            return;

        GLint oldBoundTexture;
        TGUI_GL_CHECK(glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBoundTexture));

        TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));
        TGUI_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_isSmooth ? GL_LINEAR : GL_NEAREST));

        // Restore the texture that was bound when this function was called
        TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(oldBoundTexture)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendTextureGLES2::getInternalTexture() const
    {
        return m_textureId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
