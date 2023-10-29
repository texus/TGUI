/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include "GuiBuilder.hpp"
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
#include "WidgetProperties/RichTextLabelProperties.hpp"
#include "WidgetProperties/ScrollablePanelProperties.hpp"
#include "WidgetProperties/ScrollbarProperties.hpp"
#include "WidgetProperties/SliderProperties.hpp"
#include "WidgetProperties/SpinButtonProperties.hpp"
#include "WidgetProperties/SpinControlProperties.hpp"
#include "WidgetProperties/TabsProperties.hpp"
#include "WidgetProperties/TextAreaProperties.hpp"
#include "WidgetProperties/ToggleButtonProperties.hpp"
#include "WidgetProperties/TreeViewProperties.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const float EDIT_BOX_HEIGHT = 24;
static const unsigned int UNDO_MAX_SAVES = 1000;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    bool compareRenderers(std::map<tgui::String, tgui::ObjectConverter> themePropertyValuePairs, std::map<tgui::String, tgui::ObjectConverter> widgetPropertyValuePairs)
    {
        for (auto& pair : themePropertyValuePairs)
        {
            const auto& property = pair.first;
            auto& value = pair.second;

            if (((widgetPropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::None)
              && (value.getType() != tgui::ObjectConverter::Type::None))
             || ((widgetPropertyValuePairs[property].getType() != tgui::ObjectConverter::Type::None)
              && (value.getType() == tgui::ObjectConverter::Type::None))
             || ((widgetPropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::String)
              && (value.getType() != tgui::ObjectConverter::Type::String)
              && (widgetPropertyValuePairs[property].getString() != value.getString()))
             || ((widgetPropertyValuePairs[property].getType() != tgui::ObjectConverter::Type::String)
              && (value.getType() == tgui::ObjectConverter::Type::String)
              && (widgetPropertyValuePairs[property].getString() != value.getString()))
             || ((widgetPropertyValuePairs[property].getType() != tgui::ObjectConverter::Type::String)
              && (value.getType() != tgui::ObjectConverter::Type::String)
              && (widgetPropertyValuePairs[property] != value)))
            {
                // Exception: Colors should never be compared as strings
                if (((widgetPropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::Color)
                 && (value.getType() == tgui::ObjectConverter::Type::String))
                || ((widgetPropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::String)
                 && (value.getType() == tgui::ObjectConverter::Type::Color)))
                {
                    if (widgetPropertyValuePairs[property].getColor() == value.getColor())
                        continue;
                }

                // Exception: Don't use the data pointers and try to use absolute paths to compare textures
                if ((widgetPropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::Texture)
                 && (value.getType() == tgui::ObjectConverter::Type::Texture)
                 && widgetPropertyValuePairs[property].getTexture().getData()
                 && value.getTexture().getData())
                {
                    if ((widgetPropertyValuePairs[property].getTexture().getId() == value.getTexture().getId())
                     && (widgetPropertyValuePairs[property].getTexture().getMiddleRect() == value.getTexture().getMiddleRect()))
                    {
                        continue;
                    }

                    const tgui::String absoluteFilename1 = (tgui::getResourcePath() / tgui::Filesystem::Path(widgetPropertyValuePairs[property].getTexture().getId())).asString();
                    const tgui::String absoluteFilename2 = (tgui::getResourcePath() / tgui::Filesystem::Path(value.getTexture().getId())).asString();
                    if ((absoluteFilename1 == absoluteFilename2)
                     && (widgetPropertyValuePairs[property].getTexture().getMiddleRect() == value.getTexture().getMiddleRect()))
                    {
                        continue;
                    }
                }

                // Exception: Nested renderers need to check for the same exceptions
                if ((value.getType() == tgui::ObjectConverter::Type::RendererData)
                 && (widgetPropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::RendererData))
                {
                    if (compareRenderers(value.getRenderer()->propertyValuePairs,
                                         widgetPropertyValuePairs[property].getRenderer()->propertyValuePairs))
                    {
                        continue;
                    }
                }

                return false;
            }
        }

        for (auto& pair : widgetPropertyValuePairs)
        {
            const auto& property = pair.first;
            auto& value = pair.second;

            if (((themePropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::None)
              && (value.getType() != tgui::ObjectConverter::Type::None))
             || ((themePropertyValuePairs[property].getType() != tgui::ObjectConverter::Type::None)
              && (value.getType() == tgui::ObjectConverter::Type::None))
             || ((themePropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::String)
              && (value.getType() != tgui::ObjectConverter::Type::String)
              && (themePropertyValuePairs[property].getString() != value.getString()))
             || ((themePropertyValuePairs[property].getType() != tgui::ObjectConverter::Type::String)
              && (value.getType() == tgui::ObjectConverter::Type::String)
              && (themePropertyValuePairs[property].getString() != value.getString()))
             || ((themePropertyValuePairs[property].getType() != tgui::ObjectConverter::Type::String)
              && (value.getType() != tgui::ObjectConverter::Type::String)
              && (themePropertyValuePairs[property] != value)))
            {
                // Exception: An empty texture is considered the same as an empty property
                if ((themePropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::None)
                 && (value.getType() == tgui::ObjectConverter::Type::Texture)
                 && !value.getTexture().getData())
                {
                    continue;
                }

                // Exception: Textures need to be checked differently, but this is already handled in earlier check
                if ((themePropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::Texture)
                 && (value.getType() == tgui::ObjectConverter::Type::Texture)
                 && themePropertyValuePairs[property].getTexture().getData()
                 && value.getTexture().getData())
                {
                    continue;
                }

                // Exception: Renderers need to be checked differently, but this is already handled in earlier check
                if ((value.getType() == tgui::ObjectConverter::Type::RendererData)
                 && (themePropertyValuePairs[property].getType() == tgui::ObjectConverter::Type::RendererData))
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
            const tgui::String tguiDirName = U"tgui-" + tgui::String::fromNumber(TGUI_VERSION_MAJOR);
            const auto destDir = localDataDir / tguiDirName / U"gui-builder";
            if (tgui::Filesystem::directoryExists(destDir))
                guiBuilderDataDir = destDir;
            else if (createIfNotFound)
            {
                // Create the direcory structure if it doesn't exist yet
                tgui::Filesystem::createDirectory(localDataDir / tguiDirName);
                if (tgui::Filesystem::createDirectory(localDataDir / tguiDirName / U"gui-builder"))
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

#ifdef TGUI_SYSTEM_LINUX
    // When running the executable from the terminal, both the working directory and the program path via argv[0] may be
    // incorrect if no relative or absolute path was provided but the executable was instead found inside the PATH.
    // So if we can't find our 'resources' folder, we will ask the OS in which path our executable is located.
    if (!tgui::Filesystem::directoryExists(tgui::getResourcePath() / U"resources"))
    {
        char pathStr[FILENAME_MAX];
        const ssize_t pathLen = readlink("/proc/self/exe", pathStr, FILENAME_MAX);
        if (pathLen > 0)
        {
            const tgui::Filesystem::Path absoluteFilePath(tgui::String(pathStr, static_cast<std::size_t>(pathLen)));
            const tgui::Filesystem::Path dirContainingExe(absoluteFilePath.getParentPath());
            if (tgui::Filesystem::directoryExists(dirContainingExe / U"resources"))
                tgui::setResourcePath(dirContainingExe);
        }
    }
#endif

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
    m_widgetProperties["RichTextLabel"] = std::make_unique<RichTextLabelProperties>();
    m_widgetProperties["ScrollablePanel"] = std::make_unique<ScrollablePanelProperties>();
    m_widgetProperties["Scrollbar"] = std::make_unique<ScrollbarProperties>();
    m_widgetProperties["Slider"] = std::make_unique<SliderProperties>();
    m_widgetProperties["SpinButton"] = std::make_unique<SpinButtonProperties>();
    m_widgetProperties["SpinControl"] = std::make_unique<SpinControlProperties>();
    m_widgetProperties["Tabs"] = std::make_unique<TabsProperties>();
    m_widgetProperties["TextArea"] = std::make_unique<TextAreaProperties>();
    m_widgetProperties["ToggleButton"] = std::make_unique<ToggleButtonProperties>();
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
            bool passEventToGui = true;
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
                    {
                        m_selectedForm->arrowKeyPressed(event.key);
                        passEventToGui = false;
                    }
                }
                else if (event.key.code == tgui::Event::KeyboardKey::Delete)
                {
                    if (m_selectedForm && m_selectedForm->getSelectedWidget() && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                    {
                        removeSelectedWidget();
                        passEventToGui = false;
                    }
                }
                else if (event.key.code == tgui::Event::KeyboardKey::Escape)
                {
                    if (m_selectedForm && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                    {
                        m_selectedForm->selectParent();
                        passEventToGui = false;
                    }
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::S) && controlPressed)
                {
                    if (m_selectedForm)
                    {
                        m_selectedForm->save();
                        passEventToGui = false;
                    }
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::C) && controlPressed)
                {
                    if (m_selectedForm && m_selectedForm->getSelectedWidget() && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                    {
                        copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
                        passEventToGui = false;
                    }
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::V) && controlPressed)
                {
                    if (m_selectedForm && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                    {
                        pasteWidgetFromInternalClipboard();
                        passEventToGui = false;
                    }
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::X) && controlPressed)
                {
                    if (m_selectedForm && m_selectedForm->getSelectedWidget() && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused()))
                    {
                        copyWidgetToInternalClipboard(m_selectedForm->getSelectedWidget());
                        removeSelectedWidget();
                        passEventToGui = false;
                    }
                }
                else if ((event.key.code == tgui::Event::KeyboardKey::Z) && controlPressed)
                {
                    if (m_selectedForm && (m_selectedForm->hasFocus() || m_widgetHierarchyTree->isFocused() || m_propertiesWindow->isFocused()))
                    {
                        // Make sure no property field is still focused as removing the widgets causes
                        // an unfocus event to be fired. Instead we trigger unfocus here, before destroying things.
                        if (m_propertiesWindow->isFocused())
                            m_propertiesWindow->setFocused(false);

                        loadUndoState();
                        passEventToGui = false;
                    }
                }
            }

            if (passEventToGui)
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
    for (const auto& recentFile : m_recentFiles)
    {
        if (!tgui::Filesystem::fileExists(tgui::getResourcePath() / tgui::String(recentFile)))
            continue;

        if (recentFileList.empty())
            recentFileList = "[" + tgui::Serializer::serialize(recentFile);
        else
            recentFileList += ", " + tgui::Serializer::serialize(recentFile);
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
            [this,widget=selectedWidget.get()](const tgui::String& value){
                if (widget->name != value)
                {
                    saveUndoState(GuiBuilder::UndoType::PropertyEdit);
                    changeWidgetName(value);
                }
            });

        topPosition += 10;
        m_propertyValuePairs = m_widgetProperties.at(selectedWidget->ptr->getWidgetType())->initProperties(selectedWidget->ptr);
        for (const auto& property : m_propertyValuePairs.first)
        {
            addPropertyValueWidgets(topPosition, property,
                [this,propertyName=property.first](const tgui::String& value){
                    saveUndoState(GuiBuilder::UndoType::PropertyEdit);

                    if (updateWidgetProperty(propertyName, value))
                        m_selectedForm->setChanged(true);
                    else
                    {
                        // The change wasn't accepted, so we don't need the undo state which we stored before the change
                        m_undoSaves.pop_back();
                        m_undoSavesDesc.pop_back();
                    }
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
                    [this,propertyName=property.first,widgetPtr=selectedWidget->ptr.get()](const tgui::String& value){
                        saveUndoState(GuiBuilder::UndoType::PropertyEdit);

                        if (updateWidgetProperty(propertyName, value))
                        {
                            m_selectedForm->setChanged(true);

                            // The value shouldn't always be exactly as typed. An empty string may be understood correctly when setting the property,
                            // but is can't be saved to a widget file properly. So we read the back the property to have a valid string and pass it
                            // back to the widget, so that the string stored in the renderer is always a valid string.
                            m_widgetProperties.at(widgetPtr->getWidgetType())->updateProperty(
                                widgetPtr->shared_from_this(), propertyName, m_propertyValuePairs.second[propertyName].second);
                        }
                        else
                        {
                            // The change wasn't accepted, so we don't need the undo state which we stored before the change
                            m_undoSaves.pop_back();
                            m_undoSavesDesc.pop_back();
                        }
                    });
            }

            rendererComboBox->moveToFront();
        }
    }
    else // The form itself was selected
    {
        addPropertyValueWidgets(topPosition, {"Filename", {"String", m_selectedForm->getFilename()}},
            [this](const tgui::String& value){
                if (m_selectedForm->getFilename() != value)
                {
                    m_selectedForm->setChanged(true);
                    m_selectedForm->setFilename(value);
                    m_selectedWidgetComboBox->changeItemById("form", value);
                }
            });

        addPropertyValueWidgets(topPosition, {"Width", {"UInt", tgui::String::fromNumber(m_selectedForm->getSize().x)}},
            [this](const tgui::String& value){
                if (tgui::String::fromNumber(m_selectedForm->getSize().x) != value)
                {
                    // Form is not marked as changed since the width is saved as editor property
                    const float newWidth = value.toFloat();
                    m_formSize = {newWidth, m_selectedForm->getSize().y};
                    m_selectedForm->setSize(m_formSize);
                }
            });

        addPropertyValueWidgets(topPosition, {"Height", {"UInt", tgui::String::fromNumber(m_selectedForm->getSize().y)}},
            [this](const tgui::String& value){
                if (tgui::String::fromNumber(m_selectedForm->getSize().y) != value)
                {
                    // Form is not marked as changed since the height is saved as editor property
                    const float newHeight = value.toFloat();
                    m_formSize = {m_selectedForm->getSize().x, newHeight};
                    m_selectedForm->setSize(m_formSize);
                }
            });
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::widgetSelected(const tgui::Widget::Ptr& widget)
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
    messageBox->onButtonPress([this,form,&haltProgram,panelPtr=panel.get(),msgBoxPtr=messageBox.get()](const tgui::String& button){
        if (button == "Yes")
            m_selectedForm->save();

        m_gui->remove(panelPtr->shared_from_this());
        m_gui->remove(msgBoxPtr->shared_from_this());

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

void GuiBuilder::showLoadFileWindow(const tgui::String& title, const tgui::String& loadButtonCaption, bool allowCreateFolder, bool fileMustExist, const tgui::String& defaultFilename, const std::function<void(const tgui::String&)>& onLoad)
{
    auto fileDialog = tgui::FileDialog::create(title, loadButtonCaption, allowCreateFolder);
    fileDialog->setFileMustExist(fileMustExist);
    if (m_forms.empty())
        fileDialog->setPath((tgui::getResourcePath() / m_defaultPath).getNormalForm());
    else
        fileDialog->setPath((tgui::getResourcePath() / m_forms[0]->getFilename()).getParentPath().getNormalForm());

    fileDialog->setFilename(defaultFilename);
    openWindowWithFocus(fileDialog);

    fileDialog->onFileSelect([onLoad](const tgui::String& selectedFile){
        // We can't call onLoad here directly as it isn't allowed to destroy the file dialog during the callback.
        // Instead we schedule a timer to call the onLoad function on the next iteration of the main loop.
        tgui::Timer::scheduleCallback([onLoad, selectedFile]{ onLoad(selectedFile); });
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
        showLoadFileWindow("New form", "Create", true, false, getDefaultFilename(), [this](const tgui::String& filename){
            createNewForm(filename);
        });
    });
    panel->get<tgui::Panel>("PnlLoadForm")->onClick([this]{
        showLoadFileWindow("Load form", "Load", false, true, getDefaultFilename(), [this](const tgui::String& filename){ loadForm(filename); });
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
    m_selectedWidgetComboBox->onItemSelect([this](const tgui::String&, const tgui::String& id){ m_selectedForm->selectWidgetById(id); });

    m_menuBar = m_gui->get<tgui::MenuBar>("MenuBar");
    m_menuBar->onMouseEnter([menuBar=m_menuBar.get()]{ menuBar->moveToFront(); });
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
    m_widgetHierarchyTree->onItemSelect([this](const tgui::String& name){
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
        {"RichTextLabel", []{ return tgui::RichTextLabel::create("RichTextLabel"); }},
        {"ScrollablePanel", []{ return tgui::ScrollablePanel::create({150, 150}); }},
        {"Scrollbar", []{ return tgui::Scrollbar::create(); }},
        {"Slider", []{ return tgui::Slider::create(); }},
        {"SpinButton", []{ return tgui::SpinButton::create(); }},
        {"SpinControl", []{ return tgui::SpinControl::create(); }},
        {"Tabs", []{ auto tabs = tgui::Tabs::create(); tabs->add("Tab", false); return tabs; }},
        {"TextArea", []{ return tgui::TextArea::create(); }},
        {"ToggleButton", []{ return tgui::ToggleButton::create(); }},
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

        panel->onClick([this,widget]{
            saveUndoState(GuiBuilder::UndoType::CreateNew);

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

void GuiBuilder::createNewWidget(const tgui::Widget::Ptr& widget, tgui::Container* parent, bool selectNewWidget)
{
    if (!parent)
    {
        tgui::Widget::Ptr selectedWidget = m_selectedForm->getSelectedWidget() ? m_selectedForm->getSelectedWidget()->ptr : nullptr;
        if (selectedWidget && selectedWidget->isContainer())
            parent = dynamic_cast<tgui::Container*>(selectedWidget.get());
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

        rendererComboBox->onItemSelect([this,widget=selectedWidget.get()](const tgui::String& item){
            widget->theme = item;
            if (item != "Custom")
                widget->ptr->setRenderer(m_themes[item].getRendererNoThrow(widget->ptr->getWidgetType()));
            else
                widget->ptr->setRenderer(widget->ptr->getRenderer()->getData());

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
    else if (type == "Layout")
        addPropertyValueLayout(property, value, onChange, topPosition);
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
    else if (type == "ListViewColumns")
        addPropertyListViewColumns(property, value, onChange, topPosition);
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
    saveUndoState(GuiBuilder::UndoType::Delete);
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

        messageBox->onButtonPress([this,filename,panelPtr=panel.get(),messageBoxPtr=messageBox.get()](const tgui::String& button){
            m_gui->remove(panelPtr->shared_from_this());
            m_gui->remove(messageBoxPtr->shared_from_this());

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
// For load type, 0 for saved file load, 1 for undo save load
bool GuiBuilder::loadForm(tgui::String filename, bool loadingFromFile)
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
        if (loadingFromFile)
        {
            m_selectedForm->load();
            m_undoSaves.clear(); // Reset undo saves
            m_undoSavesDesc.clear(); // Reset undo saves
        }
        else
        {
            m_selectedForm->loadState(m_undoSaves.back());
            m_undoSaves.pop_back();
            m_undoSavesDesc.pop_back();
        }
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
    for (const auto& widget : m_selectedForm->getWidgets())
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

    messageBox->onButtonPress([messageBoxPtr=messageBox.get()]{ messageBoxPtr->close(); });
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
    auto closeWindow = [this,windowPtr=window.get(),panelPtr=panel.get(),tabUsageEnabled]{
        m_gui->remove(windowPtr->shared_from_this());
        m_gui->remove(panelPtr->shared_from_this());
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

void GuiBuilder::copyWidgetRecursive(std::vector<CopiedWidget>& copiedWidgetList, const std::shared_ptr<WidgetInfo>& widgetInfo)
{
    CopiedWidget copiedWidget;
    copiedWidget.theme = widgetInfo->theme;
    copiedWidget.widget = widgetInfo->ptr->clone();
    copiedWidget.originalWidget = widgetInfo->ptr;
    (void)copiedWidget.widget->getRenderer(); // Make sure renderer isn't still shared

    if (widgetInfo->ptr->isContainer())
    {
        const auto& container = widgetInfo->ptr->cast<tgui::Container>();
        if (container) // Should aways be the case
        {
            for (const auto& childWidget : container->getWidgets())
            {
                const auto& childWidgetInfo = m_selectedForm->getWidget(tgui::String::fromNumber(childWidget.get()));
                copyWidgetRecursive(copiedWidget.childWidgets, childWidgetInfo);
            }

            // Remove the widgets inside the container itself as we stored them separately
            copiedWidget.widget->cast<tgui::Container>()->removeAllWidgets();
        }
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

void GuiBuilder::copyWidgetToInternalClipboard(const std::shared_ptr<WidgetInfo>& widgetInfo)
{
    m_copiedWidgets.clear();
    copyWidgetRecursive(m_copiedWidgets, widgetInfo);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::pasteWidgetFromInternalClipboard()
{
    if (m_copiedWidgets.empty())
        return;

    saveUndoState(GuiBuilder::UndoType::Paste);

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
    valueEditBox->onReturnOrUnfocus([onChange,editBox=valueEditBox.get()]{ onChange(editBox->getText()); });

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

tgui::Button::Ptr GuiBuilder::addPropertyValueButtonLayoutRelAbs(const tgui::String& property, float topPosition, bool valueIsAbsolute)
{
    const float scrollbarWidth = m_propertiesContainer->getScrollbarWidth();

    auto buttonRelAbs = m_propertiesContainer->get<tgui::Button>("ValueButton" + property);
    if (!buttonRelAbs)
    {
        buttonRelAbs = tgui::Button::create();
        buttonRelAbs->setTextSize(9);
        buttonRelAbs->onFocus([this]{ m_propertiesContainer->focusNextWidget(); });
        m_propertiesContainer->add(buttonRelAbs, "ValueButton" + property);
    }

    buttonRelAbs->setText(valueIsAbsolute ? L"REL" : L"ABS");

    buttonRelAbs->onPress.disconnectAll();
    buttonRelAbs->setSize({EDIT_BOX_HEIGHT, EDIT_BOX_HEIGHT});
    buttonRelAbs->setPosition({bindWidth(m_propertiesContainer) - scrollbarWidth - EDIT_BOX_HEIGHT, topPosition});
    return buttonRelAbs;
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

    valueComboBox->onItemSelect([onChange,comboBox=valueComboBox.get()]{ onChange(comboBox->getSelectedItem()); });
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
    colorPreviewPanel->onClick([this,value,onChange]{
        const tgui::Color color = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Color, value).getColor();
        auto colorPicker = tgui::ColorPicker::create("Select color", color);
        openWindowWithFocus(colorPicker);
        colorPicker->onOkPress([onChange](tgui::Color newColor){
            onChange(tgui::Serializer::serialize(newColor));
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueLayout(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    const tgui::Layout layout(value);

    assert(property == U"Left" || property == U"Top" || property == U"Width" || property == U"Height");
    const bool isHorizontal = (property == U"Left") || (property == U"Width");

    const bool layoutIsConstant = layout.isConstant();
    const bool layoutIsPercentage = !layoutIsConstant
        && value.ends_with(U'%')
        && layout.getLeftOperand()
        && layout.getRightOperand()
        && layout.getLeftOperand()->isConstant()
        && !layout.getRightOperand()->isConstant()
        && !layout.getRightOperand()->getLeftOperand()
        && !layout.getRightOperand()->getRightOperand();

    if (layoutIsConstant || layoutIsPercentage)
        addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);
    else
        addPropertyValueEditBox(property, value, onChange, topPosition, 0);

    if (layoutIsConstant)
    {
        const float layoutValue = layout.getValue();

        auto buttonRel = addPropertyValueButtonLayoutRelAbs(property, topPosition, true);
        buttonRel->onPress([this,isHorizontal,layoutValue,onChange]{
            const auto selectedWidget = m_selectedForm->getSelectedWidget();
            if (!selectedWidget || !selectedWidget->ptr || !selectedWidget->ptr->getParent())
                return;

            const auto parent = selectedWidget->ptr->getParent();
            const float parentSize = isHorizontal ? parent->getInnerSize().x : parent->getInnerSize().y;
            if (parentSize <= 0)
                return;

            const float percentage = layoutValue / parentSize * 100;
            onChange(tgui::String::fromNumberRounded(percentage, 2) + U"%");
        });
    }
    else if (layoutIsPercentage)
    {
        const float ratio = layout.getLeftOperand()->getValue();

        auto buttonAbs = addPropertyValueButtonLayoutRelAbs(property, topPosition, false);
        buttonAbs->onPress([this,isHorizontal,ratio,onChange]{
            const auto selectedWidget = m_selectedForm->getSelectedWidget();
            if (!selectedWidget || !selectedWidget->ptr || !selectedWidget->ptr->getParent())
                return;

            const auto parent = selectedWidget->ptr->getParent();
            const float parentSize = isHorizontal ? parent->getInnerSize().x : parent->getInnerSize().y;
            if (parentSize <= 0)
                return;

            const float absoluteSize = ratio * parentSize;
            onChange(tgui::String::fromNumberRounded(absoluteSize, 0));
        });
    }
    else // Layout is complex and no conversion button should be shown
    {
        auto buttonRelAbs = m_propertiesContainer->get<tgui::Button>("ValueButton" + property);
        if (buttonRelAbs)
            m_propertiesContainer->remove(buttonRelAbs);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueTextStyle(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress([this,value,onChange]{
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

        auto updateTextStyleProperty = [onChange,cbBold=checkBoxBold.get(),cbItalic=checkBoxItalic.get(),
                                        cbUnderlined=checkBoxUnderlined.get(),cbStrikeThrough=checkBoxStrikeThrough.get()]{
            unsigned int newStyle = 0;
            if (cbBold->isChecked())
                newStyle |= tgui::TextStyle::Bold;
            if (cbItalic->isChecked())
                newStyle |= tgui::TextStyle::Italic;
            if (cbUnderlined->isChecked())
                newStyle |= tgui::TextStyle::Underlined;
            if (cbStrikeThrough->isChecked())
                newStyle |= tgui::TextStyle::StrikeThrough;
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
    buttonMore->onPress([this,value,onChange]{
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

        auto updateOutlineProperty = [onChange,ebLeft=editLeft.get(),ebTop=editTop.get(),ebRight=editRight.get(),ebBottom=editBottom.get()]{
            const tgui::Outline newOutline{
                tgui::AbsoluteOrRelativeValue{ebLeft->getText()},
                tgui::AbsoluteOrRelativeValue{ebTop->getText()},
                tgui::AbsoluteOrRelativeValue{ebRight->getText()},
                tgui::AbsoluteOrRelativeValue{ebBottom->getText()},
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
    buttonMore->onPress([this,value,onChange]{
        auto multilineStringWindow = openWindowWithFocus();
        multilineStringWindow->setTitle("Set multiline text");
        multilineStringWindow->setClientSize({470, 220});
        multilineStringWindow->loadWidgetsFromFile("resources/forms/SetMultilineString.txt");

        auto textArea = multilineStringWindow->get<tgui::TextArea>("TextArea");
        textArea->setText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());
        textArea->onTextChange([onChange,textAreaPtr=textArea.get()]{ onChange(tgui::Serializer::serialize(textAreaPtr->getText())); });
        textArea->setFocused(true);

        m_gui->setTabKeyUsageEnabled(false);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueStringList(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto setArrowColor = [](const tgui::BitmapButton::Ptr& button, const tgui::Color& color){
        tgui::Texture texture = button->getImage();
        texture.setColor(color);
        button->setImage(texture);
    };

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress([this,value,onChange,setArrowColor]{
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

        listBox->onItemSelect([setArrowColor,lb=listBox.get(),eb=editBox.get(),btnRemove=buttonRemove.get(),btnUp=buttonArrowUp.get(),btnDown=buttonArrowDown.get()]{
            const int index = lb->getSelectedItemIndex();
            if (index >= 0)
            {
                btnRemove->setEnabled(true);
                eb->setText(lb->getSelectedItem());
            }
            else
                btnRemove->setEnabled(false);

            if (index > 0)
            {
                setArrowColor(btnUp->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColor());
                btnUp->setEnabled(true);
            }
            else
            {
                setArrowColor(btnUp->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColorDisabled());
                btnUp->setEnabled(false);
            }

            if ((index >= 0) && (static_cast<std::size_t>(index) + 1 < lb->getItemCount()))
            {
                setArrowColor(btnDown->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColor());
                btnDown->setEnabled(true);
            }
            else
            {
                setArrowColor(btnDown->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColorDisabled());
                btnDown->setEnabled(false);
            }
        });

        auto updateValue = [onChange,lb=listBox.get()]{
            onChange(WidgetProperties::serializeList(lb->getItems()));
        };

        buttonArrowUp->onPress([updateValue,lb=listBox.get()]{
            const std::size_t index = static_cast<std::size_t>(lb->getSelectedItemIndex());
            tgui::String value1 = lb->getItemByIndex(index - 1);
            tgui::String value2 = lb->getItemByIndex(index);
            lb->changeItemByIndex(index - 1, value2);
            lb->changeItemByIndex(index, value1);
            lb->setSelectedItemByIndex(index - 1);
            updateValue();
        });

        buttonArrowDown->onPress([updateValue,lb=listBox.get()]{
            const std::size_t index = static_cast<std::size_t>(lb->getSelectedItemIndex());
            tgui::String value1 = lb->getItemByIndex(index);
            tgui::String value2 = lb->getItemByIndex(index + 1);
            lb->changeItemByIndex(index, value2);
            lb->changeItemByIndex(index + 1, value1);
            lb->setSelectedItemByIndex(index + 1);
            updateValue();
        });

        buttonRemove->onPress([updateValue,lb=listBox.get()]{
            const std::size_t index = static_cast<std::size_t>(lb->getSelectedItemIndex());
            lb->removeItemByIndex(index);
            if (lb->getItemCount() > 0)
            {
                if (index == lb->getItemCount())
                    lb->setSelectedItemByIndex(index - 1);
                else
                    lb->setSelectedItemByIndex(index);
            }

            updateValue();
        });

        auto addItem = [updateValue,lb=listBox.get(),eb=editBox.get()]{
            lb->addItem(eb->getText());
            lb->setSelectedItemByIndex(lb->getItemCount() - 1);
            eb->setText("");
            eb->setFocused(true);
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
    buttonMore->onPress([this,value,onChange]{
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

        auto separators = std::make_shared<std::vector<tgui::SeparatorLine*>>();

        auto updateForm = [this,onChange,separators,window=textureWindow.get(),picPreview=previewPicture.get(),btnSelect=buttonSelectFile.get(),
                           ebPart=editBoxPartRect.get(),ebMiddle=editBoxMiddleRect.get(),cbSmooth=checkBoxSmooth.get()]
                          (const tgui::String& filename, tgui::UIntRect partRect, tgui::UIntRect middleRect,
                           bool smooth, bool resetPartRect, bool resetMiddleRect, bool resetSmooth){
            auto texture = picPreview->getUserData<std::shared_ptr<tgui::Texture>>();

            try
            {
                texture->load(filename, partRect, middleRect, smooth);
                onChange(tgui::Serializer::serialize(*texture));
            }
            catch (tgui::Exception&)
            {
            }

            btnSelect->setUserData(filename); // Not using texture.getId() as it would be empty if file didn't exist

            const tgui::Vector2u imageSize = texture->getImageSize();
            const tgui::Vector2f extraSpace{20, 180};
            const tgui::Vector2f minSize{235, 165};
            const tgui::Layout2d maxSize{tgui::bindWidth(*m_gui) - 50, tgui::bindHeight(*m_gui) - 50};
            const tgui::Layout scaling = tgui::bindMin(1.f, tgui::bindMin((maxSize.x - extraSpace.x) / imageSize.x, (maxSize.y - extraSpace.y) / imageSize.y));
            picPreview->setSize({imageSize.x * scaling, imageSize.y * scaling});
            window->setSize({tgui::bindMax(minSize.x, (imageSize.x * scaling) + extraSpace.x), tgui::bindMax(minSize.y, (imageSize.y * scaling) + extraSpace.y)});

            if (resetPartRect)
            {
                partRect = texture->getPartRect();
                ebPart->onTextChange.setEnabled(false);
                ebPart->setText("(" + tgui::String::fromNumber(partRect.left) + ", " + tgui::String::fromNumber(partRect.top)
                    + ", " + tgui::String::fromNumber(partRect.width) + ", " + tgui::String::fromNumber(partRect.height) + ")");
                ebPart->onTextChange.setEnabled(true);
            }

            if (resetMiddleRect)
            {
                middleRect = texture->getMiddleRect();

                ebMiddle->onTextChange.setEnabled(false);
                ebMiddle->setText("(" + tgui::String::fromNumber(middleRect.left) + ", " + tgui::String::fromNumber(middleRect.top)
                    + ", " + tgui::String::fromNumber(middleRect.width) + ", " + tgui::String::fromNumber(middleRect.height) + ")");
                ebMiddle->onTextChange.setEnabled(true);
            }

            if (resetSmooth)
            {
                cbSmooth->onChange.setEnabled(false);
                cbSmooth->setChecked(texture->isSmooth());
                cbSmooth->onChange.setEnabled(true);
            }

            picPreview->getRenderer()->setTexture(*texture);

            for (auto& separator : *separators)
                window->remove(separator->shared_from_this());
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
                    separator->setPosition(picPreview->getPosition() + line.first);
                    separator->setSize(std::max(line.second.x - line.first.x, 1.f), std::max(line.second.y - line.first.y, 1.f));
                    separator->getRenderer()->setColor({255, 128, 255});
                    window->add(separator);
                    separators->push_back(separator.get());
                }
            }
        };

        editBoxPartRect->onTextChange([=,btnSel=buttonSelectFile.get(),ebPart=editBoxPartRect.get(),cbSmooth=checkBoxSmooth.get()]{
            updateForm(btnSel->getUserData<tgui::String>(), deserializeRect(ebPart->getText()),
                       {}, cbSmooth->isChecked(), false, true, false);
        });
        editBoxMiddleRect->onTextChange([=,btnSel=buttonSelectFile.get(),ebPart=editBoxPartRect.get(),ebMiddle=editBoxMiddleRect.get(),cbSmooth=checkBoxSmooth.get()]{
            updateForm(btnSel->getUserData<tgui::String>(), deserializeRect(ebPart->getText()),
                       deserializeRect(ebMiddle->getText()), cbSmooth->isChecked(), false, false, false);
        });
        checkBoxSmooth->onChange([=,btnSel=buttonSelectFile.get(),ebPart=editBoxPartRect.get(),ebMiddle=editBoxMiddleRect.get(),cbSmooth=checkBoxSmooth.get()]{
            updateForm(btnSel->getUserData<tgui::String>(), deserializeRect(ebPart->getText()),
                       deserializeRect(ebMiddle->getText()), cbSmooth->isChecked(), false, false, false);
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

        buttonSelectFile->onPress([this,updateForm,btnSelect=buttonSelectFile.get(),cbSmooth=checkBoxSmooth.get()]{
            showLoadFileWindow("Load image", "Load", false, true, btnSelect->getUserData<tgui::String>(),
                [updateForm,cbSmooth](const tgui::String& filename){
                    updateForm(filename, {}, {}, cbSmooth->isChecked(), true, true, false);
                });
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyListViewColumns(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto setArrowColor = [](const tgui::BitmapButton::Ptr& button, const tgui::Color& color){
        tgui::Texture texture = button->getImage();
        texture.setColor(color);
        button->setImage(texture);
    };

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress([this,value,onChange,setArrowColor]{
        auto stringListWindow = openWindowWithFocus();
        stringListWindow->setTitle("Set columns");
        stringListWindow->setClientSize({490, 372});
        stringListWindow->loadWidgetsFromFile("resources/forms/SetListViewColumns.txt");

        auto listView = stringListWindow->get<tgui::ListView>("ListView");
        auto editCaption = stringListWindow->get<tgui::EditBox>("EditCaption");
        auto spinWidth = stringListWindow->get<tgui::SpinControl>("SpinWidth");
        auto comboBoxAlignment = stringListWindow->get<tgui::ComboBox>("ComboBoxAlignment");
        auto checkBoxAutoResize = stringListWindow->get<tgui::CheckBox>("CheckBoxAutoResize");
        auto checkBoxExpanded = stringListWindow->get<tgui::CheckBox>("CheckBoxExpanded");
        auto buttonAdd = stringListWindow->get<tgui::Button>("BtnAdd");
        auto buttonReplace = stringListWindow->get<tgui::Button>("BtnReplace");
        auto buttonRemove = stringListWindow->get<tgui::Button>("BtnRemove");
        auto buttonArrowUp = stringListWindow->get<tgui::BitmapButton>("BtnArrowUp");
        auto buttonArrowDown = stringListWindow->get<tgui::BitmapButton>("BtnArrowDown");

        auto serializedColumns = WidgetProperties::deserializeList(value);
        for (const auto& serializedColumn : serializedColumns)
        {
            tgui::String text;
            float width;
            tgui::ListView::ColumnAlignment alignment;
            bool autoResize;
            bool expanded;
            if (ListViewProperties::deserializeColumn(serializedColumn, text, width, alignment, autoResize, expanded))
            {
                listView->addItem({text,
                                   tgui::Serializer::serialize(width),
                                   ListViewProperties::serializeColumnAlignment(alignment),
                                   tgui::Serializer::serialize(autoResize),
                                   tgui::Serializer::serialize(expanded)});
            }
        }

        setArrowColor(buttonArrowUp, buttonArrowUp->getSharedRenderer()->getTextColorDisabled());
        setArrowColor(buttonArrowDown, buttonArrowDown->getSharedRenderer()->getTextColorDisabled());
        buttonRemove->setEnabled(false);
        buttonArrowUp->setEnabled(false);
        buttonArrowDown->setEnabled(false);

        listView->onItemSelect([setArrowColor,
                                lv=listView.get(),
                                ebCaption=editCaption.get(),
                                scWidth=spinWidth.get(),
                                cbAlign=comboBoxAlignment.get(),
                                cbxAutoResize=checkBoxAutoResize.get(),
                                cbxExpanded=checkBoxExpanded.get(),
                                btnReplace=buttonReplace.get(),
                                btnRemove=buttonRemove.get(),
                                btnUp=buttonArrowUp.get(),
                                btnDown=buttonArrowDown.get()
                               ]
        {
            const int index = lv->getSelectedItemIndex();
            if (index >= 0)
            {
                btnRemove->setEnabled(true);
                btnReplace->setEnabled(true);

                const std::vector<tgui::String>& selectedItem = lv->getItemRow(static_cast<std::size_t>(index));
                ebCaption->setText(selectedItem[0]);
                scWidth->setValue(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, selectedItem[1]).getNumber());
                cbAlign->setSelectedItem(selectedItem[2]);
                cbxAutoResize->setChecked(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Bool, selectedItem[3]).getBool());
                cbxExpanded->setChecked(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Bool, selectedItem[4]).getBool());
            }
            else
            {
                btnRemove->setEnabled(false);
                btnReplace->setEnabled(false);
            }

            if (index > 0)
            {
                setArrowColor(btnUp->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColor());
                btnUp->setEnabled(true);
            }
            else
            {
                setArrowColor(btnUp->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColorDisabled());
                btnUp->setEnabled(false);
            }

            if ((index >= 0) && (static_cast<std::size_t>(index) + 1 < lv->getItemCount()))
            {
                setArrowColor(btnDown->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColor());
                btnDown->setEnabled(true);
            }
            else
            {
                setArrowColor(btnDown->cast<tgui::BitmapButton>(), btnUp->getSharedRenderer()->getTextColorDisabled());
                btnDown->setEnabled(false);
            }
        });

        auto updateValue = [onChange,lv=listView.get()]{
            std::vector<tgui::String> newSerializedColumns;
            for (std::size_t i = 0; i < lv->getItemCount(); ++i)
            {
                const std::vector<tgui::String> item = lv->getItemRow(i);
                newSerializedColumns.emplace_back('(' + tgui::Serializer::serialize(item[0])
                    + ',' + item[1] + ',' + item[2] + ',' + item[3] + ',' + item[4] + ')');
            }
            onChange(WidgetProperties::serializeList(newSerializedColumns));
        };

        buttonArrowUp->onPress([updateValue,lv=listView.get()]{
            const std::size_t index = static_cast<std::size_t>(lv->getSelectedItemIndex());
            std::vector<tgui::String> value1 = lv->getItemRow(index - 1);
            std::vector<tgui::String> value2 = lv->getItemRow(index);
            lv->changeItem(index - 1, value2);
            lv->changeItem(index, value1);
            lv->setSelectedItem(index - 1);
            updateValue();
        });

        buttonArrowDown->onPress([updateValue,lv=listView.get()]{
            const std::size_t index = static_cast<std::size_t>(lv->getSelectedItemIndex());
            std::vector<tgui::String> value1 = lv->getItemRow(index);
            std::vector<tgui::String> value2 = lv->getItemRow(index + 1);
            lv->changeItem(index, value2);
            lv->changeItem(index + 1, value1);
            lv->setSelectedItem(index + 1);
            updateValue();
        });

        buttonRemove->onPress([updateValue,lv=listView.get()]{
            const std::size_t index = static_cast<std::size_t>(lv->getSelectedItemIndex());
            lv->removeItem(index);
            if (lv->getItemCount() > 0)
            {
                if (index == lv->getItemCount())
                    lv->setSelectedItem(index - 1);
                else
                    lv->setSelectedItem(index);
            }

            updateValue();
        });

        buttonReplace->onPress([updateValue,
                                lv=listView.get(),
                                ebCaption=editCaption.get(),
                                scWidth=spinWidth.get(),
                                cbAlign=comboBoxAlignment.get(),
                                cbxAutoResize=checkBoxAutoResize.get(),
                                cbxExpanded=checkBoxExpanded.get()]
        {
            const std::size_t index = static_cast<std::size_t>(lv->getSelectedItemIndex());
            lv->changeItem(index, {ebCaption->getText(),
                                   tgui::Serializer::serialize(scWidth->getValue()),
                                   cbAlign->getSelectedItem(),
                                   tgui::Serializer::serialize(cbxAutoResize->isChecked()),
                                   tgui::Serializer::serialize(cbxExpanded->isChecked())});
            updateValue();
        });

        auto addItem = [updateValue,
                        lv=listView.get(),
                        ebCaption=editCaption.get(),
                        scWidth=spinWidth.get(),
                        cbAlign=comboBoxAlignment.get(),
                        cbxAutoResize=checkBoxAutoResize.get(),
                        cbxExpanded=checkBoxExpanded.get()]
        {
            lv->addItem({ebCaption->getText(),
                         tgui::Serializer::serialize(scWidth->getValue()),
                         cbAlign->getSelectedItem(),
                         tgui::Serializer::serialize(cbxAutoResize->isChecked()),
                         tgui::Serializer::serialize(cbxExpanded->isChecked())});
            lv->setSelectedItem(lv->getItemCount() - 1);
            updateValue();
        };
        buttonAdd->onPress(addItem);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::addPropertyValueEditBoxInputValidator(const tgui::String& property, const tgui::String& value, const OnValueChangeFunc& onChange, float topPosition)
{
    addPropertyValueEditBox(property, value, onChange, topPosition, EDIT_BOX_HEIGHT - 1);

    auto buttonMore = addPropertyValueButtonMore(property, topPosition);
    buttonMore->onPress([this,value,onChange]{
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

        auto updateCustomValidator = [onChange,cbCustom=checkCustom.get(),ebValid=editValidator.get()]{
            cbCustom->setChecked(true);
            onChange(ebValid->getText());
        };
        auto updateValidator = [onChange,updateCustomValidator,ebValid=editValidator.get()](const tgui::String& newValue){
            ebValid->onReturnKeyPress.disconnectAll();
            ebValid->onUnfocus.disconnectAll();
            ebValid->setText(newValue);
            ebValid->onReturnOrUnfocus(updateCustomValidator);
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
    buttonMore->onPress([this,value,onChange]{
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

        auto updateTitleButtons = [onChange,cbMin=checkMinimize.get(),cbMax=checkMaximize.get(),cbClose=checkClose.get()]{
            tgui::String newValue;
            if (cbMin->isChecked())
                newValue += " | Minimize";
            if (cbMax->isChecked())
                newValue += " | Maximize";
            if (cbClose->isChecked())
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

    valueComboBox->onItemSelect([onChange,comboBox=valueComboBox.get()]{ onChange(comboBox->getSelectedItem()); });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackNewForm()
{
    loadStartScreen();

    showLoadFileWindow("New form", "Create", true, false, getDefaultFilename(), [this](const tgui::String& filename){
        createNewForm(filename);
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackLoadForm()
{
    loadStartScreen();

    showLoadFileWindow("Load form", "Load", false, true, getDefaultFilename(), [this](const tgui::String& filename){
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

    themesList->onItemSelect([btnDel=buttonDelete.get(),ebNew=newThemeEditBox.get()](const tgui::String& item){
        if (item.empty())
            btnDel->setEnabled(false);
        else
            btnDel->setEnabled(true);

        ebNew->setText(item);
    });

    newThemeEditBox->onTextChange([btnAdd=buttonAdd.get()](const tgui::String& text){
        if (text.empty())
            btnAdd->setEnabled(false);
        else
            btnAdd->setEnabled(true);
    });

    buttonAdd->onPress([this,ebNew=newThemeEditBox.get(),lbThemes=themesList.get()]{
        try
        {
            const tgui::String filename = ebNew->getText();
            if (!lbThemes->contains(filename))
            {
                tgui::Theme theme{(tgui::getResourcePath() / filename).asString()};
                lbThemes->addItem(filename);
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

    buttonDelete->onPress([this,btnDel=buttonDelete.get(),lbThemes=themesList.get()]{
        auto item = lbThemes->getSelectedItem();
        m_themes.erase(item);
        lbThemes->removeItem(item);
        btnDel->setEnabled(false);
        initProperties();
        saveGuiBuilderState();
    });

    buttonBrowse->onPress([this,ebNew=newThemeEditBox.get()]{
        auto fileDialog = tgui::FileDialog::create("Select theme file", "Select");
        fileDialog->setFileMustExist(true);

        if (!ebNew->getText().empty())
        {
            const auto inputPath = tgui::Filesystem::Path(ebNew->getText());
            fileDialog->setPath((tgui::getResourcePath() / inputPath).getParentPath().getNormalForm());
            fileDialog->setFilename(inputPath.getFilename());
        }
        openWindowWithFocus(fileDialog);

        fileDialog->onFileSelect([ebNew](const tgui::String& selectedFile){
            ebNew->setText(selectedFile);
        });
    });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackBringWidgetToFront()
{
    saveUndoState(GuiBuilder::UndoType::SendtoFront);
    m_selectedForm->getSelectedWidget()->ptr->moveToFront();
    m_selectedForm->setChanged(true);

    widgetHierarchyChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GuiBuilder::menuBarCallbackSendWidgetToBack()
{
    saveUndoState(GuiBuilder::UndoType::SendtoBack);
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
                hierarchy.reserve(nodeList.size());
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

void GuiBuilder::fillWidgetHierarchyTreeRecursively(std::vector<tgui::String>& hierarchy, const std::shared_ptr<tgui::Widget>& parentWidget)
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

    auto it = std::find_if(widgets.begin(), widgets.end(), [widget](const std::shared_ptr<WidgetInfo>& otherWidget){
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
    keyboardShortcutsWindow->setClientSize({375, 340});
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Save state to program interal memory for undo command
void GuiBuilder::saveUndoState(GuiBuilder::UndoType type)
{
    tgui::String descString;
    switch (type)
    {
    case GuiBuilder::UndoType::Delete:
        descString = "Delete";
        break;
    case GuiBuilder::UndoType::Move:
        descString = "Move";
        break;
    case GuiBuilder::UndoType::Resize:
        descString = "Resize";
        break;
    case GuiBuilder::UndoType::Paste:
        descString = "Paste";
        break;
    case GuiBuilder::UndoType::SendtoBack:
        descString = "Send to Back";
        break;
    case GuiBuilder::UndoType::SendtoFront:
        descString = "Send to Front";
        break;
    case GuiBuilder::UndoType::CreateNew:
        descString = "Create New";
        break;
    case GuiBuilder::UndoType::PropertyEdit:
        descString = "Property Edit";
        break;
    }

    // Starts deleting beginning history of saved states if > max ammount set to prevent overflow or excess memory usage
    if (m_undoSaves.size() >= UNDO_MAX_SAVES)
    {
        m_undoSaves.erase(m_undoSaves.begin());
        m_undoSavesDesc.erase(m_undoSavesDesc.begin());
    }

    // Save state and desc of saved state
    m_undoSaves.push_back(m_selectedForm->saveState());
    m_undoSavesDesc.push_back(descString);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Clear page and load previous state
void GuiBuilder::loadUndoState()
{
    if (m_undoSaves.empty())
        return;

    tgui::String selectedWidgetName;
    const auto formSize = m_selectedForm->getSize();
    const auto selectedWidget = m_selectedForm->getSelectedWidget();
    if (selectedWidget)
        selectedWidgetName = selectedWidget->name;

    const tgui::String filename = m_selectedForm->getFilename();
    m_selectedForm = nullptr;
    m_forms.clear();

    m_propertiesWindow = nullptr;
    m_propertiesContainer = nullptr;
    m_selectedWidgetComboBox = nullptr;

    m_gui->removeAllWidgets();
    m_gui->loadWidgetsFromFile("resources/forms/StartScreen.txt");

    if (!loadGuiBuilderState())
    {
        m_themes["themes/Black.txt"] = { (tgui::getResourcePath() / "themes/Black.txt").asString() };
        m_themes["themes/BabyBlue.txt"] = { (tgui::getResourcePath() / "themes/BabyBlue.txt").asString() };
        m_themes["themes/TransparentGrey.txt"] = { (tgui::getResourcePath() / "themes/TransparentGrey.txt").asString() };
    }

    loadForm(filename, false);
    m_selectedForm->focus();
    m_selectedForm->setSize(formSize);
    m_selectedForm->setChanged(true);

    if (!selectedWidgetName.empty())
        m_selectedForm->selectWidgetByName(selectedWidgetName);
}
