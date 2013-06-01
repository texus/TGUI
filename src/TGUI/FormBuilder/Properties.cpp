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
void Property##property::addProperty(tgui::Gui& gui, unsigned int propertyNumber) \
{ \
    tgui::Label::Ptr label(gui, "label_"#property); \
    label->setText(description); \
    label->setTextColor(sf::Color::Black); \
    label->setPosition(10, 14 + (40.f * propertyNumber)); \
    label->setTextSize(26); \
 \
    tgui::EditBox::Ptr editBox(gui, "text_"#property); \
    editBox->load(DEFAULT_THEME_FILE); \
    editBox->setBorders(6, 4, 6, 4); \
    editBox->setPosition(gui.getWindow()->getSize().x / 2.f, 10 + (40.f * propertyNumber)); \
    editBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 32); \
    editBox->setText(value); \
    editBox->setCallbackId(propertyNumber + 1); \
    editBox->bindCallback(tgui::EditBox::TextChanged); \
}

#define addProperty_Value(property) \
void Property##property::addProperty(tgui::Gui& gui, unsigned int propertyNumber) \
{ \
    tgui::Label::Ptr label(gui, "label_"#property); \
    label->setText(description); \
    label->setTextColor(sf::Color::Black); \
    label->setPosition(10, 14 + (40.f * propertyNumber)); \
    label->setTextSize(26); \
 \
    tgui::EditBox::Ptr editBox(gui, "text_"#property); \
    editBox->load(DEFAULT_THEME_FILE); \
    editBox->setBorders(6, 4, 6, 4); \
    editBox->setPosition(gui.getWindow()->getSize().x / 2.f, 10 + (40.f * propertyNumber)); \
    editBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 32); \
    editBox->setText(tgui::to_string(value)); \
    editBox->setCallbackId(propertyNumber + 1); \
    editBox->bindCallback(tgui::EditBox::TextChanged); \
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

PropertyConfigFile::PropertyConfigFile()
{
    description = "ConfigFile";
    value = "";
}

addProperty_String(ConfigFile)

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

PropertyAutoSize::PropertyAutoSize()
{
    description = "AutoSize";
    value = 0;
}

void PropertyAutoSize::addProperty(tgui::Gui& gui, unsigned int propertyNumber)
{
    // Create the label
    tgui::Label::Ptr label(gui, "label_AutoSize");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40.f * propertyNumber));
    label->setTextSize(26);

    // Create the combo box
    tgui::ComboBox::Ptr comboBox(gui, "combo_AutoSize");
    comboBox->load(DEFAULT_THEME_FILE);
    comboBox->setSize(gui.getWindow()->getSize().x / 2.f - 10, 40);
    comboBox->setPosition(gui.getWindow()->getSize().x / 2.f, 10 + (40.f * propertyNumber));
    comboBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 32);
    comboBox->setCallbackId(propertyNumber + 1);
    comboBox->bindCallback(tgui::ComboBox::ItemSelected);

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

void PropertyChecked::addProperty(tgui::Gui& gui, unsigned int propertyNumber)
{
    // Create the label
    tgui::Label::Ptr label(gui, "label_Checked");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40.f * propertyNumber));
    label->setTextSize(26);

    // Create the combo box
    tgui::ComboBox::Ptr comboBox(gui, "combo_Checked");
    comboBox->load(DEFAULT_THEME_FILE);
    comboBox->setSize(gui.getWindow()->getSize().x / 2.f - 10, 40);
    comboBox->setPosition(gui.getWindow()->getSize().x / 2.f, 10 + (40.f * propertyNumber));
    comboBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 32);
    comboBox->setCallbackId(propertyNumber + 1);
    comboBox->bindCallback(tgui::ComboBox::ItemSelected);

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

void PropertyPasswordChar::addProperty(tgui::Gui& gui, unsigned int propertyNumber)
{
    tgui::Label::Ptr label(gui, "label_PasswordChar");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40.f * propertyNumber));
    label->setTextSize(26);

    tgui::EditBox::Ptr editBox(gui, "text_PasswordChar");
    editBox->load(DEFAULT_THEME_FILE);
    editBox->setBorders(6, 4, 6, 4);
    editBox->setPosition(gui.getWindow()->getSize().x / 2.f, 10 + (40.f * propertyNumber));
    editBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 32);
    editBox->setMaximumCharacters(1);
    editBox->setCallbackId(propertyNumber + 1);
    editBox->bindCallback(tgui::EditBox::TextChanged);

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
    value = -1;
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

void PropertyVerticalScroll::addProperty(tgui::Gui& gui, unsigned int propertyNumber)
{
    // Create the label
    tgui::Label::Ptr label(gui, "label_VerticalScroll");
    label->setText(description);
    label->setTextColor(sf::Color::Black);
    label->setPosition(10, 14 + (40.f * propertyNumber));
    label->setTextSize(26);

    // Create the combo box
    tgui::ComboBox::Ptr comboBox(gui, "combo_VerticalScroll");
    comboBox->load(DEFAULT_THEME_FILE);
    comboBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 40);
    comboBox->setPosition(gui.getWindow()->getSize().x / 2.f, 10 + (40.f * propertyNumber));
    comboBox->setSize(gui.getWindow()->getSize().x / 2.0f - 10, 32);
    comboBox->setCallbackId(propertyNumber + 1);
    comboBox->bindCallback(tgui::ComboBox::ItemSelected);

    // Change the colors of the combo box
    comboBox->changeColors(sf::Color( 50,  50,  50),
                           sf::Color(200, 200, 200),
                           sf::Color( 10, 110, 255),
                           sf::Color(255, 255, 255));

    // Change the contents of the combo box
    comboBox->addItem("false");
    comboBox->addItem("true");
    comboBox->setSelectedItem(value);
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

void PropertiesWindow::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Window_Name);
    width.addProperty(gui, Property_Window_Width);
    height.addProperty(gui, Property_Window_Height);
    globalFont.addProperty(gui, Property_Window_GlobalFont);
}

