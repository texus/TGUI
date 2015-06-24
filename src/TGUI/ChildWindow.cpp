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
    m_Size               (0, 0),
    m_BackgroundTexture  (nullptr),
    m_TitleBarHeight     (0),
    m_SplitImage         (false),
    m_DraggingPosition   (0, 0),
    m_DistanceToSide     (5),
    m_TitleAlignment     (TitleAlignmentCentered),
    m_BorderColor        (0, 0, 0),
    m_MouseDownOnTitleBar(false),
    m_KeepInParent       (false)
    {
        m_Callback.widgetType = Type_ChildWindow;
        m_CloseButton = new Button();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const ChildWindow& childWindowToCopy) :
    Container            (childWindowToCopy),
    WidgetBorders        (childWindowToCopy),
    m_LoadedConfigFile   (childWindowToCopy.m_LoadedConfigFile),
    m_Size               (childWindowToCopy.m_Size),
    m_BackgroundColor    (childWindowToCopy.m_BackgroundColor),
    m_BackgroundTexture  (childWindowToCopy.m_BackgroundTexture),
    m_TitleText          (childWindowToCopy.m_TitleText),
    m_TitleBarHeight     (childWindowToCopy.m_TitleBarHeight),
    m_SplitImage         (childWindowToCopy.m_SplitImage),
    m_DraggingPosition   (childWindowToCopy.m_DraggingPosition),
    m_DistanceToSide     (childWindowToCopy.m_DistanceToSide),
    m_TitleAlignment     (childWindowToCopy.m_TitleAlignment),
    m_BorderColor        (childWindowToCopy.m_BorderColor),
    m_MouseDownOnTitleBar(childWindowToCopy.m_MouseDownOnTitleBar),
    m_KeepInParent       (childWindowToCopy.m_KeepInParent)
    {
        // Copy the textures
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_IconTexture, m_IconTexture);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_TextureTitleBar_L, m_TextureTitleBar_L);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_TextureTitleBar_M, m_TextureTitleBar_M);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_TextureTitleBar_R, m_TextureTitleBar_R);

        // Copy the button
        m_CloseButton = new Button(*childWindowToCopy.m_CloseButton);

        // Set the bakground sprite, if there is a background texture
        if (childWindowToCopy.m_BackgroundTexture)
        {
            m_BackgroundSprite.setTexture(*m_BackgroundTexture, true);
            m_BackgroundSprite.setScale(m_Size.x / m_BackgroundTexture->getSize().x, m_Size.y / m_BackgroundTexture->getSize().y);
            m_BackgroundSprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::~ChildWindow()
    {
        if (m_TextureTitleBar_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTitleBar_L);
        if (m_TextureTitleBar_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTitleBar_M);
        if (m_TextureTitleBar_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTitleBar_R);

        if (m_IconTexture.data != nullptr)
            TGUI_TextureManager.removeTexture(m_IconTexture);

        if (m_IconTexture.data != nullptr)
            TGUI_TextureManager.removeTexture(m_IconTexture);

        delete m_CloseButton;
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
            delete m_CloseButton;

            std::swap(m_LoadedConfigFile,    temp.m_LoadedConfigFile);
            std::swap(m_Size,                temp.m_Size);
            std::swap(m_BackgroundColor,     temp.m_BackgroundColor);
            std::swap(m_BackgroundTexture,   temp.m_BackgroundTexture);
            std::swap(m_BackgroundSprite,    temp.m_BackgroundSprite);
            std::swap(m_IconTexture,         temp.m_IconTexture);
            std::swap(m_TitleText,           temp.m_TitleText);
            std::swap(m_TitleBarHeight,      temp.m_TitleBarHeight);
            std::swap(m_SplitImage,          temp.m_SplitImage);
            std::swap(m_DraggingPosition,    temp.m_DraggingPosition);
            std::swap(m_DistanceToSide,      temp.m_DistanceToSide);
            std::swap(m_TitleAlignment,      temp.m_TitleAlignment);
            std::swap(m_BorderColor,         temp.m_BorderColor);
            std::swap(m_MouseDownOnTitleBar, temp.m_MouseDownOnTitleBar);
            std::swap(m_TextureTitleBar_L,   temp.m_TextureTitleBar_L);
            std::swap(m_TextureTitleBar_M,   temp.m_TextureTitleBar_M);
            std::swap(m_TextureTitleBar_R,   temp.m_TextureTitleBar_R);
            std::swap(m_CloseButton,         temp.m_CloseButton);
            std::swap(m_KeepInParent,        temp.m_KeepInParent);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow* ChildWindow::clone()
    {
        return new ChildWindow(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // Until the loading succeeds, the child window will be marked as unloaded
        m_Loaded = false;

        // Remove the textures when they were loaded before
        if (m_TextureTitleBar_L.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTitleBar_L);
        if (m_TextureTitleBar_M.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTitleBar_M);
        if (m_TextureTitleBar_R.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTitleBar_R);
        if (m_CloseButton->m_TextureNormal_M.data != nullptr) TGUI_TextureManager.removeTexture(m_CloseButton->m_TextureNormal_M);
        if (m_CloseButton->m_TextureHover_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_CloseButton->m_TextureHover_M);
        if (m_CloseButton->m_TextureDown_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_CloseButton->m_TextureDown_M);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_LoadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read(sectionName, properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_LoadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_LoadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_LoadedConfigFile.substr(0, slashPos+1);

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
                if (!configFile.readTexture(value, configFileFolder, m_TextureTitleBar_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage in section ChildWindow in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "titlebarimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTitleBar_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage_L in section ChildWindow in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "titlebarimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTitleBar_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage_M in section ChildWindow in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "titlebarimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTitleBar_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage_R in section ChildWindow in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "closebuttonseparatehoverimage")
            {
                m_CloseButton->m_SeparateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "closebuttonnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_CloseButton->m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonNormalImage in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "closebuttonhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_CloseButton->m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonHoverImage in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "closebuttondownimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_CloseButton->m_TextureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonDownImage in section Button in " + m_LoadedConfigFile + ".");
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
                setDistanceToSide(static_cast<unsigned int>(atoi(value.c_str())));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ChildWindow in " + m_LoadedConfigFile + ".");
        }

        // Initialize the close button if it was loaded
        if (m_CloseButton->m_TextureNormal_M.data != nullptr)
        {
            // Check if optional textures were loaded
            if (m_CloseButton->m_TextureHover_M.data != nullptr)
            {
                m_CloseButton->m_WidgetPhase |= WidgetPhase_Hover;
            }
            if (m_CloseButton->m_TextureDown_M.data != nullptr)
            {
                m_CloseButton->m_WidgetPhase |= WidgetPhase_MouseDown;
            }

            m_CloseButton->m_Size = sf::Vector2f(m_CloseButton->m_TextureNormal_M.getSize());
            m_CloseButton->m_Loaded = true;
        }
        else // Close button wan't loaded
        {
            TGUI_OUTPUT("TGUI error: Missing a CloseButtonNormalImage property in section ChildWindow in " + m_LoadedConfigFile + ".");
            return false;
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures was loaded
            if ((m_TextureTitleBar_L.data != nullptr) && (m_TextureTitleBar_M.data != nullptr) && (m_TextureTitleBar_R.data != nullptr))
            {
                m_TitleBarHeight = m_TextureTitleBar_M.getSize().y;

                float width = static_cast<float>(m_TextureTitleBar_L.getSize().x + m_TextureTitleBar_M.getSize().x + m_TextureTitleBar_R.getSize().x);

                m_Loaded = true;
                setSize(width, width * 3.0f / 4.0f);

                m_TextureTitleBar_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the child window. Is the ChildWindow section in " + m_LoadedConfigFile + " complete?");
                return false;
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_TextureTitleBar_M.data != nullptr)
            {
                m_TitleBarHeight = m_TextureTitleBar_M.getSize().y;

                m_Loaded = true;
                setSize(static_cast<float>(m_TextureTitleBar_M.getSize().x), m_TextureTitleBar_M.getSize().x * 3.0f / 4.0f);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the child window. Is the ChildWindow section in " + m_LoadedConfigFile + " complete?");
                return false;
            }
        }

        // Set the size of the title text
        m_TitleText.setCharacterSize(m_TitleBarHeight * 8 / 10);

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ChildWindow::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(float width, float height)
    {
        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the window
        m_Size.x = width;
        m_Size.y = height;

        if (m_SplitImage)
        {
            float scalingY = static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y;
            float minimumWidth = ((m_TextureTitleBar_L.getSize().x + m_TextureTitleBar_R.getSize().x) * scalingY);

            if (m_Size.x + m_LeftBorder + m_RightBorder < minimumWidth)
                m_Size.x = minimumWidth - m_LeftBorder - m_RightBorder;

            m_TextureTitleBar_L.sprite.setScale(scalingY, scalingY);
            m_TextureTitleBar_M.sprite.setScale(scalingY, scalingY);
            m_TextureTitleBar_R.sprite.setScale(scalingY, scalingY);

            m_TextureTitleBar_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(((m_Size.x + m_LeftBorder + m_RightBorder) - minimumWidth) / scalingY), m_TextureTitleBar_M.getSize().y));
        }
        else // The image is not split
        {
            m_TextureTitleBar_M.sprite.setScale((m_Size.x + m_LeftBorder + m_RightBorder) / m_TextureTitleBar_M.getSize().x,
                                                static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y);
        }

        // If there is a background texture then resize it
        if (m_BackgroundTexture)
            m_BackgroundSprite.setScale(m_Size.x / m_BackgroundTexture->getSize().x, m_Size.y / m_BackgroundTexture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getSize() const
    {
        return sf::Vector2f(m_Size.x, m_Size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        return sf::Vector2f(m_Size.x + m_LeftBorder + m_RightBorder,
                            m_Size.y + m_TopBorder + m_BottomBorder + m_TitleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBackgroundTexture(sf::Texture *const texture)
    {
        // Store the texture
        m_BackgroundTexture = texture;

        // Set the texture for the sprite
        if (m_BackgroundTexture)
        {
            m_BackgroundSprite.setTexture(*m_BackgroundTexture, true);
            m_BackgroundSprite.setScale(m_Size.x / m_BackgroundTexture->getSize().x, m_Size.y / m_BackgroundTexture->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Texture* ChildWindow::getBackgroundTexture()
    {
        return m_BackgroundTexture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleBarHeight(unsigned int height)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_Loaded == false)
            return;

        // Remember the new title bar height
        m_TitleBarHeight = height;

        // Set the size of the close button
        m_CloseButton->setSize(static_cast<float>(height) / m_TextureTitleBar_M.getSize().y * m_CloseButton->m_TextureNormal_M.getSize().x,
                               static_cast<float>(height) / m_TextureTitleBar_M.getSize().y * m_CloseButton->m_TextureNormal_M.getSize().y);

        // Set the size of the text in the title bar
        m_TitleText.setCharacterSize(static_cast<unsigned int>(m_TitleBarHeight * 0.75f));

        // Recalculate the scale of the title bar images
        if (m_SplitImage)
        {
            float scalingY = static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y;
            float minimumWidth = ((m_TextureTitleBar_L.getSize().x + m_TextureTitleBar_R.getSize().x) * scalingY);

            if (m_Size.x + m_LeftBorder + m_RightBorder < minimumWidth)
                m_Size.x = minimumWidth - m_LeftBorder - m_RightBorder;

            m_TextureTitleBar_L.sprite.setScale(scalingY, scalingY);
            m_TextureTitleBar_M.sprite.setScale(scalingY, scalingY);
            m_TextureTitleBar_R.sprite.setScale(scalingY, scalingY);

            m_TextureTitleBar_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(((m_Size.x + m_LeftBorder + m_RightBorder) - minimumWidth) / scalingY), m_TextureTitleBar_M.getSize().y));
        }
        else // The image is not split
        {
            m_TextureTitleBar_M.sprite.setScale((m_Size.x + m_LeftBorder + m_RightBorder) / m_TextureTitleBar_M.getSize().x,
                                                static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleBarHeight() const
    {
        return m_TitleBarHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChildWindow::getBackgroundColor() const
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTransparency(unsigned char transparency)
    {
        Container::setTransparency(transparency);

        m_BackgroundSprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_IconTexture.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_TextureTitleBar_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTitleBar_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTitleBar_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_CloseButton->setTransparency(m_Opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_TitleText.setString(title);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ChildWindow::getTitle() const
    {
        return m_TitleText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleColor(const sf::Color& color)
    {
        m_TitleText.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChildWindow::getTitleColor() const
    {
        return m_TitleText.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChildWindow::getBorderColor() const
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;

        // Recalculate the scale of the title bar images
        if (m_SplitImage)
        {
            float scalingY = static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y;
            float minimumWidth = ((m_TextureTitleBar_L.getSize().x + m_TextureTitleBar_R.getSize().x) * scalingY);

            if (m_Size.x + m_LeftBorder + m_RightBorder < minimumWidth)
                m_Size.x = minimumWidth - m_LeftBorder - m_RightBorder;

            m_TextureTitleBar_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(((m_Size.x + m_LeftBorder + m_RightBorder) - minimumWidth) / scalingY), m_TextureTitleBar_M.getSize().y));
        }
        else // The image is not split
        {
            m_TextureTitleBar_M.sprite.setScale((m_Size.x + m_LeftBorder + m_RightBorder) / m_TextureTitleBar_M.getSize().x,
                                                static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setDistanceToSide(unsigned int distanceToSide)
    {
        m_DistanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getDistanceToSide() const
    {
        return m_DistanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_TitleAlignment = alignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::TitleAlignment ChildWindow::getTitleAlignment() const
    {
        return m_TitleAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setIcon(const std::string& filename)
    {
        // If a texture has already been loaded then remove it first
        if (m_IconTexture.data)
            TGUI_TextureManager.removeTexture(m_IconTexture);

        // Load the icon image
        if (TGUI_TextureManager.getTexture(filename, m_IconTexture))
        {
            m_IconTexture.sprite.setScale(static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y,
                                          static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y);
        }
        else // Loading failed
            TGUI_OUTPUT("Failed to load \"" + filename + "\" as icon for the ChildWindow");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::removeIcon()
    {
        if (m_IconTexture.data)
            TGUI_TextureManager.removeTexture(m_IconTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        m_Parent->remove(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::keepInParent(bool enabled)
    {
        m_KeepInParent = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent() const
    {
        return m_KeepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getWidgetsOffset() const
    {
        return sf::Vector2f(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder + getTitleBarHeight()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(float x, float y)
    {
        if (m_KeepInParent)
        {
            if (y < 0)
                Transformable::setPosition(getPosition().x, 0);
            else if (y > m_Parent->getSize().y - m_TitleBarHeight)
                Transformable::setPosition(getPosition().x, m_Parent->getSize().y - m_TitleBarHeight);
            else
                Transformable::setPosition(getPosition().x, y);

            if (x < 0)
                Transformable::setPosition(0, getPosition().y);
            else if (x > m_Parent->getSize().x - getSize().x)
                Transformable::setPosition(m_Parent->getSize().x - getSize().x, getPosition().y);
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
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight))).contains(x, y))
        {
            for (unsigned int i = 0; i < m_Widgets.size(); ++i)
                m_Widgets[i]->mouseNotOnWidget();

            return true;
        }
        else
        {
            // Check if the mouse is inside the child window
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, m_Size.y + m_TopBorder + m_BottomBorder)).contains(x, y - m_TitleBarHeight))
                return true;
            else
            {
                if (m_MouseHover)
                {
                    mouseLeftWidget();

                    // Tell the widgets inside the child window that the mouse is no longer on top of them
                    for (unsigned int i = 0; i < m_Widgets.size(); ++i)
                        m_Widgets[i]->mouseNotOnWidget();

                    m_CloseButton->mouseNotOnWidget();
                    m_MouseHover = false;
                }

                return false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;

        // Move the childwindow to the front
        m_Parent->moveWidgetToFront(this);

        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[LeftMousePressed].empty() == false)
        {
            m_Callback.trigger = LeftMousePressed;
            m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight))).contains(x, y))
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_CloseButton->setPosition(position.x + ((m_Size.x + m_LeftBorder + m_RightBorder - m_DistanceToSide - m_CloseButton->getSize().x)), position.y + ((m_TitleBarHeight / 2.f) - (m_CloseButton->getSize().x / 2.f)));

            // Send the mouse press event to the close button
            if (m_CloseButton->mouseOnWidget(x, y))
                m_CloseButton->leftMousePressed(x, y);
            else
            {
                // The mouse went down on the title bar
                m_MouseDownOnTitleBar = true;

                // Remember where we are dragging the title bar
                m_DraggingPosition.x = x - position.x;
                m_DraggingPosition.y = y - position.y;
            }

            // Reset the position of the button
            m_CloseButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_CloseButton->m_MouseHover)
                m_CloseButton->mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, m_Size.y + m_TopBorder + m_BottomBorder + m_TitleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TitleBarHeight + m_TopBorder), m_Size.x, m_Size.y)).contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMousePressed(x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(float x , float y)
    {
        m_MouseDown = false;
        m_MouseDownOnTitleBar = false;

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight))).contains(x, y))
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_CloseButton->setPosition(position.x + ((m_Size.x + m_LeftBorder + m_RightBorder - m_DistanceToSide - m_CloseButton->getSize().x)), position.y + ((m_TitleBarHeight / 2.f) - (m_CloseButton->getSize().x / 2.f)));

            // Check if the close button was clicked
            if (m_CloseButton->m_MouseDown == true)
            {
                m_CloseButton->m_MouseDown = false;

                // Check if the mouse is still on the close button
                if (m_CloseButton->mouseOnWidget(x, y))
                {
                    // If a callback was requested then send it
                    if (m_CallbackFunctions[Closed].empty() == false)
                    {
                        m_Callback.trigger = Closed;
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
            for (unsigned int i = 0; i < m_Widgets.size(); ++i)
                m_Widgets[i]->mouseNoLongerDown();

            // Reset the position of the button
            m_CloseButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_CloseButton->m_MouseHover)
                m_CloseButton->mouseNotOnWidget();

            m_CloseButton->mouseNoLongerDown();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, m_Size.y + m_TopBorder + m_BottomBorder + m_TitleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TitleBarHeight + m_TopBorder), m_Size.x, m_Size.y)).contains(x, y) == false))
            {
                // Tell the widgets about that the mouse was released
                for (unsigned int i = 0; i < m_Widgets.size(); ++i)
                    m_Widgets[i]->mouseNoLongerDown();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMouseReleased(x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(float x, float y)
    {
        m_MouseHover = true;

        // Check if you are dragging the child window
        if (m_MouseDown && m_MouseDownOnTitleBar)
        {
            // Move the child window
            sf::Vector2f position = getPosition();
            setPosition(position.x + (x - position.x - m_DraggingPosition.x), position.y + (y - position.y - m_DraggingPosition.y));

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[Moved].empty() == false)
            {
                m_Callback.trigger = Moved;
                m_Callback.position = getPosition();
                addCallback();
            }
        }

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight))).contains(x, y))
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_CloseButton->setPosition(position.x + ((m_Size.x + m_LeftBorder + m_RightBorder - m_DistanceToSide - m_CloseButton->getSize().x)), position.y + ((m_TitleBarHeight / 2.f) - (m_CloseButton->getSize().x / 2.f)));

            // Send the hover event to the close button
            if (m_CloseButton->mouseOnWidget(x, y))
                m_CloseButton->mouseMoved(x, y);

            // Reset the position of the button
            m_CloseButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_CloseButton->m_MouseHover)
                m_CloseButton->mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, m_Size.y + m_TopBorder + m_BottomBorder + m_TitleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TitleBarHeight + m_TopBorder), m_Size.x, m_Size.y)).contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved(x - m_LeftBorder, y - m_TitleBarHeight - m_TopBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        Container::mouseWheelMoved(delta, x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();
        m_CloseButton->mouseNoLongerDown();
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
            setTitleBarHeight(atoi(value.c_str()));
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
            setDistanceToSide(atoi(value.c_str()));
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
            value = to_string(getTitleBarHeight());
        else if (property == "backgroundcolor")
            value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "title")
            value = getTitle().toAnsiString();
        else if (property == "titlecolor")
            value = "(" + to_string(int(getTitleColor().r)) + "," + to_string(int(getTitleColor().g)) + "," + to_string(int(getTitleColor().b)) + "," + to_string(int(getTitleColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + to_string(int(getBorderColor().r)) + "," + to_string(int(getBorderColor().g)) + "," + to_string(int(getBorderColor().b)) + "," + to_string(int(getBorderColor().a)) + ")";
        else if (property == "borders")
            value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
        else if (property == "distancetoside")
            value = to_string(getDistanceToSide());
        else if (property == "titlealignment")
        {
            if (m_TitleAlignment == TitleAlignmentLeft)
                value = "Left";
            else if (m_TitleAlignment == TitleAlignmentCentered)
                value = "Centered";
            else if (m_TitleAlignment == TitleAlignmentRight)
                value = "Right";
        }
        else if (property == "callback")
        {
            std::string tempValue;
            Container::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(LeftMousePressed) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(LeftMousePressed).size() == 1) && (m_CallbackFunctions.at(LeftMousePressed).front() == nullptr))
                callbacks.push_back("LeftMousePressed");
            if ((m_CallbackFunctions.find(Closed) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(Closed).size() == 1) && (m_CallbackFunctions.at(Closed).front() == nullptr))
                callbacks.push_back("Closed");
            if ((m_CallbackFunctions.find(Moved) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(Moved).size() == 1) && (m_CallbackFunctions.at(Moved).front() == nullptr))
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
        m_Parent = parent;
        setGlobalFont(m_Parent->getGlobalFont());
        m_TitleText.setFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the child window wasn't created
        if (m_Loaded == false)
            return;

        // Calculate the scale factor of the view
        const sf::View& view = target.getView();
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPanelPosition = sf::Vector2f(((getAbsolutePosition().x + m_LeftBorder - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                         ((getAbsolutePosition().y + m_TitleBarHeight + m_TopBorder - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPanelPosition = sf::Vector2f((getAbsolutePosition().x + m_Size.x + m_LeftBorder - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                             (getAbsolutePosition().y + m_TitleBarHeight + m_Size.y + m_TopBorder - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        sf::Vector2f topLeftTitleBarPosition;
        sf::Vector2f bottomRightTitleBarPosition;

        if (m_IconTexture.data)
            topLeftTitleBarPosition = sf::Vector2f(((getAbsolutePosition().x + 2*m_DistanceToSide + m_IconTexture.getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                   ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        else
            topLeftTitleBarPosition = sf::Vector2f(((getAbsolutePosition().x + m_DistanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                   ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));

        bottomRightTitleBarPosition = sf::Vector2f((getAbsolutePosition().x + m_Size.x + m_LeftBorder + m_RightBorder - (2*m_DistanceToSide) - m_CloseButton->getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                   (getAbsolutePosition().y + m_TitleBarHeight - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Adjust the transformation
        states.transform *= getTransform();

        sf::Transform oldTransform = states.transform;

        // Check if the title bar image is split
        if (m_SplitImage)
        {
            target.draw(m_TextureTitleBar_L, states);

            states.transform.translate(m_TextureTitleBar_L.getSize().x * (static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y), 0);
            target.draw(m_TextureTitleBar_M, states);

            states.transform.translate(m_Size.x + m_LeftBorder + m_RightBorder - ((m_TextureTitleBar_R.getSize().x + m_TextureTitleBar_L.getSize().x)
                                                                                  * (static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y)), 0);
            target.draw(m_TextureTitleBar_R, states);
        }
        else // The title bar image isn't split
        {
            // Draw the title bar
            target.draw(m_TextureTitleBar_M, states);
        }

        states.transform = oldTransform;

        // Draw a window icon if one was set
        if (m_IconTexture.data)
        {
            states.transform.translate(static_cast<float>(m_DistanceToSide), (m_TitleBarHeight - (m_IconTexture.getSize().y * m_IconTexture.sprite.getScale().y)) / 2.f);
            target.draw(m_IconTexture, states);
            states.transform.translate(m_IconTexture.getSize().x * m_IconTexture.sprite.getScale().x, (m_TitleBarHeight - (m_IconTexture.getSize().y * m_IconTexture.sprite.getScale().y)) / -2.f);
        }

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Check if there is a title
        if (m_TitleText.getString().isEmpty() == false)
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

            // Center the text vertically
            states.transform.translate(0, std::floor(((m_TitleBarHeight - m_TitleText.getLocalBounds().height) / 2.0f) - m_TitleText.getLocalBounds().top));

            // Draw the text, depending on the alignment
            if (m_TitleAlignment == TitleAlignmentLeft)
            {
                states.transform.translate(std::floor(static_cast<float>(m_DistanceToSide) + 0.5f), 0);
                target.draw(m_TitleText, states);
            }
            else if (m_TitleAlignment == TitleAlignmentCentered)
            {
                if (m_IconTexture.data)
                    states.transform.translate(std::floor(m_DistanceToSide + (((m_Size.x + m_LeftBorder + m_RightBorder) - 4*m_DistanceToSide - (m_IconTexture.getSize().x * m_IconTexture.sprite.getScale().x) - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width) / 2.0f) + 0.5f), 0);
                else
                    states.transform.translate(std::floor(m_DistanceToSide + (((m_Size.x + m_LeftBorder + m_RightBorder) - 3*m_DistanceToSide - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width) / 2.0f) + 0.5f), 0);

                target.draw(m_TitleText, states);
            }
            else // if (m_TitleAlignment == TitleAlignmentRight)
            {
                if (m_IconTexture.data)
                    states.transform.translate(std::floor((m_Size.x + m_LeftBorder + m_RightBorder) - (m_IconTexture.getSize().x * m_IconTexture.sprite.getScale().x) - 3*m_DistanceToSide - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width + 0.5f), 0);
                else
                    states.transform.translate(std::floor((m_Size.x + m_LeftBorder + m_RightBorder) - 2*m_DistanceToSide - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width + 0.5f), 0);

                target.draw(m_TitleText, states);
            }

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }

        // Move the close button to the correct position
        states.transform = oldTransform;
        states.transform.translate((m_Size.x + m_LeftBorder + m_RightBorder) - m_DistanceToSide - m_CloseButton->getSize().x, (m_TitleBarHeight - m_CloseButton->getSize().y) / 2.f);

        // Draw the close button
        target.draw(*m_CloseButton, states);

        // Set the correct transformation
        states.transform = oldTransform.translate(0, static_cast<float>(m_TitleBarHeight));

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_LeftBorder), m_Size.y + m_TopBorder));
        border.setFillColor(m_BorderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(m_Size.x + m_RightBorder, static_cast<float>(m_TopBorder)));
        target.draw(border, states);

        // Draw right border
        border.setSize(sf::Vector2f(static_cast<float>(m_RightBorder), m_Size.y + m_BottomBorder));
        border.setPosition(m_Size.x + m_LeftBorder, 0);
        target.draw(border, states);

        // Draw bottom border
        border.setSize(sf::Vector2f(m_Size.x + m_LeftBorder, static_cast<float>(m_BottomBorder)));
        border.setPosition(0, m_Size.y + m_TopBorder);
        target.draw(border, states);

        // Make room for the borders
        states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

        // Draw the background
        if (m_BackgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(sf::Vector2f(m_Size.x, m_Size.y));
            background.setFillColor(m_BackgroundColor);
            target.draw(background, states);
        }

        // Draw the background image if there is one
        if (m_BackgroundTexture != nullptr)
            target.draw(m_BackgroundSprite, states);

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
