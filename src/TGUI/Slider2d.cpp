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

// Special thanks to Dmitry for the Slider2d object !


#include <TGUI/Objects.hpp>
#include <TGUI/ClickableObject.hpp>
#include <TGUI/Slider2d.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::Slider2d() :
    m_Minimum            (-1, -1),
    m_Maximum            (1, 1),
    m_Value              (0, 0),
    m_ReturnThumbToCenter(false),
    m_FixedThumbSize     (true),
    m_TextureThumbNormal (NULL),
    m_TextureThumbHover  (NULL),
    m_TextureTrackNormal (NULL),
    m_TextureTrackHover  (NULL),
    m_LoadedPathname     ("")
    {
        m_Callback.objectType = Type_Slider2d;
        m_DraggableObject = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::Slider2d(const Slider2d& copy) :
    ClickableObject      (copy),
    m_Minimum            (copy.m_Minimum),
    m_Maximum            (copy.m_Maximum),
    m_Value              (copy.m_Value),
    m_ReturnThumbToCenter(copy.m_ReturnThumbToCenter),
    m_FixedThumbSize     (copy.m_FixedThumbSize),
    m_LoadedPathname     (copy.m_LoadedPathname)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal, m_TextureTrackNormal))   m_SpriteTrackNormal.setTexture(*m_TextureTrackNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover, m_TextureTrackHover))     m_SpriteTrackHover.setTexture(*m_TextureTrackHover);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal))   m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover))     m_SpriteThumbHover.setTexture(*m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::~Slider2d()
    {
        // Remove all the textures
        if (m_TextureTrackNormal != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover != NULL)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d& Slider2d::operator= (const Slider2d& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Slider2d temp(right);
            this->ClickableObject::operator=(right);

            std::swap(m_Minimum,             temp.m_Minimum);
            std::swap(m_Maximum,             temp.m_Maximum);
            std::swap(m_Value,               temp.m_Value);
            std::swap(m_ReturnThumbToCenter, temp.m_ReturnThumbToCenter);
            std::swap(m_FixedThumbSize,      temp.m_FixedThumbSize);
            std::swap(m_TextureTrackNormal,  temp.m_TextureTrackNormal);
            std::swap(m_TextureTrackHover,   temp.m_TextureTrackHover);
            std::swap(m_TextureThumbNormal,  temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,   temp.m_TextureThumbHover);
            std::swap(m_SpriteTrackNormal,   temp.m_SpriteTrackNormal);
            std::swap(m_SpriteTrackHover,    temp.m_SpriteTrackHover);
            std::swap(m_SpriteThumbNormal,   temp.m_SpriteThumbNormal);
            std::swap(m_SpriteThumbHover,    temp.m_SpriteThumbHover);
            std::swap(m_LoadedPathname,      temp.m_LoadedPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d* Slider2d::clone()
    {
        return new Slider2d(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::load(const std::string& pathname)
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
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedPathname + "info.txt");
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
        }

        // Close the info file
        infoFile.closeFile();

        // Remove all textures if they were loaded before
        if (m_TextureTrackNormal != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover != NULL)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);

        // load the required textures
        if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Normal." + imageExtension, m_TextureTrackNormal))
            && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Normal." + imageExtension, m_TextureThumbNormal)))
        {
            m_SpriteTrackNormal.setTexture(*m_TextureTrackNormal, true);
            m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal, true);

            // Store the size of the slider
            m_Size = Vector2f(m_TextureTrackNormal->getSize());
        }
        else
            return false;

        // load the optional textures
        if (m_ObjectPhase & ObjectPhase_Hover)
        {
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Hover." + imageExtension, m_TextureTrackHover))
                && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Hover." + imageExtension, m_TextureThumbHover)))
            {
                m_SpriteTrackHover.setTexture(*m_TextureTrackHover, true);
                m_SpriteThumbHover.setTexture(*m_TextureThumbHover, true);
            }
            else
                return false;
        }

        // When there is no error we will return true
        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Store the size of the slider
        m_Size.x = width;
        m_Size.y = height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Slider2d::getLoadedPathname() const
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setMinimum(const Vector2f& minimum)
    {
        // Set the new minimum
        m_Minimum = minimum;

        // When the value is below the minimum then adjust it
        if (m_Value.x < m_Minimum.x)
            m_Value.x = m_Minimum.x;
        if (m_Value.y < m_Minimum.y)
            m_Value.y = m_Minimum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setMaximum(const Vector2f& maximum)
    {
        // Set the new maximum
        m_Maximum = maximum;

        // When the value is above the maximum then adjust it
        if (m_Value.x > m_Maximum.x)
            m_Value.x = m_Maximum.x;
        if (m_Value.y > m_Maximum.y)
            m_Value.y = m_Maximum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setValue(const Vector2f& value)
    {
        // Set the new value
        m_Value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_Value.x < m_Minimum.x)
            m_Value.x = m_Minimum.x;
        else if (m_Value.x > m_Maximum.x)
            m_Value.x = m_Maximum.x;

        if (m_Value.y < m_Minimum.y)
            m_Value.y = m_Minimum.y;
        else if (m_Value.y > m_Maximum.y)
            m_Value.y = m_Maximum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider2d::getMinimum() const
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider2d::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider2d::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setFixedThumbSize(bool fixedSize)
    {
        m_FixedThumbSize = fixedSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::getFixedThumbSize() const
    {
        return m_FixedThumbSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::enableThumbCenter(bool autoCenterThumb)
    {
        m_ReturnThumbToCenter = autoCenterThumb;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::centerThumb()
    {
        setValue(Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::leftMousePressed(float x, float y)
    {
        ClickableObject::leftMousePressed(x, y);

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::leftMouseReleased(float x, float y)
    {
        ClickableObject::leftMouseReleased(x, y);

        if (m_ReturnThumbToCenter)
        {
            setValue(Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));

            if (m_CallbackFunctions[ThumbReturnToCenter].empty() == false)
            {
                m_Callback.trigger = ThumbReturnToCenter;
                m_Callback.value2d = m_Value;
                addCallback();
            }
		}
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Slider2d::mouseNoLongerDown()
	{
		m_MouseDown = false;

		if (m_ReturnThumbToCenter)
		{
		    if (m_Value != Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f))
		    {
		        setValue(Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));

                if (m_CallbackFunctions[ThumbReturnToCenter].empty() == false)
                {
                    m_Callback.trigger = ThumbReturnToCenter;
                    m_Callback.value2d = m_Value;
                    addCallback();
                }
		    }
		}
	}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredObject();

        m_MouseHover = true;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Remember the old value
        Vector2f oldValue = m_Value;

        // Check if the mouse button is down
        if (m_MouseDown)
        {
            // If the position is positive then calculate the correct value
            if ((y - position.y) > 0)
                m_Value.y = ((y - position.y) / (m_Size.y * curScale.y)) * (m_Maximum.y - m_Minimum.y) + m_Minimum.y;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_Value.y = m_Minimum.y;

            // If the position is positive then calculate the correct value
            if ((x - position.x) > 0)
                m_Value.x = ((x - position.x) / (m_Size.x * curScale.x)) * (m_Maximum.x - m_Minimum.x) + m_Minimum.x;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_Value.x = m_Minimum.x;

            // Set the new value, making sure that it lies within the minimum and maximum
            setValue(m_Value);

            // Add the callback (if the user requested it)
            if ((oldValue != m_Value) && (m_CallbackFunctions[ValueChanged].empty() == false))
            {
                m_Callback.trigger = ValueChanged;
                m_Callback.value2d = m_Value;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::objectFocused()
    {
        // A slider can't be focused (yet)
        m_Parent->unfocusObject(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the current position and scale
        Vector2f curScale = getScale();

        // Get the global position
        Vector2f topLeftPosition = states.transform.transformPoint(getPosition() - target.getView().getCenter() + (target.getView().getSize() / 2.f));
        Vector2f bottomRightPosition = states.transform.transformPoint(getPosition() + Vector2f(m_Size.x * curScale.x, m_Size.y * curScale.y) - target.getView().getCenter() + (target.getView().getSize() / 2.f));

        // Adjust the transformation
        states.transform *= getTransform();

        // Calculate the scale of the slider
        Vector2f scaling;
        scaling.x = m_Size.x / m_TextureTrackNormal->getSize().x;
        scaling.y = m_Size.y / m_TextureTrackNormal->getSize().y;

        // Set the scale of the slider
        states.transform.scale(scaling);

        // Draw the normal track image
        target.draw(m_SpriteTrackNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_SpriteTrackHover, states);

        // Undo the scale
        states.transform.scale(1.f / scaling.x, 1.f / scaling.y);

        // Check if the thumb should be scaled together with the slider
        if (m_FixedThumbSize)
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal->getSize().x * scaling.x) - (m_TextureThumbNormal->getSize().x * 0.5f),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal->getSize().y * scaling.y) - (m_TextureThumbNormal->getSize().y * 0.5f));
        }
        else // The thumb must be scaled
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal->getSize().x * scaling.x) - (m_TextureThumbNormal->getSize().x * 0.5f * scaling.y),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal->getSize().y * scaling.y) - (m_TextureThumbNormal->getSize().y * 0.5f * scaling.x));

            // Set the scale for the thumb
            states.transform.scale(scaling);
        }

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the object outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the normal thumb image
        target.draw(m_SpriteThumbNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_SpriteThumbHover, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