void PropertiesWindow::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Window_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Window_Width)
    {
        // Get the width of the window
        unsigned int windowWidth = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_WindowWidth"))->getText().toAnsiString().c_str());
        width.value = windowWidth;

        // There is a minimum size
        if (windowWidth < 32)
            windowWidth = 32;

        // Set the window size
        formWindow.getWindow()->setSize(sf::Vector2u(windowWidth, formWindow.getWindow()->getSize().y));
    }
    else if (propertyNumber == Property_Window_Height)
    {
        // Get the height of the window
        unsigned int windowHeight = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_WindowHeight"))->getText().toAnsiString().c_str());
        height.value = windowHeight;

        // There is a minimum size
        if (windowHeight < 32)
            windowHeight = 32;

        // Set the window size
        formWindow.getWindow()->setSize(sf::Vector2u(formWindow.getWindow()->getSize().x, windowHeight));
    }
    else if (propertyNumber == Property_Window_GlobalFont)
    {
/*
        globalFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_GlobalFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_GlobalFont"))->setText("Default");
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

void PropertiesPicture::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Picture_Name);
    filename.addProperty(gui, Property_Picture_Filename);
    left.addProperty(gui, Property_Picture_Left);
    top.addProperty(gui, Property_Picture_Top);
    width.addProperty(gui, Property_Picture_Width);
    height.addProperty(gui, Property_Picture_Height);
    callbackID.addProperty(gui, Property_Picture_CallbackID);
}

void PropertiesPicture::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Picture_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Picture_Filename)
    {
        filename.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Filename"))->getText();

        // Check if the file exists
        if (checkIfFileExists(filename.value.c_str()))
        {
            // The default picture is no longer used
            loadedDefaultPicture = false;

            // Load the new picture
            tgui::Picture::Ptr picture = formWindow.get(tgui::to_string(id));
            picture->load(filename.value);

            // Change the width and height properties
            width.value = static_cast<float>(picture->getSize().x);
            height.value = static_cast<float>(picture->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default picture is not loaded already then load it now
            if (loadedDefaultPicture == false)
            {
                // The default picture is loaded again
                loadedDefaultPicture = true;

                // Load the default picture
                tgui::Picture::Ptr picture = formWindow.get(tgui::to_string(id));
                picture->load("images/objects/Picture.png");

                // Adjust the scaling
                picture->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Picture_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the picture
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Picture_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the picture
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Picture_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the picture
        static_cast<tgui::Picture::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Picture_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the picture
        static_cast<tgui::Picture::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Picture_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesButton::PropertiesButton()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
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

void PropertiesButton::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Button_Name);
    configFile.addProperty(gui, Property_Button_ConfigFile);
    left.addProperty(gui, Property_Button_Left);
    top.addProperty(gui, Property_Button_Top);
    width.addProperty(gui, Property_Button_Width);
    height.addProperty(gui, Property_Button_Height);
    text.addProperty(gui, Property_Button_Text);
    textSize.addProperty(gui, Property_Button_TextSize);
    textColor.addProperty(gui, Property_Button_TextColor);
    textFont.addProperty(gui, Property_Button_TextFont);
    callbackID.addProperty(gui, Property_Button_CallbackID);
}

void PropertiesButton::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Button_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Button_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default button is no longer used
            loadedDefaultButton = false;

            // Load the new button
            tgui::Button::Ptr button = formWindow.get(tgui::to_string(id));
            button->load(configFile.value);

            // Change the width and height properties
            width.value = static_cast<float>(button->getSize().x);
            height.value = static_cast<float>(button->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default button is not loaded already then load it now
            if (loadedDefaultButton == false)
            {
                // The default button is loaded again
                loadedDefaultButton = true;

                // Load the default button
                tgui::Button::Ptr button = formWindow.get(tgui::to_string(id));
                button->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                button->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Button_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the button
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Button_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the button
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Button_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the button
        static_cast<tgui::Button::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Button_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the button
        static_cast<tgui::Button::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Button_Text)
    {
        // Store the new text
        text.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->getText();

        // Change the text of the button
        static_cast<tgui::Button::Ptr>(formWindow.get(tgui::to_string(id)))->setText(text.value);
    }
    else if (propertyNumber == Property_Button_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextSize"))->getText().toAnsiString().c_str());

        // Change the text of the button
        static_cast<tgui::Button::Ptr>(formWindow.get(tgui::to_string(id)))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_Button_TextColor)
    {
        // Store the new text color
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

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
            static_cast<tgui::Button::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            static_cast<tgui::Button::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Button_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_Button_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesCheckbox::PropertiesCheckbox()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
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

void PropertiesCheckbox::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Checkbox_Name);
    configFile.addProperty(gui, Property_Checkbox_ConfigFile);
    left.addProperty(gui, Property_Checkbox_Left);
    top.addProperty(gui, Property_Checkbox_Top);
    width.addProperty(gui, Property_Checkbox_Width);
    height.addProperty(gui, Property_Checkbox_Height);
    text.addProperty(gui, Property_Checkbox_Text);
    textSize.addProperty(gui, Property_Checkbox_TextSize);
    textColor.addProperty(gui, Property_Checkbox_TextColor);
    textFont.addProperty(gui, Property_Checkbox_TextFont);
    callbackID.addProperty(gui, Property_Checkbox_CallbackID);

    checked.addProperty(gui, Property_Checkbox_Checked);
}

void PropertiesCheckbox::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Checkbox_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Checkbox_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default checkbox is no longer used
            loadedDefaultCheckbox = false;

            // Load the new checkbox
            tgui::Checkbox::Ptr checkbox = formWindow.get(tgui::to_string(id));
            checkbox->load(configFile.value);

            // Change the width and height properties
            width.value = static_cast<float>(checkbox->getSize().x);
            height.value = static_cast<float>(checkbox->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default checkbox is not loaded already then load it now
            if (loadedDefaultCheckbox == false)
            {
                // The default checkbox is loaded again
                loadedDefaultCheckbox = true;

                // Load the default checkbox
                tgui::Checkbox::Ptr checkbox = formWindow.get(tgui::to_string(id));
                checkbox->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                checkbox->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Checkbox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the checkbox
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the checkbox
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the checkbox
        static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the checkbox
        static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Checked)
    {
        // Change the value of the combo box
        checked.value = static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_Checked"))->getSelectedItemIndex();

        // Change the checkbox image
        if (checked.value)
            static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->check();
        else
            static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->uncheck();
    }
    else if (propertyNumber == Property_Checkbox_Text)
    {
        // Store the new text
        text.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->getText();

        // Change the text of the checkbox
        static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->setText(text.value);
    }
    else if (propertyNumber == Property_Checkbox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextSize"))->getText().toAnsiString().c_str());

        // Change the text of the checkbox
        static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_Checkbox_TextColor)
    {
        // Store the new text color
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

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
            static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            static_cast<tgui::Checkbox::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Checkbox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_Checkbox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesRadioButton::PropertiesRadioButton()
{
    configFile.value = DEFAULT_THEME_FILE;
    builder = NULL;
}

void PropertiesRadioButton::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Checkbox_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Checkbox_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default radio button is no longer used
            loadedDefaultCheckbox = false;

            // Load the new radio button
            tgui::RadioButton::Ptr radioButton = formWindow.get(tgui::to_string(id));
            radioButton->load(configFile.value);

            // Change the width and height properties
            width.value = static_cast<float>(radioButton->getSize().x);
            height.value = static_cast<float>(radioButton->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default radio button is not loaded already then load it now
            if (loadedDefaultCheckbox == false)
            {
                // The default radio button is loaded again
                loadedDefaultCheckbox = true;

                // Load the default radio button
                tgui::RadioButton::Ptr radioButton = formWindow.get(tgui::to_string(id));
                radioButton->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                radioButton->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Checkbox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the radio button
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the radio button
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Checkbox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the radio button
        static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the radio button
        static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Checkbox_Checked)
    {
        // Turn the checked properties of all radio buttons to false
        for (unsigned int i=0; i<builder->radioButtons.size(); ++i)
            builder->radioButtons[i].checked.value = false;

        // Change the checked flag
        checked.value = static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_Checked"))->getSelectedItemIndex();

        // Change the radio button image
        if (checked.value)
            static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->check();
        else
            formWindow.uncheckRadioButtons();
    }
    else if (propertyNumber == Property_Checkbox_Text)
    {
        // Store the new text
        text.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->getText();

        // Change the text of the radio button
        static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->setText(text.value);
    }
    else if (propertyNumber == Property_Checkbox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextSize"))->getText().toAnsiString().c_str());

        // Change the text of the radio button
        static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_Checkbox_TextColor)
    {
        // Store the new text color
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

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
            static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            static_cast<tgui::RadioButton::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Checkbox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_Checkbox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesLabel::PropertiesLabel()
{
    id = 0;
    name.value = "";
    left.value = 0;
    top.value = 0;
    width.value = 79;
    height.value = 31;
    autoSize.value = true;
    text.value = "Label";
    textSize.value = 30;
    textColor.value = "255,255,255";
    textFont.value = "Global";
    backgroundColor.value = "0,0,0,0";
    callbackID.value = 0;
}

void PropertiesLabel::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Label_Name);
    left.addProperty(gui, Property_Label_Left);
    top.addProperty(gui, Property_Label_Top);
    width.addProperty(gui, Property_Label_Width);
    height.addProperty(gui, Property_Label_Height);
    text.addProperty(gui, Property_Label_Text);
    textSize.addProperty(gui, Property_Label_TextSize);
    textColor.addProperty(gui, Property_Label_TextColor);
    textFont.addProperty(gui, Property_Label_TextFont);
    backgroundColor.addProperty(gui, Property_Label_BackgroundColor);
    callbackID.addProperty(gui, Property_Label_CallbackID);

    autoSize.addProperty(gui, Property_Label_AutoSize);
}

void PropertiesLabel::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Label_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Label_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the label
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Label_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the label
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Label_Width)
    {
        // Store the new size
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the size of the label
        static_cast<tgui::Label::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);

        // You are no longer auto sizing
        if (!!autoSize.value == true)
        {
            autoSize.value = false;
            static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_AutoSize"))->setSelectedItem("false");
        }
    }
    else if (propertyNumber == Property_Label_Height)
    {
        // Store the new size
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the size of the label
        static_cast<tgui::Label::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);

        // You are no longer auto sizing
        if (!!autoSize.value == true)
        {
            autoSize.value = false;
            static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_AutoSize"))->setSelectedItem("false");
        }
    }
    else if (propertyNumber == Property_Label_AutoSize)
    {
        // Store the auto-size boolean
        autoSize.value = !!(static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_AutoSize"))->getSelectedItemIndex());

        // Get the pointer to the label
        tgui::Label::Ptr label = formWindow.get(tgui::to_string(id));

        // Tell the label to auto-size or not
        label->setAutoSize(!!autoSize.value);

        // Check if auto-size is enabled
        if (autoSize.value)
        {
            // Change the width and height
            width.value = static_cast<float>(label->getSize().x);
            height.value = static_cast<float>(label->getSize().y);
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
    }
    else if (propertyNumber == Property_Label_Text)
    {
        // Store the new text
        text.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->getText();

        // Get the pointer to the label
        tgui::Label::Ptr label = formWindow.get(tgui::to_string(id));

        // Change the text of the label
        if (text.value.size())
            label->setText(text.value);
        else
            label->setText("#empty#");

        // Adjust the width of the label
        width.value = static_cast<float>(label->getSize().x);
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));

        // Adjust the height of the label
        height.value = static_cast<float>(label->getSize().y);
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
    }
    else if (propertyNumber == Property_Label_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextSize"))->getText().toAnsiString().c_str());

        // Get the pointer to the label
        tgui::Label::Ptr label = formWindow.get(tgui::to_string(id));

        // Change the text of the label
        label->setTextSize(textSize.value);

        // Adjust the width of the label
        width.value = static_cast<float>(label->getSize().x);
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));

        // Adjust the height of the label
        height.value = static_cast<float>(label->getSize().y);
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
    }
    else if (propertyNumber == Property_Label_TextColor)
    {
        // Store the new text color
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

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
            static_cast<tgui::Label::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(color);
        }
        else // The string is empty, so use the default color
            static_cast<tgui::Label::Ptr>(formWindow.get(tgui::to_string(id)))->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Label_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_Label_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BackgroundColor"))->getText();

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
            static_cast<tgui::Label::Ptr>(formWindow.get(tgui::to_string(id)))->setBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            static_cast<tgui::Label::Ptr>(formWindow.get(tgui::to_string(id)))->setBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_Label_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesEditBox::PropertiesEditBox()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
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
    selectionPointColor.value = "110, 110, 255";
    selectionPointWidth.value = 2;
    callbackID.value = 0;

    loadedDefaultEditBox = true;
}

void PropertiesEditBox::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_EditBox_Name);
    configFile.addProperty(gui, Property_EditBox_ConfigFile);
    left.addProperty(gui, Property_EditBox_Left);
    top.addProperty(gui, Property_EditBox_Top);
    width.addProperty(gui, Property_EditBox_Width);
    height.addProperty(gui, Property_EditBox_Height);
    text.addProperty(gui, Property_EditBox_Text);
    textSize.addProperty(gui, Property_EditBox_TextSize);
    textFont.addProperty(gui, Property_EditBox_TextFont);
    passwordChar.addProperty(gui, Property_EditBox_PasswordChar);
    maximumCharacters.addProperty(gui, Property_EditBox_MaximumCharacters);
    borders.addProperty(gui, Property_EditBox_Borders);
    textColor.addProperty(gui, Property_EditBox_TextColor);
    selectedTextColor.addProperty(gui, Property_EditBox_SelectedTextColor);
    selectedTextBackgroundColor.addProperty(gui, Property_EditBox_SelectedTextBackgroundColor);
    selectionPointColor.addProperty(gui, Property_EditBox_SelectionPointColor);
    selectionPointWidth.addProperty(gui, Property_EditBox_SelectionPointWidth);
    callbackID.addProperty(gui, Property_EditBox_CallbackID);
}

void PropertiesEditBox::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_EditBox_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_EditBox_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default edit box is no longer used
            loadedDefaultEditBox = false;

            // Load the new edit box
            tgui::EditBox::Ptr editBox = formWindow.get(tgui::to_string(id));
            editBox->load(configFile.value);

            // Reset the size
            editBox->setSize(static_cast<float>(editBox->getSize().x), static_cast<float>(editBox->getSize().y));

            // Change the width and height properties
            width.value = static_cast<float>(editBox->getSize().x);
            height.value = static_cast<float>(editBox->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default edit box is not loaded already then load it now
            if (loadedDefaultEditBox == false)
            {
                // The default edit box is loaded again
                loadedDefaultEditBox = true;

                // Load the default edit box
                tgui::EditBox::Ptr editBox = formWindow.get(tgui::to_string(id));
                editBox->load(DEFAULT_THEME_FILE);

                // Adjust the size
                editBox->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_EditBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the edit box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_EditBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the edit box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_EditBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the edit box
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_EditBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the edit box
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_EditBox_Text)
    {
        // Store the new text
        text.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->getText();

        // Change the text of the edit box
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setText(text.value);
    }
    else if (propertyNumber == Property_EditBox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextSize"))->getText().toAnsiString().c_str());

        // Change the text of the edit box
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_EditBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_EditBox_PasswordChar)
    {
        // Get the text inside the edit box
        std::string property = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_PasswordChar"))->getText();

        // Store the new value
        if (property.size() == 0)
            passwordChar.value = 0;
        else
            passwordChar.value = property[0];

        // Change the password char
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setPasswordChar(passwordChar.value);
    }
    else if (propertyNumber == Property_EditBox_MaximumCharacters)
    {
        // Store the new maximum
        maximumCharacters.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_MaximumCharacters"))->getText().toAnsiString().c_str());

        // Set the maximum characters
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximumCharacters(maximumCharacters.value);
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->setMaximumCharacters(maximumCharacters.value);
    }
    else if (propertyNumber == Property_EditBox_Borders)
    {
        // Store the new borders
        borders.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Borders"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_EditBox_TextColor)
    {
        // Store the new text color
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

        // Get the pointer to the edit box
        tgui::EditBox::Ptr editBox = formWindow.get(tgui::to_string(id));

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
        selectedTextColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedTextColor"))->getText();

        // Get the pointer to the edit box
        tgui::EditBox::Ptr editBox = formWindow.get(tgui::to_string(id));

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
        selectedTextBackgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedTextBackgroundColor"))->getText();

        // Get the pointer to the edit box
        tgui::EditBox::Ptr editBox = formWindow.get(tgui::to_string(id));

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
    else if (propertyNumber == Property_EditBox_SelectionPointColor)
    {
        // Store the new color
        selectionPointColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectionPointColor"))->getText();

        // Get the pointer to the edit box
        tgui::EditBox::Ptr editBox = formWindow.get(tgui::to_string(id));

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
            editBox->setSelectionPointColor(color);
        }
        else // The string is empty, so use the default color
            editBox->setSelectionPointColor(sf::Color::White);

    }
    else if (propertyNumber == Property_EditBox_SelectionPointWidth)
    {
        // Store the new width
        selectionPointWidth.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectionPointWidth"))->getText().toAnsiString().c_str());

        // Set the new width
        static_cast<tgui::EditBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSelectionPointWidth(selectionPointWidth.value);
    }
    else if (propertyNumber == Property_EditBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesListBox::PropertiesListBox()
{
    id = 0;
    name.value = "";
    left.value = 0;
    top.value = 0;
    width.value = 200;
    height.value = 240;
    itemHeight.value = 25;
    maximumItems.value = 0;
    borders.value = "2, 2, 2, 2";
    backgroundColor.value = "50, 50, 50";
    textColor.value = "200, 200, 200";
    selectedBackgroundColor.value = "10, 110, 255";
    selectedTextColor.value = "255, 255, 255";
    borderColor.value = "0, 0, 0";
    items.value = "";
    selectedItem.value = -1;
    textFont.value = "Global";
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesListBox::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_ListBox_Name);
    configFile.addProperty(gui, Property_ListBox_ConfigFile);
    left.addProperty(gui, Property_ListBox_Left);
    top.addProperty(gui, Property_ListBox_Top);
    width.addProperty(gui, Property_ListBox_Width);
    height.addProperty(gui, Property_ListBox_Height);
    itemHeight.addProperty(gui, Property_ListBox_ItemHeight);
    maximumItems.addProperty(gui, Property_ListBox_MaximumItems);
    borders.addProperty(gui, Property_ListBox_Borders);
    backgroundColor.addProperty(gui, Property_ListBox_BackgroundColor);
    textColor.addProperty(gui, Property_ListBox_TextColor);
    selectedBackgroundColor.addProperty(gui, Property_ListBox_SelectedBackgroundColor);
    selectedTextColor.addProperty(gui, Property_ListBox_SelectedTextColor);
    borderColor.addProperty(gui, Property_ListBox_BorderColor);
    items.addProperty(gui, Property_ListBox_Items);
    selectedItem.addProperty(gui, Property_ListBox_SelectedItem);
    textFont.addProperty(gui, Property_ListBox_TextFont);
    callbackID.addProperty(gui, Property_ListBox_CallbackID);
}

void PropertiesListBox::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_ListBox_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_ComboBox_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));
            listBox->load(configFile.value);
        }
    }
    else if (propertyNumber == Property_ListBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the list box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_ListBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the list box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_ListBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the size of the list box
        static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_ListBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the size of the list box
        static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_ListBox_ItemHeight)
    {
        // Store the new item height
        itemHeight.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ItemHeight"))->getText().toAnsiString().c_str());

        // Change the item height in the list box
        static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setItemHeight(itemHeight.value);
    }
    else if (propertyNumber == Property_ListBox_MaximumItems)
    {
        // Store the new maximum
        maximumItems.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_MaximumItems"))->getText().toAnsiString().c_str());

        // Change the maximum items in the list box
        static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximumItems(maximumItems.value);

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
                    static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Items"))->setText(items.value);
                }
            }
        }
    }
    else if (propertyNumber == Property_ListBox_Borders)
    {
        // Store the new borders
        borders.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Borders"))->getText();

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
        static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_ListBox_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BackgroundColor"))->getText();

        // Get a pointer to the list box
        tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));

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
            listBox->setBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            listBox->setBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ListBox_TextColor)
    {
        // Store the new text color
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

        // Get a pointer to the list box
        tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));

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
            listBox->setTextColor(color);
        }
        else // The string is empty, so use the default color
            listBox->setTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ListBox_SelectedBackgroundColor)
    {
        // Store the new background color
        selectedBackgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedBackgroundColor"))->getText();

        // Get a pointer to the list box
        tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));

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
            listBox->setSelectedBackgroundColor(color);
        }
        else // The string is empty, so use the default color
            listBox->setSelectedBackgroundColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ListBox_SelectedTextColor)
    {
        // Store the new text color
        selectedTextColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedTextColor"))->getText();

        // Get a pointer to the list box
        tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));

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
            listBox->setSelectedTextColor(color);
        }
        else // The string is empty, so use the default color
            listBox->setSelectedTextColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ListBox_BorderColor)
    {
        // Store the new border color
        borderColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BorderColor"))->getText();

        // Get a pointer to the list box
        tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));

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
            listBox->setBorderColor(color);
        }
        else // The string is empty, so use the default color
            listBox->setBorderColor(sf::Color::White);
    }
    else if (propertyNumber == Property_ListBox_Items)
    {
        // Store the new items
        items.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Items"))->getText();

        // Remove all objects from the list box
        tgui::ListBox::Ptr listBox = formWindow.get(tgui::to_string(id));
        listBox->removeAllItems();

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
                // Add the item to the list box
                listBox->addItem(strItems.substr(0, commaPos));

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
                    listBox->addItem(strItems);

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
                    static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Items"))->setText(items.value);
                }
            }
        }

        // Check if an item has to be selected
        if (selectedItem.value >= 0)
        {
            // Change the selected item
            if (listBox->setSelectedItem(selectedItem.value) == false)
            {
                // If the id was too high then set the property back to -1
                selectedItem.value = -1;
                static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedItem"))->setText("-1");
            }
        }
    }
    else if (propertyNumber == Property_ListBox_SelectedItem)
    {
        // Store the new selected item
        selectedItem.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedItem"))->getText().toAnsiString().c_str());

        // Change the selected item in the list box
        if (selectedItem.value >= 0)
            static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSelectedItem(selectedItem.value);
        else
            static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->deselectItem();
    }
    else if (propertyNumber == Property_ListBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

        // Check if you are using the global font
        if (textFont.value.compare("Global") == 0)
        {
            // Set the new font
            formWindow.get<tgui::ListBox>(tgui::to_string(id))->setTextFont(formWindow.globalFont);
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
                formWindow.get<tgui::ListBox>(tgui::to_string(id))->setTextFont(sf::Font::getDefaultFont());
        }
*/
        // You are not allowed the change the font yet
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_ListBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        static_cast<tgui::ListBox::Ptr>(formWindow.get(tgui::to_string(id)))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesComboBox::PropertiesComboBox()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
    left.value = 0;
    top.value = 0;
    width.value = 240;
    height.value = 36;
    borders.value = "2, 2, 2, 2";
    backgroundColor.value = "50, 50, 50";
    textColor.value = "200, 200, 200";
    selectedBackgroundColor.value = "10, 110, 255";
    selectedTextColor.value = "255, 255, 255";
    borderColor.value = "0, 0, 0";
    items.value = "";
    selectedItem.value = -1;
    itemsToDisplay.value = 10;
    textFont.value = "Global";
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesComboBox::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_ComboBox_Name);
    configFile.addProperty(gui, Property_ComboBox_ConfigFile);
    left.addProperty(gui, Property_ComboBox_Left);
    top.addProperty(gui, Property_ComboBox_Top);
    width.addProperty(gui, Property_ComboBox_Width);
    height.addProperty(gui, Property_ComboBox_Height);
    borders.addProperty(gui, Property_ComboBox_Borders);
    backgroundColor.addProperty(gui, Property_ComboBox_BackgroundColor);
    textColor.addProperty(gui, Property_ComboBox_TextColor);
    selectedBackgroundColor.addProperty(gui, Property_ComboBox_SelectedBackgroundColor);
    selectedTextColor.addProperty(gui, Property_ComboBox_SelectedTextColor);
    borderColor.addProperty(gui, Property_ComboBox_BorderColor);
    items.addProperty(gui, Property_ComboBox_Items);
    selectedItem.addProperty(gui, Property_ComboBox_SelectedItem);
    itemsToDisplay.addProperty(gui, Property_ComboBox_ItemsToDisplay);
    textFont.addProperty(gui, Property_ComboBox_TextFont);
    callbackID.addProperty(gui, Property_ComboBox_CallbackID);
}

