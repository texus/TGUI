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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Object::Object() :
    m_Enabled        (true),
    m_Visible        (true),
    m_Loaded         (false),
    m_ObjectPhase    (0),
    m_Parent         (NULL),
    m_MouseHover     (false),
    m_MouseDown      (false),
    m_Focused        (false),
    m_AllowFocus     (false),
    m_AnimatedObject (false),
    m_DraggableObject(false),
    m_GroupObject    (false),
    m_Callback       ()
    {
        m_Callback.object = this;
        m_Callback.objectType = Type_Unknown;
        m_Callback.callbackId = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Object::Object(const Object& copy) :
    sf::Drawable     (copy),
    sf::Transformable(copy),
    m_Enabled        (copy.m_Enabled),
    m_Visible        (copy.m_Visible),
    m_Loaded         (copy.m_Loaded),
    m_ObjectPhase    (copy.m_ObjectPhase),
    m_Parent         (copy.m_Parent),
    m_MouseHover     (false),
    m_MouseDown      (false),
    m_Focused        (false),
    m_AllowFocus     (copy.m_AllowFocus),
    m_AnimatedObject (copy.m_AnimatedObject),
    m_DraggableObject(copy.m_DraggableObject),
    m_GroupObject    (copy.m_GroupObject),
    m_Callback       ()
    {
        m_Callback.object = this;
        m_Callback.objectType = copy.m_Callback.objectType;
        m_Callback.callbackId = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Object::~Object()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Object& Object::operator= (const Object& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            this->sf::Drawable::operator=(right);
            this->sf::Transformable::operator=(right);

            m_Enabled             = right.m_Enabled;
            m_Visible             = right.m_Visible;
            m_Loaded              = right.m_Loaded;
            m_ObjectPhase         = right.m_ObjectPhase;
            m_Parent              = right.m_Parent;
            m_MouseHover          = false;
            m_MouseDown           = false;
            m_Focused             = false;
            m_AllowFocus          = right.m_AllowFocus;
            m_AnimatedObject      = right.m_AnimatedObject;
            m_DraggableObject     = right.m_DraggableObject;
            m_GroupObject         = right.m_GroupObject;
            m_Callback            = Callback();
            m_Callback.object     = this;
            m_Callback.objectType = right.m_Callback.objectType;
            m_Callback.callbackId = right.m_Callback.callbackId;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Object::getScaledSize() const
    {
        return Vector2f(getSize().x * getScale().x, getSize().y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::show()
    {
        m_Visible = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::hide()
    {
        m_Visible = false;

        // If the object is focused then it must be unfocused
        m_Parent->unfocusObject(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Object::isVisible() const
    {
        return m_Visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::enable()
    {
        m_Enabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::disable()
    {
        m_Enabled = false;

        // Change the mouse button state.
        m_MouseHover = false;
        m_MouseDown = false;

        // If the object is focused then it must be unfocused
        m_Parent->unfocusObject(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Object::isEnabled() const
    {
        return m_Enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Object::isDisabled() const
    {
        return !m_Enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Object::isLoaded() const
    {
        return m_Loaded;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::extractPhases(std::string phases)
    {
        bool nextPhaseFound = true;
        std::string::size_type commaPos;

        while (nextPhaseFound)
        {
            // Search for the next comma
            commaPos = phases.find(',', 0);

            // Check if there was a comma
            if (commaPos != std::string::npos)
            {
                // Get the next phase
                std::string SinglePhase = phases.substr(0, commaPos);

                // Store the phase
                if (SinglePhase.compare("hover") == 0)
                    m_ObjectPhase |= ObjectPhase_Hover;
                else if (SinglePhase.compare("focus") == 0)
                    m_ObjectPhase |= ObjectPhase_Focused;
                else if (SinglePhase.compare("down") == 0)
                    m_ObjectPhase |= ObjectPhase_MouseDown;
                else if (SinglePhase.compare("selected") == 0)
                    m_ObjectPhase |= ObjectPhase_Selected;

                // Remove this phase from the string
                phases.erase(0, commaPos+1);
            }
            else // There was no comma
            {
                nextPhaseFound = false;

                // Store the phase
                if (phases.compare("hover") == 0)
                    m_ObjectPhase |= ObjectPhase_Hover;
                else if (phases.compare("focus") == 0)
                    m_ObjectPhase |= ObjectPhase_Focused;
                else if (phases.compare("down") == 0)
                    m_ObjectPhase |= ObjectPhase_MouseDown;
                else if (phases.compare("selected") == 0)
                    m_ObjectPhase |= ObjectPhase_Selected;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::focus()
    {
        m_Parent->focusObject(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::unfocus()
    {
        if (m_Focused)
            m_Parent->unfocusAllObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::focusNextObject()
    {
        m_Parent->unfocusObject(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Object::isFocused() const
    {
        return m_Focused;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectTypes Object::getObjectType() const
    {
        return m_Callback.objectType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group* Object::getParent() const
    {
        return m_Parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::moveToFront()
    {
        m_Parent->moveObjectToFront(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::moveToBack()
    {
        m_Parent->moveObjectToBack(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::setCallbackId(unsigned int callbackId)
    {
        m_Callback.callbackId = callbackId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Object::getCallbackId()
    {
        return m_Callback.callbackId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::update()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::addCallback()
    {
        // Get the list of callback functions
        std::list<CallbackFunction>& functions = m_CallbackFunctions[m_Callback.trigger];

        // Loop through all callback functions
        for (std::list<CallbackFunction>::const_iterator it = functions.begin(); it != functions.end(); ++it)
        {
            // Pass the callback to the correct place
            if (it->simpleFunction != NULL)
                it->simpleFunction();
            else if (it->extendedFunction != NULL)
                it->extendedFunction(m_Callback);
            else
                m_Parent->addChildCallback(m_Callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::mouseEnteredObject()
    {
        if (m_CallbackFunctions[MouseEntered].empty() == false)
        {
            m_Callback.trigger = MouseEntered;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::mouseLeftObject()
    {
        if (m_CallbackFunctions[MouseLeft].empty() == false)
        {
            m_Callback.trigger = MouseLeft;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::leftMousePressed(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::leftMouseReleased(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::mouseMoved(float, float)
    {
        if (m_MouseHover == false)
            mouseEnteredObject();

        m_MouseHover = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::keyPressed(sf::Keyboard::Key)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::textEntered(sf::Uint32)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::mouseWheelMoved(int)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::objectFocused()
    {
        if (m_CallbackFunctions[Focused].empty() == false)
        {
            m_Callback.trigger = Focused;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::objectUnfocused()
    {
        if (m_CallbackFunctions[Unfocused].empty() == false)
        {
            m_Callback.trigger = Unfocused;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::mouseNotOnObject()
    {
        if (m_MouseHover == true)
            mouseLeftObject();

        m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::mouseNoLongerDown()
    {
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::setRotation(float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Object::rotate(float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectBorders::ObjectBorders() :
    m_LeftBorder  (0),
    m_TopBorder   (0),
    m_RightBorder (0),
    m_BottomBorder(0)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector4u ObjectBorders::getBorders() const
    {
        return Vector4u(m_LeftBorder, m_TopBorder, m_RightBorder, m_BottomBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
