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


#include <TGUI/Sprite.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/Optional.hpp>

#include <cassert>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Sprite::Sprite(const Texture& texture)
    {
        setTexture(texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setTexture(const Texture& texture)
    {
        m_texture = texture;
        m_vertexColor = m_texture.getColor();
        m_shader = m_texture.getShader();

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

    void Sprite::setSize(const Vector2f& size)
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

        const Color& vertexColor = Color::applyOpacity(m_vertexColor, m_opacity);
        for (auto& vertex : m_vertices)
            vertex.color = sf::Color{vertexColor};
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

    bool Sprite::isTransparentPixel(Vector2f pos) const
    {
        if (!isSet() || (m_size.x == 0) || (m_size.y == 0))
            return true;
        if (!m_texture.getData()->image || !m_texture.getData()->texture)
            return false;

        if (getRotation() != 0)
        {
            Vector2f offset = {getTransform().transformRect(sf::FloatRect{FloatRect{{}, getSize()}}).left,
                               getTransform().transformRect(sf::FloatRect{FloatRect{{}, getSize()}}).top};

            pos = Vector2f{getInverseTransform().transformPoint(sf::Vector2f{pos}) + getInverseTransform().transformPoint(sf::Vector2f{offset})};

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
            pos -= Vector2f(getPosition());

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
                if (pos.x >= m_size.x - (imageSize.x - middleRect.left - middleRect.width) * (m_size.y / imageSize.y))
                {
                    float xDiff = pos.x - (m_size.x - (imageSize.x - middleRect.left - middleRect.width) * (m_size.y / imageSize.y));
                    pixel.x = middleRect.left + middleRect.width + (xDiff / m_size.y * imageSize.y);
                }
                else if (pos.x >= middleRect.left * (m_size.y / imageSize.y))
                {
                    float xDiff = pos.x - (middleRect.left * (m_size.y / imageSize.y));
                    pixel.x = middleRect.left + (xDiff / (m_size.x - ((imageSize.x - middleRect.width) * (m_size.y / imageSize.y))) * middleRect.width);
                }
                else // Mouse on the left part
                {
                    pixel.x = pos.x / m_size.y * imageSize.y;
                }

                pixel.y = pos.y / m_size.y * imageSize.y;
                break;
            }
            case ScalingType::Vertical:
            {
                if (pos.y >= m_size.y - (imageSize.y - middleRect.top - middleRect.height) * (m_size.x / imageSize.x))
                {
                    float yDiff = pos.y - (m_size.y - (imageSize.y - middleRect.top - middleRect.height) * (m_size.x / imageSize.x));
                    pixel.y = middleRect.top + middleRect.height + (yDiff / m_size.x * imageSize.x);
                }
                else if (pos.y >= middleRect.top * (m_size.x / imageSize.x))
                {
                    float yDiff = pos.y - (middleRect.top * (m_size.x / imageSize.x));
                    pixel.y = middleRect.top + (yDiff / (m_size.y - ((imageSize.y - middleRect.height) * (m_size.x / imageSize.x))) * middleRect.height);
                }
                else // Mouse on the top part
                {
                    pixel.y = pos.y / m_size.x * imageSize.x;
                }

                pixel.x = pos.x / m_size.x * imageSize.x;
                break;
            }
            case ScalingType::NineSlice:
            {
                if (pos.x < middleRect.left)
                    pixel.x = pos.x;
                else if (pos.x >= m_size.x - (imageSize.x - middleRect.width - middleRect.left))
                    pixel.x = pos.x - m_size.x + imageSize.x;
                else
                {
                    float xDiff = (pos.x - middleRect.left) / (m_size.x - (imageSize.x - middleRect.width)) * middleRect.width;
                    pixel.x = middleRect.left + xDiff;
                }

                if (pos.y < middleRect.top)
                    pixel.y = pos.y;
                else if (pos.y >= m_size.y - (imageSize.y - middleRect.height - middleRect.top))
                    pixel.y = pos.y - m_size.y + imageSize.y;
                else
                {
                    float yDiff = (pos.y - middleRect.top) / (m_size.y - (imageSize.y - middleRect.height)) * middleRect.height;
                    pixel.y = middleRect.top + yDiff;
                }

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
                m_svgTexture = aurora::makeCopied<sf::Texture>();

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
            {
                m_scalingType = ScalingType::Normal;
            }
            else if (middleRect.height == textureSize.y)
            {
                if (m_size.x >= (textureSize.x - middleRect.width) * (m_size.y / textureSize.y))
                    m_scalingType = ScalingType::Horizontal;
                else
                    m_scalingType = ScalingType::Normal;
            }
            else if (middleRect.width == textureSize.x)
            {
                if (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                    m_scalingType = ScalingType::Vertical;
                else
                    m_scalingType = ScalingType::Normal;
            }
            else
            {
                if (m_size.x >= textureSize.x - middleRect.width)
                {
                    if (m_size.y >= textureSize.y - middleRect.height)
                        m_scalingType = ScalingType::NineSlice;
                    else
                        m_scalingType = ScalingType::Horizontal;
                }
                else if (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                    m_scalingType = ScalingType::Vertical;
                else
                    m_scalingType = ScalingType::Normal;
            }
        }

        // Calculate the vertices based on the way we are scaling
        const sf::Color vertexColor(Color::applyOpacity(m_vertexColor, m_opacity));
        switch (m_scalingType)
        {
        case ScalingType::Normal:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            ///////////
            m_vertices.resize(4);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[3] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Horizontal:
            ///////////////////////
            // 0---2-------4---6 //
            // |   |       |   | //
            // 1---3-------5---7 //
            ///////////////////////
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[2] = {{middleRect.left * (m_size.y / textureSize.y), 0}, vertexColor, {middleRect.left, 0}};
            m_vertices[3] = {{middleRect.left * (m_size.y / textureSize.y), m_size.y}, vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[4] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), 0}, vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[5] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), m_size.y}, vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[6] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[7] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Vertical:
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
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, middleRect.top * (m_size.x / textureSize.x)}, vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{m_size.x, middleRect.top * (m_size.x / textureSize.x)}, vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::NineSlice:
            //////////////////////////////////
            // 0---1/13-----------14-----15 //
            // |    |              |     |  //
            // 2---3/11----------12/16---17 //
            // |    |              |     |  //
            // |    |              |     |  //
            // |    |              |     |  //
            // 4---5/9-----------10/18---19 //
            // |    |              |     |  //
            // 6----7-------------8/20---21 //
            //////////////////////////////////
            m_vertices.resize(22);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{middleRect.left, 0}, vertexColor, {middleRect.left, 0}};
            m_vertices[2] = {{0, middleRect.top}, vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{middleRect.left, middleRect.top}, vertexColor, {middleRect.left, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{middleRect.left, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {middleRect.left, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{middleRect.left, m_size.y}, vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[8] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y}, vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[9] = m_vertices[5];
            m_vertices[10] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {middleRect.left + middleRect.width, middleRect.top + middleRect.height}};
            m_vertices[11] = m_vertices[3];
            m_vertices[12] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), middleRect.top}, vertexColor, {middleRect.left + middleRect.width, middleRect.top}};
            m_vertices[13] = m_vertices[1];
            m_vertices[14] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), 0}, vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[15] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[16] = m_vertices[12];
            m_vertices[17] = {{m_size.x, middleRect.top}, vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[18] = m_vertices[10];
            m_vertices[19] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[20] = m_vertices[8];
            m_vertices[21] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;
        };

        if (texCoordOffset != Vector2u{})
        {
            for (auto& vertex : m_vertices)
            {
                vertex.texCoords.x += static_cast<float>(m_texture.getPartRect().left);
                vertex.texCoords.y += static_cast<float>(m_texture.getPartRect().top);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!isSet())
            return;

        // A rotation can cause the image to be shifted, so we move it upfront so that it ends at the correct location
        if (getRotation() != 0)
        {
            sf::Vector2f pos = {getTransform().transformRect(sf::FloatRect(FloatRect({}, getSize()))).left,
                                getTransform().transformRect(sf::FloatRect(FloatRect({}, getSize()))).top};

            states.transform.translate(sf::Vector2f(getPosition()) - pos);
        }

        states.transform *= getTransform();

        // Apply clipping when needed
        Optional<Clipping> clipping;
        if (m_visibleRect != FloatRect{})
            clipping.emplace(target, states, Vector2f{m_visibleRect.left, m_visibleRect.top}, Vector2f{m_visibleRect.width, m_visibleRect.height});

        if (m_texture.getData()->svgImage)
            states.texture = m_svgTexture.get();
        else
            states.texture = &m_texture.getData()->texture.value();

        states.shader = m_shader;
        target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
