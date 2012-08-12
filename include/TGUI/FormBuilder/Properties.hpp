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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Properties
{
    Property_Window_Name = 0,
    Property_Window_Width,
    Property_Window_Height,
    Property_Window_GlobalFont,

    Property_Picture_Name = 0,
    Property_Picture_Filename,
    Property_Picture_Left,
    Property_Picture_Top,
    Property_Picture_Width,
    Property_Picture_Height,
    Property_Picture_CallbackID,

    Property_Button_Name = 0,
    Property_Button_Pathname,
    Property_Button_Left,
    Property_Button_Top,
    Property_Button_Width,
    Property_Button_Height,
    Property_Button_Text,
    Property_Button_TextSize,
    Property_Button_TextColor,
    Property_Button_TextFont,
    Property_Button_CallbackID,

    Property_Checkbox_Name = 0,
    Property_Checkbox_Pathname,
    Property_Checkbox_Left,
    Property_Checkbox_Top,
    Property_Checkbox_Width,
    Property_Checkbox_Height,
    Property_Checkbox_Checked,
    Property_Checkbox_Text,
    Property_Checkbox_TextSize,
    Property_Checkbox_TextColor,
    Property_Checkbox_TextFont,
    Property_Checkbox_CallbackID,

    Property_Label_Name = 0,
    Property_Label_Left,
    Property_Label_Top,
    Property_Label_Width,
    Property_Label_Height,
    Property_Label_AutoSize,
    Property_Label_Text,
    Property_Label_TextSize,
    Property_Label_TextColor,
    Property_Label_TextFont,
    Property_Label_BackgroundColor,
    Property_Label_CallbackID,

    Property_EditBox_Name = 0,
    Property_EditBox_Pathname,
    Property_EditBox_Left,
    Property_EditBox_Top,
    Property_EditBox_Width,
    Property_EditBox_Height,
    Property_EditBox_Text,
    Property_EditBox_TextSize,
    Property_EditBox_TextFont,
    Property_EditBox_PasswordChar,
    Property_EditBox_MaximumCharacters,
    Property_EditBox_Borders,
    Property_EditBox_TextColor,
    Property_EditBox_SelectedTextColor,
    Property_EditBox_SelectedTextBackgroundColor,
    Property_EditBox_SelectionPointColor,
    Property_EditBox_SelectionPointWidth,
    Property_EditBox_CallbackID,

    Property_Listbox_Name = 0,
    Property_Listbox_Left,
    Property_Listbox_Top,
    Property_Listbox_Width,
    Property_Listbox_Height,
    Property_Listbox_ScrollbarPathname,
    Property_Listbox_ItemHeight,
    Property_Listbox_MaximumItems,
    Property_Listbox_Borders,
    Property_Listbox_BackgroundColor,
    Property_Listbox_TextColor,
    Property_Listbox_SelectedBackgroundColor,
    Property_Listbox_SelectedTextColor,
    Property_Listbox_BorderColor,
    Property_Listbox_Items,
    Property_Listbox_SelectedItem,
    Property_Listbox_TextFont,
    Property_Listbox_CallbackID,

    Property_ComboBox_Name = 0,
    Property_ComboBox_Pathname,
    Property_ComboBox_Left,
    Property_ComboBox_Top,
    Property_ComboBox_Width,
    Property_ComboBox_Height,
    Property_ComboBox_ScrollbarPathname,
    Property_ComboBox_Borders,
    Property_ComboBox_BackgroundColor,
    Property_ComboBox_TextColor,
    Property_ComboBox_SelectedBackgroundColor,
    Property_ComboBox_SelectedTextColor,
    Property_ComboBox_BorderColor,
    Property_ComboBox_Items,
    Property_ComboBox_SelectedItem,
    Property_ComboBox_ItemsToDisplay,
    Property_ComboBox_TextFont,
    Property_ComboBox_CallbackID,

    Property_Slider_Name = 0,
    Property_Slider_Pathname,
    Property_Slider_Left,
    Property_Slider_Top,
    Property_Slider_Width,
    Property_Slider_Height,
    Property_Slider_VerticalScroll,
    Property_Slider_Value,
    Property_Slider_Minimum,
    Property_Slider_Maximum,
    Property_Slider_CallbackID,

    Property_Scrollbar_Name = 0,
    Property_Scrollbar_Pathname,
    Property_Scrollbar_Left,
    Property_Scrollbar_Top,
    Property_Scrollbar_Width,
    Property_Scrollbar_Height,
    Property_Scrollbar_VerticalScroll,
    Property_Scrollbar_Value,
    Property_Scrollbar_LowValue,
    Property_Scrollbar_Maximum,
    Property_Scrollbar_CallbackID,

    Property_LoadingBar_Name = 0,
    Property_LoadingBar_Pathname,
    Property_LoadingBar_Left,
    Property_LoadingBar_Top,
    Property_LoadingBar_Width,
    Property_LoadingBar_Height,
    Property_LoadingBar_Value,
    Property_LoadingBar_Minimum,
    Property_LoadingBar_Maximum,
    Property_LoadingBar_CallbackID,

    Property_TextBox_Name = 0,
    Property_TextBox_Left,
    Property_TextBox_Top,
    Property_TextBox_Width,
    Property_TextBox_Height,
    Property_TextBox_ScrollbarPathname,
    Property_TextBox_Text,
    Property_TextBox_TextSize,
    Property_TextBox_TextFont,
    Property_TextBox_MaximumCharacters,
    Property_TextBox_Borders,
    Property_TextBox_BackgroundColor,
    Property_TextBox_TextColor,
    Property_TextBox_SelectedTextColor,
    Property_TextBox_SelectedTextBackgroundColor,
    Property_TextBox_BorderColor,
    Property_TextBox_SelectionPointColor,
    Property_TextBox_SelectionPointWidth,
    Property_TextBox_CallbackID
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Property_Type
{
    Type_ValueUnsignedInterger = 1,
    Type_ValueFloat
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Property
{
    virtual void addProperty(tgui::Window& window, unsigned int propertyNumber) = 0;

    std::string   description;
    unsigned int  type;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyValueString : public Property
{
    std::string value;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyValueUnsignedInt : public Property
{
    unsigned int value;
};

typedef PropertyValueUnsignedInt PropertyValueBool;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyValueChar : public Property
{
    char value;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyValueFloat : public Property
{
    float value;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyName : public PropertyValueString
{
    PropertyName();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyFilename : public PropertyValueString
{
    PropertyFilename();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyPathname : public PropertyValueString
{
    PropertyPathname();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyLeft : public PropertyValueFloat
{
    PropertyLeft();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyTop : public PropertyValueFloat
{
    PropertyTop();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyWindowWidth : public PropertyValueUnsignedInt
{
    PropertyWindowWidth();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyWindowHeight : public PropertyValueUnsignedInt
{
    PropertyWindowHeight();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyWidth : public PropertyValueFloat
{
    PropertyWidth();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyHeight : public PropertyValueFloat
{
    PropertyHeight();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyAutoSize : public PropertyValueBool
{
    PropertyAutoSize();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyText : public PropertyValueString
{
    PropertyText();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyTextSize : public PropertyValueUnsignedInt
{
    PropertyTextSize();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyTextColor : public PropertyValueString
{
    PropertyTextColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertySelectedTextColor : public PropertyValueString
{
    PropertySelectedTextColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertySelectedTextBackgroundColor : public PropertyValueString
{
    PropertySelectedTextBackgroundColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyTextFont : public PropertyValueString
{
    PropertyTextFont();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyGlobalFont : public PropertyValueString
{
    PropertyGlobalFont();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyChecked : public PropertyValueBool
{
    PropertyChecked();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyPasswordChar : public PropertyValueChar
{
    PropertyPasswordChar();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyMaximumCharacters : public PropertyValueUnsignedInt
{
    PropertyMaximumCharacters();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyBorders : public PropertyValueString
{
    PropertyBorders();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertySelectionPointColor : public PropertyValueString
{
    PropertySelectionPointColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertySelectionPointWidth : public PropertyValueUnsignedInt
{
    PropertySelectionPointWidth();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyScrollbarPathname : public PropertyValueString
{
    PropertyScrollbarPathname();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyItemHeight : public PropertyValueUnsignedInt
{
    PropertyItemHeight();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyMaximumItems : public PropertyValueUnsignedInt
{
    PropertyMaximumItems();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyBackgroundColor : public PropertyValueString
{
    PropertyBackgroundColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertySelectedBackgroundColor : public PropertyValueString
{
    PropertySelectedBackgroundColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyItems : public PropertyValueString
{
    PropertyItems();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertySelectedItem : public PropertyValueUnsignedInt
{
    PropertySelectedItem();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyItemsToDisplay : public PropertyValueUnsignedInt
{
    PropertyItemsToDisplay();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyBorderColor : public PropertyValueString
{
    PropertyBorderColor();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyValue : public PropertyValueUnsignedInt
{
    PropertyValue();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyLowValue : public PropertyValueUnsignedInt
{
    PropertyLowValue();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyMinimum : public PropertyValueUnsignedInt
{
    PropertyMinimum();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyMaximum : public PropertyValueUnsignedInt
{
    PropertyMaximum();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyVerticalScroll : public PropertyValueBool
{
    PropertyVerticalScroll();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyCallbackID : public PropertyValueUnsignedInt
{
    PropertyCallbackID();
    void addProperty(tgui::Window& window, unsigned int propertyNumber);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesWindow
{
    PropertiesWindow();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int         id;
    PropertyName         name;
    PropertyWindowWidth  width;
    PropertyWindowHeight height;
    PropertyGlobalFont   globalFont;

    Builder* builder;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesPicture
{
    PropertiesPicture();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int      id;
    PropertyName      name;
    PropertyFilename  filename;
    PropertyLeft      left;
    PropertyTop       top;
    PropertyWidth     width;
    PropertyHeight    height;
    PropertyCallbackID callbackID;

    // True when the default picture is already loaded
    bool loadedDefaultPicture;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesButton
{
    PropertiesButton();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int       id;
    PropertyName       name;
    PropertyPathname   pathname;
    PropertyLeft       left;
    PropertyTop        top;
    PropertyWidth      width;
    PropertyHeight     height;
    PropertyText       text;
    PropertyTextSize   textSize;
    PropertyTextColor  textColor;
    PropertyTextFont   textFont;
    PropertyCallbackID callbackID;

    // True when the default button is already loaded
    bool loadedDefaultButton;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesCheckbox
{
    PropertiesCheckbox();
    void addProperties(tgui::Window& window);
    virtual void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int       id;
    PropertyName       name;
    PropertyPathname   pathname;
    PropertyLeft       left;
    PropertyTop        top;
    PropertyWidth      width;
    PropertyHeight     height;
    PropertyText       text;
    PropertyTextSize   textSize;
    PropertyTextColor  textColor;
    PropertyTextFont   textFont;
    PropertyChecked    checked;
    PropertyCallbackID callbackID;

    // True when the default checkbox is already loaded
    bool loadedDefaultCheckbox;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesRadioButton : PropertiesCheckbox
{
    PropertiesRadioButton();
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    Builder* builder;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesLabel
{
    PropertiesLabel();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int            id;
    PropertyName            name;
    PropertyLeft            left;
    PropertyTop             top;
    PropertyWidth           width;
    PropertyHeight          height;
    PropertyAutoSize        autoSize;
    PropertyText            text;
    PropertyTextSize        textSize;
    PropertyTextColor       textColor;
    PropertyTextFont        textFont;
    PropertyBackgroundColor backgroundColor;
    PropertyCallbackID      callbackID;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesEditBox
{
    PropertiesEditBox();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int                id;
    PropertyName                name;
    PropertyPathname            pathname;
    PropertyLeft                left;
    PropertyTop                 top;
    PropertyWidth               width;
    PropertyHeight              height;
    PropertyText                text;
    PropertyTextSize            textSize;
    PropertyTextFont            textFont;
    PropertyPasswordChar        passwordChar;
    PropertyMaximumCharacters   maximumCharacters;
    PropertyBorders             borders;
    PropertyTextColor           textColor;
    PropertySelectedTextColor   selectedTextColor;
    PropertySelectedTextBackgroundColor selectedTextBackgroundColor;
    PropertySelectionPointColor selectionPointColor;
    PropertySelectionPointWidth selectionPointWidth;
    PropertyCallbackID          callbackID;

    // True when the default edit box is already loaded
    bool loadedDefaultEditBox;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesListbox
{
    PropertiesListbox();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int              id;
    PropertyName              name;
    PropertyLeft              left;
    PropertyTop               top;
    PropertyWidth             width;
    PropertyHeight            height;
    PropertyScrollbarPathname scrollbarPathname;
    PropertyItemHeight        itemHeight;
    PropertyMaximumItems      maximumItems;
    PropertyBorders           borders;
    PropertyBackgroundColor   backgroundColor;
    PropertyTextColor         textColor;
    PropertySelectedBackgroundColor selectedBackgroundColor;
    PropertySelectedTextColor selectedTextColor;
    PropertyBorderColor       borderColor;
    PropertyItems             items;
    PropertySelectedItem      selectedItem;
    PropertyTextFont          textFont;
    PropertyCallbackID        callbackID;

    // True when the default scrollbar is already loaded
    bool loadedDefaultScrollbar;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesComboBox
{
    PropertiesComboBox();
    void addProperties(tgui::Window& window);
    void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int               id;
    PropertyName               name;
    PropertyPathname           pathname;
    PropertyLeft               left;
    PropertyTop                top;
    PropertyWidth              width;
    PropertyHeight             height;
    PropertyScrollbarPathname  scrollbarPathname;
    PropertyBorders            borders;
    PropertyBackgroundColor    backgroundColor;
    PropertyTextColor          textColor;
    PropertySelectedBackgroundColor selectedBackgroundColor;
    PropertySelectedTextColor  selectedTextColor;
    PropertyBorderColor        borderColor;
    PropertyItems              items;
    PropertySelectedItem       selectedItem;
    PropertyItemsToDisplay     itemsToDisplay;
    PropertyTextFont           textFont;
    PropertyCallbackID         callbackID;

    // True when the default combo box and scrollbar are already loaded
    bool loadedDefaultComboBox;
    bool loadedDefaultScrollbar;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesSlider
{
    PropertiesSlider();
    void addProperties(tgui::Window& window);
    virtual void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int           id;
    PropertyName           name;
    PropertyPathname       pathname;
    PropertyLeft           left;
    PropertyTop            top;
    PropertyWidth          width;
    PropertyHeight         height;
    PropertyVerticalScroll verticalScroll;
    PropertyValue          value;
    PropertyMinimum        minimum;
    PropertyMaximum        maximum;
    PropertyCallbackID    callbackID;

    // True when the default slider is already loaded
    bool loadedDefaultSlider;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesScrollbar
{
    PropertiesScrollbar();
    void addProperties(tgui::Window& window);
    virtual void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int           id;
    PropertyName           name;
    PropertyPathname       pathname;
    PropertyLeft           left;
    PropertyTop            top;
    PropertyWidth          width;
    PropertyHeight         height;
    PropertyVerticalScroll verticalScroll;
    PropertyValue          value;
    PropertyLowValue       lowValue;
    PropertyMaximum        maximum;
    PropertyCallbackID     callbackID;

    // True when the default scrollbar is already loaded
    bool loadedDefaultScrollbar;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesLoadingBar
{
    PropertiesLoadingBar();
    void addProperties(tgui::Window& window);
    virtual void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int           id;
    PropertyName           name;
    PropertyPathname       pathname;
    PropertyLeft           left;
    PropertyTop            top;
    PropertyWidth          width;
    PropertyHeight         height;
    PropertyValue          value;
    PropertyMinimum        minimum;
    PropertyMaximum        maximum;
    PropertyCallbackID     callbackID;

    // True when the default loading bar is already loaded
    bool loadedDefaultLoadingBar;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertiesTextBox
{
    PropertiesTextBox();
    void addProperties(tgui::Window& window);
    virtual void updateProperty(tgui::Window& formWindow, tgui::Window& propertyWindow, unsigned int propertyNumber);

    unsigned int                id;
    PropertyName                name;
    PropertyLeft                left;
    PropertyTop                 top;
    PropertyWidth               width;
    PropertyHeight              height;
    PropertyScrollbarPathname   scrollbarPathname;
    PropertyText                text;
    PropertyTextSize            textSize;
    PropertyTextFont            textFont;
    PropertyMaximumCharacters   maximumCharacters;
    PropertyBorders             borders;
    PropertyBackgroundColor     backgroundColor;
    PropertyTextColor           textColor;
    PropertySelectedTextColor   selectedTextColor;
    PropertySelectedTextBackgroundColor selectedTextBackgroundColor;
    PropertyBorderColor         borderColor;
    PropertySelectionPointColor selectionPointColor;
    PropertySelectionPointWidth selectionPointWidth;
    PropertyCallbackID          callbackID;

    // True when the default scrollbar is already loaded
    bool loadedDefaultScrollbar;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
