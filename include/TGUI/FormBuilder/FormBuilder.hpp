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


#ifndef _FORM_BUILDER_INCLUDED_
#define _FORM_BUILDER_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/TGUI.hpp>

#ifdef __APPLE__
    #include "CoreFoundation/CoreFoundation.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_THEME_FILE "images/widgets/Black.conf"

#define VERSION "v0.2.x - 2013.07.04"


struct Builder;
#include "Properties.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum squares
{
    SQUARE_TOP_LEFT = 1,
    SQUARE_LEFT,
    SQUARE_TOP,
    SQUARE_BOTTOM_LEFT,
    SQUARE_TOP_RIGHT,
    SQUARE_BOTTOM,
    SQUARE_RIGHT,
    SQUARE_BOTTOM_RIGHT
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Builder
{
    // Constructor
    Builder();

    // Every widget needs a uinque id. This function gives them one.
    unsigned int getUniqueID();

    // Adds a new widget to the form
    unsigned int newWidget(unsigned int widgetID, const std::string widgetName = "");

    // Change the properties that are displayed, e.g. when selecting another widget.
    void changeVisibleProperties();

    // When another widget is selected, the property window has to be resized
    void resizePropertyWindow();

    // The contents of an edit box has changed, change the property.
    void updateProperty(unsigned int propertyNumber);

    // Changes the global font, which can be used by all widgets
    void setGlobalFont(const sf::Font& font);

    // Get the id of the clicked widget when you clicked on the window
    unsigned int getClickedWidgetID(sf::Event& event);

    // Get the id of the scale square below the mouse
    unsigned int getScaleSquareWidgetID(float mouseX, float mouseY);

    // Delete the currently selected widget
    void deleteWidget();

    // Move the currently selected widget
    void moveWidgetX(float pixels);
    void moveWidgetY(float pixels);

    // This function is called to resize an widget.
    void resizeWidget(float addToWidth, float addToHeight);

    // Calculates and stores the new aspect ratio of the widget
    void storeWidgetsNewAspectRatio();

    // Load the form
    bool loadForm();

    // Save the form
    void saveForm();

    // The three windows
    sf::RenderWindow mainRenderWindow;
    sf::RenderWindow widgetsRenderWindow;
    sf::RenderWindow propertyRenderWindow;

    tgui::Gui mainWindow;
    tgui::Gui widgetsWindow;
    tgui::Gui propertyWindow;

    // A list of all the widgets on the form
    std::vector<PropertiesWindow>      windows;
    std::vector<PropertiesPicture>     pictures;
    std::vector<PropertiesButton>      buttons;
    std::vector<PropertiesCheckbox>    checkboxes;
    std::vector<PropertiesRadioButton> radioButtons;
    std::vector<PropertiesLabel>       labels;
    std::vector<PropertiesEditBox>     editBoxes;
    std::vector<PropertiesListBox>     listBoxes;
    std::vector<PropertiesComboBox>    comboBoxes;
    std::vector<PropertiesSlider>      sliders;
    std::vector<PropertiesScrollbar>   scrollbars;
    std::vector<PropertiesLoadingBar>  loadingBars;
    std::vector<PropertiesTextBox>     textBoxes;

    // The id of the widget that is currently selected.
    unsigned int currentID;

    // True when an widget is being dragged
    bool draggingWidget;

    // Greater than 0 when dragging one of the scale squares
    unsigned int draggingSquare;

    // Where was the mouse when dragging?
    sf::Vector2f dragPos;

    // Hold the aspect ratios of the widgets
    std::vector<float> aspectRatios;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
