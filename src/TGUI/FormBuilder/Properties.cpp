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

#define addProperty_String(property) \
void Property##property::addProperty(tgui::Window& window, unsigned int propertyNumber) \
{ \
    tgui::Label* label = window.add<tgui::Label>("label_"#property); \
    label->setText(description); \
    label->setTextColor(sf::Color::Black); \
    label->setPosition(10, 14 + (40 * propertyNumber)); \
    label->setTextSize(26); \
 \
    tgui::EditBox* editBox = window.add<tgui::EditBox>("text_"#property); \
    editBox->load("images/objects/EditBox/" OBJECT_STYLE); \
    editBox->setBorders(6, 4, 6, 4); \
    editBox->setPosition(window.getSize().x / 2.f, static_cast<float>(10 + (40 * propertyNumber))); \
    editBox->setSize(window.getSize().x / 2.0f - 10, 32); \
    editBox->setText(value); \
    editBox->callbackID = propertyNumber + 1; \
}

#define addProperty_Value(property) \
void Property##property::addProperty(tgui::Window& window, unsigned int propertyNumber) \
{ \
    tgui::Label* label = window.add<tgui::Label>("label_"#property); \
    label->setText(description); \
    label->setTextColor(sf::Color::Black); \
    label->setPosition(10, 14 + (40 * propertyNumber)); \
    label->setTextSize(26); \
 \
    tgui::EditBox* editBox = window.add<tgui::EditBox>("text_"#property); \
    editBox->load("images/objects/EditBox/" OBJECT_STYLE); \
    editBox->setBorders(6, 4, 6, 4); \
    editBox->setPosition(window.getSize().x / 2.f, static_cast<float>(10 + (40 * propertyNumber))); \
    editBox->setSize(window.getSize().x / 2.0f - 10, 32); \
    editBox->setText(tgui::to_string(value)); \
    editBox->callbackID = propertyNumber + 1; \
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool checkIfFileExists(const char* filename)
{
    std::ifstream ifile(filename);
    return ifile.good();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyName::PropertyName()
{
    description = "Name";
    value = "";
}

addProperty_String(Name)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyFilename::PropertyFilename()
{
    description = "Filename";
    value = "";
}

addProperty_String(Filename)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyPathname::PropertyPathname()
{
    description = "Pathname";
    value = "";
}

addProperty_String(Pathname)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyLeft::PropertyLeft()
{
    description = "Left";
    value = 0;
}

addProperty_Value(Left)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyTop::PropertyTop()
{
    description = "Top";
    value = 0;
}

addProperty_Value(Top)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyWindowWidth::PropertyWindowWidth()
{
    description = "Width";
    value = 0;
}

addProperty_Value(WindowWidth)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyWindowHeight::PropertyWindowHeight()
{
    description = "Height";
    value = 0;
}

addProperty_Value(WindowHeight)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyWidth::PropertyWidth()
{
    description = "Width";
    value = 0;
}

addProperty_Value(Width)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyHeight::PropertyHeight()
{
    description = "Height";
    value = 0;
}

addProperty_Value(Height)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyText::PropertyText()
{
    description = "Text";
    value = "";
}

addProperty_String(Text)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyTextSize::PropertyTextSize()
{
    description = "Text size";
    value = 0;
}

addProperty_Value(TextSize)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyTextColor::PropertyTextColor()
{
    description = "Text color";
    value = "255,255,255";
}

addProperty_String(TextColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertySelectedTextColor::PropertySelectedTextColor()
{
    description = "Selected text color";
    value = "255,255,255";
}

addProperty_String(SelectedTextColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertySelectedTextBackgroundColor::PropertySelectedTextBackgroundColor()
{
    description = "Selected text background color";
    value = "255,255,255";
}

addProperty_String(SelectedTextBackgroundColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyUnfocusedSelectedTextBackgroundColor::PropertyUnfocusedSelectedTextBackgroundColor()
{
    description = "Unfocused selected text background color";
    value = "255,255,255";
}

addProperty_String(UnfocusedSelectedTextBackgroundColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyTextFont::PropertyTextFont()
{
    description = "Text font";
    value = "Global";
}

addProperty_String(TextFont)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyGlobalFont::PropertyGlobalFont()
{
    description = "Global font";
    value = "Default";
}

addProperty_String(GlobalFont)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyCallbackID::PropertyCallbackID()
{
    description = "Callback id";
    value = 0;
}

addProperty_Value(CallbackID)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyChecked::PropertyChecked()
{
    description = "Checked";
    value = 0;
}

void PropertyChecked::addProperty(tgui::Window& window, unsigned int propertyNumber)
{
    // Create the label
    tgui::Label* label = window.add<tgui::Label>("label_Checked");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40 * propertyNumber));
    label->setTextSize(26);

    // Create the combo box
    tgui::ComboBox* comboBox = window.add<tgui::ComboBox>("combo_Checked");
    comboBox->load("images/objects/ComboBox/" OBJECT_STYLE, static_cast<unsigned int>(window.getSize().x / 2.f - 10), 40);
    comboBox->setPosition(window.getSize().x / 2.f, static_cast<float>(10 + (40 * propertyNumber)));
    comboBox->setSize(window.getSize().x / 2.0f - 10, 32);
    comboBox->callbackID = propertyNumber + 1;

    // Change the colors of the combo box
    comboBox->changeColors(sf::Color( 50,  50,  50),
                           sf::Color(200, 200, 200),
                           sf::Color( 10, 110, 255),
                           sf::Color(255, 255, 255));

    // Change the contents of the combo box
    comboBox->addItem("false");
    comboBox->addItem("true");
    comboBox->setSelectedItem(value + 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyPasswordChar::PropertyPasswordChar()
{
    description = "Password char";
    value = 0;
}

void PropertyPasswordChar::addProperty(tgui::Window& window, unsigned int propertyNumber)
{
    tgui::Label* label = window.add<tgui::Label>("label_PasswordChar");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40 * propertyNumber));
    label->setTextSize(26);

    tgui::EditBox* editBox = window.add<tgui::EditBox>("text_PasswordChar");
    editBox->load("images/objects/EditBox/" OBJECT_STYLE);
    editBox->setBorders(6, 4, 6, 4);
    editBox->setPosition(window.getSize().x / 2.f, static_cast<float>(10 + (40 * propertyNumber)));
    editBox->setSize(window.getSize().x / 2.0f - 10, 32);
    editBox->setMaximumCharacters(1);
    editBox->callbackID = propertyNumber + 1;

    if (value != 0)
        editBox->setText(std::string(1, value));
    else
        editBox->setText("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyMaximumCharacters::PropertyMaximumCharacters()
{
    description = "Maximum characters";
    value = 0;
}

addProperty_Value(MaximumCharacters)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyBorders::PropertyBorders()
{
    description = "Borders";
    value = "0,0,0,0";
}

addProperty_String(Borders)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertySelectionPointColor::PropertySelectionPointColor()
{
    description = "Selection point color";
    value = "255,255,255";
}

addProperty_String(SelectionPointColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertySelectionPointWidth::PropertySelectionPointWidth()
{
    description = "Selection point width";
    value = 2;
}

addProperty_Value(SelectionPointWidth)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyScrollbarPathname::PropertyScrollbarPathname()
{
    description = "Scrollbar pathname";
    value = "items/objects/Scrollbar/" OBJECT_STYLE;
}

addProperty_String(ScrollbarPathname)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyItemHeight::PropertyItemHeight()
{
    description = "Item height";
    value = 10;
}

addProperty_Value(ItemHeight)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyMaximumItems::PropertyMaximumItems()
{
    description = "Maximum items";
    value = 0;
}

addProperty_Value(MaximumItems)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyBackgroundColor::PropertyBackgroundColor()
{
    description = "Background color";
    value = "255,255,255";
}

addProperty_String(BackgroundColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertySelectedBackgroundColor::PropertySelectedBackgroundColor()
{
    description = "Selected background color";
    value = "50,100,200";
}

addProperty_String(SelectedBackgroundColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyItems::PropertyItems()
{
    description = "Items";
    value = "";
}

addProperty_String(Items)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertySelectedItem::PropertySelectedItem()
{
    description = "Selected item";
    value = 0;
}

addProperty_Value(SelectedItem)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyItemsToDisplay::PropertyItemsToDisplay()
{
    description = "Items to display";
    value = 10;
}

addProperty_Value(ItemsToDisplay)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyBorderColor::PropertyBorderColor()
{
    description = "Border color";
    value = "0,0,0";
}

addProperty_String(BorderColor)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyValue::PropertyValue()
{
    description = "Value";
    value = 0;
}

addProperty_Value(Value)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyLowValue::PropertyLowValue()
{
    description = "Low value";
    value = 0;
}

addProperty_Value(LowValue)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyMinimum::PropertyMinimum()
{
    description = "Minimum";
    value = 0;
}

addProperty_Value(Minimum)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyMaximum::PropertyMaximum()
{
    description = "Maximum";
    value = 100;
}

addProperty_Value(Maximum)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyVerticalScroll::PropertyVerticalScroll()
{
    description = "Vertical scroll";
    value = false;
}

void PropertyVerticalScroll::addProperty(tgui::Window& window, unsigned int propertyNumber)
{
    // Create the label
    tgui::Label* label = window.add<tgui::Label>("label_VerticalScroll");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40 * propertyNumber));
    label->setTextSize(26);

    // Create the combo box
    tgui::ComboBox* comboBox = window.add<tgui::ComboBox>("combo_VerticalScroll");
    comboBox->load("images/objects/ComboBox/" OBJECT_STYLE, static_cast<unsigned int>(window.getSize().x / 2.0f - 10), 40);
    comboBox->setPosition(window.getSize().x / 2.f, static_cast<float>(10 + (40 * propertyNumber)));
    comboBox->setSize(window.getSize().x / 2.0f - 10, 32);
    comboBox->callbackID = propertyNumber + 1;

    // Change the colors of the combo box
    comboBox->changeColors(sf::Color( 50,  50,  50),
                           sf::Color(200, 200, 200),
                           sf::Color( 10, 110, 255),
                           sf::Color(255, 255, 255));

    // Change the contents of the combo box
    comboBox->addItem("false");
    comboBox->addItem("true");
    comboBox->setSelectedItem(value + 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesWindow::PropertiesWindow()
{
    id = 0;
    name.value = "";
    width.value = 1024;
    height.value = 768;
    globalFont.value = "Default";
    builder = NULL;
}

void PropertiesWindow::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Window_Name);
    width.addProperty(window, Property_Window_Width);
    height.addProperty(window, Property_Window_Height);
    globalFont.addProperty(window, Property_Window_GlobalFont);
}

void PropertiesWindow::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Window_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Window_Width)
    {
        // Get the width of the window
        unsigned int windowWidth = atoi(propertyWindow.get<tgui::EditBox>("text_WindowWidth")->getText().c_str());
        width.value = windowWidth;

        // There is a minimum size
        if (windowWidth < 32)
            windowWidth = 32;

        // Set the window size
        formWindow.setSize(sf::Vector2u(windowWidth, formWindow.getSize().y));
    }
    else if (propertyNumber == Property_Window_Height)
    {
        // Get the height of the window
        unsigned int windowHeight = atoi(propertyWindow.get<tgui::EditBox>("text_WindowHeight")->getText().c_str());
        height.value = windowHeight;

        // There is a minimum size
        if (windowHeight < 32)
            windowHeight = 32;

        // Set the window size
        formWindow.setSize(sf::Vector2u(formWindow.getSize().x, windowHeight));
    }
    else if (propertyNumber == Property_Window_GlobalFont)
    {
/*
        globalFont.value = propertyWindow.get<tgui::EditBox>("text_GlobalFont")->getText();

        std::string fontName;

        // If the font is not recognised then use the default font
        if (globalFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
            fontName = "Default";
        else
            fontName = globalFont.value;

        // Change the global font for all new objects
        if (fontName.compare("Default"))
            formWindow.globalFont = sf::Font::getDefaultFont();

        // Change the global font for the existing objects
        builder->setGlobalFont(formWindow.globalFont);
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_GlobalFont")->setText("Default");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesPicture::PropertiesPicture()
{
    id = 0;
    name.value = "";
    filename.value = "images/objects/Picture.png";
    left.value = 0;
    top.value = 0;
    width.value = 200;
    height.value = 200;
    callbackID.value = 0;

    loadedDefaultPicture = true;
}

void PropertiesPicture::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Picture_Name);
    filename.addProperty(window, Property_Picture_Filename);
    left.addProperty(window, Property_Picture_Left);
    top.addProperty(window, Property_Picture_Top);
    width.addProperty(window, Property_Picture_Width);
    height.addProperty(window, Property_Picture_Height);
    callbackID.addProperty(window, Property_Picture_CallbackID);
}

void PropertiesPicture::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Picture_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Picture_Filename)
    {
        filename.value = propertyWindow.get<tgui::EditBox>("text_Filename")->getText();

        // Check if the file exists
        if (checkIfFileExists(filename.value.c_str()))
        {
            // The default picture is no longer used
            loadedDefaultPicture = false;

            // Load the new picture
            tgui::Picture* picture = formWindow.get<tgui::Picture>(tgui::to_string(id));
            picture->load(filename.value);

            // Reset the scaling
            picture->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(picture->getSize().x);
            height.value = static_cast<float>(picture->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default picture is not loaded already then load it now
            if (loadedDefaultPicture == false)
            {
                // The default picture is loaded again
                loadedDefaultPicture = true;

                // Load the default picture
                tgui::Picture* picture = formWindow.get<tgui::Picture>(tgui::to_string(id));
                picture->load("images/objects/Picture.png");

                // Adjust the scaling
                picture->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Picture_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the picture
        formWindow.get<tgui::Picture>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Picture_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the picture
        formWindow.get<tgui::Picture>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Picture_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the picture
        formWindow.get<tgui::Picture>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Picture_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the picture
        formWindow.get<tgui::Picture>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Picture_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Picture>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesButton::PropertiesButton()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/Button/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 256;
    height.value = 64;
    text.value = "";
    textSize.value = 0;
    textColor.value = "200,200,200";
    textFont.value = "Global";
    callbackID.value = 0;

    loadedDefaultButton = true;
}

void PropertiesButton::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Button_Name);
    pathname.addProperty(window, Property_Button_Pathname);
    left.addProperty(window, Property_Button_Left);
    top.addProperty(window, Property_Button_Top);
    width.addProperty(window, Property_Button_Width);
    height.addProperty(window, Property_Button_Height);
    text.addProperty(window, Property_Button_Text);
    textSize.addProperty(window, Property_Button_TextSize);
    textColor.addProperty(window, Property_Button_TextColor);
    textFont.addProperty(window, Property_Button_TextFont);
    callbackID.addProperty(window, Property_Button_CallbackID);
}

void PropertiesButton::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Button_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Button_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default button is no longer used
            loadedDefaultButton = false;

            // Load the new button
            tgui::Button* button = formWindow.get<tgui::Button>(tgui::to_string(id));
            button->load(pathname.value);

            // Reset the scaling
            button->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(button->getSize().x);
            height.value = static_cast<float>(button->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default button is not loaded already then load it now
            if (loadedDefaultButton == false)
            {
                // The default button is loaded again
                loadedDefaultButton = true;

                // Load the default button
                tgui::Button* button = formWindow.get<tgui::Button>(tgui::to_string(id));
                button->load("images/objects/Button/" OBJECT_STYLE);

                // Adjust the scaling
                button->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Button_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the button
        formWindow.get<tgui::Button>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Button_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the button
        formWindow.get<tgui::Button>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Button_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the button
        formWindow.get<tgui::Button>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Button_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the button
        formWindow.get<tgui::Button>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Button_Text)
    {
        // Store the new text
        text.value = propertyWindow.get<tgui::EditBox>("text_Text")->getText();

        // Change the text of the button
        formWindow.get<tgui::Button>(tgui::to_string(id))->setText(text.value);
    }
    else if (propertyNumber == Property_Button_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(propertyWindow.get<tgui::EditBox>("text_TextSize")->getText().c_str());

        // Change the text of the button
        formWindow.get<tgui::Button>(tgui::to_string(id))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_Button_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            formWindow.get<tgui::Button>(tgui::to_string(id))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            formWindow.get<tgui::Button>(tgui::to_string(id))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Button_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::Button>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::Button>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_Button_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Button>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesCheckbox::PropertiesCheckbox()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/Checkbox/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 32;
    height.value = 32;
    text.value = "";
    textSize.value = 0;
    textColor.value = "200,200,200";
    textFont.value = "Global";
    checked.value = 0;
    callbackID.value = 0;

    loadedDefaultCheckbox = true;
}

void PropertiesCheckbox::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Checkbox_Name);
    pathname.addProperty(window, Property_Checkbox_Pathname);
    left.addProperty(window, Property_Checkbox_Left);
    top.addProperty(window, Property_Checkbox_Top);
    width.addProperty(window, Property_Checkbox_Width);
    height.addProperty(window, Property_Checkbox_Height);
    text.addProperty(window, Property_Checkbox_Text);
    textSize.addProperty(window, Property_Checkbox_TextSize);
    textColor.addProperty(window, Property_Checkbox_TextColor);
    textFont.addProperty(window, Property_Checkbox_TextFont);
    callbackID.addProperty(window, Property_Checkbox_CallbackID);

    checked.addProperty(window, Property_Checkbox_Checked);
}

void PropertiesCheckbox::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Checkbox_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Checkbox_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default checkbox is no longer used
            loadedDefaultCheckbox = false;

            // Load the new checkbox
            tgui::Checkbox* checkbox = formWindow.get<tgui::Checkbox>(tgui::to_string(id));
            checkbox->load(pathname.value);

            // Reset the scaling
            checkbox->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(checkbox->getSize().x);
            height.value = static_cast<float>(checkbox->getSize().y);


            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default checkbox is not loaded already then load it now
            if (loadedDefaultCheckbox == false)
            {
                // The default checkbox is loaded again
                loadedDefaultCheckbox = true;

                // Load the default checkbox
                tgui::Checkbox* checkbox = formWindow.get<tgui::Checkbox>(tgui::to_string(id));
                checkbox->load("images/objects/Checkbox/" OBJECT_STYLE);

                // Adjust the scaling
                checkbox->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Checkbox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the checkbox
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the checkbox
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the checkbox
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the checkbox
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Checked)
    {
        // Change the value of the combo box
        checked.value = propertyWindow.get<tgui::ComboBox>("combo_Checked")->getSelectedItemID() - 1;

        // Change the checkbox image
        if (checked.value)
            formWindow.get<tgui::Checkbox>(tgui::to_string(id))->check();
        else
            formWindow.get<tgui::Checkbox>(tgui::to_string(id))->uncheck();
    }
    else if (propertyNumber == Property_Checkbox_Text)
    {
        // Store the new text
        text.value = propertyWindow.get<tgui::EditBox>("text_Text")->getText();

        // Change the text of the checkbox
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setText(text.value);
    }
    else if (propertyNumber == Property_Checkbox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(propertyWindow.get<tgui::EditBox>("text_TextSize")->getText().c_str());

        // Change the text of the checkbox
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_Checkbox_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Checkbox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::Checkbox>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_Checkbox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Checkbox>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesRadioButton::PropertiesRadioButton()
{
    pathname.value = "images/objects/RadioButton/" OBJECT_STYLE;
    builder = NULL;
}

void PropertiesRadioButton::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Checkbox_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Checkbox_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default radio button is no longer used
            loadedDefaultCheckbox = false;

            // Load the new radio button
            tgui::RadioButton* radioButton = formWindow.get<tgui::RadioButton>(tgui::to_string(id));
            radioButton->load(pathname.value);

            // Reset the scaling
            radioButton->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(radioButton->getSize().x);
            height.value = static_cast<float>(radioButton->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default radio button is not loaded already then load it now
            if (loadedDefaultCheckbox == false)
            {
                // The default radio button is loaded again
                loadedDefaultCheckbox = true;

                // Load the default radio button
                tgui::RadioButton* radioButton = formWindow.get<tgui::RadioButton>(tgui::to_string(id));
                radioButton->load("images/objects/RadioButton/" OBJECT_STYLE);

                // Adjust the scaling
                radioButton->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Checkbox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the radio button
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the radio button
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the radio button
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the radio button
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Checked)
    {
        // Turn the checked properties of all radio buttons to false
        for (unsigned int i=0; i<builder->radioButtons.size(); ++i)
            builder->radioButtons[i].checked.value = false;

        // Change the checked flag
        checked.value = propertyWindow.get<tgui::ComboBox>("combo_Checked")->getSelectedItemID() - 1;

        // Change the radio button image
        if (checked.value)
            formWindow.get<tgui::RadioButton>(tgui::to_string(id))->check();
        else
            formWindow.uncheckRadioButtons();
    }
    else if (propertyNumber == Property_Checkbox_Text)
    {
        // Store the new text
        text.value = propertyWindow.get<tgui::EditBox>("text_Text")->getText();

        // Change the text of the radio button
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setText(text.value);
    }
    else if (propertyNumber == Property_Checkbox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(propertyWindow.get<tgui::EditBox>("text_TextSize")->getText().c_str());

        // Change the text of the radio button
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_Checkbox_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Checkbox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::RadioButton>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_Checkbox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::RadioButton>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesLabel::PropertiesLabel()
{
    id = 0;
    name.value = "";
    left.value = 0;
    top.value = 0;
    width.value = 72;
    height.value = 31;
    text.value = "Label";
    textSize.value = 30;
    textColor.value = "255,255,255";
    textFont.value = "Global";
    callbackID.value = 0;
}

void PropertiesLabel::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Label_Name);
    left.addProperty(window, Property_Label_Left);
    top.addProperty(window, Property_Label_Top);
    width.addProperty(window, Property_Label_Width);
    height.addProperty(window, Property_Label_Height);
    text.addProperty(window, Property_Label_Text);
    textSize.addProperty(window, Property_Label_TextSize);
    textColor.addProperty(window, Property_Label_TextColor);
    textFont.addProperty(window, Property_Label_TextFont);
    callbackID.addProperty(window, Property_Label_CallbackID);
}

void PropertiesLabel::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Label_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Label_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the label
        formWindow.get<tgui::Label>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Label_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the label
        formWindow.get<tgui::Label>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Label_Width)
    {
        // Store the new size
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Get the pointer to the label
        tgui::Label* label = formWindow.get<tgui::Label>(tgui::to_string(id));

        // Change the size of the label
        label->setSize(width.value, height.value);

        // Adjust the text size property
        textSize.value = label->getTextSize();
        propertyWindow.get<tgui::EditBox>("text_TextSize")->setText(tgui::to_string(textSize.value));
    }
    else if (propertyNumber == Property_Label_Height)
    {
        // Store the new size
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Get the pointer to the label
        tgui::Label* label = formWindow.get<tgui::Label>(tgui::to_string(id));

        // Change the size of the label
        label->setSize(width.value, height.value);

        // Adjust the text size property
        textSize.value = label->getTextSize();
        propertyWindow.get<tgui::EditBox>("text_TextSize")->setText(tgui::to_string(textSize.value));
    }
    else if (propertyNumber == Property_Label_Text)
    {
        // Store the new text
        text.value = propertyWindow.get<tgui::EditBox>("text_Text")->getText();

        // Get the pointer to the label
        tgui::Label* label = formWindow.get<tgui::Label>(tgui::to_string(id));

        // Change the text of the label
        if (text.value.size())
            label->setText(text.value);
        else
            label->setText("#empty#");

        // Adjust the width of the label
        width.value = static_cast<float>(label->getSize().x);
        propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));

        // Adjust the height of the label
        height.value = static_cast<float>(label->getSize().y);
        propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
    }
    else if (propertyNumber == Property_Label_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(propertyWindow.get<tgui::EditBox>("text_TextSize")->getText().c_str());

        // Get the pointer to the label
        tgui::Label* label = formWindow.get<tgui::Label>(tgui::to_string(id));

        // Change the text of the label
        label->setTextSize(textSize.value);

        // Adjust the width of the label
        width.value = static_cast<float>(label->getSize().x);
        propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));

        // Adjust the height of the label
        height.value = static_cast<float>(label->getSize().y);
        propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
    }
    else if (propertyNumber == Property_Label_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            formWindow.get<tgui::Label>(tgui::to_string(id))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            formWindow.get<tgui::Label>(tgui::to_string(id))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Label_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::Label>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::Label>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_Label_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Label>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesEditBox::PropertiesEditBox()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/EditBox/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 120;
    height.value = 40;
    text.value = "";
    textSize.value = 0;
    textFont.value = "Global";
    passwordChar.value = 0;
    maximumCharacters.value = 0;
    borders.value = "6, 4, 6, 4";
    textColor.value = "200, 200, 200";
    selectedTextColor.value = "255, 255, 255";
    selectedTextBackgroundColor.value = "10, 110, 255";
    unfocusedSelectedTextBackgroundColor.value = "110, 110, 255";
    selectionPointColor.value = "110, 110, 255";
    selectionPointWidth.value = 2;
    callbackID.value = 0;

    loadedDefaultEditBox = true;
}

void PropertiesEditBox::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_EditBox_Name);
    pathname.addProperty(window, Property_EditBox_Pathname);
    left.addProperty(window, Property_EditBox_Left);
    top.addProperty(window, Property_EditBox_Top);
    width.addProperty(window, Property_EditBox_Width);
    height.addProperty(window, Property_EditBox_Height);
    text.addProperty(window, Property_EditBox_Text);
    textSize.addProperty(window, Property_EditBox_TextSize);
    textFont.addProperty(window, Property_EditBox_TextFont);
    passwordChar.addProperty(window, Property_EditBox_PasswordChar);
    maximumCharacters.addProperty(window, Property_EditBox_MaximumCharacters);
    borders.addProperty(window, Property_EditBox_Borders);
    textColor.addProperty(window, Property_EditBox_TextColor);
    selectedTextColor.addProperty(window, Property_EditBox_SelectedTextColor);
    selectedTextBackgroundColor.addProperty(window, Property_EditBox_SelectedTextBackgroundColor);
    unfocusedSelectedTextBackgroundColor.addProperty(window, Property_EditBox_UnfocusedSelectedTextBackgroundColor);
    selectionPointColor.addProperty(window, Property_EditBox_SelectionPointColor);
    selectionPointWidth.addProperty(window, Property_EditBox_SelectionPointWidth);
    callbackID.addProperty(window, Property_EditBox_CallbackID);
}

void PropertiesEditBox::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_EditBox_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_EditBox_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default edit box is no longer used
            loadedDefaultEditBox = false;

            // Load the new edit box
            tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));
            editBox->load(pathname.value);

            // Reset the scaling
            editBox->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(editBox->getSize().x);
            height.value = static_cast<float>(editBox->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default edit box is not loaded already then load it now
            if (loadedDefaultEditBox == false)
            {
                // The default edit box is loaded again
                loadedDefaultEditBox = true;

                // Load the default edit box
                tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));
                editBox->load("images/objects/EditBox/" OBJECT_STYLE);

                // Adjust the scaling
                editBox->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_EditBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the edit box
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_EditBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the edit box
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_EditBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the edit box
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_EditBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the edit box
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_EditBox_Text)
    {
        // Store the new text
        text.value = propertyWindow.get<tgui::EditBox>("text_Text")->getText();

        // Change the text of the edit box
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setText(text.value);
    }
    else if (propertyNumber == Property_EditBox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(propertyWindow.get<tgui::EditBox>("text_TextSize")->getText().c_str());

        // Change the text of the edit box
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_EditBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::EditBox>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::EditBox>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_EditBox_PasswordChar)
    {
        // Get the text inside the edit box
        std::string property = propertyWindow.get<tgui::EditBox>("text_PasswordChar")->getText();

        // Store the new value
        if (property.size() == 0)
            passwordChar.value = 0;
        else
            passwordChar.value = property[0];

        // Change the password char
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setPasswordChar(passwordChar.value);
    }
    else if (propertyNumber == Property_EditBox_MaximumCharacters)
    {
        // Store the new maximum
        maximumCharacters.value = atoi(propertyWindow.get<tgui::EditBox>("text_MaximumCharacters")->getText().c_str());

        // Set the maximum characters
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setMaximumCharacters(maximumCharacters.value);
        propertyWindow.get<tgui::EditBox>("text_Text")->setMaximumCharacters(maximumCharacters.value);
    }
    else if (propertyNumber == Property_EditBox_Borders)
    {
        // Store the new borders
        borders.value = propertyWindow.get<tgui::EditBox>("text_Borders")->getText();

        std::string strBorders = borders.value;

        // If there is no bracket at the beginning then add one
        if (strBorders[0] != '(')
            strBorders.insert(strBorders.begin(), '(');

        // If there is no bracket at the end then add one
        if (strBorders[strBorders.length()-1] != ')')
            strBorders.insert(strBorders.end(), ')');

        // Get the border size
        tgui::Vector4u border;
        tgui::extractVector4u(strBorders, border);

        // Apply the borders
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_EditBox_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Get the pointer to the edit box
        tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            editBox->setTextColor(color);
        }
        else // The string is empty, so use the default color
            editBox->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_EditBox_SelectedTextColor)
    {
        // Store the new color
        selectedTextColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedTextColor")->getText();

        // Get the pointer to the edit box
        tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedTextColor.value.empty() == false)
        {
            std::string strColor = selectedTextColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            editBox->setSelectedTextColor(color);
        }
        else // The string is empty, so use the default color
            editBox->setSelectedTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_EditBox_SelectedTextBackgroundColor)
    {
        // Store the new color
        selectedTextBackgroundColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedTextBackgroundColor")->getText();

        // Get the pointer to the edit box
        tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedTextBackgroundColor.value.empty() == false)
        {
            std::string strColor = selectedTextBackgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            editBox->setSelectedTextBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            editBox->setSelectedTextBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_EditBox_UnfocusedSelectedTextBackgroundColor)
    {
        // Store the new color
        unfocusedSelectedTextBackgroundColor.value = propertyWindow.get<tgui::EditBox>("text_UnfocusedSelectedTextBackgroundColor")->getText();

        // Get the pointer to the edit box
        tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (unfocusedSelectedTextBackgroundColor.value.empty() == false)
        {
            std::string strColor = unfocusedSelectedTextBackgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            editBox->setUnfocusedSelectedTextBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            editBox->setUnfocusedSelectedTextBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_EditBox_SelectionPointColor)
    {
        // Store the new color
        selectionPointColor.value = propertyWindow.get<tgui::EditBox>("text_SelectionPointColor")->getText();

        // Get the pointer to the edit box
        tgui::EditBox* editBox = formWindow.get<tgui::EditBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectionPointColor.value.empty() == false)
        {
            std::string strColor = selectionPointColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            editBox->selectionPointColor = color;
        }
        else // The string is empty, so use the default color
            editBox->selectionPointColor = sf::Color::White;

    }
    else if (propertyNumber == Property_EditBox_SelectionPointWidth)
    {
        // Store the new width
        selectionPointWidth.value = atoi(propertyWindow.get<tgui::EditBox>("text_SelectionPointWidth")->getText().c_str());

        // Set the new width
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->selectionPointWidth = selectionPointWidth.value;
    }
    else if (propertyNumber == Property_EditBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::EditBox>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesListbox::PropertiesListbox()
{
    id = 0;
    name.value = "";
    left.value = 0;
    top.value = 0;
    width.value = 200;
    height.value = 240;
    scrollbarPathname.value = "images/objects/Scrollbar/" OBJECT_STYLE;
    itemHeight.value = 25;
    maximumItems.value = 0;
    borders.value = "2, 2, 2, 2";
    backgroundColor.value = "50, 50, 50";
    textColor.value = "200, 200, 200";
    selectedBackgroundColor.value = "10, 110, 255";
    selectedTextColor.value = "255, 255, 255";
    borderColor.value = "0, 0, 0";
    items.value = "";
    selectedItem.value = 0;
    textFont.value = "Global";
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesListbox::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Listbox_Name);
    left.addProperty(window, Property_Listbox_Left);
    top.addProperty(window, Property_Listbox_Top);
    width.addProperty(window, Property_Listbox_Width);
    height.addProperty(window, Property_Listbox_Height);
    scrollbarPathname.addProperty(window, Property_Listbox_ScrollbarPathname);
    itemHeight.addProperty(window, Property_Listbox_ItemHeight);
    maximumItems.addProperty(window, Property_Listbox_MaximumItems);
    borders.addProperty(window, Property_Listbox_Borders);
    backgroundColor.addProperty(window, Property_Listbox_BackgroundColor);
    textColor.addProperty(window, Property_Listbox_TextColor);
    selectedBackgroundColor.addProperty(window, Property_Listbox_SelectedBackgroundColor);
    selectedTextColor.addProperty(window, Property_Listbox_SelectedTextColor);
    borderColor.addProperty(window, Property_Listbox_BorderColor);
    items.addProperty(window, Property_Listbox_Items);
    selectedItem.addProperty(window, Property_Listbox_SelectedItem);
    textFont.addProperty(window, Property_Listbox_TextFont);
    callbackID.addProperty(window, Property_Listbox_CallbackID);
}

void PropertiesListbox::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Listbox_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Listbox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Listbox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Listbox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the size of the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Listbox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the size of the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Listbox_ScrollbarPathname)
    {
        scrollbarPathname.value = propertyWindow.get<tgui::EditBox>("text_ScrollbarPathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((scrollbarPathname.value + "/info.txt").c_str()))
        {
            // The default scrollbar is no longer used
            loadedDefaultScrollbar = false;

            // Load the new scrollbar
            formWindow.get<tgui::Listbox>(tgui::to_string(id))->setScrollbar(scrollbarPathname.value);
        }
        else // The file didn't exist
        {
            // When the default scrollbar is not loaded already then load it now
            if (loadedDefaultScrollbar == false)
            {
                // The default scrollbar is loaded again
                loadedDefaultScrollbar = true;

                // Load the default scrollbar
                formWindow.get<tgui::Listbox>(tgui::to_string(id))->setScrollbar("images/objects/Scrollbar/" OBJECT_STYLE);
            }
        }
    }
    else if (propertyNumber == Property_Listbox_ItemHeight)
    {
        // Store the new item height
        itemHeight.value = atoi(propertyWindow.get<tgui::EditBox>("text_ItemHeight")->getText().c_str());

        // Change the item height in the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setItemHeight(itemHeight.value);
    }
    else if (propertyNumber == Property_Listbox_MaximumItems)
    {
        // Store the new maximum
        maximumItems.value = atoi(propertyWindow.get<tgui::EditBox>("text_MaximumItems")->getText().c_str());

        // Change the maximum items in the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setMaximumItems(maximumItems.value);

        // If there is a maximum, then make sure that there aren't too many items
        if (maximumItems.value > 0)
        {
            std::string strItems = items.value;
            bool commaFound = true;
            std::string::size_type commaPos;
            unsigned int count = 0;
            std::string::size_type removedCharacters = 0;

            // Loop all items
            while (commaFound)
            {
                // Search for the next comma
                commaPos = strItems.find(',');

                // Check if a comma was found
                if (commaPos != std::string::npos)
                {
                    // Erase the item from the string
                    strItems.erase(0, commaPos + 1);

                    // Characters have been removed
                    removedCharacters += commaPos + 1;

                    // Another item was found
                    ++count;
                }
                else // No comma was found
                {
                    // Another item was found (when the string isn't empty)
                    if (strItems.size() > 0)
                        ++count;

                    // Stop the loop
                    commaFound = false;
                }

                // Check if the amount of items is allowed
                if (count > maximumItems.value)
                {
                    // Delete a part of the text
                    items.value.erase(removedCharacters - 1);

                    // Update the property
                    propertyWindow.get<tgui::EditBox>("text_Items")->setText(items.value);
                }
            }
        }
    }
    else if (propertyNumber == Property_Listbox_Borders)
    {
        // Store the new borders
        borders.value = propertyWindow.get<tgui::EditBox>("text_Borders")->getText();

        std::string strBorders = borders.value;

        // If there is no bracket at the beginning then add one
        if (strBorders[0] != '(')
            strBorders.insert(strBorders.begin(), '(');

        // If there is no bracket at the end then add one
        if (strBorders[strBorders.length()-1] != ')')
            strBorders.insert(strBorders.end(), ')');

        // Get the border size
        tgui::Vector4u border;
        tgui::extractVector4u(strBorders, border);

        // Apply the borders
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_Listbox_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = propertyWindow.get<tgui::EditBox>("text_BackgroundColor")->getText();

        // Get a pointer to the listbox
        tgui::Listbox* listbox = formWindow.get<tgui::Listbox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (backgroundColor.value.empty() == false)
        {
            std::string strColor = backgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            listbox->setBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            listbox->setBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Listbox_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Get a pointer to the listbox
        tgui::Listbox* listbox = formWindow.get<tgui::Listbox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            listbox->setTextColor(color);
        }
        else // The string is empty, so use the default color
            listbox->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Listbox_SelectedBackgroundColor)
    {
        // Store the new background color
        selectedBackgroundColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedBackgroundColor")->getText();

        // Get a pointer to the listbox
        tgui::Listbox* listbox = formWindow.get<tgui::Listbox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedBackgroundColor.value.empty() == false)
        {
            std::string strColor = selectedBackgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            listbox->setSelectedBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            listbox->setSelectedBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Listbox_SelectedTextColor)
    {
        // Store the new text color
        selectedTextColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedTextColor")->getText();

        // Get a pointer to the listbox
        tgui::Listbox* listbox = formWindow.get<tgui::Listbox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedTextColor.value.empty() == false)
        {
            std::string strColor = selectedTextColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            listbox->setSelectedTextColor(color);
        }
        else // The string is empty, so use the default color
            listbox->setSelectedTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Listbox_BorderColor)
    {
        // Store the new border color
        borderColor.value = propertyWindow.get<tgui::EditBox>("text_BorderColor")->getText();

        // Get a pointer to the listbox
        tgui::Listbox* listbox = formWindow.get<tgui::Listbox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (borderColor.value.empty() == false)
        {
            std::string strColor = borderColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            listbox->setBorderColor(color);
        }
        else // The string is empty, so use the default color
            listbox->setBorderColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Listbox_Items)
    {
        // Store the new items
        items.value = propertyWindow.get<tgui::EditBox>("text_Items")->getText();

        // Remove all objects from the listbox
        tgui::Listbox* listbox = formWindow.get<tgui::Listbox>(tgui::to_string(id));
        listbox->removeAllItems();

        std::string strItems = items.value;
        bool commaFound = true;
        std::string::size_type commaPos;
        std::string::size_type removedCharacters = 0;
        unsigned int count = 0;

        // Loop all items
        while (commaFound)
        {
            // Search for the next comma
            commaPos = strItems.find(',');

            // Check if a comma was found
            if (commaPos != std::string::npos)
            {
                // Add the item to the listbox
                listbox->addItem(strItems.substr(0, commaPos));

                // Erase the item from the string
                strItems.erase(0, commaPos + 1);

                // Characters have been removed
                removedCharacters += commaPos + 1;

                // Another item was found
                ++count;
            }
            else // No comma was found
            {
                // Add the last item to the list, when the string isn't empty
                if (strItems.size() > 0)
                    listbox->addItem(strItems);

                // Another item was found
                ++count;

                // Stop the loop
                commaFound = false;
            }

            // Check if there is a maximum amount of items
            if (maximumItems.value > 0)
            {
                // Check if the amount of items is allowed
                if (count > maximumItems.value)
                {
                    // Delete a part of the text
                    items.value.erase(removedCharacters - 1);

                    // Update the property
                    propertyWindow.get<tgui::EditBox>("text_Items")->setText(items.value);
                }
            }
        }

        // Check if an item has to be selected
        if (selectedItem.value > 0)
        {
            // Change the selected item
            if (listbox->setSelectedItem(selectedItem.value) == false)
            {
                // If the id was too high then set the property back to 0
                selectedItem.value = 0;
                propertyWindow.get<tgui::EditBox>("text_SelectedItem")->setText("0");
            }
        }
    }
    else if (propertyNumber == Property_Listbox_SelectedItem)
    {
        // Store the new selected item
        selectedItem.value = atoi(propertyWindow.get<tgui::EditBox>("text_SelectedItem")->getText().c_str());

        // Change the selected item in the listbox
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->setSelectedItem(selectedItem.value);
    }
    else if (propertyNumber == Property_Listbox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::Listbox>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::Listbox>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_Listbox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Listbox>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesComboBox::PropertiesComboBox()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/ComboBox/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 240;
    height.value = 36;
    scrollbarPathname.value = "images/objects/Scrollbar/" OBJECT_STYLE;
    borders.value = "2, 2, 2, 2";
    backgroundColor.value = "50, 50, 50";
    textColor.value = "200, 200, 200";
    selectedBackgroundColor.value = "10, 110, 255";
    selectedTextColor.value = "255, 255, 255";
    borderColor.value = "0, 0, 0";
    items.value = "";
    selectedItem.value = 0;
    itemsToDisplay.value = 10;
    textFont.value = "Global";
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesComboBox::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_ComboBox_Name);
    pathname.addProperty(window, Property_ComboBox_Pathname);
    left.addProperty(window, Property_ComboBox_Left);
    top.addProperty(window, Property_ComboBox_Top);
    width.addProperty(window, Property_ComboBox_Width);
    height.addProperty(window, Property_ComboBox_Height);
    scrollbarPathname.addProperty(window, Property_ComboBox_ScrollbarPathname);
    borders.addProperty(window, Property_ComboBox_Borders);
    backgroundColor.addProperty(window, Property_ComboBox_BackgroundColor);
    textColor.addProperty(window, Property_ComboBox_TextColor);
    selectedBackgroundColor.addProperty(window, Property_ComboBox_SelectedBackgroundColor);
    selectedTextColor.addProperty(window, Property_ComboBox_SelectedTextColor);
    borderColor.addProperty(window, Property_ComboBox_BorderColor);
    items.addProperty(window, Property_ComboBox_Items);
    selectedItem.addProperty(window, Property_ComboBox_SelectedItem);
    itemsToDisplay.addProperty(window, Property_ComboBox_ItemsToDisplay);
    textFont.addProperty(window, Property_ComboBox_TextFont);
    callbackID.addProperty(window, Property_ComboBox_CallbackID);
}

void PropertiesComboBox::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_ComboBox_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_ComboBox_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default combo box is no longer used
            loadedDefaultComboBox = false;

            // Load the new combo box
            tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));
            comboBox->load(pathname.value, comboBox->getSize().x, comboBox->getSize().y);

            // Reset the scaling
            comboBox->setScale(comboBox->getScale().x, 1);

            // Change the height property
            height.value = static_cast<float>(comboBox->getSize().y);

            // Update the height value
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default combo box is not loaded already then load it now
            if (loadedDefaultComboBox == false)
            {
                // The default combo box is loaded again
                loadedDefaultComboBox = true;

                // Load the default combo box
                tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));
                comboBox->load("images/objects/ComboBox/" OBJECT_STYLE, comboBox->getSize().x, comboBox->getSize().y);

                // Adjust the scaling
                comboBox->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_ComboBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the combo box
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_ComboBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the combo box
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_ComboBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the size of the combo box
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_ComboBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the size of the combo box
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_ComboBox_ScrollbarPathname)
    {
        scrollbarPathname.value = propertyWindow.get<tgui::EditBox>("text_ScrollbarPathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((scrollbarPathname.value + "/info.txt").c_str()))
        {
            // The default scrollbar is no longer used
            loadedDefaultScrollbar = false;

            // Load the new scrollbar
            formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setScrollbar(scrollbarPathname.value);
        }
        else // The file didn't exist
        {
            // When the default scrollbar is not loaded already then load it now
            if (loadedDefaultScrollbar == false)
            {
                // The default scrollbar is loaded again
                loadedDefaultScrollbar = true;

                // Load the default scrollbar
                formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setScrollbar("images/objects/Scrollbar/" OBJECT_STYLE);
            }
        }
    }
    else if (propertyNumber == Property_ComboBox_Borders)
    {
        // Store the new borders
        borders.value = propertyWindow.get<tgui::EditBox>("text_Borders")->getText();

        std::string strBorders = borders.value;

        // If there is no bracket at the beginning then add one
        if (strBorders[0] != '(')
            strBorders.insert(strBorders.begin(), '(');

        // If there is no bracket at the end then add one
        if (strBorders[strBorders.length()-1] != ')')
            strBorders.insert(strBorders.end(), ')');

        // Get the border size
        tgui::Vector4u border;
        tgui::extractVector4u(strBorders, border);

        // Apply the borders
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_ComboBox_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = propertyWindow.get<tgui::EditBox>("text_BackgroundColor")->getText();

        // Get a pointer to the combo box
        tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (backgroundColor.value.empty() == false)
        {
            std::string strColor = backgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            comboBox->setBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            comboBox->setBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ComboBox_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Get a pointer to the combo box
        tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            comboBox->setTextColor(color);
        }
        else // The string is empty, so use the default color
            comboBox->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ComboBox_SelectedBackgroundColor)
    {
        // Store the new background color
        selectedBackgroundColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedBackgroundColor")->getText();

        // Get a pointer to the combo box
        tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedBackgroundColor.value.empty() == false)
        {
            std::string strColor = selectedBackgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            comboBox->setSelectedBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            comboBox->setSelectedBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ComboBox_SelectedTextColor)
    {
        // Store the new text color
        selectedTextColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedTextColor")->getText();

        // Get a pointer to the combo box
        tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedTextColor.value.empty() == false)
        {
            std::string strColor = selectedTextColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            comboBox->setSelectedTextColor(color);
        }
        else // The string is empty, so use the default color
            comboBox->setSelectedTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ComboBox_BorderColor)
    {
        // Store the new border color
        borderColor.value = propertyWindow.get<tgui::EditBox>("text_BorderColor")->getText();

        // Get a pointer to the combo box
        tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (borderColor.value.empty() == false)
        {
            std::string strColor = borderColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            comboBox->setBorderColor(color);
        }
        else // The string is empty, so use the default color
            comboBox->setBorderColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ComboBox_Items)
    {
        // Store the new items
        items.value = propertyWindow.get<tgui::EditBox>("text_Items")->getText();

        // Remove all objects from the combo box
        tgui::ComboBox* comboBox = formWindow.get<tgui::ComboBox>(tgui::to_string(id));
        comboBox->removeAllItems();

        std::string strItems = items.value;
        bool commaFound = true;

        // Loop all items
        while (commaFound)
        {
            // Search for the next comma
            std::string::size_type commaPos = strItems.find(',');

            // Check if a comma was found
            if (commaPos != std::string::npos)
            {
                // Add the item to the combo box
                comboBox->addItem(strItems.substr(0, commaPos));

                // Erase the item from the string
                strItems.erase(0, commaPos + 1);
            }
            else // No comma was found
            {
                // Add the last item to the list, when the string isn't empty
                if (strItems.size() > 0)
                    comboBox->addItem(strItems);

                // Stop the loop
                commaFound = false;
            }
        }

        // Check if an item has to be selected
        if (selectedItem.value > 0)
        {
            // Change the selected item
            if (comboBox->setSelectedItem(selectedItem.value) == false)
            {
                // If the id was too high then set the property back to 0
                selectedItem.value = 0;
                propertyWindow.get<tgui::EditBox>("text_SelectedItem")->setText("0");
            }
        }
    }
    else if (propertyNumber == Property_ComboBox_SelectedItem)
    {
        // Store the new selected item
        selectedItem.value = atoi(propertyWindow.get<tgui::EditBox>("text_SelectedItem")->getText().c_str());

        // Change the selected item in the combo box
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setSelectedItem(selectedItem.value);
    }
    else if (propertyNumber == Property_ComboBox_ItemsToDisplay)
    {
        // Store the new amount of item to display
        itemsToDisplay.value = atoi(propertyWindow.get<tgui::EditBox>("text_ItemsToDisplay")->getText().c_str());

        // Change the amount of item to display in the combo box
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setItemsToDisplay(itemsToDisplay.value);
    }
    else if (propertyNumber == Property_ComboBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::ComboBox>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_ComboBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::ComboBox>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesSlider::PropertiesSlider()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/Slider/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 256;
    height.value = 20;
    verticalScroll.value = false;
    value.value = 0;
    minimum.value = 0;
    maximum.value = 100;
    callbackID.value = 0;

    loadedDefaultSlider = true;
}

void PropertiesSlider::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Slider_Name);
    pathname.addProperty(window, Property_Slider_Pathname);
    left.addProperty(window, Property_Slider_Left);
    top.addProperty(window, Property_Slider_Top);
    width.addProperty(window, Property_Slider_Width);
    height.addProperty(window, Property_Slider_Height);
    value.addProperty(window, Property_Slider_Value);
    minimum.addProperty(window, Property_Slider_Minimum);
    maximum.addProperty(window, Property_Slider_Maximum);
    callbackID.addProperty(window, Property_Slider_CallbackID);

    verticalScroll.addProperty(window, Property_Slider_VerticalScroll);
}

void PropertiesSlider::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Slider_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Slider_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default slider is no longer used
            loadedDefaultSlider = false;

            // Load the new slider
            tgui::Slider* slider = formWindow.get<tgui::Slider>(tgui::to_string(id));
            slider->load(pathname.value);

            // Reset the scaling
            slider->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(slider->getSize().x);
            height.value = static_cast<float>(slider->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default slider is not loaded already then load it now
            if (loadedDefaultSlider == false)
            {
                // The default slider is loaded again
                loadedDefaultSlider = true;

                // Load the default slider
                tgui::Slider* slider = formWindow.get<tgui::Slider>(tgui::to_string(id));
                slider->load("images/objects/Slider/" OBJECT_STYLE);

                // Adjust the scaling
                slider->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Slider_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Slider_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Slider_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Slider_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Slider_VerticalScroll)
    {
        // Change the direction of the slider
        verticalScroll.value = propertyWindow.get<tgui::ComboBox>("combo_VerticalScroll")->getSelectedItemID() - 1;

        // Get a pointer to the slider and to the width and height properties
        tgui::Slider* slider = formWindow.get<tgui::Slider>(tgui::to_string(id));
        tgui::EditBox* propertyWidth = propertyWindow.get<tgui::EditBox>("text_Width");
        tgui::EditBox* propertyHeight = propertyWindow.get<tgui::EditBox>("text_Height");

        // Change the direction
        slider->verticalScroll = !!verticalScroll.value;

        // Make temp buffers to switch the width and height
        float temp = width.value;
        std::string strTemp = propertyWidth->getText();

        // Change the width
        width.value = height.value;
        propertyWidth->setText(propertyHeight->getText());

        // Change the height
        height.value = temp;
        propertyHeight->setText(strTemp);

        // Set the slider to the correct size
        slider->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Slider_Value)
    {
        // Store the value
        value.value = atoi(propertyWindow.get<tgui::EditBox>("text_Value")->getText().c_str());

        // Change the value of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setValue(value.value);
    }
    else if (propertyNumber == Property_Slider_Minimum)
    {
        // Store the minimum value
        minimum.value = atoi(propertyWindow.get<tgui::EditBox>("text_Minimum")->getText().c_str());

        // Change the minimum value of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setMinimum(minimum.value);
    }
    else if (propertyNumber == Property_Slider_Maximum)
    {
        // Store the maximum value
        maximum.value = atoi(propertyWindow.get<tgui::EditBox>("text_Maximum")->getText().c_str());

        // Change the maximum value of the slider
        formWindow.get<tgui::Slider>(tgui::to_string(id))->setMaximum(maximum.value);
    }
    else if (propertyNumber == Property_Slider_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Slider>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesScrollbar::PropertiesScrollbar()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/Scrollbar/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 20;
    height.value = 256;
    verticalScroll.value = true;
    value.value = 0;
    lowValue.value = 4;
    maximum.value = 5;
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesScrollbar::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_Scrollbar_Name);
    pathname.addProperty(window, Property_Scrollbar_Pathname);
    left.addProperty(window, Property_Scrollbar_Left);
    top.addProperty(window, Property_Scrollbar_Top);
    width.addProperty(window, Property_Scrollbar_Width);
    height.addProperty(window, Property_Scrollbar_Height);
    value.addProperty(window, Property_Scrollbar_Value);
    lowValue.addProperty(window, Property_Scrollbar_LowValue);
    maximum.addProperty(window, Property_Scrollbar_Maximum);
    callbackID.addProperty(window, Property_Scrollbar_CallbackID);

    verticalScroll.addProperty(window, Property_Scrollbar_VerticalScroll);
}

void PropertiesScrollbar::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Scrollbar_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_Scrollbar_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default scrollbar is no longer used
            loadedDefaultScrollbar = false;

            // Load the new scrollbar
            tgui::Scrollbar* scrollbar = formWindow.get<tgui::Scrollbar>(tgui::to_string(id));
            scrollbar->load(pathname.value);

            // Reset the scaling
            scrollbar->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(scrollbar->getSize().x);
            height.value = static_cast<float>(scrollbar->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default scrollbar is not loaded already then load it now
            if (loadedDefaultScrollbar == false)
            {
                // The default scrollbar is loaded again
                loadedDefaultScrollbar = true;

                // Load the default scrollbar
                tgui::Scrollbar* scrollbar = formWindow.get<tgui::Scrollbar>(tgui::to_string(id));
                scrollbar->load("images/objects/Slider/" OBJECT_STYLE);

                // Adjust the scaling
                scrollbar->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Scrollbar_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Scrollbar_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Scrollbar_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Scrollbar_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Scrollbar_VerticalScroll)
    {
        // Change the direction of the scrollbar
        verticalScroll.value = propertyWindow.get<tgui::ComboBox>("combo_VerticalScroll")->getSelectedItemID() - 1;

        // Get a pointer to the width and height properties
        tgui::EditBox* propertyWidth = propertyWindow.get<tgui::EditBox>("text_Width");
        tgui::EditBox* propertyHeight = propertyWindow.get<tgui::EditBox>("text_Height");

        // Change the direction
        tgui::Scrollbar* scrollbar = formWindow.get<tgui::Scrollbar>(tgui::to_string(id));
        scrollbar->verticalScroll = !!verticalScroll.value;

        // Make temp buffers to switch the width and height
        float temp = width.value;
        std::string strTemp = propertyWidth->getText();

        // Change the width
        width.value = height.value;
        propertyWidth->setText(propertyHeight->getText());

        // Change the height
        height.value = temp;
        propertyHeight->setText(strTemp);

        // Set the scrollbar to the correct size
        scrollbar->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Scrollbar_Value)
    {
        // Store the value
        value.value = atoi(propertyWindow.get<tgui::EditBox>("text_Value")->getText().c_str());

        // The value may not be too high
        if (maximum.value >= lowValue.value)
        {
            if (value.value > maximum.value - lowValue.value)
            {
                // Adjust the value
                value.value = maximum.value - lowValue.value;

                // Adjust the property
                propertyWindow.get<tgui::EditBox>("text_Value")->setText(tgui::to_string(value.value));
            }
        }
        else
        {
            // Adjust the value
            value.value = 0;
            propertyWindow.get<tgui::EditBox>("text_Value")->setText("0");
        }

        // Change the value of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setValue(value.value);
    }
    else if (propertyNumber == Property_Scrollbar_LowValue)
    {
        // Store the low value
        lowValue.value = atoi(propertyWindow.get<tgui::EditBox>("text_LowValue")->getText().c_str());

        // Change the low value of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setLowValue(lowValue.value);
    }
    else if (propertyNumber == Property_Scrollbar_Maximum)
    {
        // Store the maximum value
        maximum.value = atoi(propertyWindow.get<tgui::EditBox>("text_Maximum")->getText().c_str());

        // Change the maximum value of the scrollbar
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->setMaximum(maximum.value);
    }
    else if (propertyNumber == Property_Scrollbar_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::Scrollbar>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesLoadingBar::PropertiesLoadingBar()
{
    id = 0;
    name.value = "";
    pathname.value = "images/objects/LoadingBar/" OBJECT_STYLE;
    left.value = 0;
    top.value = 0;
    width.value = 200;
    height.value = 40;
    value.value = 0;
    minimum.value = 0;
    maximum.value = 100;
    callbackID.value = 0;

    loadedDefaultLoadingBar = true;
}

void PropertiesLoadingBar::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_LoadingBar_Name);
    pathname.addProperty(window, Property_LoadingBar_Pathname);
    left.addProperty(window, Property_LoadingBar_Left);
    top.addProperty(window, Property_LoadingBar_Top);
    width.addProperty(window, Property_LoadingBar_Width);
    height.addProperty(window, Property_LoadingBar_Height);
    value.addProperty(window, Property_LoadingBar_Value);
    minimum.addProperty(window, Property_LoadingBar_Minimum);
    maximum.addProperty(window, Property_LoadingBar_Maximum);
    callbackID.addProperty(window, Property_LoadingBar_CallbackID);
}

void PropertiesLoadingBar::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_LoadingBar_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_LoadingBar_Pathname)
    {
        pathname.value = propertyWindow.get<tgui::EditBox>("text_Pathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((pathname.value + "/info.txt").c_str()))
        {
            // The default loading bar is no longer used
            loadedDefaultLoadingBar = false;

            // Load the new loading bar
            tgui::LoadingBar* loadingBar = formWindow.get<tgui::LoadingBar>(tgui::to_string(id));
            loadingBar->load(pathname.value);

            // Reset the scaling
            loadingBar->setScale(1, 1);

            // Change the width and height properties
            width.value = static_cast<float>(loadingBar->getSize().x);
            height.value = static_cast<float>(loadingBar->getSize().y);

            // Update the values
            propertyWindow.get<tgui::EditBox>("text_Width")->setText(tgui::to_string(width.value));
            propertyWindow.get<tgui::EditBox>("text_Height")->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default loading bar is not loaded already then load it now
            if (loadedDefaultLoadingBar == false)
            {
                // The default loading bar is loaded again
                loadedDefaultLoadingBar = true;

                // Load the default loading bar
                tgui::LoadingBar* loadingBar = formWindow.get<tgui::LoadingBar>(tgui::to_string(id));
                loadingBar->load("images/objects/Slider/" OBJECT_STYLE);

                // Adjust the scaling
                loadingBar->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_LoadingBar_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_LoadingBar_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_LoadingBar_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_LoadingBar_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_LoadingBar_Value)
    {
        // Store the value
        value.value = atoi(propertyWindow.get<tgui::EditBox>("text_Value")->getText().c_str());

        // Change the value of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setValue(value.value);
    }
    else if (propertyNumber == Property_LoadingBar_Minimum)
    {
        // Store the minimum value
        minimum.value = atoi(propertyWindow.get<tgui::EditBox>("text_Minimum")->getText().c_str());

        // Change the minimum value of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setMinimum(minimum.value);
    }
    else if (propertyNumber == Property_LoadingBar_Maximum)
    {
        // Store the maximum value
        maximum.value = atoi(propertyWindow.get<tgui::EditBox>("text_Maximum")->getText().c_str());

        // Change the maximum value of the loading bar
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->setMaximum(maximum.value);
    }
    else if (propertyNumber == Property_LoadingBar_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::LoadingBar>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesTextBox::PropertiesTextBox()
{
    id = 0;
    name.value = "";
    left.value = 0;
    top.value = 0;
    width.value = 320;
    height.value = 172;
    scrollbarPathname.value = "images/objects/Scrollbar/" OBJECT_STYLE;
    text.value = "";
    textSize.value = 24;
    textFont.value = "Global";
    maximumCharacters.value = 0;
    borders.value = "2, 2, 2, 2";
    backgroundColor.value = " 50, 50, 50";
    textColor.value = "200, 200, 200";
    selectedTextColor.value = "255, 255, 255";
    selectedTextBackgroundColor.value = "10, 110, 255";
    unfocusedSelectedTextBackgroundColor.value = "110, 110, 255";
    borderColor.value = "0, 0, 0";
    selectionPointColor.value = "110, 110, 255";
    selectionPointWidth.value = 2;
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesTextBox::addProperties(tgui::Window& window)
{
    name.addProperty(window, Property_TextBox_Name);
    left.addProperty(window, Property_TextBox_Left);
    top.addProperty(window, Property_TextBox_Top);
    width.addProperty(window, Property_TextBox_Width);
    height.addProperty(window, Property_TextBox_Height);
    scrollbarPathname.addProperty(window, Property_TextBox_ScrollbarPathname);
    text.addProperty(window, Property_TextBox_Text);
    textSize.addProperty(window, Property_TextBox_TextSize);
    textFont.addProperty(window, Property_TextBox_TextFont);
    maximumCharacters.addProperty(window, Property_TextBox_MaximumCharacters);
    borders.addProperty(window, Property_TextBox_Borders);
    backgroundColor.addProperty(window, Property_TextBox_BackgroundColor);
    textColor.addProperty(window, Property_TextBox_TextColor);
    selectedTextColor.addProperty(window, Property_TextBox_SelectedTextColor);
    selectedTextBackgroundColor.addProperty(window, Property_TextBox_SelectedTextBackgroundColor);
    unfocusedSelectedTextBackgroundColor.addProperty(window, Property_TextBox_UnfocusedSelectedTextBackgroundColor);
    borderColor.addProperty(window, Property_TextBox_BorderColor);
    selectionPointColor.addProperty(window, Property_TextBox_SelectionPointColor);
    selectionPointWidth.addProperty(window, Property_TextBox_SelectionPointWidth);
    callbackID.addProperty(window, Property_TextBox_CallbackID);
}

void PropertiesTextBox::updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_TextBox_Name)
    {
        name.value = propertyWindow.get<tgui::EditBox>("text_Name")->getText();
    }
    else if (propertyNumber == Property_TextBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Left")->getText().c_str()));

        // Change the position of the text box
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_TextBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Top")->getText().c_str()));

        // Change the position of the text box
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_TextBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Width")->getText().c_str()));

        // Change the width of the text box
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_TextBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(propertyWindow.get<tgui::EditBox>("text_Height")->getText().c_str()));

        // Change the height of the text box
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_TextBox_ScrollbarPathname)
    {
        scrollbarPathname.value = propertyWindow.get<tgui::EditBox>("text_ScrollbarPathname")->getText();

        // Check if the info file exists
        if (checkIfFileExists((scrollbarPathname.value + "/info.txt").c_str()))
        {
            // The default scrollbar is no longer used
            loadedDefaultScrollbar = false;

            // Load the new scrollbar
            formWindow.get<tgui::TextBox>(tgui::to_string(id))->setScrollbar(scrollbarPathname.value);
        }
        else // The file didn't exist
        {
            // When the default scrollbar is not loaded already then load it now
            if (loadedDefaultScrollbar == false)
            {
                // The default scrollbar is loaded again
                loadedDefaultScrollbar = true;

                // Load the default scrollbar
                formWindow.get<tgui::TextBox>(tgui::to_string(id))->setScrollbar("images/objects/Scrollbar/" OBJECT_STYLE);
            }
        }
    }
    else if (propertyNumber == Property_TextBox_Text)
    {
        // Store the new text
        text.value = propertyWindow.get<tgui::EditBox>("text_Text")->getText();

        // Change the text of the text box
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setText(text.value);
    }
    else if (propertyNumber == Property_TextBox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(propertyWindow.get<tgui::EditBox>("text_TextSize")->getText().c_str());

        // Change the text of the text box
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_TextBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = propertyWindow.get<tgui::EditBox>("text_TextFont")->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::TextBox>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
        }
        else // The global font is not used
        {
            std::string fontName;

            // If the font is not recognised then use the default font
            if (textFont.value.compare("Default") != 0) // && (fontName.compare("fontname") != 0)
                fontName = "Default";
            else
                fontName = textFont.value;

            // Change the font
            if (fontName.compare("Default"))
                formWindow.get<tgui::TextBox>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        propertyWindow.get<tgui::EditBox>("text_TextFont")->setText("Global");
    }
    else if (propertyNumber == Property_TextBox_MaximumCharacters)
    {
        // Store the new maximum
        maximumCharacters.value = atoi(propertyWindow.get<tgui::EditBox>("text_MaximumCharacters")->getText().c_str());

        // Set the maximum characters
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setMaximumCharacters(maximumCharacters.value);
        propertyWindow.get<tgui::EditBox>("text_Text")->setMaximumCharacters(maximumCharacters.value);
    }
    else if (propertyNumber == Property_TextBox_Borders)
    {
        // Store the new borders
        borders.value = propertyWindow.get<tgui::EditBox>("text_Borders")->getText();

        std::string strBorders = borders.value;

        // If there is no bracket at the beginning then add one
        if (strBorders[0] != '(')
            strBorders.insert(strBorders.begin(), '(');

        // If there is no bracket at the end then add one
        if (strBorders[strBorders.length()-1] != ')')
            strBorders.insert(strBorders.end(), ')');

        // Get the border size
        tgui::Vector4u border;
        tgui::extractVector4u(strBorders, border);

        // Apply the borders
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_TextBox_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = propertyWindow.get<tgui::EditBox>("text_BackgroundColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (backgroundColor.value.empty() == false)
        {
            std::string strColor = backgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->setBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_TextBox_TextColor)
    {
        // Store the new text color
        textColor.value = propertyWindow.get<tgui::EditBox>("text_TextColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (textColor.value.empty() == false)
        {
            std::string strColor = textColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->setTextColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_TextBox_SelectedTextColor)
    {
        // Store the new color
        selectedTextColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedTextColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedTextColor.value.empty() == false)
        {
            std::string strColor = selectedTextColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->setSelectedTextColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setSelectedTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_TextBox_SelectedTextBackgroundColor)
    {
        // Store the new color
        selectedTextBackgroundColor.value = propertyWindow.get<tgui::EditBox>("text_SelectedTextBackgroundColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectedTextBackgroundColor.value.empty() == false)
        {
            std::string strColor = selectedTextBackgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->setSelectedTextBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setSelectedTextBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_TextBox_UnfocusedSelectedTextBackgroundColor)
    {
        // Store the new color
        unfocusedSelectedTextBackgroundColor.value = propertyWindow.get<tgui::EditBox>("text_UnfocusedSelectedTextBackgroundColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (unfocusedSelectedTextBackgroundColor.value.empty() == false)
        {
            std::string strColor = unfocusedSelectedTextBackgroundColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->setUnfocusedSelectedTextBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setUnfocusedSelectedTextBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_TextBox_BorderColor)
    {
        // Store the new border color
        borderColor.value = propertyWindow.get<tgui::EditBox>("text_BorderColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (borderColor.value.empty() == false)
        {
            std::string strColor = borderColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->setBorderColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setBorderColor(sf::Color::White);
    }
    else if (propertyNumber == Property_TextBox_SelectionPointColor)
    {
        // Store the new color
        selectionPointColor.value = propertyWindow.get<tgui::EditBox>("text_SelectionPointColor")->getText();

        // Get the pointer to the text box
        tgui::TextBox* textBox = formWindow.get<tgui::TextBox>(tgui::to_string(id));

        // Only continue when the string is not empty
        if (selectionPointColor.value.empty() == false)
        {
            std::string strColor = selectionPointColor.value;

            // If there is no bracket at the beginning then add one
            if (strColor[0] != '(')
                strColor.insert(strColor.begin(), '(');

            // If there is no bracket at the end then add one
            if (strColor[strColor.length()-1] != ')')
                strColor.insert(strColor.end(), ')');

            // Convert the string to a color
            sf::Color color = tgui::extractColor(strColor);

            // Use the new color
            textBox->selectionPointColor = color;
        }
        else // The string is empty, so use the default color
            textBox->selectionPointColor = sf::Color::White;

    }
    else if (propertyNumber == Property_TextBox_SelectionPointWidth)
    {
        // Store the new width
        selectionPointWidth.value = atoi(propertyWindow.get<tgui::EditBox>("text_SelectionPointWidth")->getText().c_str());

        // Set the new width
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->selectionPointWidth = selectionPointWidth.value;
    }
    else if (propertyNumber == Property_TextBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(propertyWindow.get<tgui::EditBox>("text_CallbackID")->getText().c_str());

        // Change the callback id
        formWindow.get<tgui::TextBox>(tgui::to_string(id))->callbackID = callbackID.value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
