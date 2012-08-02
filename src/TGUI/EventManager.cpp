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
    m_FocusedObject(0),
    m_WindowScale  (1, 1),
    m_Parent       (NULL)
    {
        // Reset all the key flags
        for (unsigned int i=0; i<sf::Keyboard::KeyCount; ++i)
            m_KeyPress[i] = false;
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
                        focusObject(m_Objects[objectNr]);

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
                    tabKeyPressed();
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

    void EventManager::focusObject(OBJECT* object)
    {
        // Loop all the objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Search for the object that has to be focused
            if (m_Objects[i] == object)
            {
                // Only continue when the object wasn't already focused
                if (m_FocusedObject != i+1)
                {
                    // Unfocus the currently focused object
                    if (m_FocusedObject)
                    {
                        m_Objects[m_FocusedObject-1]->m_Focused = false;
                        m_Objects[m_FocusedObject-1]->objectUnfocused();
                    }

                    // Focus the new object
                    m_FocusedObject = i+1;
                    m_Objects[i]->m_Focused = true;
                    m_Objects[i]->objectFocused();
                }
                else
                    m_Objects[i]->m_Focused = true;

                // Another object is focused. Clear all key flags
                for (unsigned int j=0; j<sf::Keyboard::KeyCount; ++j)
                    m_KeyPress[j] = false;

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusObject(OBJECT* object)
    {
        // Check if the object is focused
        if (object->m_Focused)
        {
            // Focus the next object
            tabKeyPressed();

            // Make sure that the object gets unfocused
            if (object->m_Focused)
            {
                object->m_Focused = false;
                m_Objects[m_FocusedObject-1]->objectUnfocused();
                m_FocusedObject = 0;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusAllObjects()
    {
        if (m_FocusedObject)
        {
            m_Objects[m_FocusedObject-1]->m_Focused = false;
            m_Objects[m_FocusedObject-1]->objectUnfocused();
            m_FocusedObject = 0;
        }
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
            else if (m_Objects[i]->m_AnimatedObject)
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

    void EventManager::moveObjectToFront(OBJECT* object)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the object is found
            if (m_Objects[i] == object)
            {
                // Copy the object
                m_Objects.push_back(m_Objects[i]);

                // Focus the correct object
                if ((m_FocusedObject == 0) || (m_FocusedObject == i+1))
                    m_FocusedObject = m_Objects.size()-1;
                else if (m_FocusedObject > i+1)
                    --m_FocusedObject;

                // Remove the old object
                m_Objects.erase(m_Objects.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::moveObjectToBack(OBJECT* object)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the object is found
            if (m_Objects[i] == object)
            {
                // Copy the object
                m_Objects.insert(m_Objects.begin(), m_Objects[i]);

                // Focus the correct object
                if (m_FocusedObject == i+1)
                    m_FocusedObject = 1;
                else if (m_FocusedObject)
                    ++m_FocusedObject;

                // Remove the old object
                m_Objects.erase(m_Objects.begin() + i + 1);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::tabKeyPressed()
    {
        // Check if a panel is focused
        if (m_FocusedObject)
        {
            if (m_Objects[m_FocusedObject-1]->m_ObjectType == panel)
            {
                // Focus the next object in panel
                if (static_cast<tgui::Panel*>(m_Objects[m_FocusedObject-1])->focusNextObject())
                {
                    // Another object is focused. Clear all key flags
                    for (unsigned int i=0; i<sf::Keyboard::KeyCount; ++i)
                        m_KeyPress[i] = false;

                    // Don't continue, th panel has made the needed changes in his event manager
                    return;
                }
            }
        }

        // Loop all objects behind the focused one
        for (unsigned int i=m_FocusedObject; i<m_Objects.size(); ++i)
        {
            // If you are not allowed to focus the object, then skip it
            if (m_Objects[i]->m_AllowFocus == true)
            {
                // Make sure that the object is visible and enabled
                if ((m_Objects[i]->m_Visible) && (m_Objects[i]->m_Enabled))
                {
                    if (m_FocusedObject)
                    {
                        // unfocus the current object
                        m_Objects[m_FocusedObject-1]->m_Focused = false;
                        m_Objects[m_FocusedObject-1]->objectUnfocused();
                    }

                    // Focus on the new object
                    m_FocusedObject = i+1;
                    m_Objects[i]->m_Focused = true;
                    m_Objects[i]->objectFocused();

                    // Another object is focused. Clear all key flags
                    for (unsigned int j=0; j<sf::Keyboard::KeyCount; ++j)
                        m_KeyPress[j] = false;

                    return;
                }
            }
        }

        // None of the objects behind the focused one could be focused, so loop the ones before it
        if (m_FocusedObject)
        {
            for (unsigned int i=0; i<m_FocusedObject-1; ++i)
            {
                // If you are not allowed to focus the object, then skip it
                if (m_Objects[i]->m_AllowFocus == true)
                {
                    // Make sure that the object is visible and enabled
                    if ((m_Objects[i]->m_Visible) && (m_Objects[i]->m_Enabled))
                    {
                        // unfocus the current object
                        m_Objects[m_FocusedObject-1]->m_Focused = false;
                        m_Objects[m_FocusedObject-1]->objectUnfocused();

                        // Focus on the new object
                        m_FocusedObject = i+1;
                        m_Objects[i]->m_Focused = true;
                        m_Objects[i]->objectFocused();

                        // Another object is focused. Clear all key flags
                        for (unsigned int j=0; j<sf::Keyboard::KeyCount; ++j)
                            m_KeyPress[j] = false;

                        return;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::focusNextObject()
    {
        // Another object will be focused. Clear all key flags
        for (unsigned int i=0; i<sf::Keyboard::KeyCount; ++i)
        m_KeyPress[i] = false;

        // Loop through all objects
        for (unsigned int i=m_FocusedObject; i<m_Objects.size(); ++i)
        {
            // If you are not allowed to focus the object, then skip it
            if (m_Objects[i]->m_AllowFocus == true)
            {
                // Make sure that the object is visible and enabled
                if ((m_Objects[i]->m_Visible) && (m_Objects[i]->m_Enabled))
                {
                    // Unfocus the current object
                    m_Objects[m_FocusedObject-1]->m_Focused = false;
                    m_Objects[m_FocusedObject-1]->objectUnfocused();

                    // Focus on the new object
                    m_FocusedObject = i+1;
                    m_Objects[i]->m_Focused = true;
                    m_Objects[i]->objectFocused();

                    return true;
                }
            }
        }

        // We have the highest id
        unfocusAllObjects();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::mouseOnObject(unsigned int& objectNr, float x, float y)
    {
        bool objectFound = false;

        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Check if the object is visible and enabled
            if ((m_Objects[i]->m_Visible) && (m_Objects[i]->m_Enabled))
            {
                // Ask the object if the mouse is on top of them
                if (m_Objects[i]->mouseOnObject(x, y))
                {
                    // If there already was an object then they overlap each other
                    if (objectFound)
                        m_Objects[objectNr]->mouseNotOnObject();

                    // An object is found now
                    objectFound = true;

                    // Also remember what object should receive the event
                    objectNr = i;
                }
            }
        }

        // If our mouse is on top of an object then return true
        return objectFound;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::mouseNotOnObject()
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Tell the object that the mouse is no longer on top of it
            m_Objects[i]->mouseNotOnObject();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::mouseNoLongerDown()
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_Objects.size(); ++i)
        {
            // Tell the object that the mouse is no longer down
            m_Objects[i]->mouseNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
