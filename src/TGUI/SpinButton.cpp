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

    SpinButton::SpinButton() :
    verticalScroll        (true),
    m_Minimum             (0),
    m_Maximum             (10),
    m_Value               (0),
    m_SeparateHoverImage  (false),
    m_MouseHoverOnTopArrow(false),
    m_MouseDownOnTopArrow (false),
    m_TextureNormal       (NULL),
    m_TextureHover        (NULL),
    m_LoadedPathname      ("")
    {
        m_ObjectType = spinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton(const SpinButton& copy) :
    OBJECT                (copy),
    verticalScroll        (copy.verticalScroll),
    m_Minimum             (copy.m_Minimum),
    m_Maximum             (copy.m_Maximum),
    m_Value               (copy.m_Value),
    m_SeparateHoverImage  (copy.m_SeparateHoverImage),
    m_MouseHoverOnTopArrow(copy.m_MouseHoverOnTopArrow),
    m_MouseDownOnTopArrow (copy.m_MouseDownOnTopArrow),
    m_LoadedPathname      (copy.m_LoadedPathname)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal, m_TextureNormal))  m_SpriteNormal.setTexture(*m_TextureNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureHover, m_TextureHover))    m_SpriteHover.setTexture(*m_TextureHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::~SpinButton()
    {
        // Remove all the textures
        if (m_TextureNormal != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal);
        if (m_TextureHover != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton& SpinButton::operator= (const SpinButton& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            SpinButton temp(right);
            this->OBJECT::operator=(right);

            std::swap(verticalScroll,         temp.verticalScroll);
            std::swap(m_Minimum,              temp.m_Minimum);
            std::swap(m_Maximum,              temp.m_Maximum);
            std::swap(m_Value,                temp.m_Value);
            std::swap(m_SeparateHoverImage,   temp.m_SeparateHoverImage);
            std::swap(m_MouseHoverOnTopArrow, temp.m_MouseHoverOnTopArrow);
            std::swap(m_MouseDownOnTopArrow,  temp.m_MouseDownOnTopArrow);
            std::swap(m_TextureNormal,        temp.m_TextureNormal);
            std::swap(m_TextureHover,         temp.m_TextureHover);
            std::swap(m_SpriteNormal,         temp.m_SpriteNormal);
            std::swap(m_SpriteHover,          temp.m_SpriteHover);
            std::swap(m_LoadedPathname,       temp.m_LoadedPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton* SpinButton::clone()
    {
        return new SpinButton(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::load(const std::string pathname)
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
            TGUI_OUTPUT((((std::string("TGUI error: Failed to open ")).append(m_LoadedPathname)).append("info.txt")).c_str());
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
            else if (property.compare("extension") == 0)
            {
                imageExtension = value;
            }
            else if (property.compare("verticalscroll") == 0)
            {
                if (value.compare("true") == 0)
                    verticalScroll = true;
                else if (value.compare("false") == 0)
                    verticalScroll = false;
            }
            else if (property.compare("separatehoverimage") == 0)
            {
                if (value.compare("true") == 0)
                    m_SeparateHoverImage = true;
                else if (value.compare("false") == 0)
                    m_SeparateHoverImage = false;
            }
            else
                TGUI_OUTPUT("TGUI warning: Option not recognised: \"" + property + "\".");
        }

        // Close the info file
        infoFile.closeFile();

        // Remove all textures if they were loaded before
        if (m_TextureNormal != NULL) TGUI_TextureManager.removeTexture(m_TextureNormal);
        if (m_TextureHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureHover);


        // load the required texture
        if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Normal." + imageExtension, m_TextureNormal))
            m_SpriteNormal.setTexture(*m_TextureNormal, true);
        else
            return false;

        // load the optional textures
        if (m_ObjectPhase & ObjectPhase_Hover)
        {
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Hover." + imageExtension, m_TextureHover))
                 m_SpriteHover.setTexture(*m_TextureHover, true);
            else
                return false;
        }

        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the new scale factors
        if (verticalScroll)
            setScale(width / m_TextureNormal->getSize().x, height / (m_TextureNormal->getSize().y * 2.f));
        else
            setScale(width / (m_TextureNormal->getSize().y * 2.f), height / m_TextureNormal->getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u SpinButton::getSize() const
    {
        // Don't continue when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2u(0, 0);

        // Return the size of the track
        if (verticalScroll)
            return Vector2u(m_TextureNormal->getSize().x, m_TextureNormal->getSize().y * 2);
        else
            return Vector2u(m_TextureNormal->getSize().y * 2, m_TextureNormal->getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f SpinButton::getScaledSize() const
    {
        // Don't continue when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2f(0, 0);

        // Return the size of the track
        if (verticalScroll)
            return Vector2f(m_TextureNormal->getSize().x * getScale().x, m_TextureNormal->getSize().y * 2.f * getScale().y);
        else
            return Vector2f(m_TextureNormal->getSize().y * 2.f * getScale().x, m_TextureNormal->getSize().x * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string SpinButton::getLoadedPathname()
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_Minimum = minimum;

        // When the value is below the minimum then adjust it
        if (m_Value < m_Minimum)
            m_Value = m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_Maximum = maximum;
        else
            m_Maximum = 1;

        // When the value is above the maximum then adjust it
        if (m_Value > m_Maximum)
            m_Value = m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setValue(unsigned int value)
    {
        // Set the new value
        m_Value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_Value < m_Minimum)
            m_Value = m_Minimum;
        else if (m_Value > m_Maximum)
            m_Value = m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setVerticalScroll( bool vertical )
    {
        verticalScroll = vertical;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpinButton::getMinimum()
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpinButton::getMaximum()
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpinButton::getValue()
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::getVerticalScroll()
    {
        return verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::mouseOnObject(float x, float y)
    {
        // Don't do anything when the spin button wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the spin button
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
            return true;

        // The mouse is not on top of the spin button
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;

        // Check if the mouse is on top of the upper/right arrow
        if (verticalScroll)
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), getSize().y / 2.f)).contains(x, y))
                m_MouseDownOnTopArrow = true;
            else
                m_MouseDownOnTopArrow = false;
        }
        else
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x / 2.f, static_cast<float>(getSize().y))).contains(x, y))
                m_MouseDownOnTopArrow = false;
            else
                m_MouseDownOnTopArrow = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMouseReleased(float x, float y)
    {
        // Check if the mouse went down on the spin button
        if (m_MouseDown)
        {
            m_MouseDown = false;

            // Check if the arrow went down on the top/right arrow
            if (m_MouseDownOnTopArrow)
            {
                // Check if the mouse went up on the same arrow
                if (((verticalScroll == true)  && (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), getSize().y / 2.f)).contains(x, y)))
                 || ((verticalScroll == false) && (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x / 2.f, static_cast<float>(getSize().y))).contains(x, y) == false)))
                {
                    // Increment the value
                    if (m_Value < m_Maximum)
                        ++m_Value;
                    else
                        return;
                }
                else
                    return;
            }
            else // The mouse went down on the bottom/left arrow
            {
                // Check if the mouse went up on the same arrow
                if (((verticalScroll == true)  && (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), getSize().y / 2.f)).contains(x, y) == false))
                 || ((verticalScroll == false) && (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x / 2.f, static_cast<float>(getSize().y))).contains(x, y))))
                {
                    // Decrement the value
                    if (m_Value > m_Minimum)
                        --m_Value;
                    else
                        return;
                }
                else
                    return;
            }

            // Check if the user requested a callback
            if (callbackID)
            {
                Callback callback;
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::valueChanged;
                callback.value      = m_Value;
                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(float x, float y)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (verticalScroll)
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), getSize().y / 2.f)).contains(x, y))
                m_MouseHoverOnTopArrow = true;
            else
                m_MouseHoverOnTopArrow = false;
        }
        else
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x / 2.f, static_cast<float>(getSize().y))).contains(x, y))
                m_MouseHoverOnTopArrow = false;
            else
                m_MouseHoverOnTopArrow = true;
        }

        m_MouseHover = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::objectFocused()
    {
        // A spin button can't be focused
        m_Parent->unfocus(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the spin button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Adjust the transformation
        states.transform *= getTransform();

        // Check if the image is drawn in the same direction than it was loaded
        if (verticalScroll)
        {
            // Draw the normal image
            target.draw(m_SpriteNormal, states);

            // When the mouse is on top of the upper arrow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow)
                    target.draw(m_SpriteHover, states);
            }

            // Set the second arrow on the correct position
            states.transform.translate(0, 2.f * m_TextureNormal->getSize().y);

            // Flip the image
            states.transform.scale(1, -1);

            // Draw the normal image
            target.draw(m_SpriteNormal, states);

            // When the mouse is on top of the bottom arow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow == false)
                    target.draw(m_SpriteHover, states);
            }
        }
        else // The image is not drawn in the same direction than the loaded image
        {
            // Rotate the arrow
            states.transform.rotate(-90, static_cast<float>(m_TextureNormal->getSize().x), static_cast<float>(m_TextureNormal->getSize().y));

            // Set the left arrow on the correct position
            states.transform.translate(static_cast<float>(m_TextureNormal->getSize().y), 0);

            // Draw the normal image
            target.draw(m_SpriteNormal, states);

            // When the mouse is on top of the left arrow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow == false)
                    target.draw(m_SpriteHover, states);
            }

            // Set the right arrow on the correct position
            states.transform.translate(0, 2.f * m_TextureNormal->getSize().y);

            // Flip the image
            states.transform.scale(1, -1);

            // Draw the normal image
            target.draw(m_SpriteNormal, states);

            // When the mouse is on top of the right arow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow)
                    target.draw(m_SpriteHover, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
