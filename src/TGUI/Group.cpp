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

#include <typeinfo>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group()
    {
        globalFont = sf::Font::getDefaultFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Group& groupToCopy) :
    globalFont(groupToCopy.globalFont)
    {
        // Copy all the objects
        for (unsigned int i=0; i<groupToCopy.m_Objects.size(); ++i)
        {
            OBJECT* newObject = static_cast<OBJECT*>(groupToCopy.m_Objects[i]->clone());
            m_EventManager.addObject(newObject);

            newObject->initialize(globalFont);

            m_Objects.push_back(newObject);
            m_ObjName.push_back(groupToCopy.m_ObjName[i]);
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
            for (unsigned int i=0; i<right.m_Objects.size(); ++i)
            {
                OBJECT* newObject = static_cast<OBJECT*>(right.m_Objects[i]->clone());
                m_EventManager.addObject(newObject);

                newObject->initialize(globalFont);

                m_Objects.push_back(newObject);
                m_ObjName.push_back(right.m_ObjName[i]);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<OBJECT*>& Group::getObjects()
    {
        return m_Objects;
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
                m_EventManager.removeObject(m_Objects[i]->getObjectID());
                delete m_Objects[i];
                m_Objects.erase(m_Objects.begin() + i);

                // Also emove the name it from the list
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::remove(const OBJECT* object)
    {
        // Loop through every object
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the pointer matches
            if (m_Objects[i] == object)
            {
                // Remove the object
                m_EventManager.removeObject(m_Objects[i]->getObjectID());
                delete m_Objects[i];
                m_Objects.erase(m_Objects.begin() + i);
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
        // Remove all the objects from the event and animation manager
        m_EventManager.removeAllObjects();

        // Delete all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
            delete m_Objects[i];

        // Clear the lists
        m_Objects.clear();
        m_ObjName.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            if (m_Objects[i]->m_ObjectType == radioButton)
                static_cast<RadioButton*>(m_Objects[i])->m_Checked = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::focus(OBJECT* object)
    {
        m_EventManager.setFocus(object->getObjectID());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::unfocus(OBJECT* object)
    {
        m_EventManager.unfocusObject(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::moveObjectToFront(OBJECT* object)
    {
        m_EventManager.moveObjectToFront(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::moveObjectToBack(OBJECT* object)
    {
        m_EventManager.moveObjectToBack(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::updateTime(const sf::Time& elapsedTime)
    {
        m_EventManager.updateTime(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::drawObjectGroup(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        unsigned int objectID = 0;
        unsigned int highestID = TGUI_MAX_OBJECTS;
        unsigned int lowestID = 0;

        bool allObjectsInvisible = true;

        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Loop through all objects
            for (unsigned int j=0; j<m_Objects.size(); ++j)
            {
                // Check if the visible object has a higher id
                if ((m_Objects[j]->isVisible()) && (m_Objects[j]->m_ObjectID > lowestID))
                {
                    // Make sure that the object id is the smallest
                    if (m_Objects[j]->m_ObjectID < highestID)
                    {
                        // Remember the new highers id
                        highestID = m_Objects[j]->m_ObjectID;

                        // Remember the object that has to be drawn
                        objectID = j;

                        // We have found an object that is visible
                        allObjectsInvisible = false;
                    }
                }
            }

            // When none of the objects is visible then don't draw anything.
            if (allObjectsInvisible == true)
                break;

            // Draw the object if it is allowed
            if (lowestID < m_Objects[objectID]->m_ObjectID)
                target->draw(*m_Objects[objectID], states);

            // Remember the new lowest id
            lowestID = m_Objects[objectID]->m_ObjectID;

            // Reset the highest id
            highestID = TGUI_MAX_OBJECTS;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

