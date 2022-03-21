/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include "WidgetProperties/ChatBoxProperties.hpp"
#include "WidgetProperties/ChildWindowProperties.hpp"
#include "WidgetProperties/ComboBoxProperties.hpp"
#include "WidgetProperties/EditBoxProperties.hpp"
#include "WidgetProperties/GroupProperties.hpp"
#include "WidgetProperties/KnobProperties.hpp"
#include "WidgetProperties/LabelProperties.hpp"
#include "WidgetProperties/ListBoxProperties.hpp"
#include "WidgetProperties/ListViewProperties.hpp"
#include "WidgetProperties/PanelProperties.hpp"
#include "WidgetProperties/PictureProperties.hpp"
#include "WidgetProperties/ProgressBarProperties.hpp"
#include "WidgetProperties/RadioButtonProperties.hpp"
#include "WidgetProperties/RangeSliderProperties.hpp"
#include "WidgetProperties/ScrollablePanelProperties.hpp"
#include "WidgetProperties/ScrollbarProperties.hpp"
#include "WidgetProperties/SliderProperties.hpp"
#include "WidgetProperties/SpinButtonProperties.hpp"
#include "WidgetProperties/SpinControlProperties.hpp"
#include "WidgetProperties/TabsProperties.hpp"
#include "WidgetProperties/TextAreaProperties.hpp"
#include "WidgetProperties/TreeViewProperties.hpp"
#include "GuiBuilder.hpp"

