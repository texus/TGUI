/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Renderer/SFML-Graphics/BackendTextureSFML.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureSFML::BackendTextureSFML()
    {
        m_isSmooth = false; // TGUI_NEXT: This is only for backwards compatibility, this line can be removed in the future
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSFML::loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth)
    {
        BackendTexture::loadTextureOnly(size, pixels, smooth);

        if (!m_texture || (Vector2u{m_texture->getSize()} != size))
        {
            if (!m_texture)
                m_texture = std::make_unique<sf::Texture>();
#if SFML_VERSION_MAJOR >= 3
            if (!m_texture->resize({size.x, size.y}))
                return false;
#else
            if (!m_texture->create(size.x, size.y))
                return false;
#endif
        }

        if (m_texture)
        {
            m_texture->setSmooth(smooth);
            if (pixels)
                m_texture->update(pixels);
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSFML::setSmooth(bool smooth)
    {
        BackendTexture::setSmooth(smooth);
        if (m_texture)
            m_texture->setSmooth(smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Texture* BackendTextureSFML::getInternalTexture()
    {
        return m_texture.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Texture* BackendTextureSFML::getInternalTexture() const
    {
        return m_texture.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSFML::replaceInternalTexture(const sf::Texture& texture)
    {
        m_texture = std::make_unique<sf::Texture>(texture);

        m_pixels = nullptr;
        m_imageSize = {texture.getSize().x, texture.getSize().y};
        m_isSmooth = texture.isSmooth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
