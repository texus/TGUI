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

Builder::Builder() :
mainWindow    (sf::VideoMode(1024, 768), "TGUI Form Builder " VERSION),
objectsWindow (sf::VideoMode(112, 400), "", sf::Style::Titlebar),
propertyWindow(sf::VideoMode(300, 500), "", sf::Style::Titlebar | sf::Style::Resize),
currentID     (0),
draggingObject(false),
draggingSquare(0),
dragPos       (0, 0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::getUniqueID()
{
    static unsigned int id = 0;
    return ++id;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::newObject(unsigned int objectID, const std::string objectName)
{
    currentID = getUniqueID();

    // Check which object was created
    if (objectID == tgui::window)
    {
        // Create a new property list
        PropertiesWindow properties;
        properties.id = currentID;
        properties.name.value = objectName;
        properties.builder = this;
        windows.push_back(properties);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        windows.back().addProperties(propertyWindow);

        // Create the scale squares pictures
        mainWindow.addButton("Square_TopLeft")->load("images/Square");
        mainWindow.copyButton("Square_TopLeft", "Square_Top");
        mainWindow.copyButton("Square_TopLeft", "Square_TopRight");
        mainWindow.copyButton("Square_TopLeft", "Square_Right");
        mainWindow.copyButton("Square_TopLeft", "Square_BottomRight");
        mainWindow.copyButton("Square_TopLeft", "Square_Bottom");
        mainWindow.copyButton("Square_TopLeft", "Square_BottomLeft");
        mainWindow.copyButton("Square_TopLeft", "Square_Left");
    }
    else if (objectID == tgui::picture)
    {
        // Create a new property list
        PropertiesPicture properties;
        properties.id = currentID;
        properties.name.value = objectName;
        pictures.push_back(properties);

        // Add a picture to the form
        tgui::Picture* picture = mainWindow.addPicture(tgui::to_string(currentID));
        picture->load(pictures.back().filename.value);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        pictures.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::button)
    {
        // Create a new property list
        PropertiesButton properties;
        properties.id = currentID;
        properties.name.value = objectName;
        buttons.push_back(properties);

        // Add a button to the form
        tgui::Button* button = mainWindow.addButton(tgui::to_string(currentID));
        button->load(buttons.back().pathname.value);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        buttons.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::checkbox)
    {
        // Create a new property list
        PropertiesCheckbox properties;
        properties.id = currentID;
        properties.name.value = objectName;
        checkboxes.push_back(properties);

        // Add a checkbox to the form
        tgui::Checkbox* checkbox = mainWindow.addCheckbox(tgui::to_string(currentID));
        checkbox->load(checkboxes.back().pathname.value);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        checkboxes.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::radioButton)
    {
        // Create a new property list
        PropertiesRadioButton properties;
        properties.id = currentID;
        properties.name.value = objectName;
        properties.builder = this;
        radioButtons.push_back(properties);

        // Add a radio button to the form
        tgui::RadioButton* radioButton = mainWindow.addRadioButton(tgui::to_string(currentID));
        radioButton->load(radioButtons.back().pathname.value);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        radioButtons.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::label)
    {
        // Create a new property list
        PropertiesLabel properties;
        properties.id = currentID;
        properties.name.value = objectName;
        labels.push_back(properties);

        // Add a label to the form
        tgui::Label* label = mainWindow.addLabel(tgui::to_string(currentID));
        label->setText("Label");

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        labels.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::editBox)
    {
        // Create a new property list
        PropertiesEditBox properties;
        properties.id = currentID;
        properties.name.value = objectName;
        editBoxes.push_back(properties);

        // Add an edit box to the form
        tgui::EditBox* editBox = mainWindow.addEditBox(tgui::to_string(currentID));
        editBox->load(editBoxes.back().pathname.value);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        editBoxes.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::listbox)
    {
        // Create a new property list
        PropertiesListbox properties;
        properties.id = currentID;
        properties.name.value = objectName;
        listboxes.push_back(properties);

        // Add an listbox to the form
        tgui::Listbox* listbox = mainWindow.addListbox(tgui::to_string(currentID));
        listbox->load(200, 240, "images/objects/Scrollbar/Black", 30);
        listbox->setBorders(2, 2, 2, 2);
        listbox->changeColors(sf::Color( 50,  50,  50),
                              sf::Color(200, 200, 200),
                              sf::Color( 10, 110, 255),
                              sf::Color(255, 255, 255),
                              sf::Color::Black);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        listboxes.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::comboBox)
    {
        // Create a new property list
        PropertiesComboBox properties;
        properties.id = currentID;
        properties.name.value = objectName;
        comboBoxes.push_back(properties);

        // Add an combo box to the form
        tgui::ComboBox* comboBox = mainWindow.addComboBox(tgui::to_string(currentID));
        comboBox->load("images/objects/ComboBox/Black", 240, 10, "images/objects/Scrollbar/Black");
        comboBox->setBorders(2, 2, 2, 2);
        comboBox->changeColors(sf::Color( 50,  50,  50),
                               sf::Color(200, 200, 200),
                               sf::Color( 10, 110, 255),
                               sf::Color(255, 255, 255),
                               sf::Color::Black);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        comboBoxes.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::slider)
    {
        // Create a new property list
        PropertiesSlider properties;
        properties.id = currentID;
        properties.name.value = objectName;
        sliders.push_back(properties);

        // Add a slider to the form
        tgui::Slider* slider = mainWindow.addSlider(tgui::to_string(currentID));
        slider->load(sliders.back().pathname.value);
        slider->verticalScroll = false;

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        sliders.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::scrollbar)
    {
        // Create a new property list
        PropertiesScrollbar properties;
        properties.id = currentID;
        properties.name.value = objectName;
        scrollbars.push_back(properties);

        // Add a scrollbar to the form
        tgui::Scrollbar* scrollbar = mainWindow.addScrollbar(tgui::to_string(currentID));
        scrollbar->load(scrollbars.back().pathname.value);
        scrollbar->setMaximum(5);
        scrollbar->setLowValue(4);
        scrollbar->autoHide = false;

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        scrollbars.back().addProperties(propertyWindow);
    }
    else if (objectID == tgui::loadingBar)
    {
        // Create a new property list
        PropertiesLoadingBar properties;
        properties.id = currentID;
        properties.name.value = objectName;
        loadingBars.push_back(properties);

        // Add a loading bar to the form
        tgui::LoadingBar* loadingBar = mainWindow.addLoadingBar(tgui::to_string(currentID));
        loadingBar->load(loadingBars.back().pathname.value);

        // Show the properties of the window
        propertyWindow.removeAllObjects();
        loadingBars.back().addProperties(propertyWindow);
    }

    // Bring the scale squares to front
    mainWindow.getButton("Square_TopLeft")->moveToFront();
    mainWindow.getButton("Square_Top")->moveToFront();
    mainWindow.getButton("Square_TopRight")->moveToFront();
    mainWindow.getButton("Square_Right")->moveToFront();
    mainWindow.getButton("Square_BottomRight")->moveToFront();
    mainWindow.getButton("Square_Bottom")->moveToFront();
    mainWindow.getButton("Square_BottomLeft")->moveToFront();
    mainWindow.getButton("Square_Left")->moveToFront();

    resizePropertyWindow();
    changeVisibleProperties();

    return currentID;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::changeVisibleProperties()
{
    unsigned int i;

    // Remove all the objects from the window
    propertyWindow.removeAllObjects();

    // Check if the window is selected
    for (i=0; i<windows.size(); ++i)
    {
        if (windows[i].id == currentID)
        {
            windows[i].addProperties(propertyWindow);

            mainWindow.getButton("Square_TopLeft")->setPosition(-3, -3);
            mainWindow.getButton("Square_Top")->setPosition((windows[i].width.value / 2.f) - 3, -3);
            mainWindow.getButton("Square_TopRight")->setPosition(windows[i].width.value - 3, -3);
            mainWindow.getButton("Square_Right")->setPosition(windows[i].width.value - 3, (windows[i].height.value / 2.f) - 3);
            mainWindow.getButton("Square_BottomRight")->setPosition(windows[i].width.value - 3, windows[i].height.value - 3);
            mainWindow.getButton("Square_Bottom")->setPosition((windows[i].width.value / 2.f) - 3, windows[i].height.value - 3);
            mainWindow.getButton("Square_BottomLeft")->setPosition(-3, windows[i].height.value - 3);
            mainWindow.getButton("Square_Left")->setPosition(-3, (windows[i].height.value / 2.f) - 3);
            return;
        }
    }

    // Create the delete button
    tgui::Button* button = propertyWindow.addButton();
    button->load("images/objects/Button/Black");
    button->setText("Delete object");
    button->setSize(propertyWindow.sf::RenderWindow::getSize().x - 8, 40);
    button->callbackID = 20;

    // Check if a radio button is selected
    for (i=0; i<radioButtons.size(); ++i)
    {
        if (radioButtons[i].id == currentID)
        {
            radioButtons[i].addProperties(propertyWindow);
            button->setPosition(4, 10 + (40 * (Property_Checkbox_CallbackID + 1)));

            mainWindow.getButton("Square_TopLeft")->setPosition(radioButtons[i].left.value - 3, radioButtons[i].top.value - 3);
            mainWindow.getButton("Square_Top")->setPosition(radioButtons[i].left.value + (radioButtons[i].width.value / 2.f) - 3, radioButtons[i].top.value - 3);
            mainWindow.getButton("Square_TopRight")->setPosition(radioButtons[i].left.value + radioButtons[i].width.value - 3, radioButtons[i].top.value - 3);
            mainWindow.getButton("Square_Right")->setPosition(radioButtons[i].left.value + radioButtons[i].width.value - 3, radioButtons[i].top.value + (radioButtons[i].height.value / 2.f) - 3);
            mainWindow.getButton("Square_BottomRight")->setPosition(radioButtons[i].left.value + radioButtons[i].width.value - 3, radioButtons[i].top.value + radioButtons[i].height.value - 3);
            mainWindow.getButton("Square_Bottom")->setPosition(radioButtons[i].left.value + (radioButtons[i].width.value / 2.f) - 3, radioButtons[i].top.value + radioButtons[i].height.value - 3);
            mainWindow.getButton("Square_BottomLeft")->setPosition(radioButtons[i].left.value - 3, radioButtons[i].top.value + radioButtons[i].height.value - 3);
            mainWindow.getButton("Square_Left")->setPosition(radioButtons[i].left.value - 3, radioButtons[i].top.value + (radioButtons[i].height.value / 2.f) - 3);
            return;
        }
    }

    // Check if the other objects are selected
    #define FindObjectWithID(Object, objects) \
    for (i=0; i<objects.size(); ++i) \
    { \
        if (objects[i].id == currentID) \
        { \
            objects[i].addProperties(propertyWindow); \
            button->setPosition(4, 10 + (40 * (Property_##Object##_CallbackID + 1))); \
          \
            mainWindow.getButton("Square_TopLeft")->setPosition(objects[i].left.value - 3, objects[i].top.value - 3); \
            mainWindow.getButton("Square_Top")->setPosition(objects[i].left.value + (objects[i].width.value / 2.f) - 3, objects[i].top.value - 3); \
            mainWindow.getButton("Square_TopRight")->setPosition(objects[i].left.value + objects[i].width.value - 3, objects[i].top.value - 3); \
            mainWindow.getButton("Square_Right")->setPosition(objects[i].left.value + objects[i].width.value - 3, objects[i].top.value + (objects[i].height.value / 2.f) - 3); \
            mainWindow.getButton("Square_BottomRight")->setPosition(objects[i].left.value + objects[i].width.value - 3, objects[i].top.value + objects[i].height.value - 3); \
            mainWindow.getButton("Square_Bottom")->setPosition(objects[i].left.value + (objects[i].width.value / 2.f) - 3, objects[i].top.value + objects[i].height.value - 3); \
            mainWindow.getButton("Square_BottomLeft")->setPosition(objects[i].left.value - 3, objects[i].top.value + objects[i].height.value - 3); \
            mainWindow.getButton("Square_Left")->setPosition(objects[i].left.value - 3, objects[i].top.value + (objects[i].height.value / 2.f) - 3); \
            return; \
        } \
    }

    FindObjectWithID(Picture, pictures)
    FindObjectWithID(Button, buttons)
    FindObjectWithID(Checkbox, checkboxes)
    FindObjectWithID(Label, labels)
    FindObjectWithID(EditBox, editBoxes)
    FindObjectWithID(Listbox, listboxes)
    FindObjectWithID(ComboBox, comboBoxes)
    FindObjectWithID(Slider, sliders)
    FindObjectWithID(Scrollbar, scrollbars)
    FindObjectWithID(LoadingBar, loadingBars)

    #undef FindObjectWithID
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::resizePropertyWindow()
{
    unsigned int i;

    for (i=0; i<windows.size(); ++i)
    {
        if (windows[i].id == currentID)
        {
            propertyWindow.sf::Window::setSize(sf::Vector2u(propertyWindow.sf::RenderWindow::getSize().x, 10 + (40 * (Property_Window_GlobalFont + 1))));
            return;
        }
    }

    for (i=0; i<radioButtons.size(); ++i)
    {
        if (radioButtons[i].id == currentID)
        {
            propertyWindow.sf::Window::setSize(sf::Vector2u(propertyWindow.sf::RenderWindow::getSize().x, 60 + (40 * (Property_Checkbox_CallbackID + 1))));
            return;
        }
    }

    #define FindObjectWithID(Object, objects) \
    for (i=0; i<objects.size(); ++i) \
    { \
        if (objects[i].id == currentID) \
        { \
            propertyWindow.sf::Window::setSize(sf::Vector2u(propertyWindow.sf::RenderWindow::getSize().x, 60 + (40 * (Property_##Object##_CallbackID + 1)))); \
            return; \
        } \
    }

    FindObjectWithID(Picture, pictures)
    FindObjectWithID(Button, buttons)
    FindObjectWithID(Checkbox, checkboxes)
    FindObjectWithID(Label, labels)
    FindObjectWithID(EditBox, editBoxes)
    FindObjectWithID(Listbox, listboxes)
    FindObjectWithID(ComboBox, comboBoxes)
    FindObjectWithID(Slider, sliders)
    FindObjectWithID(Scrollbar, scrollbars)
    FindObjectWithID(LoadingBar, loadingBars)

    #undef FindObjectWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::updateProperty(unsigned int propertyNumber)
{
    unsigned int i;


    for (i=0; i<windows.size(); ++i)
    {

        if (windows[i].id == currentID)
        {
            windows[i].updateProperty(mainWindow, propertyWindow, propertyNumber); \
            mainWindow.getButton("Square_TopLeft")->setPosition(-3, -3);
            mainWindow.getButton("Square_Top")->setPosition((windows[i].width.value / 2.f) - 3, -3);
            mainWindow.getButton("Square_TopRight")->setPosition(windows[i].width.value - 3, -3);
            mainWindow.getButton("Square_Right")->setPosition(windows[i].width.value - 3, (windows[i].height.value / 2.f) - 3);
            mainWindow.getButton("Square_BottomRight")->setPosition(windows[i].width.value - 3, windows[i].height.value - 3);
            mainWindow.getButton("Square_Bottom")->setPosition((windows[i].width.value / 2.f) - 3, windows[i].height.value - 3);
            mainWindow.getButton("Square_BottomLeft")->setPosition(-3, windows[i].height.value - 3);
            mainWindow.getButton("Square_Left")->setPosition(-3, (windows[i].height.value / 2.f) - 3);
            return;
        }
    }

    #define FindObjectWithID(objects) \
    for (i=0; i<objects.size(); ++i) \
    { \
     \
        if (objects[i].id == currentID) \
        { \
            objects[i].updateProperty(mainWindow, propertyWindow, propertyNumber); \
            mainWindow.getButton("Square_TopLeft")->setPosition(objects[i].left.value - 3, objects[i].top.value - 3); \
            mainWindow.getButton("Square_Top")->setPosition(objects[i].left.value + (objects[i].width.value / 2.f) - 3, objects[i].top.value - 3); \
            mainWindow.getButton("Square_TopRight")->setPosition(objects[i].left.value + objects[i].width.value - 3, objects[i].top.value - 3); \
            mainWindow.getButton("Square_Right")->setPosition(objects[i].left.value + objects[i].width.value - 3, objects[i].top.value + (objects[i].height.value / 2.f) - 3); \
            mainWindow.getButton("Square_BottomRight")->setPosition(objects[i].left.value + objects[i].width.value - 3, objects[i].top.value + objects[i].height.value - 3); \
            mainWindow.getButton("Square_Bottom")->setPosition(objects[i].left.value + (objects[i].width.value / 2.f) - 3, objects[i].top.value + objects[i].height.value - 3); \
            mainWindow.getButton("Square_BottomLeft")->setPosition(objects[i].left.value - 3, objects[i].top.value + objects[i].height.value - 3); \
            mainWindow.getButton("Square_Left")->setPosition(objects[i].left.value - 3, objects[i].top.value + (objects[i].height.value / 2.f) - 3); \
            return; \
        } \
    }

    FindObjectWithID(pictures)
    FindObjectWithID(buttons)
    FindObjectWithID(checkboxes)
    FindObjectWithID(radioButtons)
    FindObjectWithID(labels)
    FindObjectWithID(editBoxes)
    FindObjectWithID(listboxes)
    FindObjectWithID(comboBoxes)
    FindObjectWithID(sliders)
    FindObjectWithID(scrollbars)
    FindObjectWithID(loadingBars)

    #undef FindObjectWithID
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::setGlobalFont(const sf::Font& font)
{
    TGUI_UNUSED_PARAM(font);
/*
    unsigned int i;

    // Loop through all buttons
    for (i=0; i<buttons.size(); ++i)
    {
        // Check if the global font is used
        if (buttons[i].textFont.value.compare("Global") == 0)
            buttons[i].updateProperty(mainWindow, propertyWindow, Property_Button_TextFont);
    }

    // Loop through all checkboxes
    for (i=0; i<checkboxes.size(); ++i)
    {
        // Check if the global font is used
        if (checkboxes[i].textFont.value.compare("Global") == 0)
            checkboxes[i].updateProperty(mainWindow, propertyWindow, Property_Checkbox_TextFont);
    }

    // Loop through all radio buttons
    for (i=0; i<radioButtons.size(); ++i)
    {
        // Check if the global font is used
        if (radioButtons[i].textFont.value.compare("Global") == 0)
            radioButtons[i].updateProperty(mainWindow, propertyWindow, Property_Checkbox_TextFont);
    }

    // Loop through all labels
    for (i=0; i<labels.size(); ++i)
    {
        // Check if the global font is used
        if (labels[i].textFont.value.compare("Global") == 0)
            labels[i].updateProperty(mainWindow, propertyWindow, Property_Label_TextFont);
    }

    // Loop through all edit boxes
    for (i=0; i<editBoxes.size(); ++i)
    {
        // Check if the global font is used
        if (editBoxes[i].textFont.value.compare("Global") == 0)
            editBoxes[i].updateProperty(mainWindow, propertyWindow, Property_EditBox_TextFont);
    }

    // Loop through all listboxes
    for (i=0; i<listboxes.size(); ++i)
    {
        // Check if the global font is used
        if (listboxes[i].textFont.value.compare("Global") == 0)
            listboxes[i].updateProperty(mainWindow, propertyWindow, Property_Listbox_TextFont);
    }

    // Loop through all combo boxes
    for (i=0; i<comboBoxes.size(); ++i)
    {
        // Check if the global font is used
        if (comboBoxes[i].textFont.value.compare("Global") == 0)
            comboBoxes[i].updateProperty(mainWindow, propertyWindow, Property_Listbox_TextFont);
    }
*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::getClickedObjectID(sf::Event& event)
{
    // Check if the left mouse was pressed
    if (event.mouseButton.button == sf::Mouse::Left)
    {
        float mouseX = event.mouseButton.x / (mainWindow.sf::RenderWindow::getSize().x / mainWindow.getView().getSize().x);
        float mouseY = event.mouseButton.y / (mainWindow.sf::RenderWindow::getSize().y / mainWindow.getView().getSize().y);

        unsigned int highestID = 0;
        unsigned int objectID = 1;

        #define FindObjectNr(Object, objects) \
        for (unsigned int i=0; i<objects.size(); ++i) \
        { \
            tgui::Object* object = mainWindow.get##Object(tgui::to_string(objects[i].id)); \
         \
            if (object->mouseOnObject(mouseX, mouseY)) \
            { \
                if (highestID < object->getObjectID()) \
                { \
                    if (highestID > 0) \
                        object->mouseNotOnObject(); \
                 \
                    highestID = object->getObjectID(); \
                    objectID = objects[i].id; \
                } \
            } \
        }

        FindObjectNr(Picture, pictures)
        FindObjectNr(Button, buttons)
        FindObjectNr(Checkbox, checkboxes)
        FindObjectNr(RadioButton, radioButtons)
        FindObjectNr(Label, labels)
        FindObjectNr(EditBox, editBoxes)
        FindObjectNr(Listbox, listboxes)
        FindObjectNr(ComboBox, comboBoxes)
        FindObjectNr(Slider, sliders)
        FindObjectNr(Scrollbar, scrollbars)
        FindObjectNr(LoadingBar, loadingBars)

        #undef FindObjectNr

        // If our mouse is on top of an object then return the id
        return objectID;
    }
    else
        return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::getScaleSquareObjectID(float x, float y)
{
    float mouseX = x / (mainWindow.sf::RenderWindow::getSize().x / mainWindow.getView().getSize().x);
    float mouseY = y / (mainWindow.sf::RenderWindow::getSize().y / mainWindow.getView().getSize().y);

    unsigned int highestID = 0;
    unsigned int objectID;

    #define FindObjectNr(name, objID) \
    { \
        tgui::Button* button = mainWindow.getButton(name); \
      \
        if (button->mouseOnObject(mouseX, mouseY)) \
        { \
            if (highestID < button->getObjectID()) \
            { \
                button->mouseMoved(mouseX, mouseY); \
                highestID = button->getObjectID(); \
                objectID = objID; \
            } \
        } \
    }

    FindObjectNr("Square_TopLeft", SQUARE_TOP_LEFT)
    FindObjectNr("Square_TopRight", SQUARE_TOP_RIGHT)
    FindObjectNr("Square_BottomLeft", SQUARE_BOTTOM_LEFT)
    FindObjectNr("Square_BottomRight", SQUARE_BOTTOM_RIGHT)
    FindObjectNr("Square_Top", SQUARE_TOP)
    FindObjectNr("Square_Left", SQUARE_LEFT)
    FindObjectNr("Square_Right", SQUARE_RIGHT)
    FindObjectNr("Square_Bottom", SQUARE_BOTTOM)

    #undef FindObjectNr

    // If our mouse is on top of an object then return true
    if (highestID != 0)
        return objectID;
    else
        return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::deleteObject()
{
    unsigned int i;

    // Remove the object from the window
    mainWindow.removeObject(tgui::to_string(currentID));

    // Find and remove the properties of the object
    #define FindObjectWithID(object) \
    for (i=0; i<object.size(); ++i) \
    { \
        if (object[i].id == currentID) \
        { \
            object.erase(object.begin() + i); \
          \
            currentID = windows[0].id; \
            changeVisibleProperties(); \
            resizePropertyWindow(); \
            return; \
        } \
    }

    FindObjectWithID(pictures)
    FindObjectWithID(buttons)
    FindObjectWithID(checkboxes)
    FindObjectWithID(radioButtons)
    FindObjectWithID(labels)
    FindObjectWithID(editBoxes)
    FindObjectWithID(listboxes)
    FindObjectWithID(comboBoxes)
    FindObjectWithID(sliders)
    FindObjectWithID(scrollbars)
    FindObjectWithID(loadingBars)

    #undef FindObjectWithID
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::moveObjectX(int pixels)
{
    // Get the pointer to the property
    tgui::EditBox* editbox = propertyWindow.getEditBox("text_Left");

    // Adjust the property
    int left = atoi(editbox->getText().c_str());
    left += pixels;

    // Change the contents of the edit box
    editbox->setText(tgui::to_string(left));

    unsigned int i;

    #define FindObjectWithID(Object, objects) \
    for (i=0; i<objects.size(); ++i) \
    { \
        if (objects[i].id == currentID) \
        { \
            updateProperty(Property_##Object##_Left); \
            return; \
        } \
    }

    FindObjectWithID(Picture, pictures)
    FindObjectWithID(Button, buttons)
    FindObjectWithID(Checkbox, checkboxes)
    FindObjectWithID(Checkbox, radioButtons)
    FindObjectWithID(Label, labels)
    FindObjectWithID(EditBox, editBoxes)
    FindObjectWithID(Listbox, listboxes)
    FindObjectWithID(ComboBox, comboBoxes)
    FindObjectWithID(Slider, sliders)
    FindObjectWithID(Scrollbar, scrollbars)
    FindObjectWithID(LoadingBar, loadingBars)

    #undef FindObjectWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::moveObjectY(int pixels)
{
    // Get the pointer to the property
    tgui::EditBox* editbox = propertyWindow.getEditBox("text_Top");

    // Adjust the property
    int top = atoi(editbox->getText().c_str());
    top += pixels;

    // Change the contents of the edit box
    editbox->setText(tgui::to_string(top));

    unsigned int i;

    #define FindObjectWithID(Object, objects) \
    for (i=0; i<objects.size(); ++i) \
    { \
        if (objects[i].id == currentID) \
        { \
            updateProperty(Property_##Object##_Top); \
            return; \
        } \
    }

    FindObjectWithID(Picture, pictures)
    FindObjectWithID(Button, buttons)
    FindObjectWithID(Checkbox, checkboxes)
    FindObjectWithID(Checkbox, radioButtons)
    FindObjectWithID(Label, labels)
    FindObjectWithID(EditBox, editBoxes)
    FindObjectWithID(Listbox, listboxes)
    FindObjectWithID(ComboBox, comboBoxes)
    FindObjectWithID(Slider, sliders)
    FindObjectWithID(Scrollbar, scrollbars)
    FindObjectWithID(LoadingBar, loadingBars)

    #undef FindObjectWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::resizeObject(unsigned int addToWidth, unsigned int addToHeight)
{
    // Get the pointer to the properties
    tgui::EditBox* editboxWidth = propertyWindow.getEditBox("text_Width");
    tgui::EditBox* editboxHeight = propertyWindow.getEditBox("text_Height");

    // Adjust the properties
    int width = atoi(editboxWidth->getText().c_str());
    int height = atoi(editboxHeight->getText().c_str());
    width += addToWidth;
    height += addToHeight;

    // Change the contents of the edit boxes
    editboxWidth->setText(tgui::to_string(width));
    editboxHeight->setText(tgui::to_string(height));

    unsigned int i;

    #define FindObjectWithID(Object, objects) \
    for (i=0; i<objects.size(); ++i) \
    { \
        if (objects[i].id == currentID) \
        { \
            updateProperty(Property_##Object##_Width); \
            updateProperty(Property_##Object##_Height); \
            return; \
        } \
    }

    FindObjectWithID(Picture, pictures)
    FindObjectWithID(Button, buttons)
    FindObjectWithID(Checkbox, checkboxes)
    FindObjectWithID(Checkbox, radioButtons)
    FindObjectWithID(Label, labels)
    FindObjectWithID(EditBox, editBoxes)
    FindObjectWithID(Listbox, listboxes)
    FindObjectWithID(ComboBox, comboBoxes)
    FindObjectWithID(Slider, sliders)
    FindObjectWithID(Scrollbar, scrollbars)
    FindObjectWithID(LoadingBar, loadingBars)

    #undef FindObjectWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Builder::loadForm()
{
    // Remove all existing objects from the form
    mainWindow.removeAllObjects();

    // Remove all properties
    pictures.clear();
    buttons.clear();
    checkboxes.clear();
    radioButtons.clear();
    labels.clear();
    editBoxes.clear();
    listboxes.clear();
    comboBoxes.clear();
    sliders.clear();
    scrollbars.clear();
    loadingBars.clear();

    // Select the window again
    currentID = 1;

    // I wrote these defines to avoid having the same code over and over again
    #define CHECK_SHARED_PROPERTIES(name, objects) \
        if (line.substr(0, 5).compare("size=") == 0) \
        { \
            tgui::Vector2f size; \
            if (tgui::extractVector2f(line.erase(0, 5), size) == false) \
                goto LoadingFailed; \
          \
            name->setSize(size.x, size.y); \
            objects.back().width.value = size.x; \
            objects.back().height.value = size.y; \
        } \
        else if (line.substr(0, 6).compare("width=") == 0) \
        { \
            line.erase(0, 6); \
            name->setSize(static_cast<float>(atoi(line.c_str())), name->getScaledSize().y); \
            objects.back().width.value = atoi(line.c_str()); \
        } \
        else if (line.substr(0, 7).compare("height=") == 0) \
        { \
            line.erase(0, 7); \
            name->setSize(name->getScaledSize().x, static_cast<float>(atoi(line.c_str()))); \
            objects.back().height.value = atoi(line.c_str()); \
        } \
        else if (line.substr(0, 6).compare("scale=") == 0) \
        { \
            line.erase(0, 6); \
            tgui::Vector2f objScale; \
            if (tgui::extractVector2f(line, objScale) == false) \
                goto LoadingFailed; \
          \
            name->setScale(objScale); \
            objects.back().width.value = name->getScaledSize().x; \
            objects.back().height.value = name->getScaledSize().y; \
        } \
        else if (line.substr(0, 9).compare("position=") == 0) \
        { \
            line.erase(0, 9); \
            tgui::Vector2f position; \
            if (tgui::extractVector2f(line, position) == false) \
                goto LoadingFailed; \
          \
            name->setPosition(position); \
            objects.back().left.value = position.x; \
            objects.back().top.value = position.y; \
        } \
        else if (line.substr(0, 5).compare("left=") == 0) \
        { \
            line.erase(0, 5); \
            name->setPosition(static_cast<float>(atoi(line.c_str())), name->getPosition().y); \
            objects.back().left.value = atoi(line.c_str()); \
        } \
        else if (line.substr(0, 4).compare("top=") == 0) \
        { \
            line.erase(0, 4); \
            name->setPosition(name->getPosition().x, static_cast<float>(atoi(line.c_str()))); \
            objects.back().top.value = atoi(line.c_str()); \
        } \
        else if (line.substr(0, 11).compare("callbackid=") == 0) \
        { \
            line.erase(0, 11); \
            name->callbackID = atoi(line.c_str()); \
            objects.back().callbackID.value = atoi(line.c_str()); \
        }

    #define CHECK_FOR_QUOTES \
        if (line.empty() == true) \
            goto LoadingFailed; \
         \
        if ((line[0] == '"') && (line[line.length()-1] == '"')) \
        { \
            line.erase(0, 1); \
            line.erase(line.length()-1, 1); \
        } \
        else \
            goto LoadingFailed;

    #define CHECK_BOOL(boolean) \
        if (line.compare("true") == 0) \
            boolean = true; \
        else if (line.compare("false") == 0) \
            boolean = false; \
        else \
        { \
            if (atoi(line.c_str())) \
                boolean = true; \
            else \
                boolean = false; \
        }

    /// TODO: newObject doesn't support panels
    #define COMPARE_OBJECT(length, name, objectName, id) \
        if (line.substr(0, length).compare(name) == 0) \
        { \
            line.erase(0, length); \
          \
            if (line.empty() == false) \
            { \
                CHECK_FOR_QUOTES \
            } \
          \
            unsigned int newObjectID = newObject(id, line); \
            extraPtr = static_cast<void*>(parentPtr.top()->get##objectName(tgui::to_string(newObjectID))); \
            objectID = id + 1; \
            progress.push(0); \
        }

    #define START_LOADING_OBJECT \
        if (progress.top() == 0) \
        { \
            if (line.compare("{") == 0) \
            { \
                progress.pop(); \
                progress.push(1); \
                break; \
            } \
            else \
                goto LoadingFailed; \
        } \
        else \
        { \
            if (line.compare("}") == 0) \
            { \
                objectID = parentID.top(); \
                parentID.pop(); \
                parentPtr.pop(); \
                progress.pop(); \
                break; \
            } \
        }

    // During the process some variables are needed to store what exactly was going on.
    std::stack<tgui::Group*> parentPtr;
    std::stack<unsigned int> parentID;
    std::stack<unsigned int> progress;
    unsigned int objectID = 0;
    void* extraPtr = NULL;
    bool multilineComment = false;
    
    std::vector<std::string> defineTokens;
    std::vector<std::string> defineValues;
    
    // Create a file object
    std::ifstream m_File;

    // Open the file
    m_File.open("form.txt", std::ifstream::in);

    // Check if the file was not opened
    if (m_File.is_open() == false)
        return false;

    // Stop reading when we reach the end of the file
    while (!m_File.eof())
    {
        // Get the next line
        std::string line;
        std::getline(m_File, line);

        // Check if there is a multiline comment
        if (multilineComment)
        {
            // Search for an asterisk
            std::string::size_type commentPos = line.find('*');

            // Check if there is an asterisk
            if (commentPos != std::string::npos)
            {
                // Make sure the asterisk is not the last character on the line
                if (line.length() > commentPos + 1)
                {
                    // Check if the next character is a slash
                    if (line[commentPos+1] == '/')
                    {
                        // Erase the first part of the line
                        line.erase(0, commentPos + 2);

                        // The multiline comment has been processed
                        multilineComment = false;

                        // Continue like normal
                        goto multilineCommentProcessed;
                    }
                }
                else // There is no end of the comment in this line
                    line = "";
            }
            else // There is no end of the comment in this line
                line = "";
        }
        else // There is no multiline comment
        {
          multilineCommentProcessed:

            // Search for a quote
            std::string::size_type quotePos1 = line.find('"');

            // Check if the quote was found or not
            if (quotePos1 == std::string::npos)
            {
                // Remove all spaces and tabs from the whole line
                line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
                line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                // Search for comments
                std::string::size_type commentPos = line.find('/');

                // Check if a slash was found
                if (commentPos != std::string::npos)
                {
                    // Make sure the slash is not the last character on the line
                    if (line.length() > commentPos + 1)
                    {
                        // Erase the comment (if there is one)
                        if (line[commentPos+1] == '/')
                            line.erase(commentPos);
                        else if (line[commentPos+1] == '*')
                        {
                            // Remove the rest of the line
                            line.erase(commentPos);

                            // From now on, everything is comment
                            multilineComment = true;
                        }
                        else // There is a slash in the middle of nowhere. It shouldn't be there.
                            goto LoadingFailed;
                    }
                    else // There is a slash on the end of the line. It shouldn't be there.
                        goto LoadingFailed;
                }

                // Convert the whole line to lowercase
                for (unsigned int i = 0; i < line.length(); i++)
                {
                    if ((line[i] > 64) && (line[i] < 91))
                        line[i] += 32;
                }
            }
            else
            {
                // Only remove spaces until the quote
                line.erase(std::remove(line.begin(), line.begin() + quotePos1, ' '), line.begin() + quotePos1);

                // Search for the quote again, because the position might have changed
                quotePos1 = line.find('"');

                // Only remove tabs until the quote
                line.erase(std::remove(line.begin(), line.begin() + quotePos1, '\t'), line.begin() + quotePos1);

                // Search for the quote again, because the position might have changed
                quotePos1 = line.find('"');

                // Search for comments
                std::string::size_type commentPos = line.substr(0, quotePos1).find('/');

                // Check if a slash was found
                if (commentPos != std::string::npos)
                {
                    // Erase the comment (if there is one)
                    if (line[commentPos+1] == '/')
                        line.erase(commentPos);
                    else if (line[commentPos+1] == '*')
                    {
                        // Remove the rest of the line
                        line.erase(commentPos);

                        // From now on, everything is comment
                        multilineComment = true;
                    }
                    else // There is a slash in the middle of nowhere. It shouldn't be there.
                        goto LoadingFailed;
                }

                // Search for the quote again, because the position might have changed
                quotePos1 = line.find('"');

                // The quote might have been behind the comment
                if (quotePos1 != std::string::npos)
                {
                    // Convert the part before the quote to lowercase
                    for (unsigned int i = 0; i < quotePos1; i++)
                    {
                        if ((line[i] > 64) && (line[i] < 91))
                            line[i] += 32;
                    }

                    // Search for a second quote
                    std::string::size_type quotePos2 = line.find('"', quotePos1 + 1);

                    // There must always be a second quote
                    if (quotePos2 == std::string::npos)
                        goto LoadingFailed;

                    // Remove all spaces and tabs after the quote
                    line.erase(std::remove(line.begin() + quotePos2, line.end(), ' '), line.end());
                    line.erase(std::remove(line.begin() + quotePos2, line.end(), '\t'), line.end());
                    line.erase(std::remove(line.begin() + quotePos2, line.end(), '\r'), line.end());

                    // Search for comments
                    std::string::size_type commentPos = line.find('/', quotePos2 + 1);

                    // Check if a slash was found
                    if (commentPos != std::string::npos)
                    {
                        // Make sure the slash is not the last character on the line
                        if (line.length() > commentPos + 1)
                        {
                            // Erase the comment (if there is one)
                            if (line[commentPos+1] == '/')
                                line.erase(commentPos);
                            else if (line[commentPos+1] == '*')
                            {
                                // Remove the rest of the line
                                line.erase(commentPos);

                                // From now on, everything is comment
                                multilineComment = true;
                            }
                            else // There is a slash in the middle of nowhere. It shouldn't be there.
                                goto LoadingFailed;
                        }
                        else // There is a slash on the end of the line. It shouldn't be there.
                            goto LoadingFailed;
                    }

                    // Search for the quote again, because the position might have changed
                    quotePos2 = line.find('"', quotePos1 + 1);

                    // There may never be more than two quotes
                    if (line.find('"', quotePos2 + 1) != std::string::npos)
                        goto LoadingFailed;

                    // Convert the part behind the quote to lowercase
                    for (unsigned int i = quotePos2; i < line.length(); i++)
                    {
                        if ((line[i] > 64) && (line[i] < 91))
                            line[i] += 32;
                    }
                }
            }
        }

        // Only continue when the line is not empty
        if (!line.empty())
        {
            // Check if something was defined
            if (defineTokens.empty() == false)
            {
                // Loop through all tokens
                for (unsigned int i=0; i<defineTokens.size(); ++i)
                {
                    // Search for every token in the line
                    std::string::size_type tokenPos = line.find(defineTokens[i]);
                    
                    // Check if a token was found
                    if (tokenPos != std::string::npos)
                    {
                        // Replace the token with the corresponding value
                        line.erase(tokenPos, defineTokens[i].length());
                        line.insert(tokenPos, defineValues[i]);
                    }
                }
            }

            // What happens now depends on the process
            switch (objectID)
            {
                case 0: // Done nothing yet
                {
                    // Check if this is the first line
                    if (progress.empty())
                    {
                        // The first line should contain 'window' or 'define'
                        if (line.substr(0, 7).compare("window:") == 0)
                        {
                            objectID = 0;
                            progress.push(1);
                        }
                        else // The second line is wrong
                            goto LoadingFailed;
                    }
                    else if (line.substr(0, 7).compare("define:") == 0)
                    {
                        line.erase(0, 7);
                        
                        // Search the equals sign
                        std::string::size_type equalsSignPos = line.find('=');
                        if (equalsSignPos != std::string::npos)
                        {
                            // Store the define
                            defineTokens.push_back(line.substr(0, equalsSignPos));
                            defineValues.push_back(line.erase(0, equalsSignPos + 1));
                        }
                        else // The equals sign is missing
                            goto LoadingFailed;
                    }
                    else // This is the second line
                    {
                        // The second line should contain "{"
                        if (line.compare("{") == 0)
                        {
                            objectID = tgui::window + 1;
                            progress.pop();
                        }
                        else // The second line is wrong
                            goto LoadingFailed;
                    }

                    break;
                }
                case tgui::window + 1: // The window was found
                {
                    // Find out if the loading is done
                    if (line.compare("}") == 0)
                        goto LoadingSucceeded;

                    // The next object will have the window as its parent
                    parentID.push(tgui::window + 1);
                    parentPtr.push(&mainWindow);

                    // The line doesn't contain a '}', so check what object it contains
/*                    COMPARE_OBJECT(6, "panel:", Panel, tgui::panel)
                    else*/ COMPARE_OBJECT(6, "label:", Label, tgui::label)
                    else COMPARE_OBJECT(7, "button:", Button, tgui::button)
                    else COMPARE_OBJECT(7, "slider:", Slider, tgui::slider)
                    else COMPARE_OBJECT(8, "picture:", Picture, tgui::picture)
                    else COMPARE_OBJECT(8, "listbox:", Listbox, tgui::listbox)
                    else COMPARE_OBJECT(8, "editbox:", EditBox, tgui::editBox)
                    else COMPARE_OBJECT(8, "textbox:", TextBox, tgui::textBox)
                    else COMPARE_OBJECT(9, "checkbox:", Checkbox, tgui::checkbox)
                    else COMPARE_OBJECT(9, "combobox:", ComboBox, tgui::comboBox)
                    else COMPARE_OBJECT(10, "scrollbar:", Scrollbar, tgui::scrollbar)
                    else COMPARE_OBJECT(11, "loadingbar:", LoadingBar, tgui::loadingBar)
                    else COMPARE_OBJECT(12, "radiobutton:", RadioButton, tgui::radioButton)

                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
/*                case tgui::panel + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the panel back
                    tgui::Panel* panel = static_cast<tgui::Panel*>(extraPtr);

                    CHECK_SHARED_PROPERTIES(panel, panels)
                    else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                    {
                        // Change the background color (black on error)
                        panel->backgroundColor = tgui::extractColor(line.erase(0, 16));
                    }
                    else
                    {
                        // All newly created objects must be part of the panel
                        parentID.push(tgui::panel + 1);
                        parentPtr.push(panel);

                        COMPARE_OBJECT(6, "panel:", Panel, tgui::panel)
                        else COMPARE_OBJECT(6, "label:", Label, tgui::label)
                        else COMPARE_OBJECT(7, "button:", Button, tgui::button)
                        else COMPARE_OBJECT(7, "slider:", Slider, tgui::slider)
                        else COMPARE_OBJECT(8, "picture:", Picture, tgui::picture)
                        else COMPARE_OBJECT(8, "listbox:", Listbox, tgui::listbox)
                        else COMPARE_OBJECT(8, "editbox:", EditBox, tgui::editBox)
                        else COMPARE_OBJECT(8, "textbox:", TextBox, tgui::textBox)
                        else COMPARE_OBJECT(9, "checkbox:", Checkbox, tgui::checkbox)
                        else COMPARE_OBJECT(9, "combobox:", ComboBox, tgui::comboBox)
                        else COMPARE_OBJECT(10, "scrollbar:", Scrollbar, tgui::scrollbar)
                        else COMPARE_OBJECT(11, "loadingbar:", LoadingBar, tgui::loadingBar)
                        else COMPARE_OBJECT(12, "radiobutton:", RadioButton, tgui::radioButton)
                        else // The line was wrong
                            goto LoadingFailed;
                    }

                    break;
                }
*/                case tgui::label + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the label back
                    tgui::Label* label = static_cast<tgui::Label*>(extraPtr);

                    if (line.substr(0, 5).compare("text=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The text must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Change the text
                        label->setText(line);
                        labels.back().text.value = line;
                    }
                    else if (line.substr(0, 9).compare("textsize=") == 0)
                    {
                        // Change the text size
                        unsigned int textSize = atoi(line.erase(0, 9).c_str());
                        label->setTextSize(textSize);
                        labels.back().textSize.value = textSize;
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        // Change the text color (black on error)
                        label->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        labels.back().textColor.value = line;
                    }
                    else CHECK_SHARED_PROPERTIES(label, labels)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::button + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the button back
                    tgui::Button* button = static_cast<tgui::Button*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the button
                        button->load(line);
                        buttons.back().pathname.value = line;
                    }
                    else if (line.substr(0, 5).compare("text=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The text must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Change the caption
                        button->setText(line);
                        buttons.back().text.value = line;
                    }
                    else if (line.substr(0, 9).compare("textsize=") == 0)
                    {
                        // Change the text size
                        unsigned int textSize = atoi(line.erase(0, 9).c_str());
                        button->setTextSize(textSize);
                        buttons.back().textSize.value = textSize;
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        // Change the text color (black on error)
                        button->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        buttons.back().textColor.value = line;
                    }
                    else CHECK_SHARED_PROPERTIES(button, buttons)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::slider + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the slider back
                    tgui::Slider* slider = static_cast<tgui::Slider*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the slider
                        slider->load(line);
                        sliders.back().pathname.value = line;
                    }
                    else if (line.substr(0, 6).compare("value=") == 0)
                    {
                        slider->setValue(atoi(line.erase(0, 6).c_str()));
                        sliders.back().value.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 8).compare("minimum=") == 0)
                    {
                        slider->setMinimum(atoi(line.erase(0, 8).c_str()));
                        sliders.back().minimum.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 8).compare("maximum=") == 0)
                    {
                        slider->setMaximum(atoi(line.erase(0, 8).c_str()));
                        sliders.back().maximum.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 15);

                        // Check if the value is true or false
                        bool vericalScroll;
                        CHECK_BOOL(vericalScroll)

                        slider->setVerticalScroll(vericalScroll);
                        sliders.back().verticalScroll.value = atoi(line.c_str());
                    }
                    else CHECK_SHARED_PROPERTIES(slider, sliders)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::picture + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the picture back
                    tgui::Picture* picture = static_cast<tgui::Picture*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("filename=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the picture
                        picture->load(line);
                        pictures.back().filename.value = line;
                    }
                    else CHECK_SHARED_PROPERTIES(picture, pictures)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::listbox + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the listbox back
                    tgui::Listbox* listbox = static_cast<tgui::Listbox*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 11).compare("itemheight=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 11);

                        // Set the width
                        listbox->setItemHeight(atoi(line.c_str()));
                        listboxes.back().itemHeight.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 18).compare("scrollbarpathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 18);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the scrollbar
                        listbox->setScrollbar(line);
                        listboxes.back().scrollbarPathname.value = line;
                    }
                    else if (line.substr(0, 8).compare("borders=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 8);

                        // Get the borders
                        tgui::Vector4u borders;
                        if (extractVector4u(line, borders) == false)
                            goto LoadingFailed;

                        listbox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                        listboxes.back().borders.value = line;
                    }
                    else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                    {
                        listbox->changeColors(tgui::extractColor(line.erase(0, 16)),
                                              listbox->getTextColor(),
                                              listbox->getSelectedBackgroundColor(),
                                              listbox->getSelectedTextColor(),
                                              listbox->getBorderColor());
                        listboxes.back().backgroundColor.value = line;
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        listbox->changeColors(listbox->getBackgroundColor(),
                                              tgui::extractColor(line.erase(0, 10)),
                                              listbox->getSelectedBackgroundColor(),
                                              listbox->getSelectedTextColor(),
                                              listbox->getBorderColor());
                        listboxes.back().textColor.value = line;
                    }
                    else if (line.substr(0, 24).compare("selectedbackgroundcolor=") == 0)
                    {
                        listbox->changeColors(listbox->getBackgroundColor(),
                                              listbox->getTextColor(),
                                              tgui::extractColor(line.erase(0, 24)),
                                              listbox->getSelectedTextColor(),
                                              listbox->getBorderColor());
                        listboxes.back().selectedBackgroundColor.value = line;
                    }
                    else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                    {
                        listbox->changeColors(listbox->getBackgroundColor(),
                                              listbox->getTextColor(),
                                              listbox->getSelectedBackgroundColor(),
                                              tgui::extractColor(line.erase(0, 18)),
                                              listbox->getBorderColor());
                        listboxes.back().selectedTextColor.value = line;
                    }
                    else if (line.substr(0, 12).compare("bordercolor=") == 0)
                    {
                        listbox->changeColors(listbox->getBackgroundColor(),
                                              listbox->getTextColor(),
                                              listbox->getSelectedBackgroundColor(),
                                              listbox->getSelectedTextColor(),
                                              tgui::extractColor(line.erase(0, 12)));
                        listboxes.back().borderColor.value = line;
                    }
                    else if (line.substr(0, 13).compare("maximumitems=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 13);

                        // Set the maximum items
                        listbox->setMaximumItems(atoi(line.c_str()));
                        listboxes.back().maximumItems.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 5).compare("item=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Add the item to the listbox
                        listbox->addItem(line);

                        std::string items = listboxes.back().items.value;

                        if (items.empty() == false)
                            items.push_back(',');

                        listboxes.back().items.value = items.append(line);
                    }
                    else if (line.substr(0, 13).compare("selecteditem=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 13);

                        // The line must contain something
                        if (line.empty() == true)
                            goto LoadingFailed;

                        // Check if there are quotes
                        if ((line[0] == '"') && (line[line.length()-1] == '"'))
                        {
                            line.erase(0, 1);
                            line.erase(line.length()-1, 1);

                            // Select the item
                            listbox->setSelectedItem(line);
                            listboxes.back().selectedItem.value = listbox->getSelectedItemID();
                        }
                        else // There were no quotes
                        {
                            // Select the item
                            listbox->setSelectedItem(atoi(line.c_str()));
                            listboxes.back().selectedItem.value = atoi(line.c_str());
                        }
                    }
                    else CHECK_SHARED_PROPERTIES(listbox, listboxes)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::editBox + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the edit box back
                    tgui::EditBox* editBox = static_cast<tgui::EditBox*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the edit box
                        editBox->load(line);
                        editBoxes.back().pathname.value = line;
                    }
                    else if (line.substr(0, 8).compare("borders=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 8);

                        // Get the borders
                        tgui::Vector4u borders;
                        if (extractVector4u(line, borders) == false)
                            goto LoadingFailed;

                        // Change the borders
                        editBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                        editBoxes.back().borders.value = line;
                    }
                    else if (line.substr(0, 5).compare("text=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The text must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Change the text
                        editBox->setText(line);
                        editBoxes.back().text.value = line;
                    }
                    else if (line.substr(0, 9).compare("textsize=") == 0)
                    {
                        // Change the text size
                        editBox->setTextSize(atoi(line.erase(0, 9).c_str()));
                        editBoxes.back().textSize.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        // Change the text color (black on error)
                        editBox->changeColors(tgui::extractColor(line.erase(0, 10)),
                                              editBox->getSelectedTextColor(),
                                              editBox->getSelectedTextBackgroundColor(),
                                              editBox->getUnfocusedSelectedTextBackgroundColor());
                        editBoxes.back().textColor.value = line;
                    }
                    else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                    {
                        // Change the selected text color (black on error)
                        editBox->changeColors(editBox->getTextColor(),
                                              tgui::extractColor(line.erase(0, 18)),
                                              editBox->getSelectedTextBackgroundColor(),
                                              editBox->getUnfocusedSelectedTextBackgroundColor());
                        editBoxes.back().selectedTextColor.value = line;
                    }
                    else if (line.substr(0, 28).compare("selectedtextbackgroundcolor=") == 0)
                    {
                        // Change the selected text background color (black on error)
                        editBox->changeColors(editBox->getTextColor(),
                                              editBox->getSelectedTextColor(),
                                              tgui::extractColor(line.erase(0, 28)),
                                              editBox->getUnfocusedSelectedTextBackgroundColor());
                        editBoxes.back().selectedTextBackgroundColor.value = line;
                    }
                    else if (line.substr(0, 37).compare("unfocusedselectedtextbackgroundcolor=") == 0)
                    {
                        // Change the selected text background color (black on error)
                        editBox->changeColors(editBox->getTextColor(),
                                              editBox->getSelectedTextColor(),
                                              editBox->getSelectedTextBackgroundColor(),
                                              tgui::extractColor(line.erase(0, 37)));
                        editBoxes.back().unfocusedSelectedTextBackgroundColor.value = line;
                    }
                    else if (line.substr(0, 20).compare("selectionpointcolor=") == 0)
                    {
                        // Change the selection pointer color (black on error)
                        editBox->selectionPointColor = tgui::extractColor(line.erase(0, 20));
                        editBoxes.back().selectionPointColor.value = line;
                    }
                    else if (line.substr(0, 13).compare("passwordchar=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 13);

                        // The text must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Make sure that the string is not empty
                        if (line.empty() == false)
                        {
                            // Set the password character
                            editBox->setPasswordChar(line[0]);
                            editBoxes.back().passwordChar.value = line[0];
                        }
                        else // The string is empty
                        {
                            editBox->setPasswordChar('\0');
                            editBoxes.back().passwordChar.value = '\0';
                        }
                    }
                    else if (line.substr(0, 20).compare("selectionpointwidth=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 20);

                        // Read the selection point width (0 and thus no selection point when it goes wrong)
                        editBox->selectionPointWidth = atoi(line.c_str());
                        editBoxes.back().selectionPointWidth.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 18).compare("maximumcharacters=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 18);

                        // Read the maximum characters (0 and thus no limit when it goes wrong)
                        editBox->setMaximumCharacters(atoi(line.c_str()));
                        editBoxes.back().maximumCharacters.value = atoi(line.c_str());
                    }
                    else CHECK_SHARED_PROPERTIES(editBox, editBoxes)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::textBox + 1:
                {
                    break;
                }
                case tgui::checkbox + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the checkbox back
                    tgui::Checkbox* checkbox = static_cast<tgui::Checkbox*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the checkbox
                        checkbox->load(line);
                        checkboxes.back().pathname.value = line;
                    }
                    else if (line.substr(0, 5).compare("text=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The text must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Change the caption
                        checkbox->setText(line);
                        checkboxes.back().text.value = line;
                    }
                    else if (line.substr(0, 8).compare("checked=") == 0)
                    {
                        // Remove the first part of the string
                        line.erase(0, 8);

                        // Check if the value is true or false
                        bool checked;
                        CHECK_BOOL(checked)

                        // Check or uncheck the checkbox
                        if (checked)
                            checkbox->check();
                        else
                            checkbox->uncheck();

                        checkboxes.back().checked.value = checked;
                    }
                    else if (line.substr(0, 9).compare("textsize=") == 0)
                    {
                        // Change the text size
                        checkbox->setTextSize(atoi(line.erase(0, 9).c_str()));
                        checkboxes.back().textSize.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        // Change the text color (black on error)
                        checkbox->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        checkboxes.back().textColor.value = line;
                    }
                    else CHECK_SHARED_PROPERTIES(checkbox, checkboxes)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::comboBox + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the combo box back
                    tgui::ComboBox* comboBox = static_cast<tgui::ComboBox*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the combo box
                        comboBox->load(line, comboBox->getSize().x);
                        comboBoxes.back().pathname.value = line;
                    }
                    else if (line.substr(0, 6).compare("width=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 6);

                        // Set the width
                        comboBox->setWidth(atoi(line.c_str()));
                        comboBoxes.back().width.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 18).compare("scrollbarpathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 18);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the scrollbar
                        comboBox->setScrollbar(line);
                        comboBoxes.back().scrollbarPathname.value = line;
                    }
                    else if (line.substr(0, 8).compare("borders=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 8);

                        // Get the borders
                        tgui::Vector4u borders;
                        if (extractVector4u(line, borders) == false)
                            goto LoadingFailed;

                        // Set the borders
                        comboBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                        comboBoxes.back().borders.value = line;
                    }
                    else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                    {
                        comboBox->changeColors(tgui::extractColor(line.erase(0, 16)),
                                               comboBox->getTextColor(),
                                               comboBox->getSelectedBackgroundColor(),
                                               comboBox->getSelectedTextColor(),
                                               comboBox->getBorderColor());
                        comboBoxes.back().backgroundColor.value = line;
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        comboBox->changeColors(comboBox->getBackgroundColor(),
                                               tgui::extractColor(line.erase(0, 10)),
                                               comboBox->getSelectedBackgroundColor(),
                                               comboBox->getSelectedTextColor(),
                                               comboBox->getBorderColor());
                        comboBoxes.back().textColor.value = line;
                    }
                    else if (line.substr(0, 24).compare("selectedbackgroundcolor=") == 0)
                    {
                        comboBox->changeColors(comboBox->getBackgroundColor(),
                                               comboBox->getTextColor(),
                                               tgui::extractColor(line.erase(0, 24)),
                                               comboBox->getSelectedTextColor(),
                                               comboBox->getBorderColor());
                        comboBoxes.back().selectedBackgroundColor.value = line;
                    }
                    else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                    {
                        comboBox->changeColors(comboBox->getBackgroundColor(),
                                               comboBox->getTextColor(),
                                               comboBox->getSelectedBackgroundColor(),
                                               tgui::extractColor(line.erase(0, 18)),
                                               comboBox->getBorderColor());
                        comboBoxes.back().selectedTextColor.value = line;
                    }
                    else if (line.substr(0, 12).compare("bordercolor=") == 0)
                    {
                        comboBox->changeColors(comboBox->getBackgroundColor(),
                                               comboBox->getTextColor(),
                                               comboBox->getSelectedBackgroundColor(),
                                               comboBox->getSelectedTextColor(),
                                               tgui::extractColor(line.erase(0, 12)));
                        comboBoxes.back().borderColor.value = line;
                    }
                    else if (line.substr(0, 15).compare("itemstodisplay=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 15);

                        // Set the nr of items to display
                        comboBox->setItemsToDisplay(atoi(line.c_str()));
                        comboBoxes.back().itemsToDisplay.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 5).compare("item=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Add the item to the combo box
                        comboBox->addItem(line);

                        std::string items = comboBoxes.back().items.value;

                        if (items.empty() == false)
                            items.push_back(',');

                        comboBoxes.back().items.value = items.append(line);
                    }
                    else if (line.substr(0, 13).compare("selecteditem=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 13);

                        // The line must contain something
                        if (line.empty() == true)
                            goto LoadingFailed;

                        // Check if there are quotes
                        if ((line[0] == '"') && (line[line.length()-1] == '"'))
                        {
                            line.erase(0, 1);
                            line.erase(line.length()-1, 1);

                            // Select the item
                            comboBox->setSelectedItem(line);
                            comboBoxes.back().selectedItem.value = comboBox->getSelectedItemID();
                        }
                        else // There were no quotes
                        {
                            // Select the item
                            comboBox->setSelectedItem(atoi(line.c_str()));
                            comboBoxes.back().selectedItem.value = atoi(line.c_str());
                        }
                    }
                    else CHECK_SHARED_PROPERTIES(comboBox, comboBoxes)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::scrollbar + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the scrollbar back
                    tgui::Scrollbar* scrollbar = static_cast<tgui::Scrollbar*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the scrollbar
                        scrollbar->load(line);
                        scrollbars.back().pathname.value = line;
                    }
                    else if (line.substr(0, 6).compare("value=") == 0)
                    {
                        scrollbar->setValue(atoi(line.erase(0, 6).c_str()));
                        scrollbars.back().value.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 8).compare("maximum=") == 0)
                    {
                        scrollbar->setMaximum(atoi(line.erase(0, 8).c_str()));
                        scrollbars.back().maximum.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 9).compare("lowvalue=") == 0)
                    {
                        scrollbar->setLowValue(atoi(line.erase(0, 9).c_str()));
                        scrollbars.back().lowValue.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 15);

                        // Check if the value is true or false
                        bool vericalScroll;
                        CHECK_BOOL(vericalScroll)

                        scrollbar->setVerticalScroll(vericalScroll);
                        scrollbars.back().verticalScroll.value = vericalScroll;
                    }
                    else CHECK_SHARED_PROPERTIES(scrollbar, scrollbars)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::loadingBar + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the loading bar back
                    tgui::LoadingBar* loadingBar = static_cast<tgui::LoadingBar*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the loading bar
                        loadingBar->load(line);
                        loadingBars.back().pathname.value = line;
                    }
                    else if (line.substr(0, 6).compare("value=") == 0)
                    {
                        loadingBar->setValue(atoi(line.erase(0, 6).c_str()));
                        loadingBars.back().value.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 8).compare("minimum=") == 0)
                    {
                        loadingBar->setMinimum(atoi(line.erase(0, 8).c_str()));
                        loadingBars.back().minimum.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 8).compare("maximum=") == 0)
                    {
                        loadingBar->setMaximum(atoi(line.erase(0, 8).c_str()));
                        loadingBars.back().maximum.value = atoi(line.c_str());
                    }
                    else CHECK_SHARED_PROPERTIES(loadingBar, loadingBars)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
                case tgui::radioButton + 1:
                {
                    START_LOADING_OBJECT

                    // Get the pointer to the radio button back
                    tgui::RadioButton* radioButton = static_cast<tgui::RadioButton*>(extraPtr);

                    // Find out what the next property is
                    if (line.substr(0, 9).compare("pathname=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 9);

                        // The pathname must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Load the radio button
                        radioButton->load(line);
                        radioButtons.back().pathname.value = line;
                    }
                    else if (line.substr(0, 5).compare("text=") == 0)
                    {
                        // Remove the first part of the line
                        line.erase(0, 5);

                        // The text must start and end with quotes
                        CHECK_FOR_QUOTES

                        // Change the caption
                        radioButton->setText(line);
                        radioButtons.back().text.value = line;
                    }
                    else if (line.substr(0, 8).compare("checked=") == 0)
                    {
                        // Remove the first part of the string
                        line.erase(0, 8);

                        // Check if the value is true or false
                        bool checked;
                        CHECK_BOOL(checked)

                        // Check or uncheck the radio button
                        if (checked)
                            radioButton->check();
                        else
                            radioButton->uncheck();

                        radioButtons.back().checked.value = checked;
                    }
                    else if (line.substr(0, 9).compare("textsize=") == 0)
                    {
                        // Change the text size
                        radioButton->setTextSize(atoi(line.erase(0, 9).c_str()));
                        radioButtons.back().textSize.value = atoi(line.c_str());
                    }
                    else if (line.substr(0, 10).compare("textcolor=") == 0)
                    {
                        // Change the text color (black on error)
                        radioButton->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        radioButtons.back().textColor.value = atoi(line.c_str());
                    }
                    else CHECK_SHARED_PROPERTIES(radioButton, radioButtons)
                    else // The line was wrong
                        goto LoadingFailed;

                    break;
                }
            };
        }
    }


  LoadingSucceeded:

    m_File.close();
    changeVisibleProperties();
    return true;

  LoadingFailed:

    m_File.close();
    changeVisibleProperties();
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TAB "    "

#define FindLowestID(objects, ID) \
for (i=0; i<objects.size(); ++i) \
{ \
    if ((objects[i].id < lowestID) && (objects[i].id > doneID)) \
    { \
        lowestID = objects[i].id; \
        objectIndex = i; \
        objectID = ID; \
    } \
}

#define AddBrackets(string) \
if (string.size() > 0) \
{ \
    if (string[0] != '(') \
        string.insert(string.begin(), '('); \
  \
    if (string[string.length()-1] != ')') \
        string.insert(string.end(), ')'); \
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::saveForm()
{
    // Open the form file for writing
    FILE* pFile = fopen("form.txt", "w");

    if (pFile == NULL)
        return;

    // Write the header
    std::string line("// This file was generated by TGUI Form Builder " VERSION "\n\n");
    fwrite(line.c_str(), 1, line.size(), pFile);


    unsigned int doneID = 0;

    // Write the start of the window
    line = "Window: \"";
    line.append(windows[0].name.value).append("\"\n{\n");
    fwrite(line.c_str(), 1, line.size(), pFile);

    unsigned int count = TGUI_MAX_OBJECTS;
    while (--count)
    {
        unsigned int  lowestID = TGUI_MAX_OBJECTS;
        unsigned char objectID = 0;
        unsigned int  objectIndex = 0;
        unsigned int  i;

        FindLowestID(pictures, tgui::picture)
        FindLowestID(buttons, tgui::button)
        FindLowestID(checkboxes, tgui::checkbox)
        FindLowestID(radioButtons, tgui::radioButton)
        FindLowestID(labels, tgui::label)
        FindLowestID(editBoxes, tgui::editBox)
        FindLowestID(listboxes, tgui::listbox)
        FindLowestID(comboBoxes, tgui::comboBox)
        FindLowestID(sliders, tgui::slider)
        FindLowestID(scrollbars, tgui::scrollbar)
        FindLowestID(loadingBars, tgui::loadingBar)

        // Check if you found another object
        if (objectID == 0)
            break;

        // The next id can be marked as done
        doneID = lowestID;

        // Check the type of the item
        if (objectID == tgui::picture)
        {
            line = TAB "Picture: \"";
            line.append(pictures[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Filename   = \"";
            line.append(pictures[objectIndex].filename.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left       = ";
            line.append(tgui::to_string(pictures[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top        = ";
            line.append(tgui::to_string(pictures[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width      = ";
            line.append(tgui::to_string(pictures[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height     = ";
            line.append(tgui::to_string(pictures[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(pictures[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::button)
        {
            line = TAB "Button: \"";
            line.append(buttons[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname   = \"";
            line.append(buttons[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left       = ";
            line.append(tgui::to_string(buttons[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top        = ";
            line.append(tgui::to_string(buttons[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width      = ";
            line.append(tgui::to_string(buttons[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height     = ";
            line.append(tgui::to_string(buttons[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(buttons[objectIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(buttons[objectIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(buttons[objectIndex].textColor.value)
            line.append(buttons[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(buttons[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::checkbox)
        {
            line = TAB "Checkbox: \"";
            line.append(checkboxes[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname   = \"";
            line.append(checkboxes[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left       = ";
            line.append(tgui::to_string(checkboxes[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top        = ";
            line.append(tgui::to_string(checkboxes[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width      = ";
            line.append(tgui::to_string(checkboxes[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height     = ";
            line.append(tgui::to_string(checkboxes[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Checked    = ";
            line.append(tgui::to_string(checkboxes[objectIndex].checked.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(checkboxes[objectIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(checkboxes[objectIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(checkboxes[objectIndex].textColor.value)
            line.append(checkboxes[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(checkboxes[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::radioButton)
        {
            line = TAB "RadioButton: \"";
            line.append(radioButtons[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname   = \"";
            line.append(radioButtons[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left       = ";
            line.append(tgui::to_string(radioButtons[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top        = ";
            line.append(tgui::to_string(radioButtons[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width      = ";
            line.append(tgui::to_string(radioButtons[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height     = ";
            line.append(tgui::to_string(radioButtons[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Checked    = ";
            line.append(tgui::to_string(radioButtons[objectIndex].checked.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(radioButtons[objectIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(radioButtons[objectIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(radioButtons[objectIndex].textColor.value)
            line.append(radioButtons[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(radioButtons[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::label)
        {
            line = TAB "Label: \"";
            line.append(labels[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left       = ";
            line.append(tgui::to_string(labels[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top        = ";
            line.append(tgui::to_string(labels[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(labels[objectIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(labels[objectIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(labels[objectIndex].textColor.value)
            line.append(labels[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(labels[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::editBox)
        {
            line = TAB "EditBox: \"";
            line.append(editBoxes[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname          = \"";
            line.append(editBoxes[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left              = ";
            line.append(tgui::to_string(editBoxes[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top               = ";
            line.append(tgui::to_string(editBoxes[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width             = ";
            line.append(tgui::to_string(editBoxes[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height            = ";
            line.append(tgui::to_string(editBoxes[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text              = \"";
            line.append(editBoxes[objectIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize          = ";
            line.append(tgui::to_string(editBoxes[objectIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            if (editBoxes[objectIndex].passwordChar.value != '\0')
            {
                line = TAB TAB "PasswordChar      = \"";
                line.push_back(editBoxes[objectIndex].passwordChar.value);
                line.append("\"\n");
                fwrite(line.c_str(), 1, line.size(), pFile);
            }

            line = TAB TAB "MaximumCharacters = ";
            line.append(tgui::to_string(editBoxes[objectIndex].maximumCharacters.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders           = ";
            AddBrackets(editBoxes[objectIndex].borders.value)
            line.append(editBoxes[objectIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor         = ";
            AddBrackets(editBoxes[objectIndex].textColor.value)
            line.append(editBoxes[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor = ";
            AddBrackets(editBoxes[objectIndex].selectedTextColor.value)
            line.append(editBoxes[objectIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextBackgroundColor = ";
            AddBrackets(editBoxes[objectIndex].selectedTextBackgroundColor.value)
            line.append(editBoxes[objectIndex].selectedTextBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "UnfocusedSelectedTextBackgroundColor = ";
            AddBrackets(editBoxes[objectIndex].unfocusedSelectedTextBackgroundColor.value)
            line.append(editBoxes[objectIndex].unfocusedSelectedTextBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectionPointColor = ";
            AddBrackets(editBoxes[objectIndex].selectionPointColor.value)
            line.append(editBoxes[objectIndex].selectionPointColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectionPointWidth = ";
            line.append(tgui::to_string(editBoxes[objectIndex].selectionPointWidth.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID          = ";
            line.append(tgui::to_string(editBoxes[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::listbox)
        {
            line = TAB "Listbox: \"";
            line.append(listboxes[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left                    = ";
            line.append(tgui::to_string(listboxes[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top                     = ";
            line.append(tgui::to_string(listboxes[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width                   = ";
            line.append(tgui::to_string(listboxes[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height                  = ";
            line.append(tgui::to_string(listboxes[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ScrollbarPathname       = \"";
            line.append(listboxes[objectIndex].scrollbarPathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ItemHeight              = ";
            line.append(tgui::to_string(listboxes[objectIndex].itemHeight.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "MaximumItems            = ";
            line.append(tgui::to_string(listboxes[objectIndex].maximumItems.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders                 = ";
            AddBrackets(listboxes[objectIndex].borders.value)
            line.append(listboxes[objectIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BackgroundColor         = ";
            AddBrackets(listboxes[objectIndex].backgroundColor.value)
            line.append(listboxes[objectIndex].backgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor               = ";
            AddBrackets(listboxes[objectIndex].textColor.value)
            line.append(listboxes[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedBackgroundColor = ";
            AddBrackets(listboxes[objectIndex].selectedBackgroundColor.value)
            line.append(listboxes[objectIndex].selectedBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor       = ";
            AddBrackets(listboxes[objectIndex].selectedTextColor.value)
            line.append(listboxes[objectIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BorderColor             = ";
            AddBrackets(listboxes[objectIndex].borderColor.value)
            line.append(listboxes[objectIndex].borderColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            // Make a list of all items
            {
                std::string strItems = listboxes[objectIndex].items.value;
                bool commaFound = true;

                // Loop all items
                while (commaFound)
                {
                    // Search for the next comma
                    std::string::size_type commaPos = strItems.find(',');

                    // Check if a comma was found
                    if (commaPos != std::string::npos)
                    {
                        // Add the item
                        line = TAB TAB "Item                    = \"";
                        line.append(strItems.substr(0, commaPos)).append("\"\n");
                        fwrite(line.c_str(), 1, line.size(), pFile);

                        // Erase the item from the string
                        strItems.erase(0, commaPos + 1);
                    }
                    else // No comma was found
                    {
                        // Add the last item, when the string isn't empty
                        if (strItems.size() > 0)
                        {
                            line = TAB TAB "Item                    = \"";
                            line.append(strItems).append("\"\n");
                            fwrite(line.c_str(), 1, line.size(), pFile);
                        }

                        // Stop the loop
                        commaFound = false;
                    }
                }
            }

            line = TAB TAB "SelectedItem            = ";
            line.append(tgui::to_string(listboxes[objectIndex].selectedItem.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID              = ";
            line.append(tgui::to_string(listboxes[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::comboBox)
        {
            line = TAB "ComboBox: \"";
            line.append(comboBoxes[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname                = \"";
            line.append(comboBoxes[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left                    = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top                     = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width                   = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height                  = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ScrollbarPathname       = \"";
            line.append(comboBoxes[objectIndex].scrollbarPathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders                 = ";
            AddBrackets(comboBoxes[objectIndex].borders.value)
            line.append(comboBoxes[objectIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BackgroundColor         = ";
            AddBrackets(comboBoxes[objectIndex].backgroundColor.value)
            line.append(comboBoxes[objectIndex].backgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor               = ";
            AddBrackets(comboBoxes[objectIndex].textColor.value)
            line.append(comboBoxes[objectIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedBackgroundColor = ";
            AddBrackets(comboBoxes[objectIndex].selectedBackgroundColor.value)
            line.append(comboBoxes[objectIndex].selectedBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor       = ";
            AddBrackets(comboBoxes[objectIndex].selectedTextColor.value)
            line.append(comboBoxes[objectIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BorderColor             = ";
            AddBrackets(comboBoxes[objectIndex].borderColor.value)
            line.append(comboBoxes[objectIndex].borderColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            // Make a list of all items
            {
                std::string strItems = comboBoxes[objectIndex].items.value;
                bool commaFound = true;

                // Loop all items
                while (commaFound)
                {
                    // Search for the next comma
                    std::string::size_type commaPos = strItems.find(',');

                    // Check if a comma was found
                    if (commaPos != std::string::npos)
                    {
                        // Add the item
                        line = TAB TAB "Item                    = \"";
                        line.append(strItems.substr(0, commaPos)).append("\"\n");
                        fwrite(line.c_str(), 1, line.size(), pFile);

                        // Erase the item from the string
                        strItems.erase(0, commaPos + 1);
                    }
                    else // No comma was found
                    {
                        // Add the last item, when the string isn't empty
                        if (strItems.size() > 0)
                        {
                            line = TAB TAB "Item                    = \"";
                            line.append(strItems).append("\"\n");
                            fwrite(line.c_str(), 1, line.size(), pFile);
                        }

                        // Stop the loop
                        commaFound = false;
                    }
                }
            }

            line = TAB TAB "SelectedItem            = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].selectedItem.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ItemsToDisplay          = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].itemsToDisplay.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID              = ";
            line.append(tgui::to_string(comboBoxes[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::slider)
        {
            line = TAB "Slider: \"";
            line.append(sliders[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname       = \"";
            line.append(sliders[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "VerticalScroll = ";
            line.append(tgui::to_string(sliders[objectIndex].verticalScroll.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left           = ";
            line.append(tgui::to_string(sliders[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top            = ";
            line.append(tgui::to_string(sliders[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width          = ";
            line.append(tgui::to_string(sliders[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height         = ";
            line.append(tgui::to_string(sliders[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Value          = ";
            line.append(tgui::to_string(sliders[objectIndex].value.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Minimum        = ";
            line.append(tgui::to_string(sliders[objectIndex].minimum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Maximum        = ";
            line.append(tgui::to_string(sliders[objectIndex].maximum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID     = ";
            line.append(tgui::to_string(sliders[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::scrollbar)
        {
            line = TAB "Scrollbar: \"";
            line.append(scrollbars[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname       = \"";
            line.append(scrollbars[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "VerticalScroll = ";
            line.append(tgui::to_string(scrollbars[objectIndex].verticalScroll.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left           = ";
            line.append(tgui::to_string(scrollbars[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top            = ";
            line.append(tgui::to_string(scrollbars[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width          = ";
            line.append(tgui::to_string(scrollbars[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height         = ";
            line.append(tgui::to_string(scrollbars[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "LowValue       = ";
            line.append(tgui::to_string(scrollbars[objectIndex].lowValue.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Maximum        = ";
            line.append(tgui::to_string(scrollbars[objectIndex].maximum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Value          = ";
            line.append(tgui::to_string(scrollbars[objectIndex].value.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID     = ";
            line.append(tgui::to_string(scrollbars[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (objectID == tgui::loadingBar)
        {
            line = TAB "LoadingBar: \"";
            line.append(loadingBars[objectIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Pathname   = \"";
            line.append(loadingBars[objectIndex].pathname.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Left       = ";
            line.append(tgui::to_string(loadingBars[objectIndex].left.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Top        = ";
            line.append(tgui::to_string(loadingBars[objectIndex].top.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Width      = ";
            line.append(tgui::to_string(loadingBars[objectIndex].width.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Height     = ";
            line.append(tgui::to_string(loadingBars[objectIndex].height.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Minimum    = ";
            line.append(tgui::to_string(loadingBars[objectIndex].minimum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Maximum    = ";
            line.append(tgui::to_string(loadingBars[objectIndex].maximum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Value      = ";
            line.append(tgui::to_string(loadingBars[objectIndex].value.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(loadingBars[objectIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
    }

    // Write the last bracket
    line = "}\n";
    fwrite(line.c_str(), 1, line.size(), pFile);

    // Close the file
    fclose(pFile);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
