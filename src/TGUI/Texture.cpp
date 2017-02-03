/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <SFML/OpenGL.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Texture::TextureLoaderFunc Texture::m_textureLoader = &TextureManager::getTexture;
    Texture::ImageLoaderFunc Texture::m_imageLoader = [](const sf::String& filename) -> std::shared_ptr<sf::Image>
        {
            auto image = std::make_shared<sf::Image>();
            if (image->loadFromFile(filename))
                return image;
            else
                return nullptr;
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::String& id, const sf::IntRect& partRect, const sf::IntRect& middlePart)
    {
        load(id, partRect, middlePart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::Texture& texture, const sf::IntRect& partRect, const sf::IntRect& middlePart)
    {
        load(texture, partRect, middlePart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const Texture& copy) :
        sf::Transformable {copy},
        sf::Drawable      {copy},
        m_data            (copy.m_data),
        m_vertices        (copy.m_vertices), // Did not compile in VS2013 when using braces
        m_size            {copy.m_size},
        m_middleRect      {copy.m_middleRect},
        m_textureRect     {copy.m_textureRect},
        m_vertexColor     {copy.m_vertexColor},
        m_scalingType     {copy.m_scalingType},
        m_loaded          {copy.m_loaded},
        m_id              (copy.m_id), // Did not compile in VS2013 when using braces
        m_copyCallback    {copy.m_copyCallback},
        m_destructCallback{copy.m_destructCallback}
    {
        if (m_loaded && (m_copyCallback != nullptr))
            m_copyCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::~Texture()
    {
        if (m_loaded && (m_destructCallback != nullptr))
            m_destructCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(const Texture& right)
    {
        if (this != &right)
        {
            Texture temp{right};
            sf::Transformable::operator=(right);
            sf::Drawable::operator=(right);

            std::swap(m_data,             temp.m_data);
            std::swap(m_vertices,         temp.m_vertices);
            std::swap(m_size,             temp.m_size);
            std::swap(m_middleRect,       temp.m_middleRect);
            std::swap(m_textureRect,      temp.m_textureRect);
            std::swap(m_vertexColor,      temp.m_vertexColor);
            std::swap(m_scalingType,      temp.m_scalingType);
            std::swap(m_loaded,           temp.m_loaded);
            std::swap(m_id,               temp.m_id);
            std::swap(m_copyCallback,     temp.m_copyCallback);
            std::swap(m_destructCallback, temp.m_destructCallback);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::String& id, const sf::IntRect& partRect, const sf::IntRect& middleRect)
    {
        if (m_loaded && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_loaded = false;
        if (!m_textureLoader(*this, id, partRect))
            throw Exception{"Failed to load '" + id + "'"};

        m_id = id;
        setTexture(m_data, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::Texture& texture, const sf::IntRect& partRect, const sf::IntRect& middleRect)
    {
        if (m_loaded && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_loaded = false;

        auto data = std::make_shared<TextureData>();

        if (partRect == sf::IntRect{})
            data->texture = texture;
        else
            data->texture.loadFromImage(texture.copyToImage(), partRect);

        m_id = "";
        setTexture(data, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTexture(std::shared_ptr<TextureData> data, const sf::IntRect& middleRect)
    {
        if (m_loaded && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = data;
        m_loaded = true;

        if (middleRect == sf::IntRect{})
            m_middleRect = {0, 0, static_cast<int>(m_data->texture.getSize().x), static_cast<int>(m_data->texture.getSize().y)};
        else
            m_middleRect = middleRect;

        setSize(sf::Vector2f{m_data->texture.getSize()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Texture::getId() const
    {
        return m_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData>& Texture::getData()
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<const TextureData> Texture::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setSize(const sf::Vector2f& size)
    {
        if (m_loaded)
        {
            m_size.x = std::max(size.x, 0.f);
            m_size.y = std::max(size.y, 0.f);

            updateVertices();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setColor(const sf::Color& color)
    {
        m_vertexColor = color;
        for (auto& vertex : m_vertices)
            vertex.color = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Texture::getColor() const
    {
        return m_vertexColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setSmooth(bool smooth)
    {
        if (m_loaded)
            m_data->texture.setSmooth(smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(float x, float y) const
    {
        if (!m_data->image || (m_size.x == 0) || (m_size.y == 0))
            return false;

        assert((x >= getPosition().x) && (y >= getPosition().y) && (x < getPosition().x + getSize().x) && (y < getPosition().y + getSize().y));
        x -= getPosition().x;
        y -= getPosition().y;

        // Find out on which pixel the mouse is standing
        sf::Vector2u pixel;
        switch (m_scalingType)
        {
            case ScalingType::Normal:
            {
                pixel.x = static_cast<unsigned int>(x / m_size.x * m_data->texture.getSize().x);
                pixel.y = static_cast<unsigned int>(y / m_size.y * m_data->texture.getSize().y);
                break;
            }
            case ScalingType::Horizontal:
            {
                if (x >= m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y))
                {
                    float xDiff = (x - (m_size.x - (m_data->texture.getSize().x - m_middleRect.left - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y)));
                    pixel.x = static_cast<unsigned int>(m_middleRect.left + m_middleRect.width + (xDiff / m_size.y * m_data->texture.getSize().y));
                }
                else if (x >= m_middleRect.left * (m_size.y / m_data->texture.getSize().y))
                {
                    float xDiff = x - (m_middleRect.left * (m_size.y / m_data->texture.getSize().y));
                    pixel.x = static_cast<unsigned int>(m_middleRect.left + (xDiff / (m_size.x - ((m_data->texture.getSize().x - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y))) * m_middleRect.width));
                }
                else // Mouse on the left part
                {
                    pixel.x = static_cast<unsigned int>(x / m_size.y * m_data->texture.getSize().y);
                }

                pixel.y = static_cast<unsigned int>(y / m_size.y * m_data->texture.getSize().y);
                break;
            }
            case ScalingType::Vertical:
            {
                if (y >= m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x))
                {
                    float yDiff = (y - (m_size.y - (m_data->texture.getSize().y - m_middleRect.top - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x)));
                    pixel.y = static_cast<unsigned int>(m_middleRect.top + m_middleRect.height + (yDiff / m_size.x * m_data->texture.getSize().x));
                }
                else if (y >= m_middleRect.top * (m_size.x / m_data->texture.getSize().x))
                {
                    float yDiff = y - (m_middleRect.top * (m_size.x / m_data->texture.getSize().x));
                    pixel.y = static_cast<unsigned int>(m_middleRect.top + (yDiff / (m_size.y - ((m_data->texture.getSize().y - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x))) * m_middleRect.height));
                }
                else // Mouse on the top part
                {
                    pixel.y = static_cast<unsigned int>(y / m_size.x * m_data->texture.getSize().x);
                }

                pixel.x = static_cast<unsigned int>(x / m_size.x * m_data->texture.getSize().x);
                break;
            }
            case ScalingType::NineSlice:
            {
                if (x < m_middleRect.left)
                    pixel.x = static_cast<unsigned int>(x);
                else if (x >= m_size.x - (m_data->texture.getSize().x - m_middleRect.width - m_middleRect.left))
                    pixel.x = static_cast<unsigned int>(x - m_size.x + m_data->texture.getSize().x);
                else
                {
                    float xDiff = (x - m_middleRect.left) / (m_size.x - (m_data->texture.getSize().x - m_middleRect.width)) * m_middleRect.width;
                    pixel.x = static_cast<unsigned int>(m_middleRect.left + xDiff);
                }

                if (y < m_middleRect.top)
                    pixel.y = static_cast<unsigned int>(y);
                else if (y >= m_size.y - (m_data->texture.getSize().y - m_middleRect.height - m_middleRect.top))
                    pixel.y = static_cast<unsigned int>(y - m_size.y + m_data->texture.getSize().y);
                else
                {
                    float yDiff = (y - m_middleRect.top) / (m_size.y - (m_data->texture.getSize().y - m_middleRect.height)) * m_middleRect.height;
                    pixel.y = static_cast<unsigned int>(m_middleRect.top + yDiff);
                }

                break;
            }
        };

        assert(pixel.x < m_data->texture.getSize().x && pixel.y < m_data->texture.getSize().y);
        if (m_data->image->getPixel(pixel.x + m_data->rect.left, pixel.y + m_data->rect.top).a == 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setImageLoader(const ImageLoaderFunc& func)
    {
        assert(func != nullptr);
        m_imageLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureLoader(const TextureLoaderFunc& func)
    {
        assert(func != nullptr);
        m_textureLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::ImageLoaderFunc Texture::getImageLoader()
    {
        return m_imageLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::TextureLoaderFunc Texture::getTextureLoader()
    {
        return m_textureLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setCopyCallback(const std::function<void(std::shared_ptr<TextureData>)> func)
    {
        m_copyCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setDestructCallback(const std::function<void(std::shared_ptr<TextureData>)> func)
    {
        m_destructCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::updateVertices()
    {
        // Figure out how the image is scaled best
        if (m_middleRect == sf::IntRect(0, 0, m_data->texture.getSize().x, m_data->texture.getSize().y))
        {
            m_scalingType = ScalingType::Normal;
        }
        else if (m_middleRect.height == static_cast<int>(m_data->texture.getSize().y))
        {
            if (m_size.x >= (m_data->texture.getSize().x - m_middleRect.width) * (m_size.y / m_data->texture.getSize().y))
                m_scalingType = ScalingType::Horizontal;
            else
                m_scalingType = ScalingType::Normal;
        }
        else if (m_middleRect.width == static_cast<int>(m_data->texture.getSize().x))
        {
            if (m_size.y >= (m_data->texture.getSize().y - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x))
                m_scalingType = ScalingType::Vertical;
            else
                m_scalingType = ScalingType::Normal;
        }
        else
        {
            if (m_size.x >= m_data->texture.getSize().x - m_middleRect.width)
            {
                if (m_size.y >= m_data->texture.getSize().y - m_middleRect.height)
                    m_scalingType = ScalingType::NineSlice;
                else
                    m_scalingType = ScalingType::Horizontal;
            }
            else if (m_size.y >= (m_data->texture.getSize().y - m_middleRect.height) * (m_size.x / m_data->texture.getSize().x))
                m_scalingType = ScalingType::Vertical;
            else
                m_scalingType = ScalingType::Normal;
        }

        sf::Vector2f textureSize{m_data->texture.getSize()};
        sf::FloatRect middleRect{m_middleRect};

        // Calculate the vertices based on the way we are scaling
        switch (m_scalingType)
        {
        case ScalingType::Normal:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            ///////////
            m_vertices.resize(4);
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[3] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Horizontal:
            ///////////////////////
            // 0---2-------4---6 //
            // |   |       |   | //
            // 1---3-------5---7 //
            ///////////////////////
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[2] = {{middleRect.left * (m_size.y / textureSize.y), 0}, m_vertexColor, {middleRect.left, 0}};
            m_vertices[3] = {{middleRect.left * (m_size.y / textureSize.y), m_size.y}, m_vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[4] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), 0}, m_vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[5] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), m_size.y}, m_vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[6] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[7] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
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
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, middleRect.top * (m_size.x / textureSize.x)}, m_vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{m_size.x, middleRect.top * (m_size.x / textureSize.x)}, m_vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, m_vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, m_vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
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
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{middleRect.left, 0}, m_vertexColor, {middleRect.left, 0}};
            m_vertices[2] = {{0, middleRect.top}, m_vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{middleRect.left, middleRect.top}, m_vertexColor, {middleRect.left, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{middleRect.left, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {middleRect.left, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{middleRect.left, m_size.y}, m_vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[8] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y}, m_vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[9] = m_vertices[5];
            m_vertices[10] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {middleRect.left + middleRect.width, middleRect.top + middleRect.height}};
            m_vertices[11] = m_vertices[3];
            m_vertices[12] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), middleRect.top}, m_vertexColor, {middleRect.left + middleRect.width, middleRect.top}};
            m_vertices[13] = m_vertices[1];
            m_vertices[14] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), 0}, m_vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[15] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[16] = m_vertices[12];
            m_vertices[17] = {{m_size.x, middleRect.top}, m_vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[18] = m_vertices[10];
            m_vertices[19] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[20] = m_vertices[8];
            m_vertices[21] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
            break;
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // A rotation can cause the image to be shifted, so we move it upfront so that it ends at the correct location
        if (getRotation() != 0)
        {
            sf::Vector2f pos = {getTransform().transformRect(sf::FloatRect({}, getSize())).left,
                                getTransform().transformRect(sf::FloatRect({}, getSize())).top};

            states.transform.translate(getPosition() - pos);
        }

        states.transform *= getTransform();

        if (m_loaded)
        {
            if (m_textureRect == sf::FloatRect(0, 0, 0, 0))
            {
                states.texture = &m_data->texture;
                target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
            }
            else
            {
                const sf::View& view = target.getView();

                // Calculate the scale factor of the view
                float scaleViewX = target.getSize().x / view.getSize().x;
                float scaleViewY = target.getSize().y / view.getSize().y;

                // Get the global position
                sf::Vector2f topLeftPosition = states.transform.transformPoint(((m_textureRect.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                                               ((m_textureRect.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
                sf::Vector2f bottomRightPosition = states.transform.transformPoint((m_textureRect.left + m_textureRect.width - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                                                   (m_textureRect.top + m_textureRect.height - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

                // Get the old clipping area
                GLint scissor[4];
                glGetIntegerv(GL_SCISSOR_BOX, scissor);

                // Calculate the clipping area
                GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
                GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
                GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

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
