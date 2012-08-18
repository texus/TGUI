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

    GroupObject::GroupObject()
    {
        m_GroupObject = true;
        m_EventManager.m_Parent = this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupObject::GroupObject(const GroupObject& groupToCopy) :
    OBJECT                         (groupToCopy),
    Group                          (groupToCopy)
    {
        m_GroupObject = true;
        m_EventManager.m_Parent = this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupObject::~GroupObject()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupObject& GroupObject::operator= (const GroupObject& right)
    {
        if (this != &right)
        {
            this->OBJECT::operator=(right);
            this->Group::operator=(right);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GroupObject::initialize()
    {
        globalFont = m_Parent->globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GroupObject::handleEvent(sf::Event& event, const float mouseX, const float mouseY)
    {
        // Check if the event is a mouse move or mouse down/press
        if (event.type == sf::Event::MouseMoved)
        {
            // Adjust the mouse position of the event
            event.mouseMove.x = static_cast<int>((mouseX - getPosition().x) / getScale().x);
            event.mouseMove.y = static_cast<int>((mouseY - getPosition().y) / getScale().y);
        }
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::MouseButtonReleased))
        {
            // Adjust the mouse position of the event
            event.mouseButton.x = static_cast<int>((mouseX - getPosition().x) / getScale().x);
            event.mouseButton.y = static_cast<int>((mouseY - getPosition().y) / getScale().y);
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GroupObject::focusNextObject()
    {
        return m_EventManager.focusNextObject();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

