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

FormBuilder::FormBuilder() :
    window          (sf::VideoMode(static_cast<unsigned int>(sf::VideoMode::getDesktopMode().width * 0.9f), static_cast<unsigned int>(sf::VideoMode::getDesktopMode().height * 0.9f)), "TGUI Form Builder"),
    gui             (window),
    panel           (gui),
    menu            (gui),
    activeForm      (nullptr),
    draggingWidget  (false),
    scalingWidget   (NoSelectionSquare),
    oldMousePosition(0, 0),
    propertyChanged (false)
{
    gui.setGlobalFont(FONTS_FOLDER "/" DEFAULT_FONT);
    panel->setGlobalFont(gui.getGlobalFont());

    initMenuBar();
    initWidgetsData();

    // Place the widgets on the correct size
    resize(window.getSize().x, window.getSize().y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int FormBuilder::run()
{
    while (window.isOpen())
    {
        handleEvents();
        draw();

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::initMenuBar()
{
    std::ifstream file(DATA_FOLDER "/MenuBar");
    if (!file.is_open())
        throw std::runtime_error("Failed to open '" DATA_FOLDER "/MenuBar'.");

    while (!file.eof())
    {
        std::string menuName;
        std::getline(file, menuName);

        if (!menuName.empty() && menuName[menuName.length()-1] == '\r')
            menuName.erase(menuName.length()-1);

        menu.addMenuOrder(menuName);

        std::ifstream subFile(DATA_FOLDER "/MenuBar_" + menuName);
        if (subFile.is_open())
        {
            while (!subFile.eof())
            {
                std::string menuItemName;
                std::getline(subFile, menuItemName);

                if (!menuItemName.empty() && menuItemName[menuItemName.length()-1] == '\r')
                    menuItemName.erase(menuItemName.length()-1);

                menu.addMenuItemOrder(menuName, menuItemName);
            }
        }
    }

    menu.addMenuItem("File", "New");
    menu.addMenuItem("File", "Exit");
//    menu.addMenuItem("View", "Widgets");
//    menu.addMenuItem("View", "Properties");
//    menu.addMenuItem("About", "TGUI Form Builder");

    gui.get("MenuBar")->bindCallbackEx(&FormBuilder::menuBarCallback, this, tgui::MenuBar::MenuItemClicked);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::initWidgetsData()
{
    widgetsData[""] = WidgetData();
    widgetsData["Button"] = WidgetData(tgui::Button::Ptr());
    widgetsData["ChatBox"] = WidgetData(tgui::ChatBox::Ptr());
    widgetsData["Checkbox"] = WidgetData(tgui::Checkbox::Ptr());
    widgetsData["ComboBox"] = WidgetData(tgui::ComboBox::Ptr());
    widgetsData["EditBox"] = WidgetData(tgui::EditBox::Ptr());
    widgetsData["Label"] = WidgetData(tgui::Label::Ptr());
    widgetsData["LoadingBar"] = WidgetData(tgui::LoadingBar::Ptr());
    widgetsData["ListBox"] = WidgetData(tgui::ListBox::Ptr());
    widgetsData["MenuBar"] = WidgetData(tgui::MenuBar::Ptr());
    widgetsData["Picture"] = WidgetData(tgui::Picture::Ptr());
    widgetsData["RadioButton"] = WidgetData(tgui::RadioButton::Ptr());
    widgetsData["Scrollbar"] = WidgetData(tgui::Scrollbar::Ptr());
    widgetsData["Slider"] = WidgetData(tgui::Slider::Ptr());
    widgetsData["SpinButton"] = WidgetData(tgui::SpinButton::Ptr());
    widgetsData["TextBox"] = WidgetData(tgui::TextBox::Ptr());

    tgui::Label::Ptr(widgetsData["Label"].widget)->setText("Label");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::initWidgetsWindow()
{
    tgui::ChildWindow::Ptr widgetsWindow(*panel, "WidgetsWindow");
    widgetsWindow->load(WIDGETS_FOLDER "/White.conf");
    widgetsWindow->setTitle("Widgets");
    widgetsWindow->setTitleBarHeight(20);
    widgetsWindow->setPosition(10, 30);
    widgetsWindow->setBackgroundColor(sf::Color(220, 220, 220));
    widgetsWindow->keepInParent(true);
    widgetsWindow->bindCallback(tgui::ChildWindow::Closed);
    widgetsWindow->bindCallback([this](){ gui.get("MenuBar")->moveToFront(); }, tgui::ChildWindow::LeftMousePressed);

    unsigned int i = 0;
    for (auto it = ++widgetsData.cbegin(); it != widgetsData.cend(); ++it)
    {
        tgui::Picture::Ptr picture(*widgetsWindow);
        picture->load(IMAGES_FOLDER "/" + it->first + ".png");
        picture->setPosition(10.0f + ((i % 2) * 40), 10.0f + ((i / 2) * 40));
        picture->bindCallback(std::bind(&FormBuilder::addWidget, this, it->first), tgui::Picture::LeftMouseClicked);

        ++i;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::initPropertiesWindow()
{
    tgui::ChildWindow::Ptr propertiesWindow(*panel, "PropertiesWindow");
    propertiesWindow->load(WIDGETS_FOLDER "/White.conf");
    propertiesWindow->setTitle("Properties");
    propertiesWindow->setTitleBarHeight(20);
    propertiesWindow->setBackgroundColor(sf::Color(220, 220, 220));
    propertiesWindow->keepInParent(true);
    propertiesWindow->bindCallback(tgui::ChildWindow::Closed);
    propertiesWindow->bindCallback([this](){ gui.get("MenuBar")->moveToFront(); }, tgui::ChildWindow::LeftMousePressed);
    propertiesWindow->bindCallback([this, propertiesWindow](){ propertiesWindow->unfocusWidgets(); }, tgui::ChildWindow::Unfocused);

    tgui::Grid::Ptr grid(*propertiesWindow, "Grid");
    grid->setPosition(10, 40);

    tgui::ComboBox::Ptr widgetSelector(*propertiesWindow, "WidgetSelector");
    widgetSelector->load(WIDGETS_FOLDER "/White.conf");
    widgetSelector->setSize(grid->getSize().x, 20);
    widgetSelector->setPosition(grid->getPosition().x, grid->getPosition().y - widgetSelector->getSize().y - 10);
    widgetSelector->addItem("Window");
    widgetSelector->setSelectedItem(0);
    widgetSelector->bindCallback([this](){ selectDifferentWidget(); }, tgui::ComboBox::ItemSelected);

    for (auto dataIt = widgetsData.begin(); dataIt != widgetsData.end(); ++dataIt)
    {
        if (dataIt->first != "")
        {
            dataIt->second.properties.push_back(std::make_pair("Name", WidgetData::Property("string")));

            auto list = dataIt->second.widget->getPropertyList();
            for (auto it = list.cbegin(); it != list.cend(); ++it)
            {
                dataIt->second.properties.push_back(std::make_pair(it->first, WidgetData::Property(it->second)));
            }
        }
    }

    widgetsData[""].properties.emplace(widgetsData[""].properties.end(), std::make_pair("Name", WidgetData::Property("string", "Window")));
    widgetsData[""].properties.emplace(widgetsData[""].properties.end(), std::make_pair("Filename", WidgetData::Property("string", activeForm->window->getTitle())));
    widgetsData[""].properties.emplace(widgetsData[""].properties.end(), std::make_pair("Width", WidgetData::Property("uint", tgui::to_string(int(activeForm->window->getSize().x)))));
    widgetsData[""].properties.emplace(widgetsData[""].properties.end(), std::make_pair("Height", WidgetData::Property("uint", tgui::to_string(int(activeForm->window->getSize().y)))));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        gui.handleEvent(event, false);

        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        else if (event.type == sf::Event::Resized)
        {
            resize(event.size.width, event.size.height);
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            if (draggingWidget)
                moveWidget(event.mouseMove.x, event.mouseMove.y);
            else if (scalingWidget != NoSelectionSquare)
                scaleWidget(event.mouseMove.x, event.mouseMove.y);
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            leftMousePressed(event.mouseButton.x, event.mouseButton.y);
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            draggingWidget = false;
            scalingWidget = NoSelectionSquare;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::draw()
{
    window.clear(sf::Color(200, 200, 200));
    gui.draw(false);
    window.display();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::createLayer()
{
    tgui::Panel::Ptr layer(gui, "LayerPanel");
    layer->setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    layer->setBackgroundColor(sf::Color(0, 0, 0, 100));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::removeLayer()
{
    tgui::Panel::Ptr layer = gui.get("LayerPanel");
    if (layer != nullptr)
        gui.remove(layer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::errorMessage(const std::string& message)
{
    createLayer();

    tgui::MessageBox::Ptr messageBox(*tgui::Panel::Ptr(gui.get("LayerPanel")));
    messageBox->load(WIDGETS_FOLDER "/White.conf");
    messageBox->setTitleBarHeight(20);
    messageBox->setText(message);
    messageBox->setTitle("Error");
    messageBox->addButton("Ok");
    messageBox->bindCallback([this](){ removeLayer(); }, tgui::MessageBox::Closed | tgui::MessageBox::ButtonClicked);
    messageBox->setPosition((sf::Vector2f(window.getSize()) - messageBox->getSize()) / 2.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::resize(unsigned int width, unsigned int height)
{
    tgui::MenuBar::Ptr menuBar = gui.get("MenuBar");
    menuBar->setSize(static_cast<float>(width), menuBar->getSize().y);
    menuBar->moveToFront();

    panel->setPosition(0, menuBar->getSize().y);
    panel->setSize(gui.getSize().x, gui.getSize().y - menuBar->getSize().y);

    tgui::Panel::Ptr layer = gui.get("LayerPanel");
    if (layer != nullptr)
    {
        layer->setSize(static_cast<float>(width), static_cast<float>(height));
        layer->moveToFront();
    }

    if (activeForm != nullptr)
    {
        tgui::ChildWindow::Ptr widgetsWindow = panel->get("WidgetsWindow");
        tgui::ChildWindow::Ptr propertiesWindow = panel->get("PropertiesWindow");

        widgetsWindow->setPosition(10, 10);
        widgetsWindow->setSize(90, panel->getSize().y - 20 - widgetsWindow->getTitleBarHeight());

        propertiesWindow->setSize(300, panel->getSize().y - 20 - propertiesWindow->getTitleBarHeight());

        propertiesWindow->setPosition(width - propertiesWindow->getSize().x - propertiesWindow->getBorders().left - propertiesWindow->getBorders().right - 10, 10);

        activeForm->window->setPosition((propertiesWindow->getPosition().x - widgetsWindow->getPosition().x + widgetsWindow->getSize().x - activeForm->window->getSize().x
                                         - activeForm->window->getBorders().left - activeForm->window->getBorders().right) / 2.0f + widgetsWindow->getPosition().x + activeForm->window->getBorders().left,
                                        (panel->getSize().y - activeForm->window->getSize().y - activeForm->window->getBorders().top
                                         - activeForm->window->getBorders().bottom - activeForm->window->getTitleBarHeight()) / 2.0f);

        tgui::ComboBox::Ptr widgetSelector = propertiesWindow->get("WidgetSelector");
        widgetSelector->setItemsToDisplay(static_cast<unsigned int>((propertiesWindow->getSize().y * 17 / 20) / TGUI_MAXIMUM(10, widgetSelector->getSize().y)));
    }

    window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(width), static_cast<float>(height))));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::menuBarCallback(const tgui::Callback& callback)
{
    if (callback.text == "New")
    {
        requestNewForm();
    }
    else if (callback.text == "Load")
    {
        tgui::Panel::Ptr formPanel = activeForm->window->get("Panel");

        formPanel->removeAllWidgets();

    #ifdef __APPLE__
        formPanel->loadWidgetsFromFile("../../../" + activeForm->window->getTitle());
    #else
        formPanel->loadWidgetsFromFile(activeForm->window->getTitle());
    #endif

        activeForm->widgets.clear();
        activeForm->activeWidget = &widgetsData[""];

        recreateProperties();
        activeForm->repositionSelectionSquares();

        tgui::ComboBox::Ptr widgetSelector = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("WidgetSelector");
        widgetSelector->removeAllItems();
        widgetSelector->addItem(activeForm->formData->properties.front().second.value);
        widgetSelector->setSelectedItem(0);

        // Fill the WidgetData structs about the widgets (causes the widgets to be deleted)
        auto& widgets = formPanel->getWidgets();
        for (unsigned int i = 0; i < widgets.size();)
        {
            std::string widgetName;
            formPanel->getWidgetName(widgets[i], widgetName);
            widgetSelector->addItem(widgetName);

            activeForm->widgets.push_back(WidgetData(widgets[i]));
            activeForm->widgets.back().widget->show();

            // Fill the properties of the widget data
            {
                activeForm->widgets.back().properties.push_back(std::make_pair("Name", WidgetData::Property("string", widgetName)));

                auto list = activeForm->widgets.back().widget->getPropertyList();
                for (auto it = list.cbegin(); it != list.cend(); ++it)
                {
                    activeForm->widgets.back().properties.push_back(std::make_pair(it->first, WidgetData::Property(it->second)));
                }
            }
        }

        // Add the widgets again
        for (auto it = activeForm->widgets.cbegin(); it != activeForm->widgets.cend(); ++it)
            formPanel->add(it->widget, it->properties.front().second.value);

        // Remove the edit menu
        menu.removeMenuItem("Edit", "To front");
        menu.removeMenuItem("Edit", "To back");
        menu.removeMenuItem("Edit", "Copy");
        menu.removeMenuItem("Edit", "Remove");
    }
    else if (callback.text == "Save")
    {
    #ifdef __APPLE__
        tgui::Panel::Ptr(activeForm->window->get("Panel"))->saveWidgetsToFile("../../../" + activeForm->window->getTitle());
    #else
        tgui::Panel::Ptr(activeForm->window->get("Panel"))->saveWidgetsToFile(activeForm->window->getTitle());
    #endif
    }
    else if (callback.text == "Exit")
    {
        window.close();
    }
    else if (callback.text == "To front")
    {
        activeForm->activeWidget->widget->moveToFront();
    }
    else if (callback.text == "To back")
    {
        activeForm->activeWidget->widget->moveToBack();
    }
    else if (callback.text == "Copy")
    {
        activeForm->widgetToCopy = activeForm->activeWidget;

        menu.addMenuItem("Edit", "Paste");
    }
    else if (callback.text == "Paste")
    {
        activeForm->widgets.push_back(*activeForm->widgetToCopy);
        activeForm->activeWidget = &activeForm->widgets.back();

        activeForm->activeWidget->widget = activeForm->widgetToCopy->widget.clone();
        tgui::Panel::Ptr(activeForm->window->get("Panel"))->add(activeForm->activeWidget->widget);

        // Get the name of the widget that you are copying
        std::string widgetName;
        tgui::Panel::Ptr(activeForm->window->get("Panel"))->getWidgetName(activeForm->widgetToCopy->widget, widgetName);

        // Set the position of the new widget to (0, 0)
        for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
        {
            if ((it->first == "Left") || (it->first == "Top"))
            {
                activeForm->activeWidget->widget->setProperty(it->first, "0");
            }
        }

        // Give the widget a name and add it to the list
        {
            unsigned int number = 2;
            bool found = true;
            while (found)
            {
                found = false;

                for (auto dataIt = activeForm->widgets.cbegin(); dataIt != activeForm->widgets.cend(); ++dataIt)
                {
                    for (auto propertyIt = dataIt->properties.cbegin(); propertyIt != dataIt->properties.cend(); ++propertyIt)
                    {
                        if (propertyIt->first == "Name")
                        {
                            if (propertyIt->second.value == widgetName + " (" + tgui::to_string(number) + ")")
                            {
                                number++;
                                found = true;
                                break;
                            }
                        }
                    }

                    if (found)
                        break;
                }
            }

            for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
            {
                if (it->first == "Name")
                {
                    it->second.value = widgetName + " (" + tgui::to_string(number) + ")";

                    tgui::Panel::Ptr(activeForm->window->get("Panel"))->setWidgetName(activeForm->activeWidget->widget, it->second.value);

                    tgui::ChildWindow::Ptr propertyWindow = panel->get("PropertiesWindow");
                    tgui::ComboBox::Ptr widgetSelector = propertyWindow->get("WidgetSelector");
                    widgetSelector->addItem(it->second.value);
                    break;
                }
            }
        }

        activeForm->repositionSelectionSquares();

        // Initialize the value for all properties
        for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
            activeForm->activeWidget->widget->getProperty(it->first, it->second.value);

        // The properties of this new widget should be displayed
        recreateProperties();

        // Add edit option to the menu bar
        menu.addMenuItem("Edit", "To front");
        menu.addMenuItem("Edit", "To back");
        menu.addMenuItem("Edit", "Copy");
        menu.addMenuItem("Edit", "Remove");
    }
    else if (callback.text == "Remove")
    {
        for (auto it = activeForm->widgets.begin(); it != activeForm->widgets.end(); ++it)
        {
            if (&*it == activeForm->activeWidget)
            {
                // If the copied widget is being removed then no longer allow pasting it
                if (activeForm->activeWidget == activeForm->widgetToCopy)
                    menu.removeMenuItem("Edit", "Paste");

                std::string widgetName;
                tgui::Panel::Ptr(activeForm->window->get("Panel"))->getWidgetName(it->widget, widgetName);
                tgui::ComboBox::Ptr(tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("WidgetSelector"))->removeItem(widgetName);

                activeForm->widgets.erase(it);
                activeForm->activeWidget = &widgetsData[""];
                activeForm->repositionSelectionSquares();
                recreateProperties();

                // Remove the edit menu
                menu.removeMenuItem("Edit", "To front");
                menu.removeMenuItem("Edit", "To back");
                menu.removeMenuItem("Edit", "Copy");
                menu.removeMenuItem("Edit", "Remove");

                break;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::leftMousePressed(int posX, int posY)
{
    float x = static_cast<float>(posX);
    float y = static_cast<float>(posY);

    y -= panel->getPosition().y;

    bool widgetFound = false;
    tgui::ChildWindow::Ptr childWindow = nullptr;

    // Loop through all widgets
    for (auto it = panel->getWidgets().crbegin(); it != panel->getWidgets().crend(); ++it)
    {
        // Check if the widget is visible and enabled
        if (((*it)->isVisible()) && ((*it)->isEnabled()))
        {
            if (widgetFound == false)
            {
                // Remember the widget if the mouse is on top of it
                if ((*it)->mouseOnWidget(x, y))
                {
                    childWindow = *it;
                    widgetFound = true;
                }
            }
            else // The widget was already found, so tell the other widgets that the mouse can't be on them
                (*it)->mouseNotOnWidget();
        }
    }

    // Check if one of the forms was clicked
    if ((widgetFound) && (childWindow->getTitle() != "Properties") && (childWindow->getTitle() != "Widgets"))
    {
        // Check if this was the active form
        if (activeForm->window == childWindow)
        {
            x -= childWindow->getPosition().x + childWindow->getBorders().left;
            y -= childWindow->getPosition().y + childWindow->getBorders().top + childWindow->getTitleBarHeight();

            // Don't let the widgets react when you are clicking on the title bar
            if (y < 0)
                return;

            // Only continue when the mouse is not on one of the selection squares
            scalingWidget = activeForm->mouseOnSelectionSquares(x, y);
            if (scalingWidget == NoSelectionSquare)
            {
                widgetFound = false;
                tgui::Widget::Ptr widget = nullptr;

                tgui::Panel::Ptr formPanel = childWindow->get("Panel");

                // Loop through all widgets
                for (auto it = formPanel->getWidgets().crbegin(); it != formPanel->getWidgets().crend(); ++it)
                {
                    // Check if the widget is visible and enabled
                    if (((*it)->isVisible()) && ((*it)->isEnabled()))
                    {
                        if (widgetFound == false)
                        {
                            // Remember the widget if the mouse is on top of it
                            if ((*it)->mouseOnWidget(x, y))
                            {
                                widget = *it;
                                widgetFound = true;
                            }
                        }
                        else // The widget was already found, so tell the other widgets that the mouse can't be on them
                            (*it)->mouseNotOnWidget();
                    }
                }

                // Check if you clicked on a widget
                if (widgetFound)
                {
                    // Find the widget data
                    for (auto data = activeForm->widgets.begin(); data != activeForm->widgets.end(); ++data)
                    {
                        if (data->widget == widget)
                        {
                            // You are now dragging the widget
                            draggingWidget = true;
                            oldMousePosition = sf::Vector2f(sf::Vector2i(posX, posY));

                            activateWidget(*data);
                            break;
                        }
                    }
                }
                else // The form was clicked, next to all widgets
                {
                    activeForm->activeWidget = &widgetsData[""];
                    activeForm->repositionSelectionSquares();
                    recreateProperties();

                    // Remove the edit menu
                    menu.removeMenuItem("Edit", "To front");
                    menu.removeMenuItem("Edit", "To back");
                    menu.removeMenuItem("Edit", "Copy");
                    menu.removeMenuItem("Edit", "Remove");
                }
            }
            else // A selection square was clicked, remember the position
                oldMousePosition = sf::Vector2f(sf::Vector2i(posX, posY));
        }
        else // Another form should be activated
        {
            /// TODO: Allow multiple forms
            assert(false && "You can't change active form when there is only one form!");
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::moveWidget(int x, int y)
{
    tgui::Grid::Ptr grid = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("Grid");
    tgui::EditBox::Ptr valueLeft = grid->get("ValueLeft");
    tgui::EditBox::Ptr valueTop = grid->get("ValueTop");

    // Only continue when the widget can be moved
    if ((valueLeft == nullptr) || (valueTop == nullptr))
        return;

    while (x >= oldMousePosition.x + 10)
    {
        activeForm->moveWidget(10, 0);
        oldMousePosition.x += 10;
    }

    while (x <= oldMousePosition.x - 10)
    {
        activeForm->moveWidget(-10, 0);
        oldMousePosition.x -= 10;
    }

    while (y >= oldMousePosition.y + 10)
    {
        activeForm->moveWidget(0, 10);
        oldMousePosition.y += 10;
    }

    while (y <= oldMousePosition.y - 10)
    {
        activeForm->moveWidget(0, -10);
        oldMousePosition.y -= 10;
    }

    updateProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::scaleWidget(int x, int y)
{
    tgui::Grid::Ptr grid = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("Grid");
    tgui::EditBox::Ptr valueLeft = grid->get("ValueLeft");
    tgui::EditBox::Ptr valueTop = grid->get("ValueTop");
    tgui::EditBox::Ptr valueWidth = grid->get("ValueWidth");
    tgui::EditBox::Ptr valueHeight = grid->get("ValueHeight");

    // Only continue when the widget can be scaled
    if ((valueWidth == nullptr) || (valueHeight == nullptr))
        return;

    // Check which square you are dragging
    if (scalingWidget == LeftSelectionSquare)
    {
        // Check if you are pulling the left square to the right
        while (x > oldMousePosition.x + 10)
        {
            activeForm->resizeWidget(-10, 0);
            oldMousePosition.x += 10;

            if ((valueLeft != nullptr) && (valueTop != nullptr))
                activeForm->moveWidget(10, 0);
        }

        // Check if you are pulling the left square to the left
        while (x < oldMousePosition.x - 10)
        {
            activeForm->resizeWidget(10, 0);
            oldMousePosition.x -= 10;

            if ((valueLeft != nullptr) && (valueTop != nullptr))
                activeForm->moveWidget(-10, 0);
        }
    }
    else if (scalingWidget == TopSelectionSquare)
    {
        // Check if you are pulling the top square down
        while (y > oldMousePosition.y + 10)
        {
            activeForm->resizeWidget(0, -10);
            oldMousePosition.y += 10;

            if ((valueLeft != nullptr) && (valueTop != nullptr))
                activeForm->moveWidget(0, 10);
        }

        // Check if you are pulling the top square up
        while (y < oldMousePosition.y - 10)
        {
            activeForm->resizeWidget(0, 10);
            oldMousePosition.y -= 10;

            if ((valueLeft != nullptr) && (valueTop != nullptr))
                activeForm->moveWidget(0, -10);
        }
    }
    else if (scalingWidget == RightSelectionSquare)
    {
        // Check if you are pulling the right square to the right
        while (x > oldMousePosition.x + 10)
        {
            activeForm->resizeWidget(10, 0);
            oldMousePosition.x += 10;
        }

        // Check if you are pulling the right square to the left
        while (x < oldMousePosition.x - 10)
        {
            activeForm->resizeWidget(-10, 0);
            oldMousePosition.x -= 10;
        }
    }
    else if (scalingWidget == BottomSelectionSquare)
    {
        // Check if you are pulling the bottom square down
        while (y > oldMousePosition.y + 10)
        {
            activeForm->resizeWidget(0, 10);
            oldMousePosition.y += 10;
        }

        // Check if you are pulling the bottom square up
        while (y < oldMousePosition.y - 10)
        {
            activeForm->resizeWidget(0, -10);
            oldMousePosition.y -= 10;
        }
    }
    else // One of the corners was selected
    {
        float ratio = activeForm->activeWidget->widget->getSize().y / activeForm->activeWidget->widget->getSize().x;

        sf::Vector2f change;
        if (ratio <= 1)
            change = sf::Vector2f(10.0f, 10.0f * ratio);
        else
            change = sf::Vector2f(10.0f / ratio, 10.0f);

        // Check which corner is being dragged
        if (scalingWidget == TopLeftSelectionSquare)
        {
            // Check if you are pulling the top left square to the upper left side
            while ((x < oldMousePosition.x - change.x) && (y < oldMousePosition.y - change.y))
            {
                activeForm->resizeWidget(change.x, change.y);
                oldMousePosition.x -= change.x;
                oldMousePosition.y -= change.y;

                if ((valueLeft != nullptr) && (valueTop != nullptr))
                    activeForm->moveWidget(-change.x, -change.y);
            }

            // Check if you are pulling the top left square to the bottom right side
            while ((x > oldMousePosition.x + change.x) && (y > oldMousePosition.y + change.y))
            {
                activeForm->resizeWidget(-change.x, -change.y);
                oldMousePosition.x += change.x;
                oldMousePosition.y += change.y;

                if ((valueLeft != nullptr) && (valueTop != nullptr))
                    activeForm->moveWidget(change.x, change.y);
            }
        }
        else if (scalingWidget == TopRightSelectionSquare)
        {
            // Check if you are pulling the top right square to the upper right side
            while ((x > oldMousePosition.x + change.x) && (y < oldMousePosition.y - change.y))
            {
                activeForm->resizeWidget(change.x, change.y);
                oldMousePosition.x += change.x;
                oldMousePosition.y -= change.y;

                if ((valueLeft != nullptr) && (valueTop != nullptr))
                    activeForm->moveWidget(0, -change.y);
            }

            // Check if you are pulling the top right square to the bottom left side
            while ((x < oldMousePosition.x - change.x) && (y > oldMousePosition.y + change.y))
            {
                activeForm->resizeWidget(-change.x, -change.y);
                oldMousePosition.x -= change.x;
                oldMousePosition.y += change.y;

                if ((valueLeft != nullptr) && (valueTop != nullptr))
                    activeForm->moveWidget(0, change.y);
            }
        }
        else if (scalingWidget == BottomRightSelectionSquare)
        {
            // Check if you are pulling the bottom right square to the bottom right side
            while ((x > oldMousePosition.x + change.x) && (y > oldMousePosition.y + change.y))
            {
                activeForm->resizeWidget(change.x, change.y);
                oldMousePosition.x += change.x;
                oldMousePosition.y += change.y;
            }

            // Check if you are pulling the bottom right square to the upper left side
            while ((x < oldMousePosition.x - change.x) && (y < oldMousePosition.y - change.y))
            {
                activeForm->resizeWidget(-change.x, -change.y);
                oldMousePosition.x -= change.x;
                oldMousePosition.y -= change.y;
            }
        }
        else if (scalingWidget == BottomLeftSelectionSquare)
        {
            // Check if you are pulling the bottom left square to the bottom left side
            while ((x < oldMousePosition.x - change.x) && (y > oldMousePosition.y + change.y))
            {
                activeForm->resizeWidget(change.x, change.y);
                oldMousePosition.x -= change.x;
                oldMousePosition.y += change.y;

                if ((valueLeft != nullptr) && (valueTop != nullptr))
                    activeForm->moveWidget(-change.x, 0);
            }

            // Check if you are pulling the top left square to the upper right side
            while ((x > oldMousePosition.x + change.x) && (y < oldMousePosition.y - change.y))
            {
                activeForm->resizeWidget(-change.x, -change.y);
                oldMousePosition.x += change.x;
                oldMousePosition.y -= change.y;

                if ((valueLeft != nullptr) && (valueTop != nullptr))
                    activeForm->moveWidget(change.x, 0);
            }
        }
    }

    updateProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::recreateProperties()
{
    tgui::ChildWindow::Ptr propertyWindow = panel->get("PropertiesWindow");
    tgui::Grid::Ptr grid = propertyWindow->get("Grid");
    grid->removeAllWidgets();

    unsigned int row = 1;
    for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it, ++row)
    {
        if (activeForm->activeWidget->widget != nullptr)
            activeForm->activeWidget->widget->getProperty(it->first, it->second.value);

        tgui::EditBox::Ptr value(*grid, "Value" + it->first);
        value->load(WIDGETS_FOLDER "/White.conf");
        value->setSize(140, 20);
        value->setText(it->second.value);

        tgui::EditBox::Ptr property = grid->copy(value, "Property" + it->first);
        property->disable();
        property->setText(it->first);

        // Set a callback when changing the value
        value->bindCallback(std::bind(&FormBuilder::changeProperty, this, value, property), tgui::EditBox::ReturnKeyPressed | tgui::EditBox::Unfocused);
        value->bindCallback([=](){ propertyChanged = true; }, tgui::EditBox::TextChanged);

        // Don't allow text input for numbers
        if ((it->second.type == "float") || (it->second.type == "int") || (it->second.type == "uint") || (it->second.type == "byte"))
            value->setNumbersOnly(true);

        grid->addWidget(property, row, 0);
        grid->addWidget(value, row, 1);
    }

    tgui::ComboBox::Ptr widgetSelector = propertyWindow->get("WidgetSelector");
    widgetSelector->moveToFront();
    widgetSelector->setSize(grid->getSize().x, widgetSelector->getSize().y);

    auto widgetIt = activeForm->widgets.cbegin();
    for (unsigned int i = 0; i < activeForm->widgets.size(); ++i, ++widgetIt)
    {
        if (&*widgetIt == activeForm->activeWidget)
        {
            widgetSelector->setSelectedItem(i+1);
            return;
        }
    }
    widgetSelector->setSelectedItem(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::requestNewForm()
{
    // Only one form is currently allowed
    if (activeForm != nullptr)
    {
        errorMessage("Only one form is currently allowed.");
    }
    else // Create the new form
    {
        createLayer();

        tgui::ChildWindow::Ptr requestNewFormWindow(*tgui::Panel::Ptr(gui.get("LayerPanel")));
        requestNewFormWindow->load(WIDGETS_FOLDER "/White.conf");
        requestNewFormWindow->setTitleBarHeight(20);
        requestNewFormWindow->setSize(400, 210);
        requestNewFormWindow->setPosition((sf::Vector2f(window.getSize()) - requestNewFormWindow->getSize()) / 2.0f);
        requestNewFormWindow->bindCallback(&FormBuilder::removeLayer, this, tgui::ChildWindow::Closed);

        // Add the first edit box to it
        tgui::EditBox::Ptr width(*requestNewFormWindow, "width");
        width->load(WIDGETS_FOLDER "/White.conf");
        width->setPosition(70, 40);
        width->setSize(100, 24);
        width->setTextSize(16);
        width->setMaximumCharacters(4);
        width->setText("800");
        width->setNumbersOnly(true);

        // Make a copy of the edit box
        tgui::EditBox::Ptr height = requestNewFormWindow->copy(width, "height");
        height->setPosition(230, 40);
        height->setText("600");

        // Create another edit box, for the filename
        tgui::EditBox::Ptr filename(*requestNewFormWindow, "filename");
        filename->load(WIDGETS_FOLDER "/White.conf");
        filename->setPosition(70, 110);
        filename->setSize(260, 24);
        filename->setText("form.txt");
        filename->setTextSize(16);

        // Create the labels
        tgui::Label::Ptr labelWH(*requestNewFormWindow);
        tgui::Label::Ptr labelFilename(*requestNewFormWindow);
        tgui::Label::Ptr labelX(*requestNewFormWindow);

        // Setup the labels
        labelWH->setText("Width x Height");
        labelWH->setPosition(30, 10);
        labelWH->setTextColor(sf::Color::Black);
        labelWH->setTextSize(17);

        labelFilename->setText("Filename");
        labelFilename->setPosition(30, 80);
        labelFilename->setTextColor(sf::Color::Black);
        labelFilename->setTextSize(17);

        labelX->setText("x");
        labelX->setPosition(195, 40);
        labelX->setTextColor(sf::Color::Black);
        labelX->setTextSize(17);

        // Create the button
        tgui::Button::Ptr buttonCreate(*requestNewFormWindow);
        buttonCreate->load(WIDGETS_FOLDER "/White.conf");
        buttonCreate->setPosition(120, 160);
        buttonCreate->setSize(160, 30);
        buttonCreate->setTextSize(17);
        buttonCreate->setText("Create form");

        auto createForm = [this, width, height, filename]()
                          {
                              createNewForm(filename->getText(),
                                            static_cast<float>(std::atof(width->getText().toAnsiString().c_str())),
                                            static_cast<float>(std::atof(height->getText().toAnsiString().c_str())));
                              removeLayer();
                          };
        buttonCreate->bindCallback(createForm, tgui::Button::LeftMouseClicked);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::createNewForm(const std::string& filename, float width, float height)
{
    forms.emplace(forms.end(), Form(*panel, "Form1", width, height, filename, &widgetsData[""]));
    activeForm = &forms.back();
    activeForm->window->bindCallback(tgui::ChildWindow::Closed);
    activeForm->window->bindCallback([this](){ gui.get("MenuBar")->moveToFront(); }, tgui::ChildWindow::LeftMousePressed);

    initPropertiesWindow();
    initWidgetsWindow();
    resize(static_cast<unsigned int>(gui.getSize().x), static_cast<unsigned int>(gui.getSize().y));

    activeForm->activeWidget = &widgetsData[""];

    menu.addMenuItem("File", "Save");

    // Update the contents of the properties panel
    recreateProperties();

    menu.addMenuItem("File", "Load");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::addWidget(const std::string& widgetName)
{
    activeForm->widgets.push_back(widgetsData[widgetName]);
    activeForm->activeWidget = &activeForm->widgets.back();

    activeForm->activeWidget->widget = widgetsData[widgetName].widget.clone();
    activeForm->activeWidget->widget->show();

    tgui::Panel::Ptr(activeForm->window->get("Panel"))->add(activeForm->activeWidget->widget);

    std::string value;
    if (activeForm->activeWidget->widget->getProperty("ConfigFile", value))
    {
        value = WIDGETS_FOLDER "/White.conf"; /// TODO: Open window to input ConfigFile
        activeForm->activeWidget->widget->setProperty("ConfigFile", value);
    }
    else if (activeForm->activeWidget->widget->getProperty("Filename", value))
    {
        value = IMAGES_FOLDER "/EmptyPicture.png"; /// TODO: Open window to input Filename
        activeForm->activeWidget->widget->setProperty("Filename", value);
    }

    // Give the widget a name and add it to the list
    {
        unsigned int number = 1;
        bool found = true;
        while (found)
        {
            found = false;

            for (auto dataIt = activeForm->widgets.cbegin(); dataIt != activeForm->widgets.cend(); ++dataIt)
            {
                for (auto propertyIt = dataIt->properties.cbegin(); propertyIt != dataIt->properties.cend(); ++propertyIt)
                {
                    if (propertyIt->first == "Name")
                    {
                        if (propertyIt->second.value == widgetName + tgui::to_string(number))
                        {
                            number++;
                            found = true;
                            break;
                        }
                    }
                }

                if (found)
                    break;
            }
        }

        for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
        {
            if (it->first == "Name")
            {
                it->second.value = widgetName + tgui::to_string(number);

                tgui::Panel::Ptr(activeForm->window->get("Panel"))->setWidgetName(activeForm->activeWidget->widget, it->second.value);

                tgui::ChildWindow::Ptr propertyWindow = panel->get("PropertiesWindow");
                tgui::ComboBox::Ptr widgetSelector = propertyWindow->get("WidgetSelector");
                widgetSelector->addItem(it->second.value);
                break;
            }
        }
    }

    activeForm->repositionSelectionSquares();

    // Initialize the value for all properties
    for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
        activeForm->activeWidget->widget->getProperty(it->first, it->second.value);

    // The properties of this new widget should be displayed
    recreateProperties();

    // Add edit option to the menu bar
    menu.addMenuItem("Edit", "To front");
    menu.addMenuItem("Edit", "To back");
    menu.addMenuItem("Edit", "Copy");
    menu.addMenuItem("Edit", "Remove");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::activateWidget(WidgetData& data)
{
    // If the widget wasn't active already then activate it now
    if (activeForm->activeWidget->widget != data.widget)
    {
        activeForm->activeWidget = &data;
        activeForm->repositionSelectionSquares();
        recreateProperties();

        // Add edit option to the menu bar
        menu.addMenuItem("Edit", "To front");
        menu.addMenuItem("Edit", "To back");
        menu.addMenuItem("Edit", "Copy");
        menu.addMenuItem("Edit", "Remove");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::selectDifferentWidget()
{
    tgui::ComboBox::Ptr widgetSelector = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("WidgetSelector");

    if (widgetSelector->getSelectedItemIndex() < 1)
        activateWidget(widgetsData[""]);
    else
    {
        auto it = activeForm->widgets.begin();
        std::advance(it, widgetSelector->getSelectedItemIndex()-1);
        activateWidget(*it);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::changeProperty(tgui::EditBox::Ptr value, tgui::EditBox::Ptr property)
{
    // Ignore the callback when the contents of the edit box hasn't changed
    if (!propertyChanged)
        return;

    propertyChanged = false;

    if (activeForm->activeWidget->widget != nullptr)
    {
        if (property->getText() == "Name")
        {
            for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
            {
                if (it->first == "Name")
                {
                    it->second.value = value->getText();
                    activeForm->window->get<tgui::Panel>("Panel")->setWidgetName(activeForm->activeWidget->widget, value->getText());

                    tgui::ComboBox::Ptr widgetSelector = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("WidgetSelector");

                    auto& items = widgetSelector->getItems();
                    auto widgetIt = activeForm->widgets.cbegin();
                    for (unsigned int i = 0; i < activeForm->widgets.size(); ++i, ++widgetIt)
                    {
                        if (&*widgetIt == activeForm->activeWidget)
                        {
                            items[i+1] = it->second.value;
                            break;
                        }
                    }

                    break;
                }
            }
        }
        else
        {
            activeForm->activeWidget->widget->setProperty(property->getText(), value->getText());
            activeForm->repositionSelectionSquares();
        }
    }
    else // The properties of the form were changed
    {
        if (property->getText() == "Name")
        {
            for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
            {
                if (it->first == "Name")
                {
                    it->second.value = value->getText();

                    tgui::ComboBox::Ptr widgetSelector = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("WidgetSelector");
                    widgetSelector->getItems()[0] = it->second.value;
                    break;
                }
            }
        }
        else
        {
            for (auto it = activeForm->activeWidget->properties.begin(); it != activeForm->activeWidget->properties.end(); ++it)
            {
                if (it->first == property->getText())
                {
                    it->second.value = value->getText();

                    if (it->first == "Filename")
                    {
                        activeForm->window->setTitle(it->second.value);
                    }
                    else if (it->first == "Width")
                    {
                        activeForm->window->setSize(static_cast<float>(std::atof(it->second.value.c_str())), activeForm->window->getSize().y);
                    }
                    else if (it->first == "Height")
                    {
                        activeForm->window->setSize(activeForm->window->getSize().x, static_cast<float>(std::atof(it->second.value.c_str())));
                    }

                    break;
                }
            }
        }
    }

    updateProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FormBuilder::updateProperties()
{
    // Update the contents of the edit boxes of the property window
    tgui::Grid::Ptr grid = tgui::ChildWindow::Ptr(panel->get("PropertiesWindow"))->get("Grid");
    auto it = activeForm->activeWidget->properties.begin();
    for (unsigned int i = 0; i < grid->getWidgets().size(); i += 2, ++it)
    {
        if (activeForm->activeWidget->widget != nullptr)
            activeForm->activeWidget->widget->getProperty(it->first, it->second.value);

        tgui::EditBox::Ptr(grid->getWidgets()[i])->setText(it->second.value);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
