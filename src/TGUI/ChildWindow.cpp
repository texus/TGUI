/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets.hpp>
#include <TGUI/ContainerWidget.hpp>
#include <TGUI/ClickableWidget.hpp>
#include <TGUI/Button.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/ChildWindow.hpp>

#include <SFML/OpenGL.hpp>

/// \todo Add SplitImage to title bar

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow() :
    m_Size             (0, 0),
    m_BackgroundTexture(NULL),
    m_TitleBarHeight   (0),
    m_SplitImage       (false),
    m_DraggingPosition (0, 0),
    m_DistanceToSide   (5),
    m_TitleAlignment   (TitleAlignmentCentered),
    m_BorderColor      (0, 0, 0),
    m_KeepInParent     (false)
    {
        m_Callback.widgetType = Type_ChildWindow;
        m_CloseButton = new tgui::Button();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const ChildWindow& childWindowToCopy) :
    ContainerWidget    (childWindowToCopy),
    WidgetBorders      (childWindowToCopy),
    m_LoadedConfigFile (childWindowToCopy.m_LoadedConfigFile),
    m_Size             (childWindowToCopy.m_Size),
    m_BackgroundColor  (childWindowToCopy.m_BackgroundColor),
    m_BackgroundTexture(childWindowToCopy.m_BackgroundTexture),
    m_TitleText        (childWindowToCopy.m_TitleText),
    m_TitleBarHeight   (childWindowToCopy.m_TitleBarHeight),
    m_SplitImage       (childWindowToCopy.m_SplitImage),
    m_DraggingPosition (childWindowToCopy.m_DraggingPosition),
    m_DistanceToSide   (childWindowToCopy.m_DistanceToSide),
    m_TitleAlignment   (childWindowToCopy.m_TitleAlignment),
    m_BorderColor      (childWindowToCopy.m_BorderColor),
    m_KeepInParent     (childWindowToCopy.m_KeepInParent)
    {
        // Copy the textures
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_IconTexture, m_IconTexture);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_TextureTitleBar_L, m_TextureTitleBar_L);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_TextureTitleBar_M, m_TextureTitleBar_M);
        TGUI_TextureManager.copyTexture(childWindowToCopy.m_TextureTitleBar_R, m_TextureTitleBar_R);

        // Copy the button
        m_CloseButton = new tgui::Button(*childWindowToCopy.m_CloseButton);

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
        if (m_TextureTitleBar_L.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureTitleBar_L);
        if (m_TextureTitleBar_M.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureTitleBar_M);
        if (m_TextureTitleBar_R.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureTitleBar_R);

        if (m_IconTexture.data != NULL)
            TGUI_TextureManager.removeTexture(m_IconTexture);

        if (m_IconTexture.data != NULL)
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
            this->ContainerWidget::operator=(right);
            this->WidgetBorders::operator=(right);

            // Delete the old close button
            delete m_CloseButton;

            std::swap(m_LoadedConfigFile,  temp.m_LoadedConfigFile);
            std::swap(m_Size,              temp.m_Size);
            std::swap(m_BackgroundColor,   temp.m_BackgroundColor);
            std::swap(m_BackgroundTexture, temp.m_BackgroundTexture);
            std::swap(m_BackgroundSprite,  temp.m_BackgroundSprite);
            std::swap(m_IconTexture,       temp.m_IconTexture);
            std::swap(m_TitleText,         temp.m_TitleText);
            std::swap(m_TitleBarHeight,    temp.m_TitleBarHeight);
            std::swap(m_SplitImage,        temp.m_SplitImage);
            std::swap(m_DraggingPosition,  temp.m_DraggingPosition);
            std::swap(m_DistanceToSide,    temp.m_DistanceToSide);
            std::swap(m_TitleAlignment,    temp.m_TitleAlignment);
            std::swap(m_BorderColor,       temp.m_BorderColor);
            std::swap(m_TextureTitleBar_L, temp.m_TextureTitleBar_L);
            std::swap(m_TextureTitleBar_M, temp.m_TextureTitleBar_M);
            std::swap(m_TextureTitleBar_R, temp.m_TextureTitleBar_R);
            std::swap(m_CloseButton,       temp.m_CloseButton);
            std::swap(m_KeepInParent,      temp.m_KeepInParent);
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
        m_LoadedConfigFile = configFileFilename;

        // Until the loading succeeds, the child window will be marked as unloaded
        m_Loaded = false;

        // Remove the textures when they were loaded before
        if (m_TextureTitleBar_L.data != NULL) TGUI_TextureManager.removeTexture(m_TextureTitleBar_L);
        if (m_TextureTitleBar_M.data != NULL) TGUI_TextureManager.removeTexture(m_TextureTitleBar_M);
        if (m_TextureTitleBar_R.data != NULL) TGUI_TextureManager.removeTexture(m_TextureTitleBar_R);
        if (m_CloseButton->m_TextureNormal_M.data != NULL) TGUI_TextureManager.removeTexture(m_CloseButton->m_TextureNormal_M);
        if (m_CloseButton->m_TextureHover_M.data != NULL)  TGUI_TextureManager.removeTexture(m_CloseButton->m_TextureHover_M);
        if (m_CloseButton->m_TextureDown_M.data != NULL)   TGUI_TextureManager.removeTexture(m_CloseButton->m_TextureDown_M);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("ChildWindow", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + configFileFilename + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TitlebarImage in section ChildWindow in " + configFileFilename + ".");
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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonNormalImage in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "closebuttonhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_CloseButton->m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonHoverImage in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "closebuttondownimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_CloseButton->m_TextureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CloseButtonDownImage in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "borders")
            {
                Vector4u borders;
                if (extractVector4u(value, borders))
                    setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
            }
            else if (property == "distancetoside")
            {
                setDistanceToSide(static_cast<unsigned int>(atoi(value.c_str())));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ChildWindow in " + configFileFilename + ".");
        }

        // Initialize the close button if it was loaded
        if (m_CloseButton->m_TextureNormal_M.data != NULL)
        {
            // Check if optional textures were loaded
            if (m_CloseButton->m_TextureHover_M.data != NULL)
            {
                m_CloseButton->m_WidgetPhase |= WidgetPhase_Hover;
            }
            if (m_CloseButton->m_TextureDown_M.data != NULL)
            {
                m_CloseButton->m_WidgetPhase |= WidgetPhase_MouseDown;
            }

            m_CloseButton->m_Size = Vector2f(m_CloseButton->m_TextureNormal_M.getSize());
            m_CloseButton->m_Loaded = true;
        }
        else // Close button wan't loaded
        {
            TGUI_OUTPUT("TGUI error: Missing a CloseButtonNormalImage property in section ChildWindow in " + configFileFilename + ".");
            return false;
        }

        // Make sure the required texture was loaded
        if ((m_TextureTitleBar_M.data != NULL))
        {
            m_TitleBarHeight = m_TextureTitleBar_M.getSize().y;
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the child window. Is the ChildWindow section in " + configFileFilename + " complete?");
            return false;
        }

        // Set the size of the title text
        m_TitleText.setCharacterSize(m_TitleBarHeight * 8 / 10);

        // When there is no error we will return true
        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ChildWindow::getLoadedConfigFile()
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

        // If there is a background texture then resize it
        if (m_BackgroundTexture)
            m_BackgroundSprite.setScale(m_Size.x / m_BackgroundTexture->getSize().x, m_Size.y / m_BackgroundTexture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getSize() const
    {
        return Vector2f(m_Size.x, m_Size.y);
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

    void ChildWindow::setTitlebarHeight(unsigned int height)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_Loaded == false)
            return;

        // Remember the new title bar height
        m_TitleBarHeight = height;

        // Set the size of the close button
        m_CloseButton->setSize(static_cast<float>(height) / m_TextureTitleBar_M.getSize().y * m_CloseButton->getSize().x,
                               static_cast<float>(height) / m_TextureTitleBar_M.getSize().y * m_CloseButton->getSize().y);

        // Set the size of the text in the title bar
        m_TitleText.setCharacterSize(m_TitleBarHeight * 8 / 10);
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
        ContainerWidget::setTransparency(transparency);

        m_BackgroundSprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_IconTexture.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_TextureTitleBar_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTitleBar_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTitleBar_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_CloseButton->setTransparency(m_Opacity);

        m_TitleText.setColor(sf::Color(m_TitleText.getColor().r, m_TitleText.getColor().g, m_TitleText.getColor().b, m_Opacity));

        m_BackgroundColor.a = m_Opacity;
        m_BorderColor.a = m_Opacity;
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

    bool ChildWindow::isKeptInParent()
    {
        return m_KeepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(float x, float y)
    {
        if (m_KeepInParent)
        {
            if (y < 0)
                Transformable::setPosition(getPosition().x, 0);
            else if (y > m_Parent->getDisplaySize().y - m_TitleBarHeight)
                Transformable::setPosition(getPosition().x, m_Parent->getDisplaySize().y - m_TitleBarHeight);
            else
                Transformable::setPosition(getPosition().x, y);

            if (x < 0)
                Transformable::setPosition(0, getPosition().y);
            else if (x > m_Parent->getDisplaySize().x - getSize().x)
                Transformable::setPosition(m_Parent->getDisplaySize().x - getSize().x, getPosition().y);
            else
                Transformable::setPosition(x, getPosition().y);
        }
        else
            Transformable::setPosition(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getDisplaySize()
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(float x, float y)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight + m_TopBorder))).contains(x, y))
        {
            m_EventManager.mouseNotOnWidget();
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
                    mouseLeftWidget();

                // Tell the widgets inside the child window that the mouse is no longer on top of them
                m_CloseButton->mouseNotOnWidget();
                m_EventManager.mouseNotOnWidget();
                m_MouseHover = false;
                return false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(float x, float y)
    {
        // Move the childwindow to the front
        m_Parent->focusWidget(this);
        m_Parent->moveWidgetToFront(this);

        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight))).contains(x, y))
        {
            // Get the current position
            Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_CloseButton->setPosition(position.x + ((m_Size.x + m_LeftBorder + m_RightBorder - m_DistanceToSide - m_CloseButton->getSize().x)), position.y + ((m_TitleBarHeight / 2.f) - (m_CloseButton->getSize().x / 2.f)));

            // Send the mouse press event to the close button
            if (m_CloseButton->mouseOnWidget(x, y))
                m_CloseButton->leftMousePressed(x, y);
            else
            {
                // The mouse went down on the title bar
                m_MouseDown = true;

                // Remember where we are dragging the title bar
                m_DraggingPosition.x = x - position.x;
                m_DraggingPosition.y = y - position.y;
            }

            // Reset the position of the button
            m_CloseButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the titlebar
        {
            // When the mouse is not on the titlebar, the mouse can't be on the close button
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

        ContainerWidget::leftMousePressed(x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(float x , float y)
    {
        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, static_cast<float>(m_TitleBarHeight))).contains(x, y))
        {
            // Get the current position
            Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_CloseButton->setPosition(position.x + ((m_Size.x + m_LeftBorder + m_RightBorder - m_DistanceToSide - m_CloseButton->getSize().x)), position.y + ((m_TitleBarHeight / 2.f) - (m_CloseButton->getSize().x / 2.f)));

            m_MouseDown = false;

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
            m_EventManager.mouseNoLongerDown();

            // Reset the position of the button
            m_CloseButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the titlebar
        {
            // When the mouse is not on the titlebar, the mouse can't be on the close button
            if (m_CloseButton->m_MouseHover)
                m_CloseButton->mouseNotOnWidget();

            // Change the mouse down flag
            m_MouseDown = false;
            m_CloseButton->mouseNoLongerDown();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x + m_LeftBorder + m_RightBorder, m_Size.y + m_TopBorder + m_BottomBorder + m_TitleBarHeight)).contains(x, y))
            &&  (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TitleBarHeight + m_TopBorder), m_Size.x, m_Size.y)).contains(x, y) == false))
            {
                // Tell the widgets about that the mouse was released
                m_EventManager.mouseNoLongerDown();

                // Don't send the event to the widgets
                return;
            }
        }

        ContainerWidget::leftMouseReleased(x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(float x, float y)
    {
        m_MouseHover = true;

        // Check if you are dragging the child window
        if (m_MouseDown == true)
        {
            // Move the child window
            Vector2f position = getPosition();
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
            Vector2f position = getPosition();

            // Temporary set the close button to the correct position
            m_CloseButton->setPosition(position.x + ((m_Size.x + m_LeftBorder + m_RightBorder - m_DistanceToSide - m_CloseButton->getSize().x)), position.y + ((m_TitleBarHeight / 2.f) - (m_CloseButton->getSize().x / 2.f)));

            // Send the hover event to the close button
            if (m_CloseButton->mouseOnWidget(x, y))
                m_CloseButton->mouseMoved(x, y);

            // Reset the position of the button
            m_CloseButton->setPosition(0, 0);
            return;
        }
        else // The mouse is not on top of the titlebar
        {
            // When the mouse is not on the titlebar, the mouse can't be on the close button
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

        ContainerWidget::mouseMoved(x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        ContainerWidget::mouseWheelMoved(delta, x - m_LeftBorder, y - (m_TitleBarHeight + m_TopBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        ContainerWidget::mouseNoLongerDown();
        m_CloseButton->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::setProperty(const std::string& property, const std::string& value)
    {
        if (!ContainerWidget::setProperty(property, value))
        {
            if (property == "ConfigFile")
            {
                load(value);
            }
            else if (property == "TitlebarHeight")
            {
                setTitlebarHeight(atoi(value.c_str()));
            }
            else if (property == "BackgroundColor")
            {
                setBackgroundColor(extractColor(value));
            }
            else if (property == "Title")
            {
                setTitle(value);
            }
            else if (property == "TitleColor")
            {
                setTitleColor(extractColor(value));
            }
            else if (property == "BorderColor")
            {
                setBorderColor(extractColor(value));
            }
            else if (property == "Borders")
            {
                Vector4u borders;
                if (extractVector4u(value, borders))
                    setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
            }
            else if (property == "DistanceToSide")
            {
                setDistanceToSide(atoi(value.c_str()));
            }
            else if (property == "TitleAlignment")
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
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::getProperty(const std::string& property, std::string& value)
    {
        if (!ContainerWidget::getProperty(property, value))
        {
            if (property == "ConfigFile")
                value = getLoadedConfigFile();
            else if (property == "TitlebarHeight")
                value = to_string(getTitleBarHeight());
            else if (property == "BackgroundColor")
                value = "(" + to_string(getBackgroundColor().r) + "," + to_string(getBackgroundColor().g) + "," + to_string(getBackgroundColor().b) + "," + to_string(getBackgroundColor().a) + ")";
            else if (property == "Title")
                value = getTitle().toAnsiString();
            else if (property == "TitleColor")
                value = "(" + to_string(getTitleColor().r) + "," + to_string(getTitleColor().g) + "," + to_string(getTitleColor().b) + "," + to_string(getTitleColor().a) + ")";
            else if (property == "BorderColor")
                value = "(" + to_string(getBorderColor().r) + "," + to_string(getBorderColor().g) + "," + to_string(getBorderColor().b) + "," + to_string(getBorderColor().a) + ")";
            else if (property == "Borders")
                value = "(" + to_string(getBorders().x1) + "," + to_string(getBorders().x2) + "," + to_string(getBorders().x3) + "," + to_string(getBorders().x4) + ")";
            else if (property == "DistanceToSide")
                value = to_string(getDistanceToSide());
            else if (property == "TitleAlignment")
            {
                if (m_TitleAlignment == TitleAlignmentLeft)
                    value = "left";
                else if (m_TitleAlignment == TitleAlignmentCentered)
                    value = "centered";
                else if (m_TitleAlignment == TitleAlignmentRight)
                    value = "right";
            }
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::initialize(tgui::Container *const parent)
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

        // Get the current position
        Vector2f position = getPosition();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        Vector2f viewPosition = (target.getView().getSize() / 2.f) - target.getView().getCenter();

        // Get the global position
        Vector2f topLeftPanelPosition = states.transform.transformPoint(position.x + m_LeftBorder + viewPosition.x,
                                                                        position.y + m_TitleBarHeight + m_TopBorder + viewPosition.y);
        Vector2f bottomRightPanelPosition = states.transform.transformPoint(position.x + m_Size.x + m_LeftBorder + viewPosition.x,
                                                                            position.y + m_TitleBarHeight + m_Size.y + m_TopBorder + viewPosition.y);
        Vector2f topLeftTitleBarPosition;
        Vector2f bottomRightTitleBarPosition;

        if (m_IconTexture.data)
            topLeftTitleBarPosition = states.transform.transformPoint(position.x + 2*m_DistanceToSide + (m_IconTexture.getSize().x * m_IconTexture.sprite.getScale().x) + viewPosition.x,
                                                                      position.y + viewPosition.y);
        else
            topLeftTitleBarPosition = states.transform.transformPoint(position.x + m_DistanceToSide + viewPosition.x, position.y + viewPosition.y);

        bottomRightTitleBarPosition = states.transform.transformPoint(position.x + m_Size.x + m_LeftBorder + m_RightBorder - (2*m_DistanceToSide) - m_CloseButton->getSize().x + viewPosition.x,
                                                                      position.y + m_TitleBarHeight + viewPosition.y);

        // Adjust the transformation
        states.transform *= getTransform();

        sf::Transform oldTransform = states.transform;

        // Check if the title bar image is split
        if (m_SplitImage)
        {
            // Split image is not supported yet
            return;
        }
        else // The title bar image isn't split
        {
            // Scale the title bar
            states.transform.scale((m_Size.x + m_LeftBorder + m_RightBorder) / m_TextureTitleBar_M.getSize().x, static_cast<float>(m_TitleBarHeight) / m_TextureTitleBar_M.getSize().y);

            // Draw the title bar
            target.draw(m_TextureTitleBar_M, states);

            // Undo the scaling
            states.transform.scale(static_cast<float>(m_TextureTitleBar_M.getSize().x) / (m_Size.x + m_LeftBorder + m_RightBorder), static_cast<float>(m_TextureTitleBar_M.getSize().y) / m_TitleBarHeight);
        }

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

            // Draw the text, depending on the alignment
            if (m_TitleAlignment == TitleAlignmentLeft)
            {
                states.transform.translate(static_cast<float>(m_DistanceToSide), 0);
                target.draw(m_TitleText, states);
            }
            else if (m_TitleAlignment == TitleAlignmentCentered)
            {
                if (m_IconTexture.data)
                    states.transform.translate(m_DistanceToSide + (((m_Size.x + m_LeftBorder + m_RightBorder) - 4*m_DistanceToSide - (m_IconTexture.getSize().x * m_IconTexture.sprite.getScale().x) - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width) / 2.0f), 0);
                else
                    states.transform.translate(m_DistanceToSide + (((m_Size.x + m_LeftBorder + m_RightBorder) - 3*m_DistanceToSide - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width) / 2.0f), 0);

                target.draw(m_TitleText, states);
            }
            else // if (m_TitleAlignment == TitleAlignmentRight)
            {
                if (m_IconTexture.data)
                    states.transform.translate((m_Size.x + m_LeftBorder + m_RightBorder) - (m_IconTexture.getSize().x * m_IconTexture.sprite.getScale().x) - 3*m_DistanceToSide - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width, 0);
                else
                    states.transform.translate((m_Size.x + m_LeftBorder + m_RightBorder) - 2*m_DistanceToSide - m_CloseButton->getSize().x - m_TitleText.getGlobalBounds().width, 0);

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
        sf::RectangleShape border(Vector2f(m_LeftBorder, m_Size.y + m_TopBorder + m_BottomBorder));
        border.setFillColor(m_BorderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(Vector2f(m_Size.x + m_LeftBorder + m_RightBorder, m_TopBorder));
        target.draw(border, states);

        // Draw right border
        border.setPosition(m_Size.x + m_LeftBorder, 0);
        border.setSize(Vector2f(m_RightBorder, m_Size.y + m_TopBorder + m_BottomBorder));
        target.draw(border, states);

        // Draw bottom border
        border.setPosition(0, m_Size.y + m_TopBorder);
        border.setSize(Vector2f(m_Size.x + m_LeftBorder + m_RightBorder, m_BottomBorder));
        target.draw(border, states);

        // Make room for the borders
        states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

        // Draw the background
        if (m_BackgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(Vector2f(m_Size.x, m_Size.y));
            background.setFillColor(m_BackgroundColor);
            target.draw(background, states);
        }

        // Draw the background image if there is one
        if (m_BackgroundTexture != NULL)
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
