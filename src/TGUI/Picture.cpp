/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Picture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture()
    {
        m_Callback.widgetType = Type_Picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture(const Picture& copy) :
    ClickableWidget (copy),
    m_LoadedFilename(copy.m_LoadedFilename)
    {
        // Copy the texture
        TGUI_TextureManager.copyTexture(copy.m_Texture, m_Texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::~Picture()
    {
        // Remove the texture (if we are the only one using it)
        if (m_Texture.data != nullptr)
        {
            if (m_LoadedFilename != "")
                TGUI_TextureManager.removeTexture(m_Texture);
            else
                delete m_Texture.data;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture& Picture::operator= (const Picture& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Picture temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_LoadedFilename, temp.m_LoadedFilename);
            std::swap(m_Texture,        temp.m_Texture);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture* Picture::clone()
    {
        return new Picture(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::load(const std::string& filename)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;
        m_Size.x = 0;
        m_Size.y = 0;

        // Make sure that the filename isn't empty
        if (filename.empty())
            return false;

        // If we have already loaded a texture then first delete it
        if (m_Texture.data != nullptr)
        {
            if (m_LoadedFilename != "")
                TGUI_TextureManager.removeTexture(m_Texture);
            else
                delete m_Texture.data;
        }

        m_LoadedFilename = getResourcePath() + filename;

        // Try to load the texture from the file
        if (TGUI_TextureManager.getTexture(m_LoadedFilename, m_Texture))
        {
            m_Loaded = true;

            // Remember the size of the texture
            setSize(static_cast<float>(m_Texture.getSize().x),static_cast<float>( m_Texture.getSize().y));

            return true;
        }
        else // The texture was not loaded
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::loadFromTexture(const sf::Texture& texture)
    {
        m_LoadedFilename = "";

        if (m_Texture.data != nullptr)
            TGUI_TextureManager.removeTexture(m_Texture);

        m_Texture.data = new TextureData();
        m_Texture.data->texture = texture;
        m_Texture.data->rect = sf::IntRect(0, 0, m_Texture.getSize().x, m_Texture.getSize().y);
        m_Texture.data->users = 1;
        m_Texture.sprite.setTexture(m_Texture.data->texture, true);

        m_Loaded = true;
        setSize(static_cast<float>(m_Texture.getSize().x),static_cast<float>(m_Texture.getSize().y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Picture::getLoadedFilename() const
    {
        return m_LoadedFilename;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        m_Texture.sprite.setPosition(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSize(float width, float height)
    {
        m_Size.x = width;
        m_Size.y = height;

        if (m_Loaded)
            m_Texture.sprite.setScale(m_Size.x / m_Texture.getSize().x, m_Size.y / m_Texture.getSize().y);
        else
            TGUI_OUTPUT("TGUI warning: Picture::setSize called while Picture wasn't loaded yet.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSmooth(bool smooth)
    {
        if (m_Loaded)
            m_Texture.data->texture.setSmooth(smooth);
        else
            TGUI_OUTPUT("TGUI warning: Picture::setSmooth called while Picture wasn't loaded yet.");
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::isSmooth() const
    {
        if (m_Loaded)
            return m_Texture.data->texture.isSmooth();
        else
        {
            TGUI_OUTPUT("TGUI warning: Picture::isSmooth called while Picture wasn't loaded yet.");
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_Texture.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the image wasn't loaded
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the picture
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
        {
            sf::Vector2f scaling;
            scaling.x = m_Size.x / m_Texture.getSize().x;
            scaling.y = m_Size.y / m_Texture.getSize().y;

            // Only return true when the pixel under the mouse isn't transparent
            if (m_LoadedFilename != "")
            {
                if (!m_Texture.isTransparentPixel(static_cast<unsigned int>((x - getPosition().x) / scaling.x), static_cast<unsigned int>((y - getPosition().y) / scaling.y)))
                    return true;
            }
        }

        if (m_MouseHover == true)
            mouseLeftWidget();

        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "filename")
        {
            load(value);
        }
        else if (property == "smooth")
        {
            if ((value == "true") || (value == "True"))
                setSmooth(true);
            else if ((value == "false") || (value == "False"))
                setSmooth(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Smooth' property.");
        }
        else // The property didn't match
            return ClickableWidget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "filename")
            value = getLoadedFilename();
        else if (property == "smooth")
            value = isSmooth() ? "true" : "false";
        else // The property didn't match
            return ClickableWidget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Picture::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("Filename", "string"));
        list.push_back(std::pair<std::string, std::string>("Smooth", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_Texture, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
