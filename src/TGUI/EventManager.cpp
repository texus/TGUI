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

    EventManager::EventManager() :
    m_WindowScale(1, 1),
    m_Parent     (NULL)
    {
        // Reset all the key flags
        for (unsigned int x=0; x<sf::Keyboard::KeyCount; ++x)
            m_KeyPress[x] = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::handleEvent(sf::Event& event, const sf::View& view)
    {
        // Check if a mouse button has moved
        if (event.type == sf::Event::MouseMoved)
        {
            unsigned int objectNr;
            float mouseX = event.mouseMove.x / m_WindowScale.x + view.getCenter().x - (view.getSize().x / 2.f);
            float mouseY = event.mouseMove.y / m_WindowScale.y + view.getCenter().y - (view.getSize().y / 2.f);

            // Loop through all objects
            for (unsigned int i=0; i<m_Objects.size(); ++i)
            {
                // Check if the mouse went down on the object
                if (m_Objects[i]->m_MouseDown)
                {
                    // Some objects should always receive mouse move events while dragging them,
                    // even if the mouse is no longer on top of them.
                    if ((m_Objects[i]->m_ObjectType == editBox)
                     || (m_Objects[i]->m_ObjectType == slider)
                     || (m_Objects[i]->m_ObjectType == scrollbar)
                     || (m_Objects[i]->m_ObjectType == listbox)
                     || (m_Objects[i]->m_ObjectType == comboBox)
                     || (m_Objects[i]->m_ObjectType == textBox))
                    {
                        m_Objects[i]->mouseMoved(mouseX, mouseY);
                        return;
                    }

                    // Groups also need a different treatment
                    else if (m_Objects[i]->m_ObjectType == panel)
                    {
                        // Make the event handler of the group do the rest
                        static_cast<Panel*>(m_Objects[i])->handleEvent(event, mouseX, mouseY);
                        return;
                    }
                }
            }

            // Check if the mouse is on top of an object
            if (mouseOnObject(objectNr, mouseX, mouseY))
            {
                // Check if the object is a group
                if (m_Objects[objectNr]->m_ObjectType == panel)
                {
                    // Make the event handler of the group do the rest
                    static_cast<Panel*>(m_Objects[objectNr])->handleEvent(event, mouseX, mouseY);
                }
                else // Send the event to the object
                    m_Objects[objectNr]->mouseMoved(mouseX, mouseY);
            }
        }

        // Check if a mouse button was pressed
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            // Check if the left mouse was pressed
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                unsigned int objectNr;
                float mouseX = event.mouseButton.x / m_WindowScale.x + view.getCenter().x - (view.getSize().x / 2.f);
                float mouseY = event.mouseButton.y / m_WindowScale.y + view.getCenter().y - (view.getSize().y / 2.f);

                // Check if the mouse is on top of an object
                if (mouseOnObject(objectNr, mouseX, mouseY))
                {
                    // Check if the object is a group
                    if (m_Objects[objectNr]->m_ObjectType == panel)
                    {
                        // If an object was focused then unfocus it
                        unfocusAllObjects();

                        // Make the event handler of the group do the rest
                        static_cast<Panel*>(m_Objects[objectNr])->handleEvent(event, mouseX, mouseY);
                    }
                    else // The event has to be sent to an object
                    {
                        // Check if the object is not directly linked to the main window
                        if (m_Parent != NULL)
                        {
                            // Check if the group is not yet focused
                            if (m_Parent->isFocused() == false)
                            {
                                // Focus the group
                                m_Parent->m_Parent->focus(m_Parent);
                            }
                        }

                        // Focus the object
                        setFocus(m_Objects[objectNr]->m_ObjectID);

                        // Send the event to the object
                        m_Objects[objectNr]->leftMousePressed(mouseX, mouseY);
                    }
                }
                else // The mouse didn't went down on an object, so unfocus the focused object
                    unfocusAllObjects();
            }
        }

        // Check if a mouse button was released
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            // Check if the left mouse was released
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                unsigned int objectNr;
                float mouseX = event.mouseButton.x / m_WindowScale.x + view.getCenter().x - (view.getSize().x / 2.f);
                float mouseY = event.mouseButton.y / m_WindowScale.y + view.getCenter().y - (view.getSize().y / 2.f);

                // Check if the mouse is on top of an object
                if (mouseOnObject(objectNr, mouseX, mouseY))
                {
                    // Check if the object is a group
                    if (m_Objects[objectNr]->m_ObjectType == panel)
                    {
                        // Make the event handler of the group do the rest
                        static_cast<Panel*>(m_Objects[objectNr])->handleEvent(event, mouseX, mouseY);
                    }
                    else // Send the event to the object
                        m_Objects[objectNr]->leftMouseReleased(mouseX, mouseY);

                    // Tell all the other objects that the mouse has gone up
                    for (unsigned int i=0; i<m_Objects.size(); ++i)
                    {
                        if (i != objectNr)
                        {
                            if (m_Objects[i]->m_ObjectType == panel)
                                static_cast<Panel*>(m_Objects[i])->handleEvent(event, mouseX, mouseY);
                            else
                                m_Objects[i]->mouseNoLongerDown();
                        }
                    }
                }
                else
                {
                    // Tell all the objects that the mouse has gone up
                    for (unsigned int i=0; i<m_Objects.size(); ++i)
                    {
                        if (m_Objects[i]->m_ObjectType == panel)
                            static_cast<Panel*>(m_Objects[i])->handleEvent(event, mouseX, mouseY);
                        else
                            m_Objects[i]->mouseNoLongerDown();
                    }
                }
            }
        }

        // Check if a key was pressed
        else if (event.type == sf::Event::KeyPressed)
        {
            // Mark the key as down
            m_KeyPress[event.key.code] = true;

            // Some keys may be repeated
            if ((event.key.code == sf::Keyboard::Left)
             || (event.key.code == sf::Keyboard::Right)
             || (event.key.code == sf::Keyboard::Up)
             || (event.key.code == sf::Keyboard::Down)
             || (event.key.code == sf::Keyboard::BackSpace)
             || (event.key.code == sf::Keyboard::Delete))
            {
                // Loop through all the object
                for (unsigned int i=0; i<m_Objects.size(); ++i)
                {
                    // Check if the object is focused
                    if (m_Objects[i]->m_Focused == true)
                    {
                        // Check if the object is a group
                        if (m_Objects[i]->m_ObjectType == panel)
                        {
                            // Make the event handler of the group do the rest
                            static_cast<Panel*>(m_Objects[i])->handleEvent(event);
                        }
                        else // Tell the object that the key was pressed
                            m_Objects[i]->keyPressed(event.key.code);
                    }
                }
            }
        }

        // Check if a key was released
        else if (event.type == sf::Event::KeyReleased)
        {
            // We don't handle the tab key as it is an exception
            if (event.key.code != sf::Keyboard::Tab)
            {
                // Check if nothing happend since the key was pressed
                if (m_KeyPress[event.key.code] == true)
                {
                    // Mark the key as released
                    m_KeyPress[event.key.code] = false;

                    // Avoid double callback with keys that can be repeated
                    if ((event.key.code != sf::Keyboard::Left)
                     && (event.key.code != sf::Keyboard::Right)
                     && (event.key.code != sf::Keyboard::Up)
                     && (event.key.code != sf::Keyboard::Down)
                     && (event.key.code != sf::Keyboard::BackSpace)
                     && (event.key.code != sf::Keyboard::Delete))
                    {
                        // Loop through all the object
                        for (unsigned int i=0; i<m_Objects.size(); ++i)
                        {
                            // Check if the object is focused
                            if (m_Objects[i]->m_Focused == true)
                            {
                                // Check if the object is a group
                                if (m_Objects[i]->m_ObjectType == panel)
                                {
                                    // Make the event handler of the group do the rest
                                    static_cast<Panel*>(m_Objects[i])->handleEvent(event);
                                }
                                else // Tell the object that the key was pressed
                                    m_Objects[i]->keyPressed(event.key.code);
                            }
                        }
                    }
                }
            }
            // Also check the tab key
            else if (event.key.code == sf::Keyboard::Tab)
            {
                // Check if nothing happend since the tab key was pressed
                if (m_KeyPress[sf::Keyboard::Tab] == true)
                {
                    // Change the focus to another object
                    tabkeyPressed();
                }
            }
        }

        // Also chack if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 30) && (event.text.unicode <= 126))
            {
                // Loop through all the object
                for (unsigned int i=0; i<m_Objects.size(); ++i)
                {
                    // Check if the object is focused
                    if (m_Objects[i]->m_Focused == true)
                    {
                        // Check if the object is a group
                        if (m_Objects[i]->m_ObjectType == panel)
                        {
                            // Make the event handler of the group do the rest
                            static_cast<Panel*>(m_Objects[i])->handleEvent(event);
                        }
                        else // Tell the object that the key was pressed
                            m_Objects[i]->textEntered(event.text.unicode & 255);
                    }
                }
            }
        }

        // Check if the window was scaled
        else if (event.type == sf::Event::Resized)
        {
            // Change the window scale
            m_WindowScale.x = event.size.width / view.getSize().x;
            m_WindowScale.y = event.size.height / view.getSize().y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EventManager::addObject(OBJECT* object)
    {
        // This will store the highest id from our existing objects
        unsigned int highestID = 0;

        // This will be true when there already is a focused object
        bool focusedObjectFound = false;

        // Loop all objects
        for (unsigned int x=0; x<m_Objects.size(); ++x)
        {
            // If the id is higher then change the highest id
            if (m_Objects[x]->m_ObjectID > highestID)
                highestID = m_Objects[x]->m_ObjectID;

            // Check if we found a focused object
            if (m_Objects[x]->m_Focused)
                focusedObjectFound = true;
        }

        // When there is no object focused yet
        if (focusedObjectFound == false)
        {
            // The object will be focused (if allowed), but will not receive a callback for this! (the object doesn't exist yet)
            if (m_Parent != NULL)
            {
                if (m_Parent->m_Focused == true)
                    object->m_Focused = true;
            }
            else
                object->m_Focused = true;
        }

        object->m_ObjectID = highestID + 1;

        // Add the object to the list
        m_Objects.push_back(object);

        // Return the object id
        return object->m_ObjectID;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EventManager::removeObject(unsigned int id)
    {
        // Ignore when the id is 0 (already removed)
        if (id == 0)
            return 0;

        // Loop all objects
        for (unsigned int x=0; x<m_Objects.size(); ++x)
        {
            // Check if the id matches
            if (m_Objects[x]->m_ObjectID == id)
            {
                // Try to focus on another object (if this object was focused)
                if (m_Objects[x]->m_Focused)
                    tabkeyPressed();

                // Remove the object from the list
                m_Objects.erase(m_Objects.begin()+x);
            }
        }

        // Return the new id
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::removeAllObjects()
    {
        m_Objects.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::setFocus(unsigned int id)
    {
        // These will store the currently focused and the new focused object numbers
        unsigned int currentlyFocusedObjectNr = 0;
        unsigned int newFocusedObjectNr = 0;

        bool currentFocusedObjectFound = false;
        bool newFocusedObjectFound = false;

        // Loop all the objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Search for the focused object
            if (m_Objects[i]->m_Focused)
            {
                currentlyFocusedObjectNr = i;
                currentFocusedObjectFound = true;
            }

            // Also search for the id of the object that has to be focused
            else if (m_Objects[i]->m_ObjectID == id)
            {
                newFocusedObjectNr = i;
                newFocusedObjectFound = true;
            }
        }

        // Check if we the object that has to be focused
        if (newFocusedObjectFound)
        {
            // We don't have to do the rest when we are trying to focus an object that is already focused
            if ((currentlyFocusedObjectNr != newFocusedObjectNr) || (currentFocusedObjectFound == false))
            {
                // unfocus the currently focused object
                if (currentFocusedObjectFound)
                {
                    m_Objects[currentlyFocusedObjectNr]->m_Focused = false;
                    m_Objects[currentlyFocusedObjectNr]->objectUnfocused();
                }

                // Focus the new object
                m_Objects[newFocusedObjectNr]->m_Focused = true;
                m_Objects[newFocusedObjectNr]->objectFocused();
            }

            // Another object is focused. Clear all key and mouse button flags
            for (unsigned int x=0; x<sf::Keyboard::KeyCount; ++x)
                m_KeyPress[x] = false;

            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusObject(OBJECT* object)
    {
        // Check if the object is focused
        if (object->m_Focused)
        {
            // Focus the next object
            tabkeyPressed();

            // Make sure that the object gets unfocused
            object->m_Focused = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusAllObjects()
    {
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the object is focused
            if (m_Objects[i]->m_Focused)
            {
                // Unfocus the object
                m_Objects[i]->m_Focused = false;
                m_Objects[i]->objectUnfocused();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::moveObjectToFront(OBJECT* object)
    {
        // Don't do anything when the id is 0
        if (object->m_ObjectID == 0)
            return;

        unsigned int highestID = 0;

        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the id is higher
            if (m_Objects[i]->m_ObjectID > object->m_ObjectID)
            {
                // Remember the highest id
                if (highestID < m_Objects[i]->m_ObjectID)
                    highestID = m_Objects[i]->m_ObjectID;

                // Change the id
                --m_Objects[i]->m_ObjectID;
            }
        }

        // If the object can be moved forward then do so
        if (highestID > 0)
            object->m_ObjectID = highestID;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::moveObjectToBack(OBJECT* object)
    {
        // Don't do anything when the id is 0
        if (object->m_ObjectID == 0)
            return;

        unsigned int lowestID = 0;

        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Ignore an id of 0
            if (m_Objects[i]->m_ObjectID != 0)
            {
                // Check if the id is lower
                if (m_Objects[i]->m_ObjectID < object->m_ObjectID)
                {
                    // Remember the lowest id
                    if (lowestID > m_Objects[i]->m_ObjectID)
                        lowestID = m_Objects[i]->m_ObjectID;

                    // Change the id
                    ++m_Objects[i]->m_ObjectID;
                }
            }
        }

        // If the object can be moved backward then do so
        if (lowestID > 0)
            object->m_ObjectID = lowestID;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::updateTime(const sf::Time& elapsedTime)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the object is a group or an object that uses the time
            if (m_Objects[i]->m_ObjectType == panel)
                dynamic_cast<Group*>(m_Objects[i])->updateTime(elapsedTime);
            else if (m_Objects[i]->m_ObjectType == editBox)
            {
                // Convert the object
                OBJECT_ANIMATION* object = dynamic_cast<OBJECT_ANIMATION*>(m_Objects[i]);

                // Update the elapsed time
                object->m_AnimationTimeElapsed += elapsedTime;
                object->update();
            }
            else if (m_Objects[i]->m_ObjectType == textBox)
            {
                // Convert the object
                OBJECT_ANIMATION* object = dynamic_cast<OBJECT_ANIMATION*>(m_Objects[i]);

                // Update the elapsed time
                object->m_AnimationTimeElapsed += elapsedTime;
                object->update();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::tabkeyPressed()
    {
        // These will hold the id and number of the current focused object
        unsigned int id = 0;
        unsigned int objectNr = 0;

        // Loop all objects
        for (unsigned int x=0; x<m_Objects.size(); ++x)
        {
            // Find the id of the focused object
            if (m_Objects[x]->m_Focused)
            {
                id = m_Objects[x]->m_ObjectID;
                objectNr = x;
                break;
            }
        }

        // If there is no focused object then exit
        if (id == 0)
            return;

        // We need two more id's and numbers to find the next object
        unsigned int id_Bigger = 0;
        unsigned int id_Smaller = 0;

        unsigned int nr_Bigger = 0;
        unsigned int nr_Smaller = 0;

        // Loop all objects again
        for (unsigned int x=0; x<m_Objects.size(); ++x)
        {
            // If you are not allowed to focus the object, then skip it
            if (m_Objects[x]->m_AllowFocus == true)
            {
                // Check if the ObjectID is bigger than the focused one
                if (m_Objects[x]->m_ObjectID > id)
                {
                    // Make sure that the object is visible and enabled
                    if ((m_Objects[x]->m_Visible) && (m_Objects[x]->m_Enabled))
                    {
                        // Check if this is the first time we pass her
                        if (id_Bigger == 0)
                        {
                            // It is the first time so just mark this object
                            id_Bigger = m_Objects[x]->m_ObjectID;
                            nr_Bigger = x;
                        }
                        else // We already marked an object
                        {
                            // Check if the new object has a lower id
                            if (m_Objects[x]->m_ObjectID < id_Bigger)
                            {
                                // If it has a lower id then mark this one
                                id_Bigger = m_Objects[x]->m_ObjectID;
                                nr_Bigger = x;
                            }
                        }
                    }
                }
                // Check if the object has a lower id
                else if (m_Objects[x]->m_ObjectID < id)
                {
                    // Make sure that the id is not 0 and that the object is visible and enabled
                    if ((m_Objects[x]->m_ObjectID != 0) && (m_Objects[x]->m_Visible) && (m_Objects[x]->m_Enabled))
                    {
                        // Check if this is the first time we pass here
                        if (id_Smaller == 0)
                        {
                            // If this is the first time then mark this object
                            id_Smaller = m_Objects.at(x)->m_ObjectID;
                            nr_Smaller = x;
                        }
                        else // We already marked an object
                        {
                            // Check if the new object has a lower id
                            if (m_Objects.at(x)->m_ObjectID < id_Smaller)
                            {
                                // If it has a lower id then mark this one
                                id_Smaller = m_Objects.at(x)->m_ObjectID;
                                nr_Smaller = x;
                            }
                        } //first time?
                    } //ObjectID=0 or invisible or disabled?
                } //Lower id?
            } //allow focus?
        } //Loop all objects

        // Check if we found an id that is bigger than ours
        if (id_Bigger != 0)
        {
            // unfocus the current object
            m_Objects.at(objectNr)->m_Focused = false;
            m_Objects.at(objectNr)->objectUnfocused();

            // Focus on the new object
            m_Objects.at(nr_Bigger)->m_Focused = true;
            m_Objects.at(nr_Bigger)->objectFocused();
        }
        else // We have the highest id
        {
            // Check if we found another object (with a lower id)
            if (id_Smaller != 0)
            {
                // unfocus the current object
                m_Objects.at(objectNr)->m_Focused = false;
                m_Objects.at(objectNr)->objectUnfocused();

                // Focus on the new object
                m_Objects.at(nr_Smaller)->m_Focused = true;
                m_Objects.at(nr_Smaller)->objectFocused();
            }
            else // There are no other objects so nothing is changed
                return;
        }

        // Another object is focused. Clear all key and mouse button flags
        for (unsigned int x=0; x<sf::Keyboard::KeyCount; ++x)
            m_KeyPress[x] = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::mouseOnObject(unsigned int& objectNr, float X, float Y)
    {
        unsigned int highestID = 0;

        // Loop through all objects
        for (unsigned int x=0; x<m_Objects.size(); ++x)
        {
            // Check if the object is visible and enabled
            if ((m_Objects[x]->m_Visible) && (m_Objects[x]->m_Enabled))
            {
                // Ask the object if the mouse is on top of them
                if (m_Objects[x]->mouseOnObject(X, Y))
                {
                    // Check if the id of the object is higher
                    if (highestID < m_Objects[x]->m_ObjectID)
                    {
                        // If there already was an object then they overlap each other
                        if (highestID > 0)
                            m_Objects[objectNr]->mouseNotOnObject();

                        // Save the new highest id
                        highestID = m_Objects[x]->m_ObjectID;

                        // Also remember what object should receive the event
                        objectNr = x;
                    }
                }
            }
        }

        // If our mouse is on top of an object then return true
        if (highestID != 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
