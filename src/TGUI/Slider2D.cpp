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

// Special thanks to Dmitry for the Slider2D object !


#include <TGUI/TGUI.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2D::Slider2D() :
    returnToCenter      (true),
    fixedThumbSize      (true),
    m_Minimum           (-1, -1),
    m_Maximum           (1, 1),
    m_Value             (0, 0),
    m_TextureThumbNormal(NULL),
    m_TextureThumbHover (NULL),
    m_TextureTrackNormal(NULL),
    m_TextureTrackHover (NULL),
    m_LoadedPathname    ("")
    {
        m_ObjectType = slider2D;
        m_DraggableObject = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2D::Slider2D(const Slider2D& copy) :
    OBJECT          (copy),
    returnToCenter  (copy.returnToCenter),
    fixedThumbSize  (copy.fixedThumbSize),
    m_Minimum       (copy.m_Minimum),
    m_Maximum       (copy.m_Maximum),
    m_Value         (copy.m_Value),
    m_LoadedPathname(copy.m_LoadedPathname)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal, m_TextureTrackNormal))   m_SpriteTrackNormal.setTexture(*m_TextureTrackNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover, m_TextureTrackHover))     m_SpriteTrackHover.setTexture(*m_TextureTrackHover);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal))   m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover))     m_SpriteThumbHover.setTexture(*m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2D::~Slider2D()
    {
        // Remove all the textures
        if (m_TextureTrackNormal != NULL)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover != NULL)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal != NULL)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover != NULL)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2D& Slider2D::operator= (const Slider2D& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Slider2D temp(right);
            this->OBJECT::operator=(right);

            std::swap(returnToCenter,       temp.returnToCenter);
            std::swap(fixedThumbSize,       temp.fixedThumbSize);
            std::swap(m_Minimum,            temp.m_Minimum);
            std::swap(m_Maximum,            temp.m_Maximum);
            std::swap(m_Value,              temp.m_Value);
            std::swap(m_TextureTrackNormal, temp.m_TextureTrackNormal);
            std::swap(m_TextureTrackHover,  temp.m_TextureTrackHover);
            std::swap(m_TextureThumbNormal, temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,  temp.m_TextureThumbHover);
            std::swap(m_SpriteTrackNormal,  temp.m_SpriteTrackNormal);
            std::swap(m_SpriteTrackHover,   temp.m_SpriteTrackHover);
            std::swap(m_SpriteThumbNormal,  temp.m_SpriteThumbNormal);
            std::swap(m_SpriteThumbHover,   temp.m_SpriteThumbHover);
            std::swap(m_LoadedPathname,     temp.m_LoadedPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2D* Slider2D::clone()
    {
        return new Slider2D(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2D::load(const std::string pathname)
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


        bool error = false;

        // load the required textures
        if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "Track_Normal." + imageExtension, m_TextureTrackNormal))
            && (TGUI_TextureManager.getTexture(m_LoadedPathname + "Thumb_Normal." + imageExtension, m_TextureThumbNormal)))
        {
            m_SpriteTrackNormal.setTexture(*m_TextureTrackNormal, true);
            m_SpriteThumbNormal.setTexture(*m_TextureThumbNormal, true);
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
                error = true;
        }

        // When there is no error we will return true
        m_Loaded = !error;
        return !error;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        setScale(width / m_TextureTrackNormal->getSize().x, height / m_TextureTrackNormal->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Slider2D::getSize() const
    {
        // Don't continue when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2u(0, 0);

        return Vector2u(m_TextureTrackNormal->getSize().x, m_TextureTrackNormal->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider2D::getScaledSize() const
    {
        // Don't continue when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2f(0, 0);

        return Vector2f(m_TextureTrackNormal->getSize().x * getScale().x, m_TextureTrackNormal->getSize().y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Slider2D::getLoadedPathname()
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::setMinimum(Vector2f minimum)
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

    void Slider2D::setMaximum(Vector2f maximum)
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

    void Slider2D::centerThumb()
    {
        setValue(Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::setValue(Vector2f value)
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

    Vector2f Slider2D::getMinimum()
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider2D::getMaximum()
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider2D::getValue()
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2D::mouseOnObject(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the track
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
            return true;

        // The mouse is not on top of the slider
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::leftMouseReleased(float x, float y)
    {
		TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        m_MouseDown = false;

        if (returnToCenter)
        {
            setValue(Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));
            if (callbackID > 0)
            {
                Callback callback;
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::valueChanged;
                m_Parent->addCallback(callback);
            }
		}
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Slider2D::mouseNoLongerDown()
	{
		m_MouseDown = false;

		if (returnToCenter)
		{
		    if (m_Value != Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f))
		    {
		        setValue(Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));
                if (callbackID > 0)
                {
                    Callback callback;
                    callback.object     = this;
                    callback.callbackID = callbackID;
                    callback.trigger    = Callback::valueChanged;
                    m_Parent->addCallback(callback);
                }
		    }
		}
	}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

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
                m_Value.y = ((y - position.y) / (m_TextureTrackNormal->getSize().y * curScale.y)) * (m_Maximum.y - m_Minimum.y) + m_Minimum.y;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_Value.y = m_Minimum.y;

            // If the position is positive then calculate the correct value
            if ((x - position.x) > 0)
                m_Value.x = ((x - position.x) / (m_TextureTrackNormal->getSize().x * curScale.x)) * (m_Maximum.x - m_Minimum.x) + m_Minimum.x;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_Value.x = m_Minimum.x;

            // Set the new value, making sure that it lies within the minimum and maximum
            setValue(m_Value);

            // Add the callback (if the user requested it and the value has changed)
            if ((callbackID > 0) && (oldValue != m_Value))
            {
                Callback callback;
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::valueChanged;
                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::objectFocused()
    {
        // A slider can't be focused (yet)
        m_Parent->unfocus(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2D::draw(sf::RenderTarget& target, sf::RenderStates states) const
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
        Vector2f bottomRightPosition = states.transform.transformPoint(getPosition().x + (m_TextureTrackNormal->getSize().x * curScale.x) - target.getView().getCenter().x + (target.getView().getSize().x / 2.f),
                                                                       getPosition().y + (m_TextureTrackNormal->getSize().y * curScale.y) - target.getView().getCenter().y + (target.getView().getSize().y / 2.f));

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Adjust the transformation
        states.transform *= getTransform();

        // Draw the normal track image
        target.draw(m_SpriteTrackNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
            target.draw(m_SpriteTrackHover, states);

        // Reset the transform
        states.transform = oldTransform;
        states.transform.translate(getPosition());

        // Check if the thumb should be scaled together with the slider
        if (fixedThumbSize)
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal->getSize().x * curScale.x) - (m_TextureThumbNormal->getSize().x * 0.5f),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal->getSize().y * curScale.y) - (m_TextureThumbNormal->getSize().y * 0.5f));
        }
        else // The thumb must be scaled
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal->getSize().x * curScale.x) - (m_TextureThumbNormal->getSize().x * 0.5f * curScale.y),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal->getSize().y * curScale.y) - (m_TextureThumbNormal->getSize().y * 0.5f * curScale.x));

            // Set the scale for the thumb
            states.transform.scale(curScale.x, curScale.y);
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
