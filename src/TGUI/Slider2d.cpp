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
    m_FixedThumbSize     (true)
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
    m_FixedThumbSize     (copy.m_FixedThumbSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal, m_TextureTrackNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover, m_TextureTrackHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::~Slider2d()
    {
        // Remove all the textures
        if (m_TextureTrackNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);
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
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d* Slider2d::clone()
    {
        return new Slider2d(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::load(const std::string& configFileFilename)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Remove all textures if they were loaded before
        if (m_TextureTrackNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);

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
        if (!configFile.read("Slider2d", properties, values))
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

///!!! TODO  Add SeperateHoverImage option

            if (property == "tracknormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Slider2d in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Slider2d in " + configFileFilename + ".");
                    return false;
                }
            }
            if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Slider2d in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Slider2d in " + configFileFilename + ".");
                    return false;
                }
            }
        }

        // Make sure the required textures were loaded
        if ((m_TextureTrackNormal.data != NULL) && (m_TextureThumbNormal.data != NULL))
        {
            // Set the size of the slider
            m_Size = Vector2f(m_TextureTrackNormal.getSize());
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider2d section in " + configFileFilename + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if ((m_TextureTrackHover.data != NULL) && (m_TextureThumbHover.data != NULL))
        {
            m_ObjectPhase |= ObjectPhase_Hover;
        }

        return m_Loaded = true;
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
        scaling.x = m_Size.x / m_TextureTrackNormal.getSize().x;
        scaling.y = m_Size.y / m_TextureTrackNormal.getSize().y;

        // Set the scale of the slider
        states.transform.scale(scaling);

        // Draw the normal track image
        target.draw(m_TextureTrackNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_TextureTrackHover, states);

        // Undo the scale
        states.transform.scale(1.f / scaling.x, 1.f / scaling.y);

        // Check if the thumb should be scaled together with the slider
        if (m_FixedThumbSize)
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal.getSize().x * scaling.x) - (m_TextureThumbNormal.getSize().x * 0.5f),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal.getSize().y * scaling.y) - (m_TextureThumbNormal.getSize().y * 0.5f));
        }
        else // The thumb must be scaled
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal.getSize().x * scaling.x) - (m_TextureThumbNormal.getSize().x * 0.5f * scaling.y),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal.getSize().y * scaling.y) - (m_TextureThumbNormal.getSize().y * 0.5f * scaling.x));

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
        target.draw(m_TextureThumbNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_TextureThumbHover, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