#include <fstream>
#include <cassert>
#include <memory>
#include <string>
#include <thread> // this_thread::sleep_for
#include <cctype> // isdigit
#include <cmath> // max
#include <stack>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const float EDIT_BOX_HEIGHT = 24;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    bool compareRenderers(std::map<tgui::String, tgui::ObjectConverter> themePropertyValuePairs, std::map<tgui::String, tgui::ObjectConverter> widgetPropertyValuePairs)
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

                    const tgui::String absoluteFilename1 = (tgui::getResourcePath() / tgui::Filesystem::Path(widgetPropertyValuePairs[themeIt->first].getTexture().getId())).asString();
                    const tgui::String absoluteFilename2 = (tgui::getResourcePath() / tgui::Filesystem::Path(themeIt->second.getTexture().getId())).asString();
                    if ((absoluteFilename1 == absoluteFilename2)
                     && (widgetPropertyValuePairs[themeIt->first].getTexture().getMiddleRect() == themeIt->second.getTexture().getMiddleRect()))
                    {
                        continue;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Vector2f parseSize(tgui::String str)
    {
        if (str.empty())
            return {800, 600};

        // Remove the brackets around the value
        if ((str.front() == '(') && (str.back() == ')'))
            str = str.substr(1, str.length() - 2);

        if (str.empty())
            return {800, 600};

        const auto commaPos = str.find(',');
        if (commaPos == tgui::String::npos)
            return {800, 600};

        return {str.substr(0, commaPos).trim().toFloat(), str.substr(commaPos + 1).trim().toFloat()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Filesystem::Path getDataDirectory(bool createIfNotFound)
    {
        const auto localDataDir = tgui::Filesystem::getLocalDataDirectory();

        tgui::Filesystem::Path guiBuilderDataDir;
        if (!localDataDir.isEmpty() && tgui::Filesystem::directoryExists(localDataDir))
        {
            tgui::String versionNumber = tgui::String::fromNumber(TGUI_VERSION_MAJOR) + U"." + tgui::String::fromNumber(TGUI_VERSION_MINOR);
            const auto destDir = localDataDir / U"tgui" / versionNumber / U"gui-builder";
            if (tgui::Filesystem::directoryExists(destDir))
                guiBuilderDataDir = destDir;
            else if (createIfNotFound)
            {
                // Create the direcory structure if it doesn't exist yet
                tgui::Filesystem::createDirectory(localDataDir / U"tgui");
                tgui::Filesystem::createDirectory(localDataDir / U"tgui" / versionNumber);
                if (tgui::Filesystem::createDirectory(localDataDir / U"tgui" / versionNumber / U"gui-builder"))
                    guiBuilderDataDir = destDir;
            }
        }

        return guiBuilderDataDir;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiBuilder::GuiBuilder(const tgui::String& programName) :
    m_window      {tgui::DefaultBackendWindow::create(1300, 680, "TGUI - GUI Builder")},
    m_gui         {m_window->getGui()},
    m_themes      {{"White", *tgui::Theme::getDefault()}},
    m_defaultTheme{"White"},
    m_programPath {tgui::Filesystem::Path(programName).getParentPath()}
{
    // If the program is started from a different folder then it wouldn't be able to find its resources unless we set this path.
    // One case where this seems to be required is to start the executable on macOS by double-clicking it.
    tgui::setResourcePath((tgui::Filesystem::getCurrentWorkingDirectory() / m_programPath).getNormalForm());

    m_widgetProperties["BitmapButton"] = std::make_unique<BitmapButtonProperties>();
    m_widgetProperties["Button"] = std::make_unique<ButtonProperties>();
    m_widgetProperties["ChatBox"] = std::make_unique<ChatBoxProperties>();
    m_widgetProperties["CheckBox"] = std::make_unique<RadioButtonProperties>();
    m_widgetProperties["ChildWindow"] = std::make_unique<ChildWindowProperties>();
    m_widgetProperties["ComboBox"] = std::make_unique<ComboBoxProperties>();
    m_widgetProperties["EditBox"] = std::make_unique<EditBoxProperties>();
    m_widgetProperties["Group"] = std::make_unique<GroupProperties>();
    m_widgetProperties["Knob"] = std::make_unique<KnobProperties>();
    m_widgetProperties["Label"] = std::make_unique<LabelProperties>();
    m_widgetProperties["ListBox"] = std::make_unique<ListBoxProperties>();
    m_widgetProperties["ListView"] = std::make_unique<ListViewProperties>();
    m_widgetProperties["Panel"] = std::make_unique<PanelProperties>();
    m_widgetProperties["Picture"] = std::make_unique<PictureProperties>();
    m_widgetProperties["ProgressBar"] = std::make_unique<ProgressBarProperties>();
    m_widgetProperties["RadioButton"] = std::make_unique<RadioButtonProperties>();
    m_widgetProperties["RangeSlider"] = std::make_unique<RangeSliderProperties>();
    m_widgetProperties["ScrollablePanel"] = std::make_unique<ScrollablePanelProperties>();
    m_widgetProperties["Scrollbar"] = std::make_unique<ScrollbarProperties>();
    m_widgetProperties["Slider"] = std::make_unique<SliderProperties>();
    m_widgetProperties["SpinButton"] = std::make_unique<SpinButtonProperties>();
    m_widgetProperties["SpinControl"] = std::make_unique<SpinControlProperties>();
    m_widgetProperties["Tabs"] = std::make_unique<TabsProperties>();
    m_widgetProperties["TextArea"] = std::make_unique<TextAreaProperties>();
    m_widgetProperties["TreeView"] = std::make_unique<TreeViewProperties>();

    m_window->setIcon((tgui::getResourcePath() / "resources/Icon.png").asString());

    loadStartScreen();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiBuilder::~GuiBuilder()
{
    saveGuiBuilderState();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::mainLoop()
{
    while (m_window->isOpen())
    {
        tgui::Event event;
        while (m_window->pollEvent(event))
        {
            if (event.type == tgui::Event::Type::Closed)
            {
                while (!m_forms.empty())
                    closeForm(m_forms[0].get());

                m_window->close();
            }
            else if (event.type == tgui::Event::Type::MouseButtonReleased)
            {
                if (m_selectedForm && !m_foregroundPanel)
                {
                    if (event.mouseButton.button == tgui::Event::MouseButton::Left)
                        m_selectedForm->mouseReleased();
                    else if (event.mouseButton.button == tgui::Event::MouseButton::Right)
                    {
                        if (m_popupMenu)
                        {
                            removePopupMenu();
                        }
                        else if (m_selectedForm->rightMouseClick({event.mouseButton.x, event.mouseButton.y}))
                        {
                            auto panel = tgui::Panel::create({"100%", "100%"});
                            panel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
                            m_gui->add(panel);

                            m_popupMenu = tgui::ListBox::create();
                            panel->add(m_popupMenu);
                            if (m_selectedForm->getSelectedWidget())
                            {
                                m_popupMenu->addItem("Bring to front");
                                m_popupMenu->addItem("Send to back");
                                m_popupMenu->addItem("Cut");
                                m_popupMenu->addItem("Copy");
                            }
                            if (!m_copiedWidgets.empty())
                                m_popupMenu->addItem("Paste");
                            if (m_selectedForm->getSelectedWidget())
                                m_popupMenu->addItem("Delete");

                            if (m_popupMenu->getItemCount() > 0)
                            {
                                const tgui::Outline outline = m_popupMenu->getSharedRenderer()->getPadding() + m_popupMenu->getSharedRenderer()->getBorders();
                                m_popupMenu->setPosition({static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
                                m_popupMenu->setSize({150, (m_popupMenu->getItemHeight() * m_popupMenu->getItemCount()) + outline.getTop() + outline.getBottom()});

                                panel->onClick([this]{ removePopupMenu(); });
                                m_popupMenu->onMouseRelease([this](const tgui::String& item){
                                    if (item == "Bring to front")
                                        menuBarCallbackBringWidgetToFront();
                                    else if (item == "Send to back")
                                        menuBarCallbackSendWidgetToBack();
                                    else if (item == "Cut")
                                        menuBarCallbackCutWidget();
                                    else if (item == "Copy")
                                        menuBarCallbackCopyWidget();
                                    else if (item == "Paste")
                                        menuBarCallbackPasteWidget();
                                    else if (item == "Delete")
                                        menuBarCallbackDeleteWidget();
                                });
                            }
                            else // The popup menu is empty
                                removePopupMenu();
                        }
                    }
                }
            }
            else if (event.type == tgui::Event::Type::MouseMoved)
            {
                if (m_selectedForm)
                    m_selectedForm->mouseMoved({event.mouseMove.x, event.mouseMove.y});
            }
            else if (event.type == tgui::Event::Type::KeyPressed)
            {
#ifdef TGUI_SYSTEM_MACOS
                const auto controlPressed = event.key.system;
#else
                const auto controlPressed = event.key.control;
#endif
                if ((event.key.code == tgui::Event::KeyboardKey::Left) || (event.key.code == tgui::Event::KeyboardKey::Right)
                 || (event.key.code == tgui::Event::KeyboardKey::Up) || (event.key.code == tgui::Event::KeyboardKey::Down))
                {
                    if (m_selectedForm && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                        m_selectedForm->arrowKeyPressed(event.key);
                }
                else if (event.key.code == tgui::Event::KeyboardKey::Delete)
                {
                    if (m_selectedForm && m_selectedForm->getSelectedWidget() && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                        removeSelectedWidget();
                }
                else if (event.key.code == tgui::Event::KeyboardKey::Escape)
                {
                    if (m_selectedForm && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                        m_selectedForm->selectParent();
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::S) && controlPressed)
                {
                    if (m_selectedForm)
                        m_selectedForm->save();
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::C) && controlPressed)
                {
                    if (m_selectedForm && m_selectedForm->getSelectedWidget() && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                        copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::V) && controlPressed)
                {
                    if (m_selectedForm && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                        pasteWidgetFromInternalClipboard();
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::X) && controlPressed)
                {
                    if (m_selectedForm && m_selectedForm->getSelectedWidget() && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                    {
                        copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
                        removeSelectedWidget();
                    }
                }
            }

            m_gui->handleEvent(event);
        }

        if (m_selectedForm && m_selectedForm->hasFocus())
            m_selectedForm->updateAlignmentLines();

        m_window->draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiBuilder::loadGuiBuilderState()
{
    m_recentFiles.clear();

    std::size_t fileSize = 0;
    std::unique_ptr<std::uint8_t[]> fileContents = nullptr;
    tgui::Filesystem::Path dataDir = getDataDirectory(false);
    if (!dataDir.isEmpty())
    {
        // Try to open the files in the local user data folder
        fileContents = tgui::readFileToMemory((dataDir / "GuiBuilderState.txt").asString(), fileSize);
    }

    // If we failed to read from the user directory then try reading from the current working directory
    if (!fileContents)
        fileContents = tgui::readFileToMemory("GuiBuilderState.txt", fileSize);

    if (!fileContents)
        return false;

    /// TODO: Optimize this (parse function should be able to use a string view directly on file contents)
    std::stringstream stream{std::string{reinterpret_cast<const char*>(fileContents.get()), fileSize}};
    const auto node = tgui::DataIO::parse(stream);

    if (node->propertyValuePairs["RecentFiles"])
    {
        if (node->propertyValuePairs["FormSize"])
        {
            const auto& size = node->propertyValuePairs["FormSize"]->value;
            m_formSize = parseSize(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, size).getString());
        }

        for (const auto& value : node->propertyValuePairs["RecentFiles"]->valueList)
        {
            tgui::String filename = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString();
            if (tgui::Filesystem::fileExists(tgui::getResourcePath() / filename))
            {
                m_recentFiles.push_back(filename);
                if (m_recentFiles.size() == 5)
                    break;
            }
        }
    }

    if (node->propertyValuePairs["Themes"])
    {
        for (const auto& theme : node->propertyValuePairs["Themes"]->valueList)
        {
            const auto deserializedTheme = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, theme).getString();
            try
            {
                m_themes[deserializedTheme] = {deserializedTheme};
            }
            catch (const tgui::Exception& e)
            {
                displayErrorMessage(U"Failed to import theme '" + deserializedTheme + U"', reason: " + e.what());
            }
        }
    }

    if (node->propertyValuePairs["DefaultPath"])
    {
        m_defaultPath = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, node->propertyValuePairs["DefaultPath"]->value).getString();
        if (!m_defaultPath.empty() && (m_defaultPath[m_defaultPath.length()-1] != '/') && (m_defaultPath[m_defaultPath.length()-1] != '\\'))
            m_defaultPath += '/';
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::saveGuiBuilderState()
{
    auto node = std::make_unique<tgui::DataIO::Node>();

    // Save the list of recent opened forms
    tgui::String recentFileList;
    for (auto fileIt = m_recentFiles.begin(); fileIt != m_recentFiles.end(); ++fileIt)
    {
        if (!tgui::Filesystem::fileExists(tgui::getResourcePath() / tgui::String(*fileIt)))
            continue;

        if (recentFileList.empty())
            recentFileList = "[" + tgui::Serializer::serialize(*fileIt);
        else
            recentFileList += ", " + tgui::Serializer::serialize(*fileIt);
    }
    if (!recentFileList.empty())
    {
        recentFileList += "]";
        node->propertyValuePairs["RecentFiles"] = std::make_unique<tgui::DataIO::ValueNode>(recentFileList);
    }

    // Save the list of themes
    if (m_themes.size() > 1)
    {
        auto themeIt = m_themes.begin();
        if (themeIt->first == "White")
            themeIt++;

        tgui::String themeList = "[" + tgui::Serializer::serialize(themeIt->first);
        for (++themeIt; themeIt != m_themes.end(); ++themeIt)
        {
            if (themeIt->first != "White")
                themeList += ", " + tgui::Serializer::serialize(themeIt->first);
        }

        themeList += "]";
        node->propertyValuePairs["Themes"] = std::make_unique<tgui::DataIO::ValueNode>(themeList);
    }

    node->propertyValuePairs["FormSize"] = std::make_unique<tgui::DataIO::ValueNode>(
        "(" + tgui::String::fromNumber(m_formSize.x) + ", " + tgui::String::fromNumber(m_formSize.y) + ")");

    node->propertyValuePairs["DefaultPath"] = std::make_unique<tgui::DataIO::ValueNode>(tgui::Serializer::serialize(m_defaultPath));

    std::stringstream stream;
    tgui::DataIO::emit(node, stream);

    bool fileWrittenToDataDir = false;
    const tgui::Filesystem::Path dataDir = getDataDirectory(true);
    if (!dataDir.isEmpty())
    {
        // Try to write the file to the local user data folder
        if (tgui::writeFile((dataDir / "GuiBuilderState.txt").asString(), stream))
            fileWrittenToDataDir = true;
    }

    // If we failed to write to the user directory then try writing to the current working directory
    if (!fileWrittenToDataDir)
    {
        if (!tgui::writeFile("GuiBuilderState.txt", stream))
            displayErrorMessage("Failed to open GuiBuilderState.txt for writing");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::formSaved(const tgui::String& filename)
{
    // Update the default path
    const auto slashPos = filename.find_last_of(L"/\\");
    if (slashPos != tgui::String::npos)
        m_defaultPath = filename.substr(0, slashPos+1);
    else
        m_defaultPath = "";

    // Update the recent files
    if (m_recentFiles.empty() || (m_recentFiles.front() != filename))
    {
        m_recentFiles.erase(std::remove_if(m_recentFiles.begin(), m_recentFiles.end(), [filename](const tgui::String& recentFile){ return filename == recentFile; }), m_recentFiles.end());
        m_recentFiles.insert(m_recentFiles.begin(), filename);
    }

    saveGuiBuilderState();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::reloadProperties()
{
    auto selectedWidget = m_selectedForm->getSelectedWidget();
    float topPosition = 0;

    if (selectedWidget)
    {
        // If the widget isn't supported by the Gui Builder (i.e. it was manually placed in the form file), then don't try to create properties
        if (m_widgetProperties.find(selectedWidget->ptr->getWidgetType()) == m_widgetProperties.end())
        {
            std::cerr << "Warning: widget of type '" << selectedWidget->ptr->getWidgetType() << "' can't be edited." << std::endl;
            return;
        }

        addPropertyValueWidgets(topPosition, {"Name", {"String", selectedWidget->name}},
            TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& value){
                if (selectedWidget->name != value)
                    changeWidgetName(value);
            });

        topPosition += 10;
        m_propertyValuePairs = m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->initProperties(selectedWidget->ptr);
        for (const auto& property : m_propertyValuePairs.first)
        {
            addPropertyValueWidgets(topPosition, property,
                TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& value){
                    if (updateWidgetProperty(property.first, value))
                        m_selectedForm->setChanged(true);
                });
        }

        topPosition += 10;
        auto rendererComboBox = m_propertiesContainer->get<tgui::ComboBox>("RendererSelectorComboBox");
        rendererComboBox->setPosition({0, topPosition});

        if (static_cast<std::size_t>(rendererComboBox->getSelectedItemIndex()) + 1 == rendererComboBox->getItemCount()) // If "Custom" is selected
        {
            topPosition += rendererComboBox->getSize().y + 10;
            for (const auto& property : m_propertyValuePairs.second)
            {
                addPropertyValueWidgets(topPosition, property,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& value){
                        if (updateWidgetProperty(property.first, value))
                        {
                            m_selectedForm->setChanged(true);

                            // The value shouldn't always be exactly as typed. An empty string may be understood correctly when setting the property,
                            // but is can't be saved to a widget file properly. So we read the back the property to have a valid string and pass it
                            // back to the widget, so that the string stored in the renderer is always a valid string.
                            m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->updateProperty(selectedWidget->ptr, property.first, m_propertyValuePairs.second[property.first].second);
                        }
                    });
            }

            rendererComboBox->moveToFront();
        }
    }
    else // The form itself was selected
    {
        addPropertyValueWidgets(topPosition, {"Filename", {"String", m_selectedForm->getFilename()}},
            TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& value){
                if (m_selectedForm->getFilename() != value)
                {
                    m_selectedForm->setChanged(true);
                    m_selectedForm->setFilename(value);
                    m_selectedWidgetComboBox->changeItemById("form", value);
                }
            });

        addPropertyValueWidgets(topPosition, {"Width", {"UInt", tgui::String::fromNumber(m_selectedForm->getSize().x)}},
            TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& value){
                if (tgui::String::fromNumber(m_selectedForm->getSize().x) != value)
                {
                    // Form is not marked as changed since the width is saved as editor property
                    const float newWidth = value.toFloat();
                    m_formSize = { newWidth, m_selectedForm->getSize().y };
                    m_selectedForm->setSize(m_formSize);
                }
            });

        addPropertyValueWidgets(topPosition, {"Height", {"UInt", tgui::String::fromNumber(m_selectedForm->getSize().y)}},
            TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& value){
                if (tgui::String::fromNumber(m_selectedForm->getSize().y) != value)
                {
                    // Form is not marked as changed since the height is saved as editor property
                    const float newHeight = value.toFloat();
                    m_formSize = { m_selectedForm->getSize().x, newHeight};
                    m_selectedForm->setSize(m_formSize);
                }
            });
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::widgetSelected(tgui::Widget::Ptr widget)
{
    initProperties();

    if (widget)
        m_selectedWidgetComboBox->setSelectedItemById(tgui::String::fromNumber(widget.get()));
    else
        m_selectedWidgetComboBox->setSelectedItemById("form");

    updateSelectedWidgetHierarchy();
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
    m_gui->add(panel);

    auto messageBox = tgui::MessageBox::create("Saving form", "The form was changed, do you want to save the changes?", {"Yes", "No"});
    messageBox->setPosition("(&.size - size) / 2");
    m_gui->add(messageBox);

    bool haltProgram = true;
#if __cplusplus > 201703L
    messageBox->onButtonPress([=,this,&haltProgram](const tgui::String& button){
#else
    messageBox->onButtonPress([=,&haltProgram](const tgui::String& button){
#endif
        if (button == "Yes")
            m_selectedForm->save();

        m_gui->remove(panel);
        m_gui->remove(messageBox);

        if (m_selectedForm == form)
            m_selectedForm = nullptr;

        m_forms.erase(std::find_if(m_forms.begin(), m_forms.end(), [form](const auto& f){ return f.get() == form; }));
        if (m_forms.empty())
            loadStartScreen();

        haltProgram = false;
    });

    // The closeForm function has to halt the execution of the normal main loop (to be able to prevent closing the window)
    while (haltProgram && m_window->isOpen())
    {
        tgui::Event event;
        while (m_window->pollEvent(event))
        {
            if (event.type == tgui::Event::Type::Closed)
            {
                // Attempting to close the window, while already having asked whether the form should be saved, will result in the close without saving
                m_window->close();
                m_forms.clear();
            }

            m_gui->handleEvent(event);
        }

        m_window->draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::showLoadFileWindow(const tgui::String& title, const tgui::String& loadButtonCaption, bool fileMustExist, const tgui::String& defaultFilename, const std::function<void(const tgui::String&)>& onLoad)
{
    auto fileDialog = tgui::FileDialog::create(title, loadButtonCaption);
    fileDialog->setFileMustExist(fileMustExist);
    if (m_forms.empty())
        fileDialog->setPath((tgui::getResourcePath() / m_defaultPath).getNormalForm());
    else
        fileDialog->setPath((tgui::getResourcePath() / m_forms[0]->getFilename()).getParentPath().getNormalForm());

    fileDialog->setFilename(defaultFilename);
    openWindowWithFocus(fileDialog);

    fileDialog->onFileSelect([onLoad](const std::vector<tgui::Filesystem::Path>& selectedFiles){
        if (!selectedFiles.empty())
            onLoad(selectedFiles[0].asString());
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadStartScreen()
{
    while (!m_forms.empty())
        closeForm(m_forms[0].get());

    m_propertiesWindow = nullptr;
    m_propertiesContainer = nullptr;
    m_selectedWidgetComboBox = nullptr;

    m_gui->removeAllWidgets();
    m_gui->loadWidgetsFromFile("resources/forms/StartScreen.txt");

    if (!loadGuiBuilderState())
    {
        m_themes["themes/Black.txt"] = {(tgui::getResourcePath() / "themes/Black.txt").asString()};
        m_themes["themes/BabyBlue.txt"] = {(tgui::getResourcePath() / "themes/BabyBlue.txt").asString()};
        m_themes["themes/TransparentGrey.txt"] = {(tgui::getResourcePath() / "themes/TransparentGrey.txt").asString()};
    }

    auto panel = m_gui->get<tgui::Panel>("MainPanel");
    panel->get<tgui::Panel>("PnlNewForm")->onClick([this]{
        showLoadFileWindow("New form", "Create", false, getDefaultFilename(), [this](const tgui::String& filename){
            createNewForm(filename);
        });
    });
    panel->get<tgui::Panel>("PnlLoadForm")->onClick([this]{
        showLoadFileWindow("Load form", "Load", true, getDefaultFilename(), [this](const tgui::String& filename){ loadForm(filename); });
    });

    if (m_recentFiles.empty())
        panel->get("LblNoRecentFiles")->setVisible(true);
    else
    {
        for (unsigned int i = 0; (i < 5) && (i < m_recentFiles.size()); ++i)
        {
            auto labelRecentForm = panel->get<tgui::Label>("LblRecentForm" + tgui::String::fromNumber(i+1));
            labelRecentForm->setText(m_recentFiles[i]);
            labelRecentForm->setVisible(true);
            labelRecentForm->onClick([this,filename=m_recentFiles[i]]{ loadForm(filename); });

            auto buttonRemoveFormFromList = panel->get<tgui::ClickableWidget>("BtnDeleteRecentForm" + tgui::String::fromNumber(i+1));
            buttonRemoveFormFromList->setVisible(true);
            buttonRemoveFormFromList->onClick([this,filename=m_recentFiles[i]]{
                m_recentFiles.erase(std::remove_if(m_recentFiles.begin(), m_recentFiles.end(), [filename](const tgui::String& recentFile){ return filename == recentFile; }), m_recentFiles.end());
                saveGuiBuilderState();
                loadStartScreen();
            });
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadEditingScreen(const tgui::String& filename)
{
    m_gui->removeAllWidgets();
    m_gui->loadWidgetsFromFile("resources/forms/EditingScreen.txt");

    m_forms.push_back(std::make_unique<Form>(this, filename, m_gui->get<tgui::ChildWindow>("Form"), m_formSize));
    m_selectedForm = m_forms[0].get();

    m_propertiesWindow = m_gui->get<tgui::ChildWindow>("PropertiesWindow");
    m_propertiesContainer = m_propertiesWindow->get<tgui::ScrollablePanel>("Properties");
    m_selectedWidgetComboBox = m_propertiesWindow->get<tgui::ComboBox>("SelectedWidgetComboBox");

    m_selectedWidgetComboBox->addItem(filename, "form");
    m_selectedWidgetComboBox->setSelectedItemById("form");
    m_selectedWidgetComboBox->onItemSelect([this](tgui::String, tgui::String id){ m_selectedForm->selectWidgetById(id); });

    m_menuBar = m_gui->get<tgui::MenuBar>("MenuBar");
    m_menuBar->onMouseEnter([this]{ m_menuBar->moveToFront(); });
    m_menuBar->connectMenuItem({"File", "New"}, [this]{ menuBarCallbackNewForm(); });
    m_menuBar->connectMenuItem({"File", "Load"}, [this]{ menuBarCallbackLoadForm(); });
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

    m_menuBar->removeSubMenuItems({"File", "Recent"});
    bool addedRecentFile = false;
    for (const auto& recentFile : m_recentFiles)
    {
        if (filename == recentFile)
            continue;

        addedRecentFile = true;
        m_menuBar->addMenuItem({"File", "Recent", recentFile});
        m_menuBar->connectMenuItem({"File", "Recent", recentFile}, [this,recentFile]{
            tgui::Timer::scheduleCallback([this,recentFile]{ menuBarCallbackLoadRecent(recentFile); });
        });
    }
    m_menuBar->setMenuItemEnabled({"File", "Recent"}, addedRecentFile);

    const auto hierarchyWindow = m_gui->get<tgui::ChildWindow>("HierarchyWindow");
    m_widgetHierarchyTree = hierarchyWindow->get<tgui::TreeView>("WidgetsTree");
    m_widgetHierarchyTree->onItemSelect([this](tgui::String name){
        if (!name.empty())
            m_selectedForm->selectWidgetByName(name);
    });

    loadToolbox();
    initProperties();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::loadToolbox()
{
    auto toolboxWindow = m_gui->get<tgui::ChildWindow>("ToolboxWindow");
    auto toolbox = toolboxWindow->get<tgui::ScrollablePanel>("Widgets");

    const auto widgets = std::vector<std::pair<tgui::String, std::function<tgui::Widget::Ptr()>>>{
        {"BitmapButton", []{ return tgui::BitmapButton::create("BitBtn"); }},
        {"Button", []{ return tgui::Button::create("Button"); }},
        {"ChatBox", []{ return tgui::ChatBox::create(); }},
        {"CheckBox", []{ return tgui::CheckBox::create(); }},
        {"ChildWindow", []{ return tgui::ChildWindow::create(); }},
        {"ComboBox", []{ return tgui::ComboBox::create(); }},
        {"EditBox", []{ return tgui::EditBox::create(); }},
        {"Group", []{ return tgui::Group::create({150, 150}); }},
        {"Knob", []{ return tgui::Knob::create(); }},
        {"Label", []{ return tgui::Label::create("Label"); }},
        {"ListBox", []{ return tgui::ListBox::create(); }},
        {"ListView", []{ return tgui::ListView::create(); }},
        {"Panel", []{ return tgui::Panel::create({150, 150}); }},
        {"Picture", []{ return tgui::Picture::create((tgui::getResourcePath() / "resources/DefaultPicture.png").asString()); }},
        {"ProgressBar", []{ return tgui::ProgressBar::create(); }},
        {"RadioButton", []{ return tgui::RadioButton::create(); }},
        {"RangeSlider", []{ return tgui::RangeSlider::create(); }},
        {"ScrollablePanel", []{ return tgui::ScrollablePanel::create({150, 150}); }},
        {"Scrollbar", []{ return tgui::Scrollbar::create(); }},
        {"Slider", []{ return tgui::Slider::create(); }},
        {"SpinButton", []{ return tgui::SpinButton::create(); }},
        {"SpinControl", []{ return tgui::SpinControl::create(); }},
        {"Tabs", []{ auto tabs = tgui::Tabs::create(); tabs->add("Tab", false); return tabs; }},
        {"TextArea", []{ return tgui::TextArea::create(); }},
        {"TreeView", []{ return tgui::TreeView::create(); }},
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
        panel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
        panel->add(icon);
        panel->add(name);
        verticalLayout->add(panel);
        toolbox->add(verticalLayout);

        panel->onClick(TGUI_LAMBDA_CAPTURE_EQ_THIS{
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
            {
                selectedWidget->theme = "Custom";
                auto rendererComboBox = m_propertiesContainer->get<tgui::ComboBox>("RendererSelectorComboBox");
                rendererComboBox->setSelectedItem(selectedWidget->theme);
            }

            m_selectedForm->focus();
        });

        topPosition += verticalLayout->getSize().y;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::createNewWidget(tgui::Widget::Ptr widget, tgui::Container* parent, bool selectNewWidget)
{
    if (!parent)
    {
        tgui::Widget::Ptr selectedWidget = m_selectedForm->getSelectedWidget() ? m_selectedForm->getSelectedWidget()->ptr : nullptr;
        if (selectedWidget && selectedWidget->isContainer())
            parent = static_cast<tgui::Container*>(selectedWidget.get());
        else if (selectedWidget)
            parent = selectedWidget->getParent();
    }

    const tgui::String id = tgui::String::fromNumber(widget.get());
    const tgui::String name = m_selectedForm->addWidget(widget, parent, selectNewWidget);
    m_selectedWidgetComboBox->addItem(name, id);

    if (selectNewWidget)
        m_selectedWidgetComboBox->setSelectedItemById(id);

    widgetHierarchyChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiBuilder::updateWidgetProperty(const tgui::String& property, const tgui::String& value)
{
    tgui::String oldValue;
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
        m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->updateProperty(selectedWidget->ptr, property, oldValue);
        valueChanged = false;

        reloadProperties(); // Restore the old value before showing the error, of we keep repeating the error
        displayErrorMessage(tgui::String(U"Exception caught when setting property: ") + e.what());
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
    m_menuBar->setMenuItemEnabled({"Widget", "Paste"}, !m_copiedWidgets.empty());
    m_menuBar->setMenuItemEnabled({"Widget", "Delete"}, (selectedWidget != nullptr));
    m_menuBar->setMenuEnabled("Widget", (selectedWidget != nullptr) || !m_copiedWidgets.empty());

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

        rendererComboBox->onItemSelect(TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& item){
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
        propertyEditBox->onFocus([this]{ m_propertiesContainer->focusNextWidget(); });
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
        const std::vector<tgui::String> enumValues = tgui::Deserializer::split(type.substr(5, type.size() - 6), ',');
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

void GuiBuilder::changeWidgetName(const tgui::String& name)
{
    assert(m_selectedForm->getSelectedWidget() != nullptr);

    if (!m_selectedForm->setSelectedWidgetName(name))
    {
        // A widget with this name already existed, so reset the value of the edit box
        reloadProperties();
        return;
    }

    m_selectedWidgetComboBox->changeItemById(tgui::String::fromNumber(m_selectedForm->getSelectedWidget()->ptr.get()), name);

    widgetHierarchyChanged();
    m_selectedForm->setChanged(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::initSelectedWidgetComboBoxAfterLoad()
{
    const auto& widgets = m_selectedForm->getWidgets();
    for (const auto& widget : widgets)
    {
        const tgui::String id = tgui::String::fromNumber(widget->ptr.get());
        m_selectedWidgetComboBox->addItem(widget->name, id);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::removeSelectedWidget()
{
    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    assert(selectedWidget != nullptr);

    // Remove the child widgets from the combo box
    if (selectedWidget->ptr->isContainer())
    {
        std::vector<tgui::String> childIds;
        std::stack<tgui::Container::Ptr> parentsToSearch;
        parentsToSearch.push(selectedWidget->ptr->cast<tgui::Container>());
        while (!parentsToSearch.empty())
        {
            tgui::Container::Ptr parent = parentsToSearch.top();
            parentsToSearch.pop();
            for (const auto& widget : parent->getWidgets())
            {
                childIds.push_back(tgui::String::fromNumber(widget.get()));
                if (widget->isContainer())
                    parentsToSearch.push(widget->cast<tgui::Container>());
            }
        }

        for (const auto& childId : childIds)
            m_selectedWidgetComboBox->removeItemById(childId);
    }

    // Now remove the widget itself
    const tgui::String id = tgui::String::fromNumber(selectedWidget->ptr.get());
    m_selectedForm->removeWidget(id);
    m_selectedWidgetComboBox->removeItemById(id);
    m_selectedWidgetComboBox->setSelectedItemById("form");

    widgetHierarchyChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::removePopupMenu()
{
    if (!m_popupMenu)
        return;

    // Remove the popup menu and the transparent panel behind it
    m_popupMenu->getParent()->getParent()->remove(m_popupMenu->getParent()->shared_from_this());
    m_popupMenu = nullptr;

    if (m_selectedForm)
        m_selectedForm->focus();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::createNewForm(tgui::String filename)
{
    // If the filename is an absolute path that contains the resource path then make it relative
    const tgui::String basePath = tgui::getResourcePath().asString();
    if (filename.find(basePath) == 0)
    {
        filename.erase(0, basePath.length());
        if ((filename[0] == '/') || (filename[0] == '\\'))
            filename.erase(0, 1);
    }

    if (tgui::Filesystem::fileExists(tgui::getResourcePath() / filename))
    {
        auto panel = tgui::Panel::create({"100%", "100%"});
        panel->getRenderer()->setBackgroundColor({0, 0, 0, 175});
        m_gui->add(panel);

        auto messageBox = tgui::MessageBox::create("Create form", "The form already exists, are you certain you want to overwrite it?", {"Yes", "No"});
        messageBox->setPosition("(&.size - size) / 2");
        m_gui->add(messageBox);

        messageBox->onButtonPress(TGUI_LAMBDA_CAPTURE_EQ_THIS(const tgui::String& button){
            m_gui->remove(panel);
            m_gui->remove(messageBox);

            if (button == "Yes")
            {
                loadEditingScreen(filename);
                m_selectedForm->setChanged(true);
            }
        });
    }
    else // File didn't exist yet, so we can just create it
    {
        loadEditingScreen(filename);
        m_selectedForm->setChanged(true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiBuilder::loadForm(tgui::String filename)
{
    // If the filename is an absolute path that contains the resource path then make it relative
    const tgui::String basePath = tgui::getResourcePath().getNormalForm().asString();
    if (filename.find(basePath) == 0)
    {
        filename.erase(0, basePath.length());
        if ((filename[0] == '/') || (filename[0] == '\\'))
            filename.erase(0, 1);
    }

    loadEditingScreen(filename);

    try
    {
        m_selectedForm->load();
    }
    catch (const tgui::Exception& e)
    {
        loadStartScreen();
        widgetHierarchyChanged();
        displayErrorMessage("Failed to load '" + filename + "', reason: " + e.what());
        return false;
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

    widgetHierarchyChanged();
    initSelectedWidgetComboBoxAfterLoad();

    m_recentFiles.erase(std::remove_if(m_recentFiles.begin(), m_recentFiles.end(), [filename](const tgui::String& recentFile){ return filename == recentFile; }), m_recentFiles.end());
    m_recentFiles.insert(m_recentFiles.begin(), filename);
    saveGuiBuilderState();
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::displayErrorMessage(const tgui::String& error)
{
    std::cerr << error << std::endl;

    auto messageBox = tgui::MessageBox::create("Error", error, {"OK"});
    messageBox->setPosition("(&.size - size) / 2");
    openWindowWithFocus(messageBox);

    messageBox->onButtonPress([messageBox]{ messageBox->close(); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::ChildWindow::Ptr GuiBuilder::openWindowWithFocus(tgui::ChildWindow::Ptr window)
{
    auto panel = tgui::Panel::create({"100%", "100%"});
    panel->getRenderer()->setBackgroundColor({0, 0, 0, 175});
    m_gui->add(panel);

    window->setPosition("(&.w - w) / 2", "(&.h - h) / 2");
    m_gui->add(window);

    window->setFocused(true);

    const bool tabUsageEnabled = m_gui->isTabKeyUsageEnabled();
    auto closeWindow = TGUI_LAMBDA_CAPTURE_EQ_THIS{
        m_gui->remove(window);
        m_gui->remove(panel);
        m_gui->setTabKeyUsageEnabled(tabUsageEnabled);
        m_foregroundPanel = nullptr;
    };

    panel->onClick(closeWindow);
    window->onClose(closeWindow);
    window->onEscapeKeyPress(closeWindow);

    m_foregroundPanel = panel;
    return window;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::String GuiBuilder::getDefaultFilename() const
{
    return "form.txt";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::copyWidgetRecursive(std::vector<CopiedWidget>& copiedWidgetList, std::shared_ptr<WidgetInfo> widgetInfo)
{
    CopiedWidget copiedWidget;
    copiedWidget.theme = widgetInfo->theme;
    copiedWidget.widget = widgetInfo->ptr->clone();
    copiedWidget.originalWidget = widgetInfo->ptr;
    copiedWidget.widget->getRenderer(); // Make sure renderer isn't still shared

    if (widgetInfo->ptr->isContainer())
    {
        const auto& container = widgetInfo->ptr->cast<tgui::Container>();
        for (const auto& childWidget : container->getWidgets())
        {
            const auto& childWidgetInfo = m_selectedForm->getWidget(tgui::String::fromNumber(childWidget.get()));
            copyWidgetRecursive(copiedWidget.childWidgets, childWidgetInfo);
        }

        // Remove the widgets inside the container itself as we stored them separately
        copiedWidget.widget->cast<tgui::Container>()->removeAllWidgets();
    }

    copiedWidgetList.push_back(std::move(copiedWidget));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::pasteWidgetRecursive(const CopiedWidget& copiedWidget, tgui::Container* parent)
{
    auto widget = copiedWidget.widget->clone(); // Clone again, as we may be pasting same widget multiple times
    createNewWidget(widget, parent, false);

    m_selectedForm->getWidget(tgui::String::fromNumber(widget.get()))->theme = copiedWidget.theme;

    for (const auto& copiedChild : copiedWidget.childWidgets)
        pasteWidgetRecursive(copiedChild, widget->cast<tgui::Container>().get());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::copyWidgetToInternalClipboard(std::shared_ptr<WidgetInfo> widgetInfo)
{
    m_copiedWidgets.clear();
    copyWidgetRecursive(m_copiedWidgets, widgetInfo);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::pasteWidgetFromInternalClipboard()
{
    if (m_copiedWidgets.empty())
        return;

    // When selecting a container and pressing ctrl+C immediately followed by ctrl+V, the the new container should be a sibling
    // of the old one, not a child. So don't try to paste a container inside itself.
    if (m_selectedForm->getSelectedWidget() && (m_selectedForm->getSelectedWidget()->ptr == m_copiedWidgets[0].originalWidget))
        m_selectedForm->selectParent();

    auto widget = m_copiedWidgets[0].widget->clone(); // Clone again, as we may be pasting same widget multiple times
    createNewWidget(widget);
    m_selectedForm->getWidget(tgui::String::fromNumber(widget.get()))->theme = m_copiedWidgets[0].theme;

    // Copy child widgets
    if (!m_copiedWidgets[0].childWidgets.empty())
    {
        const auto& container = widget->cast<tgui::Container>();
        for (const auto& copiedChild : m_copiedWidgets[0].childWidgets)
            pasteWidgetRecursive(copiedChild, container.get());
    }

    // If widget still has same parent then move the widget a bit down and to the right to visually show that the new widget has been created.
    // If the widget lies outside its parent then move its position so that it becomes visible.
    bool originalWidgetMovedSinceCopy = (widget->getPosition() != m_copiedWidgets[0].originalWidget->getPosition());
    if (widget->getPositionLayout().x.isConstant())
    {
        float newX = widget->getPosition().x;
        if ((widget->getParent() == m_copiedWidgets[0].originalWidget->getParent()) && !originalWidgetMovedSinceCopy)
            newX += 10;
        if (newX + (widget->getSize().x / 2.f) > widget->getParent()->getSize().x)
            newX = widget->getParent()->getSize().x -  (widget->getSize().x / 2.f);

        updateWidgetProperty("Left", tgui::String::fromNumber(newX));
    }
    if (widget->getPositionLayout().y.isConstant())
    {
        float newY = widget->getPosition().y;
        if ((widget->getParent() == m_copiedWidgets[0].originalWidget->getParent()) && !originalWidgetMovedSinceCopy)
            newY += 10;
        if (newY + (widget->getSize().y / 2.f) > widget->getParent()->getSize().y)
            newY = widget->getParent()->getSize().y -  (widget->getSize().y / 2.f);

        updateWidgetProperty("Top", tgui::String::fromNumber(newY));
    }

    initProperties();

    m_selectedForm->setChanged(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::EditBox::Ptr GuiBuilder::addPropertyValueEditBox(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition, float rightPadding)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto valueEditBox = m_propertiesContainer->get<tgui::EditBox>("Value" + property);
    if (!valueEditBox)
    {
        valueEditBox = tgui::EditBox::create();
        m_propertiesContainer->add(valueEditBox, "Value" + property);
        valueEditBox->setCaretPosition(0); // Show the first part of the contents instead of the last part when the text does not fit
    }

    valueEditBox->onUnfocus.disconnectAll();
    valueEditBox->onReturnKeyPress.disconnectAll();
    valueEditBox->setPosition({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, topPosition});
    valueEditBox->setSize({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f - rightPadding, EDIT_BOX_HEIGHT});
    valueEditBox->setText(value);

    valueEditBox->onReturnOrUnfocus([=]{ onChange(valueEditBox->getText()); });
    return valueEditBox;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::Button::Ptr GuiBuilder::addPropertyValueButtonMore(const tgui::String& property, float topPosition)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto buttonMore = m_propertiesContainer->get<tgui::Button>("ValueButton" + property);
    if (!buttonMore)
    {
        buttonMore = tgui::Button::create();
        buttonMore->setText(L"\u22EF");
        buttonMore->setTextSize(18);
        buttonMore->onFocus([this]{ m_propertiesContainer->focusNextWidget(); });
        m_propertiesContainer->add(buttonMore, "ValueButton" + property);
    }

    buttonMore->onPress.disconnectAll();
    buttonMore->setSize({EDIT_BOX_HEIGHT, EDIT_BOX_HEIGHT});
    buttonMore->setPosition({bindWidth(m_propertiesContainer) - scrollbarWidth - EDIT_BOX_HEIGHT, topPosition});
    return buttonMore;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueBool(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
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

    valueComboBox->onItemSelect.disconnectAll();
    valueComboBox->setPosition({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, topPosition});
    valueComboBox->setSize({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, EDIT_BOX_HEIGHT});

    tgui::String str = value.toLower();
    if (str == "true" || str == "yes" || str == "on" || str == "y" || str == "t" || str == "1")
        valueComboBox->setSelectedItemByIndex(1);
    else
        valueComboBox->setSelectedItemByIndex(0);

    valueComboBox->onItemSelect([=]{ onChange(valueComboBox->getSelectedItem()); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueColor(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
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
        colorPreviewPanel->getRenderer()->setBorderColor(tgui::Color::Black);
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
        colorPreviewPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

    colorPreviewPanel->onClick.disconnectAll();
    colorPreviewPanel->onClick(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        const tgui::Color color = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Color, value).getColor();
        auto colorPicker = tgui::ColorPicker::create("Select color", color);
        openWindowWithFocus(colorPicker);
        colorPicker->onOkPress([=](tgui::Color newColor){
            onChange(tgui::Serializer::serialize(newColor));
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueTextStyle(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto textStyleWindow = openWindowWithFocus();
        textStyleWindow->setTitle("Set text style");
        textStyleWindow->setClientSize({180, 160});
        textStyleWindow->loadWidgetsFromFile("resources/forms/SetTextStyle.txt");

        auto checkBoxBold = textStyleWindow->get<tgui::CheckBox>("CheckBoxBold");
        auto checkBoxItalic = textStyleWindow->get<tgui::CheckBox>("CheckBoxItalic");
        auto checkBoxUnderlined = textStyleWindow->get<tgui::CheckBox>("CheckBoxUnderlined");
        auto checkBoxStrikeThrough = textStyleWindow->get<tgui::CheckBox>("CheckBoxStrikeThrough");

        unsigned int style = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::TextStyle, value).getTextStyle();
        checkBoxBold->setChecked((style & tgui::TextStyle::Bold) != 0);
        checkBoxItalic->setChecked((style & tgui::TextStyle::Italic) != 0);
        checkBoxUnderlined->setChecked((style & tgui::TextStyle::Underlined) != 0);
        checkBoxStrikeThrough->setChecked((style & tgui::TextStyle::StrikeThrough) != 0);

        auto updateTextStyleProperty = [=]{
            unsigned int newStyle = 0;
            newStyle |= (checkBoxBold->isChecked() ? tgui::TextStyle::Bold : 0);
            newStyle |= (checkBoxItalic->isChecked() ? tgui::TextStyle::Italic : 0);
            newStyle |= (checkBoxUnderlined->isChecked() ? tgui::TextStyle::Underlined : 0);
            newStyle |= (checkBoxStrikeThrough->isChecked() ? tgui::TextStyle::StrikeThrough : 0);
            onChange(tgui::Serializer::serialize(tgui::TextStyles{newStyle}));
        };
        checkBoxBold->onChange(updateTextStyleProperty);
        checkBoxItalic->onChange(updateTextStyleProperty);
        checkBoxUnderlined->onChange(updateTextStyleProperty);
        checkBoxStrikeThrough->onChange(updateTextStyleProperty);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueOutline(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto outlineWindow = openWindowWithFocus();
        outlineWindow->setTitle("Set outline");
        outlineWindow->setClientSize({150, 150});
        outlineWindow->loadWidgetsFromFile("resources/forms/SetOutline.txt");

        auto editLeft = outlineWindow->get<tgui::EditBox>("EditLeft");
        auto editTop = outlineWindow->get<tgui::EditBox>("EditTop");
        auto editRight = outlineWindow->get<tgui::EditBox>("EditRight");
        auto editBottom = outlineWindow->get<tgui::EditBox>("EditBottom");

        tgui::Outline outline = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Outline, value).getOutline();
        editLeft->setText(tgui::String::fromNumber(outline.getLeft()));
        editTop->setText(tgui::String::fromNumber(outline.getTop()));
        editRight->setText(tgui::String::fromNumber(outline.getRight()));
        editBottom->setText(tgui::String::fromNumber(outline.getBottom()));

        auto updateOutlineProperty = [=]{
            const tgui::Outline newOutline{
                tgui::AbsoluteOrRelativeValue{editLeft->getText()},
                tgui::AbsoluteOrRelativeValue{editTop->getText()},
                tgui::AbsoluteOrRelativeValue{editRight->getText()},
                tgui::AbsoluteOrRelativeValue{editBottom->getText()},
            };
            onChange(tgui::Serializer::serialize(newOutline));
        };
        editLeft->onReturnOrUnfocus(updateOutlineProperty);
        editTop->onReturnOrUnfocus(updateOutlineProperty);
        editRight->onReturnOrUnfocus(updateOutlineProperty);
        editBottom->onReturnOrUnfocus(updateOutlineProperty);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueMultilineString(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto multilineStringWindow = openWindowWithFocus();
        multilineStringWindow->setTitle("Set multiline text");
        multilineStringWindow->setClientSize({470, 220});
        multilineStringWindow->loadWidgetsFromFile("resources/forms/SetMultilineString.txt");

        auto textArea = multilineStringWindow->get<tgui::TextArea>("TextArea");
        textArea->setText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());
        textArea->onTextChange([=]{ onChange(tgui::Serializer::serialize(textArea->getText())); });
        textArea->setFocused(true);

        m_gui->setTabKeyUsageEnabled(false);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueStringList(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto setArrowColor = [](tgui::BitmapButton::Ptr button, tgui::Color color){
        tgui::Texture texture = button->getImage();
        texture.setColor(color);
        button->setImage(texture);
    };

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto stringListWindow = openWindowWithFocus();
        stringListWindow->setTitle("Set string list");
        stringListWindow->setClientSize({352, 215});
        stringListWindow->loadWidgetsFromFile("resources/forms/SetStringList.txt");

        auto listBox = stringListWindow->get<tgui::ListBox>("ListBox");
        auto editBox = stringListWindow->get<tgui::EditBox>("EditBox");
        auto buttonAdd = stringListWindow->get<tgui::Button>("BtnAdd");
        auto buttonRemove = stringListWindow->get<tgui::Button>("BtnRemove");
        auto buttonArrowUp = stringListWindow->get<tgui::BitmapButton>("BtnArrowUp");
        auto buttonArrowDown = stringListWindow->get<tgui::BitmapButton>("BtnArrowDown");

        std::vector<tgui::String> items = WidgetProperties::deserializeList(value);
        for (const auto& item : items)
            listBox->addItem(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, item).getString());

        setArrowColor(buttonArrowUp, buttonArrowUp->getSharedRenderer()->getTextColorDisabled());
        setArrowColor(buttonArrowDown, buttonArrowDown->getSharedRenderer()->getTextColorDisabled());
        buttonRemove->setEnabled(false);
        buttonArrowUp->setEnabled(false);
        buttonArrowDown->setEnabled(false);

        listBox->onItemSelect([=]{
            const int index = listBox->getSelectedItemIndex();
            if (index >= 0)
            {
                buttonRemove->setEnabled(true);
                editBox->setText(listBox->getSelectedItem());
            }
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

        buttonArrowUp->onPress([=]{
            const std::size_t index = static_cast<std::size_t>(listBox->getSelectedItemIndex());
            tgui::String value1 = listBox->getItemByIndex(index - 1);
            tgui::String value2 = listBox->getItemByIndex(index);
            listBox->changeItemByIndex(index - 1, value2);
            listBox->changeItemByIndex(index, value1);
            listBox->setSelectedItemByIndex(index - 1);
            updateValue();
        });

        buttonArrowDown->onPress([=]{
            const std::size_t index = static_cast<std::size_t>(listBox->getSelectedItemIndex());
            tgui::String value1 = listBox->getItemByIndex(index);
            tgui::String value2 = listBox->getItemByIndex(index + 1);
            listBox->changeItemByIndex(index, value2);
            listBox->changeItemByIndex(index + 1, value1);
            listBox->setSelectedItemByIndex(index + 1);
            updateValue();
        });

        buttonRemove->onPress([=]{
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
        buttonAdd->onPress(addItem);
        editBox->onReturnKeyPress(addItem);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueTexture(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto textureWindow = openWindowWithFocus();
        textureWindow->setTitle("Set texture");
        textureWindow->setClientSize({235, 250});
        textureWindow->loadWidgetsFromFile("resources/forms/SetTexture.txt");

        auto previewPicture = textureWindow->get<tgui::Picture>("ImagePreview");
        auto buttonSelectFile = textureWindow->get<tgui::Button>("BtnSelectFile");
        auto editBoxPartRect = textureWindow->get<tgui::EditBox>("EditPartRect");
        auto editBoxMiddleRect = textureWindow->get<tgui::EditBox>("EditMiddleRect");
        auto checkBoxSmooth = textureWindow->get<tgui::CheckBox>("CheckBoxSmooth");

        auto deserializeRect = [=](tgui::String str) -> tgui::UIntRect {
            if (str.empty())
                return {};

            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            const std::vector<tgui::String> tokens = tgui::Deserializer::split(str, ',');
            if (tokens.size() == 4)
                return {tokens[0].toUInt(), tokens[1].toUInt(), tokens[2].toUInt(), tokens[3].toUInt()};
            else
                return {};
        };

        previewPicture->setUserData(std::make_shared<tgui::Texture>());

        auto separators = std::make_shared<std::vector<tgui::SeparatorLine::Ptr>>();

        auto updateForm = TGUI_LAMBDA_CAPTURE_EQ_THIS(tgui::String filename, tgui::UIntRect partRect, tgui::UIntRect middleRect, bool smooth, bool resetPartRect, bool resetMiddleRect, bool resetSmooth){
            auto texture = previewPicture->getUserData<std::shared_ptr<tgui::Texture>>();

            try
            {
                texture->load(filename, partRect, middleRect, smooth);
                onChange(tgui::Serializer::serialize(*texture));
            }
            catch (tgui::Exception&)
            {
            }

            buttonSelectFile->setUserData(filename); // Not using texture.getId() as it would be empty if file didn't exist

            const tgui::Vector2u imageSize = texture->getImageSize();
            const tgui::Vector2f extraSpace{20, 180};
            const tgui::Vector2f minSize{235, 165};
            const tgui::Layout2d maxSize{tgui::bindWidth(*m_gui) - 50, tgui::bindHeight(*m_gui) - 50};
            const tgui::Layout scaling = tgui::bindMin(1.f, tgui::bindMin((maxSize.x - extraSpace.x) / imageSize.x, (maxSize.y - extraSpace.y) / imageSize.y));
            previewPicture->setSize({imageSize.x * scaling, imageSize.y * scaling});
            textureWindow->setSize({tgui::bindMax(minSize.x, (imageSize.x * scaling) + extraSpace.x), tgui::bindMax(minSize.y, (imageSize.y * scaling) + extraSpace.y)});

            if (resetPartRect)
            {
                partRect = texture->getPartRect();
                editBoxPartRect->onTextChange.setEnabled(false);
                editBoxPartRect->setText("(" + tgui::String::fromNumber(partRect.left) + ", " + tgui::String::fromNumber(partRect.top)
                    + ", " + tgui::String::fromNumber(partRect.width) + ", " + tgui::String::fromNumber(partRect.height) + ")");
                editBoxPartRect->onTextChange.setEnabled(true);
            }

            if (resetMiddleRect)
            {
                middleRect = texture->getMiddleRect();

                editBoxMiddleRect->onTextChange.setEnabled(false);
                editBoxMiddleRect->setText("(" + tgui::String::fromNumber(middleRect.left) + ", " + tgui::String::fromNumber(middleRect.top)
                    + ", " + tgui::String::fromNumber(middleRect.width) + ", " + tgui::String::fromNumber(middleRect.height) + ")");
                editBoxMiddleRect->onTextChange.setEnabled(true);
            }

            if (resetSmooth)
            {
                checkBoxSmooth->onChange.setEnabled(false);
                checkBoxSmooth->setChecked(texture->isSmooth());
                checkBoxSmooth->onChange.setEnabled(true);
            }

            previewPicture->getRenderer()->setTexture(*texture);

            for (auto& separator : *separators)
                textureWindow->remove(separator);
            separators->clear();

            if ((middleRect != tgui::UIntRect{}) && (middleRect != tgui::UIntRect{0, 0, imageSize.x, imageSize.y}))
            {
                std::vector<std::pair<tgui::Vector2f, tgui::Vector2f>> lines;
                if ((middleRect.left != 0) || (middleRect.width != imageSize.x))
                {
                    lines.emplace_back(tgui::Vector2f{static_cast<float>(middleRect.left), 0} * scaling.getValue(),
                                       tgui::Vector2f{static_cast<float>(middleRect.left), static_cast<float>(imageSize.y)} * scaling.getValue());
                    lines.emplace_back(tgui::Vector2f{static_cast<float>(middleRect.left + middleRect.width), 0} * scaling.getValue(),
                                       tgui::Vector2f{static_cast<float>(middleRect.left + middleRect.width), static_cast<float>(imageSize.y)} * scaling.getValue());
                }
                if ((middleRect.top != 0) || (middleRect.height != imageSize.y))
                {
                    lines.emplace_back(tgui::Vector2f{0, static_cast<float>(middleRect.top)} * scaling.getValue(),
                                       tgui::Vector2f{static_cast<float>(imageSize.x), static_cast<float>(middleRect.top)} * scaling.getValue());
                    lines.emplace_back(tgui::Vector2f{0, static_cast<float>(middleRect.top + middleRect.height)} * scaling.getValue(),
                                       tgui::Vector2f{static_cast<float>(imageSize.x), static_cast<float>(middleRect.top + middleRect.height)} * scaling.getValue());
                }

                for (const auto& line : lines)
                {
                    auto separator = tgui::SeparatorLine::create();
                    separator->setPosition(previewPicture->getPosition() + line.first);
                    separator->setSize(std::max(line.second.x - line.first.x, 1.f), std::max(line.second.y - line.first.y, 1.f));
                    separator->getRenderer()->setColor({255, 128, 255});
                    textureWindow->add(separator);
                    separators->push_back(separator);
                }
            }
        };

        editBoxPartRect->onTextChange([=]{
            updateForm(buttonSelectFile->getUserData<tgui::String>(), deserializeRect(editBoxPartRect->getText()),
                       {}, checkBoxSmooth->isChecked(), false, true, false);
        });
        editBoxMiddleRect->onTextChange([=]{
            updateForm(buttonSelectFile->getUserData<tgui::String>(), deserializeRect(editBoxPartRect->getText()),
                       deserializeRect(editBoxMiddleRect->getText()), checkBoxSmooth->isChecked(), false, false, false);
        });
        checkBoxSmooth->onChange([=]{
            updateForm(buttonSelectFile->getUserData<tgui::String>(), deserializeRect(editBoxPartRect->getText()),
                       deserializeRect(editBoxMiddleRect->getText()), checkBoxSmooth->isChecked(), false, false, false);
        });

        tgui::Texture originalTexture;
        try
        {
            originalTexture = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Texture, value).getTexture();
        }
        catch (tgui::Exception& e)
        {
            // Is it possible to reach this code? It would mean the existing value was already bad, but we don't allow setting bad values?
            displayErrorMessage(tgui::String(U"Exception caught when loading image: ") + e.what());
        }

        const tgui::String originalFilename = originalTexture.getId();
        const tgui::UIntRect originalPartRect = originalTexture.getPartRect();
        const tgui::UIntRect originalMiddleRect = originalTexture.getMiddleRect();
        const bool originalSmooth = originalTexture.isSmooth();
        updateForm(originalFilename, originalPartRect, originalMiddleRect, originalSmooth, true, true, true);

        buttonSelectFile->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
            showLoadFileWindow("Load image", "Load", true, buttonSelectFile->getUserData<tgui::String>(), [=](const tgui::String& filename){
                updateForm(filename, {}, {}, checkBoxSmooth->isChecked(), true, true, false);
            });
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueEditBoxInputValidator(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto outlineWindow = openWindowWithFocus();
        outlineWindow->setTitle("Set accepted input");
        outlineWindow->setClientSize({190, 215});
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
        auto updateValidator = [=](const tgui::String& newValue){
            editValidator->onReturnKeyPress.disconnectAll();
            editValidator->onUnfocus.disconnectAll();
            editValidator->setText(newValue);
            editValidator->onReturnOrUnfocus(updateCustomValidator);
            onChange(newValue);
        };
        checkAny->onCheck([=]{ updateValidator(tgui::EditBox::Validator::All); });
        checkInt->onCheck([=]{ updateValidator(tgui::EditBox::Validator::Int); });
        checkUInt->onCheck([=]{ updateValidator(tgui::EditBox::Validator::UInt); });
        checkFloat->onCheck([=]{ updateValidator(tgui::EditBox::Validator::Float); });
        editValidator->onReturnOrUnfocus(updateCustomValidator);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueChildWindowTitleButtons(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto titleButtonWindow = openWindowWithFocus();
        titleButtonWindow->setTitle("Set title buttons");
        titleButtonWindow->setClientSize({125, 125});
        titleButtonWindow->loadWidgetsFromFile("resources/forms/SetChildWindowTitleButtons.txt");

        auto checkClose = titleButtonWindow->get<tgui::RadioButton>("CheckBoxClose");
        auto checkMaximize = titleButtonWindow->get<tgui::RadioButton>("CheckBoxMaximize");
        auto checkMinimize = titleButtonWindow->get<tgui::RadioButton>("CheckBoxMinimize");

        for (const auto& elem : tgui::Deserializer::split(value, '|'))
        {
            tgui::String titleButtonStr = elem.trim().toLower();
            if (titleButtonStr == "close")
                checkClose->setChecked(true);
            else if (titleButtonStr == "maximize")
                checkMaximize->setChecked(true);
            else if (titleButtonStr == "minimize")
                checkMinimize->setChecked(true);
        }

        auto updateTitleButtons = [=]{
            tgui::String newValue;
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
        checkClose->onChange(updateTitleButtons);
        checkMaximize->onChange(updateTitleButtons);
        checkMinimize->onChange(updateTitleButtons);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueEnum(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition, const std::vector<tgui::String>& enumValues)
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

    valueComboBox->onItemSelect.disconnectAll();
    valueComboBox->setPosition({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, topPosition});
    valueComboBox->setSize({(bindWidth(m_propertiesContainer) - scrollbarWidth) / 2.f, EDIT_BOX_HEIGHT});

    tgui::String valueLower = value.toLower();
    for (unsigned int i = 0; i < enumValues.size(); ++i)
    {
        if (enumValues[i].toLower() == valueLower)
            valueComboBox->setSelectedItemByIndex(i);
    }

    valueComboBox->onItemSelect([=]{ onChange(valueComboBox->getSelectedItem()); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackNewForm()
{
    loadStartScreen();

    showLoadFileWindow("New form", "Create", false, getDefaultFilename(), [this](const tgui::String& filename){
        createNewForm(filename);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackLoadForm()
{
    loadStartScreen();

    showLoadFileWindow("Load form", "Load", true, getDefaultFilename(), [this](const tgui::String& filename){
        loadForm(filename);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackLoadRecent(const tgui::String& filename)
{
    loadStartScreen();
    loadForm(filename);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackSaveFile()
{
    m_selectedForm->save();

    saveGuiBuilderState();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackQuit()
{
    saveGuiBuilderState();

    while (!m_forms.empty())
        closeForm(m_forms[0].get());

    m_window->close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackEditThemes()
{
    auto editThemesWindow = openWindowWithFocus();
    editThemesWindow->setTitle("Edit themes");
    editThemesWindow->setClientSize({620, 280});
    editThemesWindow->loadWidgetsFromFile("resources/forms/EditThemes.txt");

    auto buttonAdd = editThemesWindow->get<tgui::Button>("ButtonAdd");
    auto buttonDelete = editThemesWindow->get<tgui::Button>("ButtonDelete");
    auto buttonBrowse = editThemesWindow->get<tgui::Button>("ButtonBrowse");
    auto newThemeEditBox = editThemesWindow->get<tgui::EditBox>("NewThemeEditBox");
    auto themesList = editThemesWindow->get<tgui::ListBox>("ThemesList");

    themesList->removeAllItems();
    for (auto& theme : m_themes)
    {
        if (!theme.second.getPrimary().empty())
            themesList->addItem(theme.second.getPrimary());
    }

    themesList->onItemSelect([=](tgui::String item){
        if (item.empty())
            buttonDelete->setEnabled(false);
        else
            buttonDelete->setEnabled(true);

        newThemeEditBox->setText(item);
    });

    newThemeEditBox->onTextChange([=](tgui::String text){
        if (text.empty())
            buttonAdd->setEnabled(false);
        else
            buttonAdd->setEnabled(true);
    });

    buttonAdd->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        try
        {
            const tgui::String filename = newThemeEditBox->getText();
            if (!themesList->contains(filename))
            {
                tgui::Theme theme{(tgui::getResourcePath() / filename).asString()};
                themesList->addItem(filename);
                m_themes[filename] = theme;
            }
        }
        catch (const tgui::Exception& e)
        {
            displayErrorMessage(tgui::String(U"Exception caught when adding theme: ") + e.what());
        }

        initProperties();
        saveGuiBuilderState();
    });

    buttonDelete->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto item = themesList->getSelectedItem();
        m_themes.erase(item);
        themesList->removeItem(item);
        buttonDelete->setEnabled(false);
        initProperties();
        saveGuiBuilderState();
    });

    buttonBrowse->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
        auto fileDialog = tgui::FileDialog::create("Select theme file", "Select");
        fileDialog->setFileMustExist(true);

        if (!newThemeEditBox->getText().empty())
        {
            const auto inputPath = tgui::Filesystem::Path(newThemeEditBox->getText());
            fileDialog->setPath((tgui::getResourcePath() / inputPath).getParentPath().getNormalForm());
            fileDialog->setFilename(inputPath.getFilename());
        }
        openWindowWithFocus(fileDialog);

        fileDialog->onFileSelect([newThemeEditBox](const std::vector<tgui::Filesystem::Path>& selectedFiles){
            if (!selectedFiles.empty())
                newThemeEditBox->setText(selectedFiles[0].asString());
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackBringWidgetToFront()
{
    m_selectedForm->getSelectedWidget()->ptr->moveToFront();
    m_selectedForm->setChanged(true);

    widgetHierarchyChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackSendWidgetToBack()
{
    m_selectedForm->getSelectedWidget()->ptr->moveToBack();
    m_selectedForm->setChanged(true);

    widgetHierarchyChanged();
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

void GuiBuilder::updateSelectedWidgetHierarchy()
{
    m_widgetHierarchyTree->onItemSelect.setEnabled(false);

    m_widgetHierarchyTree->deselectItem();
    if (m_selectedForm->getSelectedWidget())
    {
        std::stack<std::vector<tgui::TreeView::ConstNode>> treeNodes;
        for (const auto& node : m_widgetHierarchyTree->getNodes())
            treeNodes.push({node});
        while (!treeNodes.empty())
        {
            std::vector<tgui::TreeView::ConstNode> nodeList = treeNodes.top();
            treeNodes.pop();

            if (nodeList.back().text == m_selectedForm->getSelectedWidget()->name)
            {
                std::vector<tgui::String> hierarchy;
                for (const auto& node : nodeList)
                    hierarchy.push_back(node.text);

                m_widgetHierarchyTree->selectItem(hierarchy);
                break;
            }

            for (const auto& node : nodeList.back().nodes)
            {
                treeNodes.push(nodeList);
                treeNodes.top().push_back(node);
            }
        }
    }
    else
        m_widgetHierarchyTree->selectItem({m_selectedForm->getFilename()});

    m_widgetHierarchyTree->onItemSelect.setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::fillWidgetHierarchyTreeRecursively(std::vector<tgui::String>& hierarchy, std::shared_ptr<tgui::Widget> parentWidget)
{
    m_widgetHierarchyTree->addItem(hierarchy);

    if (auto* asContainer = dynamic_cast<tgui::Container*>(parentWidget.get()))
    {
        const size_t widgetCount = asContainer->getWidgets().size();
        for (size_t i = 0; i < widgetCount; ++i)
        {
            hierarchy.push_back(asContainer->getWidgets()[i]->getWidgetName());
            fillWidgetHierarchyTreeRecursively(hierarchy, asContainer->getWidgets()[i]);
            hierarchy.pop_back();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::widgetHierarchyChanged()
{
    m_widgetHierarchyTree->removeAllItems();

    if (m_selectedForm == nullptr)
        return;

    std::vector<tgui::String> widgetHiearchy{m_selectedForm->getFilename()};
    fillWidgetHierarchyTreeRecursively(widgetHiearchy, m_selectedForm->getRootWidgetsGroup());

    updateSelectedWidgetHierarchy();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiBuilder::fillWidgetHierarchy(std::vector<tgui::String>& hierarchy, tgui::Widget* widget)
{
    tgui::Container* parent = widget->getParent();

    assert(parent);
    assert(m_selectedForm);

    // finding current widget name in parent's container

    auto widgets = m_selectedForm->getWidgets();

    auto it = std::find_if(widgets.begin(), widgets.end(), [&](std::shared_ptr<WidgetInfo> otherWidget){
        return otherWidget->ptr.get() == widget;
    });

    // if found - adding to hierarchy, and repeat for parent widget
    // if not - add file name as root

    if (it != widgets.end())
    {
        hierarchy.push_back((*it)->name);

        bool wasFound = false;

        const size_t widgetCount = parent->getWidgets().size();
        for (size_t i = 0; i < widgetCount; ++i)
        {
            if ((parent->getWidgets()[i]).get() == widget)
            {
                wasFound = fillWidgetHierarchy(hierarchy, parent);
                break;
            }
        }

        if (!wasFound)
            hierarchy.pop_back();

        return wasFound;
    }
    else
    {
        hierarchy.push_back(m_selectedForm->getFilename());
        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackKeyboardShortcuts()
{
    auto keyboardShortcutsWindow = openWindowWithFocus();
    keyboardShortcutsWindow->setTitle("Keyboard shortcuts");
    keyboardShortcutsWindow->setClientSize({375, 310});
    keyboardShortcutsWindow->loadWidgetsFromFile("resources/forms/KeyboardShortcuts.txt");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackAbout()
{
    auto aboutWindow = openWindowWithFocus();
    aboutWindow->setTitle("About");
    aboutWindow->setClientSize({365, 130});
    aboutWindow->loadWidgetsFromFile("resources/forms/About.txt");

    auto labelVersion = aboutWindow->get<tgui::Label>("LabelVersion");
    labelVersion->setText(tgui::String::fromNumber(TGUI_VERSION_MAJOR) + "." + tgui::String::fromNumber(TGUI_VERSION_MINOR) + "." + tgui::String::fromNumber(TGUI_VERSION_PATCH));
}
