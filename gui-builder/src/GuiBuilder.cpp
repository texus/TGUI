/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include "WidgetProperties/BitmapButtonProperties.hpp"
#include "WidgetProperties/ButtonProperties.hpp"
#include "WidgetProperties/ChildWindowProperties.hpp"
#include "WidgetProperties/ComboBoxProperties.hpp"
#include "WidgetProperties/EditBoxProperties.hpp"
#include "WidgetProperties/GroupProperties.hpp"
#include "WidgetProperties/KnobProperties.hpp"
#include "WidgetProperties/LabelProperties.hpp"
#include "WidgetProperties/ListBoxProperties.hpp"
#include "WidgetProperties/PanelProperties.hpp"
#include "WidgetProperties/PictureProperties.hpp"
#include "WidgetProperties/ProgressBarProperties.hpp"
#include "WidgetProperties/RadioButtonProperties.hpp"
#include "WidgetProperties/RangeSliderProperties.hpp"
#include "WidgetProperties/ScrollablePanelProperties.hpp"
#include "WidgetProperties/ScrollbarProperties.hpp"
#include "WidgetProperties/SliderProperties.hpp"
#include "WidgetProperties/SpinButtonProperties.hpp"
#include "WidgetProperties/TabsProperties.hpp"
#include "WidgetProperties/TextBoxProperties.hpp"
#include "GuiBuilder.hpp"

#include <fstream>
#include <cassert>
#include <memory>
#include <string>
#include <cctype> // isdigit
#include <cmath> // max
#include <map>

#ifdef SFML_SYSTEM_WINDOWS
    #include <direct.h> // _getcwd
    #define getcwd _getcwd
