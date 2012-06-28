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


#include "FormBuilder.hpp"

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

    sf::Clock clock;

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
                // Set the viewport to the new size
                app.mainWindow.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));

                // Change the size of the transparent image
                app.mainWindow.getPicture("1")->setSize(event.size.width, event.size.height);

                // Pass the change to tgui
                app.mainWindow.handleEvent(event);

                // Store the new size
                app.windows[0].width.value = event.size.width;
                app.windows[0].height.value = event.size.height;

                // Check if the window is selected
                if (app.currentID == app.windows[0].id)
                {
                    // Refresh the properties
                    app.propertyWindow.removeAllObjects();
                    app.changeVisibleProperties();
                }
            }
            else
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    // Check if you start dragging one of the scale squares
                    unsigned int id = app.getScaleSquareObjectID(event.mouseButton.x, event.mouseButton.y);
                    if (id > 0)
                    {
                        app.draggingSquare = id;
                        app.dragPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
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
                            app.changeVisibleProperties();
                            app.resizePropertyWindow();
                        }

                        // Check if you clicked on an object
                        if (id != 1)
                        {
                            app.draggingObject = true;
                            app.dragPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
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
                            app.moveObjectX(-10);
                            app.dragPos.x -=10;
                        }

                        // The mouse should have moved at least 10 pixels right
                        while (event.mouseMove.x > app.dragPos.x + 10)
                        {
                            app.moveObjectX(10);
                            app.dragPos.x +=10;
                        }

                        // The mouse should have moved at least 10 pixels up
                        while (event.mouseMove.y < app.dragPos.y - 10)
                        {
                            app.moveObjectY(-10);
                            app.dragPos.y -=10;
                        }

                        // The mouse should have moved at least 10 pixels down
                        while (event.mouseMove.y > app.dragPos.y + 10)
                        {
                            app.moveObjectY(10);
                            app.dragPos.y +=10;
                        }
                    }
                    else // You are not dragging an object
                    {
                        // Make the scale squares react on mouse hover
                        app.getScaleSquareObjectID(event.mouseMove.x, event.mouseMove.y);

                        // Check if you were dragging a scale square
                        if (app.draggingSquare > 0)
                        {
                            // Check which square you are dragging
                            if (app.draggingSquare == SQUARE_RIGHT)
                            {
                                // Check if you are pulling the right square to the right
                                while (event.mouseMove.x > app.dragPos.x + 10)
                                {
                                    app.dragPos.x += 10;
                                    app.resizeObject(10, 0);
                                }

                                // Check if you are pulling the right square to the left
                                while (event.mouseMove.x < app.dragPos.x - 10)
                                {
                                    app.dragPos.x -= 10;
                                    app.resizeObject(-10, 0);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_LEFT)
                            {
                                // Check if you are pulling the left square to the right
                                while (event.mouseMove.x > app.dragPos.x + 10)
                                {
                                    app.dragPos.x += 10;
                                    app.resizeObject(-10, 0);
                                    app.moveObjectX(10);
                                }

                                // Check if you are pulling the left square to the left
                                while (event.mouseMove.x < app.dragPos.x - 10)
                                {
                                    app.dragPos.x -= 10;
                                    app.resizeObject(10, 0);
                                    app.moveObjectX(-10);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_BOTTOM)
                            {
                                // Check if you are pulling the bottom square down
                                while (event.mouseMove.y > app.dragPos.y + 10)
                                {
                                    app.dragPos.y += 10;
                                    app.resizeObject(0, 10);
                                }

                                // Check if you are pulling the bottom square up
                                while (event.mouseMove.y < app.dragPos.y - 10)
                                {
                                    app.dragPos.y -= 10;
                                    app.resizeObject(0, -10);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_TOP)
                            {
                                // Check if you are pulling the top square down
                                while (event.mouseMove.y > app.dragPos.y + 10)
                                {
                                    app.dragPos.y += 10;
                                    app.resizeObject(0, -10);
                                    app.moveObjectY(10);
                                }

                                // Check if you are pulling the top square up
                                while (event.mouseMove.y < app.dragPos.y - 10)
                                {
                                    app.dragPos.y -= 10;
                                    app.resizeObject(0, 10);
                                    app.moveObjectY(-10);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_TOP_LEFT)
                            {
                                // Check if you are pulling the top left square to the upper left side
                                while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y < app.dragPos.y - 10))
                                {
                                    app.dragPos.x -= 10;
                                    app.dragPos.y -= 10;
                                    app.resizeObject(10, 10);
                                    app.moveObjectX(-10);
                                    app.moveObjectY(-10);
                                }

                                // Check if you are pulling the top left square to the bottom right side
                                while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y > app.dragPos.y + 10))
                                {
                                    app.dragPos.x += 10;
                                    app.dragPos.y += 10;
                                    app.resizeObject(-10, -10);
                                    app.moveObjectX(10);
                                    app.moveObjectY(10);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_TOP_RIGHT)
                            {
                                // Check if you are pulling the top right square to the upper right side
                                while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y < app.dragPos.y - 10))
                                {
                                    app.dragPos.x += 10;
                                    app.dragPos.y -= 10;
                                    app.resizeObject(10, 10);
                                    app.moveObjectY(-10);
                                }

                                // Check if you are pulling the top right square to the bottom left side
                                while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y > app.dragPos.y + 10))
                                {
                                    app.dragPos.x -= 10;
                                    app.dragPos.y += 10;
                                    app.resizeObject(-10, -10);
                                    app.moveObjectY(10);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_BOTTOM_LEFT)
                            {
                                // Check if you are pulling the bottom left square to the bottom left side
                                while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y > app.dragPos.y + 10))
                                {
                                    app.dragPos.x -= 10;
                                    app.dragPos.y += 10;
                                    app.resizeObject(10, 10);
                                    app.moveObjectX(-10);
                                }

                                // Check if you are pulling the top left square to the upper right side
                                while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y < app.dragPos.y - 10))
                                {
                                    app.dragPos.x += 10;
                                    app.dragPos.y -= 10;
                                    app.resizeObject(-10, -10);
                                    app.moveObjectX(10);
                                }
                            }
                            else if (app.draggingSquare == SQUARE_BOTTOM_RIGHT)
                            {
                                // Check if you are pulling the bottom right square to the bottom right side
                                while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y > app.dragPos.y + 10))
                                {
                                    app.dragPos.x += 10;
                                    app.dragPos.y += 10;
                                    app.resizeObject(10, 10);
                                }

                                // Check if you are pulling the bottom right square to the upper left side
                                while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y < app.dragPos.y - 10))
                                {
                                    app.dragPos.x -= 10;
                                    app.dragPos.y -= 10;
                                    app.resizeObject(-10, -10);
                                }
                            }
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        app.draggingObject = false;
                        app.draggingSquare = 0;
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
                // Make sure that the size isn't too small and adjust the view
                if (event.size.width < 200)
                {
                    app.propertyWindow.setSize(sf::Vector2u(200, event.size.height));
                    app.propertyWindow.setView(sf::View(sf::FloatRect(0, 0, 200, event.size.height)));
                }
                else
                    app.propertyWindow.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));

                // Update the properties
                app.propertyWindow.removeAllObjects();
                app.changeVisibleProperties();
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
            if (callback.callbackID == 20)
                app.deleteObject();
            else // A property was changed
                app.updateProperty(callback.callbackID - 1);
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
