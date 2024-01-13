/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "Tests.hpp"

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/FileDialogIconLoader.hpp>
#endif

TEST_CASE("[FileDialog]")
{
    tgui::FileDialog::Ptr dialog = tgui::FileDialog::create();
    dialog->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        dialog->onFileSelect([](){});
        dialog->onFileSelect([](const tgui::String&){});
        dialog->onFileSelect([](const tgui::Filesystem::Path&){});
        dialog->onFileSelect([](const std::vector<tgui::Filesystem::Path>&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(dialog)->getSignal("FileSelected").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(dialog->getWidgetType() == "FileDialog");
    }

    SECTION("Path")
    {
        SECTION("String")
        {
            dialog->setPath("/bin");
        }

        SECTION("Path object")
        {
            dialog->setPath(tgui::Filesystem::Path("/bin"));
        }

        REQUIRE(dialog->getPath() == tgui::Filesystem::Path("/bin"));
    }

    SECTION("Filename")
    {
        REQUIRE(dialog->getFilename() == "");
        dialog->setFilename("Config.txt");
        REQUIRE(dialog->getFilename() == "Config.txt");
    }

    SECTION("FileTypeFilters")
    {
        REQUIRE(dialog->getFileTypeFiltersIndex() == 0);
        REQUIRE(dialog->getFileTypeFilters().size() == 1);
        REQUIRE(dialog->getFileTypeFilters()[0].first == "All files (*)");
        REQUIRE(dialog->getFileTypeFilters()[0].second.empty());

        dialog->setFileTypeFilters({
            {"Single", {"*.txt"}},
            {"Multiple", {"*.png", "*.jpg"}},
            {"Everything", {}}
        }, 1);

        REQUIRE(dialog->getFileTypeFiltersIndex() == 1);
        REQUIRE(dialog->getFileTypeFilters().size() == 3);
        REQUIRE(dialog->getFileTypeFilters()[0].first == "Single");
        REQUIRE(dialog->getFileTypeFilters()[0].second.size() == 1);
        REQUIRE(dialog->getFileTypeFilters()[0].second[0] == "*.txt");
        REQUIRE(dialog->getFileTypeFilters()[1].first == "Multiple");
        REQUIRE(dialog->getFileTypeFilters()[1].second.size() == 2);
        REQUIRE(dialog->getFileTypeFilters()[1].second[0] == "*.png");
        REQUIRE(dialog->getFileTypeFilters()[1].second[1] == "*.jpg");
        REQUIRE(dialog->getFileTypeFilters()[2].first == "Everything");
        REQUIRE(dialog->getFileTypeFilters()[2].second.empty());
    }

    SECTION("Captions")
    {
        dialog->setConfirmButtonText("Use that file");
        dialog->setCancelButtonText("Don't do it!");
        dialog->setFilenameLabelText("What file?");
        dialog->setListViewColumnCaptions("Moniker", "Largeness", "Timing");

        REQUIRE(dialog->getConfirmButtonText() == "Use that file");
        REQUIRE(dialog->getCancelButtonText() == "Don't do it!");
        REQUIRE(dialog->getFilenameLabelText() == "What file?");
        REQUIRE(dialog->getListViewColumnCaptions() == std::make_tuple("Moniker", "Largeness", "Timing"));
    }

    SECTION("Options")
    {
        REQUIRE(dialog->getFileMustExist());
        dialog->setFileMustExist(false);
        REQUIRE(!dialog->getFileMustExist());
        dialog->setFileMustExist(true);
        REQUIRE(dialog->getFileMustExist());

        REQUIRE(!dialog->getSelectingDirectory());
        dialog->setSelectingDirectory(true);
        REQUIRE(dialog->getSelectingDirectory());
        dialog->setSelectingDirectory(false);
        REQUIRE(!dialog->getSelectingDirectory());

        REQUIRE(!dialog->getMultiSelect());
        dialog->setMultiSelect(true);
        REQUIRE(dialog->getMultiSelect());
        dialog->setMultiSelect(false);
        REQUIRE(!dialog->getMultiSelect());
    }

    SECTION("Icon loader")
    {
        class CustomIconLoader : public tgui::FileDialogIconLoader {};
        auto iconLoader = std::make_shared<CustomIconLoader>();

        REQUIRE(dialog->getIconLoader() != nullptr);
        REQUIRE(dialog->getIconLoader() != iconLoader);

        dialog->setIconLoader(iconLoader);
        REQUIRE(dialog->getIconLoader() == iconLoader);
    }

    testWidgetRenderer(dialog->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = dialog->getRenderer();

        tgui::LabelRenderer filenameLabelRenderer;
        filenameLabelRenderer.setBackgroundColor(tgui::Color::Red);

        tgui::EditBoxRenderer editBoxRenderer;
        editBoxRenderer.setBackgroundColor(tgui::Color::Green);

        tgui::ComboBoxRenderer fileTypeComboBoxRenderer;
        fileTypeComboBoxRenderer.setBackgroundColor(tgui::Color::Blue);

        tgui::ListViewRenderer listViewRenderer;
        listViewRenderer.setBackgroundColor(tgui::Color::Yellow);

        tgui::ButtonRenderer buttonsRenderer;
        buttonsRenderer.setBackgroundColor(tgui::Color::Cyan);

        tgui::ButtonRenderer backButtonRenderer;
        backButtonRenderer.setBackgroundColor(tgui::Color::Magenta);

        tgui::ButtonRenderer forwardButtonRenderer;
        forwardButtonRenderer.setBackgroundColor(tgui::Color::Black);

        tgui::ButtonRenderer upButtonRenderer;
        upButtonRenderer.setBackgroundColor(tgui::Color::White);

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(255, 99, 71)"));
            REQUIRE_NOTHROW(renderer->setProperty("ArrowsOnNavigationButtonsVisible", "false"));
            REQUIRE_NOTHROW(renderer->setProperty("FilenameLabel", "{ BackgroundColor = Red; }"));
            REQUIRE_NOTHROW(renderer->setProperty("EditBox", "{ BackgroundColor = Green; }"));
            REQUIRE_NOTHROW(renderer->setProperty("FileTypeComboBox", "{ BackgroundColor = Blue; }"));
            REQUIRE_NOTHROW(renderer->setProperty("ListView", "{ BackgroundColor = Yellow; }"));
            REQUIRE_NOTHROW(renderer->setProperty("Button", "{ BackgroundColor = Cyan; }"));
            REQUIRE_NOTHROW(renderer->setProperty("BackButton", "{ BackgroundColor = Magenta; }"));
            REQUIRE_NOTHROW(renderer->setProperty("ForwardButton", "{ BackgroundColor = Black; }"));
            REQUIRE_NOTHROW(renderer->setProperty("UpButton", "{ BackgroundColor = White; }"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{255, 99, 71}));
            REQUIRE_NOTHROW(renderer->setProperty("ArrowsOnNavigationButtonsVisible", false));
            REQUIRE_NOTHROW(renderer->setProperty("FilenameLabel", filenameLabelRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("EditBox", editBoxRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("FileTypeComboBox", fileTypeComboBoxRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("ListView", listViewRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("Button", buttonsRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("BackButton", backButtonRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("ForwardButton", forwardButtonRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("UpButton", upButtonRenderer.getData()));
        }

        SECTION("functions")
        {
            renderer->setBackgroundColor({255, 99, 71});
            renderer->setArrowsOnNavigationButtonsVisible(false);
            renderer->setFilenameLabel(filenameLabelRenderer.getData());
            renderer->setEditBox(editBoxRenderer.getData());
            renderer->setFileTypeComboBox(fileTypeComboBoxRenderer.getData());
            renderer->setListView(listViewRenderer.getData());
            renderer->setButton(buttonsRenderer.getData());
            renderer->setBackButton(backButtonRenderer.getData());
            renderer->setForwardButton(forwardButtonRenderer.getData());
            renderer->setUpButton(upButtonRenderer.getData());
        }

        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(255, 99, 71));
        REQUIRE(!renderer->getProperty("ArrowsOnNavigationButtonsVisible").getBool());

        REQUIRE(renderer->getFilenameLabel()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Red);
        REQUIRE(renderer->getEditBox()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Green);
        REQUIRE(renderer->getFileTypeComboBox()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Blue);
        REQUIRE(renderer->getListView()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Yellow);
        REQUIRE(renderer->getButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Cyan);
        REQUIRE(renderer->getBackButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Magenta);
        REQUIRE(renderer->getForwardButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Black);
        REQUIRE(renderer->getUpButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::White);
    }

    SECTION("Saving and loading from file")
    {
        dialog->setPath("/bin");
        dialog->setFilename("config.txt");
        dialog->setFileTypeFilters({
            {"Single", {"*.txt"}},
            {"Multiple", {"*.png", "*.jpg"}},
            {"Everything", {}}
        }, 1);
        dialog->setConfirmButtonText("Use that file");
        dialog->setCancelButtonText("Don't do it!");
        dialog->setFilenameLabelText("What file?");
        dialog->setListViewColumnCaptions("Moniker", "Largeness", "Timing");
        dialog->setFileMustExist(false);
        dialog->setSelectingDirectory(true);
        dialog->setMultiSelect(true);

        auto renderer = dialog->getRenderer();
        renderer->setBackgroundColor({255, 99, 71});
        renderer->setArrowsOnNavigationButtonsVisible(false);

        testSavingWidget("FileDialog", dialog, false);
    }
}
