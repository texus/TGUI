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
        globalFont = sf::Font::getDefaultFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Group& groupToCopy) :
    globalFont(groupToCopy.globalFont)
    {
        // Copy all the objects
        for (unsigned int i=0; i<groupToCopy.m_Objects.size(); ++i)
        {
            if (groupToCopy.m_Objects[i]->m_ObjectType == picture)
                copyObject(static_cast<Picture*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == button)
                copyObject(static_cast<Button*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == checkbox)
                copyObject(static_cast<Checkbox*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == radioButton)
                copyObject(static_cast<RadioButton*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == label)
                copyObject(static_cast<Label*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == editBox)
                copyObject(static_cast<EditBox*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == listbox)
                copyObject(static_cast<Listbox*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == comboBox)
                copyObject(static_cast<ComboBox*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == slider)
                copyObject(static_cast<Slider*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == scrollbar)
                copyObject(static_cast<Scrollbar*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == loadingBar)
                copyObject(static_cast<LoadingBar*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == textBox)
                copyObject(static_cast<TextBox*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == panel)
                copyObject(static_cast<Panel*>(groupToCopy.m_Objects[i]));
            if (groupToCopy.m_Objects[i]->m_ObjectType == spriteSheet)
                copyObject(static_cast<SpriteSheet*>(groupToCopy.m_Objects[i]));
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

            // Copy all the objects
            for (unsigned int i=0; i<right.m_Objects.size(); ++i)
            {
                if (right.m_Objects[i]->m_ObjectType == picture)
                    copyObject(static_cast<Picture*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == button)
                    copyObject(static_cast<Button*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == checkbox)
                    copyObject(static_cast<Checkbox*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == radioButton)
                    copyObject(static_cast<RadioButton*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == label)
                    copyObject(static_cast<Label*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == editBox)
                    copyObject(static_cast<EditBox*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == listbox)
                    copyObject(static_cast<Listbox*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == comboBox)
                    copyObject(static_cast<ComboBox*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == slider)
                    copyObject(static_cast<Slider*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == scrollbar)
                    copyObject(static_cast<Scrollbar*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == loadingBar)
                    copyObject(static_cast<LoadingBar*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == textBox)
                    copyObject(static_cast<TextBox*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == panel)
                    copyObject(static_cast<Panel*>(right.m_Objects[i]));
                if (right.m_Objects[i]->m_ObjectType == spriteSheet)
                    copyObject(static_cast<SpriteSheet*>(right.m_Objects[i]));
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

    void Group::removeObject(const std::string objectName)
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
                    }
                }
            }

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

