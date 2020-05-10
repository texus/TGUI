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


#include <TGUI/Global.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/TextureManager.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Texture::TextureLoaderFunc Texture::m_textureLoader = &TextureManager::getTexture;
    Texture::ImageLoaderFunc Texture::m_imageLoader = [](const String& filename) -> std::shared_ptr<sf::Image>
        {
            auto image = std::make_shared<sf::Image>();
            if (image->loadFromFile(filename.toAnsiString()))
                return image;
            else
                return nullptr;
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const String& id, const UIntRect& partRect, const UIntRect& middlePart, bool smooth)
    {
        load(id, partRect, middlePart, smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::Texture& texture, const UIntRect& partRect, const UIntRect& middlePart)
    {
        load(texture, partRect, middlePart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const Texture& other) :
        m_data            {other.m_data},
        m_color           {other.m_color},
        m_shader          {other.m_shader},
        m_partRect        {other.m_partRect},
        m_middleRect      {other.m_middleRect},
        m_id              {other.m_id},
        m_copyCallback    {other.m_copyCallback},
        m_destructCallback{other.m_destructCallback}
    {
        if (getData() && (m_copyCallback != nullptr))
            m_copyCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(Texture&& other) noexcept :
        m_data            {std::move(other.m_data)},
        m_color           {std::move(other.m_color)},
        m_shader          {std::move(other.m_shader)},
        m_partRect        {std::move(other.m_partRect)},
        m_middleRect      {std::move(other.m_middleRect)},
        m_id              {std::move(other.m_id)},
        m_copyCallback    {std::move(other.m_copyCallback)},
        m_destructCallback{std::move(other.m_destructCallback)}
    {
        other.m_data = nullptr;
        other.m_copyCallback = nullptr;
        other.m_destructCallback = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::~Texture()
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(const Texture& other)
    {
        if (this != &other)
        {
            Texture temp{other};

            std::swap(m_data,             temp.m_data);
            std::swap(m_color,            temp.m_color);
            std::swap(m_shader,           temp.m_shader);
            std::swap(m_partRect,         temp.m_partRect);
            std::swap(m_middleRect,       temp.m_middleRect);
            std::swap(m_id,               temp.m_id);
            std::swap(m_copyCallback,     temp.m_copyCallback);
            std::swap(m_destructCallback, temp.m_destructCallback);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            m_data             = std::move(other.m_data);
            m_color            = std::move(other.m_color);
            m_shader           = std::move(other.m_shader);
            m_partRect         = std::move(other.m_partRect);
            m_middleRect       = std::move(other.m_middleRect);
            m_id               = std::move(other.m_id);
            m_copyCallback     = std::move(other.m_copyCallback);
            m_destructCallback = std::move(other.m_destructCallback);

            other.m_data = nullptr;
            other.m_copyCallback = nullptr;
            other.m_destructCallback = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const String& id, const UIntRect& partRect, const UIntRect& middleRect, bool smooth)
    {
        if (id.empty())
        {
            *this = Texture{};
            return;
        }

        if (getData() && (m_destructCallback != nullptr))
        {
            m_destructCallback(getData());
            m_destructCallback = nullptr;
        }

        m_data = nullptr;

        std::shared_ptr<TextureData> data;
#ifdef TGUI_SYSTEM_WINDOWS
        if ((id[0] != '/') && (id[0] != '\\') && ((id.length() <= 1) || (id[1] != ':')))
#else
        if (id[0] != '/')
#endif
        {
            data = m_textureLoader(*this, getResourcePath() + id, partRect, smooth);
            if (!data)
                throw Exception{"Failed to load '" + getResourcePath() + id + "'"};
        }
        else
        {
            data = m_textureLoader(*this, id, partRect, smooth);
            if (!data)
                throw Exception{"Failed to load '" + id + "'"};
        }

        assert(data->svgImage || data->texture);

        m_id = id;
        setTextureData(data, partRect, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::Texture& texture, const UIntRect& partRect, const UIntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
        {
            m_destructCallback(getData());
            m_destructCallback = nullptr;
        }

        m_data = nullptr;
        auto data = std::make_shared<TextureData>();
        data->texture = texture; /// TODO: Allow reusing data based on address of input texture

        m_id = "";
        setTextureData(data, partRect, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& Texture::getId() const
    {
        return m_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData> Texture::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Texture::getImageSize() const
    {
        if (!m_data)
            return {0, 0};

        if (m_data->svgImage)
            return Vector2u{m_data->svgImage->getSize()};
        else
            return getPartRect().getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    UIntRect Texture::getPartRect() const
    {
        return m_partRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isSmooth() const
    {
        if (m_data && m_data->texture)
            return m_data->texture->isSmooth();
        else
            return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setColor(const Color& color)
    {
        m_color = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Texture::getColor() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setShader(sf::Shader* shader)
    {
        m_shader = shader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Shader* Texture::getShader() const
    {
        return m_shader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    UIntRect Texture::getMiddleRect() const
    {
        return m_middleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(Vector2u pixel) const
    {
        if (!m_data || !m_data->image)
            return false;

        const UIntRect& partRect = getPartRect();
        assert(pixel.x < partRect.width && pixel.y < partRect.height);

        if (m_data->image->getPixel(pixel.x + partRect.left, pixel.y + partRect.top).a == 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setCopyCallback(const CallbackFunc& func)
    {
        m_copyCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setDestructCallback(const CallbackFunc& func)
    {
        m_destructCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator==(const Texture& right) const
    {
        return (m_id == right.m_id)
            && (!m_id.empty() || (m_data == right.m_data))
            && (m_middleRect == right.m_middleRect)
            && (m_shader == right.m_shader)
            && (m_color == right.m_color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator!=(const Texture& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setImageLoader(const ImageLoaderFunc& func)
    {
        assert(func != nullptr);
        m_imageLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::ImageLoaderFunc& Texture::getImageLoader()
    {
        return m_imageLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureLoader(const TextureLoaderFunc& func)
    {
        assert(func != nullptr);
        m_textureLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::TextureLoaderFunc& Texture::getTextureLoader()
    {
        return m_textureLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureData(std::shared_ptr<TextureData> data, const UIntRect& partRect, const UIntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
        {
            m_destructCallback(getData());
            m_destructCallback = nullptr;
        }

        m_data = data;

        if (partRect == UIntRect{})
        {
            if (m_data->svgImage)
                m_partRect = {0, 0, static_cast<unsigned int>(m_data->svgImage->getSize().x), static_cast<unsigned int>(m_data->svgImage->getSize().y)};
            else
                m_partRect = {0, 0, data->texture->getSize().x, data->texture->getSize().y};
        }
        else
            m_partRect = partRect;

        if (middleRect == UIntRect{})
            m_middleRect = {0, 0, m_partRect.width, m_partRect.height};
        else
        {
            m_middleRect = middleRect;

            // If the middle rect was only partially provided then we need to calculate the width and height ourselves
            if ((middleRect.left > 0) && (middleRect.top > 0) && (middleRect.width == 0) && (middleRect.height == 0))
            {
                if (m_partRect.width > 2 * middleRect.left)
                    m_middleRect.width = m_partRect.width - (2 * middleRect.left);

                if (m_partRect.width > 2 * middleRect.left)
                    m_middleRect.height = m_partRect.height - (2 * middleRect.top);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
