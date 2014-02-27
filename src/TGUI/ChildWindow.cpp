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


#include <cmath>

#include <SFML/OpenGL.hpp>

#include <TGUI/Button.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ChildWindow.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow() :
    m_size             (0, 0),
    m_backgroundTexture(nullptr),
    m_titleBarHeight   (0),
    m_splitImage       (false),
    m_draggingPosition (0, 0),
    m_distanceToSide   (5),
    m_titleAlignment   (TitleAlignmentCentered),
    m_borderColor      (0, 0, 0),
    m_keepInParent     (false)
    {
        m_callback.widgetType = Type_ChildWindow;
        m_closeButton = new Button();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const ChildWindow& childWindowToCopy) :
    Container          (childWindowToCopy),
    WidgetBorders      (childWindowToCopy),
    m_loadedConfigFile (childWindowToCopy.m_loadedConfigFile),
    m_size             (childWindowToCopy.m_size),
    m_backgroundColor  (childWindowToCopy.m_backgroundColor),
    m_backgroundTexture(childWindowToCopy.m_backgroundTexture),
    m_titleText        (childWindowToCopy.m_titleText),
    m_titleBarHeight   (childWindowToCopy.m_titleBarHeight),
    m_splitImage       (childWindowToCopy.m_splitImage),
    m_draggingPosition (childWindowToCopy.m_draggingPosition),
    m_distanceToSide   (childWindowToCopy.m_distanceToSide),
    m_titleAlignment   (childWindowToCopy.m_titleAlignment),
    m_borderColor      (childWindowToCopy.m_borderColor),
    m_keepInParent     (childWindowToCopy.m_keepInParent)
    {
        // Copy the textures
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_iconTexture, m_iconTexture);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_textureTitleBar_L, m_textureTitleBar_L);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_textureTitleBar_M, m_textureTitleBar_M);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_textureTitleBar_R, m_textureTitleBar_R);

        // Copy the button
        m_closeButton = new Button(*childWindowToCopy.m_closeButton);

        // Set the bakground sprite, if there is a background texture
        if (childWindowToCopy.m_backgroundTexture)
        {
            m_backgroundSprite.setTexture(*m_backgroundTexture, true);
            m_backgroundSprite.setScale(m_size.x / m_backgroundTexture->getSize().x, m_size.y / m_backgroundTexture->getSize().y);
            m_backgroundSprite.setColor(sf::Color(255, 255, 255, m_opacity));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::~ChildWindow()
    {
        if (m_textureTitleBar_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTitleBar_L);
        if (m_textureTitleBar_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTitleBar_M);
        if (m_textureTitleBar_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTitleBar_R);

        if (m_iconTexture.data != nullptr)
            TGUI_TextureManager.removeTexture(m_iconTexture);

        if (m_iconTexture.data != nullptr)
            TGUI_TextureManager.removeTexture(m_iconTexture);

        delete m_closeButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow& ChildWindow::operator= (const ChildWindow& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            ChildWindow temp(right);
            this->Container::operator=(right);
            this->WidgetBorders::operator=(right);

            // Delete the old close button
            delete m_closeButton;

            std::swap(m_loadedConfigFile,  temp.m_loadedConfigFile);
            std::swap(m_size,              temp.m_size);
            std::swap(m_backgroundColor,   temp.m_backgroundColor);
            std::swap(m_backgroundTexture, temp.m_backgroundTexture);
            std::swap(m_backgroundSprite,  temp.m_backgroundSprite);
            std::swap(m_iconTexture,       temp.m_iconTexture);
            std::swap(m_titleText,         temp.m_titleText);
            std::swap(m_titleBarHeight,    temp.m_titleBarHeight);
            std::swap(m_splitImage,        temp.m_splitImage);
            std::swap(m_draggingPosition,  temp.m_draggingPosition);
            std::swap(m_distanceToSide,    temp.m_distanceToSide);
            std::swap(m_titleAlignment,    temp.m_titleAlignment);
            std::swap(m_borderColor,       temp.m_borderColor);
            std::swap(m_textureTitleBar_L, temp.m_textureTitleBar_L);
            std::swap(m_textureTitleBar_M, temp.m_textureTitleBar_M);
            std::swap(m_textureTitleBar_R, temp.m_textureTitleBar_R);
            std::swap(m_closeButton,       temp.m_closeButton);
            std::swap(m_keepInParent,      temp.m_keepInParent);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow* ChildWindow::clone()
    {
        return new ChildWindow(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Until the loading succeeds, the child window will be marked as unloaded
        m_loaded = false;

        // Remove the textures when they were loaded before
        if (m_textureTitleBar_L.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTitleBar_L);
        if (m_textureTitleBar_M.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTitleBar_M);
        if (m_textureTitleBar_R.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTitleBar_R);
        if (m_closeButton->m_textureNormal_M.data != nullptr) TGUI_TextureManager.removeTexture(m_closeButton->m_textureNormal_M);
        if (m_closeButton->m_textureHover_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_closeButton->m_textureHover_M);
        if (m_closeButton->m_textureDown_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_closeButton->m_textureDown_M);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_loadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_loadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("ChildWindow", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_loadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "backgroundcolor")
            {
                setBackgroundColor(configFile.readColor(value));
            }
            else if (property == "titlecolor")
            {
                setTitleColor(configFile.readColor(value));
            }
            else if (property == "bordercolor")
            {
                setBorderColor(configFile.readColor(value));
            }
            else if (property == "titlebarimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTitleBar_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage in section ChildWindow in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = false;
            }
            else if (property == "titlebarimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTitleBar_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage_L in section ChildWindow in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "titlebarimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTitleBar_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage_M in section ChildWindow in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = true;
            }
            else if (property == "titlebarimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTitleBar_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage_R in section ChildWindow in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "closebuttonseparatehoverimage")
            {
                m_closeButton->m_separateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "closebuttonnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_closeButton->m_textureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonNormalImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "closebuttonhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_closeButton->m_textureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonHoverImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "closebuttondownimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_closeButton->m_textureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonDownImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "distancetoside")
            {
                setDistanceToSide(std::stoul(value));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ChildWindow in " + m_loadedConfigFile + ".");
        }

        // Initialize the close button if it was loaded
        if (m_closeButton->m_textureNormal_M.data != nullptr)
        {
            // Check if optional textures were loaded
            if (m_closeButton->m_textureHover_M.data != nullptr)
            {
                m_closeButton->m_widgetPhase |= WidgetPhase_Hover;
            }
            if (m_closeButton->m_textureDown_M.data != nullptr)
            {
                m_closeButton->m_widgetPhase |= WidgetPhase_MouseDown;
            }

            m_closeButton->m_size = sf::Vector2f(m_closeButton->m_textureNormal_M.getSize());
            m_closeButton->m_loaded = true;
        }
        else // Close button wan't loaded
        {
            TGUI_OUTPUT("TGUI error: Missing a CloseButtonNormalImage property in section ChildWindow in " + m_loadedConfigFile + ".");
            return false;
        }

        // Check if the image is split
        if (m_splitImage)
        {
            // Make sure the required textures was loaded
            if ((m_textureTitleBar_L.data != nullptr) && (m_textureTitleBar_M.data != nullptr) && (m_textureTitleBar_R.data != nullptr))
            {
                m_titleBarHeight = m_textureTitleBar_M.getSize().y;

                float width = m_textureTitleBar_L.getSize().x + m_textureTitleBar_M.getSize().x + m_textureTitleBar_R.getSize().x;

                m_loaded = true;
                setSize(width, width * 3.0f / 4.0f);

                m_textureTitleBar_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the child window. Is the ChildWindow section in " + m_loadedConfigFile + " complete?");
                return false;
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_textureTitleBar_M.data != nullptr)
            {
                m_titleBarHeight = m_textureTitleBar_M.getSize().y;

                m_loaded = true;
                setSize(m_textureTitleBar_M.getSize().x, m_textureTitleBar_M.getSize().x * 3.0f / 4.0f);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the child window. Is the ChildWindow section in " + m_loadedConfigFile + " complete?");
                return false;
            }
        }

        // Set the size of the title text
        m_titleText.setCharacterSize(m_titleBarHeight * 8 / 10);

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ChildWindow::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(float width, float height)
    {
        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the window
        m_size.x = width;
        m_size.y = height;

        if (m_splitImage)
        {
            float scalingY = static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y;
            float minimumWidth = ((m_textureTitleBar_L.getSize().x + m_textureTitleBar_R.getSize().x) * scalingY);

            if (m_size.x + m_leftBorder + m_rightBorder < minimumWidth)
                m_size.x = minimumWidth - m_leftBorder - m_rightBorder;

            m_textureTitleBar_L.sprite.setScale(scalingY, scalingY);
            m_textureTitleBar_M.sprite.setScale(scalingY, scalingY);
            m_textureTitleBar_R.sprite.setScale(scalingY, scalingY);

            m_textureTitleBar_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(((m_size.x + m_leftBorder + m_rightBorder) - minimumWidth) / scalingY), m_textureTitleBar_M.getSize().y));
        }
        else // The image is not split
        {
            m_textureTitleBar_M.sprite.setScale((m_size.x + m_leftBorder + m_rightBorder) / m_textureTitleBar_M.getSize().x,
                                                static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y);
        }

        // If there is a background texture then resize it
        if (m_backgroundTexture)
            m_backgroundSprite.setScale(m_size.x / m_backgroundTexture->getSize().x, m_size.y / m_backgroundTexture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getSize() const
    {
        return sf::Vector2f(m_size.x, m_size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        return sf::Vector2f(m_size.x + m_leftBorder + m_rightBorder,
                            m_size.y + m_topBorder + m_bottomBorder + m_titleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBackgroundTexture(sf::Texture *const texture)
    {
        // Store the texture
        m_backgroundTexture = texture;

        // Set the texture for the sprite
        if (m_backgroundTexture)
        {
            m_backgroundSprite.setTexture(*m_backgroundTexture, true);
            m_backgroundSprite.setScale(m_size.x / m_backgroundTexture->getSize().x, m_size.y / m_backgroundTexture->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Texture* ChildWindow::getBackgroundTexture()
    {
        return m_backgroundTexture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleBarHeight(unsigned int height)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_loaded == false)
            return;

        // Remember the new title bar height
        m_titleBarHeight = height;

        // Set the size of the close button
        m_closeButton->setSize(static_cast<float>(height) / m_textureTitleBar_M.getSize().y * m_closeButton->m_textureNormal_M.getSize().x,
                               static_cast<float>(height) / m_textureTitleBar_M.getSize().y * m_closeButton->m_textureNormal_M.getSize().y);

        // Set the size of the text in the title bar
        m_titleText.setCharacterSize(m_titleBarHeight * 8 / 10);

        // Recalculate the scale of the title bar images
        if (m_splitImage)
        {
            float scalingY = static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y;
            float minimumWidth = ((m_textureTitleBar_L.getSize().x + m_textureTitleBar_R.getSize().x) * scalingY);

            if (m_size.x + m_leftBorder + m_rightBorder < minimumWidth)
                m_size.x = minimumWidth - m_leftBorder - m_rightBorder;

            m_textureTitleBar_L.sprite.setScale(scalingY, scalingY);
            m_textureTitleBar_M.sprite.setScale(scalingY, scalingY);
            m_textureTitleBar_R.sprite.setScale(scalingY, scalingY);

            m_textureTitleBar_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(((m_size.x + m_leftBorder + m_rightBorder) - minimumWidth) / scalingY), m_textureTitleBar_M.getSize().y));
        }
        else // The image is not split
        {
            m_textureTitleBar_M.sprite.setScale((m_size.x + m_leftBorder + m_rightBorder) / m_textureTitleBar_M.getSize().x,
                                                static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleBarHeight() const
    {
        return m_titleBarHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChildWindow::getBackgroundColor() const
    {
        return m_backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTransparency(unsigned char transparency)
    {
        Container::setTransparency(transparency);

        m_backgroundSprite.setColor(sf::Color(255, 255, 255, m_opacity));

        m_iconTexture.sprite.setColor(sf::Color(255, 255, 255, m_opacity));

        m_textureTitleBar_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTitleBar_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTitleBar_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));

        m_closeButton->setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setString(title);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ChildWindow::getTitle() const
    {
        return m_titleText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleColor(const sf::Color& color)
    {
        m_titleText.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChildWindow::getTitleColor() const
    {
        return m_titleText.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBorderColor(const sf::Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChildWindow::getBorderColor() const
    {
        return m_borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        // Set the new border size
        m_leftBorder   = leftBorder;
        m_topBorder    = topBorder;
        m_rightBorder  = rightBorder;
        m_bottomBorder = bottomBorder;

        // Recalculate the scale of the title bar images
        if (m_splitImage)
        {
            float scalingY = static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y;
            float minimumWidth = ((m_textureTitleBar_L.getSize().x + m_textureTitleBar_R.getSize().x) * scalingY);

            if (m_size.x + m_leftBorder + m_rightBorder < minimumWidth)
                m_size.x = minimumWidth - m_leftBorder - m_rightBorder;

            m_textureTitleBar_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(((m_size.x + m_leftBorder + m_rightBorder) - minimumWidth) / scalingY), m_textureTitleBar_M.getSize().y));
        }
        else // The image is not split
        {
            m_textureTitleBar_M.sprite.setScale((m_size.x + m_leftBorder + m_rightBorder) / m_textureTitleBar_M.getSize().x,
                                                static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getDistanceToSide() const
    {
        return m_distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::TitleAlignment ChildWindow::getTitleAlignment() const
    {
        return m_titleAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setIcon(const std::string& filename)
    {
        // If a texture has already been loaded then remove it first
        if (m_iconTexture.data)
            TGUI_TextureManager.removeTexture(m_iconTexture);

        // Load the icon image
        if (TGUI_TextureManager.getTexture(filename, m_iconTexture))
        {
            m_iconTexture.sprite.setScale(static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y,
                                          static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y);
        }
        else // Loading failed
            TGUI_OUTPUT("Failed to load \"" + filename + "\" as icon for the ChildWindow");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::removeIcon()
    {
        if (m_iconTexture.data)
            TGUI_TextureManager.removeTexture(m_iconTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        m_parent->remove(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::keepInParent(bool enabled)
    {
        m_keepInParent = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent()
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(float x, float y)
    {
        if (m_keepInParent)
        {
            if (y < 0)
                Transformable::setPosition(getPosition().x, 0);
            else if (y > m_parent->getSize().y - m_titleBarHeight)
                Transformable::setPosition(getPosition().x, m_parent->getSize().y - m_titleBarHeight);
            else
                Transformable::setPosition(getPosition().x, y);

            if (x < 0)
                Transformable::setPosition(0, getPosition().y);
            else if (x > m_parent->getSize().x - getSize().x)
                Transformable::setPosition(m_parent->getSize().x - getSize().x, getPosition().y);
            else
                Transformable::setPosition(x, getPosition().y);
        }
        else
            Transformable::setPosition(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(float x, float y)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_loaded == false)
            return false;

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNotOnWidget();

            return true;
        }
        else
        {
            // Check if the mouse is inside the child window
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, m_size.y + m_topBorder + m_bottomBorder)).contains(x, y - m_titleBarHeight))
                return true;
            else
            {
                if (m_mouseHover)
                {
                    mouseLeftWidget();

                    // Tell the widgets inside the child window that the mouse is no longer on top of them
                    for (unsigned int i = 0; i < m_widgets.size(); ++i)
                        m_widgets[i]->mouseNotOnWidget();

                    m_closeButton->mouseNotOnWidget();
                    m_mouseHover = false;
                }

                return false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(float x, float y)
    {
        // Move the childwindow to the front
        m_parent->moveWidgetToFront(this);

        // Add the callback (if the user requested it)
        if (m_callbackFunctions[LeftMousePressed].empty() == false)
        {
            m_callback.trigger = LeftMousePressed;
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_closeButton->setPosition(position.x + ((m_size.x + m_leftBorder + m_rightBorder - m_distanceToSide - m_closeButton->getSize().x)), position.y + ((m_titleBarHeight / 2.f) - (m_closeButton->getSize().x / 2.f)));

            // Send the mouse press event to the close button
            if (m_closeButton->mouseOnWidget(x, y))
                m_closeButton->leftMousePressed(x, y);
            else
            {
                // The mouse went down on the title bar
                m_mouseDown = true;

                // Remember where we are dragging the title bar
                m_draggingPosition.x = x - position.x;
                m_draggingPosition.y = y - position.y;
            }

            // Reset the position of the button
            m_closeButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, m_size.y + m_topBorder + m_bottomBorder + m_titleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_leftBorder), static_cast<float>(m_titleBarHeight + m_topBorder), m_size.x, m_size.y)).contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMousePressed(x - m_leftBorder, y - (m_titleBarHeight + m_topBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(float x , float y)
    {
        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_closeButton->setPosition(position.x + ((m_size.x + m_leftBorder + m_rightBorder - m_distanceToSide - m_closeButton->getSize().x)), position.y + ((m_titleBarHeight / 2.f) - (m_closeButton->getSize().x / 2.f)));

            m_mouseDown = false;

            // Check if the close button was clicked
            if (m_closeButton->m_mouseDown == true)
            {
                m_closeButton->m_mouseDown = false;

                // Check if the mouse is still on the close button
                if (m_closeButton->mouseOnWidget(x, y))
                {
                    // If a callback was requested then send it
                    if (m_callbackFunctions[Closed].empty() == false)
                    {
                        m_callback.trigger = Closed;
                        addCallback();
                    }
                    else // The user won't stop the closing, so destroy the window
                    {
                        destroy();
                        return;
                    }
                }
            }

            // Tell the widgets that the mouse is no longer down
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNoLongerDown();

            // Reset the position of the button
            m_closeButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNotOnWidget();

            // Change the mouse down flag
            m_mouseDown = false;
            m_closeButton->mouseNoLongerDown();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, m_size.y + m_topBorder + m_bottomBorder + m_titleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_leftBorder), static_cast<float>(m_titleBarHeight + m_topBorder), m_size.x, m_size.y)).contains(x, y) == false))
            {
                // Tell the widgets about that the mouse was released
                for (unsigned int i = 0; i < m_widgets.size(); ++i)
                    m_widgets[i]->mouseNoLongerDown();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMouseReleased(x - m_leftBorder, y - (m_titleBarHeight + m_topBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(float x, float y)
    {
        m_mouseHover = true;

        // Check if you are dragging the child window
        if (m_mouseDown == true)
        {
            // Move the child window
            sf::Vector2f position = getPosition();
            setPosition(position.x + (x - position.x - m_draggingPosition.x), position.y + (y - position.y - m_draggingPosition.y));

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[Moved].empty() == false)
            {
                m_callback.trigger = Moved;
                m_callback.position = getPosition();
                addCallback();
            }
        }

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_closeButton->setPosition(position.x + ((m_size.x + m_leftBorder + m_rightBorder - m_distanceToSide - m_closeButton->getSize().x)), position.y + ((m_titleBarHeight / 2.f) - (m_closeButton->getSize().x / 2.f)));

            // Send the hover event to the close button
            if (m_closeButton->mouseOnWidget(x, y))
                m_closeButton->mouseMoved(x, y);

            // Reset the position of the button
            m_closeButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_leftBorder + m_rightBorder, m_size.y + m_topBorder + m_bottomBorder + m_titleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_leftBorder), static_cast<float>(m_titleBarHeight + m_topBorder), m_size.x, m_size.y)).contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved(x - m_leftBorder, y - (m_titleBarHeight + m_topBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        Container::mouseWheelMoved(delta, x - m_leftBorder, y - (m_titleBarHeight + m_topBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();
        m_closeButton->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "titlebarheight")
        {
            setTitleBarHeight(std::stoi(value));
        }
        else if (property == "backgroundcolor")
        {
            setBackgroundColor(extractColor(value));
        }
        else if (property == "title")
        {
            setTitle(value);
        }
        else if (property == "titlecolor")
        {
            setTitleColor(extractColor(value));
        }
        else if (property == "bordercolor")
        {
            setBorderColor(extractColor(value));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders.left, borders.top, borders.right, borders.bottom);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(std::stoi(value));
        }
        else if (property == "titlealignment")
        {
            if ((value == "left") || (value == "Left"))
                setTitleAlignment(TitleAlignmentLeft);
            else if ((value == "centered") || (value == "Centered"))
                setTitleAlignment(TitleAlignmentCentered);
            else if ((value == "right") || (value == "Right"))
                setTitleAlignment(TitleAlignmentRight);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'TitleAlignment' property.");
        }
        else if (property == "callback")
        {
            Container::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "LeftMousePressed") || (*it == "leftmousepressed"))
                    bindCallback(LeftMousePressed);
                else if ((*it == "Closed") || (*it == "closed"))
                    bindCallback(Closed);
                else if ((*it == "Moved") || (*it == "moved"))
                    bindCallback(Moved);
            }
        }
        else // The property didn't match
            return Container::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "titlebarheight")
            value = std::to_string(getTitleBarHeight());
        else if (property == "backgroundcolor")
            value = "(" + std::to_string(int(getBackgroundColor().r)) + "," + std::to_string(int(getBackgroundColor().g)) + "," + std::to_string(int(getBackgroundColor().b)) + "," + std::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "title")
            value = getTitle().toAnsiString();
        else if (property == "titlecolor")
            value = "(" + std::to_string(int(getTitleColor().r)) + "," + std::to_string(int(getTitleColor().g)) + "," + std::to_string(int(getTitleColor().b)) + "," + std::to_string(int(getTitleColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + std::to_string(int(getBorderColor().r)) + "," + std::to_string(int(getBorderColor().g)) + "," + std::to_string(int(getBorderColor().b)) + "," + std::to_string(int(getBorderColor().a)) + ")";
        else if (property == "borders")
            value = "(" + std::to_string(getBorders().left) + "," + std::to_string(getBorders().top) + "," + std::to_string(getBorders().right) + "," + std::to_string(getBorders().bottom) + ")";
        else if (property == "distancetoside")
            value = std::to_string(getDistanceToSide());
        else if (property == "titlealignment")
        {
            if (m_titleAlignment == TitleAlignmentLeft)
                value = "Left";
            else if (m_titleAlignment == TitleAlignmentCentered)
                value = "Centered";
            else if (m_titleAlignment == TitleAlignmentRight)
                value = "Right";
        }
        else if (property == "callback")
        {
            std::string tempValue;
            Container::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(LeftMousePressed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMousePressed).size() == 1) && (m_callbackFunctions.at(LeftMousePressed).front() == nullptr))
                callbacks.push_back("LeftMousePressed");
            if ((m_callbackFunctions.find(Closed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(Closed).size() == 1) && (m_callbackFunctions.at(Closed).front() == nullptr))
                callbacks.push_back("Closed");
            if ((m_callbackFunctions.find(Moved) != m_callbackFunctions.end()) && (m_callbackFunctions.at(Moved).size() == 1) && (m_callbackFunctions.at(Moved).front() == nullptr))
                callbacks.push_back("Moved");

            encodeList(callbacks, value);

            if (value.empty() || tempValue.empty())
                value += tempValue;
            else
                value += "," + tempValue;
        }
        else // The property didn't match
            return Container::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > ChildWindow::getPropertyList() const
    {
        auto list = Container::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("TitleBarHeight", "uint"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("Title", "string"));
        list.push_back(std::pair<std::string, std::string>("TitleColor", "color"));
        list.push_back(std::pair<std::string, std::string>("BorderColor", "color"));
        list.push_back(std::pair<std::string, std::string>("Borders", "borders"));
        list.push_back(std::pair<std::string, std::string>("DistanceToSide", "uint"));
        list.push_back(std::pair<std::string, std::string>("TitleAlignment", "custom"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::initialize(Container *const parent)
    {
        m_parent = parent;
        setGlobalFont(m_parent->getGlobalFont());
        m_titleText.setFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the child window wasn't created
        if (m_loaded == false)
            return;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        sf::Vector2f viewPosition = (target.getView().getSize() / 2.f) - target.getView().getCenter();

        // Get the global position
        sf::Vector2f topLeftPanelPosition = states.transform.transformPoint(position.x + m_leftBorder + viewPosition.x,
                                                                            position.y + m_titleBarHeight + m_topBorder + viewPosition.y);
        sf::Vector2f bottomRightPanelPosition = states.transform.transformPoint(position.x + m_size.x + m_leftBorder + viewPosition.x,
                                                                                position.y + m_titleBarHeight + m_size.y + m_topBorder + viewPosition.y);
        sf::Vector2f topLeftTitleBarPosition;
        sf::Vector2f bottomRightTitleBarPosition;

        if (m_iconTexture.data)
            topLeftTitleBarPosition = states.transform.transformPoint(position.x + 2*m_distanceToSide + (m_iconTexture.getSize().x * m_iconTexture.sprite.getScale().x) + viewPosition.x,
                                                                      position.y + viewPosition.y);
        else
            topLeftTitleBarPosition = states.transform.transformPoint(position.x + m_distanceToSide + viewPosition.x, position.y + viewPosition.y);

        bottomRightTitleBarPosition = states.transform.transformPoint(position.x + m_size.x + m_leftBorder + m_rightBorder - (2*m_distanceToSide) - m_closeButton->getSize().x + viewPosition.x,
                                                                      position.y + m_titleBarHeight + viewPosition.y);

        // Adjust the transformation
        states.transform *= getTransform();

        sf::Transform oldTransform = states.transform;

        // Check if the title bar image is split
        if (m_splitImage)
        {
            target.draw(m_textureTitleBar_L, states);

            states.transform.translate(m_textureTitleBar_L.getSize().x * (static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y), 0);
            target.draw(m_textureTitleBar_M, states);

            states.transform.translate(m_size.x + m_leftBorder + m_rightBorder - ((m_textureTitleBar_R.getSize().x + m_textureTitleBar_L.getSize().x)
                                                                                  * (static_cast<float>(m_titleBarHeight) / m_textureTitleBar_M.getSize().y)), 0);
            target.draw(m_textureTitleBar_R, states);
        }
        else // The title bar image isn't split
        {
            // Draw the title bar
            target.draw(m_textureTitleBar_M, states);
        }

        states.transform = oldTransform;

        // Draw a window icon if one was set
        if (m_iconTexture.data)
        {
            states.transform.translate(static_cast<float>(m_distanceToSide), (m_titleBarHeight - (m_iconTexture.getSize().y * m_iconTexture.sprite.getScale().y)) / 2.f);
            target.draw(m_iconTexture, states);
            states.transform.translate(m_iconTexture.getSize().x * m_iconTexture.sprite.getScale().x, (m_titleBarHeight - (m_iconTexture.getSize().y * m_iconTexture.sprite.getScale().y)) / -2.f);
        }

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Check if there is a title
        if (m_titleText.getString().isEmpty() == false)
        {
            // Calculate the clipping area
            GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftTitleBarPosition.x * scaleViewX), scissor[0]);
            GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftTitleBarPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
            GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightTitleBarPosition.x * scaleViewX), scissor[0] + scissor[2]);
            GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightTitleBarPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

            // If the widget outside the window then don't draw anything
            if (scissorRight < scissorLeft)
                scissorRight = scissorLeft;
            else if (scissorBottom < scissorTop)
                scissorTop = scissorBottom;

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            // Draw the text, depending on the alignment
            if (m_titleAlignment == TitleAlignmentLeft)
            {
                states.transform.translate(std::floor(static_cast<float>(m_distanceToSide) + 0.5f), 0);
                target.draw(m_titleText, states);
            }
            else if (m_titleAlignment == TitleAlignmentCentered)
            {
                if (m_iconTexture.data)
                    states.transform.translate(std::floor(m_distanceToSide + (((m_size.x + m_leftBorder + m_rightBorder) - 4*m_distanceToSide - (m_iconTexture.getSize().x * m_iconTexture.sprite.getScale().x) - m_closeButton->getSize().x - m_titleText.getGlobalBounds().width) / 2.0f) + 0.5f), 0);
                else
                    states.transform.translate(std::floor(m_distanceToSide + (((m_size.x + m_leftBorder + m_rightBorder) - 3*m_distanceToSide - m_closeButton->getSize().x - m_titleText.getGlobalBounds().width) / 2.0f) + 0.5f), 0);

                target.draw(m_titleText, states);
            }
            else // if (m_titleAlignment == TitleAlignmentRight)
            {
                if (m_iconTexture.data)
                    states.transform.translate(std::floor((m_size.x + m_leftBorder + m_rightBorder) - (m_iconTexture.getSize().x * m_iconTexture.sprite.getScale().x) - 3*m_distanceToSide - m_closeButton->getSize().x - m_titleText.getGlobalBounds().width + 0.5f), 0);
                else
                    states.transform.translate(std::floor((m_size.x + m_leftBorder + m_rightBorder) - 2*m_distanceToSide - m_closeButton->getSize().x - m_titleText.getGlobalBounds().width + 0.5f), 0);

                target.draw(m_titleText, states);
            }

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }

        // Move the close button to the correct position
        states.transform = oldTransform;
        states.transform.translate((m_size.x + m_leftBorder + m_rightBorder) - m_distanceToSide - m_closeButton->getSize().x, (m_titleBarHeight - m_closeButton->getSize().y) / 2.f);

        // Draw the close button
        target.draw(*m_closeButton, states);

        // Set the correct transformation
        states.transform = oldTransform.translate(0, static_cast<float>(m_titleBarHeight));

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_leftBorder), m_size.y + m_topBorder));
        border.setFillColor(m_borderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(m_size.x + m_rightBorder, static_cast<float>(m_topBorder)));
        target.draw(border, states);

        // Draw right border
        border.setSize(sf::Vector2f(static_cast<float>(m_rightBorder), m_size.y + m_bottomBorder));
        border.setPosition(m_size.x + m_leftBorder, 0);
        target.draw(border, states);

        // Draw bottom border
        border.setSize(sf::Vector2f(m_size.x + m_leftBorder, static_cast<float>(m_bottomBorder)));
        border.setPosition(0, m_size.y + m_topBorder);
        target.draw(border, states);

        // Make room for the borders
        states.transform.translate(static_cast<float>(m_leftBorder), static_cast<float>(m_topBorder));

        // Draw the background
        if (m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(sf::Vector2f(m_size.x, m_size.y));
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }

        // Draw the background image if there is one
        if (m_backgroundTexture != nullptr)
            target.draw(m_backgroundSprite, states);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPanelPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPanelPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPanelPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPanelPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the widgets in the child window
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
