/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include "ButtonProperties.hpp"
#include "ComboBoxProperties.hpp"
#include "EditBoxProperties.hpp"
#include "LabelProperties.hpp"
#include "ListBoxProperties.hpp"
#include "PictureProperties.hpp"
#include "ProgressBarProperties.hpp"
#include "RadioButtonProperties.hpp"
#include "ScrollbarProperties.hpp"
#include "SliderProperties.hpp"
#include "SpinButtonProperties.hpp"
#include "TextBoxProperties.hpp"
#include "GuiBuilder.hpp"

#include <memory>
#include <string>
#include <cctype> // isdigit
#include <cmath> // max
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const float EDIT_BOX_HEIGHT = 24;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiBuilder::GuiBuilder() :
    m_window{{1280, 680}, "TGUI - GUI Builder"},
    m_gui{m_window}
{
    m_window.setFramerateLimit(60);

    m_widgetProperties["Button"] = std::make_unique<ButtonProperties>();
    m_widgetProperties["CheckBox"] = std::make_unique<RadioButtonProperties>();
    m_widgetProperties["ComboBox"] = std::make_unique<ComboBoxProperties>();
    m_widgetProperties["EditBox"] = std::make_unique<EditBoxProperties>();
    m_widgetProperties["Label"] = std::make_unique<LabelProperties>();
    m_widgetProperties["ListBox"] = std::make_unique<ListBoxProperties>();
    m_widgetProperties["Picture"] = std::make_unique<PictureProperties>();
    m_widgetProperties["ProgressBar"] = std::make_unique<ProgressBarProperties>();
    m_widgetProperties["RadioButton"] = std::make_unique<RadioButtonProperties>();
    m_widgetProperties["Scrollbar"] = std::make_unique<ScrollbarProperties>();
    m_widgetProperties["Slider"] = std::make_unique<SliderProperties>();
    m_widgetProperties["SpinButton"] = std::make_unique<SpinButtonProperties>();
    m_widgetProperties["TextBox"] = std::make_unique<TextBoxProperties>();

    loadStartScreen();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::mainLoop()
{
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                while (!m_forms.empty())
                    closeForm(m_forms[0].get());

                m_window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                m_window.setView(sf::View({0, 0, (float)event.size.width, (float)event.size.height}));
                m_gui.setView(m_window.getView());
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (m_selectedForm)
                    m_selectedForm->mouseReleased();
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                if (m_selectedForm)
                    m_selectedForm->mouseMoved({event.mouseMove.x, event.mouseMove.y});
            }

            m_gui.handleEvent(event);
        }

        m_window.clear({200, 200, 200});
        m_gui.draw();
        m_window.display();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::reloadProperties()
{
    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    m_propertyValuePairs = m_widgetProperties.at(selectedWidget->getWidgetType())->initProperties(selectedWidget);
    for (const auto& property : m_propertyValuePairs.first)
    {
        auto valueEditBox = m_propertiesContainer->get<tgui::EditBox>("Value" + property.first);
        valueEditBox->setText(property.second.second);
    }
    for (const auto& property : m_propertyValuePairs.second)
    {
        auto valueEditBox = m_propertiesContainer->get<tgui::EditBox>("Value" + property.first);
        valueEditBox->setText(property.second.second);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::widgetSelected(tgui::Widget::Ptr widget)
{
    initProperties();

    if (widget)
        m_selectedWidgetComboBox->setSelectedItemById(tgui::to_string(widget.get()));
    else
        m_selectedWidgetComboBox->setSelectedItemById("form");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::closeForm(Form* form)
{
    if (!m_selectedForm->isChanged())
    {
        if (m_selectedForm == form)
            m_selectedForm = nullptr;

        m_forms.erase(std::find_if(m_forms.begin(), m_forms.end(), [form](const auto& f){ return f.get() == form; }));
        if (m_forms.empty())
            loadStartScreen();

        return;
    }

    auto panel = tgui::Panel::create({"100%", "100%"});
    panel->getRenderer()->setBackgroundColor({0, 0, 0, 150});
    m_gui.add(panel);

    auto messageBox = tgui::MessageBox::create("Saving form", "The form was changed, do you want to save the changes?", {"Yes", "No"});
    messageBox->setPosition("(&.size - size) / 2");
    m_gui.add(messageBox);

    bool haltProgram = true;
    messageBox->connect("ButtonPressed", [=,&haltProgram](const sf::String& button){
        if (button == "Yes")
            m_selectedForm->save();

        m_gui.remove(panel);
        m_gui.remove(messageBox);

        if (m_selectedForm == form)
            m_selectedForm = nullptr;

        m_forms.erase(std::find_if(m_forms.begin(), m_forms.end(), [form](const auto& f){ return f.get() == form; }));
        if (m_forms.empty())
            loadStartScreen();

        haltProgram = false;
    });

    // The closeForm function has to halt the execution of the normal main loop (to be able to prevent closing the window)
    while (haltProgram && m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // Attempting to close the window, while already having asked whether the form should be saved, will result in the close without saving
                m_window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                m_window.setView(sf::View({0, 0, (float)event.size.width, (float)event.size.height}));
                m_gui.setView(m_window.getView());
            }

            m_gui.handleEvent(event);
        }

        m_window.clear({200, 200, 200});
        m_gui.draw();
        m_window.display();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadStartScreen()
{
    while (!m_forms.empty())
        closeForm(m_forms[0].get());

    m_propertiesWindow = nullptr;
    m_propertiesContainer = nullptr;
    m_selectedWidgetComboBox = nullptr;

    m_gui.removeAllWidgets();
    m_gui.loadWidgetsFromFile("start_screen.txt");

    auto filenameEditBox = m_gui.get<tgui::Panel>("MainPanel")->get<tgui::EditBox>("FilenameEditBox");
    m_gui.get<tgui::Panel>("MainPanel")->get("NewButton")->connect("pressed", [=]{ loadEditingScreen(filenameEditBox->getText()); });

    m_gui.get<tgui::Panel>("MainPanel")->get("LoadButton")->connect("pressed", [=]{
        loadEditingScreen(filenameEditBox->getText());
        if (m_selectedForm->load())
            initSelectedWidgetComboBoxAfterLoad();
        else
            loadStartScreen();
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadEditingScreen(const std::string& filename)
{
    m_gui.removeAllWidgets();
    m_gui.loadWidgetsFromFile("editing_screen.txt");

    m_forms.push_back(std::make_unique<Form>(this, filename, m_gui.get<tgui::ChildWindow>("Form")));
    m_selectedForm = m_forms[0].get();

    m_propertiesWindow = m_gui.get<tgui::ChildWindow>("PropertiesWindow");
    m_propertiesContainer = m_propertiesWindow->get<tgui::Group>("Properties");
    m_selectedWidgetComboBox = m_propertiesWindow->get<tgui::ComboBox>("SelectedWidgetComboBox");

    m_selectedWidgetComboBox->addItem(filename, "form");
    m_selectedWidgetComboBox->setSelectedItemById("form");
    m_selectedWidgetComboBox->connect("ItemSelected", [this](std::string, std::string id){ m_selectedForm->selectWidgetById(id); });

    loadToolbox();
    initProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadToolbox()
{
    auto toolboxWindow = m_gui.get<tgui::ChildWindow>("ToolboxWindow");
    auto toolbox = toolboxWindow->get<tgui::ScrollablePanel>("Widgets");

    const auto widgets = std::vector<std::pair<std::string, std::function<tgui::Widget::Ptr()>>>{
        {"Button", []{ return tgui::Button::create("Button"); }},
        {"CheckBox", []{ return tgui::CheckBox::create(); }},
        {"ComboBox", []{ return tgui::ComboBox::create(); }},
        {"EditBox", []{ return tgui::EditBox::create(); }},
        {"Label", []{ return tgui::Label::create("Label"); }},
        {"ListBox", []{ return tgui::ListBox::create(); }},
        {"Picture", []{ return tgui::Picture::create("resources/DefaultPicture.png"); }},
        {"ProgressBar", []{ return tgui::ProgressBar::create(); }},
        {"RadioButton", []{ return tgui::RadioButton::create(); }},
        {"Scrollbar", []{ return tgui::Scrollbar::create(); }},
        {"Slider", []{ return tgui::Slider::create(); }},
        {"SpinButton", []{ return tgui::SpinButton::create(); }},
        {"TextBox", []{ return tgui::TextBox::create(); }}
    };

    float topPosition = 0;
    for (auto& widget : widgets)
    {
        auto icon = tgui::Picture::create("resources/widget-icons/" + widget.first + ".png");
        auto name = tgui::Label::create(widget.first);
        name->setPosition({icon->getSize().x * 1.1f, "50% - 10"});
        name->setTextSize(14);

        auto verticalLayout = tgui::VerticalLayout::create();
        verticalLayout->setPosition(0, topPosition);
        verticalLayout->setSize({"&.w - 16", icon->getSize().y + 4});
        verticalLayout->getRenderer()->setPadding({2});

        auto panel = tgui::Panel::create();
        panel->getRenderer()->setBackgroundColor(sf::Color::Transparent);
        panel->add(icon);
        panel->add(name);
        verticalLayout->add(panel);
        toolbox->add(verticalLayout);

        panel->connect("Clicked", [=]{ createNewWidget(widget.second()); });

        topPosition += verticalLayout->getSize().y;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::createNewWidget(tgui::Widget::Ptr widget)
{
    const std::string id = tgui::to_string(widget.get());
    const std::string name = m_selectedForm->addWidget(widget);
    m_selectedWidgetComboBox->addItem(name, id);
    m_selectedWidgetComboBox->setSelectedItemById(id);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::updateWidgetProperty(const std::string& property, const std::string& value)
{
    const auto selectedWidget = m_selectedForm->getSelectedWidget();

    try {
        m_widgetProperties.at(selectedWidget->getWidgetType())->updateProperty(selectedWidget, property, value);
    }
    catch (const tgui::Exception& e) {
        std::cout << "Exception caught when setting property: " << e.what() << std::endl;
        m_widgetProperties.at(selectedWidget->getWidgetType())->updateProperty(selectedWidget, property, m_previousValue);
    }

    reloadProperties(); // reload all properties in case something else changed
    m_selectedForm->updateSelectionSquarePositions(); // update the position of the selection squares in case the position or size of the widget changed
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::initProperties()
{
    m_propertiesContainer->removeAllWidgets();

    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    float topPosition = 0;

    if (selectedWidget)
    {
        auto buttonCopy = tgui::Button::create("Copy");
        buttonCopy->setSize({((bindWidth(m_propertiesContainer) - 16.f) / 4.f) - 5, 25});
        buttonCopy->setPosition({0, topPosition});
        buttonCopy->connect("Pressed", [this]{ createNewWidget(m_selectedForm->getSelectedWidget()->clone()); });
        m_propertiesContainer->add(buttonCopy);

        auto buttonRemove = tgui::Button::create("Remove");
        buttonRemove->setSize({((bindWidth(m_propertiesContainer) - 16.f) / 4.f) - 5, 25});
        buttonRemove->setPosition({((bindWidth(m_propertiesContainer) - 20 - 16.f) / 4.f) + (20.f * 1.f / 3.f), topPosition});
        buttonRemove->connect("Pressed", [this]{ removeSelectedWidget(); });
        m_propertiesContainer->add(buttonRemove);

        auto buttonToFront = tgui::Button::create("ToFront");
        buttonToFront->setSize({((bindWidth(m_propertiesContainer) - 16.f) / 4.f) - 4, 25});
        buttonToFront->setPosition({2.f * ((bindWidth(m_propertiesContainer) - 20 - 16.f) / 4.f) + (20.f * 2.f / 3.f), topPosition});
        buttonToFront->connect("Pressed", [this]{ m_selectedForm->getSelectedWidget()->moveToFront(); m_selectedForm->setChanged(true); });
        m_propertiesContainer->add(buttonToFront);

        auto buttonToBack = tgui::Button::create("ToBack");
        buttonToBack->setSize({((bindWidth(m_propertiesContainer) - 16.f) / 4.f) - 5, 25});
        buttonToBack->setPosition({3.f * ((bindWidth(m_propertiesContainer) - 20 - 16.f) / 4.f) + (20.f * 3.f / 3.f), topPosition});
        buttonToBack->connect("Pressed", [this]{ m_selectedForm->getSelectedWidget()->moveToBack(); m_selectedForm->setChanged(true); });
        m_propertiesContainer->add(buttonToBack);

        const auto smallestTextsize = std::min({buttonCopy->getTextSize(), buttonRemove->getTextSize(), buttonToFront->getTextSize(), buttonToBack->getTextSize()});
        buttonCopy->setTextSize(smallestTextsize);
        buttonRemove->setTextSize(smallestTextsize);
        buttonToFront->setTextSize(smallestTextsize);
        buttonToBack->setTextSize(smallestTextsize);

        topPosition += 35;

        auto valueEditBox = addPropertyValueEditBoxes(topPosition, {"Name", {"String", m_selectedForm->getSelectedWidgetName()}});
        valueEditBox->connect({"ReturnKeyPressed", "unfocused"}, [=]{
            if (m_previousValue != valueEditBox->getText())
            {
                m_selectedForm->setChanged(true);
                changeWidgetName(valueEditBox->getText());
                m_previousValue = valueEditBox->getText();
            }
        });

        topPosition += 10;
        m_propertyValuePairs = m_widgetProperties.at(selectedWidget->getWidgetType())->initProperties(selectedWidget);
        for (const auto& property : m_propertyValuePairs.first)
        {
            valueEditBox = addPropertyValueEditBoxes(topPosition, property);
            valueEditBox->connect({"ReturnKeyPressed", "unfocused"}, [=]{
                if (m_previousValue != valueEditBox->getText())
                {
                    m_selectedForm->setChanged(true);
                    updateWidgetProperty(property.first, valueEditBox->getText());
                    m_previousValue = valueEditBox->getText();
                }
            });
        }

        topPosition += 10;
        for (const auto& property : m_propertyValuePairs.second)
        {
            valueEditBox = addPropertyValueEditBoxes(topPosition, property);
            valueEditBox->connect({"ReturnKeyPressed", "unfocused"}, [=]{
                if (m_previousValue != valueEditBox->getText())
                {
                    m_selectedForm->setChanged(true);
                    updateWidgetProperty(property.first, valueEditBox->getText());
                    m_previousValue = valueEditBox->getText();
                }
            });
        }
    }
    else // The form itself was selected
    {
        auto valueEditBox = addPropertyValueEditBoxes(topPosition, {"Filename", {"String", m_selectedForm->getFilename()}});
        valueEditBox->connect({"ReturnKeyPressed", "unfocused"}, [=]{
            if (m_previousValue != valueEditBox->getText())
            {
                m_selectedForm->setChanged(true);
                m_selectedForm->setFilename(valueEditBox->getText());
                m_selectedWidgetComboBox->changeItemById("form", valueEditBox->getText());
                m_previousValue = valueEditBox->getText();
            }
        });

        valueEditBox = addPropertyValueEditBoxes(topPosition, {"Width", {"Float", tgui::to_string(m_selectedForm->getSize().x)}});
        valueEditBox->setInputValidator(tgui::EditBox::Validator::UInt);
        valueEditBox->connect({"ReturnKeyPressed", "unfocused"}, [=]{
            if (m_previousValue != valueEditBox->getText())
            {
                // Form is not marked as changed since the width is not saved
                m_selectedForm->setSize({tgui::stoi(valueEditBox->getText()), m_selectedForm->getSize().y});
                m_previousValue = valueEditBox->getText();
            }
        });

        valueEditBox = addPropertyValueEditBoxes(topPosition, {"Height", {"Float", tgui::to_string(m_selectedForm->getSize().y)}});
        valueEditBox->setInputValidator(tgui::EditBox::Validator::UInt);
        valueEditBox->connect({"ReturnKeyPressed", "unfocused"}, [=]{
            if (m_previousValue != valueEditBox->getText())
            {
                // Form is not marked as changed since the height is not saved
                m_selectedForm->setSize({m_selectedForm->getSize().x, tgui::stoi(valueEditBox->getText())});
                m_previousValue = valueEditBox->getText();
            }
        });
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::EditBox::Ptr GuiBuilder::addPropertyValueEditBoxes(float& topPosition, const std::pair<std::string, std::pair<std::string, std::string>>& propertyValuePair)
{
    const auto& property = propertyValuePair.first;
    //const auto& type = propertyValuePair.second.first;
    const auto& value = propertyValuePair.second.second;

    auto propertyEditBox = tgui::EditBox::create();
    propertyEditBox->setPosition({0, topPosition});
    propertyEditBox->setSize({((bindWidth(m_propertiesContainer) - 16.f) / 2.f) + propertyEditBox->getRenderer()->getBorders().getRight(), EDIT_BOX_HEIGHT});
    propertyEditBox->setReadOnly();
    propertyEditBox->setText(property);
    propertyEditBox->setCaretPosition(0); // Show the first part of the contents instead of the last part when the text does not fit
    m_propertiesContainer->add(propertyEditBox, "Property" + property);

    auto valueEditBox = tgui::EditBox::create();
    valueEditBox->setPosition({(bindWidth(m_propertiesContainer) - 16.f) / 2.f, topPosition});
    valueEditBox->setSize({(bindWidth(m_propertiesContainer) - 16.f) / 2.f, EDIT_BOX_HEIGHT});
    valueEditBox->setText(value);
    valueEditBox->setCaretPosition(0); // Show the first part of the contents instead of the last part when the text does not fit
    valueEditBox->connect({"focused"}, [=]{ m_previousValue = valueEditBox->getText(); });
    m_propertiesContainer->add(valueEditBox, "Value" + property);

    topPosition += EDIT_BOX_HEIGHT - valueEditBox->getRenderer()->getBorders().getBottom();

    return valueEditBox;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::changeWidgetName(const std::string& name)
{
    m_selectedForm->setSelectedWidgetName(name);
    m_selectedWidgetComboBox->changeItemById(tgui::to_string(m_selectedForm->getSelectedWidget().get()), name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::initSelectedWidgetComboBoxAfterLoad()
{
    const auto& widgets = m_selectedForm->getWidgetsAndNames();
    for (const auto& pair : widgets)
    {
        const std::string id = tgui::to_string(pair.first.get());
        m_selectedWidgetComboBox->addItem(pair.second, id);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::removeSelectedWidget()
{
    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    const std::string id = tgui::to_string(selectedWidget.get());

    m_selectedForm->removeWidget(id);
    m_selectedWidgetComboBox->removeItemById(id);
    m_selectedWidgetComboBox->setSelectedItemById("form");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
