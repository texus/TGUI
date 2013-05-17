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

    Slider::Slider() :
    m_MouseDownOnThumb    (false),
    m_Minimum             (  0),
    m_Maximum             (100),
    m_Value               (  0),
    m_VerticalScroll      (true),
    m_VerticalImage       (true),
    m_SplitImage          (false),
    m_SeparateHoverImage  (false),
    m_TextureTrackNormal_L(NULL),
    m_TextureTrackHover_L (NULL),
    m_TextureTrackNormal_M(NULL),
    m_TextureTrackHover_M (NULL),
    m_TextureTrackNormal_R(NULL),
    m_TextureTrackHover_R (NULL),
    m_TextureThumbNormal  (NULL),
    m_TextureThumbHover   (NULL),
    m_LoadedPathname      ("")
    {
        m_ObjectType = slider;
        m_DraggableObject = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider(const Slider& copy) :
    OBJECT               (copy),
    m_MouseDownOnThumb   (copy.m_MouseDownOnThumb),
    m_MouseDownOnThumbPos(copy.m_MouseDownOnThumbPos),
    m_Minimum            (copy.m_Minimum),
    m_Maximum            (copy.m_Maximum),
    m_Value              (copy.m_Value),
    m_VerticalScroll     (copy.m_VerticalScroll),
    m_VerticalImage      (copy.m_VerticalImage),
    m_SplitImage         (copy.m_SplitImage),
    m_SeparateHoverImage (copy.m_SeparateHoverImage),
    m_Size               (copy.m_Size),
    m_ThumbSize          (copy.m_ThumbSize),
    m_LoadedPathname     (copy.m_LoadedPathname)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal_L, m_TextureTrackNormal_L)) m_SpriteTrackNormal_L.setTexture(*m_TextureTrackNormal_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover_L, m_TextureTrackHover_L))   m_SpriteTrackHover_L.setTexture(*m_TextureTrackHover_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal_M, m_TextureTrackNormal_M)) m_SpriteTrackNormal_M.setTexture(*m_TextureTrackNormal_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover_M, m_TextureTrackHover_M))   m_SpriteTrackHover_M.setTexture(*m_TextureTrackHover_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal_R, m_TextureTrackNormal_R)) m_SpriteTrackNormal_R.setTexture(*m_TextureTrackNormal_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover_R, m_TextureTrackHover_R))   m_SpriteTrackHover_R.setTexture(*m_TextureTrackHover_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal))     m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover))       m_SpriteThumbHover.setTexture(*m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::~Slider()
    {
        // Remove all the textures
        if (m_TextureTrackNormal_L != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_L);
        if (m_TextureTrackHover_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_L);
        if (m_TextureTrackNormal_M != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_M);
        if (m_TextureTrackHover_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_M);
        if (m_TextureTrackNormal_R != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_R);
        if (m_TextureTrackHover_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_R);
        if (m_TextureThumbNormal != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)    TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider& Slider::operator= (const Slider& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Slider temp(right);
            this->OBJECT::operator=(right);

            std::swap(m_MouseDownOnThumb,     temp.m_MouseDownOnThumb);
            std::swap(m_MouseDownOnThumbPos,  temp.m_MouseDownOnThumbPos);
            std::swap(m_Minimum,              temp.m_Minimum);
            std::swap(m_Maximum,              temp.m_Maximum);
            std::swap(m_Value,                temp.m_Value);
            std::swap(m_VerticalScroll,       temp.m_VerticalScroll);
            std::swap(m_VerticalImage,        temp.m_VerticalImage);
            std::swap(m_SplitImage,           temp.m_SplitImage);
            std::swap(m_SeparateHoverImage,   temp.m_SeparateHoverImage);
            std::swap(m_Size,                 temp.m_Size);
            std::swap(m_ThumbSize,            temp.m_ThumbSize);
            std::swap(m_TextureTrackNormal_L, temp.m_TextureTrackNormal_L);
            std::swap(m_TextureTrackHover_L,  temp.m_TextureTrackHover_L);
            std::swap(m_TextureTrackNormal_M, temp.m_TextureTrackNormal_M);
            std::swap(m_TextureTrackHover_M,  temp.m_TextureTrackHover_M);
            std::swap(m_TextureTrackNormal_R, temp.m_TextureTrackNormal_R);
            std::swap(m_TextureTrackHover_R,  temp.m_TextureTrackHover_R);
            std::swap(m_TextureThumbNormal,   temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,    temp.m_TextureThumbHover);
            std::swap(m_SpriteTrackNormal_L,  temp.m_SpriteTrackNormal_L);
            std::swap(m_SpriteTrackHover_L,   temp.m_SpriteTrackHover_L);
            std::swap(m_SpriteTrackNormal_M,  temp.m_SpriteTrackNormal_M);
            std::swap(m_SpriteTrackHover_M,   temp.m_SpriteTrackHover_M);
            std::swap(m_SpriteTrackNormal_R,  temp.m_SpriteTrackNormal_R);
            std::swap(m_SpriteTrackHover_R,   temp.m_SpriteTrackHover_R);
            std::swap(m_SpriteThumbNormal,    temp.m_SpriteThumbNormal);
            std::swap(m_SpriteThumbHover,     temp.m_SpriteThumbHover);
            std::swap(m_LoadedPathname,       temp.m_LoadedPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider* Slider::clone()
    {
        return new Slider(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::load(const std::string& pathname)
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

        // m_VerticalScroll will be true unless said otherwise
        m_VerticalImage = true;
        m_VerticalScroll = true;

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
                if ((value.compare("false") == 0) || (value.compare("0") == 0))
                {
                    m_VerticalImage = false;
                    m_VerticalScroll = false;
                }
                else
                {
                    if ((value.compare("true") != 0) && (value.compare("1") != 0))
                        TGUI_OUTPUT("TGUI warning: Wrong value passed to VerticalScroll: \"" + value + "\".");
                }
            }
            else if (property.compare("splitimage") == 0)
            {
                if (value.compare("true") == 0)
                    m_SplitImage = true;
                else if (value.compare("false") == 0)
                    m_SplitImage = false;
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
        if (m_TextureTrackNormal_L != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_L);
        if (m_TextureTrackHover_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_L);
        if (m_TextureTrackNormal_M != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_M);
        if (m_TextureTrackHover_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_M);
        if (m_TextureTrackNormal_R != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_R);
        if (m_TextureTrackHover_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_R);
        if (m_TextureThumbNormal != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)    TGUI_TextureManager.removeTexture(m_TextureThumbHover);

        // Check if the image is split
        if (m_SplitImage)
        {
            // load the required textures
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Track_Normal." + imageExtension, m_TextureTrackNormal_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Track_Normal." + imageExtension, m_TextureTrackNormal_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Track_Normal." + imageExtension, m_TextureTrackNormal_R))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Normal." + imageExtension, m_TextureThumbNormal)))
            {
                m_SpriteTrackNormal_L.setTexture(*m_TextureTrackNormal_L, true);
                m_SpriteTrackNormal_M.setTexture(*m_TextureTrackNormal_M, true);
                m_SpriteTrackNormal_R.setTexture(*m_TextureTrackNormal_R, true);
                m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal, true);

                // Set the size of the slider
                if (m_VerticalImage)
                    m_Size = Vector2f(static_cast<float>(m_TextureTrackNormal_M->getSize().x), static_cast<float>(m_TextureTrackNormal_L->getSize().y + m_TextureTrackNormal_M->getSize().y + m_TextureTrackNormal_R->getSize().y));
                else
                    m_Size = Vector2f(static_cast<float>(m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_M->getSize().x + m_TextureTrackNormal_R->getSize().x), static_cast<float>(m_TextureTrackNormal_M->getSize().y));

                // Set the thumb size
                m_ThumbSize = Vector2f(m_TextureThumbNormal->getSize());
            }
            else
                return false;

            // load the optional textures
            if (m_ObjectPhase & ObjectPhase_Hover)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Track_Hover." + imageExtension, m_TextureTrackHover_L))
                 && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Track_Hover." + imageExtension, m_TextureTrackHover_M))
                 && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Track_Hover." + imageExtension, m_TextureTrackHover_R))
                 && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Hover." + imageExtension, m_TextureThumbHover)))
                 {
                     m_SpriteTrackHover_L.setTexture(*m_TextureTrackHover_L, true);
                     m_SpriteTrackHover_M.setTexture(*m_TextureTrackHover_M, true);
                     m_SpriteTrackHover_R.setTexture(*m_TextureTrackHover_R, true);
                     m_SpriteThumbHover.setTexture(*m_TextureThumbHover, true);
                 }
                else
                    return false;
            }
        }
        else // The image isn't split
        {
            // load the required textures
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Normal." + imageExtension, m_TextureTrackNormal_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Normal." + imageExtension, m_TextureThumbNormal)))
            {
                m_SpriteTrackNormal_M.setTexture(*m_TextureTrackNormal_M, true);
                m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal, true);

                // Set the size of the slider
                m_Size = Vector2f(m_TextureTrackNormal_M->getSize());

                // Set the thumb size
                m_ThumbSize = Vector2f(m_TextureThumbNormal->getSize());
            }
            else
                return false;

            // load the optional textures
            if (m_ObjectPhase & ObjectPhase_Hover)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Hover." + imageExtension, m_TextureTrackHover_M))
                 && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Hover." + imageExtension, m_TextureThumbHover)))
                 {
                     m_SpriteTrackHover_M.setTexture(*m_TextureTrackHover_M, true);
                     m_SpriteThumbHover.setTexture(*m_TextureThumbHover, true);
                 }
                else
                    return false;
            }
        }

        // When there is no error we will return true
        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the size of the slider
        m_Size.x = width;
        m_Size.y = height;

        // A negative size is not allowed for this object
        if (m_Size.x < 0) m_Size.x = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;

        // Set the thumb size
        if (m_VerticalImage == m_VerticalScroll)
        {
            if (m_VerticalScroll)
            {
                m_ThumbSize.x = (m_Size.x / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().x;
                m_ThumbSize.y = (m_Size.x / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().y;
            }
            else
            {
                m_ThumbSize.x = (m_Size.y / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().x;
                m_ThumbSize.y = (m_Size.y / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().y;
            }
        }
        else
        {
            if (m_VerticalScroll)
            {
                m_ThumbSize.x = (m_Size.x / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().y;
                m_ThumbSize.y = (m_Size.x / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().x;
            }
            else
            {
                m_ThumbSize.x = (m_Size.y / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().y;
                m_ThumbSize.y = (m_Size.y / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().x;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Slider::getSize() const
    {
        if (m_Loaded)
            return Vector2u(m_Size);
        else
            return Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getScaledSize() const
    {
        if (m_Loaded)
            return Vector2f(m_Size.x * getScale().x, m_Size.y * getScale().y);
        else
            return Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Slider::getLoadedPathname() const
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_Minimum = minimum;

        // When the value is below the minimum then adjust it
        if (m_Value < m_Minimum)
            m_Value = m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(unsigned int maximum)
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

    void Slider::setValue(unsigned int value)
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

    void Slider::setVerticalScroll(bool verticalScroll)
    {
        // Only continue when the value changed
        if (m_VerticalScroll != verticalScroll)
        {
            // Change the internal value
            m_VerticalScroll = verticalScroll;

            // Swap the width and height if needed
            if (m_VerticalScroll)
            {
                if (m_Size.x > m_Size.y)
                    m_Size = Vector2f(m_Size.y, m_Size.x);
            }
            else // The slider lies horizontal
            {
                if (m_Size.y > m_Size.x)
                    m_Size = Vector2f(m_Size.y, m_Size.x);
            }

            // Set the thumb size
            if (m_VerticalImage == m_VerticalScroll)
            {
                if (m_VerticalScroll)
                {
                    m_ThumbSize.x = (m_Size.x / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().x;
                    m_ThumbSize.y = (m_Size.x / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().y;
                }
                else
                {
                    m_ThumbSize.x = (m_Size.y / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().x;
                    m_ThumbSize.y = (m_Size.y / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().y;
                }
            }
            else
            {
                if (m_VerticalScroll)
                {
                    m_ThumbSize.x = (m_Size.x / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().y;
                    m_ThumbSize.y = (m_Size.x / m_TextureTrackNormal_M->getSize().y) * m_TextureThumbNormal->getSize().x;
                }
                else
                {
                    m_ThumbSize.x = (m_Size.y / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().y;
                    m_ThumbSize.y = (m_Size.y / m_TextureTrackNormal_M->getSize().x) * m_TextureThumbNormal->getSize().x;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll()
    {
        return m_VerticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMinimum() const
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnObject(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Calculate the thumb size and position
        float thumbWidth, thumbHeight;
        float thumbLeft,  thumbTop;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // The size is different when the image is rotated
        if (m_VerticalImage == m_VerticalScroll)
        {
            thumbWidth = m_ThumbSize.x * curScale.x;
            thumbHeight = m_ThumbSize.y * curScale.y;
        }
        else
        {
            thumbWidth = m_ThumbSize.y * curScale.x;
            thumbHeight = m_ThumbSize.x * curScale.y;
        }

        // Calculate the thumb position
        if (m_VerticalScroll)
        {
            thumbLeft = ((m_Size.x * curScale.x) - thumbWidth) * 0.5f;
            thumbTop = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * (m_Size.y * curScale.y)) - (thumbHeight * 0.5f);
        }
        else // The slider lies horizontal
        {
            thumbLeft = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * (m_Size.x * curScale.x)) - (thumbWidth * 0.5f);
            thumbTop = ((m_Size.y * curScale.y) - thumbHeight) * 0.5f;
        }

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(position.x + thumbLeft, position.y + thumbTop, thumbWidth, thumbHeight).contains(x, y))
        {
            m_MouseDownOnThumb = true;
            m_MouseDownOnThumbPos.x = x - position.x - thumbLeft;
            m_MouseDownOnThumbPos.y = y - position.y - thumbTop;
            return true;
        }
        else // The mouse is not on top of the thumb
            m_MouseDownOnThumb = false;

        // Check if the mouse is on top of the track
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
            return true;

        // The mouse is not on top of the slider
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        m_MouseHover = true;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Remember the old value
        unsigned int oldValue = m_Value;

        // Check if the mouse button is down
        if (m_MouseDown)
        {
            // Check in which direction the slider goes
            if (m_VerticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Calculate the thumb height
                    float thumbHeight = m_ThumbSize.y * curScale.y;

                    // Set the new value
                    if ((y - m_MouseDownOnThumbPos.y + (thumbHeight / 2.0f) - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - m_MouseDownOnThumbPos.y + (thumbHeight / 2.0f) - position.y) / (m_Size.y * curScale.x)) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else
                        setValue(m_Minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if ((y - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - position.y) / (m_Size.y * curScale.x)) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        m_Value = m_Minimum;
                }
            }
            else // the slider lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Calculate the thumb width
                    float thumbWidth = m_ThumbSize.x * curScale.x;

                    // Set the new value
                    if ((x - m_MouseDownOnThumbPos.x + (thumbWidth / 2.0f) - position.x) > 0)
                        setValue(static_cast <unsigned int> ((((x - m_MouseDownOnThumbPos.x + (thumbWidth / 2.0f) - position.x) / (m_Size.x * curScale.x)) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else
                        setValue(m_Minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if (x - position.x > 0)
                        setValue(static_cast<unsigned int>((((x - position.x) / (m_Size.x * curScale.x)) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        m_Value = m_Minimum;
                }
            }
        }

        // Add the callback (if the user requested it and the value has changed)
        if ((callbackID > 0) && (oldValue != m_Value))
        {
            Callback callback;
            callback.object     = this;
            callback.callbackID = callbackID;
            callback.trigger    = Callback::valueChanged;
            callback.value      = m_Value;
            m_Parent->addCallback(callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::keyPressed(sf::Keyboard::Key key)
    {
        // TODO: Respond on arrow presses

        TGUI_UNUSED_PARAM(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseWheelMoved(int delta)
    {
        if (static_cast<int>(m_Value) - delta < static_cast<int>(m_Minimum))
            m_Value = m_Minimum;
        else
            setValue(static_cast<unsigned int>(m_Value - delta));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::objectFocused()
    {
        // A slider can't be focused (yet)
        m_Parent->unfocusObject(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        Vector2f scaling;

        // Apply the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Check if the image is split
        if (m_SplitImage)
        {
            // Get the scale factors
            if (m_VerticalScroll == m_VerticalImage)
            {
                scaling.x = m_Size.x / (m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_M->getSize().x + m_TextureTrackNormal_R->getSize().x);
                scaling.y = m_Size.y / m_TextureTrackNormal_M->getSize().y;
            }
            else
            {
                // Check in what direction the slider should rotate
                if ((m_VerticalImage == true) && (m_VerticalScroll == false))
                {
                    // Set the rotation
                    states.transform.rotate(-90,
                                            (m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_M->getSize().x + m_TextureTrackNormal_R->getSize().x) * 0.5f,
                                            m_TextureTrackNormal_M->getSize().x * 0.5f);
                }
                else // if ((m_VerticalImage == false) && (m_VerticalScroll == true))
                {
                    // Set the rotation
                    states.transform.rotate(90,
                                            (m_TextureTrackNormal_L->getSize().y + m_TextureTrackNormal_M->getSize().y + m_TextureTrackNormal_R->getSize().y) * 0.5f,
                                            m_TextureTrackNormal_M->getSize().y * 0.5f);
                }

                scaling.x = m_Size.x / (m_TextureTrackNormal_L->getSize().y + m_TextureTrackNormal_M->getSize().y + m_TextureTrackNormal_R->getSize().y);
                scaling.y = m_Size.y / m_TextureTrackNormal_M->getSize().x;
            }

            // Set the scale for the left image
            states.transform.scale(scaling.y, scaling.y);

            // Draw the left image
            {
                // Check if there is a separate hover image
                if (m_SeparateHoverImage)
                {
                    // Draw the correct image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_SpriteTrackHover_L, states);
                    else
                        target.draw(m_SpriteTrackNormal_L, states);
                }
                else
                {
                    // Draw the normal track image
                    target.draw(m_SpriteTrackNormal_L, states);

                    // When the mouse is on top of the slider then draw the hover image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_SpriteTrackHover_L, states);
                }

                // When the slider is focused then draw an extra image
//                if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
//                    target.draw(m_SpriteFocused_L, states);
            }

            // Check if the middle image may be drawn
            if ((scaling.y * (m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_R->getSize().x))
                < scaling.x * (m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_M->getSize().x + m_TextureTrackNormal_R->getSize().x))
            {
                // Calculate the scale for our middle image
                float scaleX = (((m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_M->getSize().x + m_TextureTrackNormal_R->getSize().x)  * scaling.x)
                                 - ((m_TextureTrackNormal_L->getSize().x + m_TextureTrackNormal_R->getSize().x) * scaling.y))
                               / m_TextureTrackNormal_M->getSize().x;

                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_TextureTrackNormal_L->getSize().x), 0);

                // Set the scale for the middle image
                states.transform.scale(scaleX / scaling.y, 1);

                // Draw the middle image
                {
                    // Check if there is a separate hover image
                    if (m_SeparateHoverImage)
                    {
                        // Draw the correct image
                        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                            target.draw(m_SpriteTrackHover_M, states);
                        else
                            target.draw(m_SpriteTrackNormal_M, states);
                    }
                    else
                    {
                        // Draw the normal track image
                        target.draw(m_SpriteTrackNormal_M, states);

                        // When the mouse is on top of the slider then draw the hover image
                        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                            target.draw(m_SpriteTrackHover_M, states);
                    }

                    // When the slider is focused then draw an extra image
//                    if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
//                        target.draw(m_SpriteFocused_M, states);
                }

                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureTrackNormal_M->getSize().x), 0);

                // Set the scale for the right image
                states.transform.scale(scaling.y / scaleX, 1);
            }
            else // The middle image is not drawn
                states.transform.translate(static_cast<float>(m_TextureTrackNormal_L->getSize().x), 0);

            // Draw the right image
            {
                // Check if there is a separate hover image
                if (m_SeparateHoverImage)
                {
                    // Draw the correct image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_SpriteTrackHover_R, states);
                    else
                        target.draw(m_SpriteTrackNormal_R, states);
                }
                else
                {
                    // Draw the normal track image
                    target.draw(m_SpriteTrackNormal_R, states);

                    // When the mouse is on top of the slider then draw the hover image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_SpriteTrackHover_R, states);
                }

                // When the slider is focused then draw an extra image
//                if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
//                    target.draw(m_SpriteFocused_R, states);
            }
        }
        else // The image is not split
        {
            if (m_VerticalScroll == m_VerticalImage)
            {
                // Set the scaling
                scaling.x = m_Size.x / m_TextureTrackNormal_M->getSize().x;
                scaling.y = m_Size.y / m_TextureTrackNormal_M->getSize().y;
                states.transform.scale(scaling);
            }
            else
            {
                // Set the scaling
                scaling.x = m_Size.x / m_TextureTrackNormal_M->getSize().y;
                scaling.y = m_Size.y / m_TextureTrackNormal_M->getSize().x;
                states.transform.scale(scaling);

                // Set the rotation
                if ((m_VerticalImage == true) && (m_VerticalScroll == false))
                    states.transform.rotate(-90, m_TextureTrackNormal_M->getSize().x * 0.5f, m_TextureTrackNormal_M->getSize().x * 0.5f);
                else // if ((m_VerticalImage == false) && (m_VerticalScroll == true))
                    states.transform.rotate(90, m_TextureTrackNormal_M->getSize().y * 0.5f, m_TextureTrackNormal_M->getSize().y * 0.5f);
            }

            // Draw the normal track image
            target.draw(m_SpriteTrackNormal_M, states);

            // When the mouse is on top of the slider then draw the hover image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                target.draw(m_SpriteTrackHover_M, states);
        }

        // Reset the transform
        states.transform = oldTransform;

        // The thumb will be on a different position when we are scrolling vertically or not
        if (m_VerticalScroll)
        {
            // Set the translation and scale for the thumb
            states.transform.translate(static_cast<int>(m_Size.x - m_ThumbSize.x) * 0.5f,
                                       ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.y) - (m_ThumbSize.y * 0.5f));

            // Set the scale for the thumb
            states.transform.scale(scaling.x, scaling.x);
        }
        else // the slider lies horizontal
        {
            // Set the translation and scale for the thumb
            states.transform.translate(((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.x) - (m_ThumbSize.x * 0.5f),
                                        (m_Size.y - m_ThumbSize.y) * 0.5f);

            // Set the scale for the thumb
            states.transform.scale(scaling.y, scaling.y);
        }

        // It is possible that the image is not drawn in the same direction than the loaded image
        if ((m_VerticalImage == true) && (m_VerticalScroll == false))
        {
            // Set the rotation
            states.transform.rotate(-90, m_TextureThumbNormal->getSize().x * 0.5f, m_TextureThumbNormal->getSize().x * 0.5f);
        }
        else if ((m_VerticalImage == false) && (m_VerticalScroll == true))
        {
            // Set the rotation
            states.transform.rotate(90, m_TextureThumbNormal->getSize().y * 0.5f, m_TextureThumbNormal->getSize().y * 0.5f);
        }

        // Draw the normal thumb image
        target.draw(m_SpriteThumbNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_SpriteThumbHover, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
