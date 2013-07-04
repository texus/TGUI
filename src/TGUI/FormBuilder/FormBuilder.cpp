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

Builder::Builder() :
mainRenderWindow    (sf::VideoMode(1024, 768), "TGUI Form Builder " VERSION),
widgetsRenderWindow (sf::VideoMode(112, 400), "", sf::Style::Titlebar),
propertyRenderWindow(sf::VideoMode(300, 500), "", sf::Style::Titlebar | sf::Style::Resize),
mainWindow          (mainRenderWindow),
widgetsWindow       (widgetsRenderWindow),
propertyWindow      (propertyRenderWindow),
currentID           (0),
draggingWidget      (false),
draggingSquare      (0),
dragPos             (0, 0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::getUniqueID()
{
    static unsigned int id = 0;
    return ++id;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::newWidget(unsigned int widgetID, const std::string widgetName)
{
    currentID = getUniqueID();

    // Check which widget was created
    if (widgetID == tgui::Type_Unknown)
    {
        // Create a new property list
        PropertiesWindow properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        properties.builder = this;
        windows.push_back(properties);

        // Show the properties of the window
        propertyWindow.removeAllWidgets();
        windows.back().addProperties(propertyWindow);

        // Create an empty picture with the size of the window
        tgui::Picture::Ptr picture(mainWindow, "1");
        picture->load("images/Empty.png");
        picture->setSize(static_cast<float>(windows.back().width.value), static_cast<float>(windows.back().height.value));

        // Create the scale squares pictures
        tgui::Button::Ptr(mainWindow, "Square_TopLeft")->load("images/Square/Square.conf");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Top");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_TopRight");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Right");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_BottomRight");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Bottom");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_BottomLeft");
        mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Left");
    }
    else if (widgetID == tgui::Type_Picture)
    {
        // Create a new property list
        PropertiesPicture properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        pictures.push_back(properties);

        // Add a picture to the form
        tgui::Picture::Ptr picture(mainWindow, tgui::to_string(currentID));
        picture->load(pictures.back().filename.value);

        // Store the aspect ratio
        aspectRatios.push_back(picture->getSize().y / picture->getSize().x);
    }
    else if (widgetID == tgui::Type_Button)
    {
        // Create a new property list
        PropertiesButton properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        buttons.push_back(properties);

        // Add a button to the form
        tgui::Button::Ptr button(mainWindow, tgui::to_string(currentID));
        button->load(buttons.back().configFile.value);
        button->setSize(256, 64);

        // Store the aspect ratio
        aspectRatios.push_back(button->getSize().y / button->getSize().x);
    }
    else if (widgetID == tgui::Type_Checkbox)
    {
        // Create a new property list
        PropertiesCheckbox properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        checkboxes.push_back(properties);

        // Add a checkbox to the form
        tgui::Checkbox::Ptr checkbox(mainWindow, tgui::to_string(currentID));
        checkbox->load(checkboxes.back().configFile.value);

        // Store the aspect ratio
        aspectRatios.push_back(checkbox->getSize().y / checkbox->getSize().x);
    }
    else if (widgetID == tgui::Type_RadioButton)
    {
        // Create a new property list
        PropertiesRadioButton properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        properties.builder = this;
        radioButtons.push_back(properties);

        // Add a radio button to the form
        tgui::RadioButton::Ptr radioButton(mainWindow, tgui::to_string(currentID));
        radioButton->load(radioButtons.back().configFile.value);

        // Store the aspect ratio
        aspectRatios.push_back(radioButton->getSize().y / radioButton->getSize().x);
    }
    else if (widgetID == tgui::Type_Label)
    {
        // Create a new property list
        PropertiesLabel properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        labels.push_back(properties);

        // Add a label to the form
        tgui::Label::Ptr label(mainWindow, tgui::to_string(currentID));
        label->setText("Label");

        // Store the aspect ratio
        aspectRatios.push_back(label->getSize().y / label->getSize().x);
    }
    else if (widgetID == tgui::Type_EditBox)
    {
        // Create a new property list
        PropertiesEditBox properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        editBoxes.push_back(properties);

        // Add an edit box to the form
        tgui::EditBox::Ptr editBox(mainWindow, tgui::to_string(currentID));
        editBox->load(editBoxes.back().configFile.value);
        editBox->setBorders(2, 2, 2, 2);
        editBox->setSelectionPointColor(sf::Color(110, 110, 255));
        editBox->changeColors(sf::Color(200, 200, 200),
                              sf::Color(255, 255, 255),
                              sf::Color( 10, 110, 255));

        // Store the aspect ratio
        aspectRatios.push_back(editBox->getSize().y / editBox->getSize().x);
    }
    else if (widgetID == tgui::Type_ListBox)
    {
        // Create a new property list
        PropertiesListBox properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        listBoxes.push_back(properties);

        // Add an list box to the form
        tgui::ListBox::Ptr listBox(mainWindow, tgui::to_string(currentID));
        listBox->load(DEFAULT_THEME_FILE);
        listBox->setSize(200, 240);
        listBox->setItemHeight(30);
        listBox->setBorders(2, 2, 2, 2);
        listBox->changeColors(sf::Color( 50,  50,  50),
                              sf::Color(200, 200, 200),
                              sf::Color( 10, 110, 255),
                              sf::Color(255, 255, 255),
                              sf::Color::Black);

        // Store the aspect ratio
        aspectRatios.push_back(listBox->getSize().y / listBox->getSize().x);
    }
    else if (widgetID == tgui::Type_ComboBox)
    {
        // Create a new property list
        PropertiesComboBox properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        comboBoxes.push_back(properties);

        // Add an combo box to the form
        tgui::ComboBox::Ptr comboBox(mainWindow, tgui::to_string(currentID));
        comboBox->load(DEFAULT_THEME_FILE);
        comboBox->setSize(240, 36);
        comboBox->setMaximumItems(10);
        comboBox->setBorders(2, 2, 2, 2);
        comboBox->changeColors(sf::Color( 50,  50,  50),
                               sf::Color(200, 200, 200),
                               sf::Color( 10, 110, 255),
                               sf::Color(255, 255, 255),
                               sf::Color::Black);

        // Store the aspect ratio
        aspectRatios.push_back(comboBox->getSize().y / comboBox->getSize().x);
    }
    else if (widgetID == tgui::Type_Slider)
    {
        // Create a new property list
        PropertiesSlider properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        sliders.push_back(properties);

        // Add a slider to the form
        tgui::Slider::Ptr slider(mainWindow, tgui::to_string(currentID));
        slider->load(sliders.back().configFile.value);
        slider->setVerticalScroll(false);

        // Store the aspect ratio
        aspectRatios.push_back(slider->getSize().y / slider->getSize().x);
    }
    else if (widgetID == tgui::Type_Scrollbar)
    {
        // Create a new property list
        PropertiesScrollbar properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        scrollbars.push_back(properties);

        // Add a scrollbar to the form
        tgui::Scrollbar::Ptr scrollbar(mainWindow, tgui::to_string(currentID));
        scrollbar->load(scrollbars.back().configFile.value);
        scrollbar->setMaximum(5);
        scrollbar->setLowValue(4);
        scrollbar->setAutoHide(false);

        // Store the aspect ratio
        aspectRatios.push_back(scrollbar->getSize().y / scrollbar->getSize().x);
    }
    else if (widgetID == tgui::Type_LoadingBar)
    {
        // Create a new property list
        PropertiesLoadingBar properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        loadingBars.push_back(properties);

        // Add a loading bar to the form
        tgui::LoadingBar::Ptr loadingBar(mainWindow, tgui::to_string(currentID));
        loadingBar->load(loadingBars.back().configFile.value);
        loadingBar->setValue(0);

        // Store the aspect ratio
        aspectRatios.push_back(loadingBar->getSize().y / loadingBar->getSize().x);
    }
    else if (widgetID == tgui::Type_TextBox)
    {
        // Create a new property list
        PropertiesTextBox properties;
        properties.id = currentID;
        properties.name.value = widgetName;
        textBoxes.push_back(properties);

        // Add a text box to the form
        tgui::TextBox::Ptr textBox(mainWindow, tgui::to_string(currentID));
        textBox->load(textBoxes.back().configFile.value);
        textBox->setSize(320, 172);
        textBox->setTextSize(24);
        textBox->setBorders(2, 2, 2, 2);
        textBox->setSelectionPointColor(sf::Color(110, 110, 255));
        textBox->changeColors(sf::Color( 50,  50,  50),
                              sf::Color(200, 200, 200),
                              sf::Color(255, 255, 255),
                              sf::Color( 10, 110, 255),
                              sf::Color::Black);

        // Store the aspect ratio
        aspectRatios.push_back(textBox->getSize().y / textBox->getSize().x);
    }

    // Bring the scale squares to front
    mainWindow.get("Square_TopLeft")->moveToFront();
    mainWindow.get("Square_Top")->moveToFront();
    mainWindow.get("Square_TopRight")->moveToFront();
    mainWindow.get("Square_Right")->moveToFront();
    mainWindow.get("Square_BottomRight")->moveToFront();
    mainWindow.get("Square_Bottom")->moveToFront();
    mainWindow.get("Square_BottomLeft")->moveToFront();
    mainWindow.get("Square_Left")->moveToFront();

    resizePropertyWindow();

    return currentID;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::changeVisibleProperties()
{
    unsigned int i;

    // Remove all the widgets from the window
    propertyWindow.removeAllWidgets();

    // Check if the window is selected
    for (i=0; i<windows.size(); ++i)
    {
        if (windows[i].id == currentID)
        {
            windows[i].addProperties(propertyWindow);

            mainWindow.get("Square_TopLeft")->setPosition(-3, -3);
            mainWindow.get("Square_Top")->setPosition((windows[i].width.value / 2.f) - 3, -3);
            mainWindow.get("Square_TopRight")->setPosition(static_cast<float>(windows[i].width.value - 3), -3);
            mainWindow.get("Square_Right")->setPosition(static_cast<float>(windows[i].width.value - 3), (windows[i].height.value / 2.f) - 3);
            mainWindow.get("Square_BottomRight")->setPosition(static_cast<float>(windows[i].width.value - 3), static_cast<float>(windows[i].height.value - 3));
            mainWindow.get("Square_Bottom")->setPosition((windows[i].width.value / 2.f) - 3, static_cast<float>(windows[i].height.value - 3));
            mainWindow.get("Square_BottomLeft")->setPosition(-3, static_cast<float>(windows[i].height.value - 3));
            mainWindow.get("Square_Left")->setPosition(-3, (windows[i].height.value / 2.f) - 3);
            return;
        }
    }

    // Create the delete button
    tgui::Button::Ptr button(propertyWindow);
    button->load(DEFAULT_THEME_FILE);
    button->setText("Delete widget");
    button->setSize(propertyRenderWindow.getSize().x - 8.0, 40.0);
    button->setCallbackId(50);
    button->bindCallback(tgui::Button::LeftMouseClicked);

    // Check if a radio button is selected
    for (i=0; i<radioButtons.size(); ++i)
    {
        if (radioButtons[i].id == currentID)
        {
            radioButtons[i].addProperties(propertyWindow);
            button->setPosition(4, 10 + (40 * (Property_Checkbox_CallbackID + 1)));

            mainWindow.get("Square_TopLeft")->setPosition(radioButtons[i].left.value - 3, radioButtons[i].top.value - 3);
            mainWindow.get("Square_Top")->setPosition(radioButtons[i].left.value + (radioButtons[i].width.value / 2.f) - 3, radioButtons[i].top.value - 3);
            mainWindow.get("Square_TopRight")->setPosition(radioButtons[i].left.value + radioButtons[i].width.value - 3, radioButtons[i].top.value - 3);
            mainWindow.get("Square_Right")->setPosition(radioButtons[i].left.value + radioButtons[i].width.value - 3, radioButtons[i].top.value + (radioButtons[i].height.value / 2.f) - 3);
            mainWindow.get("Square_BottomRight")->setPosition(radioButtons[i].left.value + radioButtons[i].width.value - 3, radioButtons[i].top.value + radioButtons[i].height.value - 3);
            mainWindow.get("Square_Bottom")->setPosition(radioButtons[i].left.value + (radioButtons[i].width.value / 2.f) - 3, radioButtons[i].top.value + radioButtons[i].height.value - 3);
            mainWindow.get("Square_BottomLeft")->setPosition(radioButtons[i].left.value - 3, radioButtons[i].top.value + radioButtons[i].height.value - 3);
            mainWindow.get("Square_Left")->setPosition(radioButtons[i].left.value - 3, radioButtons[i].top.value + (radioButtons[i].height.value / 2.f) - 3);
            return;
        }
    }

    // Check if the other widgets are selected
    #define FindWidgetWithID(Widget, widgets) \
    for (i=0; i<widgets.size(); ++i) \
    { \
        if (widgets[i].id == currentID) \
        { \
            widgets[i].addProperties(propertyWindow); \
            button->setPosition(4, 10 + (40 * (Property_##Widget##_CallbackID + 1))); \
          \
            mainWindow.get("Square_TopLeft")->setPosition(widgets[i].left.value - 3, widgets[i].top.value - 3); \
            mainWindow.get("Square_Top")->setPosition(widgets[i].left.value + (widgets[i].width.value / 2.f) - 3, widgets[i].top.value - 3); \
            mainWindow.get("Square_TopRight")->setPosition(widgets[i].left.value + widgets[i].width.value - 3, widgets[i].top.value - 3); \
            mainWindow.get("Square_Right")->setPosition(widgets[i].left.value + widgets[i].width.value - 3, widgets[i].top.value + (widgets[i].height.value / 2.f) - 3); \
            mainWindow.get("Square_BottomRight")->setPosition(widgets[i].left.value + widgets[i].width.value - 3, widgets[i].top.value + widgets[i].height.value - 3); \
            mainWindow.get("Square_Bottom")->setPosition(widgets[i].left.value + (widgets[i].width.value / 2.f) - 3, widgets[i].top.value + widgets[i].height.value - 3); \
            mainWindow.get("Square_BottomLeft")->setPosition(widgets[i].left.value - 3, widgets[i].top.value + widgets[i].height.value - 3); \
            mainWindow.get("Square_Left")->setPosition(widgets[i].left.value - 3, widgets[i].top.value + (widgets[i].height.value / 2.f) - 3); \
            return; \
        } \
    }

    FindWidgetWithID(Picture, pictures)
    FindWidgetWithID(Button, buttons)
    FindWidgetWithID(Checkbox, checkboxes)
    FindWidgetWithID(Label, labels)
    FindWidgetWithID(EditBox, editBoxes)
    FindWidgetWithID(ListBox, listBoxes)
    FindWidgetWithID(ComboBox, comboBoxes)
    FindWidgetWithID(Slider, sliders)
    FindWidgetWithID(Scrollbar, scrollbars)
    FindWidgetWithID(LoadingBar, loadingBars)
    FindWidgetWithID(TextBox, textBoxes)

    #undef FindWidgetWithID
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::resizePropertyWindow()
{
    unsigned int i;

    for (i=0; i<windows.size(); ++i)
    {
        if (windows[i].id == currentID)
        {
            propertyRenderWindow.setSize(sf::Vector2u(propertyRenderWindow.getSize().x, 10 + (40 * (Property_Window_GlobalFont + 1))));
            return;
        }
    }

    for (i=0; i<radioButtons.size(); ++i)
    {
        if (radioButtons[i].id == currentID)
        {
            propertyRenderWindow.setSize(sf::Vector2u(propertyRenderWindow.getSize().x, 60 + (40 * (Property_Checkbox_CallbackID + 1))));
            return;
        }
    }

    #define FindWidgetWithID(Widget, widgets) \
    for (i=0; i<widgets.size(); ++i) \
    { \
        if (widgets[i].id == currentID) \
        { \
            propertyRenderWindow.setSize(sf::Vector2u(propertyRenderWindow.getSize().x, 60 + (40 * (Property_##Widget##_CallbackID + 1)))); \
            return; \
        } \
    }

    FindWidgetWithID(Picture, pictures)
    FindWidgetWithID(Button, buttons)
    FindWidgetWithID(Checkbox, checkboxes)
    FindWidgetWithID(Label, labels)
    FindWidgetWithID(EditBox, editBoxes)
    FindWidgetWithID(ListBox, listBoxes)
    FindWidgetWithID(ComboBox, comboBoxes)
    FindWidgetWithID(Slider, sliders)
    FindWidgetWithID(Scrollbar, scrollbars)
    FindWidgetWithID(LoadingBar, loadingBars)
    FindWidgetWithID(TextBox, textBoxes)

    #undef FindWidgetWithID
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
            mainWindow.get("Square_TopLeft")->setPosition(-3, -3);
            mainWindow.get("Square_Top")->setPosition((windows[i].width.value / 2.f) - 3, -3);
            mainWindow.get("Square_TopRight")->setPosition(static_cast<float>(windows[i].width.value - 3), -3);
            mainWindow.get("Square_Right")->setPosition(static_cast<float>(windows[i].width.value - 3), (windows[i].height.value / 2.f) - 3);
            mainWindow.get("Square_BottomRight")->setPosition(static_cast<float>(windows[i].width.value - 3), static_cast<float>(windows[i].height.value - 3));
            mainWindow.get("Square_Bottom")->setPosition((windows[i].width.value / 2.f) - 3, static_cast<float>(windows[i].height.value - 3));
            mainWindow.get("Square_BottomLeft")->setPosition(-3, static_cast<float>(windows[i].height.value - 3));
            mainWindow.get("Square_Left")->setPosition(-3, (windows[i].height.value / 2.f) - 3);
            return;
        }
    }

    #define FindWidgetWithID(widgets) \
    for (i=0; i<widgets.size(); ++i) \
    { \
     \
        if (widgets[i].id == currentID) \
        { \
            widgets[i].updateProperty(mainWindow, propertyWindow, propertyNumber); \
            mainWindow.get("Square_TopLeft")->setPosition(widgets[i].left.value - 3, widgets[i].top.value - 3); \
            mainWindow.get("Square_Top")->setPosition(widgets[i].left.value + (widgets[i].width.value / 2.f) - 3, widgets[i].top.value - 3); \
            mainWindow.get("Square_TopRight")->setPosition(widgets[i].left.value + widgets[i].width.value - 3, widgets[i].top.value - 3); \
            mainWindow.get("Square_Right")->setPosition(widgets[i].left.value + widgets[i].width.value - 3, widgets[i].top.value + (widgets[i].height.value / 2.f) - 3); \
            mainWindow.get("Square_BottomRight")->setPosition(widgets[i].left.value + widgets[i].width.value - 3, widgets[i].top.value + widgets[i].height.value - 3); \
            mainWindow.get("Square_Bottom")->setPosition(widgets[i].left.value + (widgets[i].width.value / 2.f) - 3, widgets[i].top.value + widgets[i].height.value - 3); \
            mainWindow.get("Square_BottomLeft")->setPosition(widgets[i].left.value - 3, widgets[i].top.value + widgets[i].height.value - 3); \
            mainWindow.get("Square_Left")->setPosition(widgets[i].left.value - 3, widgets[i].top.value + (widgets[i].height.value / 2.f) - 3); \
            return; \
        } \
    }

    FindWidgetWithID(pictures)
    FindWidgetWithID(buttons)
    FindWidgetWithID(checkboxes)
    FindWidgetWithID(radioButtons)
    FindWidgetWithID(labels)
    FindWidgetWithID(editBoxes)
    FindWidgetWithID(listBoxes)
    FindWidgetWithID(comboBoxes)
    FindWidgetWithID(sliders)
    FindWidgetWithID(scrollbars)
    FindWidgetWithID(loadingBars)
    FindWidgetWithID(textBoxes)

    #undef FindWidgetWithID
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::setGlobalFont(const sf::Font&/* font*/)
{
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

    // Loop through all list boxes
    for (i=0; i<listBoxes.size(); ++i)
    {
        // Check if the global font is used
        if (listBoxes[i].textFont.value.compare("Global") == 0)
            listBoxes[i].updateProperty(mainWindow, propertyWindow, Property_ListBox_TextFont);
    }

    // Loop through all combo boxes
    for (i=0; i<comboBoxes.size(); ++i)
    {
        // Check if the global font is used
        if (comboBoxes[i].textFont.value.compare("Global") == 0)
            comboBoxes[i].updateProperty(mainWindow, propertyWindow, Property_ListBox_TextFont);
    }
*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::getClickedWidgetID(sf::Event& event)
{
    // Check if the left mouse was pressed
    if (event.mouseButton.button == sf::Mouse::Left)
    {
        float mouseX = event.mouseButton.x / (mainRenderWindow.getSize().x / mainRenderWindow.getView().getSize().x);
        float mouseY = event.mouseButton.y / (mainRenderWindow.getSize().y / mainRenderWindow.getView().getSize().y);

        unsigned int highestID = 0;
        unsigned int widgetID = 1;

        #define FindWidgetNr(Widget, widgets) \
        for (unsigned int i=0; i<widgets.size(); ++i) \
        { \
            tgui::Widget::Ptr widget = mainWindow.get(tgui::to_string(widgets[i].id)); \
         \
            if (widget->mouseOnWidget(mouseX, mouseY)) \
            { \
                if (highestID < widgets[i].id) \
                { \
                    if (highestID > 0) \
                        widget->mouseNotOnWidget(); \
                 \
                    highestID = widgets[i].id; \
                    widgetID = widgets[i].id; \
                } \
            } \
        }

        FindWidgetNr(Picture, pictures)
        FindWidgetNr(Button, buttons)
        FindWidgetNr(Checkbox, checkboxes)
        FindWidgetNr(RadioButton, radioButtons)
        FindWidgetNr(Label, labels)
        FindWidgetNr(EditBox, editBoxes)
        FindWidgetNr(ListBox, listBoxes)
        FindWidgetNr(ComboBox, comboBoxes)
        FindWidgetNr(Slider, sliders)
        FindWidgetNr(Scrollbar, scrollbars)
        FindWidgetNr(LoadingBar, loadingBars)
        FindWidgetNr(TextBox, textBoxes)

        #undef FindWidgetNr

        // If our mouse is on top of an widget then return the id
        return widgetID;
    }
    else
        return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Builder::getScaleSquareWidgetID(float x, float y)
{
    float mouseX = x / (mainRenderWindow.getSize().x / mainRenderWindow.getView().getSize().x);
    float mouseY = y / (mainRenderWindow.getSize().y / mainRenderWindow.getView().getSize().y);

    unsigned int highestID = 0;
    unsigned int widgetID = 0;

    #define FindWidgetNr(name, objID) \
    { \
        tgui::Button::Ptr button = mainWindow.get(name); \
      \
        if (button->mouseOnWidget(mouseX, mouseY)) \
        { \
            if (highestID < objID) \
            { \
                button->mouseMoved(mouseX, mouseY); \
                highestID = objID; \
                widgetID = objID; \
            } \
        } \
    }

    FindWidgetNr("Square_TopLeft", SQUARE_TOP_LEFT)
    FindWidgetNr("Square_TopRight", SQUARE_TOP_RIGHT)
    FindWidgetNr("Square_BottomLeft", SQUARE_BOTTOM_LEFT)
    FindWidgetNr("Square_BottomRight", SQUARE_BOTTOM_RIGHT)
    FindWidgetNr("Square_Top", SQUARE_TOP)
    FindWidgetNr("Square_Left", SQUARE_LEFT)
    FindWidgetNr("Square_Right", SQUARE_RIGHT)
    FindWidgetNr("Square_Bottom", SQUARE_BOTTOM)

    #undef FindWidgetNr

    // If our mouse is on top of an widget then return true
    if (highestID != 0)
        return widgetID;
    else
        return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::deleteWidget()
{
    if (currentID > 1)
    {
        unsigned int i;

        // Remove the widget from the window
        mainWindow.remove(mainWindow.get(tgui::to_string(currentID)));

        // Find and remove the properties of the widget
        #define FindWidgetWithID(widget) \
        for (i=0; i<widget.size(); ++i) \
        { \
            if (widget[i].id == currentID) \
            { \
                widget.erase(widget.begin() + i); \
              \
                currentID = windows[0].id; \
                resizePropertyWindow(); \
                return; \
            } \
        }

        FindWidgetWithID(pictures)
        FindWidgetWithID(buttons)
        FindWidgetWithID(checkboxes)
        FindWidgetWithID(radioButtons)
        FindWidgetWithID(labels)
        FindWidgetWithID(editBoxes)
        FindWidgetWithID(listBoxes)
        FindWidgetWithID(comboBoxes)
        FindWidgetWithID(sliders)
        FindWidgetWithID(scrollbars)
        FindWidgetWithID(loadingBars)
        FindWidgetWithID(textBoxes)

        #undef FindWidgetWithID
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::moveWidgetX(float pixels)
{
    unsigned int i;

    // Get the pointer to the property
    tgui::EditBox::Ptr editbox = propertyWindow.get("text_Left");

    // Adjust the property
    float left = static_cast<float>(atof(editbox->getText().toAnsiString().c_str()));
    left += pixels;

    // Change the contents of the edit box
    editbox->setText(tgui::to_string(left));

    #define FindWidgetWithID(Widget, widgets) \
    for (i=0; i<widgets.size(); ++i) \
    { \
        if (widgets[i].id == currentID) \
        { \
            updateProperty(Property_##Widget##_Left); \
            return; \
        } \
    }

    FindWidgetWithID(Picture, pictures)
    FindWidgetWithID(Button, buttons)
    FindWidgetWithID(Checkbox, checkboxes)
    FindWidgetWithID(Checkbox, radioButtons)
    FindWidgetWithID(Label, labels)
    FindWidgetWithID(EditBox, editBoxes)
    FindWidgetWithID(ListBox, listBoxes)
    FindWidgetWithID(ComboBox, comboBoxes)
    FindWidgetWithID(Slider, sliders)
    FindWidgetWithID(Scrollbar, scrollbars)
    FindWidgetWithID(LoadingBar, loadingBars)
    FindWidgetWithID(TextBox, textBoxes)

    #undef FindWidgetWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::moveWidgetY(float pixels)
{
    unsigned int i;

    // Get the pointer to the property
    tgui::EditBox::Ptr editbox = propertyWindow.get("text_Top");

    // Adjust the property
    float top = static_cast<float>(atof(editbox->getText().toAnsiString().c_str()));
    top += pixels;

    // Change the contents of the edit box
    editbox->setText(tgui::to_string(top));

    #define FindWidgetWithID(Widget, widgets) \
    for (i=0; i<widgets.size(); ++i) \
    { \
        if (widgets[i].id == currentID) \
        { \
            updateProperty(Property_##Widget##_Top); \
            return; \
        } \
    }

    FindWidgetWithID(Picture, pictures)
    FindWidgetWithID(Button, buttons)
    FindWidgetWithID(Checkbox, checkboxes)
    FindWidgetWithID(Checkbox, radioButtons)
    FindWidgetWithID(Label, labels)
    FindWidgetWithID(EditBox, editBoxes)
    FindWidgetWithID(ListBox, listBoxes)
    FindWidgetWithID(ComboBox, comboBoxes)
    FindWidgetWithID(Slider, sliders)
    FindWidgetWithID(Scrollbar, scrollbars)
    FindWidgetWithID(LoadingBar, loadingBars)
    FindWidgetWithID(TextBox, textBoxes)

    #undef FindWidgetWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::resizeWidget(float addToWidth, float addToHeight)
{
    unsigned int i;

    // Ignore labels
    for (i = 0; i < labels.size(); ++i)
    {
        if (labels[i].id == currentID)
            return;
    }

    // Get the pointer to the properties
    tgui::EditBox::Ptr editboxWidth = propertyWindow.get("text_Width");
    tgui::EditBox::Ptr editboxHeight = propertyWindow.get("text_Height");

    // Adjust the properties
    float width = static_cast<float>(atof(editboxWidth->getText().toAnsiString().c_str()));
    float height = static_cast<float>(atof(editboxHeight->getText().toAnsiString().c_str()));
    width += addToWidth;
    height += addToHeight;

    // Change the contents of the edit boxes
    editboxWidth->setText(tgui::to_string(width));
    editboxHeight->setText(tgui::to_string(height));

    #define FindWidgetWithID(Widget, widgets) \
    for (i=0; i<widgets.size(); ++i) \
    { \
        if (widgets[i].id == currentID) \
        { \
            updateProperty(Property_##Widget##_Width); \
            updateProperty(Property_##Widget##_Height); \
            return; \
        } \
    }

    FindWidgetWithID(Picture, pictures)
    FindWidgetWithID(Button, buttons)
    FindWidgetWithID(Checkbox, checkboxes)
    FindWidgetWithID(Checkbox, radioButtons)
    FindWidgetWithID(EditBox, editBoxes)
    FindWidgetWithID(ListBox, listBoxes)
    FindWidgetWithID(ComboBox, comboBoxes)
    FindWidgetWithID(Slider, sliders)
    FindWidgetWithID(Scrollbar, scrollbars)
    FindWidgetWithID(LoadingBar, loadingBars)
    FindWidgetWithID(TextBox, textBoxes)

    #undef FindWidgetWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Builder::storeWidgetsNewAspectRatio()
{
    unsigned int i;

        #define FindWidgetWithID(Widget, widgets) \
        for (i=0; i<widgets.size(); ++i) \
        { \
            if (widgets[i].id == currentID) \
            { \
                aspectRatios[currentID-2] = widgets[i].height.value / widgets[i].width.value; \
                return; \
            } \
        }

        FindWidgetWithID(Picture, pictures)
        FindWidgetWithID(Button, buttons)
        FindWidgetWithID(Checkbox, checkboxes)
        FindWidgetWithID(Checkbox, radioButtons)
        FindWidgetWithID(Label, labels)
        FindWidgetWithID(EditBox, editBoxes)
        FindWidgetWithID(ListBox, listBoxes)
        FindWidgetWithID(ComboBox, comboBoxes)
        FindWidgetWithID(Slider, sliders)
        FindWidgetWithID(Scrollbar, scrollbars)
        FindWidgetWithID(LoadingBar, loadingBars)
        FindWidgetWithID(TextBox, textBoxes)

        #undef FindWidgetWithID
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Builder::loadForm()
{
    // Remove all existing widgets from the form
    mainWindow.removeAllWidgets();

    // Create an empty picture with the size of the window
    tgui::Picture::Ptr picture(mainWindow, "1");
    picture->load("images/Empty.png");
    picture->setSize(static_cast<float>(windows.back().width.value), static_cast<float>(windows.back().height.value));

    // Recreate the scale squares pictures
    tgui::Button::Ptr(mainWindow, "Square_TopLeft")->load("images/Square/Square.conf");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Top");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_TopRight");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Right");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_BottomRight");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Bottom");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_BottomLeft");
    mainWindow.copy(mainWindow.get("Square_TopLeft"), "Square_Left");

    // Remove all properties
    pictures.clear();
    buttons.clear();
    checkboxes.clear();
    radioButtons.clear();
    labels.clear();
    editBoxes.clear();
    listBoxes.clear();
    comboBoxes.clear();
    sliders.clear();
    scrollbars.clear();
    loadingBars.clear();
    textBoxes.clear();

    // Make sure the window is selected when it goes wrong
    currentID = 1;

    // Create a temporary window
    tgui::Gui tempWindow;
    tempWindow.setGlobalFont(mainWindow.getGlobalFont());

    // Load all the widgets in the temporary window
#ifdef __APPLE__
    if (tempWindow.loadWidgetsFromFile("../../../form.txt"))
#else
    if (tempWindow.loadWidgetsFromFile("form.txt"))
#endif
    {
        // Get a list of all the loaded widgets
        std::vector<tgui::Widget::Ptr> widgets = tempWindow.getWidgets();
        std::vector<sf::String> widgetNames = tempWindow.getWidgetNames();

        // Loop through all widgets
        for (unsigned int i=0; i<widgets.size(); ++i)
        {
            // Check what the widget type is
            if (widgets[i]->getWidgetType() == tgui::Type_Label)
            {
                // Convert the widget to a label (which it is)
                tgui::Label::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                labels.back().left.value = widget->getPosition().x;
                labels.back().top.value = widget->getPosition().y;
                labels.back().text.value = widget->getText();
                labels.back().textSize.value = widget->getTextSize();
                labels.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                labels.back().textFont.value = "Global";
                labels.back().backgroundColor.value = tgui::convertColorToString(widget->getBackgroundColor());
                labels.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::Label::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->setPosition(widget->getPosition());
                realWidget->setText(widget->getText());
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setTextColor(widget->getTextColor());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setBackgroundColor(widget->getBackgroundColor());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_Picture)
            {
                // Convert the widget to a picture (which it is)
                tgui::Picture::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                pictures.back().filename.value = widget->getLoadedFilename();
                pictures.back().left.value = widget->getPosition().x;
                pictures.back().top.value = widget->getPosition().y;
                pictures.back().width.value = widget->getSize().x;
                pictures.back().height.value = widget->getSize().y;
                pictures.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::Picture::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedFilename());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_Button)
            {
                // Convert the widget to a button (which it is)
                tgui::Button::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                buttons.back().configFile.value = widget->getLoadedConfigFile();
                buttons.back().left.value = widget->getPosition().x;
                buttons.back().top.value = widget->getPosition().y;
                buttons.back().width.value = widget->getSize().x;
                buttons.back().height.value = widget->getSize().y;
                buttons.back().text.value = widget->getText();
                buttons.back().textSize.value = widget->getTextSize();
                buttons.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                buttons.back().textFont.value = "Global";
                buttons.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::Button::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setText(widget->getText());
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setTextColor(widget->getTextColor());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_Checkbox)
            {
                // Convert the widget to a checkbox (which it is)
                tgui::Checkbox::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                checkboxes.back().configFile.value = widget->getLoadedConfigFile();
                checkboxes.back().left.value = widget->getPosition().x;
                checkboxes.back().top.value = widget->getPosition().y;
                checkboxes.back().width.value = widget->getSize().x;
                checkboxes.back().height.value = widget->getSize().y;
                checkboxes.back().checked.value = widget->isChecked();
                checkboxes.back().text.value = widget->getText();
                checkboxes.back().textSize.value = widget->getTextSize();
                checkboxes.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                checkboxes.back().textFont.value = "Global";
                checkboxes.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::Checkbox::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setText(widget->getText());
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setTextColor(widget->getTextColor());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setCallbackId(widget->getCallbackId());

                if (widget->isChecked())
                    realWidget->check();
                else
                    realWidget->uncheck();
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_RadioButton)
            {
                // Convert the widget to a radio button (which it is)
                tgui::RadioButton::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                radioButtons.back().configFile.value = widget->getLoadedConfigFile();
                radioButtons.back().left.value = widget->getPosition().x;
                radioButtons.back().top.value = widget->getPosition().y;
                radioButtons.back().width.value = widget->getSize().x;
                radioButtons.back().height.value = widget->getSize().y;
                radioButtons.back().checked.value = widget->isChecked();
                radioButtons.back().text.value = widget->getText();
                radioButtons.back().textSize.value = widget->getTextSize();
                radioButtons.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                radioButtons.back().textFont.value = "Global";
                radioButtons.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::RadioButton::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setText(widget->getText());
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setTextColor(widget->getTextColor());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setCallbackId(widget->getCallbackId());

                if (widget->isChecked())
                    realWidget->check();
                else
                    realWidget->uncheck();
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_EditBox)
            {
                // Convert the widget to an edit box (which it is)
                tgui::EditBox::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                editBoxes.back().configFile.value = widget->getLoadedConfigFile();
                editBoxes.back().left.value = widget->getPosition().x;
                editBoxes.back().top.value = widget->getPosition().y;
                editBoxes.back().width.value = widget->getSize().x;
                editBoxes.back().height.value = widget->getSize().y;
                editBoxes.back().text.value = widget->getText();
                editBoxes.back().textSize.value = widget->getTextSize();
                editBoxes.back().textFont.value = "Global";
                editBoxes.back().passwordChar.value = widget->getPasswordChar();
                editBoxes.back().maximumCharacters.value = widget->getMaximumCharacters();
                editBoxes.back().borders.value = "(" + tgui::to_string(widget->getBorders().x1) + "," + tgui::to_string(widget->getBorders().x2) + "," + tgui::to_string(widget->getBorders().x3) + "," + tgui::to_string(widget->getBorders().x4) + ")";
                editBoxes.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                editBoxes.back().selectedTextColor.value = tgui::convertColorToString(widget->getSelectedTextColor());
                editBoxes.back().selectedTextBackgroundColor.value = tgui::convertColorToString(widget->getSelectedTextBackgroundColor());
                editBoxes.back().selectionPointColor.value = tgui::convertColorToString(widget->getSelectionPointColor());
                editBoxes.back().selectionPointWidth.value = widget->getSelectionPointWidth();
                editBoxes.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::EditBox::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setText(widget->getText());
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setPasswordChar(widget->getPasswordChar());
                realWidget->setMaximumCharacters(widget->getMaximumCharacters());
                realWidget->setBorders(widget->getBorders().x1, widget->getBorders().x2, widget->getBorders().x3, widget->getBorders().x4);
                realWidget->changeColors(widget->getTextColor(), widget->getSelectedTextColor(), widget->getSelectedTextBackgroundColor());
                realWidget->setSelectionPointColor(widget->getSelectionPointColor());
                realWidget->setSelectionPointWidth(widget->getSelectionPointWidth());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_Slider)
            {
                // Convert the widget to a slider (which it is)
                tgui::Slider::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                sliders.back().configFile.value = widget->getLoadedConfigFile();
                sliders.back().left.value = widget->getPosition().x;
                sliders.back().top.value = widget->getPosition().y;
                sliders.back().width.value = widget->getSize().x;
                sliders.back().height.value = widget->getSize().y;
                sliders.back().verticalScroll.value = widget->getVerticalScroll();
                sliders.back().value.value = widget->getValue();
                sliders.back().minimum.value = widget->getMinimum();
                sliders.back().maximum.value = widget->getMaximum();
                sliders.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::Slider::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());

                realWidget->setVerticalScroll(widget->getVerticalScroll());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setMinimum(widget->getMinimum());
                realWidget->setMaximum(widget->getMaximum());
                realWidget->setValue(widget->getValue());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_Scrollbar)
            {
                // Convert the widget to a scrollbar (which it is)
                tgui::Scrollbar::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                scrollbars.back().configFile.value = widget->getLoadedConfigFile();
                scrollbars.back().left.value = widget->getPosition().x;
                scrollbars.back().top.value = widget->getPosition().y;
                scrollbars.back().width.value = widget->getSize().x;
                scrollbars.back().height.value = widget->getSize().y;
                scrollbars.back().verticalScroll.value = widget->getVerticalScroll();
                scrollbars.back().value.value = widget->getValue();
                scrollbars.back().lowValue.value = widget->getLowValue();
                scrollbars.back().maximum.value = widget->getMaximum();
                scrollbars.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::Scrollbar::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setVerticalScroll(widget->getVerticalScroll());
                realWidget->setLowValue(widget->getLowValue());
                realWidget->setMaximum(widget->getMaximum());
                realWidget->setValue(widget->getValue());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_ListBox)
            {
                // Convert the widget to a list box (which it is)
                tgui::ListBox::Ptr widget = widgets[i];

                // Get a list of the items
                std::vector<sf::String> items = widget->getItems();

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                listBoxes.back().configFile.value = widget->getLoadedConfigFile();
                listBoxes.back().left.value = widget->getPosition().x;
                listBoxes.back().top.value = widget->getPosition().y;
                listBoxes.back().width.value = widget->getSize().x;
                listBoxes.back().height.value = widget->getSize().y;
                listBoxes.back().itemHeight.value = widget->getItemHeight();
                listBoxes.back().maximumItems.value = widget->getMaximumItems();
                listBoxes.back().borders.value = "(" + tgui::to_string(widget->getBorders().x1) + "," + tgui::to_string(widget->getBorders().x2) + "," + tgui::to_string(widget->getBorders().x3) + "," + tgui::to_string(widget->getBorders().x4) + ")";
                listBoxes.back().backgroundColor.value = tgui::convertColorToString(widget->getBackgroundColor());
                listBoxes.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                listBoxes.back().selectedBackgroundColor.value = tgui::convertColorToString(widget->getSelectedBackgroundColor());
                listBoxes.back().selectedTextColor.value = tgui::convertColorToString(widget->getSelectedTextColor());
                listBoxes.back().borderColor.value = tgui::convertColorToString(widget->getBorderColor());
                listBoxes.back().textFont.value = "Global";
                listBoxes.back().callbackID.value = widget->getCallbackId();

                // If there is an item then add it to the list box
                if (items.empty() == false)
                {
                    listBoxes.back().items.value = items[0];

                    // Add the other items
                    for (unsigned int j=1; j<items.size(); ++j)
                        listBoxes.back().items.value += "," + items[j];
                }
                listBoxes.back().selectedItem.value = widget->getSelectedItemIndex();

                // Draw the widget in the correct way
                tgui::ListBox::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setSize(static_cast<unsigned int>(widget->getSize().x), static_cast<unsigned int>(widget->getSize().y));
                realWidget->setItemHeight(widget->getItemHeight());
                realWidget->setPosition(widget->getPosition());
                realWidget->setMaximumItems(widget->getMaximumItems());
                realWidget->setBorders(widget->getBorders().x1, widget->getBorders().x2, widget->getBorders().x3, widget->getBorders().x4);
                realWidget->changeColors(widget->getBackgroundColor(), widget->getTextColor(), widget->getSelectedBackgroundColor(), widget->getSelectedTextColor(), widget->getBorderColor());
                for (unsigned int j=0; j<items.size(); ++j) realWidget->addItem(items[j]);
                realWidget->setSelectedItem(widget->getSelectedItem());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_ComboBox)
            {
                // Convert the widget to a combo box (which it is)
                tgui::ComboBox::Ptr widget = widgets[i];

                // Get a list of the items
                std::vector<sf::String> items = widget->getItems();

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                comboBoxes.back().configFile.value = widget->getLoadedConfigFile();
                comboBoxes.back().left.value = widget->getPosition().x;
                comboBoxes.back().top.value = widget->getPosition().y;
                comboBoxes.back().width.value = widget->getSize().x;
                comboBoxes.back().height.value = widget->getSize().y;
                comboBoxes.back().borders.value = "(" + tgui::to_string(widget->getBorders().x1) + "," + tgui::to_string(widget->getBorders().x2) + "," + tgui::to_string(widget->getBorders().x3) + "," + tgui::to_string(widget->getBorders().x4) + ")";
                comboBoxes.back().backgroundColor.value = tgui::convertColorToString(widget->getBackgroundColor());
                comboBoxes.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                comboBoxes.back().selectedBackgroundColor.value = tgui::convertColorToString(widget->getSelectedBackgroundColor());
                comboBoxes.back().selectedTextColor.value = tgui::convertColorToString(widget->getSelectedTextColor());
                comboBoxes.back().borderColor.value = tgui::convertColorToString(widget->getBorderColor());
                comboBoxes.back().itemsToDisplay.value = widget->getItemsToDisplay();
                comboBoxes.back().textFont.value = "Global";
                comboBoxes.back().callbackID.value = widget->getCallbackId();

                // If there is an item then add it to the list box
                if (items.empty() == false)
                {
                    comboBoxes.back().items.value = items[0];

                    // Add the other items
                    for (unsigned int j=1; j<items.size(); ++j)
                        comboBoxes.back().items.value += "," + items[j];
                }
                comboBoxes.back().selectedItem.value = widget->getSelectedItemIndex();

                // Draw the widget in the correct way
                tgui::ComboBox::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setItemsToDisplay(widget->getItemsToDisplay());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setBorders(widget->getBorders().x1, widget->getBorders().x2, widget->getBorders().x3, widget->getBorders().x4);
                realWidget->changeColors(widget->getBackgroundColor(), widget->getTextColor(), widget->getSelectedBackgroundColor(), widget->getSelectedTextColor(), widget->getBorderColor());
                for (unsigned int j=0; j<items.size(); ++j) realWidget->addItem(items[j]);
                realWidget->setSelectedItem(widget->getSelectedItem());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_LoadingBar)
            {
                // Convert the widget to a loading bar (which it is)
                tgui::LoadingBar::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                loadingBars.back().configFile.value = widget->getLoadedConfigFile();
                loadingBars.back().left.value = widget->getPosition().x;
                loadingBars.back().top.value = widget->getPosition().y;
                loadingBars.back().width.value = widget->getSize().x;
                loadingBars.back().height.value = widget->getSize().y;
                loadingBars.back().value.value = widget->getValue();
                loadingBars.back().minimum.value = widget->getMinimum();
                loadingBars.back().maximum.value = widget->getMaximum();
                loadingBars.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::LoadingBar::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setMinimum(widget->getMinimum());
                realWidget->setMaximum(widget->getMaximum());
                realWidget->setValue(widget->getValue());
                realWidget->setCallbackId(widget->getCallbackId());
            }
            else if (widgets[i]->getWidgetType() == tgui::Type_TextBox)
            {
                // Convert the widget to a text box (which it is)
                tgui::TextBox::Ptr widget = widgets[i];

                // Create and fill the properties of the widget
                unsigned int id = newWidget(widgets[i]->getWidgetType(), widgetNames[i]);
                textBoxes.back().configFile.value = widget->getLoadedConfigFile();
                textBoxes.back().left.value = widget->getPosition().x;
                textBoxes.back().top.value = widget->getPosition().y;
                textBoxes.back().width.value = widget->getSize().x;
                textBoxes.back().height.value = widget->getSize().y;
                textBoxes.back().text.value = widget->getText();
                textBoxes.back().textSize.value = widget->getTextSize();
                textBoxes.back().textFont.value = "Global";
                textBoxes.back().maximumCharacters.value = widget->getMaximumCharacters();
                textBoxes.back().borders.value = "(" + tgui::to_string(widget->getBorders().x1) + "," + tgui::to_string(widget->getBorders().x2) + "," + tgui::to_string(widget->getBorders().x3) + "," + tgui::to_string(widget->getBorders().x4) + ")";
                textBoxes.back().backgroundColor.value = tgui::convertColorToString(widget->getBackgroundColor());
                textBoxes.back().textColor.value = tgui::convertColorToString(widget->getTextColor());
                textBoxes.back().selectedTextColor.value = tgui::convertColorToString(widget->getSelectedTextColor());
                textBoxes.back().selectedTextBackgroundColor.value = tgui::convertColorToString(widget->getSelectedTextBackgroundColor());
                textBoxes.back().borderColor.value = tgui::convertColorToString(widget->getBorderColor());
                textBoxes.back().selectionPointColor.value = tgui::convertColorToString(widget->getSelectionPointColor());
                textBoxes.back().selectionPointWidth.value = widget->getSelectionPointWidth();
                textBoxes.back().callbackID.value = widget->getCallbackId();

                // Draw the widget in the correct way
                tgui::TextBox::Ptr realWidget = mainWindow.get(tgui::to_string(id));
                realWidget->load(widget->getLoadedConfigFile());
                realWidget->setSize(static_cast<unsigned int>(widget->getSize().x), static_cast<unsigned int>(widget->getSize().y));
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setPosition(widget->getPosition());
                realWidget->setSize(widget->getSize().x, widget->getSize().y);
                realWidget->setText(widget->getText());
                realWidget->setTextSize(widget->getTextSize());
                realWidget->setTextFont(mainWindow.getGlobalFont());
                realWidget->setMaximumCharacters(widget->getMaximumCharacters());
                realWidget->setBorders(widget->getBorders().x1, widget->getBorders().x2, widget->getBorders().x3, widget->getBorders().x4);
                realWidget->changeColors(widget->getBackgroundColor(), widget->getTextColor(), widget->getSelectedTextColor(), widget->getSelectedTextBackgroundColor(), widget->getBorderColor());
                realWidget->setSelectionPointColor(widget->getSelectionPointColor());
                realWidget->setSelectionPointWidth(widget->getSelectionPointWidth());
                realWidget->setCallbackId(widget->getCallbackId());
            }
        }

        // Select the window
        currentID = 1;
        resizePropertyWindow();

        // The file has been loaded successfully.
        return true;
    }
    else // Loading failed
        return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_WIDGETS  10000
#define TAB "    "

#define FindHighestID(widgets) \
for (i=0; i<widgets.size(); ++i) \
{ \
    if (widgets[i].id > count) \
        count = widgets[i].id; \
}

#define FindLowestID(widgets, ID) \
for (i=0; i<widgets.size(); ++i) \
{ \
    if ((widgets[i].id < lowestID) && (widgets[i].id > doneID)) \
    { \
        lowestID = widgets[i].id; \
        widgetIndex = i; \
        widgetID = ID; \
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
#ifdef __APPLE__
    FILE* pFile = fopen("../../../form.txt", "w");
#else
    FILE* pFile = fopen("form.txt", "w");
#endif

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

    unsigned int  i;
    unsigned int count = 0;

    FindHighestID(pictures);
    FindHighestID(buttons);
    FindHighestID(checkboxes);
    FindHighestID(radioButtons);
    FindHighestID(labels);
    FindHighestID(editBoxes);
    FindHighestID(listBoxes);
    FindHighestID(comboBoxes);
    FindHighestID(sliders);
    FindHighestID(scrollbars);
    FindHighestID(loadingBars);
    FindHighestID(textBoxes);

    while (count--)
    {
        unsigned int  lowestID = MAX_WIDGETS;
        unsigned char widgetID = 0;
        unsigned int  widgetIndex = 0;

        FindLowestID(pictures, tgui::Type_Picture)
        FindLowestID(buttons, tgui::Type_Button)
        FindLowestID(checkboxes, tgui::Type_Checkbox)
        FindLowestID(radioButtons, tgui::Type_RadioButton)
        FindLowestID(labels, tgui::Type_Label)
        FindLowestID(editBoxes, tgui::Type_EditBox)
        FindLowestID(listBoxes, tgui::Type_ListBox)
        FindLowestID(comboBoxes, tgui::Type_ComboBox)
        FindLowestID(sliders, tgui::Type_Slider)
        FindLowestID(scrollbars, tgui::Type_Scrollbar)
        FindLowestID(loadingBars, tgui::Type_LoadingBar)
        FindLowestID(textBoxes, tgui::Type_TextBox)

        // Check if you found another widget
        if (widgetID == 0)
            break;

        // The next id can be marked as done
        doneID = lowestID;

        // Check the type of the item
        if (widgetID == tgui::Type_Picture)
        {
            line = TAB "Picture: \"";
            line.append(pictures[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Filename   = \"";
            line.append(pictures[widgetIndex].filename.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position   = ";
            line.append("(").append(tgui::to_string(pictures[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(pictures[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size       = ";
            line.append("(").append(tgui::to_string(pictures[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(pictures[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(pictures[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_Button)
        {
            line = TAB "Button: \"";
            line.append(buttons[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile = \"";
            line.append(buttons[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position   = ";
            line.append("(").append(tgui::to_string(buttons[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(buttons[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size       = ";
            line.append("(").append(tgui::to_string(buttons[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(buttons[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(buttons[widgetIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(buttons[widgetIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(buttons[widgetIndex].textColor.value)
            line.append(buttons[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(buttons[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_Checkbox)
        {
            line = TAB "Checkbox: \"";
            line.append(checkboxes[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile = \"";
            line.append(checkboxes[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position   = ";
            line.append("(").append(tgui::to_string(checkboxes[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(checkboxes[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size       = ";
            line.append("(").append(tgui::to_string(checkboxes[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(checkboxes[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Checked    = ";
            line.append(tgui::to_string(checkboxes[widgetIndex].checked.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(checkboxes[widgetIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(checkboxes[widgetIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(checkboxes[widgetIndex].textColor.value)
            line.append(checkboxes[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(checkboxes[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_RadioButton)
        {
            line = TAB "RadioButton: \"";
            line.append(radioButtons[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile = \"";
            line.append(radioButtons[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position   = ";
            line.append("(").append(tgui::to_string(radioButtons[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(radioButtons[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size       = ";
            line.append("(").append(tgui::to_string(radioButtons[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(radioButtons[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Checked    = ";
            line.append(tgui::to_string(radioButtons[widgetIndex].checked.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text       = \"";
            line.append(radioButtons[widgetIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize   = ";
            line.append(tgui::to_string(radioButtons[widgetIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor  = ";
            AddBrackets(radioButtons[widgetIndex].textColor.value)
            line.append(radioButtons[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(radioButtons[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_Label)
        {
            line = TAB "Label: \"";
            line.append(labels[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position         = ";
            line.append("(").append(tgui::to_string(labels[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(labels[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text             = \"";
            line.append(labels[widgetIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize         = ";
            line.append(tgui::to_string(labels[widgetIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor        = ";
            AddBrackets(labels[widgetIndex].textColor.value)
            line.append(labels[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BackgroundColor  = ";
            AddBrackets(labels[widgetIndex].backgroundColor.value)
            line.append(labels[widgetIndex].backgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID       = ";
            line.append(tgui::to_string(labels[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_EditBox)
        {
            line = TAB "EditBox: \"";
            line.append(editBoxes[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile          = \"";
            line.append(editBoxes[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position            = ";
            line.append("(").append(tgui::to_string(editBoxes[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(editBoxes[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size                = ";
            line.append("(").append(tgui::to_string(editBoxes[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(editBoxes[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Text                = \"";
            line.append(editBoxes[widgetIndex].text.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize            = ";
            line.append(tgui::to_string(editBoxes[widgetIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            if (editBoxes[widgetIndex].passwordChar.value != '\0')
            {
                line = TAB TAB "PasswordChar        = \"";
                line.push_back(editBoxes[widgetIndex].passwordChar.value);
                line.append("\"\n");
                fwrite(line.c_str(), 1, line.size(), pFile);
            }

            line = TAB TAB "MaximumCharacters   = ";
            line.append(tgui::to_string(editBoxes[widgetIndex].maximumCharacters.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders             = ";
            AddBrackets(editBoxes[widgetIndex].borders.value)
            line.append(editBoxes[widgetIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor           = ";
            AddBrackets(editBoxes[widgetIndex].textColor.value)
            line.append(editBoxes[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor   = ";
            AddBrackets(editBoxes[widgetIndex].selectedTextColor.value)
            line.append(editBoxes[widgetIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextBackgroundColor = ";
            AddBrackets(editBoxes[widgetIndex].selectedTextBackgroundColor.value)
            line.append(editBoxes[widgetIndex].selectedTextBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectionPointColor = ";
            AddBrackets(editBoxes[widgetIndex].selectionPointColor.value)
            line.append(editBoxes[widgetIndex].selectionPointColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectionPointWidth = ";
            line.append(tgui::to_string(editBoxes[widgetIndex].selectionPointWidth.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID          = ";
            line.append(tgui::to_string(editBoxes[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_ListBox)
        {
            line = TAB "ListBox: \"";
            line.append(listBoxes[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile              = \"";
            line.append(listBoxes[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position                = ";
            line.append("(").append(tgui::to_string(listBoxes[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(listBoxes[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size                    = ";
            line.append("(").append(tgui::to_string(listBoxes[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(listBoxes[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ItemHeight              = ";
            line.append(tgui::to_string(listBoxes[widgetIndex].itemHeight.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "MaximumItems            = ";
            line.append(tgui::to_string(listBoxes[widgetIndex].maximumItems.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders                 = ";
            AddBrackets(listBoxes[widgetIndex].borders.value)
            line.append(listBoxes[widgetIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BackgroundColor         = ";
            AddBrackets(listBoxes[widgetIndex].backgroundColor.value)
            line.append(listBoxes[widgetIndex].backgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor               = ";
            AddBrackets(listBoxes[widgetIndex].textColor.value)
            line.append(listBoxes[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedBackgroundColor = ";
            AddBrackets(listBoxes[widgetIndex].selectedBackgroundColor.value)
            line.append(listBoxes[widgetIndex].selectedBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor       = ";
            AddBrackets(listBoxes[widgetIndex].selectedTextColor.value)
            line.append(listBoxes[widgetIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BorderColor             = ";
            AddBrackets(listBoxes[widgetIndex].borderColor.value)
            line.append(listBoxes[widgetIndex].borderColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            // Make a list of all items
            {
                std::string strItems = listBoxes[widgetIndex].items.value;
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
            line.append(tgui::to_string(listBoxes[widgetIndex].selectedItem.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID              = ";
            line.append(tgui::to_string(listBoxes[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_ComboBox)
        {
            line = TAB "ComboBox: \"";
            line.append(comboBoxes[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile              = \"";
            line.append(comboBoxes[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position                = ";
            line.append("(").append(tgui::to_string(comboBoxes[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(comboBoxes[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size                    = ";
            line.append("(").append(tgui::to_string(comboBoxes[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(comboBoxes[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders                 = ";
            AddBrackets(comboBoxes[widgetIndex].borders.value)
            line.append(comboBoxes[widgetIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BackgroundColor         = ";
            AddBrackets(comboBoxes[widgetIndex].backgroundColor.value)
            line.append(comboBoxes[widgetIndex].backgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor               = ";
            AddBrackets(comboBoxes[widgetIndex].textColor.value)
            line.append(comboBoxes[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedBackgroundColor = ";
            AddBrackets(comboBoxes[widgetIndex].selectedBackgroundColor.value)
            line.append(comboBoxes[widgetIndex].selectedBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor       = ";
            AddBrackets(comboBoxes[widgetIndex].selectedTextColor.value)
            line.append(comboBoxes[widgetIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BorderColor             = ";
            AddBrackets(comboBoxes[widgetIndex].borderColor.value)
            line.append(comboBoxes[widgetIndex].borderColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            // Make a list of all items
            {
                std::string strItems = comboBoxes[widgetIndex].items.value;
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
            line.append(tgui::to_string(comboBoxes[widgetIndex].selectedItem.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ItemsToDisplay          = ";
            line.append(tgui::to_string(comboBoxes[widgetIndex].itemsToDisplay.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID              = ";
            line.append(tgui::to_string(comboBoxes[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_Slider)
        {
            line = TAB "Slider: \"";
            line.append(sliders[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile     = \"";
            line.append(sliders[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "VerticalScroll = ";
            line.append(tgui::to_string(sliders[widgetIndex].verticalScroll.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position       = ";
            line.append("(").append(tgui::to_string(sliders[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(sliders[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size           = ";
            line.append("(").append(tgui::to_string(sliders[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(sliders[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Value          = ";
            line.append(tgui::to_string(sliders[widgetIndex].value.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Minimum        = ";
            line.append(tgui::to_string(sliders[widgetIndex].minimum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Maximum        = ";
            line.append(tgui::to_string(sliders[widgetIndex].maximum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID     = ";
            line.append(tgui::to_string(sliders[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_Scrollbar)
        {
            line = TAB "Scrollbar: \"";
            line.append(scrollbars[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile     = \"";
            line.append(scrollbars[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "VerticalScroll = ";
            line.append(tgui::to_string(scrollbars[widgetIndex].verticalScroll.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position       = ";
            line.append("(").append(tgui::to_string(scrollbars[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(scrollbars[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size           = ";
            line.append("(").append(tgui::to_string(scrollbars[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(scrollbars[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "LowValue       = ";
            line.append(tgui::to_string(scrollbars[widgetIndex].lowValue.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Maximum        = ";
            line.append(tgui::to_string(scrollbars[widgetIndex].maximum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Value          = ";
            line.append(tgui::to_string(scrollbars[widgetIndex].value.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID     = ";
            line.append(tgui::to_string(scrollbars[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_LoadingBar)
        {
            line = TAB "LoadingBar: \"";
            line.append(loadingBars[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile = \"";
            line.append(loadingBars[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position   = ";
            line.append("(").append(tgui::to_string(loadingBars[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(loadingBars[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size       = ";
            line.append("(").append(tgui::to_string(loadingBars[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(loadingBars[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Minimum    = ";
            line.append(tgui::to_string(loadingBars[widgetIndex].minimum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Maximum    = ";
            line.append(tgui::to_string(loadingBars[widgetIndex].maximum.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Value      = ";
            line.append(tgui::to_string(loadingBars[widgetIndex].value.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID = ";
            line.append(tgui::to_string(loadingBars[widgetIndex].callbackID.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB "}\n\n";
            fwrite(line.c_str(), 1, line.size(), pFile);
        }
        else if (widgetID == tgui::Type_TextBox)
        {
            line = TAB "TextBox: \"";
            line.append(textBoxes[widgetIndex].name.value).append("\"\n" TAB "{\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "ConfigFile          = \"";
            line.append(textBoxes[widgetIndex].configFile.value).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Position            = ";
            line.append("(").append(tgui::to_string(textBoxes[widgetIndex].left.value)).append(",");
            line.append(tgui::to_string(textBoxes[widgetIndex].top.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Size                = ";
            line.append("(").append(tgui::to_string(textBoxes[widgetIndex].width.value)).append(",");
            line.append(tgui::to_string(textBoxes[widgetIndex].height.value)).append(")").append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            // Replace the newlines, tabs and quotes
            std::string text = textBoxes[widgetIndex].text.value;
            std::string::size_type pos = 0;
            while ((pos = text.find("\n", pos)) != std::string::npos)
                text.replace(pos, 1, "\\n");
            pos = 0;
            while ((pos = text.find("\t", pos)) != std::string::npos)
                text.replace(pos, 1, "\\t");
            pos = 0;
            while ((pos = text.find("\"", pos)) != std::string::npos)
                text.replace(pos, 1, "\\\"");

            line = TAB TAB "Text                = \"";
            line.append(text).append("\"\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextSize            = ";
            line.append(tgui::to_string(textBoxes[widgetIndex].textSize.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "MaximumCharacters   = ";
            line.append(tgui::to_string(textBoxes[widgetIndex].maximumCharacters.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "Borders             = ";
            AddBrackets(textBoxes[widgetIndex].borders.value)
            line.append(textBoxes[widgetIndex].borders.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BackgroundColor     = ";
            AddBrackets(textBoxes[widgetIndex].backgroundColor.value)
            line.append(textBoxes[widgetIndex].backgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "TextColor           = ";
            AddBrackets(textBoxes[widgetIndex].textColor.value)
            line.append(textBoxes[widgetIndex].textColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextColor   = ";
            AddBrackets(textBoxes[widgetIndex].selectedTextColor.value)
            line.append(textBoxes[widgetIndex].selectedTextColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectedTextBackgroundColor = ";
            AddBrackets(textBoxes[widgetIndex].selectedTextBackgroundColor.value)
            line.append(textBoxes[widgetIndex].selectedTextBackgroundColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "BorderColor         = ";
            AddBrackets(textBoxes[widgetIndex].borderColor.value)
            line.append(textBoxes[widgetIndex].borderColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectionPointColor = ";
            AddBrackets(textBoxes[widgetIndex].selectionPointColor.value)
            line.append(textBoxes[widgetIndex].selectionPointColor.value).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "SelectionPointWidth = ";
            line.append(tgui::to_string(textBoxes[widgetIndex].selectionPointWidth.value)).append("\n");
            fwrite(line.c_str(), 1, line.size(), pFile);

            line = TAB TAB "CallbackID          = ";
            line.append(tgui::to_string(textBoxes[widgetIndex].callbackID.value)).append("\n");
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

