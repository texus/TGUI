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

    AnimatedButton::AnimatedButton() :
    m_LoadedPathname    (""),
    m_SeparateHoverImage(true),
    m_TextSize          (0),
    m_CurrentFrame      (1)
    {
        m_ObjectType = animatedButton;
        m_AnimatedObject = true;

        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedButton::AnimatedButton(const AnimatedButton& copy) :
    OBJECT               (copy),
    OBJECT_ANIMATION     (copy),
    m_DurationsNormal    (copy.m_DurationsNormal),
    m_DurationsMouseHover(copy.m_DurationsMouseHover),
    m_DurationsMouseDown (copy.m_DurationsMouseDown),
    m_DurationsFocused   (copy.m_DurationsFocused),
    m_LoadedPathname     (copy.m_LoadedPathname),
    m_SeparateHoverImage (copy.m_SeparateHoverImage),
    m_Text               (copy.m_Text),
    m_TextSize           (copy.m_TextSize),
    m_CurrentFrame       (copy.m_CurrentFrame)
    {
        unsigned int i;

        for (i=0; i<copy.m_TexturesNormal.size(); ++i)
        {
            m_TexturesNormal.push_back(NULL);
            m_SpritesNormal.push_back(sf::Sprite());
            if (TGUI_TextureManager.copyTexture(copy.m_TexturesNormal[i], m_TexturesNormal[i]))
                m_SpritesNormal.back().setTexture(*m_TexturesNormal[i]);
        }

        for (i=0; i<copy.m_TexturesMouseHover.size(); ++i)
        {
            m_TexturesMouseHover.push_back(NULL);
            m_SpritesMouseHover.push_back(sf::Sprite());
            if (TGUI_TextureManager.copyTexture(copy.m_TexturesMouseHover[i], m_TexturesMouseHover[i]))
                m_SpritesMouseHover.back().setTexture(*m_TexturesMouseHover[i]);
        }

        for (i=0; i<copy.m_TexturesMouseDown.size(); ++i)
        {
            m_TexturesMouseDown.push_back(NULL);
            m_SpritesMouseDown.push_back(sf::Sprite());
            if (TGUI_TextureManager.copyTexture(copy.m_TexturesMouseDown[i], m_TexturesMouseDown[i]))
                m_SpritesMouseDown.back().setTexture(*m_TexturesMouseDown[i]);
        }

        for (i=0; i<copy.m_TexturesFocused.size(); ++i)
        {
            m_TexturesFocused.push_back(NULL);
            m_SpritesFocused.push_back(sf::Sprite());
            if (TGUI_TextureManager.copyTexture(copy.m_TexturesFocused[i], m_TexturesFocused[i]))
                m_SpritesFocused.back().setTexture(*m_TexturesFocused[i]);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedButton::~AnimatedButton()
    {
        unsigned int i;

        for (i=0; i<m_TexturesNormal.size(); ++i)
        {
            if (m_TexturesNormal[i] != NULL)
                TGUI_TextureManager.removeTexture(m_TexturesNormal[i]);
        }

        for (i=0; i<m_TexturesMouseHover.size(); ++i)
        {
            if (m_TexturesMouseHover[i] != NULL)
                TGUI_TextureManager.removeTexture(m_TexturesMouseHover[i]);
        }

        for (i=0; i<m_TexturesMouseDown.size(); ++i)
        {
            if (m_TexturesMouseDown[i] != NULL)
                TGUI_TextureManager.removeTexture(m_TexturesMouseDown[i]);
        }

        for (i=0; i<m_TexturesFocused.size(); ++i)
        {
            if (m_TexturesFocused[i] != NULL)
                TGUI_TextureManager.removeTexture(m_TexturesFocused[i]);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedButton& AnimatedButton::operator= (const AnimatedButton& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            AnimatedButton temp(right);
            this->OBJECT::operator=(right);
            this->OBJECT_ANIMATION::operator=(right);

            std::swap(m_TexturesNormal,      temp.m_TexturesNormal);
            std::swap(m_TexturesMouseHover,  temp.m_TexturesMouseHover);
            std::swap(m_TexturesMouseDown,   temp.m_TexturesMouseDown);
            std::swap(m_TexturesFocused,     temp.m_TexturesFocused);
            std::swap(m_SpritesNormal,       temp.m_SpritesNormal);
            std::swap(m_SpritesMouseHover,   temp.m_SpritesMouseHover);
            std::swap(m_SpritesMouseDown,    temp.m_SpritesMouseDown);
            std::swap(m_SpritesFocused,      temp.m_SpritesFocused);
            std::swap(m_DurationsNormal,     temp.m_DurationsNormal);
            std::swap(m_DurationsMouseHover, temp.m_DurationsMouseHover);
            std::swap(m_DurationsMouseDown,  temp.m_DurationsMouseDown);
            std::swap(m_DurationsFocused,    temp.m_DurationsFocused);
            std::swap(m_LoadedPathname,      temp.m_LoadedPathname);
            std::swap(m_SeparateHoverImage,  temp.m_SeparateHoverImage);
            std::swap(m_Text,                temp.m_Text);
            std::swap(m_TextSize,            temp.m_TextSize);
            std::swap(m_CurrentFrame,        temp.m_CurrentFrame);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::initialize()
    {
        m_Text.setFont(m_Parent->globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedButton* AnimatedButton::clone()
    {
        return new AnimatedButton(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedButton::load(const std::string pathname)
    {
        unsigned int i;

        // If the button was loaded before then remove the old textures first
        for (i=0; i<m_TexturesNormal.size(); ++i)      if (m_TexturesNormal[i] != NULL)     TGUI_TextureManager.removeTexture(m_TexturesNormal[i]);
        for (i=0; i<m_TexturesMouseHover.size(); ++i)  if (m_TexturesMouseHover[i] != NULL) TGUI_TextureManager.removeTexture(m_TexturesMouseHover[i]);
        for (i=0; i<m_TexturesMouseDown.size(); ++i)   if (m_TexturesMouseDown[i] != NULL)  TGUI_TextureManager.removeTexture(m_TexturesMouseDown[i]);
        for (i=0; i<m_TexturesFocused.size(); ++i)     if (m_TexturesFocused[i] != NULL)    TGUI_TextureManager.removeTexture(m_TexturesFocused[i]);

        m_TexturesNormal.clear();
        m_TexturesMouseHover.clear();
        m_TexturesMouseDown.clear();
        m_TexturesFocused.clear();
        m_SpritesNormal.clear();
        m_SpritesMouseHover.clear();
        m_SpritesMouseDown.clear();
        m_SpritesFocused.clear();
        m_DurationsNormal.clear();
        m_DurationsMouseHover.clear();
        m_DurationsMouseDown.clear();
        m_DurationsFocused.clear();

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
            TGUI_OUTPUT((((std::string("TGUI error: Failed to open ")).append(m_LoadedPathname)).append("info.txt")).c_str());
            return false;
        }

        std::string property;
        std::string value;

        // Set some default settings
        std::string imageExtension = "png";
        unsigned int defaultFrameDuration = 0;

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            // Check what the property is
            if (property.compare("separatehoverimage") == 0)
            {
                if (value.compare("true") == 0)
                    m_SeparateHoverImage = true;
                else if (value.compare("false") == 0)
                    m_SeparateHoverImage = false;
            }
            else if (property.compare("textcolor") == 0)
            {
                m_Text.setColor(extractColor(value));
            }
            else if (property.compare("frameduration") == 0)
            {
                defaultFrameDuration = atoi(value.c_str());

                for (i=0; i<m_DurationsNormal.size(); ++i)
                    m_DurationsNormal[i] = defaultFrameDuration;

                for (i=0; i<m_DurationsMouseHover.size(); ++i)
                    m_DurationsMouseHover[i] = defaultFrameDuration;

                for (i=0; i<m_DurationsMouseDown.size(); ++i)
                    m_DurationsMouseDown[i] = defaultFrameDuration;

                for (i=0; i<m_DurationsFocused.size(); ++i)
                    m_DurationsFocused[i] = defaultFrameDuration;
            }
            else if (property.substr(0, 11).compare("normalimage") == 0)
            {
                property.erase(0, 11);
                unsigned int id = getLoadingID(property);

                // Strip the quotes from the filename
                if (value.length() > 2)
                {
                    if ((value[0] == '"') && (value[value.length()-1] == '"'))
                        value = value.substr(1, value.length()-2);
                    else
                    {
                        TGUI_OUTPUT("TGUI error: Image filename is missing quotes.");
                        return false;
                    }
                }
                else
                {
                    TGUI_OUTPUT("TGUI error: Image filename was too short. Did you forget the quotes?");
                    return false;
                }

                // Load the texture
                sf::Texture* texture;
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + value, texture) == false)
                {
                    TGUI_OUTPUT("TGUI error: Failed to load the image: \"" + m_LoadedPathname + value + "\".");
                    return false;
                }

                if (id)
                {
                    // Check if the space isn't reserved yet
                    if (m_TexturesNormal.size() == id - 1)
                    {
                        m_TexturesNormal.push_back(texture);
                        m_SpritesNormal.push_back(sf::Sprite(*texture));
                        m_DurationsNormal.push_back(defaultFrameDuration);
                    }
                    else
                        m_TexturesNormal[id-1] = texture;
                }
                else
                    return false;
            }
            else if (property.substr(0, 10).compare("hoverimage") == 0)
            {
                m_ObjectPhase |= ObjectPhase_Hover;

                property.erase(0, 10);
                unsigned int id = getLoadingID(property);

                // Strip the quotes from the filename
                if (value.length() > 2)
                {
                    if ((value[0] == '"') && (value[value.length()-1] == '"'))
                        value = value.substr(1, value.length()-2);
                    else
                    {
                        TGUI_OUTPUT("TGUI error: Image filename is missing quotes.");
                        return false;
                    }
                }
                else
                {
                    TGUI_OUTPUT("TGUI error: Image filename was too short. Did you forget the quotes?");
                    return false;
                }

                // Load the texture
                sf::Texture* texture;
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + value, texture) == false)
                {
                    TGUI_OUTPUT("TGUI error: Failed to load the image: \"" + m_LoadedPathname + value + "\".");
                    return false;
                }

                if (id)
                {
                    // Check if the space isn't reserved yet
                    if (m_TexturesMouseHover.size() == id - 1)
                    {
                        m_TexturesMouseHover.push_back(texture);
                        m_SpritesMouseHover.push_back(sf::Sprite(*texture));
                        m_DurationsMouseHover.push_back(defaultFrameDuration);
                    }
                    else
                        m_TexturesMouseHover[id-1] = texture;
                }
                else
                    return false;
            }
            else if (property.substr(0, 9).compare("downimage") == 0)
            {
                m_ObjectPhase |= ObjectPhase_MouseDown;

                property.erase(0, 9);
                unsigned int id = getLoadingID(property);

                // Strip the quotes from the filename
                if (value.length() > 2)
                {
                    if ((value[0] == '"') && (value[value.length()-1] == '"'))
                        value = value.substr(1, value.length()-2);
                    else
                    {
                        TGUI_OUTPUT("TGUI error: Image filename is missing quotes.");
                        return false;
                    }
                }
                else
                {
                    TGUI_OUTPUT("TGUI error: Image filename was too short. Did you forget the quotes?");
                    return false;
                }

                // Load the texture
                sf::Texture* texture;
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + value, texture) == false)
                {
                    TGUI_OUTPUT("TGUI error: Failed to load the image: \"" + m_LoadedPathname + value + "\".");
                    return false;
                }

                if (id)
                {
                    // Check if the space isn't reserved yet
                    if (m_TexturesMouseDown.size() == id - 1)
                    {
                        m_TexturesMouseDown.push_back(texture);
                        m_SpritesMouseDown.push_back(sf::Sprite(*texture));
                        m_DurationsMouseDown.push_back(defaultFrameDuration);
                    }
                    else
                        m_TexturesMouseDown[id-1] = texture;
                }
                else
                    return false;
            }
            else if (property.substr(0, 10).compare("focusimage") == 0)
            {
                property.erase(0, 10);
                unsigned int id = getLoadingID(property);

                // Strip the quotes from the filename
                if (value.length() > 2)
                {
                    if ((value[0] == '"') && (value[value.length()-1] == '"'))
                        value = value.substr(1, value.length()-2);
                    else
                    {
                        TGUI_OUTPUT("TGUI error: Image filename is missing quotes.");
                        return false;
                    }
                }
                else
                {
                    TGUI_OUTPUT("TGUI error: Image filename was too short. Did you forget the quotes?");
                    return false;
                }

                // Load the texture
                sf::Texture* texture;
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + value, texture) == false)
                {
                    TGUI_OUTPUT("TGUI error: Failed to load the image: \"" + m_LoadedPathname + value + "\".");
                    return false;
                }

                if (id)
                {
                    // Check if the space isn't reserved yet
                    if (m_TexturesFocused.size() == id - 1)
                    {
                        m_TexturesFocused.push_back(texture);
                        m_SpritesFocused.push_back(sf::Sprite(*texture));
                        m_DurationsFocused.push_back(defaultFrameDuration);
                    }
                    else
                        m_TexturesFocused[id-1] = texture;
                }
                else
                    return false;
            }
            else if (property.substr(0, 14).compare("normalduration") == 0)
            {
                property.erase(0, 14);
                unsigned int id = getLoadingID(property);

                if (m_DurationsNormal.size() == id - 1)
                {
                    m_TexturesNormal.push_back(NULL);
                    m_SpritesNormal.push_back(sf::Sprite());
                    m_DurationsNormal.push_back(atoi(value.c_str()));
                }
                else
                    m_DurationsNormal[id-1] = atoi(value.c_str());
            }
            else if (property.substr(0, 13).compare("hoverduration") == 0)
            {
                property.erase(0, 13);
                unsigned int id = getLoadingID(property);

                if (m_DurationsMouseHover.size() == id - 1)
                {
                    m_TexturesMouseHover.push_back(NULL);
                    m_SpritesMouseHover.push_back(sf::Sprite());
                    m_DurationsMouseHover.push_back(atoi(value.c_str()));
                }
                else
                    m_DurationsMouseHover[id-1] = atoi(value.c_str());
            }
            else if (property.substr(0, 12).compare("downduration") == 0)
            {
                property.erase(0, 12);
                unsigned int id = getLoadingID(property);

                if (m_DurationsMouseDown.size() == id - 1)
                {
                    m_TexturesMouseDown.push_back(NULL);
                    m_SpritesMouseDown.push_back(sf::Sprite());
                    m_DurationsMouseDown.push_back(atoi(value.c_str()));
                }
                else
                    m_DurationsMouseDown[id-1] = atoi(value.c_str());
            }
            else if (property.substr(0, 13).compare("focusduration") == 0)
            {
                property.erase(0, 14);
                unsigned int id = getLoadingID(property);

                if (m_DurationsFocused.size() == id - 1)
                {
                    m_TexturesFocused.push_back(NULL);
                    m_SpritesFocused.push_back(sf::Sprite());
                    m_DurationsFocused.push_back(atoi(value.c_str()));
                }
                else
                    m_DurationsFocused[id-1] = atoi(value.c_str());
            }
            else
                TGUI_OUTPUT("TGUI warning: Option not recognised: \"" + property + "\".");
        }

        // Close the info file
        infoFile.closeFile();

        // At least one normal image should be loaded
        if (m_TexturesNormal.empty() == false)
        {
            if (m_TexturesNormal[0] != NULL)
            {
                m_Loaded = true;
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::setSize(float width, float height)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        setScale(width / m_TexturesNormal[0]->getSize().x, height / m_TexturesNormal[0]->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u AnimatedButton::getSize() const
    {
        // Don't continue when the button wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2u(0, 0);

        return Vector2u(m_TexturesNormal[0]->getSize().x, m_TexturesNormal[0]->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f AnimatedButton::getScaledSize() const
    {
        // Don't continue when the button wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2f(0, 0);

        return Vector2f(m_TexturesNormal[0]->getSize().x * getScale().x, m_TexturesNormal[0]->getSize().y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string AnimatedButton::getLoadedPathname()
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::setText(const std::string text)
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
            float size = m_TexturesNormal[0]->getSize().y * 0.9f;
            m_Text.setCharacterSize(static_cast<unsigned int>(size));


            // We must make sure that the text isn't too width
            if (m_Text.getGlobalBounds().width > (m_TexturesNormal[0]->getSize().x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_Text.setCharacterSize(static_cast<unsigned int> (size / (m_Text.getGlobalBounds().width / (m_TexturesNormal[0]->getSize().x * 0.8f))));
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(static_cast<unsigned int>(m_TextSize * getScale().y));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string AnimatedButton::getText()
    {
        return m_Text.getString().toAnsiString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* AnimatedButton::getTextFont()
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& AnimatedButton::getTextColor()
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::setTextSize(const unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString().toAnsiString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int AnimatedButton::getTextSize()
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::setFrame(unsigned int frame)
    {
        if (frame == 0)
            frame = 1;

        if (m_Loaded == false)
            return;

        // Check in what phase the button is currently
        if (m_MouseDown == true)
        {
            if (frame > m_TexturesMouseDown.size())
                m_CurrentFrame = m_TexturesMouseDown.size();
            else
                m_CurrentFrame = frame;
        }
        else if (m_MouseHover == true)
        {
            if (frame > m_TexturesMouseHover.size())
                m_CurrentFrame = m_TexturesMouseHover.size();
            else
                m_CurrentFrame = frame;
        }
        else
        {
            if (frame > m_TexturesNormal.size())
                m_CurrentFrame = m_TexturesNormal.size();
            else
                m_CurrentFrame = frame;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedButton::mouseOnObject(float x, float y)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the button
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
            return true;
        else
        {
            if (m_MouseHover == true)
            {
                m_MouseHover = false;
                m_CurrentFrame = 1;
            }
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::leftMousePressed(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        // Set the mouse down flag
        m_MouseDown = true;
        m_CurrentFrame = 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::leftMouseReleased(float x, float y)
    {
        // Check if we clicked on the button (not just mouse release)
        if (m_MouseDown == true)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.object      = this;
                callback.callbackID  = callbackID;
                callback.trigger     = Callback::mouseClick;
                callback.mouseButton = sf::Mouse::Left;
                callback.mouseX      = x - getPosition().x;
                callback.mouseY      = y - getPosition().y;
                m_Parent->addCallback(callback);
            }

            m_MouseDown = false;
            m_CurrentFrame = 1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::mouseMoved(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        if (m_MouseHover == false)
        {
            m_MouseHover = true;
            m_CurrentFrame = 1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::keyPressed(sf::Keyboard::Key key)
    {
        // Check if the space key or the return key was pressed
        if (key == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.object     = this;
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
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::keyPress_Return;
                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::objectFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_ObjectPhase & ObjectPhase_Focused) == 0)
            m_Parent->unfocus(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::mouseNotOnObject()
    {
        if (m_MouseHover == true)
        {
            m_MouseHover = false;
            m_CurrentFrame = 1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::mouseNoLongerDown()
    {
        if (m_MouseDown == true)
        {
            m_MouseDown = false;
            m_CurrentFrame = 1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int AnimatedButton::getLoadingID(std::string property)
    {
        // Get the image id
        unsigned int id = atoi(property.c_str());
        if (id == 0)
        {
            TGUI_OUTPUT("TGUI warning: Image has id 0 which is not allowed, id 1 will be used instead.");
            id = 1;
        }

        // Check if the id is too big
        if (m_TexturesNormal.size() < id - 1)
        {
            TGUI_OUTPUT("TGUI error: Image id is too high. Were all lower ids defined earlier?");
            return 0;
        }

        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::update()
    {
        if (m_Loaded == false)
            return;

        // Check in what phase the button is currently
        if (m_MouseDown == true)
        {
            // Check if the next frame should be displayed
            while (m_AnimationTimeElapsed.asMilliseconds() > m_DurationsMouseDown[m_CurrentFrame-1])
            {
                // Make sure the frame duration isn't 0
                if (m_DurationsMouseDown[m_CurrentFrame-1] > 0)
                {
                    // Decrease the elapsed time
                    m_AnimationTimeElapsed -= sf::milliseconds(m_DurationsMouseDown[m_CurrentFrame-1]);

                    // Make the next frame visible
                    if (m_CurrentFrame < m_TexturesMouseDown.size())
                        ++m_CurrentFrame;
                    else
                        m_CurrentFrame = 1;
                }
                else // The frame has to remain visible
                    m_AnimationTimeElapsed = sf::Time();
            }
        }
        else if (m_MouseHover == true)
        {
            // Check if the next frame should be displayed
            while (m_AnimationTimeElapsed.asMilliseconds() > m_DurationsMouseHover[m_CurrentFrame-1])
            {
                // Make sure the frame duration isn't 0
                if (m_DurationsMouseHover[m_CurrentFrame-1] > 0)
                {
                    // Decrease the elapsed time
                    m_AnimationTimeElapsed -= sf::milliseconds(m_DurationsMouseHover[m_CurrentFrame-1]);

                    // Make the next frame visible
                    if (m_CurrentFrame < m_TexturesMouseHover.size())
                        ++m_CurrentFrame;
                    else
                        m_CurrentFrame = 1;
                }
                else // The frame has to remain visible
                    m_AnimationTimeElapsed = sf::Time();
            }
        }
        else
        {
            // Check if the next frame should be displayed
            while (m_AnimationTimeElapsed.asMilliseconds() > m_DurationsNormal[m_CurrentFrame-1])
            {
                // Make sure the frame duration isn't 0
                if (m_DurationsNormal[m_CurrentFrame-1] > 0)
                {
                    // Decrease the elapsed time
                    m_AnimationTimeElapsed -= sf::milliseconds(m_DurationsNormal[m_CurrentFrame-1]);

                    // Make the next frame visible
                    if (m_CurrentFrame < m_TexturesNormal.size())
                        ++m_CurrentFrame;
                    else
                        m_CurrentFrame = 1;
                }
                else // The frame has to remain visible
                    m_AnimationTimeElapsed = sf::Time();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Adjust the transformation
        states.transform *= getTransform();

        // Check if there is a separate hover image
        if (m_SeparateHoverImage)
        {
            // Draw the correct image
            if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
            {
                target.draw(m_SpritesMouseDown[m_CurrentFrame-1], states);
            }
            else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                target.draw(m_SpritesMouseHover[m_CurrentFrame-1], states);
            }
            else
                target.draw(m_SpritesNormal[m_CurrentFrame-1], states);
        }
        else
        {
            // Draw the button
            if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                target.draw(m_SpritesMouseDown[m_CurrentFrame-1], states);
            else
                target.draw(m_SpritesNormal[m_CurrentFrame-1], states);

            // When the mouse is on top of the button then draw an extra image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                target.draw(m_SpritesMouseHover[m_CurrentFrame-1], states);
        }

        // When the button is focused then draw an extra image
        if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
            target.draw(m_SpritesFocused[m_CurrentFrame-1], states);

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
                // Calculate the new position for the text
                rect.left = (((m_TexturesNormal[0]->getSize().x * curScale.x) - rect.width) * 0.5f) - rect.left;
                rect.top = (((m_TexturesNormal[0]->getSize().y * curScale.y) - rect.height) * 0.5f) - rect.top;

                // Set the new position
                states.transform.translate(static_cast<float>(static_cast<int>(rect.left + 0.5f)), static_cast<float>(static_cast<int>(rect.top + 0.5f)));
            }
            else // autoscale
            {
                // Get the text size
                unsigned int size = m_Text.getCharacterSize();

                // Calculate a better size for the text
                float NewSize = m_TexturesNormal[0]->getSize().y * curScale.y * 0.9f;

                // Calculate the scale factor
                float textScale = NewSize / size;

                // Check if the text isn't too width
                if ((m_Text.getGlobalBounds().width * textScale) > (m_TexturesNormal[0]->getSize().x * curScale.x * 0.8f))
                {
                    // Change the scale
                    textScale = (m_TexturesNormal[0]->getSize().x * curScale.x * 0.8f) / m_Text.getGlobalBounds().width;
                }

                // Calculate the new position for the text
                rect.left = (((m_TexturesNormal[0]->getSize().x * curScale.x) - (rect.width * textScale)) * 0.5f) - (rect.left * textScale);
                rect.top = (((m_TexturesNormal[0]->getSize().y * curScale.y) - (rect.height * textScale)) * 0.5f) - (rect.top * textScale);

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

