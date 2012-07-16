/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI Form Builder
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


#include "../../../include/TGUI/FormBuilder/FormBuilder.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
        return 1;

    CFRelease(resourcesURL);

    chdir(path);
#endif

    sf::Clock resizeDelayClock;

    Builder app;

    // Initialize the objects window
    if (!app.objectsWindow.loadObjectsFromFile("FormObjectsWindow.txt"))
        return 1;

    // Select the window by default
    app.newObject(tgui::window);

    // Quit when the window is closed
    while (app.mainWindow.isOpen())
    {
        sf::Event event;

        // Check the events of the main window
        while (app.mainWindow.pollEvent(event))
        {
            // Check if the window has to be closed
            if (event.type == sf::Event::Closed)
            {
                app.objectsWindow.close();
                app.propertyWindow.close();
                app.mainWindow.close();
            }

            // Check if the window was resized
            if (event.type == sf::Event::Resized)
            {
                // You must use the properties to change the size of the window
                if ((event.size.width == (TGUI_MAXIMUM(app.windows[0].width.value, 32))) && (event.size.height == (TGUI_MAXIMUM(app.windows[0].height.value, 32))))
                {
                    // Set the viewport to the new size
                    app.mainWindow.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

                    // Change the size of the transparent image
                    app.mainWindow.get<tgui::Picture>("1")->setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));

                    // Pass the change to tgui
                    app.mainWindow.handleEvent(event);
                }
                else // You tried to scale the window
                    app.mainWindow.setSize(sf::Vector2u(TGUI_MAXIMUM(app.windows[0].width.value, 32), TGUI_MAXIMUM(app.windows[0].height.value, 32)));
            }
            else
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    // Check if you start dragging one of the scale squares
                    unsigned int id = app.getScaleSquareObjectID(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    if (id > 0)
                    {
                        app.draggingSquare = id;
                        app.dragPos = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    }
                    else // None of the scale squares was clicked
                    {
                        // Get the id of the object that you clicked on
                        id = app.getClickedObjectID(event);

                        // Check if the id differs from the currently selected object
                        if (app.currentID != id)
                        {
                            // Select the clicked object
                            app.currentID = id;

                            // Show the properties of the newly selected object
                            app.resizePropertyWindow();
                        }

                        // Check if you clicked on an object
                        if (id != 1)
                        {
                            app.draggingObject = true;
                            app.dragPos = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                        }
                    }
                }
                else if (event.type == sf::Event::MouseMoved)
                {
                    // Check if you are dragging an object
                    if (app.draggingObject)
                    {
                        // The mouse should have moved at least 10 pixels left
                        while (event.mouseMove.x < app.dragPos.x - 10)
                        {
                            app.moveObjectX(-10, app.currentID);
                            app.dragPos.x -=10;
                        }

                        // The mouse should have moved at least 10 pixels right
                        while (event.mouseMove.x > app.dragPos.x + 10)
                        {
                            app.moveObjectX(10, app.currentID);
                            app.dragPos.x +=10;
                        }

                        // The mouse should have moved at least 10 pixels up
                        while (event.mouseMove.y < app.dragPos.y - 10)
                        {
                            app.moveObjectY(-10, app.currentID);
                            app.dragPos.y -=10;
                        }

                        // The mouse should have moved at least 10 pixels down
                        while (event.mouseMove.y > app.dragPos.y + 10)
                        {
                            app.moveObjectY(10, app.currentID);
                            app.dragPos.y +=10;
                        }
                    }
                    else // You are not dragging an object
                    {
                        // Check if you were dragging a scale square and the window isn't selected
                        if ((app.draggingSquare > 0) && (app.currentID > 1))
                        {
                            // Check which square you are dragging
                            if (app.draggingSquare == SQUARE_RIGHT)
                            {
                                // Check if you are pulling the right square to the right
                                while (event.mouseMove.x > app.dragPos.x + 10)
                                {
                                    app.dragPos.x += 10;
                                    app.resizeObject(10, 0, app.currentID);
                                }

                                // Check if you are pulling the right square to the left
                                while (event.mouseMove.x < app.dragPos.x - 10)
                                {
                                    app.dragPos.x -= 10;
                                    app.resizeObject(-10, 0, app.currentID);
                                }

                                // Store the new aspect ratio of the object
                                app.storeObjectsNewAspectRatio();
                            }
                            else if (app.draggingSquare == SQUARE_LEFT)
                            {
                                // Check if you are pulling the left square to the right
                                while (event.mouseMove.x > app.dragPos.x + 10)
                                {
                                    app.dragPos.x += 10;
                                    app.resizeObject(-10, 0, app.currentID);
                                    app.moveObjectX(10, app.currentID, true);
                                }

                                // Check if you are pulling the left square to the left
                                while (event.mouseMove.x < app.dragPos.x - 10)
                                {
                                    app.dragPos.x -= 10;
                                    app.resizeObject(10, 0, app.currentID);
                                    app.moveObjectX(-10, app.currentID, true);
                                }

                                // Store the new aspect ratio of the object
                                app.storeObjectsNewAspectRatio();
                            }
                            else if (app.draggingSquare == SQUARE_BOTTOM)
                            {
                                // Check if you are pulling the bottom square down
                                while (event.mouseMove.y > app.dragPos.y + 10)
                                {
                                    app.dragPos.y += 10;
                                    app.resizeObject(0, 10, app.currentID);
                                }

                                // Check if you are pulling the bottom square up
                                while (event.mouseMove.y < app.dragPos.y - 10)
                                {
                                    app.dragPos.y -= 10;
                                    app.resizeObject(0, -10, app.currentID);
                                }

                                // Store the new aspect ratio of the object
                                app.storeObjectsNewAspectRatio();
                            }
                            else if (app.draggingSquare == SQUARE_TOP)
                            {
                                // Check if you are pulling the top square down
                                while (event.mouseMove.y > app.dragPos.y + 10)
                                {
                                    app.dragPos.y += 10;
                                    app.resizeObject(0, -10, app.currentID);
                                    app.moveObjectY(10, app.currentID, true);
                                }

                                // Check if you are pulling the top square up
                                while (event.mouseMove.y < app.dragPos.y - 10)
                                {
                                    app.dragPos.y -= 10;
                                    app.resizeObject(0, 10, app.currentID);
                                    app.moveObjectY(-10, app.currentID, true);
                                }

                                // Store the new aspect ratio of the object
                                app.storeObjectsNewAspectRatio();
                            }
                            else // One of the corners was selected
                            {
                                // Check if the object is wider than it is high
                                if (app.aspectRatios[app.currentID-2] < 1)
                                {
                                    float ratio = 10.f * app.aspectRatios[app.currentID-2];

                                    // Check which corner is being dragged
                                    if (app.draggingSquare == SQUARE_TOP_LEFT)
                                    {
                                        // Check if you are pulling the top left square to the upper left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(10, ratio, app.currentID);
                                            app.moveObjectX(-10, app.currentID, true);
                                            app.moveObjectY(-ratio, app.currentID, true);
                                        }

                                        // Check if you are pulling the top left square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(-10, -ratio, app.currentID);
                                            app.moveObjectX(10, app.currentID, true);
                                            app.moveObjectY(ratio, app.currentID, true);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_TOP_RIGHT)
                                    {
                                        // Check if you are pulling the top right square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(10, ratio, app.currentID);
                                            app.moveObjectY(-ratio, app.currentID, true);
                                        }

                                        // Check if you are pulling the top right square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(-10, -ratio, app.currentID);
                                            app.moveObjectY(ratio, app.currentID, true);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_LEFT)
                                    {
                                        // Check if you are pulling the bottom left square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(10, ratio, app.currentID);
                                            app.moveObjectX(-10, app.currentID, true);
                                        }

                                        // Check if you are pulling the top left square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(-10, -ratio, app.currentID);
                                            app.moveObjectX(10, app.currentID, true);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_RIGHT)
                                    {
                                        // Check if you are pulling the bottom right square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(10, ratio, app.currentID);
                                        }

                                        // Check if you are pulling the bottom right square to the upper left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(-10, -ratio, app.currentID);
                                        }
                                    }
                                }
                                else // The object is higher than it is wide
                                {
                                    float ratio = 10.f / app.aspectRatios[app.currentID-2];

                                    // Check which corner is being dragged
                                    if (app.draggingSquare == SQUARE_TOP_LEFT)
                                    {
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(ratio, 10, app.currentID);
                                            app.moveObjectX(-ratio, app.currentID, true);
                                            app.moveObjectY(-10, app.currentID, true);
                                        }

                                        // Check if you are pulling the top left square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(-ratio, -10, app.currentID);
                                            app.moveObjectX(ratio, app.currentID, true);
                                            app.moveObjectY(10, app.currentID, true);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_TOP_RIGHT)
                                    {
                                        // Check if you are pulling the top right square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(ratio, 10, app.currentID);
                                            app.moveObjectY(-10, app.currentID, true);
                                        }

                                        // Check if you are pulling the top right square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(-ratio, -10, app.currentID);
                                            app.moveObjectY(10, app.currentID, true);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_LEFT)
                                    {
                                        // Check if you are pulling the bottom left square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(ratio, 10, app.currentID);
                                            app.moveObjectX(-ratio, app.currentID, true);
                                        }

                                        // Check if you are pulling the top left square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(-ratio, -10, app.currentID);
                                            app.moveObjectX(ratio, app.currentID, true);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_RIGHT)
                                    {
                                        // Check if you are pulling the bottom right square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(ratio, 10, app.currentID);
                                        }

                                        // Check if you are pulling the bottom right square to the upper left side
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(-ratio, -10, app.currentID);
                                        }
                                    }
                                }
                            }
                        }
                        else // You are not dragging a scale square
                        {
                            // Make the scale squares react on mouse hover
                            app.getScaleSquareObjectID(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        app.draggingObject = false;
                        app.draggingSquare = 0;

                        // If the mouse is not on the scale square then don't let it be filled
                        app.getScaleSquareObjectID(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    }
                }
            }
        }

        // Check the events of the objects window
        while (app.objectsWindow.pollEvent(event))
        {
            // Pass the event to tgui
            app.objectsWindow.handleEvent(event);
        }

        // Check the events of the property window
        while (app.propertyWindow.pollEvent(event))
        {
            // Check if the window was resized
            if (event.type == sf::Event::Resized)
            {
                // Only react when the window size really changed
                if ((event.size.width != app.windows[0].width.value) || (event.size.height != app.windows[0].height.value))
                {
                    // Check if the window really changed its size
                    // Make sure that the size isn't too small and adjust the view
                    if (event.size.width < 200)
                    {
                        app.propertyWindow.setSize(sf::Vector2u(200, event.size.height));
                        app.propertyWindow.setView(sf::View(sf::FloatRect(0, 0, 200, static_cast<float>(event.size.height))));
                    }
                    else
                        app.propertyWindow.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

                    // Update the properties
                    app.changeVisibleProperties();
                }
            }

            // Pass the change to tgui
            app.propertyWindow.handleEvent(event);
        }

        tgui::Callback callback;

        // Get the callback from the objects window
        while (app.objectsWindow.getCallback(callback))
        {
            // Check if the load of save button was pressed
            if (callback.callbackID == 20)
                app.loadForm();
            else if (callback.callbackID == 21)
                app.saveForm();
            else // Add an object to the form
                app.newObject(callback.callbackID);
        }

        // Get the callback from the property window
        while (app.propertyWindow.getCallback(callback))
        {
            // Check if you clicked the delete button
            if (callback.callbackID == 50)
                app.deleteObject();
            else // A property was changed
            {
                // Update the property
                app.updateProperty(callback.callbackID - 1);

                // Just in case the width of height would have changed, store the aspect ratio
                app.storeObjectsNewAspectRatio();
            }
        }

        // Get the elapsed time
        int elapsedTime = resizeDelayClock.getElapsedTime().asMilliseconds();
        resizeDelayClock.restart();

        // Check if there are resize delays
        for (unsigned int i=0; i<app.resizeObjectDelays.size(); ++i)
        {
            app.resizeObjectDelays[i].m_Delay -= elapsedTime;

            // Check if the delay reached 0
            if (app.resizeObjectDelays[i].m_Delay <= 0)
            {
                // Execute the resize
                app.resizeObject(app.resizeObjectDelays[i].m_AddToWidth, app.resizeObjectDelays[i].m_AddToHeight, app.resizeObjectDelays[i].m_Id, 0);

                // If needed then also change the position of the object
                if (app.resizeObjectDelays[i].m_AddToPositionX != 0)
                    app.moveObjectX(app.resizeObjectDelays[i].m_AddToPositionX, app.resizeObjectDelays[i].m_Id);

                if (app.resizeObjectDelays[i].m_AddToPositionY != 0)
                    app.moveObjectY(app.resizeObjectDelays[i].m_AddToPositionY, app.resizeObjectDelays[i].m_Id);

                // Remove the object from the list
                app.resizeObjectDelays.erase(app.resizeObjectDelays.begin() + i);
                --i;
            }
        }

        // Clear the windows
        app.mainWindow.clear(sf::Color(230, 230, 230));
        app.objectsWindow.clear(sf::Color(230, 230, 230));
        app.propertyWindow.clear(sf::Color(230, 230, 230));

        // Draw the windows
        app.mainWindow.drawGUI();
        app.objectsWindow.drawGUI();
        app.propertyWindow.drawGUI();

        // Display the windows
        app.mainWindow.display();
        app.objectsWindow.display();
        app.propertyWindow.display();

        // Sleep for a moment
        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
