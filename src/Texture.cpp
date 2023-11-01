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


#include <TGUI/Texture.hpp>
#include <TGUI/Global.hpp>
#include <TGUI/Base64.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/TextureManager.hpp>
#include <TGUI/Backend/Renderer/BackendTexture.hpp>
#include <TGUI/Loading/ImageLoader.hpp>

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
    #include <SFML/Graphics/Image.hpp>
    #include <SFML/Graphics/Texture.hpp>
    #include <SFML/Graphics/Shader.hpp>
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <memory>
    #include <cstdint>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    bool Texture::m_defaultSmooth = true;

    Texture::TextureLoaderFunc Texture::m_textureLoader = &TextureManager::getTexture;
    Texture::BackendTextureLoaderFunc Texture::m_backendTextureLoader = [](BackendTexture& backendTexture, const String& filename, bool smooth)
        {
            Vector2u imageSize;
            auto pixelPtr = ImageLoader::loadFromFile(filename, imageSize);
            if (!pixelPtr)
                return false;

            return backendTexture.load(imageSize, std::move(pixelPtr), smooth);
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const String& id, const UIntRect& partRect, const UIntRect& middlePart, bool smooth)
    {
        load(id, partRect, middlePart, smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !defined(TGUI_REMOVE_DEPRECATED_CODE)
TGUI_IGNORE_DEPRECATED_WARNINGS_START
    Texture::Texture(const sf::Texture& texture, const UIntRect& partRect, const UIntRect& middlePart)
    {
        load(texture, partRect, middlePart);
    }
TGUI_IGNORE_DEPRECATED_WARNINGS_END
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const Texture& other) :
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
        m_shader          {other.m_shader},
#endif
        m_data            {other.m_data},
        m_color           {other.m_color},
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
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
        m_shader          {std::move(other.m_shader)},
#endif
        m_data            {std::move(other.m_data)},
        m_color           {std::move(other.m_color)},
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

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
            std::swap(m_shader,           temp.m_shader);
#endif
            std::swap(m_data,             temp.m_data);
            std::swap(m_color,            temp.m_color);
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
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
            m_shader           = std::move(other.m_shader);
#endif
            m_data             = std::move(other.m_data);
            m_color            = std::move(other.m_color);
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
            data = m_textureLoader(*this, (getResourcePath() / id).asString(), smooth);
            if (!data)
                throw Exception{U"Failed to load '" + (getResourcePath() / id).asString() + U"'"};
        }
        else
        {
            data = m_textureLoader(*this, id, smooth);
            if (!data)
                throw Exception{U"Failed to load '" + id + U"'"};
        }

        TGUI_ASSERT(data->svgImage || data->backendTexture, "TextureLoaderFunc returned non-nullptr but didn't initialized backendTexture or svgImage");

        m_id = id;
        setTextureData(data, partRect, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !defined(TGUI_REMOVE_DEPRECATED_CODE)
    void Texture::load(const sf::Texture& texture, const UIntRect& partRect, const UIntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
        {
            m_destructCallback(getData());
            m_destructCallback = nullptr;
        }

        m_data = nullptr;
        loadFromPixelData(texture.getSize(), texture.copyToImage().getPixelsPtr(), partRect, middleRect);
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::loadFromMemory(const std::uint8_t* fileData, std::size_t fileDataSize, const UIntRect& partRect, const UIntRect& middleRect, bool smooth)
    {
        auto data = std::make_shared<TextureData>();
        data->backendTexture = getBackend()->createTexture();

        Vector2u imageSize;
        auto pixelPtr = ImageLoader::loadFromMemory(fileData, fileDataSize, imageSize);
        if (!pixelPtr)
            throw Exception{U"Failed to load texture from provided memory location (" + String(fileDataSize) + U" bytes)"};

        if (!data->backendTexture->load(imageSize, std::move(pixelPtr), smooth))
            throw Exception{U"Failed to load texture from pixels that were loaded from file in memory"};

        m_id = "";
        setTextureData(data, partRect, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::loadFromPixelData(Vector2u size, const std::uint8_t* pixels, const UIntRect& partRect, const UIntRect& middleRect, bool smooth)
    {
        auto data = std::make_shared<TextureData>();
        data->backendTexture = getBackend()->createTexture();

        auto pixelPtr = MakeUniqueForOverwrite<std::uint8_t[]>(size.x * size.y * 4);
        std::memcpy(pixelPtr.get(), pixels, size.x * size.y * 4);

        if (!data->backendTexture->load(size, std::move(pixelPtr), smooth))
            throw Exception{U"Failed to load texture from provided pixel data"};

        m_id = "";
        setTextureData(data, partRect, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::loadFromBase64(CharStringView imageAsBase64, const UIntRect& partRect, const UIntRect& middleRect, bool smooth)
    {
        const std::vector<std::uint8_t> fileData = base64Decode(imageAsBase64);
        loadFromMemory(fileData.data(), fileData.size(), partRect, middleRect, smooth);
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
        if (m_data && m_data->backendTexture)
            return m_data->backendTexture->isSmooth();
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
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
    void Texture::setShader(sf::Shader* shader)
    {
        m_shader = shader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Shader* Texture::getShader() const
    {
        return m_shader;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setMiddleRect(const UIntRect& middleRect)
    {
        if (middleRect == UIntRect{})
            m_middleRect = {0, 0, m_partRect.width, m_partRect.height};
        else
        {
            m_middleRect = middleRect;

            // If the middle rect was only partially provided then we need to calculate the width and height ourselves
            if (((middleRect.left > 0) || (middleRect.top > 0)) && (middleRect.width == 0) && (middleRect.height == 0))
            {
                if (m_partRect.width > 2 * middleRect.left)
                    m_middleRect.width = m_partRect.width - (2 * middleRect.left);

                if (m_partRect.height > 2 * middleRect.top)
                    m_middleRect.height = m_partRect.height - (2 * middleRect.top);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    UIntRect Texture::getMiddleRect() const
    {
        return m_middleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(Vector2u pixel) const
    {
        if (!m_data || !m_data->backendTexture)
            return false;

        const UIntRect& partRect = getPartRect();
        TGUI_ASSERT(pixel.x < partRect.width && pixel.y < partRect.height, "Texture::isTransparentPixel called with pixel outside texture rectangle");

        return m_data->backendTexture->isTransparentPixel({pixel.x + partRect.left, pixel.y + partRect.top});
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
#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
            && (m_shader == right.m_shader)
#endif
            && (m_color == right.m_color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator!=(const Texture& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setDefaultSmooth(bool smooth)
    {
        m_defaultSmooth = smooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::getDefaultSmooth()
    {
        return m_defaultSmooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setBackendTextureLoader(const BackendTextureLoaderFunc& func)
    {
        TGUI_ASSERT(func != nullptr, "Texture::setBackendTextureLoader called with nullptr");
        m_backendTextureLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::BackendTextureLoaderFunc& Texture::getBackendTextureLoader()
    {
        return m_backendTextureLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureLoader(const TextureLoaderFunc& func)
    {
        TGUI_ASSERT(func != nullptr, "Texture::setTextureLoader called with nullptr");
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

        m_data = std::move(data);

        if (partRect == UIntRect{})
        {
            if (m_data->svgImage)
                m_partRect = {0, 0, static_cast<unsigned int>(m_data->svgImage->getSize().x), static_cast<unsigned int>(m_data->svgImage->getSize().y)};
            else
            {
                const Vector2u textureSize = m_data->backendTexture->getSize();
                m_partRect = {0, 0, textureSize.x, textureSize.y};
            }
        }
        else
            m_partRect = partRect;

        setMiddleRect(middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
