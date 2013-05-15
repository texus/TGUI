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

    Builder app;

    sf::Font font;
    if (font.loadFromFile("DejaVuSans.ttf") == false)
    {
        std::cerr << "Failed to load DejaVuSans.ttf" << std::endl;
        return 1;
    }
    app.mainWindow.setGlobalFont(font);
    app.propertyWindow.setGlobalFont(font);
    app.objectsWindow.setGlobalFont(font);

    // Initialize the objects window
    if (!app.objectsWindow.loadObjectsFromFile("FormObjectsWindow.txt"))
    {
        std::cerr << "Failed to load FormObjectsWindow.txt" << std::endl;
        return 1;
    }

    // The objects loaded from the file should send callback
    std::vector<tgui::Object::Ptr> objects = app.objectsWindow.getObjects();
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i]->bindCallback(tgui::ClickableObject::LeftMouseClicked);
    }

    // Select the window by default
    app.newObject(tgui::Type_Unknown);

    // Quit when the window is closed
    while (app.mainRenderWindow.isOpen())
    {
        sf::Event event;

        // Check the events of the main window
        while (app.mainRenderWindow.pollEvent(event))
        {
            // Check if the window has to be closed
            if (event.type == sf::Event::Closed)
            {
                app.objectsRenderWindow.close();
                app.propertyRenderWindow.close();
                app.mainRenderWindow.close();
            }

            // Check if the window was resized
            if (event.type == sf::Event::Resized)
            {
                // You must use the properties to change the size of the window
                if ((event.size.width == (TGUI_MAXIMUM(app.windows[0].width.value, 32))) && (event.size.height == (TGUI_MAXIMUM(app.windows[0].height.value, 32))))
                {
                    // Set the viewport to the new size
                    app.mainRenderWindow.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

                    // Change the size of the transparent image
                    static_cast<tgui::Picture::Ptr>(app.mainWindow.get("1"))->setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));

                    // Pass the change to tgui
                    app.mainWindow.handleEvent(event);
                }
                else // You tried to scale the window
                    app.mainRenderWindow.setSize(sf::Vector2u(TGUI_MAXIMUM(app.windows[0].width.value, 32), TGUI_MAXIMUM(app.windows[0].height.value, 32)));
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
                            app.changeVisibleProperties();
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
                                    app.resizeObject(10, 0);
                                }

                                // Check if you are pulling the right square to the left
                                while (event.mouseMove.x < app.dragPos.x - 10)
                                {
                                    app.dragPos.x -= 10;
                                    app.resizeObject(-10, 0);
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

                                // Store the new aspect ratio of the object
                                app.storeObjectsNewAspectRatio();
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

                                // Store the new aspect ratio of the object
                                app.storeObjectsNewAspectRatio();
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
                                            app.resizeObject(10, ratio);
                                            app.moveObjectX(-10);
                                            app.moveObjectY(-ratio);
                                        }

                                        // Check if you are pulling the top left square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(-10, -ratio);
                                            app.moveObjectX(10);
                                            app.moveObjectY(ratio);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_TOP_RIGHT)
                                    {
                                        // Check if you are pulling the top right square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(10, ratio);
                                            app.moveObjectY(-ratio);
                                        }

                                        // Check if you are pulling the top right square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(-10, -ratio);
                                            app.moveObjectY(ratio);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_LEFT)
                                    {
                                        // Check if you are pulling the bottom left square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(10, ratio);
                                            app.moveObjectX(-10);
                                        }

                                        // Check if you are pulling the top left square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(-10, -ratio);
                                            app.moveObjectX(10);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_RIGHT)
                                    {
                                        // Check if you are pulling the bottom right square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + 10) && (event.mouseMove.y > app.dragPos.y + ratio))
                                        {
                                            app.dragPos.x += 10;
                                            app.dragPos.y += ratio;
                                            app.resizeObject(10, ratio);
                                        }

                                        // Check if you are pulling the bottom right square to the upper left side
                                        while ((event.mouseMove.x < app.dragPos.x - 10) && (event.mouseMove.y < app.dragPos.y - ratio))
                                        {
                                            app.dragPos.x -= 10;
                                            app.dragPos.y -= ratio;
                                            app.resizeObject(-10, -ratio);
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
                                            app.resizeObject(ratio, 10);
                                            app.moveObjectX(-ratio);
                                            app.moveObjectY(-10);
                                        }

                                        // Check if you are pulling the top left square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(-ratio, -10);
                                            app.moveObjectX(ratio);
                                            app.moveObjectY(10);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_TOP_RIGHT)
                                    {
                                        // Check if you are pulling the top right square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(ratio, 10);
                                            app.moveObjectY(-10);
                                        }

                                        // Check if you are pulling the top right square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(-ratio, -10);
                                            app.moveObjectY(10);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_LEFT)
                                    {
                                        // Check if you are pulling the bottom left square to the bottom left side
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(ratio, 10);
                                            app.moveObjectX(-ratio);
                                        }

                                        // Check if you are pulling the top left square to the upper right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(-ratio, -10);
                                            app.moveObjectX(ratio);
                                        }
                                    }
                                    else if (app.draggingSquare == SQUARE_BOTTOM_RIGHT)
                                    {
                                        // Check if you are pulling the bottom right square to the bottom right side
                                        while ((event.mouseMove.x > app.dragPos.x + ratio) && (event.mouseMove.y > app.dragPos.y + 10))
                                        {
                                            app.dragPos.x += ratio;
                                            app.dragPos.y += 10;
                                            app.resizeObject(ratio, 10);
                                        }

                                        // Check if you are pulling the bottom right square to the upper left side
                                        while ((event.mouseMove.x < app.dragPos.x - ratio) && (event.mouseMove.y < app.dragPos.y - 10))
                                        {
                                            app.dragPos.x -= ratio;
                                            app.dragPos.y -= 10;
                                            app.resizeObject(-ratio, -10);
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
        while (app.objectsRenderWindow.pollEvent(event))
        {
            // Pass the event to tgui
            app.objectsWindow.handleEvent(event);
        }

        // Check the events of the property window
        while (app.propertyRenderWindow.pollEvent(event))
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
                        app.propertyRenderWindow.setSize(sf::Vector2u(200, event.size.height));
                        app.propertyRenderWindow.setView(sf::View(sf::FloatRect(0, 0, 200, static_cast<float>(event.size.height))));
                    }
                    else
                        app.propertyRenderWindow.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));

                    // Update the properties
                    app.changeVisibleProperties();
                }
            }

            // Pass the change to tgui
            app.propertyWindow.handleEvent(event);
        }

        tgui::Callback callback;

        // Get the callback from the objects window
        while (app.objectsWindow.pollCallback(callback))
        {
            // Check if the load of save button was pressed
            if (callback.id == 20)
                app.loadForm();
            else if (callback.id == 21)
                app.saveForm();
            else // Add an object to the form
                app.newObject(callback.id);
        }

        // Get the callback from the property window
        while (app.propertyWindow.pollCallback(callback))
        {
            // Check if you clicked the delete button
            if (callback.id == 50)
                app.deleteObject();
            else // A property was changed
            {
                // Update the property
                app.updateProperty(callback.id - 1);

                // Just in case the width of height would have changed, store the aspect ratio
                app.storeObjectsNewAspectRatio();
            }
        }

        app.mainRenderWindow.clear(sf::Color(230, 230, 230));
        app.mainWindow.draw();
        app.mainRenderWindow.display();

        app.objectsRenderWindow.clear(sf::Color(230, 230, 230));
        app.objectsWindow.draw();
        app.objectsRenderWindow.display();

        app.propertyRenderWindow.clear(sf::Color(230, 230, 230));
        app.propertyWindow.draw();
        app.propertyRenderWindow.display();

        // Sleep for a moment
        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

