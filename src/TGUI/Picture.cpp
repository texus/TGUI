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


#include <TGUI/Picture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture()
    {
        m_callback.widgetType = Type_Picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::create(const std::string& filename, bool fullyClickable)
    {
        auto picture = std::make_shared<Picture>();

        picture->m_fullyClickable = fullyClickable;
        picture->m_loadedFilename = getResourcePath() + filename;

        // Try to load the texture from the file
        TGUI_TextureManager.getTexture(picture->m_texture, picture->m_loadedFilename);

        // Remember the size of the texture
        picture->setSize(picture->m_texture.getImageSize());

        return picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        m_texture.setPosition(position.getValue());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSize(const Layout& size)
    {
        Widget::setSize(size);

        m_texture.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_texture.setColor({255, 255, 255, m_opacity});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the picture
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
        {
            // We sometimes want clicks to go through transparent parts of the picture
            if (!m_fullyClickable)
            {
                if (!m_texture.isTransparentPixel(x, y))
                    return true;
            }
        }

        if (m_mouseHover == true)
            mouseLeftWidget();

        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void Picture::setProperty(std::string property, const std::string& value)
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
                throw Exception{"Failed to parse 'Smooth' property."};
        }
        else // The property didn't match
            ClickableWidget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "filename")
            value = getLoadedFilename();
        else if (property == "smooth")
            value = isSmooth() ? "true" : "false";
        else // The property didn't match
            ClickableWidget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Picture::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("Filename", "string"));
        list.push_back(std::pair<std::string, std::string>("Smooth", "bool"));
        return list;
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_texture, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
