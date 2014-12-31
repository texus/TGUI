/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_FORM_BUILDER_FORM_BUILDER_HPP
#define TGUI_FORM_BUILDER_FORM_BUILDER_HPP

#include <TGUI/TGUI.hpp>

#include <algorithm>
#include <stdexcept>

#include "WidgetData.hpp"
#include "MenuBar.hpp"
#include "Form.hpp"

#ifdef __APPLE__
    #include "CoreFoundation/CoreFoundation.h"
#endif

#define VERSION "v" + tgui::to_string(TGUI_VERSION_MAJOR) + "." + tgui::to_string(TGUI_VERSION_MINOR) + "." + tgui::to_string(TGUI_VERSION_PATCH)
#define DEFAULT_FONT  "DejaVuSans.ttf"

#ifndef WIDGETS_FOLDER
    #define WIDGETS_FOLDER  "widgets"
#endif

#ifndef IMAGES_FOLDER
    #define IMAGES_FOLDER   "images"
#endif

#ifndef DATA_FOLDER
    #define DATA_FOLDER     "."
#endif

#ifndef FONTS_FOLDER
    #define FONTS_FOLDER    "."
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FormBuilder
{
public:

        FormBuilder();
    int run();


private:

    void initMenuBar();
    void initWidgetsData();
    void initWidgetsWindow();
    void initPropertiesWindow();

    void handleEvents();
    void update();
    void draw();

    void createLayer();
    void removeLayer();
    void errorMessage(const std::string& message);

    void resize(unsigned int width, unsigned int height);
    void menuBarCallback(const tgui::Callback& callback);
    void leftMousePressed(int x, int y);
    void moveWidget(int x, int y);
    void scaleWidget(int x, int y);

    void recreateProperties();
    void requestNewForm();
    void createNewForm(const std::string& filename, float width, float height);

    void addWidget(const std::string& widgetName);
    void activateWidget(WidgetData& data);
    void selectDifferentWidget();

    void changeProperty(tgui::EditBox::Ptr value, tgui::EditBox::Ptr property);
    void updateProperties();

private:

    sf::RenderWindow window;
    tgui::Gui gui;
    tgui::Panel::Ptr panel;

    MenuBar menu;

    std::map<std::string, WidgetData> widgetsData;

    std::list<Form> forms;

    Form* activeForm;

    bool draggingWidget;
    SelectionSquares scalingWidget;
    sf::Vector2f oldMousePosition;

    bool propertyChanged;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_FORM_BUILDER_FORM_BUILDER_HPP

