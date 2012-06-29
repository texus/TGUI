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
    verticalScroll    (true),
    m_MouseDownOnThumb  (false),
    m_Minimum           (  0),
    m_Maximum           (100),
    m_Value             (  0),
    m_VerticalImage     (true),
    m_TextureTrackNormal(NULL),
    m_TextureTrackHover (NULL),
    m_TextureThumbNormal(NULL),
    m_TextureThumbHover (NULL)
    {
        m_ObjectType = slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider(const Slider& copy) :
    OBJECT               (copy),
    verticalScroll       (copy.verticalScroll),
    m_MouseDownOnThumb   (copy.m_MouseDownOnThumb),
    m_MouseDownOnThumbPos(copy.m_MouseDownOnThumbPos),
    m_Minimum            (copy.m_Minimum),
    m_Maximum            (copy.m_Maximum),
    m_Value              (copy.m_Value),
    m_VerticalImage      (copy.m_VerticalImage)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal, m_TextureTrackNormal)) m_SpriteTrackNormal.setTexture(*m_TextureTrackNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover, m_TextureTrackHover))   m_SpriteTrackHover.setTexture(*m_TextureTrackHover);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal)) m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover))   m_SpriteThumbHover.setTexture(*m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::~Slider()
    {
        // Remove all the textures
        if (m_TextureTrackNormal != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal != NULL) TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider& Slider::operator= (const Slider& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Slider temp(right);
            this->OBJECT::operator=(right);

            std::swap(verticalScroll,        temp.verticalScroll);
            std::swap(m_MouseDownOnThumb,    temp.m_MouseDownOnThumb);
            std::swap(m_MouseDownOnThumbPos, temp.m_MouseDownOnThumbPos);
            std::swap(m_Minimum,             temp.m_Minimum);
            std::swap(m_Maximum,             temp.m_Maximum);
            std::swap(m_Value,               temp.m_Value);
            std::swap(m_VerticalImage,       temp.m_VerticalImage);
            std::swap(m_TextureTrackNormal,  temp.m_TextureTrackNormal);
            std::swap(m_TextureTrackHover,   temp.m_TextureTrackHover);
            std::swap(m_TextureThumbNormal,  temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,   temp.m_TextureThumbHover);
            std::swap(m_SpriteTrackNormal,   temp.m_SpriteTrackNormal);
            std::swap(m_SpriteTrackHover,    temp.m_SpriteTrackHover);
            std::swap(m_SpriteThumbNormal,   temp.m_SpriteThumbNormal);
            std::swap(m_SpriteThumbHover,    temp.m_SpriteThumbHover);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::load(const std::string pathname)
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
                m_VerticalImage = !!atoi(value.c_str());
                verticalScroll = m_VerticalImage;
            }
        }

        // Close the info file
        infoFile.closeFile();

        // Remove all textures if they were loaded before
        if (m_TextureTrackNormal != NULL) TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal != NULL) TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbHover);

        // load the required textures
        if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Normal." + imageExtension, m_TextureTrackNormal))
        && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Normal." + imageExtension, m_TextureThumbNormal)))
        {
            m_SpriteTrackNormal.setTexture(*m_TextureTrackNormal, true);
            m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal, true);
        }
        else
            return false;

        bool error = false;

        // load the optional textures

        if (m_ObjectPhase & objectPhase::hover)
        {
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Hover." + imageExtension, m_TextureTrackHover))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Hover." + imageExtension, m_TextureThumbHover)))
             {
                 m_SpriteTrackHover.setTexture(*m_TextureTrackHover, true);
                 m_SpriteThumbHover.setTexture(*m_TextureThumbHover, true);
             }
            else
                error = true;
        }

        // When there is no error we will return true
        m_Loaded = !error;
        return !error;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new scale factors
        if (verticalScroll == m_VerticalImage)
            setScale(width / m_TextureTrackNormal->getSize().x, height / m_TextureTrackNormal->getSize().y);
        else
            setScale(width / m_TextureTrackNormal->getSize().y, height / m_TextureTrackNormal->getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Slider::getSize() const
    {
        // Don't continue when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2u(0, 0);

        // Return the size of the track
        if (verticalScroll == m_VerticalImage)
            return Vector2u(m_TextureTrackNormal->getSize().x, m_TextureTrackNormal->getSize().y);
        else
            return Vector2u(m_TextureTrackNormal->getSize().y, m_TextureTrackNormal->getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getScaledSize() const
    {
        // Don't continue when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2f(0, 0);

        // Return the size of the track
        if (verticalScroll == m_VerticalImage)
            return Vector2f(m_TextureTrackNormal->getSize().x * getScale().x, m_TextureTrackNormal->getSize().y * getScale().y);
        else
            return Vector2f(m_TextureTrackNormal->getSize().y * getScale().x, m_TextureTrackNormal->getSize().x * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Slider::getLoadedPathname()
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

    void Slider::setVerticalScroll( bool vertical )
    {
        verticalScroll = vertical;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMinimum()
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMaximum()
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getValue()
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll()
    {
        return verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnObject(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();


        // Calculate the track size, thumb size and thumb position
        float trackWidth, trackHeight;
        float thumbWidth, thumbHeight;
        float thumbLeft,  thumbTop;

        // The size is different when the image is rotated
        if (m_VerticalImage == verticalScroll)
        {
            trackWidth = m_TextureTrackNormal->getSize().x * curScale.x;
            trackHeight = m_TextureTrackNormal->getSize().y * curScale.y;
            thumbWidth = static_cast<float>(m_TextureThumbNormal->getSize().x);
            thumbHeight = static_cast<float>(m_TextureThumbNormal->getSize().y);
        }
        else
        {
            trackWidth = m_TextureTrackNormal->getSize().y * curScale.x;
            trackHeight = m_TextureTrackNormal->getSize().x * curScale.y;
            thumbWidth = static_cast<float>(m_TextureThumbNormal->getSize().y);
            thumbHeight = static_cast<float>(m_TextureThumbNormal->getSize().x);
        }

        // The scaling depends on how the slider lies
        if (verticalScroll)
        {
            // Calculate the thumb position
            thumbTop = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * trackHeight) - (thumbHeight * curScale.x * 0.5f);
            thumbLeft = (trackWidth - (thumbWidth * curScale.x)) * 0.5f;
        }
        else // The slider lies horizontal
        {
            // Calculate the thumb position
            thumbLeft = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * trackWidth) - (thumbWidth * curScale.y * 0.5f);
            thumbTop = (trackHeight - (thumbHeight * curScale.y)) * 0.5f;
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
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
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
            if (verticalScroll)
            {
                // Calculate the track height
                float trackHeight;

                if (m_VerticalImage == verticalScroll)
                    trackHeight = m_TextureTrackNormal->getSize().y * curScale.y;
                else
                    trackHeight = m_TextureTrackNormal->getSize().x * curScale.y;

                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Calculate the thumb height
                    float  thumbHeight;

                    if (m_VerticalImage == verticalScroll)
                        thumbHeight = m_TextureThumbNormal->getSize().y * curScale.x;
                    else
                        thumbHeight = m_TextureThumbNormal->getSize().x * curScale.x;

                    // Set the new value
                    if ((y - m_MouseDownOnThumbPos.y + (thumbHeight / 2.0f) - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - m_MouseDownOnThumbPos.y + (thumbHeight / 2.0f) - position.y) / trackHeight) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else
                        setValue(m_Minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if ((y - position.y) > 0)
                            setValue(static_cast <unsigned int> ((((y - position.y) / trackHeight) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        m_Value = m_Minimum;
                }
            }
            else // the slider lies horizontal
            {
                // Calculate the track width
                float trackWidth;

                if (m_VerticalImage == verticalScroll)
                    trackWidth = m_TextureTrackNormal->getSize().x * curScale.x;
                else
                    trackWidth = m_TextureTrackNormal->getSize().y * curScale.x;

                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Calculate the thumb width
                    float thumbWidth;

                    if (m_VerticalImage == verticalScroll)
                        thumbWidth = m_TextureThumbNormal->getSize().x * curScale.y;
                    else
                        thumbWidth = m_TextureThumbNormal->getSize().y * curScale.y;

                    // Set the new value
                    if ((x - m_MouseDownOnThumbPos.x + (thumbWidth / 2.0f) - position.x) > 0)
                        setValue(static_cast <unsigned int> ((((x - m_MouseDownOnThumbPos.x + (thumbWidth / 2.0f) - position.x) / trackWidth) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else
                        setValue(m_Minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if ((x - position.x) > 0)
                            setValue(static_cast <unsigned int> ((((x - position.x) / trackWidth) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        m_Value = m_Minimum;
                }
            }
        }

        // Add the callback (if the user requested it and the value has changed)
        if ((callbackID > 0) && (oldValue != m_Value))
        {
            Callback callback;
            callback.callbackID = callbackID;
            callback.trigger    = Callback::valueChanged;
            callback.value      = m_Value;
            m_Parent->addCallback(callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::keyPressed(sf::Keyboard::Key key)
    {
        /// TODO: Respond on arrow presses

        TGUI_UNUSED_PARAM(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::objectFocused()
    {
        // A slider can't be focused (yet)
        m_Parent->unfocus(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        float trackWidth, trackHeight;
        float thumbWidth, thumbHeight;

        // Get the current scale
        Vector2f curScale = getScale();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Adjust the transformation
        states.transform *= getTransform();


        // It is possible that the image is not drawn in the same direction than the loaded image
        if ((m_VerticalImage == true) && (verticalScroll == false))
        {
            // Set the rotation
            states.transform.rotate(-90, m_TextureTrackNormal->getSize().x * 0.5f, m_TextureTrackNormal->getSize().x * 0.5f);

            // Calculate the track size
            trackWidth = static_cast<float>(m_TextureTrackNormal->getSize().y);
            trackHeight = static_cast<float>(m_TextureTrackNormal->getSize().x);

            // Calculate the thumb size
            thumbWidth = static_cast<float>(m_TextureThumbNormal->getSize().y);
            thumbHeight = static_cast<float>(m_TextureThumbNormal->getSize().x);
        }
        else if ((m_VerticalImage == false) && (verticalScroll == true))
        {
            // Set the rotation
            states.transform.rotate(90, m_TextureTrackNormal->getSize().y * 0.5f, m_TextureTrackNormal->getSize().y * 0.5f);

            // Calculate the track size
            trackWidth = static_cast<float>(m_TextureTrackNormal->getSize().y);
            trackHeight = static_cast<float>(m_TextureTrackNormal->getSize().x);

            // Calculate the thumb size
            thumbWidth = static_cast<float>(m_TextureThumbNormal->getSize().y);
            thumbHeight = static_cast<float>(m_TextureThumbNormal->getSize().x);
        }
        else
        {
            // Calculate the track size
            trackWidth = static_cast<float>(m_TextureTrackNormal->getSize().x);
            trackHeight = static_cast<float>(m_TextureTrackNormal->getSize().y);

            // Calculate the thumb size
            thumbWidth = static_cast<float>(m_TextureThumbNormal->getSize().x);
            thumbHeight = static_cast<float>(m_TextureThumbNormal->getSize().y);
        }

        // Draw the normal track image
        target.draw(m_SpriteTrackNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if (m_MouseHover)
            target.draw(m_SpriteTrackHover, states);


        // Reset the transform
        states.transform = oldTransform;
        states.transform.translate(getPosition());

        // The thumb will be on a different position when we are scrolling vertically or not
        if (verticalScroll)
        {
            // Set the translation and scale for the thumb
            states.transform.translate(static_cast<int>(trackWidth - thumbWidth) * 0.5f * curScale.x,
                                       ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * trackHeight * curScale.y)
                                        - (thumbHeight * 0.5f * curScale.x));

            // Set the scale for the thumb
            states.transform.scale(curScale.x, curScale.x);
        }
        else // the slider lies horizontal
        {
            // Set the translation and scale for the thumb
            states.transform.translate(((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * trackWidth * curScale.x)
                                        - (thumbWidth * 0.5f * curScale.y),
                                       static_cast<int>(trackHeight - thumbHeight) * 0.5f * curScale.y);

            // Set the scale for the thumb
            states.transform.scale(curScale.y, curScale.y);
        }

        // It is possible that the image is not drawn in the same direction than the loaded image
        if ((m_VerticalImage == true) && (verticalScroll == false))
        {
            // Set the rotation
            states.transform.rotate(-90, m_TextureThumbNormal->getSize().x * 0.5f, m_TextureThumbNormal->getSize().x * 0.5f);
        }
        else if ((m_VerticalImage == false) && (verticalScroll == true))
        {
            // Set the rotation
            states.transform.rotate(90, m_TextureThumbNormal->getSize().y * 0.5f, m_TextureThumbNormal->getSize().y * 0.5f);
        }


        // Draw the normal thumb image
        target.draw(m_SpriteThumbNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if (m_MouseHover)
            target.draw(m_SpriteThumbHover, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
