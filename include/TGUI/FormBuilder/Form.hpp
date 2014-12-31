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


#ifndef TGUI_FORM_BUILDER_FORM_HPP
#define TGUI_FORM_BUILDER_FORM_HPP

#include <TGUI/TGUI.hpp>

#include "WidgetData.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SelectionSquares
{
    NoSelectionSquare,
    LeftSelectionSquare,
    TopLeftSelectionSquare,
    TopSelectionSquare,
    TopRightSelectionSquare,
    RightSelectionSquare,
    BottomRightSelectionSquare,
    BottomSelectionSquare,
    BottomLeftSelectionSquare
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Form
{
public:

    Form(tgui::Container& parent,
         const std::string& name,
         float width,
         float height,
         const std::string& filename,
         WidgetData* formData);

    void repositionSelectionSquares();
    SelectionSquares mouseOnSelectionSquares(float x, float y);

    void resizeWidget(float deltaWidth, float deltaHeight);
    void moveWidget(float deltaPosX, float deltaPosY);


public:

    tgui::ChildWindow::Ptr window;

    std::list<WidgetData> widgets;
    const WidgetData* formData;
    WidgetData* activeWidget;

    WidgetData* widgetToCopy;


private:



//  bool saved
};

#endif // TGUI_FORM_BUILDER_FORM_HPP
