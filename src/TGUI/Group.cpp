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

    Group::Group()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Group& groupToCopy) :
    globalFont(groupToCopy.globalFont)
    {
        // Copy all the objects
        for (unsigned int i=0; i<groupToCopy.m_EventManager.m_Objects.size(); ++i)
        {
            OBJECT* newObject = static_cast<OBJECT*>(groupToCopy.m_EventManager.m_Objects[i]->clone());
            m_EventManager.m_Objects.push_back(newObject);
            m_ObjName.push_back(groupToCopy.m_ObjName[i]);

            newObject->m_Parent = this;
            newObject->initialize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::~Group()
    {
        removeAllObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group& Group::operator= (const Group& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            // Copy the font
            globalFont = right.globalFont;

            // Remove all the old objects
            removeAllObjects();

            // Copy all the objects
            for (unsigned int i=0; i<right.m_EventManager.m_Objects.size(); ++i)
            {
                OBJECT* newObject = static_cast<OBJECT*>(right.m_EventManager.m_Objects[i]->clone());
                m_EventManager.m_Objects.push_back(newObject);
                m_ObjName.push_back(right.m_ObjName[i]);

                newObject->m_Parent = this;
                newObject->initialize();
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<OBJECT*>& Group::getObjects()
    {
        return m_EventManager.m_Objects;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string>& Group::getObjectNames()
    {
        return m_ObjName;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::remove(const std::string objectName)
    {
        // Loop through every object
        for (unsigned int i=0; i<m_ObjName.size(); ++i)
        {
            // Check if the name matches
            if (m_ObjName[i].compare(objectName) == 0)
            {
                // Remove the object
                delete m_EventManager.m_Objects[i];
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i);

                // Also emove the name it from the list
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::remove(OBJECT* object)
    {
        // Loop through every object
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Check if the pointer matches
            if (m_EventManager.m_Objects[i] == object)
            {
                // Unfocus the object, just in case it was focused
                m_EventManager.unfocusObject(object);

                // Remove the object
                delete m_EventManager.m_Objects[i];
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i);
                object = NULL;

                // Also emove the name it from the list
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::removeAllObjects()
    {
        // Delete all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
            delete m_EventManager.m_Objects[i];

        // Clear the lists
        m_EventManager.m_Objects.clear();
        m_ObjName.clear();

        // There are no more objects, so none of the objects can be focused
        m_EventManager.m_FocusedObject = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            if (m_EventManager.m_Objects[i]->m_ObjectType == radioButton)
                static_cast<RadioButton*>(m_EventManager.m_Objects[i])->m_Checked = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::focus(OBJECT* object)
    {
        m_EventManager.focusObject(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::unfocus(OBJECT* object)
    {
        m_EventManager.focusObject(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::moveObjectToFront(OBJECT* object)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Check if the object is found
            if (m_EventManager.m_Objects[i] == object)
            {
                // Copy the object
                m_EventManager.m_Objects.push_back(m_EventManager.m_Objects[i]);
                m_ObjName.push_back(m_ObjName[i]);

                // Focus the correct object
                if ((m_EventManager.m_FocusedObject == 0) || (m_EventManager.m_FocusedObject == i+1))
                    m_EventManager.m_FocusedObject = m_EventManager.m_Objects.size()-1;
                else if (m_EventManager.m_FocusedObject > i+1)
                    --m_EventManager.m_FocusedObject;

                // Remove the old object
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i);
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::moveObjectToBack(OBJECT* object)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Check if the object is found
            if (m_EventManager.m_Objects[i] == object)
            {
                // Copy the object
                OBJECT* obj = m_EventManager.m_Objects[i];
                std::string name = m_ObjName[i];
                m_EventManager.m_Objects.insert(m_EventManager.m_Objects.begin(), obj);
                m_ObjName.insert(m_ObjName.begin(), name);

                // Focus the correct object
                if (m_EventManager.m_FocusedObject == i+1)
                    m_EventManager.m_FocusedObject = 1;
                else if (m_EventManager.m_FocusedObject)
                    ++m_EventManager.m_FocusedObject;

                // Remove the old object
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i + 1);
                m_ObjName.erase(m_ObjName.begin() + i + 1);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::updateTime(const sf::Time& elapsedTime)
    {
        m_EventManager.updateTime(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::drawObjectGroup(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Draw the object if it is visible
            if (m_EventManager.m_Objects[i]->m_Visible)
                target->draw(*m_EventManager.m_Objects[i], states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

