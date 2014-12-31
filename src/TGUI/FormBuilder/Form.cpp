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


#include "../../../include/TGUI/FormBuilder/FormBuilder.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Form::Form(tgui::Container& parent,
           const std::string& name,
           float width,
           float height,
           const std::string& filename,
           WidgetData* formDataPtr) :
    window      (parent, name),
    widgets     (),
    formData    (formDataPtr),
    activeWidget(formDataPtr),
    widgetToCopy(nullptr)
{
    window->load(WIDGETS_FOLDER "/White.conf");
    window->setSize(width, height);
    window->setTitle(filename);
    window->setTitleBarHeight(20);
    window->keepInParent(true);

    tgui::Panel::Ptr panel(*window, "Panel");
    panel->setSize(window->getSize().x, window->getSize().y);
    panel->disable();

    // Initialize the selection squares
    {
        tgui::Button::Ptr square(*window);
        square->load(IMAGES_FOLDER "/square/square.conf");
        square->hide();

        window->copy(square, "LeftSquare");
        window->copy(square, "TopLeftSquare");
        window->copy(square, "TopSquare");
        window->copy(square, "TopRightSquare");
        window->copy(square, "RightSquare");
        window->copy(square, "BottomRightSquare");
        window->copy(square, "BottomSquare");
        window->copy(square, "BottomLeftSquare");

        repositionSelectionSquares();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::repositionSelectionSquares()
{
    tgui::Button::Ptr leftSquare = window->get("LeftSquare");
    tgui::Button::Ptr topLeftSquare = window->get("TopLeftSquare");
    tgui::Button::Ptr topSquare = window->get("TopSquare");
    tgui::Button::Ptr topRightSquare = window->get("TopRightSquare");
    tgui::Button::Ptr rightSquare = window->get("RightSquare");
    tgui::Button::Ptr bottomRightSquare = window->get("BottomRightSquare");
    tgui::Button::Ptr bottomSquare = window->get("BottomSquare");
    tgui::Button::Ptr bottomLeftSquare = window->get("BottomLeftSquare");

    // The squares are only visible when a widget is selected
    if (activeWidget == formData)
    {
        leftSquare->hide();
        topLeftSquare->hide();
        topSquare->hide();
        topRightSquare->hide();
        rightSquare->hide();
        bottomRightSquare->hide();
        bottomSquare->hide();
        bottomLeftSquare->hide();
    }
    else // A widget is selected
    {
        leftSquare->show();
        topLeftSquare->show();
        topSquare->show();
        topRightSquare->show();
        rightSquare->show();
        bottomRightSquare->show();
        bottomSquare->show();
        bottomLeftSquare->show();

        leftSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x - (leftSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y + (activeWidget->widget->getSize().y / 2.0) - (leftSquare->getSize().y / 2.0)))));
        topLeftSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x - (topLeftSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y - (topLeftSquare->getSize().y / 2.0)))));
        topSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x + (activeWidget->widget->getSize().x / 2.0) - (topSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y - (topSquare->getSize().y / 2.0)))));
        topRightSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x + activeWidget->widget->getSize().x - (topRightSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y - (topRightSquare->getSize().y / 2.0)))));
        rightSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x + activeWidget->widget->getSize().x - (rightSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y + (activeWidget->widget->getSize().y / 2.0) - (rightSquare->getSize().y / 2.0)))));
        bottomRightSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x + activeWidget->widget->getSize().x - (bottomRightSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y + activeWidget->widget->getSize().y - (bottomRightSquare->getSize().y / 2.0)))));
        bottomSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x + (activeWidget->widget->getSize().x / 2.0) - (bottomSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y + activeWidget->widget->getSize().y - (bottomSquare->getSize().y / 2.0)))));
        bottomLeftSquare->setPosition(sf::Vector2f(sf::Vector2i(
                                    static_cast<int>(activeWidget->widget->getPosition().x - (bottomLeftSquare->getSize().x / 2.0)),
                                    static_cast<int>(activeWidget->widget->getPosition().y + activeWidget->widget->getSize().y - (bottomLeftSquare->getSize().y / 2.0)))));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SelectionSquares Form::mouseOnSelectionSquares(float x, float y)
{
    if (!window->get("LeftSquare")->isVisible())
        return NoSelectionSquare;
    else if (window->get("LeftSquare")->mouseOnWidget(x, y))
        return LeftSelectionSquare;
    else if (window->get("TopLeftSquare")->mouseOnWidget(x, y))
        return TopLeftSelectionSquare;
    else if (window->get("TopSquare")->mouseOnWidget(x, y))
        return TopSelectionSquare;
    else if (window->get("TopRightSquare")->mouseOnWidget(x, y))
        return TopRightSelectionSquare;
    else if (window->get("RightSquare")->mouseOnWidget(x, y))
        return RightSelectionSquare;
    else if (window->get("BottomRightSquare")->mouseOnWidget(x, y))
        return BottomRightSelectionSquare;
    else if (window->get("BottomSquare")->mouseOnWidget(x, y))
        return BottomSelectionSquare;
    else if (window->get("BottomLeftSquare")->mouseOnWidget(x, y))
        return BottomLeftSelectionSquare;
    else
        return NoSelectionSquare;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::resizeWidget(float widthDiff, float heightDiff)
{
    activeWidget->widget->setSize(activeWidget->widget->getSize().x + widthDiff, activeWidget->widget->getSize().y + heightDiff);

    repositionSelectionSquares();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Form::moveWidget(float xDiff, float yDiff)
{
    activeWidget->widget->setPosition(activeWidget->widget->getPosition().x + xDiff, activeWidget->widget->getPosition().y + yDiff);

    repositionSelectionSquares();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
