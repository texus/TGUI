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
#include <TGUI/GroupObject.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EventManager::EventManager() :
    m_FocusedObject(0),
    m_Parent       (NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::handleEvent(sf::Event& event)
    {
        // Check if a mouse button has moved
        if (event.type == sf::Event::MouseMoved)
        {
            // Loop through all objects
            for (unsigned int i=0; i<m_Objects.size(); ++i)
            {
                // Check if the mouse went down on the object
                if (m_Objects[i]->m_MouseDown)
                {
                    // Some objects should always receive mouse move events while dragging them, even if the mouse is no longer on top of them.
                    if (m_Objects[i]->m_DraggableObject)
                    {
                        m_Objects[i]->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        return;
                    }

                    // Groups also need a different treatment
                    else if (m_Objects[i]->m_GroupObject)
                    {
                        // Make the event handler of the group do the rest
                        static_cast<GroupObject::Ptr>(m_Objects[i])->handleEvent(event, static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        return;
                    }
                }
            }

            // Check if the mouse is on top of an object
            unsigned int objectNr;
            if (mouseOnObject(objectNr, static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)))
            {
                // Check if the object is a group
                if (m_Objects[objectNr]->m_GroupObject)
                {
                    // Make the event handler of the group do the rest
                    static_cast<GroupObject::Ptr>(m_Objects[objectNr])->handleEvent(event, static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                }
                else // Send the event to the object
                    m_Objects[objectNr]->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            }
        }

        // Check if a mouse button was pressed
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            // Check if the left mouse was pressed
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse is on top of an object
                unsigned int objectNr;
                if (mouseOnObject(objectNr, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
                {
                    // Focus the object
                    focusObject(m_Objects[objectNr].get());

                    // Check if the object is a group
                    if (m_Objects[objectNr]->m_GroupObject)
                    {
                        // If another object was focused then unfocus it now
                        if ((m_FocusedObject) && (m_FocusedObject != objectNr+1))
                        {
                            m_Objects[m_FocusedObject-1]->m_Focused = false;
                            m_Objects[m_FocusedObject-1]->objectUnfocused();
                            m_FocusedObject = 0;
                        }

                        // Make the event handler of the group do the rest
                        static_cast<GroupObject::Ptr>(m_Objects[objectNr])->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    }
                    else // The event has to be sent to an object
                        m_Objects[objectNr]->leftMousePressed(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
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
                // Check if the mouse is on top of an object
                unsigned int objectNr;
                if (mouseOnObject(objectNr, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
                {
                    // Check if the object is a group
                    if (m_Objects[objectNr]->m_GroupObject)
                    {
                        // Make the event handler of the group do the rest
                        static_cast<GroupObject::Ptr>(m_Objects[objectNr])->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    }
                    else // Send the event to the object
                        m_Objects[objectNr]->leftMouseReleased(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

                    // Tell all the other objects that the mouse has gone up
                    for (unsigned int i=0; i<m_Objects.size(); ++i)
                    {
                        if (i != objectNr)
                        {
                            if (m_Objects[i]->m_GroupObject)
                                static_cast<GroupObject::Ptr>(m_Objects[i])->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
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
                        if (m_Objects[i]->m_GroupObject)
                            static_cast<GroupObject::Ptr>(m_Objects[i])->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                        else
                            m_Objects[i]->mouseNoLongerDown();
                    }
                }
            }
        }

        // Check if a key was pressed
        else if (event.type == sf::Event::KeyPressed)
        {
            // Only continue when the character was recognised
            if (event.key.code != sf::Keyboard::Unknown)
            {
                // Check if there is a focused object
                if (m_FocusedObject)
                {
                    // Check if the object is a group
                    if (m_Objects[m_FocusedObject-1]->m_GroupObject)
                    {
                        // Make the event handler of the group do the rest
                        static_cast<GroupObject::Ptr>(m_Objects[m_FocusedObject-1])->handleEvent(event);
                    }
                    else // The event has to be send to a normal object
                    {
                        // Check the pressed key
                        if ((event.key.code == sf::Keyboard::Left)
                         || (event.key.code == sf::Keyboard::Right)
                         || (event.key.code == sf::Keyboard::Up)
                         || (event.key.code == sf::Keyboard::Down)
                         || (event.key.code == sf::Keyboard::BackSpace)
                         || (event.key.code == sf::Keyboard::Delete)
                         || (event.key.code == sf::Keyboard::Space)
                         || (event.key.code == sf::Keyboard::Return))
                        {
                            // Tell the object that the key was pressed
                            m_Objects[m_FocusedObject-1]->keyPressed(event.key.code);
                        }
                    }
                }
            }
        }

        // Check if a key was released
        else if (event.type == sf::Event::KeyReleased)
        {
            // Change the focus to another object when the tab key was pressed
            if (event.key.code == sf::Keyboard::Tab)
                tabKeyPressed();
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 30) && (event.text.unicode != 127))
            {
                // Check if there is a focused object
                if (m_FocusedObject)
                {
                    // Check if the object is a group
                    if (m_Objects[m_FocusedObject-1]->m_GroupObject)
                    {
                        // Make the event handler of the group do the rest
                        static_cast<GroupObject::Ptr>(m_Objects[m_FocusedObject-1])->handleEvent(event);
                    }
                    else // Tell the object that the key was pressed
                        m_Objects[m_FocusedObject-1]->textEntered(event.text.unicode);
                }
            }
        }

        // Check for mouse wheel scrolling
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            // Find the object under the mouse
            unsigned int objectNr;
            if (mouseOnObject(objectNr, static_cast<float>(event.mouseWheel.x), static_cast<float>(event.mouseWheel.y)))
            {
                // Check if the object is a group
                if (m_Objects[objectNr]->m_GroupObject)
                {
                    // Make the event handler of the group do the rest
                    static_cast<GroupObject::Ptr>(m_Objects[objectNr])->handleEvent(event, static_cast<float>(event.mouseWheel.x), static_cast<float>(event.mouseWheel.y));
                }
                else // Send the event to the object
                    m_Objects[objectNr]->mouseWheelMoved(event.mouseWheel.delta);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::focusObject(Object *const object)
    {
        // Loop all the objects
        for (unsigned int i = 0; i < m_Objects.size(); ++i)
        {
            // Search for the object that has to be focused
            if (m_Objects[i].get() == object)
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
                    object->m_Focused = true;
                    object->objectFocused();
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusObject(Object *const object)
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
                object->objectUnfocused();
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
            if (m_Objects[i]->m_AnimatedObject)
            {
                // Update the elapsed time
                m_Objects[i]->m_AnimationTimeElapsed += elapsedTime;
                m_Objects[i]->update();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::tabKeyPressed()
    {
        // Don't do anything when the tab key usage is disabled
        if (tabKeyUsageEnabled == false)
            return;

        // Check if a group is focused
        if (m_FocusedObject)
        {
            if (m_Objects[m_FocusedObject-1]->m_GroupObject)
            {
                // Focus the next object in group
                if (static_cast<GroupObject::Ptr>(m_Objects[m_FocusedObject-1])->focusNextObject())
                    return;
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
                        return;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::focusNextObject()
    {
        // Don't do anything when the tab key usage is disabled
        if (tabKeyUsageEnabled == false)
            return false;

        // Loop through all objects
        for (unsigned int i=m_FocusedObject; i<m_Objects.size(); ++i)
        {
            // If you are not allowed to focus the object, then skip it
            if (m_Objects[i]->m_AllowFocus == true)
            {
                // Make sure that the object is visible and enabled
                if ((m_Objects[i]->m_Visible) && (m_Objects[i]->m_Enabled))
                {
                    if (m_FocusedObject > 0)
                    {
                        // Unfocus the current object
                        m_Objects[m_FocusedObject-1]->m_Focused = false;
                        m_Objects[m_FocusedObject-1]->objectUnfocused();
                    }

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