void PropertiesComboBox::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_ComboBox_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_ComboBox_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default combo box is no longer used
            loadedDefaultComboBox = false;

            // Load the new combo box
            tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));
            comboBox->load(configFile.value);

            // Change the height property
            height.value = static_cast<float>(comboBox->getSize().y);

            // Update the height value
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default combo box is not loaded already then load it now
            if (loadedDefaultComboBox == false)
            {
                // The default combo box is loaded again
                loadedDefaultComboBox = true;

                // Load the default combo box
                tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));
                comboBox->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                comboBox->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_ComboBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the combo box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_ComboBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the combo box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_ComboBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the size of the combo box
        static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_ComboBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the size of the combo box
        static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_ComboBox_Borders)
    {
        // Store the new borders
        borders.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Borders"))->getText();

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
        static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_ComboBox_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BackgroundColor"))->getText();

        // Get a pointer to the combo box
        tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));

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
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

        // Get a pointer to the combo box
        tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));

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
        selectedBackgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedBackgroundColor"))->getText();

        // Get a pointer to the combo box
        tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));

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
        selectedTextColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedTextColor"))->getText();

        // Get a pointer to the combo box
        tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));

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
        borderColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BorderColor"))->getText();

        // Get a pointer to the combo box
        tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));

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
        items.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Items"))->getText();

        // Remove all objects from the combo box
        tgui::ComboBox::Ptr comboBox = formWindow.get(tgui::to_string(id));
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
        if (selectedItem.value >= 0)
        {
            // Change the selected item
            if (comboBox->setSelectedItem(selectedItem.value) == false)
            {
                // If the id was too high then set the property back to -1
                selectedItem.value = -1;
                static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedItem"))->setText("-1");
            }
        }
    }
    else if (propertyNumber == Property_ComboBox_SelectedItem)
    {
        // Store the new selected item
        selectedItem.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedItem"))->getText().toAnsiString().c_str());

        // Change the selected item in the combo box
        if (selectedItem.value >= 0)
            static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSelectedItem(selectedItem.value);
        else
            static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->deselectItem();
    }
    else if (propertyNumber == Property_ComboBox_ItemsToDisplay)
    {
        // Store the new amount of item to display
        itemsToDisplay.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ItemsToDisplay"))->getText().toAnsiString().c_str());

        // Change the amount of item to display in the combo box
        static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->setItemsToDisplay(itemsToDisplay.value);
    }
    else if (propertyNumber == Property_ComboBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_ComboBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        static_cast<tgui::ComboBox::Ptr>(formWindow.get(tgui::to_string(id)))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesSlider::PropertiesSlider()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
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

void PropertiesSlider::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Slider_Name);
    configFile.addProperty(gui, Property_Slider_ConfigFile);
    left.addProperty(gui, Property_Slider_Left);
    top.addProperty(gui, Property_Slider_Top);
    width.addProperty(gui, Property_Slider_Width);
    height.addProperty(gui, Property_Slider_Height);
    value.addProperty(gui, Property_Slider_Value);
    minimum.addProperty(gui, Property_Slider_Minimum);
    maximum.addProperty(gui, Property_Slider_Maximum);
    callbackID.addProperty(gui, Property_Slider_CallbackID);

    verticalScroll.addProperty(gui, Property_Slider_VerticalScroll);
}