#else
    #include <unistd.h> // getcwd
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const float EDIT_BOX_HEIGHT = 24;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    bool compareRenderers(std::map<std::string, tgui::ObjectConverter> themePropertyValuePairs, std::map<std::string, tgui::ObjectConverter> widgetPropertyValuePairs)
    {
        for (auto themeIt = themePropertyValuePairs.begin(); themeIt != themePropertyValuePairs.end(); ++themeIt)
        {
            if (((widgetPropertyValuePairs[themeIt->first].getType() == tgui::ObjectConverter::Type::None)
              && (themeIt->second.getType() != tgui::ObjectConverter::Type::None))
             || ((widgetPropertyValuePairs[themeIt->first].getType() != tgui::ObjectConverter::Type::None)
              && (themeIt->second.getType() == tgui::ObjectConverter::Type::None))
             || ((widgetPropertyValuePairs[themeIt->first].getType() == tgui::ObjectConverter::Type::String)
              && (themeIt->second.getType() != tgui::ObjectConverter::Type::String)
              && (widgetPropertyValuePairs[themeIt->first].getString() != themeIt->second.getString()))
             || ((widgetPropertyValuePairs[themeIt->first].getType() != tgui::ObjectConverter::Type::String)
              && (themeIt->second.getType() == tgui::ObjectConverter::Type::String)
              && (widgetPropertyValuePairs[themeIt->first].getString() != themeIt->second.getString()))
             || ((widgetPropertyValuePairs[themeIt->first].getType() != tgui::ObjectConverter::Type::String)
              && (themeIt->second.getType() != tgui::ObjectConverter::Type::String)
              && (widgetPropertyValuePairs[themeIt->first] != themeIt->second)))
            {
                // Exception: Colors should never be compared as strings
                if (((widgetPropertyValuePairs[themeIt->first].getType() == tgui::ObjectConverter::Type::Color)
                 && (themeIt->second.getType() == tgui::ObjectConverter::Type::String))
                || ((widgetPropertyValuePairs[themeIt->first].getType() == tgui::ObjectConverter::Type::String)
                 && (themeIt->second.getType() == tgui::ObjectConverter::Type::Color)))
                {
                    if (widgetPropertyValuePairs[themeIt->first].getColor() == themeIt->second.getColor())
                        continue;
                }

                // Exception: Don't use the data pointers and try to use absolute paths to compare textures
                if ((widgetPropertyValuePairs[themeIt->first].getType() == tgui::ObjectConverter::Type::Texture)
                 && (themeIt->second.getType() == tgui::ObjectConverter::Type::Texture)
                 && widgetPropertyValuePairs[themeIt->first].getTexture().getData()
                 && themeIt->second.getTexture().getData())
                {
                    if ((widgetPropertyValuePairs[themeIt->first].getTexture().getId() == themeIt->second.getTexture().getId())
                     && (widgetPropertyValuePairs[themeIt->first].getTexture().getMiddleRect() == themeIt->second.getTexture().getMiddleRect()))
                    {
                        continue;
                    }

                    char* buffer = getcwd(nullptr, 0);
                    if (buffer)
                    {
                        std::string workingDirectory = buffer;
                        free(buffer);

                        if (!workingDirectory.empty()
                         && (workingDirectory[workingDirectory.size() - 1] != '/')
                         && (workingDirectory[workingDirectory.size() - 1] != '\\'))
                        {
                            workingDirectory.push_back('/');
                        }

                        std::string absoluteFilename = workingDirectory + widgetPropertyValuePairs[themeIt->first].getTexture().getId();
                        if ((absoluteFilename == themeIt->second.getTexture().getId())
                         && (widgetPropertyValuePairs[themeIt->first].getTexture().getMiddleRect() == themeIt->second.getTexture().getMiddleRect()))
                        {
                            continue;
                        }
                    }
                }

                // Exception: Nested renderers need to check for the same exceptions
                if ((themeIt->second.getType() == tgui::ObjectConverter::Type::RendererData)
                 && (widgetPropertyValuePairs[themeIt->first].getType() == tgui::ObjectConverter::Type::RendererData))
                {
                    if (compareRenderers(themeIt->second.getRenderer()->propertyValuePairs,
                                         widgetPropertyValuePairs[themeIt->first].getRenderer()->propertyValuePairs))
                    {
                        continue;
                    }
                }

                return false;
            }
        }

        for (auto widgetIt = widgetPropertyValuePairs.begin(); widgetIt != widgetPropertyValuePairs.end(); ++widgetIt)
        {
            if (((themePropertyValuePairs[widgetIt->first].getType() == tgui::ObjectConverter::Type::None)
              && (widgetIt->second.getType() != tgui::ObjectConverter::Type::None))
             || ((themePropertyValuePairs[widgetIt->first].getType() != tgui::ObjectConverter::Type::None)
              && (widgetIt->second.getType() == tgui::ObjectConverter::Type::None))
             || ((themePropertyValuePairs[widgetIt->first].getType() == tgui::ObjectConverter::Type::String)
              && (widgetIt->second.getType() != tgui::ObjectConverter::Type::String)
              && (themePropertyValuePairs[widgetIt->first].getString() != widgetIt->second.getString()))
             || ((themePropertyValuePairs[widgetIt->first].getType() != tgui::ObjectConverter::Type::String)
              && (widgetIt->second.getType() == tgui::ObjectConverter::Type::String)
              && (themePropertyValuePairs[widgetIt->first].getString() != widgetIt->second.getString()))
             || ((themePropertyValuePairs[widgetIt->first].getType() != tgui::ObjectConverter::Type::String)
              && (widgetIt->second.getType() != tgui::ObjectConverter::Type::String)
              && (themePropertyValuePairs[widgetIt->first] != widgetIt->second)))
            {
                // Exception: An empty texture is considered the same as an empty property
                if ((themePropertyValuePairs[widgetIt->first].getType() == tgui::ObjectConverter::Type::None)
                 && (widgetIt->second.getType() == tgui::ObjectConverter::Type::Texture)
                 && !widgetIt->second.getTexture().getData())
                {
                    continue;
                }

                // Exception: Textures need to be checked differently, but this is already handled in earlier check
                if ((themePropertyValuePairs[widgetIt->first].getType() == tgui::ObjectConverter::Type::Texture)
                 && (widgetIt->second.getType() == tgui::ObjectConverter::Type::Texture)
                 && themePropertyValuePairs[widgetIt->first].getTexture().getData()
                 && widgetIt->second.getTexture().getData())
                {
                    continue;
                }

                // Exception: Renderers need to be checked differently, but this is already handled in earlier check
                if ((widgetIt->second.getType() == tgui::ObjectConverter::Type::RendererData)
                 && (themePropertyValuePairs[widgetIt->first].getType() == tgui::ObjectConverter::Type::RendererData))
                {
                    continue;
                }

                return false;
            }
        }

        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiBuilder::GuiBuilder() :
    m_window{{1300, 680}, "TGUI - GUI Builder"},
    m_gui         {m_window},
    m_themes      {{"White", *tgui::Theme::getDefault()}},
    m_defaultTheme{"White"}
{
    m_window.setFramerateLimit(60);

    m_widgetProperties["BitmapButton"] = std::make_unique<BitmapButtonProperties>();
    m_widgetProperties["Button"] = std::make_unique<ButtonProperties>();
    m_widgetProperties["CheckBox"] = std::make_unique<RadioButtonProperties>();
    m_widgetProperties["ChildWindow"] = std::make_unique<ChildWindowProperties>();
    m_widgetProperties["ComboBox"] = std::make_unique<ComboBoxProperties>();
    m_widgetProperties["EditBox"] = std::make_unique<EditBoxProperties>();
    m_widgetProperties["Group"] = std::make_unique<GroupProperties>();
    m_widgetProperties["Knob"] = std::make_unique<KnobProperties>();
    m_widgetProperties["Label"] = std::make_unique<LabelProperties>();
    m_widgetProperties["ListBox"] = std::make_unique<ListBoxProperties>();
    m_widgetProperties["Panel"] = std::make_unique<PanelProperties>();
    m_widgetProperties["Picture"] = std::make_unique<PictureProperties>();
    m_widgetProperties["ProgressBar"] = std::make_unique<ProgressBarProperties>();
    m_widgetProperties["RadioButton"] = std::make_unique<RadioButtonProperties>();
    m_widgetProperties["RangeSlider"] = std::make_unique<RangeSliderProperties>();
    m_widgetProperties["ScrollablePanel"] = std::make_unique<ScrollablePanelProperties>();
    m_widgetProperties["Scrollbar"] = std::make_unique<ScrollbarProperties>();
    m_widgetProperties["Slider"] = std::make_unique<SliderProperties>();
    m_widgetProperties["SpinButton"] = std::make_unique<SpinButtonProperties>();
    m_widgetProperties["Tabs"] = std::make_unique<TabsProperties>();
    m_widgetProperties["TextBox"] = std::make_unique<TextBoxProperties>();

    std::ifstream stateInputFile{"GuiBuilderState.txt"};
    if (stateInputFile.is_open())
    {
        std::stringstream stream;
        stream << stateInputFile.rdbuf();

        const auto node = tgui::DataIO::parse(stream);
        m_lastOpenedFile = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, node->propertyValuePairs["lastopenedfile"]->value).getString();
        for (const auto& theme : node->propertyValuePairs["themes"]->valueList)
        {
            const auto deserializedTheme = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, theme).getString();
            m_themes[deserializedTheme] = {deserializedTheme};
        }
    }
    else
    {
        m_lastOpenedFile = "form.txt";
        m_themes["themes/Black.txt"] = {"themes/Black.txt"};
        m_themes["themes/BabyBlue.txt"] = {"themes/BabyBlue.txt"};
        m_themes["themes/TransparentGrey.txt"] = {"themes/TransparentGrey.txt"};
    }

    sf::Image icon;
    if (icon.loadFromFile("resources/Icon.png"))
        m_window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    loadStartScreen();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiBuilder::~GuiBuilder()
{
    auto node = std::make_unique<tgui::DataIO::Node>();
    node->propertyValuePairs["LastOpenedFile"] = std::make_unique<tgui::DataIO::ValueNode>(tgui::Serializer::serialize(m_lastOpenedFile));

    if (m_themes.size() > 1)
    {
        auto themeIt = m_themes.begin();
        if (themeIt->first == "White")
            themeIt++;

        std::string themeList = "[" + tgui::Serializer::serialize(themeIt->first);
        for (; themeIt != m_themes.end(); ++themeIt)
        {
            if (themeIt->first != "White")
                themeList += ", " + tgui::Serializer::serialize(themeIt->first);
        }

        themeList += "]";
        node->propertyValuePairs["Themes"] = std::make_unique<tgui::DataIO::ValueNode>(themeList);
    }

    std::stringstream stream;
    tgui::DataIO::emit(node, stream);

    std::ofstream stateOutputFile{"GuiBuilderState.txt"};
    if (stateOutputFile.is_open())
        stateOutputFile << stream.rdbuf();
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
                m_window.setView(sf::View({0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)}));
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
            else if (event.type == sf::Event::KeyPressed)
            {
                if ((event.key.code == sf::Keyboard::Key::Left) || (event.key.code == sf::Keyboard::Key::Right)
                 || (event.key.code == sf::Keyboard::Key::Up) || (event.key.code == sf::Keyboard::Key::Down))
                {
                    if (m_selectedForm && m_selectedForm->hasFocus())
                        m_selectedForm->arrowKeyPressed(event.key);
                }
                else if (event.key.code == sf::Keyboard::Key::Delete)
                {
                    if (m_selectedForm && m_selectedForm->hasFocus() && m_selectedForm->getSelectedWidget())
                        removeSelectedWidget();
                }
                else if (event.key.code == sf::Keyboard::Key::Escape)
                {
                    if (m_selectedForm && m_selectedForm->hasFocus())
                        m_selectedForm->selectParent();
                }
                else if ((event.key.code == sf::Keyboard::Key::S) && event.key.control)
                {
                    if (m_selectedForm)
                        m_selectedForm->save();
                }
                else if ((event.key.code == sf::Keyboard::Key::C) && event.key.control)
                {
                    if (m_selectedForm && m_selectedForm->hasFocus() && m_selectedForm->getSelectedWidget())
                        copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
                }
                else if ((event.key.code == sf::Keyboard::Key::V) && event.key.control)
                {
                    if (m_selectedForm && m_selectedForm->hasFocus())
                        pasteWidgetFromInternalClipboard();
                }
                else if ((event.key.code == sf::Keyboard::Key::X) && event.key.control)
                {
                    if (m_selectedForm && m_selectedForm->hasFocus() && m_selectedForm->getSelectedWidget())
                    {
                        copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
                        removeSelectedWidget();
                    }
                }
            }

            m_gui.handleEvent(event);
        }

        m_window.clear({200, 200, 200});
        m_gui.draw();

        if (m_selectedForm && m_selectedForm->hasFocus())
            m_selectedForm->drawExtra(m_window);

        m_window.display();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::reloadProperties()
{
    auto selectedWidget = m_selectedForm->getSelectedWidget();
    float topPosition = 0;

    if (selectedWidget)
    {
        addPropertyValueWidgets(topPosition, {"Name", {"String", selectedWidget->name}},
            [=](const sf::String& value){
                if (selectedWidget->name != value)
                {
                    m_selectedForm->setChanged(true);
                    changeWidgetName(value);
                }
            });

        topPosition += 10;
        m_propertyValuePairs = m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->initProperties(selectedWidget->ptr);
        for (const auto& property : m_propertyValuePairs.first)
        {
            addPropertyValueWidgets(topPosition, property,
                [=](const sf::String& value){
                    if (updateWidgetProperty(property.first, value))
                        m_selectedForm->setChanged(true);
                });
        }

        topPosition += 10;
        auto rendererComboBox = m_propertiesContainer->get<tgui::ComboBox>("RendererSelectorComboBox");
        rendererComboBox->setPosition({0, topPosition});

        if (static_cast<std::size_t>(rendererComboBox->getSelectedItemIndex() + 1) == rendererComboBox->getItemCount()) // If "Custom" is selected
        {
            topPosition += rendererComboBox->getSize().y + 10;
            for (const auto& property : m_propertyValuePairs.second)
            {
                addPropertyValueWidgets(topPosition, property,
                    [=](const sf::String& value){
                        if (updateWidgetProperty(property.first, value))
                        {
                            m_selectedForm->setChanged(true);

                            // The value shouldn't always be exactly as typed. An empty string may be understood correctly when setting the property,
                            // but is can't be saved to a widget file properly. So we read the back the property to have a valid string and pass it
                            // back to the widget, so that the string stored in the renderer is always a valid string.
                            m_selectedForm->getSelectedWidget()->ptr->getRenderer()->setProperty(property.first, m_propertyValuePairs.second[property.first].second);
                        }
                    });
            }

            rendererComboBox->moveToFront();
        }
    }
    else // The form itself was selected
    {
        addPropertyValueWidgets(topPosition, {"Filename", {"String", m_selectedForm->getFilename()}},
            [=](const sf::String& value){
                if (m_selectedForm->getFilename() != value)
                {
                    m_selectedForm->setChanged(true);
                    m_selectedForm->setFilename(value);
                    m_selectedWidgetComboBox->changeItemById("form", value);
                }
            });

        addPropertyValueWidgets(topPosition, {"Width", {"UInt", tgui::to_string(m_selectedForm->getSize().x)}},
            [=](const sf::String& value){
                if (tgui::to_string(m_selectedForm->getSize().x) != value)
                {
                    // Form is not marked as changed since the width is not saved
                    m_selectedForm->setSize({tgui::stoi(value), m_selectedForm->getSize().y});
                }
            });

        addPropertyValueWidgets(topPosition, {"Height", {"UInt", tgui::to_string(m_selectedForm->getSize().y)}},
            [=](const sf::String& value){
                if (tgui::to_string(m_selectedForm->getSize().y) != value)
                {
                    // Form is not marked as changed since the height is not saved
                    m_selectedForm->setSize({m_selectedForm->getSize().x, tgui::stoi(value)});
                }
            });
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
    panel->getRenderer()->setBackgroundColor({0, 0, 0, 175});
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
                m_forms.clear();
            }
            else if (event.type == sf::Event::Resized)
            {
                m_window.setView(sf::View({0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)}));
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
    m_gui.loadWidgetsFromFile("resources/forms/StartScreen.txt");

    auto filenameEditBox = m_gui.get<tgui::Panel>("MainPanel")->get<tgui::EditBox>("FilenameEditBox");
    filenameEditBox->setText(m_lastOpenedFile);

    m_gui.get<tgui::Panel>("MainPanel")->get("NewButton")->connect("pressed", [=]{ loadEditingScreen(filenameEditBox->getText()); m_selectedForm->setChanged(true); });

    m_gui.get<tgui::Panel>("MainPanel")->get("LoadButton")->connect("pressed", [=]{ loadEditingScreen(filenameEditBox->getText()); loadForm(); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadEditingScreen(const std::string& filename)
{
    m_lastOpenedFile = filename;

    m_gui.removeAllWidgets();
    m_gui.loadWidgetsFromFile("resources/forms/EditingScreen.txt");

    m_forms.push_back(std::make_unique<Form>(this, filename, m_gui.get<tgui::ChildWindow>("Form")));
    m_selectedForm = m_forms[0].get();

    m_propertiesWindow = m_gui.get<tgui::ChildWindow>("PropertiesWindow");
    m_propertiesContainer = m_propertiesWindow->get<tgui::ScrollablePanel>("Properties");
    m_selectedWidgetComboBox = m_propertiesWindow->get<tgui::ComboBox>("SelectedWidgetComboBox");

    m_selectedWidgetComboBox->addItem(filename, "form");
    m_selectedWidgetComboBox->setSelectedItemById("form");
    m_selectedWidgetComboBox->connect("ItemSelected", [this](std::string, std::string id){ m_selectedForm->selectWidgetById(id); });

    m_menuBar = m_gui.get<tgui::MenuBar>("MenuBar");
    m_menuBar->connect("MouseEntered", [](tgui::Widget::Ptr menuBar, std::string){ menuBar->moveToFront(); });
    m_menuBar->connectMenuItem({"File", "New / Load"}, [this]{ menuBarCallbackNewOrLoadFile(); });
    m_menuBar->connectMenuItem({"File", "Save"}, [this]{ menuBarCallbackSaveFile(); });
    m_menuBar->connectMenuItem({"File", "Quit"}, [this]{ menuBarCallbackQuit(); });
    m_menuBar->connectMenuItem({"Themes", "Edit"}, [this]{ menuBarCallbackEditThemes(); });
    m_menuBar->connectMenuItem({"Widget", "Bring to front"}, [this]{ menuBarCallbackBringWidgetToFront(); });
    m_menuBar->connectMenuItem({"Widget", "Send to back"}, [this]{ menuBarCallbackSendWidgetToBack(); });
    m_menuBar->connectMenuItem({"Widget", "Cut"}, [this]{ menuBarCallbackCutWidget(); });
    m_menuBar->connectMenuItem({"Widget", "Copy"}, [this]{ menuBarCallbackCopyWidget(); });
    m_menuBar->connectMenuItem({"Widget", "Paste"}, [this]{ menuBarCallbackPasteWidget(); });
    m_menuBar->connectMenuItem({"Widget", "Delete"}, [this]{ menuBarCallbackDeleteWidget(); });
    m_menuBar->connectMenuItem({"Help", "Keyboard shortcuts"}, [this]{ menuBarCallbackKeyboardShortcuts(); });
    m_menuBar->connectMenuItem({"Help", "About"}, [this]{ menuBarCallbackAbout(); });

    loadToolbox();
    initProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadToolbox()
{
    auto toolboxWindow = m_gui.get<tgui::ChildWindow>("ToolboxWindow");
    auto toolbox = toolboxWindow->get<tgui::ScrollablePanel>("Widgets");

    const auto widgets = std::vector<std::pair<std::string, std::function<tgui::Widget::Ptr()>>>{
        {"BitmapButton", []{ return tgui::BitmapButton::create("BitBtn"); }},
        {"Button", []{ return tgui::Button::create("Button"); }},
        {"CheckBox", []{ return tgui::CheckBox::create(); }},
        {"ChildWindow", []{ return tgui::ChildWindow::create(); }},
        {"ComboBox", []{ return tgui::ComboBox::create(); }},
        {"EditBox", []{ return tgui::EditBox::create(); }},
        {"Group", []{ return tgui::Group::create({150, 150}); }},
        {"Knob", []{ return tgui::Knob::create(); }},
        {"Label", []{ return tgui::Label::create("Label"); }},
        {"ListBox", []{ return tgui::ListBox::create(); }},
        {"Panel", []{ return tgui::Panel::create({150, 150}); }},
        {"Picture", []{ return tgui::Picture::create("resources/DefaultPicture.png"); }},
        {"ProgressBar", []{ return tgui::ProgressBar::create(); }},
        {"RadioButton", []{ return tgui::RadioButton::create(); }},
        {"RangeSlider", []{ return tgui::RangeSlider::create(); }},
        {"ScrollablePanel", []{ return tgui::ScrollablePanel::create({150, 150}); }},
        {"Scrollbar", []{ return tgui::Scrollbar::create(); }},
        {"Slider", []{ return tgui::Slider::create(); }},
        {"SpinButton", []{ return tgui::SpinButton::create(); }},
        {"Tabs", []{ auto tabs = tgui::Tabs::create(); tabs->add("Tab", false); return tabs; }},
        {"TextBox", []{ return tgui::TextBox::create(); }},
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
        verticalLayout->setSize({bindWidth(toolbox) - toolbox->getScrollbarWidth(), icon->getSize().y + 4});
        verticalLayout->getRenderer()->setPadding({2});

        auto panel = tgui::Panel::create();
        panel->getRenderer()->setBackgroundColor(sf::Color::Transparent);
        panel->add(icon);
        panel->add(name);
        verticalLayout->add(panel);
        toolbox->add(verticalLayout);

        panel->connect("Clicked", [=]{
            createNewWidget(widget.second());

            auto selectedWidget = m_selectedForm->getSelectedWidget();

            auto renderer = m_themes[m_defaultTheme].getRendererNoThrow(selectedWidget->ptr->getWidgetType());

            // Although the white theme has an empty Picture renderer, the gui builder should not use it and display a placeholder image instead
            if ((widget.first == "Picture") && (m_defaultTheme == "White"))
                renderer = nullptr;

            if (renderer)
            {
                selectedWidget->theme = m_defaultTheme;
                selectedWidget->ptr->setRenderer(renderer);
            }
            else
                selectedWidget->theme = "Custom";
        });

        topPosition += verticalLayout->getSize().y;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::createNewWidget(tgui::Widget::Ptr widget)
{
    tgui::Container* parent = nullptr;
    tgui::Widget::Ptr selectedWidget = m_selectedForm->getSelectedWidget() ? m_selectedForm->getSelectedWidget()->ptr : nullptr;
    if (selectedWidget && selectedWidget->isContainer())
        parent = static_cast<tgui::Container*>(selectedWidget.get());
    else if (selectedWidget)
        parent = selectedWidget->getParent();

    const std::string id = tgui::to_string(widget.get());
    const std::string name = m_selectedForm->addWidget(widget, parent);
    m_selectedWidgetComboBox->addItem(name, id);
    m_selectedWidgetComboBox->setSelectedItemById(id);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::recursiveCopyWidget(tgui::Container::Ptr oldContainer, tgui::Container::Ptr newContainer)
{
    const auto& oldWidgets = oldContainer->getWidgets();
    const auto& newWidgets = newContainer->getWidgets();
    assert(oldWidgets.size() == newWidgets.size());

    for (std::size_t i = 0; i < newWidgets.size(); ++i)
    {
        const std::string name = m_selectedForm->addExistingWidget(newWidgets[i]);
        m_selectedWidgetComboBox->addItem(name, tgui::to_string(newWidgets[i].get()));

        m_selectedForm->getWidget(tgui::to_string(newWidgets[i].get()))->theme = m_selectedForm->getWidget(tgui::to_string(oldWidgets[i].get()))->theme;

        if (newWidgets[i]->isContainer())
            recursiveCopyWidget(std::static_pointer_cast<tgui::Container>(oldWidgets[i]), std::static_pointer_cast<tgui::Container>(newWidgets[i]));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::copyWidget(std::shared_ptr<WidgetInfo> widgetInfo)
{
    tgui::Widget::Ptr widget = widgetInfo->ptr->clone();

    const std::string id = tgui::to_string(widget.get());
    const std::string name = m_selectedForm->addWidget(widget, widgetInfo->ptr->getParent());
    m_selectedWidgetComboBox->addItem(name, id);

    if (widget->isContainer())
        recursiveCopyWidget(std::static_pointer_cast<tgui::Container>(widgetInfo->ptr), std::static_pointer_cast<tgui::Container>(widget));

    m_selectedWidgetComboBox->setSelectedItemById(id);
    m_selectedForm->getSelectedWidget()->theme = widgetInfo->theme;
    initProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiBuilder::updateWidgetProperty(const std::string& property, const std::string& value)
{
    sf::String oldValue;
    if (m_propertyValuePairs.first.find(property) != m_propertyValuePairs.first.end())
        oldValue = m_propertyValuePairs.first[property].second;
    else
        oldValue = m_propertyValuePairs.second[property].second;

    if (oldValue == value)
        return false;

    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    assert(selectedWidget != nullptr);

    bool valueChanged;
    try
    {
        m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->updateProperty(selectedWidget->ptr, property, value);
        valueChanged = true;
    }
    catch (const tgui::Exception& e)
    {
        std::cout << "Exception caught when setting property: " << e.what() << std::endl;

        m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->updateProperty(selectedWidget->ptr, property, oldValue);
        valueChanged = false;
    }

    reloadProperties(); // reload all properties in case something else changed
    m_selectedForm->updateSelectionSquarePositions(); // update the position of the selection squares in case the position or size of the widget changed
    return valueChanged;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::initProperties()
{
    m_propertiesContainer->removeAllWidgets();

    auto selectedWidget = m_selectedForm->getSelectedWidget();

    m_menuBar->setMenuItemEnabled({"Widget", "Bring to front"}, (selectedWidget != nullptr));
    m_menuBar->setMenuItemEnabled({"Widget", "Send to back"}, (selectedWidget != nullptr));
    m_menuBar->setMenuItemEnabled({"Widget", "Cut"}, (selectedWidget != nullptr));
    m_menuBar->setMenuItemEnabled({"Widget", "Copy"}, (selectedWidget != nullptr));
    m_menuBar->setMenuItemEnabled({"Widget", "Paste"}, !m_copiedWidgetType.empty());
    m_menuBar->setMenuItemEnabled({"Widget", "Delete"}, (selectedWidget != nullptr));
    m_menuBar->setMenuEnabled("Widget", (selectedWidget != nullptr) || !m_copiedWidgetType.empty());

    if (selectedWidget)
    {
        const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

        auto rendererComboBox = tgui::ComboBox::create();
        rendererComboBox->setSize({bindWidth(m_propertiesContainer) - scrollbarWidth, EDIT_BOX_HEIGHT});
        rendererComboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Automatic);
        rendererComboBox->setChangeItemOnScroll(false);

        for (auto& theme : m_themes)
            rendererComboBox->addItem(theme.first);

        rendererComboBox->addItem("Custom");

        // Set the theme to Custom if the theme used by the widget would have been deleted
        if (!rendererComboBox->contains(selectedWidget->theme))
            selectedWidget->theme = "Custom";

        rendererComboBox->setSelectedItem(selectedWidget->theme);
        m_propertiesContainer->add(rendererComboBox, "RendererSelectorComboBox");

        rendererComboBox->connect("ItemSelected", [=](const std::string& item){
            selectedWidget->theme = item;
            if (item != "Custom")
                selectedWidget->ptr->setRenderer(m_themes[item].getRendererNoThrow(selectedWidget->ptr->getWidgetType()));
            else
                selectedWidget->ptr->setRenderer(selectedWidget->ptr->getRenderer()->getData());

            m_selectedForm->setChanged(true);
            initProperties();
        });
    }

    reloadProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueWidgets(float& topPosition, const PropertyValuePair& propertyValuePair, const OnValueChangeFunc& onChange)
{
    const auto& property = propertyValuePair.first;
    const auto& type = propertyValuePair.second.first;
    const auto& value = propertyValuePair.second.second;
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto propertyEditBox = m_propertiesContainer->get<tgui::EditBox>("Property" + property);
    if (!propertyEditBox)
    {
        propertyEditBox = tgui::EditBox::create();
        propertyEditBox->setPosition({0, topPosition});
        propertyEditBox->setSize({((bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f) + propertyEditBox->getRenderer()->getBorders().getRight(), EDIT_BOX_HEIGHT});
        propertyEditBox->setReadOnly();
        propertyEditBox->setText(property);
        m_propertiesContainer->add(propertyEditBox, "Property" + property);
        propertyEditBox->setCaretPosition(0); // Show the first part of the contents instead of the last part when the text does not fit
        propertyEditBox->connect("Focused", [=]{ m_propertiesContainer->focusNextWidget(); });
    }

    if (type == "Bool")
        addPropertyValueBool(property, value, onChange, topPosition);
    else if (type == "Color")
        addPropertyValueColor(property, value, onChange, topPosition);
    else if (type == "TextStyle")
        addPropertyValueTextStyle(property, value, onChange, topPosition);
    else if (type == "Outline")
        addPropertyValueOutline(property, value, onChange, topPosition);
    else if (type == "MultilineString")
        addPropertyValueMultilineString(property, value, onChange, topPosition);
    else if (type == "List<String>")
        addPropertyValueStringList(property, value, onChange, topPosition);
    else if (type == "Texture")
        addPropertyValueTexture(property, value, onChange, topPosition);
    else if (type == "EditBoxInputValidator")
        addPropertyValueEditBoxInputValidator(property, value, onChange, topPosition);
    else if (type == "ChildWindowTitleButtons")
        addPropertyValueChildWindowTitleButtons(property, value, onChange, topPosition);
    else if (type.substr(0, 5) == "Enum{")
    {
        const std::vector<std::string> enumValues = tgui::Deserializer::split(type.substr(5, type.size() - 6), ',');
        addPropertyValueEnum(property, value, onChange, topPosition, enumValues);
    }
    else if (type == "Font")
    {
        // TODO: Allowing picking font from list (e.g. managed similar to themes) or via file dialog.
        addPropertyValueEditBox(property, value, onChange, topPosition, 0);
    }
    else
    {
        auto valueEditBox = addPropertyValueEditBox(property, value, onChange, topPosition, 0);
        if (type == "UInt")
            valueEditBox->setInputValidator(tgui::EditBox::Validator::UInt);
        else if (type == "Int")
            valueEditBox->setInputValidator(tgui::EditBox::Validator::Int);
        else if (type == "Float")
            valueEditBox->setInputValidator(tgui::EditBox::Validator::Float);
        else if (type == "Char")
            valueEditBox->setMaximumCharacters(1);
    }

    topPosition += EDIT_BOX_HEIGHT - propertyEditBox->getRenderer()->getBorders().getBottom();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::changeWidgetName(const std::string& name)
{
    assert(m_selectedForm->getSelectedWidget() != nullptr);

    m_selectedForm->setSelectedWidgetName(name);
    m_selectedWidgetComboBox->changeItemById(tgui::to_string(m_selectedForm->getSelectedWidget()->ptr.get()), name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::initSelectedWidgetComboBoxAfterLoad()
{
    const auto& widgets = m_selectedForm->getWidgets();
    for (const auto& widget : widgets)
    {
        const std::string id = tgui::to_string(widget->ptr.get());
        m_selectedWidgetComboBox->addItem(widget->name, id);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::removeSelectedWidget()
{
    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    assert(selectedWidget != nullptr);

    const std::string id = tgui::to_string(selectedWidget->ptr.get());

    m_selectedForm->removeWidget(id);
    m_selectedWidgetComboBox->removeItemById(id);
    m_selectedWidgetComboBox->setSelectedItemById("form");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadForm()
{
    if (!m_selectedForm->load())
    {
        loadStartScreen();
        return;
    }

    // Try to match renderers with themes (this could create false positives but it is better than not being able to load themes at all)
    // Many cases are still unsupported (e.g. nested renderers), in which case the renderer will not be shared after loading
    for (auto& widget : m_selectedForm->getWidgets())
    {
        for (auto& theme : m_themes)
        {
            auto themeRenderer = theme.second.getRendererNoThrow(widget->ptr->getWidgetType());
            if (themeRenderer == nullptr)
                continue;

            // Create a temporary widget with the theme so that the types of the properties are set (otherwise all properties are still just strings).
            // This should make the comparison below slightly more accurate as it allows to compare some types instead of only strings.
            tgui::WidgetFactory::getConstructFunction(widget->ptr->getWidgetType())()->setRenderer(themeRenderer);

            if (compareRenderers(themeRenderer->propertyValuePairs, widget->ptr->getSharedRenderer()->getPropertyValuePairs()))
            {
                widget->theme = theme.first;
                widget->ptr->setRenderer(themeRenderer); // Use the exact same renderer as the new widgets to keep it shared
                break;
            }
        }
    }

    initSelectedWidgetComboBoxAfterLoad();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::ChildWindow::Ptr GuiBuilder::openWindowWithFocus()
{
    auto panel = tgui::Panel::create({"100%", "100%"});
    panel->getRenderer()->setBackgroundColor({0, 0, 0, 175});
    m_gui.add(panel);

    auto window = tgui::ChildWindow::create();
    window->setPosition("(&.w - w) / 2", "(&.h - h) / 2");
    m_gui.add(window);

    window->setFocused(true);

    const bool tabUsageEnabled = m_gui.isTabKeyUsageEnabled();
    auto closeWindow = [=]{
        m_gui.remove(window);
        m_gui.remove(panel);
        m_gui.setTabKeyUsageEnabled(tabUsageEnabled);
    };

    panel->connect("Clicked", closeWindow);
    window->connect({"Closed", "EscapeKeyPressed"}, closeWindow);

    return window;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::copyWidgetToInternalClipboard(std::shared_ptr<WidgetInfo> widgetInfo)
{
    m_copiedWidgetType = widgetInfo->ptr->getWidgetType();
    m_copiedWidgetTheme = widgetInfo->theme;
    m_copiedWidgetPropertyValuePairs = m_widgetProperties.at(m_copiedWidgetType)->initProperties(widgetInfo->ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::pasteWidgetFromInternalClipboard()
{
    if (m_copiedWidgetType.empty())
        return;

    auto widget = tgui::WidgetFactory::getConstructFunction(m_copiedWidgetType)();
    createNewWidget(widget);

    for (const auto& property : m_copiedWidgetPropertyValuePairs.first)
        updateWidgetProperty(property.first, property.second.second);
    for (const auto& property : m_copiedWidgetPropertyValuePairs.second)
        updateWidgetProperty(property.first, property.second.second);

    m_selectedForm->getSelectedWidget()->theme = m_copiedWidgetTheme;
    initProperties();

    m_selectedForm->setChanged(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::EditBox::Ptr GuiBuilder::addPropertyValueEditBox(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition, float rightPadding)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto valueEditBox = m_propertiesContainer->get<tgui::EditBox>("Value" + property);
    if (!valueEditBox)
    {
        valueEditBox = tgui::EditBox::create();
        m_propertiesContainer->add(valueEditBox, "Value" + property);
        valueEditBox->setCaretPosition(0); // Show the first part of the contents instead of the last part when the text does not fit
    }

    valueEditBox->disconnectAll("Unfocused");
    valueEditBox->disconnectAll("ReturnKeyPressed");
    valueEditBox->setPosition({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, topPosition});
    valueEditBox->setSize({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f - rightPadding, EDIT_BOX_HEIGHT});
    valueEditBox->setText(value);

    valueEditBox->connect({"ReturnKeyPressed", "Unfocused"}, [=]{ onChange(valueEditBox->getText()); });
    return valueEditBox;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::Button::Ptr GuiBuilder::addPropertyValueButtonMore(const std::string& property, float topPosition)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto buttonMore = m_propertiesContainer->get<tgui::Button>("ValueButton" + property);
    if (!buttonMore)
    {
        buttonMore = tgui::Button::create();
        buttonMore->setText(L"\u22EF");
        buttonMore->setTextSize(18);
        buttonMore->connect("Focused", [=]{ m_propertiesContainer->focusNextWidget(); });
        m_propertiesContainer->add(buttonMore, "ValueButton" + property);
    }

    buttonMore->disconnectAll("pressed");
    buttonMore->setSize({EDIT_BOX_HEIGHT, EDIT_BOX_HEIGHT});
    buttonMore->setPosition({bindWidth(m_propertiesContainer) - scrollbarWidth - EDIT_BOX_HEIGHT, topPosition});
    return buttonMore;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueBool(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto valueComboBox = m_propertiesContainer->get<tgui::ComboBox>("ValueComboBox" + property);
    if (!valueComboBox)
    {
        valueComboBox = tgui::ComboBox::create();
        valueComboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Automatic);
        valueComboBox->setChangeItemOnScroll(false);
        valueComboBox->addItem("False");
        valueComboBox->addItem("True");
        m_propertiesContainer->add(valueComboBox, "ValueComboBox" + property);
    }

    valueComboBox->disconnectAll("ItemSelected");
    valueComboBox->setPosition({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, topPosition});
    valueComboBox->setSize({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, EDIT_BOX_HEIGHT});

    std::string str = tgui::toLower(value);
    if (str == "true" || str == "yes" || str == "on" || str == "y" || str == "t" || str == "1")
        valueComboBox->setSelectedItemByIndex(1);
    else
        valueComboBox->setSelectedItemByIndex(0);

    valueComboBox->connect("ItemSelected", [=]{ onChange(valueComboBox->getSelectedItem()); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueColor(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto transparentPicture = m_propertiesContainer->get<tgui::Picture>("ValueTransparentPicture" + property);
    auto colorPreviewPanel = m_propertiesContainer->get<tgui::Panel>("ValueColorPanel" + property);
    if (!transparentPicture)
    {
        transparentPicture = tgui::Picture::create("resources/Transparent.png");
        m_propertiesContainer->add(transparentPicture, "ValueTransparentPicture" + property);

        colorPreviewPanel = tgui::Panel::create();
        colorPreviewPanel->getRenderer()->setBorders(1);
        colorPreviewPanel->getRenderer()->setBorderColor(sf::Color::Black);
        m_propertiesContainer->add(colorPreviewPanel, "ValueColorPanel" + property);
    }

    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);
    transparentPicture->setSize({EDIT_BOX_HEIGHT, EDIT_BOX_HEIGHT});
    transparentPicture->setPosition({bindWidth(m_propertiesContainer) - scrollbarWidth - EDIT_BOX_HEIGHT, topPosition});

    colorPreviewPanel->setSize({EDIT_BOX_HEIGHT, EDIT_BOX_HEIGHT});
    colorPreviewPanel->setPosition({bindWidth(m_propertiesContainer) - scrollbarWidth - EDIT_BOX_HEIGHT, topPosition});
    if (value != "None")
        colorPreviewPanel->getRenderer()->setBackgroundColor(value);
    else
        colorPreviewPanel->getRenderer()->setBackgroundColor(sf::Color::Transparent);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueTextStyle(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto textStyleWindow = openWindowWithFocus();
        textStyleWindow->setTitle("Set text style");
        textStyleWindow->setSize(180, 160);
        textStyleWindow->loadWidgetsFromFile("resources/forms/SetTextStyle.txt");

        auto checkBoxBold = textStyleWindow->get<tgui::CheckBox>("CheckBoxBold");
        auto checkBoxItalic = textStyleWindow->get<tgui::CheckBox>("CheckBoxItalic");
        auto checkBoxUnderlined = textStyleWindow->get<tgui::CheckBox>("CheckBoxUnderlined");
        auto checkBoxStrikeThrough = textStyleWindow->get<tgui::CheckBox>("CheckBoxStrikeThrough");

        unsigned int style = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::TextStyle, value).getTextStyle();
        checkBoxBold->setChecked(style & sf::Text::Style::Bold);
        checkBoxItalic->setChecked(style & sf::Text::Style::Italic);
        checkBoxUnderlined->setChecked(style & sf::Text::Style::Underlined);
        checkBoxStrikeThrough->setChecked(style & sf::Text::Style::StrikeThrough);

        auto updateTextStyleProperty = [=]{
            unsigned int newStyle = 0;
            newStyle |= (checkBoxBold->isChecked() ? sf::Text::Style::Bold : 0);
            newStyle |= (checkBoxItalic->isChecked() ? sf::Text::Style::Italic : 0);
            newStyle |= (checkBoxUnderlined->isChecked() ? sf::Text::Style::Underlined : 0);
            newStyle |= (checkBoxStrikeThrough->isChecked() ? sf::Text::Style::StrikeThrough : 0);
            onChange(tgui::Serializer::serialize(tgui::TextStyle{newStyle}));
        };
        checkBoxBold->connect("changed", updateTextStyleProperty);
        checkBoxItalic->connect("changed", updateTextStyleProperty);
        checkBoxUnderlined->connect("changed", updateTextStyleProperty);
        checkBoxStrikeThrough->connect("changed", updateTextStyleProperty);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueOutline(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto outlineWindow = openWindowWithFocus();
        outlineWindow->setTitle("Set outline");
        outlineWindow->setSize(150, 150);
        outlineWindow->loadWidgetsFromFile("resources/forms/SetOutline.txt");

        auto editLeft = outlineWindow->get<tgui::EditBox>("EditLeft");
        auto editTop = outlineWindow->get<tgui::EditBox>("EditTop");
        auto editRight = outlineWindow->get<tgui::EditBox>("EditRight");
        auto editBottom = outlineWindow->get<tgui::EditBox>("EditBottom");

        tgui::Outline outline = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Outline, value).getOutline();
        editLeft->setText(tgui::to_string(outline.getLeft()));
        editTop->setText(tgui::to_string(outline.getTop()));
        editRight->setText(tgui::to_string(outline.getRight()));
        editBottom->setText(tgui::to_string(outline.getBottom()));

        auto updateOutlineProperty = [=]{
            const tgui::Outline newOutline{
                tgui::AbsoluteOrRelativeValue{editLeft->getText()},
                tgui::AbsoluteOrRelativeValue{editTop->getText()},
                tgui::AbsoluteOrRelativeValue{editRight->getText()},
                tgui::AbsoluteOrRelativeValue{editBottom->getText()},
            };
            onChange(tgui::Serializer::serialize(newOutline));
        };
        editLeft->connect({"ReturnKeyPressed", "Unfocused"}, updateOutlineProperty);
        editTop->connect({"ReturnKeyPressed", "Unfocused"}, updateOutlineProperty);
        editRight->connect({"ReturnKeyPressed", "Unfocused"}, updateOutlineProperty);
        editBottom->connect({"ReturnKeyPressed", "Unfocused"}, updateOutlineProperty);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueMultilineString(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto multilineStringWindow = openWindowWithFocus();
        multilineStringWindow->setTitle("Set multiline text");
        multilineStringWindow->setSize(470, 220);
        multilineStringWindow->loadWidgetsFromFile("resources/forms/SetMultilineString.txt");

        auto textBox = multilineStringWindow->get<tgui::TextBox>("TextBox");
        textBox->setText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());
        textBox->connect("TextChanged", [=]{ onChange(tgui::Serializer::serialize(textBox->getText())); });
        textBox->setFocused(true);

        m_gui.setTabKeyUsageEnabled(false);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueStringList(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto setArrowColor = [](tgui::BitmapButton::Ptr button, sf::Color color){
        tgui::Texture texture = button->getImage();
        texture.setColor(color);
        button->setImage(texture);
    };

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto stringListWindow = openWindowWithFocus();
        stringListWindow->setTitle("Set string list");
        stringListWindow->setSize(352, 215);
        stringListWindow->loadWidgetsFromFile("resources/forms/SetStringList.txt");

        auto listBox = stringListWindow->get<tgui::ListBox>("ListBox");
        auto editBox = stringListWindow->get<tgui::EditBox>("EditBox");
        auto buttonAdd = stringListWindow->get<tgui::Button>("BtnAdd");
        auto buttonRemove = stringListWindow->get<tgui::Button>("BtnRemove");
        auto buttonArrowUp = stringListWindow->get<tgui::BitmapButton>("BtnArrowUp");
        auto buttonArrowDown = stringListWindow->get<tgui::BitmapButton>("BtnArrowDown");

        std::vector<sf::String> items = WidgetProperties::deserializeList(value);
        for (const auto& item : items)
            listBox->addItem(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, item).getString());

        setArrowColor(buttonArrowUp, buttonArrowUp->getSharedRenderer()->getTextColorDisabled());
        setArrowColor(buttonArrowDown, buttonArrowDown->getSharedRenderer()->getTextColorDisabled());
        buttonRemove->setEnabled(false);
        buttonArrowUp->setEnabled(false);
        buttonArrowDown->setEnabled(false);

        listBox->connect("ItemSelected", [=]{
            const int index = listBox->getSelectedItemIndex();
            if (index >= 0)
                buttonRemove->setEnabled(true);
            else
                buttonRemove->setEnabled(false);

            if (index > 0)
            {
                setArrowColor(buttonArrowUp, buttonArrowUp->getSharedRenderer()->getTextColor());
                buttonArrowUp->setEnabled(true);
            }
            else
            {
                setArrowColor(buttonArrowUp, buttonArrowUp->getSharedRenderer()->getTextColorDisabled());
                buttonArrowUp->setEnabled(false);
            }

            if ((index >= 0) && (static_cast<std::size_t>(index) + 1 < listBox->getItemCount()))
            {
                setArrowColor(buttonArrowDown, buttonArrowUp->getSharedRenderer()->getTextColor());
                buttonArrowDown->setEnabled(true);
            }
            else
            {
                setArrowColor(buttonArrowDown, buttonArrowUp->getSharedRenderer()->getTextColorDisabled());
                buttonArrowDown->setEnabled(false);
            }
        });

        auto updateValue = [=]{
            onChange(WidgetProperties::serializeList(listBox->getItems()));
        };

        buttonArrowUp->connect("Pressed", [=]{
            const std::size_t index = static_cast<std::size_t>(listBox->getSelectedItemIndex());
            sf::String value1 = listBox->getItemByIndex(index - 1);
            sf::String value2 = listBox->getItemByIndex(index);
            listBox->changeItemByIndex(index - 1, value2);
            listBox->changeItemByIndex(index, value1);
            listBox->setSelectedItemByIndex(index - 1);
            updateValue();
        });

        buttonArrowDown->connect("Pressed", [=]{
            const std::size_t index = static_cast<std::size_t>(listBox->getSelectedItemIndex());
            sf::String value1 = listBox->getItemByIndex(index);
            sf::String value2 = listBox->getItemByIndex(index + 1);
            listBox->changeItemByIndex(index, value2);
            listBox->changeItemByIndex(index + 1, value1);
            listBox->setSelectedItemByIndex(index + 1);
            updateValue();
        });

        buttonRemove->connect("Pressed", [=]{
            const std::size_t index = static_cast<std::size_t>(listBox->getSelectedItemIndex());
            listBox->removeItemByIndex(index);
            if (listBox->getItemCount() > 0)
            {
                if (index == listBox->getItemCount())
                    listBox->setSelectedItemByIndex(index - 1);
                else
                    listBox->setSelectedItemByIndex(index);
            }

            updateValue();
        });

        auto addItem = [=]{
            listBox->addItem(editBox->getText());
            listBox->setSelectedItemByIndex(listBox->getItemCount() - 1);
            editBox->setText("");
            editBox->setFocused(true);
            updateValue();
        };
        buttonAdd->connect("Pressed", addItem);
        editBox->connect("ReturnKeyPressed", addItem);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueTexture(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto textureWindow = openWindowWithFocus();
        textureWindow->setTitle("Set texture");
        textureWindow->setSize(235, 235);
        textureWindow->loadWidgetsFromFile("resources/forms/SetTexture.txt");

        auto previewCanvas = textureWindow->get<tgui::Canvas>("ImagePreview");
        auto buttonSelectFile = textureWindow->get<tgui::Button>("BtnSelectFile");
        auto editBoxPartRect = textureWindow->get<tgui::EditBox>("EditPartRect");
        auto editBoxMiddleRect = textureWindow->get<tgui::EditBox>("EditMiddleRect");

        auto deserializeRect = [=](std::string str) -> sf::IntRect {
            if (str.empty())
                return {};

            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            const std::vector<std::string> tokens = tgui::Deserializer::split(str, ',');
            if (tokens.size() == 4)
                return {tgui::stoi(tokens[0]), tgui::stoi(tokens[1]), tgui::stoi(tokens[2]), tgui::stoi(tokens[3])};
            else
                return {};
        };

        previewCanvas->setUserData(std::make_shared<tgui::Texture>());

        auto updateForm = [=](sf::String filename, sf::IntRect partRect, sf::IntRect middleRect, bool resetPartRect, bool resetMiddleRect, bool resetSize){
            auto texture = previewCanvas->getUserData<std::shared_ptr<tgui::Texture>>();
            if (resetSize)
            {
                try
                {
                    texture->load(filename, partRect, middleRect);
                    onChange(tgui::Serializer::serialize(*texture));
                }
                catch (tgui::Exception& e)
                {
                }

                buttonSelectFile->setUserData(filename); // Not using texture.getId() as it would be empty if file didn't exist
            }

            const sf::Vector2f imageSize = texture->getImageSize();
            const sf::Vector2f extraSpace{20, 135};
            const sf::Vector2f minSize{235, 140};
            const tgui::Layout2d maxSize{tgui::bindWidth(m_gui) - 50, tgui::bindHeight(m_gui) - 50};
            const tgui::Layout scaling = tgui::bindMin(1.f, tgui::bindMin((maxSize.x - extraSpace.x) / imageSize.x, (maxSize.y - extraSpace.y) / imageSize.y));
            if (resetSize)
            {
                previewCanvas->onSizeChange.setEnabled(false);
                previewCanvas->setSize({imageSize.x * scaling, imageSize.y * scaling});
                previewCanvas->onSizeChange.setEnabled(true);

                textureWindow->setSize({tgui::bindMax(minSize.x, (imageSize.x * scaling) + extraSpace.x), tgui::bindMax(minSize.y, (imageSize.y * scaling) + extraSpace.y)});
            }

            if (resetPartRect)
            {
                if (texture->getData() && (texture->getData()->rect == sf::IntRect{}))
                    partRect = {0, 0, static_cast<int>(imageSize.x), static_cast<int>(imageSize.y)};
                else
                    partRect = texture->getData() ? texture->getData()->rect : sf::IntRect{};

                editBoxPartRect->onTextChange.setEnabled(false);
                editBoxPartRect->setText("(" + tgui::to_string(partRect.left) + ", " + tgui::to_string(partRect.top)
                    + ", " + tgui::to_string(partRect.width) + ", " + tgui::to_string(partRect.height) + ")");
                editBoxPartRect->onTextChange.setEnabled(true);
            }

            if (resetMiddleRect)
            {
                middleRect = texture->getMiddleRect();

                editBoxMiddleRect->onTextChange.setEnabled(false);
                editBoxMiddleRect->setText("(" + tgui::to_string(middleRect.left) + ", " + tgui::to_string(middleRect.top)
                    + ", " + tgui::to_string(middleRect.width) + ", " + tgui::to_string(middleRect.height) + ")");
                editBoxMiddleRect->onTextChange.setEnabled(true);
            }

            tgui::Sprite sprite{*texture};
            sprite.setScale({scaling.getValue(), scaling.getValue()});

            previewCanvas->clear(sf::Color::Transparent);
            previewCanvas->draw(sprite);

            if ((middleRect != sf::IntRect{}) && (middleRect != sf::IntRect{0, 0, static_cast<int>(imageSize.x), static_cast<int>(imageSize.y)}))
            {
                std::vector<sf::Vertex> lines;
                if ((middleRect.left != 0) || (middleRect.width != static_cast<int>(imageSize.x)))
                {
                    lines.push_back({sf::Vector2f{middleRect.left + 0.5f, 0} * scaling.getValue(), sf::Color{255, 128, 255}});
                    lines.push_back({sf::Vector2f{middleRect.left + 0.5f, imageSize.y} * scaling.getValue(), sf::Color{255, 128, 255}});

                    lines.push_back({sf::Vector2f{middleRect.left + middleRect.width - 0.5f, 0} * scaling.getValue(), sf::Color{255, 128, 255}});
                    lines.push_back({sf::Vector2f{middleRect.left + middleRect.width - 0.5f, imageSize.y} * scaling.getValue(), sf::Color{255, 128, 255}});
                }

                if ((middleRect.top != 0) || (middleRect.height != static_cast<int>(imageSize.y)))
                {
                    lines.push_back({sf::Vector2f{0, middleRect.top + 0.5f} * scaling.getValue(), sf::Color{255, 128, 255}});
                    lines.push_back({sf::Vector2f{imageSize.x, middleRect.top + 0.5f} * scaling.getValue(), sf::Color{255, 128, 255}});

                    lines.push_back({sf::Vector2f{0, middleRect.top + middleRect.height - 0.5f} * scaling.getValue(), sf::Color{255, 128, 255}});
                    lines.push_back({sf::Vector2f{imageSize.x, middleRect.top + middleRect.height - 0.5f} * scaling.getValue(), sf::Color{255, 128, 255}});
                }

                if (!lines.empty())
                    previewCanvas->draw(lines.data(), lines.size(), sf::Lines);
            }

            previewCanvas->display();
        };

        editBoxPartRect->connect("TextChanged", [=]{
            updateForm(buttonSelectFile->getUserData<sf::String>(), deserializeRect(editBoxPartRect->getText()), {}, false, true, true);
        });
        editBoxMiddleRect->connect("TextChanged", [=]{
            updateForm(buttonSelectFile->getUserData<sf::String>(), deserializeRect(editBoxPartRect->getText()), deserializeRect(editBoxMiddleRect->getText()), false, false, true);
        });
        previewCanvas->connect("SizeChanged", [=]{
            updateForm(buttonSelectFile->getUserData<sf::String>(), deserializeRect(editBoxPartRect->getText()), deserializeRect(editBoxMiddleRect->getText()), false, false, false);
        });

        tgui::Texture originalTexture;
        try
        {
            originalTexture = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Texture, value).getTexture();
        }
        catch (tgui::Exception& e)
        {
            std::cout << "Exception caught when loading image: " << e.what() << std::endl;
        }

        sf::String originalFilename = originalTexture.getId();
        sf::IntRect originalPartRect = originalTexture.getData() ? originalTexture.getData()->rect : sf::IntRect{};
        sf::IntRect originalMiddleRect = originalTexture.getMiddleRect();
        updateForm(originalFilename, originalPartRect, originalMiddleRect, true, true, true);

        buttonSelectFile->connect("pressed", [=]{
            auto filenameWindow = openWindowWithFocus();
            filenameWindow->setTitle("Load image");
            filenameWindow->setSize(400, 100);
            filenameWindow->loadWidgetsFromFile("resources/forms/LoadFile.txt");

            auto editBoxFilename = filenameWindow->get<tgui::EditBox>("EditFilename");
            auto buttonLoadImage = filenameWindow->get<tgui::Button>("BtnLoad");
            auto buttonCancelLoadingImage = filenameWindow->get<tgui::Button>("BtnCancel");

            editBoxFilename->setText(buttonSelectFile->getUserData<sf::String>());
            editBoxFilename->setFocused(true);

            buttonCancelLoadingImage->connect("pressed", [=]{ filenameWindow->close(); });
            buttonLoadImage->connect("pressed", [=]{
                updateForm(editBoxFilename->getText(), {}, {}, true, true, true);
                filenameWindow->close();
            });
            editBoxFilename->connect("ReturnKeyPressed", [=]{
                updateForm(editBoxFilename->getText(), {}, {}, true, true, true);
                filenameWindow->close();
            });
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueEditBoxInputValidator(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto outlineWindow = openWindowWithFocus();
        outlineWindow->setTitle("Set accepted input");
        outlineWindow->setSize(190, 215);
        outlineWindow->loadWidgetsFromFile("resources/forms/SetEditBoxInputValidator.txt");

        auto checkAny = outlineWindow->get<tgui::RadioButton>("CheckAny");
        auto checkInt = outlineWindow->get<tgui::RadioButton>("CheckInt");
        auto checkUInt = outlineWindow->get<tgui::RadioButton>("CheckUInt");
        auto checkFloat = outlineWindow->get<tgui::RadioButton>("CheckFloat");
        auto checkCustom = outlineWindow->get<tgui::RadioButton>("CheckCustom");
        auto editValidator = outlineWindow->get<tgui::EditBox>("EditValidator");

        if (value == tgui::EditBox::Validator::All)
            checkAny->setChecked(true);
        else if (value == tgui::EditBox::Validator::Int)
            checkInt->setChecked(true);
        else if (value == tgui::EditBox::Validator::UInt)
            checkUInt->setChecked(true);
        else if (value == tgui::EditBox::Validator::Float)
            checkFloat->setChecked(true);
        else
            checkCustom->setChecked(true);

        editValidator->setText(value);

        auto updateCustomValidator = [=]{
            checkCustom->setChecked(true);
            onChange(editValidator->getText());
        };
        auto updateValidator = [=](const sf::String& newValue){
            editValidator->disconnectAll("ReturnKeyPressed");
            editValidator->disconnectAll("Unfocused");
            editValidator->setText(newValue);
            editValidator->connect({"ReturnKeyPressed", "Unfocused"}, updateCustomValidator);
            onChange(newValue);
        };
        checkAny->connect("Checked", [=]{ updateValidator(tgui::EditBox::Validator::All); });
        checkInt->connect("Checked", [=]{ updateValidator(tgui::EditBox::Validator::Int); });
        checkUInt->connect("Checked", [=]{ updateValidator(tgui::EditBox::Validator::UInt); });
        checkFloat->connect("Checked", [=]{ updateValidator(tgui::EditBox::Validator::Float); });
        editValidator->connect({"ReturnKeyPressed", "Unfocused"}, updateCustomValidator);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueChildWindowTitleButtons(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->connect("pressed", [=]{
        auto titleButtonWindow = openWindowWithFocus();
        titleButtonWindow->setTitle("Set title buttons");
        titleButtonWindow->setSize(125, 125);
        titleButtonWindow->loadWidgetsFromFile("resources/forms/SetChildWindowTitleButtons.txt");

        auto checkClose = titleButtonWindow->get<tgui::RadioButton>("CheckBoxClose");
        auto checkMaximize = titleButtonWindow->get<tgui::RadioButton>("CheckBoxMaximize");
        auto checkMinimize = titleButtonWindow->get<tgui::RadioButton>("CheckBoxMinimize");

        for (const auto& elem : tgui::Deserializer::split(value, '|'))
        {
            std::string titleButtonStr = tgui::toLower(tgui::trim(elem));
            if (titleButtonStr == "close")
                checkClose->setChecked(true);
            else if (titleButtonStr == "maximize")
                checkMaximize->setChecked(true);
            else if (titleButtonStr == "minimize")
                checkMinimize->setChecked(true);
        }

        auto updateTitleButtons = [=]{
            std::string newValue;
            if (checkMinimize->isChecked())
                newValue += " | Minimize";
            if (checkMaximize->isChecked())
                newValue += " | Maximize";
            if (checkClose->isChecked())
                newValue += " | Close";

            if (!newValue.empty())
                newValue.erase(0, 3);
            else
                newValue = "None";

            onChange(newValue);
        };
        checkClose->connect("Changed", updateTitleButtons);
        checkMaximize->connect("Changed", updateTitleButtons);
        checkMinimize->connect("Changed", updateTitleButtons);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueEnum(const std::string& property, const sf::String& value, const OnValueChangeFunc& onChange, float topPosition, const std::vector<std::string>& enumValues)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto valueComboBox = m_propertiesContainer->get<tgui::ComboBox>("ValueComboBox" + property);
    if (!valueComboBox)
    {
        valueComboBox = tgui::ComboBox::create();
        valueComboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Automatic);
        valueComboBox->setChangeItemOnScroll(false);
        m_propertiesContainer->add(valueComboBox, "ValueComboBox" + property);

        for (const auto& enumValue : enumValues)
            valueComboBox->addItem(enumValue);
    }

    valueComboBox->disconnectAll("ItemSelected");
    valueComboBox->setPosition({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, topPosition});
    valueComboBox->setSize({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, EDIT_BOX_HEIGHT});

    std::string valueLower = tgui::toLower(value);
    for (unsigned int i = 0; i < enumValues.size(); ++i)
    {
        if (tgui::toLower(enumValues[i]) == valueLower)
            valueComboBox->setSelectedItemByIndex(i);
    }

    valueComboBox->connect("ItemSelected", [=]{ onChange(valueComboBox->getSelectedItem()); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackNewOrLoadFile()
{
    while (!m_forms.empty())
        closeForm(m_forms[0].get());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackSaveFile()
{
    m_selectedForm->save();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackQuit()
{
    while (!m_forms.empty())
        closeForm(m_forms[0].get());

    m_window.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackEditThemes()
{
    auto editThemesWindow = openWindowWithFocus();
    editThemesWindow->setTitle("Edit themes");
    editThemesWindow->setSize({320, 280});
    editThemesWindow->loadWidgetsFromFile("resources/forms/EditThemes.txt");

    auto buttonAdd = editThemesWindow->get<tgui::Button>("ButtonAdd");
    auto buttonDelete = editThemesWindow->get<tgui::Button>("ButtonDelete");
    auto newThemeEditBox = editThemesWindow->get<tgui::EditBox>("NewThemeEditBox");
    auto themesList = editThemesWindow->get<tgui::ListBox>("ThemesList");

    themesList->removeAllItems();
    for (auto& theme : m_themes)
    {
        if (!theme.second.getPrimary().empty())
            themesList->addItem(theme.second.getPrimary());
    }

    themesList->connect("ItemSelected", [=](std::string item){
        if (item.empty())
            buttonDelete->setEnabled(false);
        else
            buttonDelete->setEnabled(true);
    });

    newThemeEditBox->connect("TextChanged", [=](std::string text){
        if (text.empty())
            buttonAdd->setEnabled(false);
        else
            buttonAdd->setEnabled(true);
    });

    buttonAdd->connect("Pressed", [=]{
        try
        {
            const std::string filename = newThemeEditBox->getText();
            if (!themesList->contains(filename))
            {
                tgui::Theme theme{filename};
                themesList->addItem(filename);
                m_themes[filename] = theme;
            }
        }
        catch (const tgui::Exception& e)
        {
            std::cout << "Exception caught when adding theme: " << e.what() << std::endl;
        }

        initProperties();
    });

    buttonDelete->connect("Pressed", [=]{
        auto item = themesList->getSelectedItem();
        m_themes.erase(item);
        themesList->removeItem(item);
        buttonDelete->setEnabled(false);
        initProperties();
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackBringWidgetToFront()
{
    m_selectedForm->getSelectedWidget()->ptr->moveToFront();
    m_selectedForm->setChanged(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackSendWidgetToBack()
{
    m_selectedForm->getSelectedWidget()->ptr->moveToBack();
    m_selectedForm->setChanged(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackCutWidget()
{
    copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
    removeSelectedWidget();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackCopyWidget()
{
    copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackPasteWidget()
{
    pasteWidgetFromInternalClipboard();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackDeleteWidget()
{
    removeSelectedWidget();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackKeyboardShortcuts()
{
    auto keyboardShortcutsWindow = openWindowWithFocus();
    keyboardShortcutsWindow->setTitle("Keyboard shortcuts");
    keyboardShortcutsWindow->setSize(375, 310);
    keyboardShortcutsWindow->loadWidgetsFromFile("resources/forms/KeyboardShortcuts.txt");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackAbout()
{
    auto aboutWindow = openWindowWithFocus();
    aboutWindow->setTitle("About");
    aboutWindow->setSize(365, 130);
    aboutWindow->loadWidgetsFromFile("resources/forms/About.txt");

    auto labelVersion = aboutWindow->get<tgui::Label>("LabelVersion");
    labelVersion->setText(tgui::to_string(TGUI_VERSION_MAJOR) + "." + tgui::to_string(TGUI_VERSION_MINOR) + "." + tgui::to_string(TGUI_VERSION_PATCH));
}