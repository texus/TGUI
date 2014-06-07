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

#include <TGUI/Button.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ChildWindow.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow()
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
        m_iconTexture      (childWindowToCopy.m_iconTexture),
        m_textureTitleBar  (childWindowToCopy.m_textureTitleBar),
        m_titleText        (childWindowToCopy.m_titleText),
        m_titleBarHeight   (childWindowToCopy.m_titleBarHeight),
        m_draggingPosition (childWindowToCopy.m_draggingPosition),
        m_distanceToSide   (childWindowToCopy.m_distanceToSide),
        m_titleAlignment   (childWindowToCopy.m_titleAlignment),
        m_borderColor      (childWindowToCopy.m_borderColor),
        m_keepInParent     (childWindowToCopy.m_keepInParent)
    {
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

/// TODO: Get rid of manual memory managment
/**
    ChildWindow::~ChildWindow()
    {
        delete m_closeButton;
    }
*/
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
            std::swap(m_textureTitleBar,   temp.m_textureTitleBar);
            std::swap(m_titleText,         temp.m_titleText);
            std::swap(m_titleBarHeight,    temp.m_titleBarHeight);
            std::swap(m_draggingPosition,  temp.m_draggingPosition);
            std::swap(m_distanceToSide,    temp.m_distanceToSide);
            std::swap(m_titleAlignment,    temp.m_titleAlignment);
            std::swap(m_borderColor,       temp.m_borderColor);
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

    void ChildWindow::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Remove the textures when they were loaded before
        if (m_textureTitleBar.getData() != nullptr)              TGUI_TextureManager.removeTexture(m_textureTitleBar);
        if (m_closeButton->m_textureNormal.getData() != nullptr) TGUI_TextureManager.removeTexture(m_closeButton->m_textureNormal);
        if (m_closeButton->m_textureHover.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_closeButton->m_textureHover);
        if (m_closeButton->m_textureDown.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_closeButton->m_textureDown);

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "ChildWindow");

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "backgroundcolor")
                setBackgroundColor(configFile.readColor(it));
            else if (it->first == "titlecolor")
                setTitleColor(configFile.readColor(it));
            else if (it->first == "bordercolor")
                setBorderColor(configFile.readColor(it));
            else if (it->first == "titlebarimage")
                configFile.readTexture(it, configFileFolder, m_textureTitleBar);
            else if (it->first == "closebuttonseparatehoverimage")
                m_closeButton->m_separateHoverImage = configFile.readBool(it);
            else if (it->first == "closebuttonnormalimage")
                configFile.readTexture(it, configFileFolder, m_closeButton->m_textureNormal);
            else if (it->first == "closebuttonhoverimage")
                configFile.readTexture(it, configFileFolder, m_closeButton->m_textureHover);
            else if (it->first == "closebuttondownimage")
                configFile.readTexture(it, configFileFolder, m_closeButton->m_textureDown);
            else if (it->first == "borders")
            {
                Borders borders;
                if (extractBorders(it->second, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
                else
                    throw Exception("Failed to parse the 'Borders' property in section ChildWindow in " + m_loadedConfigFile);
            }
            else if (it->first == "distancetoside")
                setDistanceToSide(tgui::stoul(it->second));
            else
                throw Exception("Unrecognized property '" + it->first + "' in section ChildWindow in " + m_loadedConfigFile + ".");
        }

        // Make sure the close button was loaded
        if (m_closeButton->m_textureNormal.getData() == nullptr)
            throw Exception("Missing a CloseButtonNormalImage property in section ChildWindow in " + m_loadedConfigFile + ".");

        m_closeButton->setSize(m_closeButton->m_textureNormal.getImageSize().x, m_closeButton->m_textureNormal.getImageSize().y);

        // Make sure the required texture was loaded
        if (m_textureTitleBar.getData() == nullptr)
            throw Exception("Not all needed images were loaded for the child window. Is the ChildWindow section in " + m_loadedConfigFile + " complete?");

        m_titleBarHeight = m_textureTitleBar.getImageSize().y;

        setSize(m_textureTitleBar.getImageSize().x, m_textureTitleBar.getImageSize().x * 3.0f / 4.0f);

        // Set the size of the title text
        m_titleText.setCharacterSize(m_titleBarHeight * 8 / 10);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ChildWindow::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(float x, float y)
    {
        if (m_keepInParent)
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - m_titleBarHeight)
                y = m_parent->getSize().y - m_titleBarHeight;

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getSize().x)
                x = m_parent->getSize().x - getSize().x;
        }

        Transformable::setPosition(x, y);

        m_textureTitleBar.setPosition(x, y);
        m_iconTexture.setPosition(x + m_distanceToSide, y + ((m_titleBarHeight - m_iconTexture.getSize().y) / 2.0f));

        if (m_titleAlignment == TitleAlignmentLeft)
        {
            if (m_iconTexture.getData())
                m_titleText.setPosition(x + 2*m_distanceToSide + m_iconTexture.getSize().x - m_titleText.getLocalBounds().left,
                                        y + ((m_titleBarHeight - m_titleText.getLocalBounds().height) / 2.0f) - m_titleText.getLocalBounds().top);
            else
                m_titleText.setPosition(x + m_distanceToSide - m_titleText.getLocalBounds().left,
                                        y + ((m_titleBarHeight - m_titleText.getLocalBounds().height) / 2.0f) - m_titleText.getLocalBounds().top);

        }
        else if (m_titleAlignment == TitleAlignmentCentered)
        {
            if (m_iconTexture.getData())
                m_titleText.setPosition(x + 2*m_distanceToSide + m_iconTexture.getSize().x + (((m_size.x + m_borders.left + m_borders.right) - 4*m_distanceToSide - m_iconTexture.getSize().x - m_closeButton->getSize().x - m_titleText.getLocalBounds().width) / 2.0f) - m_titleText.getLocalBounds().left,
                                        y + ((m_titleBarHeight - m_titleText.getLocalBounds().height) / 2.0f) - m_titleText.getLocalBounds().top);
            else
                m_titleText.setPosition(x + m_distanceToSide + (((m_size.x + m_borders.left + m_borders.right) - 3*m_distanceToSide - m_closeButton->getSize().x - m_titleText.getLocalBounds().width) / 2.0f) - m_titleText.getLocalBounds().left,
                                        y + ((m_titleBarHeight - m_titleText.getLocalBounds().height) / 2.0f) - m_titleText.getLocalBounds().top);
        }
        else // if (m_titleAlignment == TitleAlignmentRight)
        {
            if (m_iconTexture.getData())
                m_titleText.setPosition(x + (m_size.x + m_borders.left + m_borders.right) - 2*m_distanceToSide - m_closeButton->getSize().x - m_titleText.getLocalBounds().width - m_titleText.getLocalBounds().left,
                                        y + ((m_titleBarHeight - m_titleText.getLocalBounds().height) / 2.0f) - m_titleText.getLocalBounds().top);
            else
                m_titleText.setPosition(x + (m_size.x + m_borders.left + m_borders.right) - 2*m_distanceToSide - m_closeButton->getSize().x - m_titleText.getLocalBounds().width - m_titleText.getLocalBounds().left,
                                        y + ((m_titleBarHeight - m_titleText.getLocalBounds().height) / 2.0f) - m_titleText.getLocalBounds().top);
        }

        m_closeButton->setPosition(x + (m_size.x + m_borders.left + m_borders.right) - m_distanceToSide - m_closeButton->getSize().x,
                                   y + ((m_titleBarHeight - m_closeButton->getSize().y) / 2.0f));
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

        m_textureTitleBar.setSize((width + m_borders.left + m_borders.right), static_cast<float>(m_titleBarHeight));

        // If there is a background texture then resize it
        if (m_backgroundTexture)
            m_backgroundSprite.setScale(m_size.x / m_backgroundTexture->getSize().x, m_size.y / m_backgroundTexture->getSize().y);

        // If there is an icon then give it the correct size
        if (m_iconTexture.getData())
        {
            m_iconTexture.setSize(m_titleBarHeight / m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().x,
                                  m_titleBarHeight / m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().y);
        }

        // Reposition the images and text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getSize() const
    {
        return sf::Vector2f(m_size.x, m_size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        return sf::Vector2f(m_size.x + m_borders.left + m_borders.right,
                            m_size.y + m_borders.top + m_borders.bottom + m_titleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::setGlobalFont(const std::string& filename)
    {
        if (Container::setGlobalFont(filename))
        {
            m_titleText.setFont(*getGlobalFont());
            setTitle(getTitle());
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setGlobalFont(const sf::Font& font)
    {
        Container::setGlobalFont(font);

        m_titleText.setFont(*getGlobalFont());
        setTitle(getTitle());
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
        // Remember the new title bar height
        m_titleBarHeight = height;

        // Set the size of the close button
        m_closeButton->setSize(height / m_textureTitleBar.getImageSize().y * m_closeButton->m_textureNormal.getImageSize().x,
                               height / m_textureTitleBar.getImageSize().y * m_closeButton->m_textureNormal.getImageSize().y);

        // Set the size of the text in the title bar
        m_titleText.setCharacterSize(static_cast<unsigned int>(m_titleBarHeight * 0.75f));

        m_textureTitleBar.setSize(m_size.x + m_borders.left + m_borders.right, static_cast<float>(m_titleBarHeight));

        // Reposition the images and text
        setPosition(getPosition());
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
        m_iconTexture.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTitleBar.setColor(sf::Color(255, 255, 255, m_opacity));

        m_closeButton->setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setString(title);

        // Reposition the images and text
        setPosition(getPosition());
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

    void ChildWindow::setBorders(float leftBorder, float topBorder, float rightBorder, float bottomBorder)
    {
        // Set the new border size
        m_borders.left   = leftBorder;
        m_borders.top    = topBorder;
        m_borders.right  = rightBorder;
        m_borders.bottom = bottomBorder;

        m_textureTitleBar.setSize(m_size.x + m_borders.left + m_borders.right, static_cast<float>(m_titleBarHeight));

        // Reposition the images and text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;

        // Reposition the images and text
        setPosition(getPosition());
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

        // Reposition the images and text
        setPosition(getPosition());
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
        if (m_iconTexture.getData())
            TGUI_TextureManager.removeTexture(m_iconTexture);

        // Load the icon image
        TGUI_TextureManager.getTexture(m_iconTexture, filename);

        m_iconTexture.setSize(m_titleBarHeight / m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().x,
                              m_titleBarHeight / m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().y);

        // Reposition the images and text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::removeIcon()
    {
        if (m_iconTexture.getData())
            TGUI_TextureManager.removeTexture(m_iconTexture);

        // Reposition the images and text
        setPosition(getPosition());
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

    bool ChildWindow::isKeptInParent() const
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getWidgetsOffset() const
    {
        return sf::Vector2f(m_borders.left, m_borders.top + getTitleBarHeight());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNotOnWidget();

            return true;
        }
        else
        {
            // Check if the mouse is inside the child window
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, m_size.y + m_borders.top + m_borders.bottom)).contains(x, y - m_titleBarHeight))
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
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            // Send the mouse press event to the close button
            if (m_closeButton->mouseOnWidget(x, y))
                m_closeButton->leftMousePressed(x, y);
            else
            {
                // The mouse went down on the title bar
                m_mouseDown = true;

                // Remember where we are dragging the title bar
                m_draggingPosition.x = x - getPosition().x;
                m_draggingPosition.y = y - getPosition().y;
            }

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, m_size.y + m_borders.top + m_borders.bottom + m_titleBarHeight)).contains(x, y))
             && (getTransform().transformRect(sf::FloatRect(m_borders.left, static_cast<float>(m_titleBarHeight + m_borders.top), m_size.x, m_size.y)).contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMousePressed(x - m_borders.left, y - (m_titleBarHeight + m_borders.top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(float x , float y)
    {
        // Check if the mouse is on top of the title bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
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
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, m_size.y + m_borders.top + m_borders.bottom + m_titleBarHeight)).contains(x, y))
             && (getTransform().transformRect(sf::FloatRect(m_borders.left, static_cast<float>(m_titleBarHeight + m_borders.top), m_size.x, m_size.y)).contains(x, y) == false))
            {
                // Tell the widgets about that the mouse was released
                for (unsigned int i = 0; i < m_widgets.size(); ++i)
                    m_widgets[i]->mouseNoLongerDown();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMouseReleased(x - m_borders.left, y - (m_titleBarHeight + m_borders.top));
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
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, static_cast<float>(m_titleBarHeight))).contains(x, y))
        {
            // Send the hover event to the close button
            if (m_closeButton->mouseOnWidget(x, y))
                m_closeButton->mouseMoved(x, y);

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((getTransform().transformRect(sf::FloatRect(0, 0, m_size.x + m_borders.left + m_borders.right, m_size.y + m_borders.top + m_borders.bottom + m_titleBarHeight)).contains(x, y))
             && (getTransform().transformRect(sf::FloatRect(m_borders.left, static_cast<float>(m_titleBarHeight + m_borders.top), m_size.x, m_size.y)).contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved(x - m_borders.left, y - (m_titleBarHeight + m_borders.top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        Container::mouseWheelMoved(delta, x - m_borders.left, y - (m_titleBarHeight + m_borders.top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();
        m_closeButton->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "titlebarheight")
        {
            setTitleBarHeight(tgui::stoi(value));
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
                throw Exception("Failed to parse 'Borders' property.");
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(tgui::stoi(value));
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
                throw Exception("Failed to parse 'TitleAlignment' property.");
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
            Container::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "titlebarheight")
            value = tgui::to_string(getTitleBarHeight());
        else if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "title")
            value = getTitle().toAnsiString();
        else if (property == "titlecolor")
            value = "(" + tgui::to_string(int(getTitleColor().r)) + "," + tgui::to_string(int(getTitleColor().g)) + "," + tgui::to_string(int(getTitleColor().b)) + "," + tgui::to_string(int(getTitleColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + tgui::to_string(int(getBorderColor().r)) + "," + tgui::to_string(int(getBorderColor().g)) + "," + tgui::to_string(int(getBorderColor().b)) + "," + tgui::to_string(int(getBorderColor().a)) + ")";
        else if (property == "borders")
            value = "(" + tgui::to_string(getBorders().left) + "," + tgui::to_string(getBorders().top) + "," + tgui::to_string(getBorders().right) + "," + tgui::to_string(getBorders().bottom) + ")";
        else if (property == "distancetoside")
            value = tgui::to_string(getDistanceToSide());
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
            Container::getProperty(property, value);
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
        Widget::initialize(parent);

        if (!m_fontPtr && m_parent->getGlobalFont())
            setGlobalFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the title bar
        target.draw(m_textureTitleBar, states);

        // Draw a window icon if one was set
        if (m_iconTexture.getData())
            target.draw(m_iconTexture, states);

        // Calculate the scale factor of the view
        const sf::View& view = target.getView();
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPanelPosition = sf::Vector2f(((getAbsolutePosition().x + m_borders.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                         ((getAbsolutePosition().y + m_titleBarHeight + m_borders.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPanelPosition = sf::Vector2f((getAbsolutePosition().x + m_size.x + m_borders.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                             (getAbsolutePosition().y + m_titleBarHeight + m_size.y + m_borders.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        sf::Vector2f topLeftTitleBarPosition;
        sf::Vector2f bottomRightTitleBarPosition;

        if (m_iconTexture.getData())
            topLeftTitleBarPosition = sf::Vector2f(((getAbsolutePosition().x + 2*m_distanceToSide + m_iconTexture.getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                   ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        else
            topLeftTitleBarPosition = sf::Vector2f(((getAbsolutePosition().x + m_distanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                   ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));

        bottomRightTitleBarPosition = sf::Vector2f((getAbsolutePosition().x + m_size.x + m_borders.left + m_borders.right - (2*m_distanceToSide) - m_closeButton->getSize().x - view.getCenter().x + (view.getSize().x / 2.f))
                                                   * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                   (getAbsolutePosition().y + m_titleBarHeight - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

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

            // Draw the text in the title bar
            target.draw(m_titleText, states);

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }

        // Draw the close button
        target.draw(*m_closeButton, states);

        // Set the correct transformation
        states.transform *= getTransform();
        states.transform.translate(0, static_cast<float>(m_titleBarHeight));

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(m_borders.left, m_size.y + m_borders.top));
        border.setFillColor(m_borderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(m_size.x + m_borders.right, m_borders.top));
        border.setPosition(m_borders.left, 0);
        target.draw(border, states);

        // Draw right border
        border.setSize(sf::Vector2f(m_borders.right, m_size.y + m_borders.bottom));
        border.setPosition(m_size.x + m_borders.left, m_borders.top);
        target.draw(border, states);

        // Draw bottom border
        border.setSize(sf::Vector2f(m_size.x + m_borders.left, m_borders.bottom));
        border.setPosition(0, m_size.y + m_borders.top);
        target.draw(border, states);

        // Make room for the borders
        states.transform.translate(m_borders.left, m_borders.top);

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