void PropertiesSlider::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Slider_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Slider_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default slider is no longer used
            loadedDefaultSlider = false;

            // Load the new slider
            tgui::Slider::Ptr slider = formWindow.get(tgui::to_string(id));
            slider->load(configFile.value);

            // Reset the size
            slider->setSize(static_cast<float>(slider->getSize().x), static_cast<float>(slider->getSize().y));

            // Change the width and height properties
            width.value = static_cast<float>(slider->getSize().x);
            height.value = static_cast<float>(slider->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default slider is not loaded already then load it now
            if (loadedDefaultSlider == false)
            {
                // The default slider is loaded again
                loadedDefaultSlider = true;

                // Load the default slider
                tgui::Slider::Ptr slider = formWindow.get(tgui::to_string(id));
                slider->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                slider->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Slider_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the slider
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Slider_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the slider
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Slider_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the slider
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Slider_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the slider
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Slider_VerticalScroll)
    {
        // Change the direction of the slider
        verticalScroll.value = static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_VerticalScroll"))->getSelectedItemIndex();

        // Get a pointer to the slider and to the width and height properties
        tgui::Slider::Ptr slider = formWindow.get(tgui::to_string(id));
        tgui::EditBox::Ptr propertyWidth = propertyWindow.get("text_Width");
        tgui::EditBox::Ptr propertyHeight = propertyWindow.get("text_Height");

        // Change the direction
        slider->setVerticalScroll(!!verticalScroll.value);

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
        value.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());

        // Change the value of the slider
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(value.value);
    }
    else if (propertyNumber == Property_Slider_Minimum)
    {
        // Store the minimum value
        minimum.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Minimum"))->getText().toAnsiString().c_str());

        // Change the minimum value of the slider
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setMinimum(minimum.value);

        // Reset the value
        int val = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(val);
    }
    else if (propertyNumber == Property_Slider_Maximum)
    {
        // Store the maximum value
        maximum.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Maximum"))->getText().toAnsiString().c_str());

        // Change the maximum value of the slider
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximum(maximum.value);

        // Reset the value
        int val = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());
        static_cast<tgui::Slider::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(val);
    }
    else if (propertyNumber == Property_Slider_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesScrollbar::PropertiesScrollbar()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
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

void PropertiesScrollbar::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_Scrollbar_Name);
    configFile.addProperty(gui, Property_Scrollbar_ConfigFile);
    left.addProperty(gui, Property_Scrollbar_Left);
    top.addProperty(gui, Property_Scrollbar_Top);
    width.addProperty(gui, Property_Scrollbar_Width);
    height.addProperty(gui, Property_Scrollbar_Height);
    value.addProperty(gui, Property_Scrollbar_Value);
    lowValue.addProperty(gui, Property_Scrollbar_LowValue);
    maximum.addProperty(gui, Property_Scrollbar_Maximum);
    callbackID.addProperty(gui, Property_Scrollbar_CallbackID);

    verticalScroll.addProperty(gui, Property_Scrollbar_VerticalScroll);
}

