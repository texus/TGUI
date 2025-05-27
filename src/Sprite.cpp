/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Sprite.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Optional.hpp>
#include <TGUI/Backend/Window/Backend.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Sprite::Sprite(const Texture& texture)
    {
        setTexture(texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::Sprite(const Sprite& other) :
        m_size       (other.m_size),
        m_texture    (other.m_texture),
        m_svgTexture (nullptr),
        m_vertices   (other.m_vertices),
        m_indices    (other.m_indices),
        m_visibleRect(other.m_visibleRect),
        m_vertexColor(other.m_vertexColor),
        m_opacity    (other.m_opacity),
        m_rotation   (other.m_rotation),
        m_position   (other.m_position),
        m_scalingType(other.m_scalingType)
    {
        if (m_texture.getData() && m_texture.getData()->svgImage)
            updateVertices();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::Sprite(Sprite&& other) noexcept :
        m_size       (std::move(other.m_size)),
        m_texture    (std::move(other.m_texture)),
        m_svgTexture (std::move(other.m_svgTexture)),
        m_vertices   (std::move(other.m_vertices)),
        m_indices    (std::move(other.m_indices)),
        m_visibleRect(std::move(other.m_visibleRect)),
        m_vertexColor(std::move(other.m_vertexColor)),
        m_opacity    (std::move(other.m_opacity)),
        m_rotation   (std::move(other.m_rotation)),
        m_position   (std::move(other.m_position)),
        m_scalingType(std::move(other.m_scalingType))
    {
        if (m_svgTexture)
        {
            TGUI_ASSERT(isBackendSet(), "Backend must still exist when Sprite is move-constructed");
            getBackend()->unregisterSvgSprite(&other);
            getBackend()->registerSvgSprite(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::~Sprite()
    {
        if (m_svgTexture)
        {
            TGUI_ASSERT(isBackendSet(), "Backend must still exist when Sprite is destroyed");
            getBackend()->unregisterSvgSprite(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite& Sprite::operator= (const Sprite& other)
    {
        if (this != &other)
        {
            if (m_svgTexture)
                getBackend()->unregisterSvgSprite(this);

            Sprite temp(other);

            std::swap(m_size,        temp.m_size);
            std::swap(m_texture,     temp.m_texture);
            std::swap(m_svgTexture,  temp.m_svgTexture);
            std::swap(m_vertices,    temp.m_vertices);
            std::swap(m_indices,     temp.m_indices);
            std::swap(m_visibleRect, temp.m_visibleRect);
            std::swap(m_vertexColor, temp.m_vertexColor);
            std::swap(m_opacity,     temp.m_opacity);
            std::swap(m_rotation,    temp.m_rotation);
            std::swap(m_position,    temp.m_position);
            std::swap(m_scalingType, temp.m_scalingType);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite& Sprite::operator= (Sprite&& other) noexcept
    {
        if (this != &other)
        {
            if (m_svgTexture)
                getBackend()->unregisterSvgSprite(this);

            m_size        = std::move(other.m_size);
            m_texture     = std::move(other.m_texture);
            m_svgTexture  = std::move(other.m_svgTexture);
            m_vertices    = std::move(other.m_vertices);
            m_indices     = std::move(other.m_indices);
            m_visibleRect = std::move(other.m_visibleRect);
            m_vertexColor = std::move(other.m_vertexColor);
            m_opacity     = std::move(other.m_opacity);
            m_rotation    = std::move(other.m_rotation);
            m_position    = std::move(other.m_position);
            m_scalingType = std::move(other.m_scalingType);

            if (m_svgTexture)
            {
                TGUI_ASSERT(isBackendSet(), "Backend must still exist when Sprite is moved");
                getBackend()->unregisterSvgSprite(&other);
                getBackend()->registerSvgSprite(this);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setTexture(const Texture& texture)
    {
        m_texture = texture;
        m_vertexColor = m_texture.getColor();

        if (isSet())
        {
            if (getSize() == Vector2f{})
                setSize(Vector2f{texture.getImageSize()});
            else
                updateVertices();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& Sprite::getTexture() const
    {
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Sprite::isSet() const
    {
        return m_texture.getData() != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setSize(Vector2f size)
    {
        m_size.x = std::max(size.x, 0.f);
        m_size.y = std::max(size.y, 0.f);

        if (isSet())
            updateVertices();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Sprite::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setOpacity(float opacity)
    {
        m_opacity = opacity;

        const auto vertexColor = Vertex::Color(Color::applyOpacity(m_vertexColor, m_opacity));
        for (auto& vertex : m_vertices)
            vertex.color = Vertex::Color{vertexColor};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Sprite::getOpacity() const
    {
        return m_opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setVisibleRect(const FloatRect& visibleRect)
    {
        m_visibleRect = visibleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatRect Sprite::getVisibleRect() const
    {
        return m_visibleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setPosition(Vector2f position)
    {
        m_position = position;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Sprite::getPosition() const
    {
        return m_position;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setRotation(float angle)
    {
        m_rotation = angle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Sprite::getRotation() const
    {
        return m_rotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Sprite::isTransparentPixel(Vector2f pos) const
    {
        if (!isSet() || (m_size.x <= 0) || (m_size.y <= 0))
            return true;
        if (!m_texture.getData()->backendTexture)
            return false;

        if (getRotation() != 0)
        {
            const Transform transform = Transform().rotate(getRotation()).translate(getPosition());
            const Transform inverseTransform = transform.getInverse();
            const FloatRect transformedRect = transform.transformRect({{}, getSize()});
            pos = inverseTransform.transformPoint(pos) + inverseTransform.transformPoint(transformedRect.getPosition());

            // Watch out for rounding errors
            const float epsilon = 0.00001f;
            if ((pos.x < 0) && (pos.x > -epsilon))
                pos.x = 0;
            if ((pos.y < 0) && (pos.y > -epsilon))
                pos.y = 0;
            if ((pos.x >= getSize().x) && (pos.x < getSize().x + epsilon))
                pos.x = getSize().x;
            if ((pos.y >= getSize().y) && (pos.y < getSize().y + epsilon))
                pos.y = getSize().y;
        }
        else // There is no rotation
            pos -= getPosition();

        if ((pos.x < 0) || (pos.y < 0) || (pos.x >= getSize().x) || (pos.y >= getSize().y))
            return true;

        // Find out on which pixel the mouse is standing
        Vector2f pixel;
        FloatRect middleRect = FloatRect{m_texture.getMiddleRect()};
        Vector2u imageSize = m_texture.getImageSize();
        switch (m_scalingType)
        {
            case ScalingType::Normal:
            {
                pixel.x = pos.x / m_size.x * imageSize.x;
                pixel.y = pos.y / m_size.y * imageSize.y;
                break;
            }
            case ScalingType::Horizontal:
            {
                const float scaleFactor = (m_size.x >= (imageSize.x - middleRect.width) * (m_size.y / imageSize.y))
                    ? (m_size.y / imageSize.y)
                    : (m_size.x / (imageSize.x - middleRect.width));
                const float middleLeft = middleRect.left * scaleFactor;
                const float middleRight = m_size.x - (imageSize.x - middleRect.left - middleRect.width) * scaleFactor;

                if (pos.x >= middleRight)
                    pixel.x = (middleRect.left + middleRect.width) + ((pos.x - middleRight) / scaleFactor);
                else if (pos.x >= middleLeft)
                    pixel.x = middleRect.left + (pos.x - middleLeft) / (middleRight - middleLeft) * middleRect.width;
                else // Mouse on the left part
                    pixel.x = pos.x / scaleFactor;

                pixel.y = pos.y / m_size.y * imageSize.y;
                break;
            }
            case ScalingType::Vertical:
            {
                const float scaleFactor = (m_size.y >= (imageSize.y - middleRect.height) * (m_size.x / imageSize.x))
                    ? (m_size.x / imageSize.x)
                    : (m_size.y / (imageSize.y - middleRect.height));
                const float middleTop = middleRect.top * scaleFactor;
                const float middleBottom = m_size.y - ((imageSize.y - middleRect.top - middleRect.height) * scaleFactor);

                if (pos.y >= middleBottom)
                    pixel.y = (middleRect.top + middleRect.height) + ((pos.y - middleBottom) / scaleFactor);
                else if (pos.y >= middleTop)
                    pixel.y = middleRect.top + (pos.y - middleTop) / (middleBottom - middleTop) * middleRect.height;
                else // Mouse on the top part
                    pixel.y = pos.y / scaleFactor;

                pixel.x = pos.x / m_size.x * imageSize.x;
                break;
            }
            case ScalingType::NineSlice:
            {
                float scaleFactor;
                if (m_texture.getScaledNineSlice())
                    scaleFactor = std::min(m_size.x / imageSize.x, m_size.y / imageSize.y);
                else
                {
                    const float scaleFactorX = (m_size.x >= imageSize.x - middleRect.width) ? 1 : (m_size.x / (imageSize.x - middleRect.width));
                    const float scaleFactorY = (m_size.y >= imageSize.y - middleRect.height) ? 1 : (m_size.y / (imageSize.y - middleRect.height));
                    scaleFactor = std::min(scaleFactorX, scaleFactorY);
                }
                const float middleLeft = middleRect.left * scaleFactor;
                const float middleTop = middleRect.top * scaleFactor;
                const float middleRight = m_size.x - ((imageSize.x - middleRect.left - middleRect.width) * scaleFactor);
                const float middleBottom = m_size.y - ((imageSize.y - middleRect.top - middleRect.height) * scaleFactor);

                if (pos.x < middleLeft)
                    pixel.x = pos.x / scaleFactor;
                else if (pos.x >= middleRight)
                    pixel.x = (middleRect.left + middleRect.width) + ((pos.x - middleRight) / scaleFactor);
                else
                    pixel.x = middleRect.left + (pos.x - middleLeft) / (middleRight - middleLeft) * middleRect.width;

                if (pos.y < middleTop)
                    pixel.y = pos.y / scaleFactor;
                else if (pos.y >= middleBottom)
                    pixel.y = (middleRect.top + middleRect.height) + ((pos.y - middleBottom) / scaleFactor);
                else
                    pixel.y = middleRect.top + (pos.y - middleTop) / (middleBottom - middleTop) * middleRect.height;

                break;
            }
        };

        return m_texture.isTransparentPixel({static_cast<unsigned int>(pixel.x), static_cast<unsigned int>(pixel.y)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::ScalingType Sprite::getScalingType() const
    {
        return m_scalingType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::updateVertices()
    {
        // Figure out how the image is scaled best
        Vector2f textureSize;
        FloatRect middleRect;
        Vector2u texCoordOffset;
        if (m_texture.getData()->svgImage)
        {
            if (!m_svgTexture)
            {
                m_svgTexture = getBackend()->createTexture();

                TGUI_ASSERT(isBackendSet(), "Backend must still exist when SVG texture is loaded in Sprite");
                getBackend()->registerSvgSprite(this);
            }

            const Vector2u svgTextureSize{
                static_cast<unsigned int>(std::round(getSize().x)),
                static_cast<unsigned int>(std::round(getSize().y))};

            m_texture.getData()->svgImage->rasterize(*m_svgTexture, svgTextureSize);

            m_scalingType = ScalingType::Normal;
            textureSize = getSize();
        }
        else
        {
            texCoordOffset = m_texture.getPartRect().getPosition();
            textureSize = Vector2f{m_texture.getPartRect().getSize()};
            middleRect = FloatRect{m_texture.getMiddleRect()};
            if (middleRect == FloatRect(0, 0, textureSize.x, textureSize.y))
                m_scalingType = ScalingType::Normal;
            else if (middleRect.height == textureSize.y)
                m_scalingType = ScalingType::Horizontal;
            else if (middleRect.width == textureSize.x)
                m_scalingType = ScalingType::Vertical;
            else
                m_scalingType = ScalingType::NineSlice;
        }

        // Calculate the vertices based on the way we are scaling
        const Vertex::Color vertexColor(Color::applyOpacity(m_vertexColor, m_opacity));
        switch (m_scalingType)
        {
            case ScalingType::Normal:
            {
                ///////////
                // 0---1 //
                // |   | //
                // 2---3 //
                ///////////
                m_vertices = {
                    {{0, 0}, vertexColor, {0, 0}},
                    {{m_size.x, 0}, vertexColor, {textureSize.x, 0}},
                    {{0, m_size.y}, vertexColor, {0, textureSize.y}},
                    {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}},
                };
                m_indices = {
                    0, 2, 1,
                    1, 2, 3
                };
                break;
            }
            case ScalingType::Horizontal:
            {
                ///////////////////////
                // 0---2-------4---6 //
                // |   |       |   | //
                // 1---3-------5---7 //
                ///////////////////////
                const float scaleFactor = (m_size.x >= (textureSize.x - middleRect.width) * (m_size.y / textureSize.y))
                    ? (m_size.y / textureSize.y)
                    : (m_size.x / (textureSize.x - middleRect.width));
                const float middleLeft = middleRect.left * scaleFactor;
                const float middleRight = m_size.x - (textureSize.x - middleRect.left - middleRect.width) * scaleFactor;
                m_vertices = {
                    {{0, 0}, vertexColor, {0, 0}},
                    {{0, m_size.y}, vertexColor, {0, textureSize.y}},
                    {{middleLeft, 0}, vertexColor, {middleRect.left, 0}},
                    {{middleLeft, m_size.y}, vertexColor, {middleRect.left, textureSize.y}},
                    {{middleRight, 0}, vertexColor, {middleRect.left + middleRect.width, 0}},
                    {{middleRight, m_size.y}, vertexColor, {middleRect.left + middleRect.width, textureSize.y}},
                    {{m_size.x, 0}, vertexColor, {textureSize.x, 0}},
                    {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}},
                };
                m_indices = {
                    0, 1, 2,
                    1, 3, 2,
                    2, 3, 4,
                    3, 5, 4,
                    4, 5, 6,
                    5, 7, 6
                };
                break;
            }
            case ScalingType::Vertical:
            {
                ///////////
                // 0---1 //
                // |   | //
                // 2---3 //
                // |   | //
                // |   | //
                // |   | //
                // 4---5 //
                // |   | //
                // 6---7-//
                ///////////
                const float scaleFactor = (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                    ? (m_size.x / textureSize.x)
                    : (m_size.y / (textureSize.y - middleRect.height));
                const float middleTop = middleRect.top * scaleFactor;
                const float middleBottom = m_size.y - ((textureSize.y - middleRect.top - middleRect.height) * scaleFactor);
                m_vertices = {
                    {{0, 0}, vertexColor, {0, 0}},
                    {{m_size.x, 0}, vertexColor, {textureSize.x, 0}},
                    {{0, middleTop}, vertexColor, {0, middleRect.top}},
                    {{m_size.x, middleTop}, vertexColor, {textureSize.x, middleRect.top}},
                    {{0, middleBottom}, vertexColor, {0, middleRect.top + middleRect.height}},
                    {{m_size.x, middleBottom}, vertexColor, {textureSize.x, middleRect.top + middleRect.height}},
                    {{0, m_size.y}, vertexColor, {0, textureSize.y}},
                    {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}},
                };
                m_indices = {
                    0, 2, 1,
                    1, 2, 3,
                    2, 4, 3,
                    3, 4, 5,
                    4, 6, 5,
                    5, 6, 7
                };
                break;
            }
            case ScalingType::NineSlice:
            {
                ///////////////////////////////
                // 0----1-----------11----12 //
                // |    |            |    |  //
                // 2----3-----------10----13 //
                // |    |            |    |  //
                // |    |            |    |  //
                // |    |            |    |  //
                // 4----5------------9----14 //
                // |    |            |    |  //
                // 6----7------------8----15 //
                ///////////////////////////////
                float scaleFactor;
                if (m_texture.getScaledNineSlice())
                    scaleFactor = std::min(m_size.x / textureSize.x, m_size.y / textureSize.y);
                else
                {
                    const float scaleFactorX = (m_size.x >= textureSize.x - middleRect.width) ? 1 : (m_size.x / (textureSize.x - middleRect.width));
                    const float scaleFactorY = (m_size.y >= textureSize.y - middleRect.height) ? 1 : (m_size.y / (textureSize.y - middleRect.height));
                    scaleFactor = std::min(scaleFactorX, scaleFactorY);
                }
                const float middleLeft = middleRect.left * scaleFactor;
                const float middleTop = middleRect.top * scaleFactor;
                const float middleRight = m_size.x - ((textureSize.x - middleRect.left - middleRect.width) * scaleFactor);
                const float middleBottom = m_size.y - ((textureSize.y - middleRect.top - middleRect.height) * scaleFactor);
                m_vertices = {
                    {{0, 0}, vertexColor, {0, 0}},
                    {{middleLeft, 0}, vertexColor, {middleRect.left, 0}},
                    {{0, middleTop}, vertexColor, {0, middleRect.top}},
                    {{middleLeft, middleTop}, vertexColor, {middleRect.left, middleRect.top}},
                    {{0, middleBottom}, vertexColor, {0, middleRect.top + middleRect.height}},
                    {{middleLeft, middleBottom}, vertexColor, {middleRect.left, middleRect.top + middleRect.height}},
                    {{0, m_size.y}, vertexColor, {0, textureSize.y}},
                    {{middleLeft, m_size.y}, vertexColor, {middleRect.left, textureSize.y}},
                    {{middleRight, m_size.y}, vertexColor, {middleRect.left + middleRect.width, textureSize.y}},
                    {{middleRight, middleBottom}, vertexColor, {middleRect.left + middleRect.width, middleRect.top + middleRect.height}},
                    {{middleRight, middleTop}, vertexColor, {middleRect.left + middleRect.width, middleRect.top}},
                    {{middleRight, 0}, vertexColor, {middleRect.left + middleRect.width, 0}},
                    {{m_size.x, 0}, vertexColor, {textureSize.x, 0}},
                    {{m_size.x, middleTop}, vertexColor, {textureSize.x, middleRect.top}},
                    {{m_size.x, middleBottom}, vertexColor, {textureSize.x, middleRect.top + middleRect.height}},
                    {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}},
                };
                m_indices = {
                    0, 2, 1,
                    1, 2, 3,
                    2, 4, 3,
                    3, 4, 5,
                    4, 6, 5,
                    6, 7, 5,
                    7, 8, 5,
                    8, 9, 5,
                    5, 9, 3,
                    9, 10, 3,
                    3, 10, 1,
                    1, 10, 11,
                    11, 10, 12,
                    12, 10, 13,
                    10, 13, 9,
                    13, 9, 14,
                    9, 8, 14,
                    8, 15, 14
                };
                break;
            }
        };

        if (texCoordOffset != Vector2u{})
        {
            for (auto& vertex : m_vertices)
            {
                vertex.texCoords.x += static_cast<float>(m_texture.getPartRect().left);
                vertex.texCoords.y += static_cast<float>(m_texture.getPartRect().top);
            }
        }

        // Normalize the texture coordinates
        if (m_texture.getData()->svgImage)
        {
            const Vector2f svgTextureSize{std::round(getSize().x), std::round(getSize().y)};
            if ((svgTextureSize.x != 0) && (svgTextureSize.y != 0))
            {
                for (auto& vertex : m_vertices)
                {
                    vertex.texCoords.x /= svgTextureSize.x;
                    vertex.texCoords.y /= svgTextureSize.y;
                }
            }
        }
        else if (m_texture.getData()->backendTexture)
        {
            const Vector2f backendTextureSize{m_texture.getData()->backendTexture->getSize()};
            if ((backendTextureSize.x != 0) && (backendTextureSize.y != 0))
            {
                for (auto& vertex : m_vertices)
                {
                    vertex.texCoords.x /= backendTextureSize.x;
                    vertex.texCoords.y /= backendTextureSize.y;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
