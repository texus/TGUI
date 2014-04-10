/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <SFML/OpenGL.hpp>
#include <TGUI/Texture.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture() :
        sf::Transformable(),
        sf::Drawable     (),
        m_data           (nullptr),
        m_scalingType    (Normal),
        m_rotation       (0)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTexture(TextureData& data, const sf::IntRect& middleRect)
    {
        m_data = &data;
        m_middleRect = middleRect;

        setSize(m_data->texture.getSize().x, m_data->texture.getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextureData* Texture::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setSize(float width, float height)
    {
        if (m_data != nullptr)
        {
            m_size = sf::Vector2f(width, height);

            if (m_size.x < 0)
                m_size.x = -m_size.x;
            if (m_size.y < 0)
                m_size.y = -m_size.y;

            setOrigin(m_size.x / 2.0f, m_size.x / 2.0f);

            updateVertices();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Texture::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Texture::getImageSize() const
    {
        if (m_data)
            return sf::Vector2f(m_data->texture.getSize());
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setColor(const sf::Color& color)
    {
        for (auto& vertex : m_vertices)
            vertex.color = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureRect(const sf::FloatRect& textureRect)
    {
        m_textureRect = textureRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::updateVertices()
    {
        // Figure out how the image is scaled best
        if (m_middleRect == sf::IntRect(0, 0, m_data->texture.getSize().x, m_data->texture.getSize().y))
        {
            m_scalingType = Normal;
        }
        else if (m_middleRect.height == static_cast<int>(m_data->texture.getSize().y))
        {
            if (m_size.x >= (m_data->texture.getSize().x - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y))
                m_scalingType = Horizontal;
            else
                m_scalingType = Normal;
        }
        else if (m_middleRect.width == static_cast<int>(m_data->texture.getSize().x))
        {
            if (m_size.y >= (m_data->texture.getSize().y - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x))
                m_scalingType = Vertical;
            else
                m_scalingType = Normal;
        }
        else
        {
            if (m_size.x >= m_data->texture.getSize().x - m_middleRect.width)
            {
                if (m_size.y >= m_data->texture.getSize().y - m_middleRect.height)
                    m_scalingType = NineSliceScaling;
                else
                {
                    if (m_size.x >= (m_data->texture.getSize().x - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y))
                        m_scalingType = Horizontal;
                    else
                        m_scalingType = Normal;
                }
            }
            else
            {
                if (m_size.y >= (m_data->texture.getSize().y - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x))
                    m_scalingType = Vertical;
                else
                    m_scalingType = Normal;
            }
        }

        // Calculate the vertices based on the way we are scaling
        switch (m_scalingType)
        {
        case Normal:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            ///////////
            m_vertices.resize(4);
            m_vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
            m_vertices[1] = sf::Vertex(sf::Vector2f(m_size.x, 0), sf::Vector2f(m_data->texture.getSize().x, 0));
            m_vertices[2] = sf::Vertex(sf::Vector2f(0, m_size.y), sf::Vector2f(0, m_data->texture.getSize().y));
            m_vertices[3] = sf::Vertex(sf::Vector2f(m_size.x, m_size.y), sf::Vector2f(m_data->texture.getSize().x, m_data->texture.getSize().y));
            break;

        case Horizontal:
            ///////////////////////
            // 0---2-------4---6 //
            // |   |       |   | //
            // 1---3-------5---7 //
            ///////////////////////
            m_vertices.resize(8);
            m_vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
            m_vertices[1] = sf::Vertex(sf::Vector2f(0, m_size.y), sf::Vector2f(0, m_data->texture.getSize().y));
            m_vertices[2] = sf::Vertex(sf::Vector2f(m_middleRect.left * (m_size.y / m_data->texture.getSize().y), 0), sf::Vector2f(m_middleRect.left, 0));
            m_vertices[3] = sf::Vertex(sf::Vector2f(m_middleRect.left * (m_size.y / m_data->texture.getSize().y), m_size.y), sf::Vector2f(m_middleRect.left, m_data->texture.getSize().y));
            m_vertices[4] = sf::Vertex(sf::Vector2f(m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y), 0), sf::Vector2f(m_middleRect.left + m_middleRect.width, 0));
            m_vertices[5] = sf::Vertex(sf::Vector2f(m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y), m_size.y), sf::Vector2f(m_middleRect.left + m_middleRect.width, m_data->texture.getSize().y));
            m_vertices[6] = sf::Vertex(sf::Vector2f(m_size.x, 0), sf::Vector2f(m_data->texture.getSize().x, 0));
            m_vertices[7] = sf::Vertex(sf::Vector2f(m_size.x, m_size.y), sf::Vector2f(m_data->texture.getSize().x, m_data->texture.getSize().y));
            break;

        case Vertical:
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
            m_vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
            m_vertices[1] = sf::Vertex(sf::Vector2f(m_size.x, 0), sf::Vector2f(m_data->texture.getSize().x, 0));
            m_vertices[2] = sf::Vertex(sf::Vector2f(0, m_middleRect.top * (m_size.x / m_data->texture.getSize().x)), sf::Vector2f(0, m_middleRect.top));
            m_vertices[3] = sf::Vertex(sf::Vector2f(m_size.x, m_middleRect.top * (m_size.x / m_data->texture.getSize().x)), sf::Vector2f(m_data->texture.getSize().x, m_middleRect.top));
            m_vertices[4] = sf::Vertex(sf::Vector2f(0, m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x)), sf::Vector2f(0, m_middleRect.top + m_middleRect.height));
            m_vertices[5] = sf::Vertex(sf::Vector2f(m_size.x, m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x)), sf::Vector2f(m_data->texture.getSize().x, m_middleRect.top + m_middleRect.height));
            m_vertices[6] = sf::Vertex(sf::Vector2f(0, m_size.y), sf::Vector2f(0, m_data->texture.getSize().y));
            m_vertices[7] = sf::Vertex(sf::Vector2f(m_size.x, m_size.y), sf::Vector2f(m_data->texture.getSize().x, m_data->texture.getSize().y));
            break;

        case NineSliceScaling:
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
            m_vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
            m_vertices[1] = sf::Vertex(sf::Vector2f(m_middleRect.left, 0), sf::Vector2f(m_middleRect.left, 0));
            m_vertices[2] = sf::Vertex(sf::Vector2f(0, m_middleRect.top), sf::Vector2f(0, m_middleRect.top));
            m_vertices[3] = sf::Vertex(sf::Vector2f(m_middleRect.left, m_middleRect.top), sf::Vector2f(m_middleRect.left, m_middleRect.top));
            m_vertices[4] = sf::Vertex(sf::Vector2f(0, m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height)), sf::Vector2f(0, m_middleRect.top + m_middleRect.height));
            m_vertices[5] = sf::Vertex(sf::Vector2f(m_middleRect.left, m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height)), sf::Vector2f(m_middleRect.left, m_middleRect.top + m_middleRect.height));
            m_vertices[6] = sf::Vertex(sf::Vector2f(0, m_size.y), sf::Vector2f(0, m_data->texture.getSize().y));
            m_vertices[7] = sf::Vertex(sf::Vector2f(m_middleRect.left, m_size.y), sf::Vector2f(m_middleRect.left, m_data->texture.getSize().y));
            m_vertices[8] = sf::Vertex(sf::Vector2f(m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width), m_size.y), sf::Vector2f(m_middleRect.left + m_middleRect.width, m_data->texture.getSize().y));
            m_vertices[9] = m_vertices[5];
            m_vertices[10] = sf::Vertex(sf::Vector2f(m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width), m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height)), sf::Vector2f(m_middleRect.left + m_middleRect.width, m_middleRect.top + m_middleRect.height));
            m_vertices[11] = m_vertices[3];
            m_vertices[12] = sf::Vertex(sf::Vector2f(m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width), m_middleRect.top), sf::Vector2f(m_middleRect.left + m_middleRect.width, m_middleRect.top));
            m_vertices[13] = m_vertices[1];
            m_vertices[14] = sf::Vertex(sf::Vector2f(m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width), 0), sf::Vector2f(m_middleRect.left + m_middleRect.width, 0));
            m_vertices[15] = sf::Vertex(sf::Vector2f(m_size.x, 0), sf::Vector2f(m_data->texture.getSize().x, 0));
            m_vertices[16] = m_vertices[12];
            m_vertices[17] = sf::Vertex(sf::Vector2f(m_size.x, m_middleRect.top), sf::Vector2f(m_data->texture.getSize().x, m_middleRect.top));
            m_vertices[18] = m_vertices[10];
            m_vertices[19] = sf::Vertex(sf::Vector2f(m_size.x, m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height)), sf::Vector2f(m_data->texture.getSize().x, m_middleRect.top + m_middleRect.height));
            m_vertices[20] = m_vertices[8];
            m_vertices[21] = sf::Vertex(sf::Vector2f(m_size.x, m_size.y), sf::Vector2f(m_data->texture.getSize().x, m_data->texture.getSize().y));
            break;
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(float x, float y) const
    {
        x -= getPosition().x;
        y -= getPosition().y;

        // Find out on which pixel the mouse is standing
        sf::Vector2u pixel;
        switch (m_scalingType)
        {
        case Normal:
            pixel.x = static_cast<unsigned int>(x / m_size.x * m_data->texture.getSize().x);
            pixel.y = static_cast<unsigned int>(y / m_size.y * m_data->texture.getSize().y);
            break;

        case Horizontal:
        case Vertical:
        case NineSliceScaling:
            return false;
        };

        if (m_data->image->getPixel(pixel.x + m_data->rect.left, pixel.y + m_data->rect.top).a == 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getOrigin());
        states.transform *= getTransform();

        if (m_data != nullptr)
        {
            if (m_textureRect == sf::FloatRect(0, 0, 0, 0))
            {
                states.texture = &m_data->texture;
                target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
            }
            else
            {
                // Calculate the scale factor of the view
                float scaleViewX = target.getSize().x / target.getView().getSize().x;
                float scaleViewY = target.getSize().y / target.getView().getSize().y;

                // Get the global position
                sf::Vector2f topLeftPosition = states.transform.transformPoint(sf::Vector2f(m_textureRect.left, m_textureRect.top) - target.getView().getCenter() + (target.getView().getSize() / 2.f));
                sf::Vector2f bottomRightPosition = states.transform.transformPoint(sf::Vector2f(m_textureRect.left, m_textureRect.top) + sf::Vector2f(m_textureRect.width, m_textureRect.height) - target.getView().getCenter() + (target.getView().getSize() / 2.f));

                // Get the old clipping area
                GLint scissor[4];
                glGetIntegerv(GL_SCISSOR_BOX, scissor);

                // Calculate the clipping area
                GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
                GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
                GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

                // If the object outside the window then don't draw anything
                if (scissorRight < scissorLeft)
                    scissorRight = scissorLeft;
                else if (scissorBottom < scissorTop)
                    scissorTop = scissorBottom;

                // Set the clipping area
                glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

                // Draw the texture
                states.texture = &m_data->texture;
                target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);

                // Reset the old clipping area
                glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