void PropertiesScrollbar::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_Scrollbar_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_Scrollbar_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default scrollbar is no longer used
            loadedDefaultScrollbar = false;

            // Load the new scrollbar
            tgui::Scrollbar::Ptr scrollbar = formWindow.get(tgui::to_string(id));
            scrollbar->load(configFile.value);

            // Reset the size
            scrollbar->setSize(static_cast<float>(scrollbar->getSize().x), static_cast<float>(scrollbar->getSize().y));

            // Change the width and height properties
            width.value = static_cast<float>(scrollbar->getSize().x);
            height.value = static_cast<float>(scrollbar->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default scrollbar is not loaded already then load it now
            if (loadedDefaultScrollbar == false)
            {
                // The default scrollbar is loaded again
                loadedDefaultScrollbar = true;

                // Load the default scrollbar
                tgui::Scrollbar::Ptr scrollbar = formWindow.get(tgui::to_string(id));
                scrollbar->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                scrollbar->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_Scrollbar_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the scrollbar
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Scrollbar_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the scrollbar
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_Scrollbar_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the scrollbar
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Scrollbar_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the scrollbar
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_Scrollbar_VerticalScroll)
    {
        // Change the direction of the scrollbar
        verticalScroll.value = static_cast<tgui::ComboBox::Ptr>(propertyWindow.get("combo_VerticalScroll"))->getSelectedItemIndex();

        // Get a pointer to the width and height properties
        tgui::EditBox::Ptr propertyWidth = propertyWindow.get("text_Width");
        tgui::EditBox::Ptr propertyHeight = propertyWindow.get("text_Height");

        // Change the direction
        tgui::Scrollbar::Ptr scrollbar = formWindow.get(tgui::to_string(id));
        scrollbar->setVerticalScroll(!!verticalScroll.value);

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
        value.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());

        // The value may not be too high
        if (maximum.value >= lowValue.value)
        {
            if (value.value > maximum.value - lowValue.value)
            {
                // Adjust the value
                value.value = maximum.value - lowValue.value;

                // Adjust the property
                static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->setText(tgui::to_string(value.value));
            }
        }
        else
        {
            // Adjust the value
            value.value = 0;
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->setText("0");
        }

        // Change the value of the scrollbar
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(value.value);
    }
    else if (propertyNumber == Property_Scrollbar_LowValue)
    {
        // Store the low value
        lowValue.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_LowValue"))->getText().toAnsiString().c_str());

        // Change the low value of the scrollbar
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setLowValue(lowValue.value);

        // Reset the value
        int val = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(val);
    }
    else if (propertyNumber == Property_Scrollbar_Maximum)
    {
        // Store the maximum value
        maximum.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Maximum"))->getText().toAnsiString().c_str());

        // Change the maximum value of the scrollbar
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximum(maximum.value);

        // Reset the value
        int val = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());
        static_cast<tgui::Scrollbar::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(val);
    }
    else if (propertyNumber == Property_Scrollbar_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesLoadingBar::PropertiesLoadingBar()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
    left.value = 0;
    top.value = 0;
    width.value = 400;
    height.value = 40;
    value.value = 0;
    minimum.value = 0;
    maximum.value = 100;
    callbackID.value = 0;

    loadedDefaultLoadingBar = true;
}

