/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button() :
    m_TextureNormal_L       (NULL),
    m_TextureMouseHover_L   (NULL),
    m_TextureMouseDown_L    (NULL),
    m_TextureFocused_L      (NULL),
    m_TextureNormal_M       (NULL),
    m_TextureMouseHover_M   (NULL),
    m_TextureMouseDown_M    (NULL),
    m_TextureFocused_M      (NULL),
    m_TextureNormal_R       (NULL),
    m_TextureMouseHover_R   (NULL),
    m_TextureMouseDown_R    (NULL),
    m_TextureFocused_R      (NULL),
    m_SplitImage            (false),
    m_TextSize              (0)
    {
        m_ObjectType = button;

        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button(const Button& copy) :
    OBJECT      (copy),
    m_SplitImage(copy.m_SplitImage),
    m_Text      (copy.m_Text),
    m_TextSize  (copy.m_TextSize)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L))         m_SpriteNormal_L.setTexture(*m_TextureNormal_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M))         m_SpriteNormal_M.setTexture(*m_TextureNormal_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R))         m_SpriteNormal_R.setTexture(*m_TextureNormal_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover_L, m_TextureMouseHover_L)) m_SpriteMouseHover_L.setTexture(*m_TextureMouseHover_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover_M, m_TextureMouseHover_M)) m_SpriteMouseHover_M.setTexture(*m_TextureMouseHover_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover_R, m_TextureMouseHover_R)) m_SpriteMouseHover_R.setTexture(*m_TextureMouseHover_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseDown_L, m_TextureMouseDown_L))   m_SpriteMouseDown_L.setTexture(*m_TextureMouseDown_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseDown_M, m_TextureMouseDown_M))   m_SpriteMouseDown_M.setTexture(*m_TextureMouseDown_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseDown_R, m_TextureMouseDown_R))   m_SpriteMouseDown_R.setTexture(*m_TextureMouseDown_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_L, m_TextureFocused_L))       m_SpriteFocused_L.setTexture(*m_TextureFocused_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_M, m_TextureFocused_M))       m_SpriteFocused_M.setTexture(*m_TextureFocused_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_R, m_TextureFocused_R))       m_SpriteFocused_R.setTexture(*m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::~Button()
    {
        // Remove all the textures from memory (if we are the only one who use the textures)

        if (m_TextureNormal_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureMouseHover_L != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_L);
        if (m_TextureMouseHover_M != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_M);
        if (m_TextureMouseHover_R != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_R);

        if (m_TextureMouseDown_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_L);
        if (m_TextureMouseDown_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_M);
        if (m_TextureMouseDown_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_R);

        if (m_TextureFocused_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button& Button::operator= (const Button& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Button temp(right);
            this->OBJECT::operator=(right);

            std::swap(m_TextureNormal_L,     temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,     temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,     temp.m_TextureNormal_R);
            std::swap(m_TextureMouseHover_L, temp.m_TextureMouseHover_L);
            std::swap(m_TextureMouseHover_M, temp.m_TextureMouseHover_M);
            std::swap(m_TextureMouseHover_R, temp.m_TextureMouseHover_R);
            std::swap(m_TextureMouseDown_L,  temp.m_TextureMouseDown_L);
            std::swap(m_TextureMouseDown_M,  temp.m_TextureMouseDown_M);
            std::swap(m_TextureMouseDown_R,  temp.m_TextureMouseDown_R);
            std::swap(m_TextureFocused_L,    temp.m_TextureFocused_L);
            std::swap(m_TextureFocused_M,    temp.m_TextureFocused_M);
            std::swap(m_TextureFocused_R,    temp.m_TextureFocused_R);
            std::swap(m_SpriteNormal_L,      temp.m_SpriteNormal_L);
            std::swap(m_SpriteNormal_M,      temp.m_SpriteNormal_M);
            std::swap(m_SpriteNormal_R,      temp.m_SpriteNormal_R);
            std::swap(m_SpriteMouseHover_L,  temp.m_SpriteMouseHover_L);
            std::swap(m_SpriteMouseHover_M,  temp.m_SpriteMouseHover_M);
            std::swap(m_SpriteMouseHover_R,  temp.m_SpriteMouseHover_R);
            std::swap(m_SpriteMouseDown_L,   temp.m_SpriteMouseDown_L);
            std::swap(m_SpriteMouseDown_M,   temp.m_SpriteMouseDown_M);
            std::swap(m_SpriteMouseDown_R,   temp.m_SpriteMouseDown_R);
            std::swap(m_SpriteFocused_L,     temp.m_SpriteFocused_L);
            std::swap(m_SpriteFocused_M,     temp.m_SpriteFocused_M);
            std::swap(m_SpriteFocused_R,     temp.m_SpriteFocused_R);
            std::swap(m_SplitImage,          temp.m_SplitImage);
            std::swap(m_Text,                temp.m_Text);
            std::swap(m_TextSize,            temp.m_TextSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::load(const std::string pathname)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Make sure that the pathname isn't empty
        if (pathname.empty())
            return false;

        // Store the pathname
        m_LoadedPathname = pathname;

        // When the pathname does not end with a "/" then we will add it
        if (m_LoadedPathname.at(m_LoadedPathname.length()-1) != '/')
            m_LoadedPathname.push_back('/');

        // Open the info file
        InfoFileParser infoFile;
        if (infoFile.openFile(m_LoadedPathname + "info.txt") == false)
        {
            TGUI_OUTPUT((((std::string("TGUI: Failed to open ")).append(m_LoadedPathname)).append("info.txt")).c_str());
            return false;
        }

        std::string property;
        std::string value;

        // Set some default settings
        m_SplitImage = false;
        std::string imageExtension = "png";

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            // Check what the property is
            if (property.compare("splitimage") == 0)
            {
                // Find out if it is split or not
                if (value.compare("true") == 0)
                    m_SplitImage = true;
                else if (value.compare("false") == 0)
                    m_SplitImage = false;
            }
            else if (property.compare("phases") == 0)
            {
                // Get and store the different phases
                extractPhases(value);
            }
            else if (property.compare("textcolor") == 0)
            {
                m_Text.setColor(extractColor(value));
            }
            else if (property.compare("extension") == 0)
            {
                imageExtension = value;
            }
        }

        // Close the info file
        infoFile.closeFile();

        // If the button was loaded before then remove the old textures first
        if (m_TextureNormal_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureMouseHover_L != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_L);
        if (m_TextureMouseHover_M != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_M);
        if (m_TextureMouseHover_R != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_R);
        if (m_TextureMouseDown_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_L);
        if (m_TextureMouseDown_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_M);
        if (m_TextureMouseDown_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_R);
        if (m_TextureFocused_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_R);

        // Check if the image is split
        if (m_SplitImage)
        {
            // load the required texture
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Normal." + imageExtension, m_TextureNormal_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Normal." + imageExtension, m_TextureNormal_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Normal." + imageExtension, m_TextureNormal_R)))
            {
                m_SpriteNormal_L.setTexture(*m_TextureNormal_L, true);
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
                m_SpriteNormal_R.setTexture(*m_TextureNormal_R, true);
            }
            else
                return false;

            bool error = false;

            // load the optional textures
            if (m_ObjectPhase & objectPhase::focused)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Focus." + imageExtension, m_TextureFocused_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Focus." + imageExtension, m_TextureFocused_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Focus." + imageExtension, m_TextureFocused_R)))
                {
                    m_SpriteFocused_L.setTexture(*m_TextureFocused_L, true);
                    m_SpriteFocused_M.setTexture(*m_TextureFocused_M, true);
                    m_SpriteFocused_R.setTexture(*m_TextureFocused_R, true);

                    m_AllowFocus = true;
                }
                else
                    error = true;
            }

            if (m_ObjectPhase & objectPhase::hover)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Hover." + imageExtension, m_TextureMouseHover_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Hover." + imageExtension, m_TextureMouseHover_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Hover." + imageExtension, m_TextureMouseHover_R)))
                {
                    m_SpriteMouseHover_L.setTexture(*m_TextureMouseHover_L, true);
                    m_SpriteMouseHover_M.setTexture(*m_TextureMouseHover_M, true);
                    m_SpriteMouseHover_R.setTexture(*m_TextureMouseHover_R, true);
                }
                else
                    error = true;
            }

            if (m_ObjectPhase & objectPhase::mouseDown)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Down." + imageExtension, m_TextureMouseDown_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Down." + imageExtension, m_TextureMouseDown_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Down." + imageExtension, m_TextureMouseDown_R)))
                {
                    m_SpriteMouseDown_L.setTexture(*m_TextureMouseDown_L, true);
                    m_SpriteMouseDown_M.setTexture(*m_TextureMouseDown_M, true);
                    m_SpriteMouseDown_R.setTexture(*m_TextureMouseDown_R, true);
                }
                else
                    error = true;
            }

            // When there is no error we will return true
            m_Loaded = !error;
            return !error;
        }
        else // The image isn't split
        {
            // load the required texture
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Normal." + imageExtension, m_TextureNormal_M))
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
            else
                return false;

            bool error = false;

            // load the optional textures
            if (m_ObjectPhase & objectPhase::focused)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Focus." + imageExtension, m_TextureFocused_M))
                {
                    m_SpriteFocused_M.setTexture(*m_TextureFocused_M, true);
                    m_AllowFocus = true;
                }
                else
                    error = true;
            }

            if (m_ObjectPhase & objectPhase::hover)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Hover." + imageExtension, m_TextureMouseHover_M))
                    m_SpriteMouseHover_M.setTexture(*m_TextureMouseHover_M, true);
                else
                    error = true;
            }

            if (m_ObjectPhase & objectPhase::mouseDown)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Down." + imageExtension, m_TextureMouseDown_M))
                    m_SpriteMouseDown_M.setTexture(*m_TextureMouseDown_M, true);
                else
                    error = true;
            }

            // When there is no error we will return true
            m_Loaded = !error;
            return !error;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(float width, float height)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if the image is split
        if (m_SplitImage)
        {
            setScale(width / (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x),
                     height / m_TextureNormal_M->getSize().y);
        }
        else // The image is not split
        {
            setScale(width / m_TextureNormal_M->getSize().x,
                     height / m_TextureNormal_M->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Button::getSize() const
    {
        // Don't continue when the button wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2u(0, 0);

        // Check if the image is split
        if (m_SplitImage)
        {
            // Return the size of the three images
            return Vector2u(m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x, m_TextureNormal_M->getSize().y);
        }
        else // The image is not split
        {
            // Return the size of the image
            return Vector2u(m_TextureNormal_M->getSize().x, m_TextureNormal_M->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Button::getScaledSize() const
    {
        // Don't continue when the button wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2f(0, 0);

        // Check if the image is split
        if (m_SplitImage)
        {
            // Return the size of the three images
            return Vector2f((m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x) * getScale().x,
                            m_TextureNormal_M->getSize().y * getScale().y);
        }
        else // The image is not split
        {
            // Return the size of the image
            return Vector2f(m_TextureNormal_M->getSize().x * getScale().x, m_TextureNormal_M->getSize().y * getScale().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Button::getLoadedPathname()
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const std::string text)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new text
        m_Text.setString(text);

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate a possible text size
            float size = m_TextureNormal_M->getSize().y * 0.9f;
            m_Text.setCharacterSize(static_cast<unsigned int>(size));

            // The calculation is slightly different when the image is split
            if (m_SplitImage)
            {
                // Calculate the button width
                unsigned int buttonWidth = m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x;

                // We must make sure that the text isn't too width
                if (m_Text.getGlobalBounds().width > (buttonWidth * 0.8f))
                {
                    // The text is too width, so make it smaller
                    m_Text.setCharacterSize(static_cast<unsigned int> (size / (m_Text.getGlobalBounds().width / (buttonWidth*0.8f))));
                }
            }
            else // The image is not split
            {
                // We must make sure that the text isn't too width
                if (m_Text.getGlobalBounds().width > (m_TextureNormal_M->getSize().x * 0.8f))
                {
                    // The text is too width, so make it smaller
                    m_Text.setCharacterSize(static_cast<unsigned int> (size / (m_Text.getGlobalBounds().width / (m_TextureNormal_M->getSize().x*0.8f))));
                }
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(static_cast<unsigned int>(m_TextSize * getScale().y));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Button::getText()
    {
        return m_Text.getString().toAnsiString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& Button::getTextFont()
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Button::getTextColor()
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(const unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString().toAnsiString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize()
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::mouseOnObject(float x, float y)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the button
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;
        else
        {
            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        // Set the mouse down flag
        m_MouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(float x, float y)
    {
        // Check if we clicked on the button (not just mouse release)
        if (m_MouseDown == true)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID  = callbackID;
                callback.trigger     = Callback::mouseClick;
                callback.mouseButton = sf::Mouse::Left;
                callback.mouseX      = x - getPosition().x;
                callback.mouseY      = y - getPosition().y;
                m_Parent->addCallback(callback);
            }

            m_MouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseMoved(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        m_MouseHover = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(sf::Keyboard::Key key)
    {
        // Check if the space key or the return key was pressed
        if (key == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::keyPress_Space;
                m_Parent->addCallback(callback);
            }
        }
        else if (key == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::keyPress_Return;
                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::objectFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_ObjectPhase & objectPhase::focused) == 0)
            m_Parent->unfocus(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Drawing the button image will be different when the image is split
        if (m_SplitImage)
        {
            // Set the position of the left image
            states.transform.translate(position);

            // Set the scale for the left image
            states.transform.scale(curScale.y, curScale.y);

            // Draw the left image
            {
                // When the button is down then draw another image
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & objectPhase::mouseDown))
                    target.draw(m_SpriteMouseDown_L, states);
                else
                    target.draw(m_SpriteNormal_L, states);

                // When the button is focused then draw an extra image
                if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                    target.draw(m_SpriteFocused_L, states);

                // When the mouse is on top of the button then draw an extra image
                if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                    target.draw(m_SpriteMouseHover_L, states);
            }

            // Check if the middle image may be drawn
            if ((curScale.y * (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x))
                < curScale.x * (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x))
            {
                // Calculate the scale for our middle image
                float scaleX = (((m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x)  * curScale.x)
                                 - ((m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * curScale.y))
                               / m_TextureNormal_M->getSize().x;

                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                // Set the scale for the middle image
                states.transform.scale(scaleX / curScale.y, 1);

                // Draw the middle image
                {
                    // When the button is down then draw another image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & objectPhase::mouseDown))
                        target.draw(m_SpriteMouseDown_M, states);
                    else
                        target.draw(m_SpriteNormal_M, states);

                    // When the button is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                        target.draw(m_SpriteFocused_M, states);

                    // When the mouse is on top of the button then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                        target.draw(m_SpriteMouseHover_M, states);
                }

                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_M->getSize().x), 0);

                // Set the scale for the right image
                states.transform.scale(curScale.y / scaleX, 1);

                // Draw the right image
                {
                    // When the button is down then draw another image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & objectPhase::mouseDown))
                        target.draw(m_SpriteMouseDown_R, states);
                    else
                        target.draw(m_SpriteNormal_R, states);

                    // When the button is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                        target.draw(m_SpriteFocused_R, states);

                    // When the mouse is on top of the button then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                        target.draw(m_SpriteMouseHover_R, states);
                }
            }
            else // The button isn't width enough, we will draw it at minimum size
            {
                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                // Draw the right image
                {
                    // When the button is down then draw another image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & objectPhase::mouseDown))
                        target.draw(m_SpriteMouseDown_R, states);
                    else
                        target.draw(m_SpriteNormal_R, states);

                    // When the button is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                        target.draw(m_SpriteFocused_R, states);

                    // When the mouse is on top of the button then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                        target.draw(m_SpriteMouseHover_R, states);
                }
            }
        }
        else // The image is not split
        {
            // Adjust the transformation
            states.transform *= getTransform();

            // Draw the button
            if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & objectPhase::mouseDown))
                target.draw(m_SpriteMouseDown_M, states);
            else
                target.draw(m_SpriteNormal_M, states);

            // When the button is focused then draw an extra image
            if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                target.draw(m_SpriteFocused_M, states);

            // When the mouse is on top of the button then draw an extra image
            if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                target.draw(m_SpriteMouseHover_M, states);
        }

        // If the button has a text then also draw the text
        if (m_Text.getString().getSize() > 0)
        {
            // Reset the transformations
            states.transform = oldTransform;
            states.transform.translate(position);

            // Get the current size of the text, so that we can recalculate the position
            sf::FloatRect rect = m_Text.getGlobalBounds();

            // Check if the user has chosen a text size
            if (m_TextSize > 0)
            {
                // The calculation will be different when the image is split
                if (m_SplitImage)
                {
                    // Calculate the new position for the text
                    rect.top = (((m_TextureNormal_M->getSize().y * curScale.y) - rect.height) * 0.5f) - rect.top;
                    rect.left = ((((m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x)
                                   * curScale.x) - rect.width) * 0.5f) - rect.left;
                }
                else // The image is not split
                {
                    // Calculate the new position for the text
                    rect.left = (((m_TextureNormal_M->getSize().x * curScale.x) - rect.width) * 0.5f) - rect.left;
                    rect.top = (((m_TextureNormal_M->getSize().y * curScale.y) - rect.height) * 0.5f) - rect.top;
                }

                // Set the new position
                states.transform.translate(static_cast<float>(static_cast<int>(rect.left + 0.5f)), static_cast<float>(static_cast<int>(rect.top + 0.5f)));
            }
            else // autoscale
            {
                // Get the text size
                unsigned int size = m_Text.getCharacterSize();

                // Calculate a better size for the text
                float NewSize = m_TextureNormal_M->getSize().y * curScale.y * 0.9f;

                // Calculate the scale factor
                float textScale = NewSize / size;

                // The calculation will be different when the image is split
                if (m_SplitImage)
                {
                    float buttonWidth;

                    // Check if the middle image is draw
                    if ((curScale.y * (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x))
                        < curScale.x * (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x))
                    {
                        buttonWidth = (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x) * curScale.x;
                    }
                    else
                        buttonWidth = (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * curScale.y;


                    // Check if the text isn't too width
                    if ((m_Text.getGlobalBounds().width * textScale) > (buttonWidth * 0.8f))
                    {
                        // Change the scale
                        textScale = (buttonWidth * 0.8f) / m_Text.getGlobalBounds().width;
                    }

                    // Calculate the new position for the text
                    rect.left = ((buttonWidth - (rect.width * textScale)) * 0.5f) - (rect.left * textScale);
                    rect.top = (((m_TextureNormal_M->getSize().y * curScale.y) - (rect.height * textScale)) * 0.5f) - (rect.top * textScale);
                }
                else // The image isn't split
                {
                    // Check if the text isn't too width
                    if ((m_Text.getGlobalBounds().width * textScale) > (m_TextureNormal_M->getSize().x * curScale.x * 0.8f))
                    {
                        // Change the scale
                        textScale = (m_TextureNormal_M->getSize().x * curScale.x * 0.8f) / m_Text.getGlobalBounds().width;
                    }

                    // Calculate the new position for the text
                    rect.left = (((m_TextureNormal_M->getSize().x * curScale.x) - (rect.width * textScale)) * 0.5f) - (rect.left * textScale);
                    rect.top = (((m_TextureNormal_M->getSize().y * curScale.y) - (rect.height * textScale)) * 0.5f) - (rect.top * textScale);
                }

                // Set the new position
                states.transform.translate(rect.left, rect.top);

                // Set the scale for the text
                states.transform.scale(textScale, textScale);
            }

            // Draw the text
            target.draw(m_Text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

