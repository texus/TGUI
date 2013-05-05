/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/ClickableObject.hpp>
#include <TGUI/SpinButton.hpp>

///!!!  TODO: Instead of ignoring the down arrow, use it instead of just flipping the up arrow.
///!!!  TODO: Allow images next to each other (e.g. plus and minus icon).

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton() :
    m_VerticalScroll      (true),
    m_Minimum             (0),
    m_Maximum             (10),
    m_Value               (0),
    m_SeparateHoverImage  (false),
    m_MouseHoverOnTopArrow(false),
    m_MouseDownOnTopArrow (false)
    {
        m_Callback.objectType = Type_SpinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton(const SpinButton& copy) :
    ClickableObject       (copy),
    m_VerticalScroll      (copy.m_VerticalScroll),
    m_Minimum             (copy.m_Minimum),
    m_Maximum             (copy.m_Maximum),
    m_Value               (copy.m_Value),
    m_SeparateHoverImage  (copy.m_SeparateHoverImage),
    m_MouseHoverOnTopArrow(copy.m_MouseHoverOnTopArrow),
    m_MouseDownOnTopArrow (copy.m_MouseDownOnTopArrow)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpNormal, m_TextureArrowUpNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpHover, m_TextureArrowUpHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownNormal, m_TextureArrowDownNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownHover, m_TextureArrowDownHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::~SpinButton()
    {
        if (m_TextureArrowUpNormal.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton& SpinButton::operator= (const SpinButton& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            SpinButton temp(right);
            this->ClickableObject::operator=(right);

            std::swap(m_VerticalScroll,         temp.m_VerticalScroll);
            std::swap(m_Minimum,                temp.m_Minimum);
            std::swap(m_Maximum,                temp.m_Maximum);
            std::swap(m_Value,                  temp.m_Value);
            std::swap(m_SeparateHoverImage,     temp.m_SeparateHoverImage);
            std::swap(m_MouseHoverOnTopArrow,   temp.m_MouseHoverOnTopArrow);
            std::swap(m_MouseDownOnTopArrow,    temp.m_MouseDownOnTopArrow);
            std::swap(m_TextureArrowUpNormal,   temp.m_TextureArrowUpNormal);
            std::swap(m_TextureArrowUpHover,    temp.m_TextureArrowUpHover);
            std::swap(m_TextureArrowDownNormal, temp.m_TextureArrowDownNormal);
            std::swap(m_TextureArrowDownHover,  temp.m_TextureArrowDownHover);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton* SpinButton::clone()
    {
        return new SpinButton(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::load(const std::string& configFileFilename)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If the button was loaded before then remove the old textures first
        if (m_TextureArrowUpNormal.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != NULL) TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);

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
        if (!configFile.read("SpinButton", properties, values))
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

            if (property == "separatehoverimage")
            {
                m_SeparateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "arrowupnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpNormalImage in section SpinButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowuphoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpHoverImage in section SpinButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowdownnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownNormalImage in section SpinButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowdownhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownHoverImage in section SpinButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section SpinButton in " + configFileFilename + ".");
        }

        // Make sure the required textures were loaded
        if ((m_TextureArrowUpNormal.data != NULL) && (m_TextureArrowDownNormal.data != NULL))
        {
            m_Size.x = m_TextureArrowUpNormal.getSize().x;
            m_Size.y = m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y;
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the spin button. Is the SpinButton section in " + configFileFilename + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if ((m_TextureArrowUpHover.data != NULL) && (m_TextureArrowDownHover.data != NULL))
        {
            m_ObjectPhase |= ObjectPhase_Hover;
        }

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setSize(float width, float height)
    {
        // Don't do anything when the spin button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Store the new size
        m_Size.x = width;
        m_Size.y = height;

        // A negative size is not allowed for this object
        if (m_Size.x < 0) m_Size.x = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;
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

    unsigned int SpinButton::getMinimum() const
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpinButton::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpinButton::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setVerticalScroll(bool verticalScroll)
    {
        m_VerticalScroll = verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinButton::getVerticalScroll() const
    {
        return m_VerticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;

        // Check if the mouse is on top of the upper/right arrow
        if (m_VerticalScroll)
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y / 2.f)).contains(x, y))
                m_MouseDownOnTopArrow = true;
            else
                m_MouseDownOnTopArrow = false;
        }
        else
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x / 2.f, m_Size.y)).contains(x, y))
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
                if (((m_VerticalScroll == true)  && (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y / 2.f)).contains(x, y)))
                 || ((m_VerticalScroll == false) && (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x / 2.f, m_Size.y)).contains(x, y) == false)))
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
                if (((m_VerticalScroll == true)  && (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y / 2.f)).contains(x, y) == false))
                 || ((m_VerticalScroll == false) && (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x / 2.f, m_Size.y)).contains(x, y))))
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

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ValueChanged].empty() == false)
            {
                m_Callback.trigger = ValueChanged;
                m_Callback.value   = static_cast<int>(m_Value);
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(float x, float y)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (m_VerticalScroll)
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y / 2.f)).contains(x, y))
                m_MouseHoverOnTopArrow = true;
            else
                m_MouseHoverOnTopArrow = false;
        }
        else
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x / 2.f, m_Size.y)).contains(x, y))
                m_MouseHoverOnTopArrow = false;
            else
                m_MouseHoverOnTopArrow = true;
        }

        if (m_MouseHover == false)
            mouseEnteredObject();

        m_MouseHover = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::objectFocused()
    {
        // A spin button can't be focused
        m_Parent->unfocusObject(this);
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
        if (m_VerticalScroll)
        {
            states.transform.scale(m_Size.x / m_TextureArrowUpNormal.getSize().x, m_Size.y / (m_TextureArrowUpNormal.getSize().y * 2.f));

            // Draw the normal image
            target.draw(m_TextureArrowUpNormal, states);

            // When the mouse is on top of the upper arrow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow)
                    target.draw(m_TextureArrowUpHover, states);
            }

            // Set the second arrow on the correct position
            states.transform.translate(0, 2.f * m_TextureArrowUpNormal.getSize().y);

            // Flip the image
            states.transform.scale(1, -1);

            // Draw the normal image
            target.draw(m_TextureArrowUpNormal, states);

            // When the mouse is on top of the bottom arow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow == false)
                    target.draw(m_TextureArrowUpHover, states);
            }
        }
        else // The image is not drawn in the same direction than the loaded image
        {
            states.transform.scale(m_Size.x / (m_TextureArrowUpNormal.getSize().y * 2.f), m_Size.y / m_TextureArrowUpNormal.getSize().x);

            // Rotate the arrow
            states.transform.rotate(-90, static_cast<float>(m_TextureArrowUpNormal.getSize().x), static_cast<float>(m_TextureArrowUpNormal.getSize().y));

            // Set the left arrow on the correct position
            states.transform.translate(static_cast<float>(m_TextureArrowUpNormal.getSize().y), 0);

            // Draw the normal image
            target.draw(m_TextureArrowUpNormal, states);

            // When the mouse is on top of the left arrow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow == false)
                    target.draw(m_TextureArrowUpHover, states);
            }

            // Set the right arrow on the correct position
            states.transform.translate(0, 2.f * m_TextureArrowUpNormal.getSize().y);

            // Flip the image
            states.transform.scale(1, -1);

            // Draw the normal image
            target.draw(m_TextureArrowUpNormal, states);

            // When the mouse is on top of the right arow then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            {
                if (m_MouseHoverOnTopArrow)
                    target.draw(m_TextureArrowUpHover, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
