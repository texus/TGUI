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

    Checkbox::Checkbox() :
    allowTextClick    (true),
    m_Checked           (false),
    m_TextSize          (0),
    m_TextureUnchecked  (NULL),
    m_TextureChecked    (NULL),
    m_TextureMouseHover (NULL),
    m_TextureFocused    (NULL),
    m_LoadedPathname    ("")
    {
        m_ObjectType = checkbox;

        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::Checkbox(const Checkbox& copy) :
    OBJECT          (copy),
    allowTextClick  (copy.allowTextClick),
    m_Checked       (copy.m_Checked),
    m_Text          (copy.m_Text),
    m_TextSize      (copy.m_TextSize),
    m_LoadedPathname(copy.m_LoadedPathname)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureUnchecked, m_TextureUnchecked))   m_SpriteUnchecked.setTexture(*m_TextureUnchecked);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureChecked, m_TextureChecked))       m_SpriteChecked.setTexture(*m_TextureChecked);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover, m_TextureMouseHover)) m_SpriteMouseHover.setTexture(*m_TextureMouseHover);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused, m_TextureFocused))       m_SpriteFocused.setTexture(*m_TextureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::~Checkbox()
    {
        if (m_TextureUnchecked != NULL)   TGUI_TextureManager.removeTexture(m_TextureUnchecked);
        if (m_TextureChecked != NULL)     TGUI_TextureManager.removeTexture(m_TextureChecked);
        if (m_TextureMouseHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseHover);
        if (m_TextureFocused != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox& Checkbox::operator= (const Checkbox& right)
    {
        if (this != &right)
        {
            Checkbox temp(right);
            this->OBJECT::operator=(right);

            std::swap(allowTextClick,      temp.allowTextClick);
            std::swap(m_Checked,           temp.m_Checked);
            std::swap(m_Text,              temp.m_Text);
            std::swap(m_TextSize,          temp.m_TextSize);
            std::swap(m_TextureUnchecked,  temp.m_TextureUnchecked);
            std::swap(m_TextureChecked,    temp.m_TextureChecked);
            std::swap(m_TextureMouseHover, temp.m_TextureMouseHover);
            std::swap(m_TextureFocused,    temp.m_TextureFocused);
            std::swap(m_SpriteUnchecked,   temp.m_SpriteUnchecked);
            std::swap(m_SpriteChecked,     temp.m_SpriteChecked);
            std::swap(m_SpriteMouseHover,  temp.m_SpriteMouseHover);
            std::swap(m_SpriteFocused,     temp.m_SpriteFocused);
            std::swap(m_LoadedPathname,    temp.m_LoadedPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::initialize()
    {
        m_Text.setFont(m_Parent->globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox* Checkbox::clone()
    {
        return new Checkbox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Checkbox::load(const std::string pathname)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Make sure that the pathname isn't empty
        if (pathname.empty())
            return false;

        // Store the pathname
        m_LoadedPathname = pathname;

        // When the pathname does not end with a "/" then we will add it
        if (m_LoadedPathname[m_LoadedPathname.length()-1] != '/')
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

        std::string imageExtension = "png";

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            // Check what the property is
            if (property.compare("phases") == 0)
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

        // If the checkbox was loaded before then remove the old textures
        if (m_TextureUnchecked != NULL)   TGUI_TextureManager.removeTexture(m_TextureUnchecked);
        if (m_TextureChecked != NULL)     TGUI_TextureManager.removeTexture(m_TextureChecked);
        if (m_TextureMouseHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseHover);
        if (m_TextureFocused != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused);

        // load the required textures
        if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Checked." + imageExtension, m_TextureChecked))
         && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Unchecked." + imageExtension, m_TextureUnchecked)))
        {
            m_SpriteChecked.setTexture(*m_TextureChecked, true);
            m_SpriteUnchecked.setTexture(*m_TextureUnchecked, true);
        }
        else
            return false;

        bool error = false;

        // load the optional textures
        if (m_ObjectPhase & ObjectPhase_Focused)
        {
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Focus." + imageExtension, m_TextureFocused))
            {
                m_SpriteFocused.setTexture(*m_TextureFocused, true);
                m_AllowFocus = true;
            }
            else
                error = true;
        }

        if (m_ObjectPhase & ObjectPhase_Hover)
        {
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Hover." + imageExtension, m_TextureMouseHover))
                m_SpriteMouseHover.setTexture(*m_TextureMouseHover, true);
            else
                error = true;
        }

        // When there is no error we will return true
        m_Loaded = !error;
        return !error;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::setSize(float width, float height)
    {
        // Don't do anything when the checkbox wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the new scale factors
        setScale(width / m_TextureUnchecked->getSize().x, height / m_TextureUnchecked->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Checkbox::getSize() const
    {
        if (m_Loaded)
            return Vector2u(m_TextureUnchecked->getSize().x, m_TextureUnchecked->getSize().y);
        else
            return Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Checkbox::getScaledSize() const
    {
        if (m_Loaded)
            return Vector2f(m_TextureUnchecked->getSize().x * getScale().x, m_TextureUnchecked->getSize().y * getScale().y);
        else
            return Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Checkbox::getLoadedPathname()
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::check()
    {
        m_Checked = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::uncheck()
    {
        m_Checked = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Checkbox::isChecked()
    {
        return m_Checked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::setText(const std::string text)
    {
        // Don't do anything when the checkbox wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new text
        m_Text.setString(text);

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextureUnchecked->getSize().y);
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Checkbox::getText()
    {
        return m_Text.getString().toAnsiString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Checkbox::getTextFont()
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::setTextColor(const sf::Color& Color)
    {
        m_Text.setColor(Color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Checkbox::getTextColor()
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::setTextSize(const unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString().toAnsiString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Checkbox::getTextSize()
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Checkbox::mouseOnObject(float x, float y)
    {
        // Don't do anything when the checkbox wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the image
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
            return true;
        else
        {
            // Check if the mouse is on top of the text
            if (allowTextClick)
            {
                if (m_Text.getGlobalBounds().contains(x - (getPosition().x + (m_TextureUnchecked->getSize().x * 11.0f / 10.0f * getScale().x)), y - getPosition().y))
                    return true;
            }
        }

        // The mouse is not on top of the checkox
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::leftMousePressed(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        // Set the mouse down flag
        m_MouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::leftMouseReleased(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        // Check if we clicked on the button (not just mouse release)
        if (m_MouseDown == true)
        {
            // Check or uncheck the checkbox
            if (m_Checked)
                uncheck();
            else
                check();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::mouseClick;
                callback.checked    = m_Checked;
                m_Parent->addCallback(callback);
            }

            m_MouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::mouseMoved(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        m_MouseHover = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::keyPressed(sf::Keyboard::Key key)
    {
        // Check if the space key or the return key was pressed
        if (key == sf::Keyboard::Space)
        {
            // Check or uncheck the checkbox
            if (m_Checked)
                uncheck();
            else
                check();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::keyPress_Space;
                callback.checked    = m_Checked;
                m_Parent->addCallback(callback);
            }
        }
        else if (key == sf::Keyboard::Return)
        {
            // Check or uncheck the checkbox
            if (m_Checked)
                uncheck();
            else
                check();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::keyPress_Return;
                callback.checked    = m_Checked;
                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::objectFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_ObjectPhase & ObjectPhase_Focused) == 0)
            m_Parent->unfocus(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't do draw when the checkbox wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Adjust the transformation
        states.transform *= getTransform();

        // Check if the checkbox was checked
        if (m_Checked)
        {
            // The checked image may be a little bigger than the unchecked one, so put it on the right position
            states.transform.translate(0, m_SpriteUnchecked.getGlobalBounds().height - m_SpriteChecked.getGlobalBounds().height);

            // Draw the image of the checked checkbox
            target.draw(m_SpriteChecked, states);

            // Undo the translation
            states.transform.translate(0, m_SpriteChecked.getGlobalBounds().height - m_SpriteUnchecked.getGlobalBounds().height);
        }
        else // The checkbox was unchecked, just draw it
            target.draw(m_SpriteUnchecked, states);

        // When the checkbox is focused then draw an extra image
        if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
            target.draw(m_SpriteFocused, states);

        // When the mouse is on top of the checkbox then draw an extra image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_SpriteMouseHover, states);


        sf::FloatRect textBounds = m_Text.getGlobalBounds();

        // Set the position of the text
        states.transform.translate(m_TextureUnchecked->getSize().x * 11.0f / 10.0f,
                                   ((m_TextureUnchecked->getSize().y - textBounds.height) / 2.0f) - textBounds.top);

        // Change the scale to draw the text
        states.transform.scale(getScale().y / getScale().x, 1);

        // Draw the text
        target.draw(m_Text, states);

    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