void PropertiesLoadingBar::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_LoadingBar_Name);
    configFile.addProperty(gui, Property_LoadingBar_ConfigFile);
    left.addProperty(gui, Property_LoadingBar_Left);
    top.addProperty(gui, Property_LoadingBar_Top);
    width.addProperty(gui, Property_LoadingBar_Width);
    height.addProperty(gui, Property_LoadingBar_Height);
    value.addProperty(gui, Property_LoadingBar_Value);
    minimum.addProperty(gui, Property_LoadingBar_Minimum);
    maximum.addProperty(gui, Property_LoadingBar_Maximum);
    callbackID.addProperty(gui, Property_LoadingBar_CallbackID);
}

void PropertiesLoadingBar::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_LoadingBar_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_LoadingBar_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists(configFile.value.c_str()))
        {
            // The default loading bar is no longer used
            loadedDefaultLoadingBar = false;

            // Load the new loading bar
            tgui::LoadingBar::Ptr loadingBar = formWindow.get(tgui::to_string(id));
            loadingBar->load(configFile.value);

            // Change the width and height properties
            width.value = static_cast<float>(loadingBar->getSize().x);
            height.value = static_cast<float>(loadingBar->getSize().y);

            // Update the values
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->setText(tgui::to_string(width.value));
            static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->setText(tgui::to_string(height.value));
        }
        else // The file didn't exist
        {
            // When the default loading bar is not loaded already then load it now
            if (loadedDefaultLoadingBar == false)
            {
                // The default loading bar is loaded again
                loadedDefaultLoadingBar = true;

                // Load the default loading bar
                tgui::LoadingBar::Ptr loadingBar = formWindow.get(tgui::to_string(id));
                loadingBar->load(DEFAULT_THEME_FILE);

                // Adjust the scaling
                loadingBar->setSize(width.value, height.value);
            }
        }
    }
    else if (propertyNumber == Property_LoadingBar_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the loading bar
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_LoadingBar_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the loading bar
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_LoadingBar_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the loading bar
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_LoadingBar_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the loading bar
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_LoadingBar_Value)
    {
        // Store the value
        value.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());

        // Change the value of the loading bar
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(value.value);
    }
    else if (propertyNumber == Property_LoadingBar_Minimum)
    {
        // Store the minimum value
        minimum.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Minimum"))->getText().toAnsiString().c_str());

        // Change the minimum value of the loading bar
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setMinimum(minimum.value);

        // Reset the maximum
        int max = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Maximum"))->getText().toAnsiString().c_str());
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximum(max);

        // Reset the value
        int val = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(val);
    }
    else if (propertyNumber == Property_LoadingBar_Maximum)
    {
        // Store the maximum value
        maximum.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Maximum"))->getText().toAnsiString().c_str());

        // Change the maximum value of the loading bar
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximum(maximum.value);

        // Reset the minimum
        int min = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Minimum"))->getText().toAnsiString().c_str());
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setMinimum(min);

        // Reset the value
        int val = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Value"))->getText().toAnsiString().c_str());
        static_cast<tgui::LoadingBar::Ptr>(formWindow.get(tgui::to_string(id)))->setValue(val);
    }
    else if (propertyNumber == Property_LoadingBar_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesTextBox::PropertiesTextBox()
{
    id = 0;
    name.value = "";
    configFile.value = DEFAULT_THEME_FILE;
    left.value = 0;
    top.value = 0;
    width.value = 320;
    height.value = 172;
    text.value = "";
    textSize.value = 24;
    textFont.value = "Global";
    maximumCharacters.value = 0;
    borders.value = "2, 2, 2, 2";
    backgroundColor.value = " 50, 50, 50";
    textColor.value = "200, 200, 200";
    selectedTextColor.value = "255, 255, 255";
    selectedTextBackgroundColor.value = "10, 110, 255";
    borderColor.value = "0, 0, 0";
    selectionPointColor.value = "110, 110, 255";
    selectionPointWidth.value = 2;
    callbackID.value = 0;

    loadedDefaultScrollbar = true;
}

void PropertiesTextBox::addProperties(tgui::Gui& gui)
{
    name.addProperty(gui, Property_TextBox_Name);
    configFile.addProperty(gui, Property_TextBox_ConfigFile);
    left.addProperty(gui, Property_TextBox_Left);
    top.addProperty(gui, Property_TextBox_Top);
    width.addProperty(gui, Property_TextBox_Width);
    height.addProperty(gui, Property_TextBox_Height);
    text.addProperty(gui, Property_TextBox_Text);
    textSize.addProperty(gui, Property_TextBox_TextSize);
    textFont.addProperty(gui, Property_TextBox_TextFont);
    maximumCharacters.addProperty(gui, Property_TextBox_MaximumCharacters);
    borders.addProperty(gui, Property_TextBox_Borders);
    backgroundColor.addProperty(gui, Property_TextBox_BackgroundColor);
    textColor.addProperty(gui, Property_TextBox_TextColor);
    selectedTextColor.addProperty(gui, Property_TextBox_SelectedTextColor);
    selectedTextBackgroundColor.addProperty(gui, Property_TextBox_SelectedTextBackgroundColor);
    borderColor.addProperty(gui, Property_TextBox_BorderColor);
    selectionPointColor.addProperty(gui, Property_TextBox_SelectionPointColor);
    selectionPointWidth.addProperty(gui, Property_TextBox_SelectionPointWidth);
    callbackID.addProperty(gui, Property_TextBox_CallbackID);
}

void PropertiesTextBox::updateProperty(tgui::Gui& formWindow, tgui::Gui& propertyWindow, unsigned int propertyNumber)
{
    // Check what the property is
    if (propertyNumber == Property_TextBox_Name)
    {
        name.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Name"))->getText();
    }
    else if (propertyNumber == Property_ComboBox_ConfigFile)
    {
        configFile.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_ConfigFile"))->getText();

        // Check if the config file exists
        if (checkIfFileExists((configFile.value).c_str()))
        {
            tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));
            textBox->load(configFile.value);
        }
    }
    else if (propertyNumber == Property_TextBox_Left)
    {
        // Store the new position
        left.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Left"))->getText().toAnsiString().c_str()));

        // Change the position of the text box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_TextBox_Top)
    {
        // Store the new position
        top.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Top"))->getText().toAnsiString().c_str()));

        // Change the position of the text box
        formWindow.get(tgui::to_string(id))->setPosition(left.value, top.value);
    }
    else if (propertyNumber == Property_TextBox_Width)
    {
        // Store the new width
        width.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Width"))->getText().toAnsiString().c_str()));

        // Change the width of the text box
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_TextBox_Height)
    {
        // Store the new height
        height.value = static_cast<float>(atof(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Height"))->getText().toAnsiString().c_str()));

        // Change the height of the text box
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSize(width.value, height.value);
    }
    else if (propertyNumber == Property_TextBox_Text)
    {
        // Store the new text
        text.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->getText();

        // Change the text of the text box
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setText(text.value);
    }
    else if (propertyNumber == Property_TextBox_TextSize)
    {
        // Store the new text size
        textSize.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextSize"))->getText().toAnsiString().c_str());

        // Change the text of the text box
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setTextSize(textSize.value);
    }
    else if (propertyNumber == Property_TextBox_TextFont)
    {
/*
        // Store the new text font
        textFont.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->getText();

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
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextFont"))->setText("Global");
    }
    else if (propertyNumber == Property_TextBox_MaximumCharacters)
    {
        // Store the new maximum
        maximumCharacters.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_MaximumCharacters"))->getText().toAnsiString().c_str());

        // Set the maximum characters
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setMaximumCharacters(maximumCharacters.value);
        static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Text"))->setMaximumCharacters(maximumCharacters.value);
    }
    else if (propertyNumber == Property_TextBox_Borders)
    {
        // Store the new borders
        borders.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_Borders"))->getText();

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
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setBorders(border.x1, border.x2, border.x3, border.x4);
    }
    else if (propertyNumber == Property_TextBox_BackgroundColor)
    {
        // Store the new background color
        backgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BackgroundColor"))->getText();

        // Get the pointer to the text box
        tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));

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
        textColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_TextColor"))->getText();

        // Get the pointer to the text box
        tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));

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
        selectedTextColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedTextColor"))->getText();

        // Get the pointer to the text box
        tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));

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
        selectedTextBackgroundColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectedTextBackgroundColor"))->getText();

        // Get the pointer to the text box
        tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));

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
    else if (propertyNumber == Property_TextBox_BorderColor)
    {
        // Store the new border color
        borderColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_BorderColor"))->getText();

        // Get the pointer to the text box
        tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));

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
        selectionPointColor.value = static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectionPointColor"))->getText();

        // Get the pointer to the text box
        tgui::TextBox::Ptr textBox = formWindow.get(tgui::to_string(id));

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
            textBox->setSelectionPointColor(color);
        }
        else // The string is empty, so use the default color
            textBox->setSelectionPointColor(sf::Color::White);

    }
    else if (propertyNumber == Property_TextBox_SelectionPointWidth)
    {
        // Store the new width
        selectionPointWidth.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_SelectionPointWidth"))->getText().toAnsiString().c_str());

        // Set the new width
        static_cast<tgui::TextBox::Ptr>(formWindow.get(tgui::to_string(id)))->setSelectionPointWidth(selectionPointWidth.value);
    }
    else if (propertyNumber == Property_TextBox_CallbackID)
    {
        // Store the new callback id
        callbackID.value = atoi(static_cast<tgui::EditBox::Ptr>(propertyWindow.get("text_CallbackID"))->getText().toAnsiString().c_str());

        // Change the callback id
        formWindow.get(tgui::to_string(id))->setCallbackId(callbackID.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

