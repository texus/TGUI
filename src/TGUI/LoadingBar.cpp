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

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::LoadingBar() :
    m_Minimum       (  0),
    m_Maximum       (100),
    m_Value         (  0),
    m_SplitImage    (false),
    m_TextureBack_L (NULL),
    m_TextureBack_M (NULL),
    m_TextureBack_R (NULL),
    m_TextureFront_L(NULL),
    m_TextureFront_M(NULL),
    m_TextureFront_R(NULL),
    m_TextSize      (0),
    m_LoadedPathname("")
    {
        m_ObjectType = loadingBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::LoadingBar(const LoadingBar& copy) :
    OBJECT          (copy),
    m_Minimum       (copy.m_Minimum),
    m_Maximum       (copy.m_Maximum),
    m_Value         (copy.m_Value),
    m_SplitImage    (copy.m_SplitImage),
    m_Size          (copy.m_Size),
    m_Text          (copy.m_Text),
    m_TextSize      (copy.m_TextSize),
    m_LoadedPathname(copy.m_LoadedPathname)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureBack_L, m_TextureBack_L))     m_SpriteBack_L.setTexture(*m_TextureBack_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureBack_M, m_TextureBack_M))     m_SpriteBack_M.setTexture(*m_TextureBack_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureBack_R, m_TextureBack_R))     m_SpriteBack_R.setTexture(*m_TextureBack_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFront_L, m_TextureFront_L))   m_SpriteFront_L.setTexture(*m_TextureFront_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFront_M, m_TextureFront_M))   m_SpriteFront_M.setTexture(*m_TextureFront_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFront_R, m_TextureFront_R))   m_SpriteFront_R.setTexture(*m_TextureFront_R);

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::~LoadingBar()
    {
        if (m_TextureBack_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureBack_L);
        if (m_TextureBack_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureBack_M);
        if (m_TextureBack_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureBack_R);

        if (m_TextureFront_L != NULL) TGUI_TextureManager.removeTexture(m_TextureFront_L);
        if (m_TextureFront_M != NULL) TGUI_TextureManager.removeTexture(m_TextureFront_M);
        if (m_TextureFront_R != NULL) TGUI_TextureManager.removeTexture(m_TextureFront_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar& LoadingBar::operator= (const LoadingBar& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            LoadingBar temp(right);
            this->OBJECT::operator=(right);

            std::swap(m_Minimum,        temp.m_Minimum);
            std::swap(m_Maximum,        temp.m_Maximum);
            std::swap(m_Value,          temp.m_Value);
            std::swap(m_SplitImage,     temp.m_SplitImage);
            std::swap(m_Size,           temp.m_Size);
            std::swap(m_TextureBack_L,  temp.m_TextureBack_L);
            std::swap(m_TextureBack_M,  temp.m_TextureBack_M);
            std::swap(m_TextureBack_R,  temp.m_TextureBack_R);
            std::swap(m_TextureFront_L, temp.m_TextureFront_L);
            std::swap(m_TextureFront_M, temp.m_TextureFront_M);
            std::swap(m_TextureFront_R, temp.m_TextureFront_R);
            std::swap(m_SpriteBack_L,   temp.m_SpriteBack_L);
            std::swap(m_SpriteBack_M,   temp.m_SpriteBack_M);
            std::swap(m_SpriteBack_R,   temp.m_SpriteBack_R);
            std::swap(m_SpriteFront_L,  temp.m_SpriteFront_L);
            std::swap(m_SpriteFront_M,  temp.m_SpriteFront_M);
            std::swap(m_SpriteFront_R,  temp.m_SpriteFront_R);
            std::swap(m_Text,           temp.m_Text);
            std::swap(m_TextSize,       temp.m_TextSize);
            std::swap(m_LoadedPathname, temp.m_LoadedPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::initialize()
    {
        m_Text.setFont(m_Parent->globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar* LoadingBar::clone()
    {
        return new LoadingBar(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool LoadingBar::load(const std::string& pathname)
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

        // Set some default settings
        m_SplitImage = false;
        std::string imageExtension = "png";

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            if (property.compare("splitimage") == 0)
            {
                // Find out if it is split or not
                if (value.compare("true") == 0)
                    m_SplitImage = true;
                else if (value.compare("false") == 0)
                    m_SplitImage = false;
            }
            else if (property.compare("extension") == 0)
            {
                imageExtension = value;
            }
        }

        // Close the info file
        infoFile.closeFile();

        // Remove all textures if they were loaded before
        if (m_TextureBack_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureBack_L);
        if (m_TextureBack_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureBack_M);
        if (m_TextureBack_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureBack_R);
        if (m_TextureFront_L != NULL) TGUI_TextureManager.removeTexture(m_TextureFront_L);
        if (m_TextureFront_M != NULL) TGUI_TextureManager.removeTexture(m_TextureFront_M);
        if (m_TextureFront_R != NULL) TGUI_TextureManager.removeTexture(m_TextureFront_R);

        // Load multiple images when the image is split
        if (m_SplitImage)
        {
            // load the required textures
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Back." + imageExtension, m_TextureBack_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Back." + imageExtension, m_TextureBack_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Back." + imageExtension, m_TextureBack_R))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Front." + imageExtension, m_TextureFront_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Front." + imageExtension, m_TextureFront_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Front." + imageExtension, m_TextureFront_R)))
            {
                m_SpriteBack_L.setTexture(*m_TextureBack_L, true);
                m_SpriteBack_M.setTexture(*m_TextureBack_M, true);
                m_SpriteBack_R.setTexture(*m_TextureBack_R, true);

                m_SpriteFront_L.setTexture(*m_TextureFront_L, true);
                m_SpriteFront_M.setTexture(*m_TextureFront_M, true);
                m_SpriteFront_R.setTexture(*m_TextureFront_R, true);

                // Set the size of the loading bar
                m_Size.x = static_cast<float>(m_TextureBack_L->getSize().x + m_TextureBack_M->getSize().x + m_TextureBack_R->getSize().x);
                m_Size.y = static_cast<float>(m_TextureBack_M->getSize().y);
            }
            else
                return false;
        }
        else // The image is not split
        {
            // load the required textures
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Back." + imageExtension, m_TextureBack_M))
            && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Front." + imageExtension, m_TextureFront_M)))
            {
                m_SpriteBack_M.setTexture(*m_TextureBack_M, true);
                m_SpriteFront_M.setTexture(*m_TextureFront_M, true);

                // Set the size of the loading bar
                m_Size = Vector2f(m_TextureBack_M->getSize());
            }
            else
                return false;
        }

        // Calculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();

        // Loading has succeeded
        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setSize(float width, float height)
    {
        // Don't do anything when the loading bar wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the loading bar
        m_Size.x = width;
        m_Size.y = height;

        // Recalculate the size of the front image
        recalculateSize();

        // Recalculate the text size
        setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u LoadingBar::getSize() const
    {
        if (m_Loaded)
            return Vector2u(m_Size);
        else
            return Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f LoadingBar::getScaledSize() const
    {
        // Don't continue when the loading bar wasn't loaded correctly
        if (m_Loaded)
            return Vector2f(m_Size.x * getScale().y, m_Size.y * getScale().y);
        else
            return Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string LoadingBar::getLoadedPathname() const
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_Minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_Minimum > m_Maximum)
            m_Maximum = m_Minimum;

        // When the value is below the minimum then adjust it
        if (m_Value < m_Minimum)
            m_Value = m_Minimum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        m_Maximum = maximum;

        // The maximum can never be below the minimum
        if (m_Maximum < m_Minimum)
            m_Minimum = m_Maximum;

        // When the value is above the maximum then adjust it
        if (m_Value > m_Maximum)
            m_Value = m_Maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setValue(unsigned int value)
    {
        // Set the new value
        m_Value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_Value < m_Minimum)
            m_Value = m_Minimum;
        else if (m_Value > m_Maximum)
            m_Value = m_Maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::incrementValue()
    {
        // When the value is still below the maximum then adjust it
        if (m_Value < m_Maximum)
        {
            ++m_Value;

            // The value has changed, so send a callback (if needed)
            if (callbackID > 0)
            {
                Callback callback;
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::valueChanged;
                callback.value      = m_Value;
                m_Parent->addCallback(callback);
            }
        }

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();

        // return the new value
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getMinimum() const
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setText(const sf::String& text)
    {
        // Don't do anything when the loading bar wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new text
        m_Text.setString(text);

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate a possible text size
            float size = m_Size.y * 0.85f;
            m_Text.setCharacterSize(static_cast<unsigned int>(size));
            m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));

            // Make sure that the text isn't too width
            if (m_Text.getGlobalBounds().width > (m_Size.x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_Text.setCharacterSize(static_cast<unsigned int>(size / (m_Text.getGlobalBounds().width / (m_Size.x * 0.8f))));
                m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String LoadingBar::getText() const
    {
        return m_Text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* LoadingBar::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& LoadingBar::getTextColor() const
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool LoadingBar::mouseOnObject(float x, float y)
    {
        // Don't do anything when the loading bar wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the button
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
            return true;
        else
        {
            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::recalculateSize()
    {
        // Don't calculate anything when the loading bar wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if the image is split
        if (m_SplitImage)
        {
            // Get the current scale
            float scalingY = m_Size.y / m_TextureBack_M->getSize().y;

            // Get the bounds of the sprites
            sf::FloatRect backBounds_L = m_SpriteBack_L.getLocalBounds();
            sf::FloatRect backBounds_M = m_SpriteBack_M.getLocalBounds();
            sf::FloatRect backBounds_R = m_SpriteBack_R.getLocalBounds();

            // Calculate the necessary sizes
            float totalWidth;
            float middleTextureWidth;
            float frontSize;

            // Check if the middle image is drawn
            if ((scalingY * (m_TextureBack_L->getSize().x + m_TextureBack_R->getSize().x)) < m_Size.x)
            {
                totalWidth = m_Size.x;
                middleTextureWidth = totalWidth - ((m_TextureBack_L->getSize().x + m_TextureBack_R->getSize().x) * scalingY);
            }
            else // The loading bar is too small
            {
                totalWidth = (m_TextureBack_L->getSize().x + m_TextureBack_R->getSize().x) * scalingY;
                middleTextureWidth = 0;
            }

            // Only change the width when not dividing by zero
            if ((m_Maximum - m_Minimum) > 0)
                frontSize = totalWidth * (m_Value / static_cast<float>(m_Maximum - m_Minimum));
            else
                frontSize = totalWidth;

            // Check if a part of the left piece should be visible
            if (frontSize > 0)
            {
                // Check if a piece of the middle part should be drawn
                if (frontSize > m_TextureBack_L->getSize().x * scalingY)
                {
                    // Check if a piece of the right part should be drawn
                    if (frontSize > (m_TextureBack_L->getSize().x * scalingY) + middleTextureWidth)
                    {
                        // Check if the bar is not full
                        if (frontSize < totalWidth)
                            backBounds_R.width = frontSize - ((m_TextureBack_L->getSize().x * scalingY) + middleTextureWidth);
                    }
                    else // Only a part of the middle image should be drawn
                    {
                        backBounds_M.width = (frontSize - (m_TextureBack_L->getSize().x * scalingY)) / middleTextureWidth * m_TextureBack_M->getSize().x;
                        backBounds_R.width = 0;
                    }
                }
                else // Only a part of the left piece should be drawn
                {
                    backBounds_L.width = frontSize;
                    backBounds_M.width = 0;
                    backBounds_R.width = 0;
                }
            }
            else // Nothing should be drawn
            {
                backBounds_L.width = 0;
                backBounds_M.width = 0;
                backBounds_R.width = 0;
            }

            m_SpriteFront_L.setTextureRect(sf::IntRect(backBounds_L));
            m_SpriteFront_M.setTextureRect(sf::IntRect(backBounds_M));
            m_SpriteFront_R.setTextureRect(sf::IntRect(backBounds_R));
        }
        else // The image is not split
        {
            // Calculate the size of the front sprite
            sf::IntRect frontBounds(m_SpriteBack_M.getLocalBounds());

            // Only change the width when not dividing by zero
            if ((m_Maximum - m_Minimum) > 0)
                frontBounds.width = static_cast<int>(m_TextureBack_M->getSize().x * (m_Value / static_cast<float>(m_Maximum - m_Minimum)));
            else
                frontBounds.width = static_cast<int>(m_TextureBack_M->getSize().x);

            // Set the size of the front image
            m_SpriteFront_M.setTextureRect(frontBounds);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the loading bar wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Apply the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Check if the image is split
        if (m_SplitImage)
        {
            // Get the scale the images
            float scalingY = m_Size.y / m_TextureBack_M->getSize().y;

            // Scale the image
            states.transform.scale(scalingY, scalingY);

            // Draw the left image of the loading bar
            target.draw(m_SpriteBack_L, states);
            target.draw(m_SpriteFront_L, states);

            // Check if the middle image may be drawn
            if ((scalingY * (m_TextureBack_L->getSize().x + m_TextureBack_R->getSize().x)) < m_Size.x)
            {
                // Put the middle image on the correct position
                states.transform.translate(m_SpriteBack_L.getGlobalBounds().width, 0);

                // Calculate the scale for our middle image
                float scaleX = (m_Size.x - ((m_TextureBack_L->getSize().x + m_TextureBack_R->getSize().x) * scalingY)) / m_TextureBack_M->getSize().x;

                // Set the scale for the middle image
                states.transform.scale(scaleX / scalingY, 1);

                // Draw the middle image
                target.draw(m_SpriteBack_M, states);
                target.draw(m_SpriteFront_M, states);

                // Put the right image on the correct position
                states.transform.translate(m_SpriteBack_M.getGlobalBounds().width, 0);

                // Set the scale for the right image
                states.transform.scale(scalingY / scaleX, 1);

                // Draw the right image
                target.draw(m_SpriteBack_R, states);
                target.draw(m_SpriteFront_R, states);
            }
            else // The loading bar isn't width enough, we will draw it at minimum size
            {
                // Put the right image on the correct position
                states.transform.translate(m_SpriteBack_L.getGlobalBounds().width, 0);

                // Draw the right image
                target.draw(m_SpriteBack_R, states);
                target.draw(m_SpriteFront_R, states);
            }
        }
        else // The image is not split
        {
            // Scale the image
            states.transform.scale(m_Size.x / m_TextureBack_M->getSize().x, m_Size.y / m_TextureBack_M->getSize().y);

            // Draw the loading bar
            target.draw(m_SpriteBack_M, states);
            target.draw(m_SpriteFront_M, states);
        }

        // Check if there is a text to draw
        if (m_Text.getString().isEmpty() == false)
        {
            // Reset the transformations
            states.transform = oldTransform;

            // Get the current size of the text, so that we can recalculate the position
            sf::FloatRect rect = m_Text.getGlobalBounds();

            // Calculate the new position for the text
            rect.left = (m_Size.x - rect.width) * 0.5f - rect.left;
            rect.top = (m_Size.y - rect.height) * 0.5f - rect.top;

            // Set the new position
            states.transform.translate(std::floor(rect.left + 0.5f), std::floor(rect.top + 0.5f));

            // Draw the text
            target.draw(m_Text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
