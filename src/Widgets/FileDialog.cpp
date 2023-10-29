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


#include <TGUI/Widgets/FileDialog.hpp>
#include <TGUI/FileDialogIconLoader.hpp>

#if TGUI_EXPERIMENTAL_USE_STD_MODULE
    #ifdef TGUI_SYSTEM_WINDOWS
        #include <time.h> // localtime_s
    #endif
#else
    #include <vector>
    #include <map>
    #include <ctime>
#endif

#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/extlibs/IncludeWindows.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char FileDialog::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog::FileDialog(const char* typeName, bool initRenderer) :
        ChildWindow{typeName, false},
        m_iconLoader(FileDialogIconLoader::createInstance())
    {
        setClientSize({600, 400});

        m_buttonBack = Button::create();
        m_buttonForward = Button::create();
        m_buttonUp = Button::create();
        m_editBoxPath = EditBox::create();
        m_listView = ListView::create();
        m_labelFilename = Label::create();
        m_editBoxFilename = EditBox::create();
        m_comboBoxFileTypes = ComboBox::create();
        m_buttonCancel = Button::create();
        m_buttonConfirm = Button::create();
        m_buttonCreateFolder = Button::create();

        m_buttonCancel->setText("Cancel");
        m_buttonConfirm->setText("Open");

        add(m_buttonBack, "#TGUI_INTERNAL$ButtonBack#");
        add(m_buttonForward, "#TGUI_INTERNAL$ButtonForward#");
        add(m_buttonUp, "#TGUI_INTERNAL$ButtonUp#");
        add(m_editBoxPath, "#TGUI_INTERNAL$EditBoxPath#");
        add(m_listView, "#TGUI_INTERNAL$ListView#");
        add(m_labelFilename, "#TGUI_INTERNAL$LabelFilename#");
        add(m_editBoxFilename, "#TGUI_INTERNAL$EditBoxFilename#");
        add(m_comboBoxFileTypes, "#TGUI_INTERNAL$ComboBoxFileTypes#");
        add(m_buttonCancel, "#TGUI_INTERNAL$ButtonCancel#");
        add(m_buttonConfirm, "#TGUI_INTERNAL$ButtonConfirm#");

        m_buttonConfirm->setOrigin(1, 1);
        m_buttonCancel->setOrigin(1, 1);
        m_comboBoxFileTypes->setOrigin(1, 1);
        m_editBoxFilename->setOrigin(1, 1);

#ifdef TGUI_SYSTEM_WINDOWS
        m_buttonCancel->setPosition("100% - 10", "100% - 10");
        m_buttonConfirm->setPosition("#TGUI_INTERNAL$ButtonCancel#.left - 10", "100% - 10");
#else
        m_buttonConfirm->setPosition("100% - 10", "100% - 10");
        m_buttonCancel->setPosition("#TGUI_INTERNAL$ButtonConfirm#.left - 10", "100% - 10");
#endif
        m_comboBoxFileTypes->setWidth("33%");
        m_comboBoxFileTypes->setPosition("100% - 10", "#TGUI_INTERNAL$ButtonConfirm#.top - 15");

        m_labelFilename->setPosition({10, "#TGUI_INTERNAL$ComboBoxFileTypes#.top + (#TGUI_INTERNAL$ComboBoxFileTypes#.height - #TGUI_INTERNAL$LabelFilename#.height) / 2"});

        m_editBoxFilename->setPosition("#TGUI_INTERNAL$ComboBoxFileTypes#.left - 10", "#TGUI_INTERNAL$ComboBoxFileTypes#.y");
        m_editBoxFilename->setWidth("#TGUI_INTERNAL$EditBoxFilename#.x - #TGUI_INTERNAL$LabelFilename#.width - #TGUI_INTERNAL$LabelFilename#.x - 5");

        m_buttonBack->setPosition({10, 10});
        m_buttonBack->setSize({"height", "#TGUI_INTERNAL$EditBoxPath#.height"});

        m_buttonForward->setPosition({"#TGUI_INTERNAL$ButtonBack#.x + #TGUI_INTERNAL$ButtonBack#.width + 5", 10});
        m_buttonForward->setSize({"height", "#TGUI_INTERNAL$EditBoxPath#.height"});

        m_buttonUp->setPosition({"#TGUI_INTERNAL$ButtonForward#.x + #TGUI_INTERNAL$ButtonForward#.width + 15", 10});
        m_buttonUp->setSize({"height", "#TGUI_INTERNAL$EditBoxPath#.height"});

        m_editBoxPath->setPosition({"#TGUI_INTERNAL$ButtonUp#.x + #TGUI_INTERNAL$ButtonUp#.width + 5", 10});
        m_editBoxPath->setWidth("100% - #TGUI_INTERNAL$EditBoxPath#.x - 10");

        m_listView->setPosition({10, "#TGUI_INTERNAL$EditBoxPath#.y + #TGUI_INTERNAL$EditBoxPath#.height + 10"});
        m_listView->setSize({"100% - 20", "#TGUI_INTERNAL$EditBoxFilename#.top - top - 10"});

        m_listView->addColumn("Name", 0);
        m_listView->addColumn("Size", 75, ListView::ColumnAlignment::Right);
        m_listView->addColumn("Modified", 145, ListView::ColumnAlignment::Right);
        m_listView->setColumnExpanded(0, true);

        if (m_iconLoader->supportsSystemIcons())
            m_listView->setFixedIconSize({static_cast<float>(m_listView->getItemHeight()) * 0.8f, 0});

        m_labelFilename->setText("Filename:");
        m_editBoxPath->setText(m_currentDirectory.asString());
        m_buttonBack->setText(U"\u2190");
        m_buttonForward->setText(U"\u2192");
        m_buttonUp->setText(U"\u2191");

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<FileDialogRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setFileTypeFilters({});

        setPath(Filesystem::getCurrentWorkingDirectory());
        connectSignals();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog::FileDialog(const FileDialog& other) :
        ChildWindow             {other},
        onFileSelect            {other.onFileSelect},
        onCancel                {other.onCancel},
        m_currentDirectory      {other.m_currentDirectory},
        m_filesInDirectory      {other.m_filesInDirectory},
        m_fileIcons             {other.m_fileIcons},
        m_sortColumnIndex       {other.m_sortColumnIndex},
        m_sortInversed          {other.m_sortInversed},
        m_pathHistory           {other.m_pathHistory},
        m_pathHistoryIndex      {other.m_pathHistoryIndex},
        m_fileMustExist         {other.m_fileMustExist},
        m_selectingDirectory    {other.m_selectingDirectory},
        m_multiSelect           {other.m_multiSelect},
        m_fileTypeFilters       {other.m_fileTypeFilters},
        m_selectedFileTypeFilter{other.m_selectedFileTypeFilter},
        m_iconLoader            {FileDialogIconLoader::createInstance()},
        m_selectedFiles         {other.m_selectedFiles}
    {
        identifyChildWidgets();
        connectSignals();

        // Update the file list (to refresh icons)
        changePath(m_currentDirectory, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog::FileDialog(FileDialog&& other) noexcept :
        ChildWindow             {std::move(other)},
        onFileSelect            {std::move(other.onFileSelect)},
        onCancel                {std::move(other.onCancel)},
        m_buttonBack            {std::move(other.m_buttonBack)},
        m_buttonForward         {std::move(other.m_buttonForward)},
        m_buttonUp              {std::move(other.m_buttonUp)},
        m_editBoxPath           {std::move(other.m_editBoxPath)},
        m_listView              {std::move(other.m_listView)},
        m_labelFilename         {std::move(other.m_labelFilename)},
        m_editBoxFilename       {std::move(other.m_editBoxFilename)},
        m_comboBoxFileTypes     {std::move(other.m_comboBoxFileTypes)},
        m_buttonCancel          {std::move(other.m_buttonCancel)},
        m_buttonConfirm         {std::move(other.m_buttonConfirm)},
        m_buttonCreateFolder    {std::move(other.m_buttonCreateFolder)},
        m_currentDirectory      {std::move(other.m_currentDirectory)},
        m_filesInDirectory      {std::move(other.m_filesInDirectory)},
        m_fileIcons             {std::move(other.m_fileIcons)},
        m_sortColumnIndex       {std::move(other.m_sortColumnIndex)},
        m_sortInversed          {std::move(other.m_sortInversed)},
        m_pathHistory           {std::move(other.m_pathHistory)},
        m_pathHistoryIndex      {std::move(other.m_pathHistoryIndex)},
        m_fileMustExist         {std::move(other.m_fileMustExist)},
        m_selectingDirectory    {std::move(other.m_selectingDirectory)},
        m_multiSelect           {std::move(other.m_multiSelect)},
        m_fileTypeFilters       {std::move(other.m_fileTypeFilters)},
        m_selectedFileTypeFilter{std::move(other.m_selectedFileTypeFilter)},
        m_iconLoader            {std::move(other.m_iconLoader)},
        m_selectedFiles         {std::move(other.m_selectedFiles)}
    {
        connectSignals();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog& FileDialog::operator= (const FileDialog& other)
    {
        if (this != &other)
        {
            ChildWindow::operator=(other);

            onFileSelect = other.onFileSelect;
            onCancel = other.onCancel;
            m_currentDirectory = other.m_currentDirectory;
            m_filesInDirectory = other.m_filesInDirectory;
            m_fileIcons = other.m_fileIcons;
            m_sortColumnIndex = other.m_sortColumnIndex;
            m_sortInversed = other.m_sortInversed;
            m_pathHistory = other.m_pathHistory;
            m_pathHistoryIndex = other.m_pathHistoryIndex;
            m_fileMustExist = other.m_fileMustExist;
            m_selectingDirectory = other.m_selectingDirectory;
            m_multiSelect = other.m_multiSelect;
            m_fileTypeFilters = other.m_fileTypeFilters;
            m_selectedFileTypeFilter = other.m_selectedFileTypeFilter;
            m_iconLoader = FileDialogIconLoader::createInstance();
            m_selectedFiles = other.m_selectedFiles;

            identifyChildWidgets();
            connectSignals();

            // Update the file list (to refresh icons)
            changePath(m_currentDirectory, false);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog& FileDialog::operator= (FileDialog&& other) noexcept
    {
        if (this != &other)
        {
            onFileSelect = std::move(other.onFileSelect);
            onCancel = std::move(other.onCancel);
            m_buttonBack = std::move(other.m_buttonBack);
            m_buttonForward = std::move(other.m_buttonForward);
            m_buttonUp = std::move(other.m_buttonUp);
            m_editBoxPath = std::move(other.m_editBoxPath);
            m_listView = std::move(other.m_listView);
            m_labelFilename = std::move(other.m_labelFilename);
            m_editBoxFilename = std::move(other.m_editBoxFilename);
            m_comboBoxFileTypes = std::move(other.m_comboBoxFileTypes);
            m_buttonCancel = std::move(other.m_buttonCancel);
            m_buttonConfirm = std::move(other.m_buttonConfirm);
            m_buttonCreateFolder = std::move(other.m_buttonCreateFolder);
            m_currentDirectory = std::move(other.m_currentDirectory);
            m_filesInDirectory = std::move(other.m_filesInDirectory);
            m_fileIcons = std::move(other.m_fileIcons);
            m_sortColumnIndex = std::move(other.m_sortColumnIndex);
            m_sortInversed = std::move(other.m_sortInversed);
            m_pathHistory = std::move(other.m_pathHistory);
            m_pathHistoryIndex = std::move(other.m_pathHistoryIndex);
            m_fileMustExist = std::move(other.m_fileMustExist);
            m_selectingDirectory = std::move(other.m_selectingDirectory);
            m_multiSelect = std::move(other.m_multiSelect);
            m_fileTypeFilters = std::move(other.m_fileTypeFilters);
            m_selectedFileTypeFilter = std::move(other.m_selectedFileTypeFilter);
            m_iconLoader = std::move(other.m_iconLoader);
            m_selectedFiles = std::move(other.m_selectedFiles);
            ChildWindow::operator=(std::move(other));

            connectSignals();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog::Ptr FileDialog::create(const String& title, const String& confirmButtonText, bool allowCreateFolder)
    {
        auto fileDialog = std::make_shared<FileDialog>();
        fileDialog->setTitle(title);
        fileDialog->setConfirmButtonText(confirmButtonText);
        fileDialog->setAllowCreateFolder(allowCreateFolder);
        return fileDialog;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialog::Ptr FileDialog::copy(const FileDialog::ConstPtr& dialog)
    {
        if (dialog)
            return std::static_pointer_cast<FileDialog>(dialog->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogRenderer* FileDialog::getSharedRenderer()
    {
        return aurora::downcast<FileDialogRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const FileDialogRenderer* FileDialog::getSharedRenderer() const
    {
        return aurora::downcast<const FileDialogRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogRenderer* FileDialog::getRenderer()
    {
        return aurora::downcast<FileDialogRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Filesystem::Path>& FileDialog::getSelectedPaths() const
    {
        return m_selectedFiles;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setPath(const String& path)
    {
        setPath(Filesystem::Path(path));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setPath(const Filesystem::Path& path)
    {
        m_pathHistoryIndex = 0;
        m_pathHistory.clear();
        m_pathHistory.push_back(path);
        historyChanged();

        changePath(path, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Filesystem::Path& FileDialog::getPath() const
    {
        return m_currentDirectory;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setFilename(const String& filename)
    {
        m_editBoxFilename->setText(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& FileDialog::getFilename() const
    {
        return m_editBoxFilename->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setFileTypeFilters(const std::vector<std::pair<String, std::vector<String>>>& filters, std::size_t defaultIndex)
    {
        m_fileTypeFilters.clear();
        for (const auto& filter : filters)
        {
            std::vector<String> expressions = filter.second;
            for (auto& expression : expressions)
            {
                if (expression.empty() || (expression == U"*") || (expression == U"*.*"))
                {
                    // The expression doesn't filter any files and shouldn't exist
                    expressions.clear();
                    break;
                }

                expression = expression.toLower();
            }

            m_fileTypeFilters.emplace_back(filter.first, expressions);
        }

        if (m_fileTypeFilters.size() == 0)
            m_fileTypeFilters.emplace_back("All files (*)", std::vector<String>());

        m_comboBoxFileTypes->removeAllItems();

        for (const auto& filter : m_fileTypeFilters)
            m_comboBoxFileTypes->addItem(filter.first);

        if (defaultIndex > m_comboBoxFileTypes->getItemCount())
        {
            defaultIndex = m_comboBoxFileTypes->getItemCount() - 1;
            TGUI_PRINT_WARNING("defaultIndex too high in FileDialog::setFileTypeFilters");
        }

        m_comboBoxFileTypes->setSelectedItemByIndex(defaultIndex);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<std::pair<String, std::vector<String>>>& FileDialog::getFileTypeFilters() const
    {
        return m_fileTypeFilters;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t FileDialog::getFileTypeFiltersIndex() const
    {
        TGUI_ASSERT(m_comboBoxFileTypes->getSelectedItemIndex() >= 0, "File type combo box always need to have item selected");
        return static_cast<std::size_t>(m_comboBoxFileTypes->getSelectedItemIndex());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setConfirmButtonText(const String& text)
    {
        m_buttonConfirm->setText(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& FileDialog::getConfirmButtonText() const
    {
        return m_buttonConfirm->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setCancelButtonText(const String& text)
    {
        m_buttonCancel->setText(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& FileDialog::getCancelButtonText() const
    {
        return m_buttonCancel->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setCreateFolderButtonText(const String& text)
    {
        m_buttonCreateFolder->setText(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& FileDialog::getCreateFolderButtonText() const
    {
        return m_buttonCreateFolder->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setAllowCreateFolder(bool allowCreateFolder)
    {
        m_allowCreateFolder = allowCreateFolder;

        if (nullptr == m_buttonCreateFolder->getParent() && m_allowCreateFolder)
            addCreateFolderButton();
        else
            remove(m_buttonCreateFolder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::getAllowCreateFolder() const
    {
        return m_allowCreateFolder;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setFilenameLabelText(const String& labelText)
    {
        m_labelFilename->setText(labelText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& FileDialog::getFilenameLabelText() const
    {
        return m_labelFilename->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setListViewColumnCaptions(const String& nameColumnText, const String& sizeColumnText, const String& modifiedColumnText)
    {
        m_listView->setColumnText(0, nameColumnText);
        m_listView->setColumnText(1, sizeColumnText);
        m_listView->setColumnText(2, modifiedColumnText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::tuple<String, String, String> FileDialog::getListViewColumnCaptions() const
    {
        return std::make_tuple(m_listView->getColumnText(0), m_listView->getColumnText(1), m_listView->getColumnText(2));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setFileMustExist(bool enforceExistence)
    {
        m_fileMustExist = enforceExistence;
        updateConfirmButtonEnabled();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::getFileMustExist() const
    {
        return m_fileMustExist;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setSelectingDirectory(bool selectDirectories)
    {
        m_selectingDirectory = selectDirectories;

        if (selectDirectories)
        {
            m_comboBoxFileTypes->setVisible(false);
            m_editBoxFilename->setPosition({"100% - 10", m_editBoxFilename->getPositionLayout().y});
        }
        else
        {
            m_comboBoxFileTypes->setVisible(true);
            m_editBoxFilename->setPosition({"#TGUI_INTERNAL$ComboBoxFileTypes#.x - #TGUI_INTERNAL$ComboBoxFileTypes#.width - 10",
                                            m_editBoxFilename->getPositionLayout().y});
        }

        changePath(m_currentDirectory, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::getSelectingDirectory() const
    {
        return m_selectingDirectory;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setMultiSelect(bool multiSelect)
    {
        m_multiSelect = multiSelect;
        m_listView->setMultiSelect(multiSelect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::getMultiSelect() const
    {
        return m_multiSelect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::setIconLoader(std::shared_ptr<FileDialogIconLoader> iconLoader)
    {
        TGUI_ASSERT(iconLoader != nullptr, "Icon loader can't be a nullptr");
        m_iconLoader = std::move(iconLoader);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<FileDialogIconLoader> FileDialog::getIconLoader() const
    {
        return m_iconLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::keyPressed(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Enter) && (!m_editBoxPath->isFocused()))
        {
            if (m_createFolderDialogOpen)
            {
                EditBox::Ptr folderNameEditBox = get<EditBox>("FolderNameEditBox");
                createFolder(folderNameEditBox->getText());
                destroyCreateFolderDialog();
                return;
            }

            // Simulate a press on the confirm button
            if (m_buttonConfirm->isEnabled())
                confirmButtonPressed();
        }
        else if ((event.code == Event::KeyboardKey::Escape) && m_createFolderDialogOpen)
        {
            destroyCreateFolderDialog();
        }
        else
        {
            ChildWindow::keyPressed(event);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Enter) || (event.code == Event::KeyboardKey::Escape))
            return true;
        else
            return ChildWindow::canHandleKeyPress(event);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::textEntered(char32_t key)
    {
        // We will only search through the list view if no other widget was focused. Pass the event to the focused widget otherwise.
        if (m_focusedWidget && (m_focusedWidget != m_listView))
        {
            ChildWindow::textEntered(key);
            return;
        }

        // We currently only search for the typed character. In the future we should remember previously typed characters too.
        const String searchStr{key};

        // Select the first item in the list view that starts with the search string (case-insensitive), starting the
        // search from the currently selected item.
        const std::size_t startIndex = static_cast<std::size_t>(std::max(0, m_listView->getSelectedItemIndex()));
        const std::size_t itemCount = m_listView->getItemCount();
        for (std::size_t i = 0; i < itemCount; ++i)
        {
            const std::size_t index = (startIndex + i) % itemCount;
            const String& item = m_listView->getItem(index);
            if (item.empty() || (item.length() < searchStr.length()))
                continue;

            if (item.startsWithIgnoreCase(searchStr))
            {
                m_listView->setSelectedItem(index);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::updateTime(Duration elapsedTime)
    {
        const bool childWidgetUpdated = ChildWindow::updateTime(elapsedTime);

        if (!m_iconLoader->update())
            return childWidgetUpdated;

        m_fileIcons = m_iconLoader->retrieveFileIcons();

        const int oldSelectedItem = m_listView->getSelectedItemIndex();
        sortFilesInListView();
        if (oldSelectedItem >= 0)
            m_listView->setSelectedItem(static_cast<std::size_t>(oldSelectedItem));

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::changePath(const Filesystem::Path& path, bool updateHistory)
    {
        if (updateHistory && (m_currentDirectory != path))
        {
            if (m_pathHistoryIndex + 1 < m_pathHistory.size())
                m_pathHistory.erase(m_pathHistory.begin() + static_cast<std::ptrdiff_t>(m_pathHistoryIndex + 1), m_pathHistory.end());

            m_pathHistory.push_back(path);
            m_pathHistoryIndex = m_pathHistory.size() - 1;
            historyChanged();
        }

        m_currentDirectory = path;
        m_editBoxPath->setText(m_currentDirectory.asString());

        m_filesInDirectory = Filesystem::listFilesInDirectory(path);

#ifdef TGUI_SYSTEM_WINDOWS
        if (path.asString().empty())
        {
            m_listView->setHeaderVisible(false);
            m_listView->setShowVerticalGridLines(false);
            m_listView->removeAllItems();
            wchar_t logicalDrives[MAX_PATH];

            if (GetLogicalDriveStringsW(MAX_PATH, logicalDrives))
            {
                wchar_t* drive = logicalDrives;
                while (*drive)
                {
                    m_listView->setItemData(m_listView->addItem(Filesystem::Path(drive).asString()), true);
                    drive += std::wcslen(drive) + 1;
                }
            }
            m_filesInDirectory.clear();
            return;
        }
#endif // TGUI_SYSTEM_WINDOWS

        // If only directories should be shown then remove the files from the list
        if (m_selectingDirectory)
        {
            m_filesInDirectory.erase(std::remove_if(m_filesInDirectory.begin(), m_filesInDirectory.end(),
                [](const Filesystem::FileInfo& info){ return !info.directory; }), m_filesInDirectory.end());
        }

        m_fileIcons.clear();
        if (m_iconLoader->hasGenericIcons())
        {
            m_fileIcons.reserve(m_filesInDirectory.size());
            for (const auto& file : m_filesInDirectory)
                m_fileIcons.push_back(m_iconLoader->getGenericFileIcon(file));
        }

        m_iconLoader->requestFileIcons(m_filesInDirectory);

        m_listView->setVerticalScrollbarValue(0);
        sortFilesInListView();
        updateConfirmButtonEnabled();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::historyChanged()
    {
        m_buttonBack->setEnabled(m_pathHistoryIndex > 0);
        m_buttonForward->setEnabled(m_pathHistoryIndex + 1 < m_pathHistory.size());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::sortFilesInListView()
    {
        const auto oldScrollbarValue = m_listView->getVerticalScrollbarValue();

        std::vector<std::pair<Filesystem::FileInfo, Texture>> items;
        items.reserve(m_filesInDirectory.size());
        if (!m_fileIcons.empty())
        {
            TGUI_ASSERT(m_filesInDirectory.size() == m_fileIcons.size(), "Icon count must match file count in FileDialog::sortFilesInListView");
            for (std::size_t i = 0; i < m_filesInDirectory.size(); ++i)
                items.emplace_back(m_filesInDirectory[i], m_fileIcons[i]);
        }
        else // There are no file icons
        {
            for (const auto& file : m_filesInDirectory)
                items.emplace_back(file, Texture{});
        }

        std::sort(items.begin(), items.end(), [this](const auto& leftItem, const auto& rightItem){
            const Filesystem::FileInfo& left = leftItem.first;
            const Filesystem::FileInfo& right = rightItem.first;
            if (m_sortColumnIndex == 2) // Sort by modification date
            {
                if (m_sortInversed)
                    return left.modificationTime < right.modificationTime;
                else
                    return left.modificationTime > right.modificationTime;
            }
            else if (m_sortColumnIndex == 1) // Sort by file size
            {
                if (left.directory != right.directory)
                    return right.directory; // Place directories at the end of the list
                else if (left.directory) // Both are directories, sort them alphabetically by filename since they have no size
                    return left.filename.toLower() < right.filename.toLower();
                else // Both are files, sort them by file size
                {
                    if (m_sortInversed)
                        return left.fileSize < right.fileSize;
                    else
                        return left.fileSize > right.fileSize;
                }
            }
            else // Sort by filename
            {
                if (left.directory != right.directory)
                    return left.directory; // Place directories in front of files
                else // Both are directories or both are files, so sort alphabetically
                {
                    if (m_sortInversed)
                        return left.filename.toLower() > right.filename.toLower();
                    else
                        return left.filename.toLower() < right.filename.toLower();
                }
            }
        });

        if (!m_listView->getHeaderVisible())
        {
            m_listView->setHeaderVisible(true);
            m_listView->setShowVerticalGridLines(true);
        }
        m_listView->removeAllItems();
        for (const auto& item : items)
        {
            const Filesystem::FileInfo& file = item.first;
            const Texture& icon = item.second;

            // Filter the files
            if (!file.directory && !m_fileTypeFilters[m_selectedFileTypeFilter].second.empty())
            {
                const String& lowercaseFilename = file.filename.toLower();

                bool fileAccepted = false;
                for (const auto& filter : m_fileTypeFilters[m_selectedFileTypeFilter].second)
                {
                    TGUI_ASSERT(!filter.empty(), "FileDialog::sortFilesInListView can't have empty filter, they are removed in setFileTypeFilters");

                    if (filter[0] == '*')
                    {
                        if ((lowercaseFilename.length() >= filter.length() - 1)
                         && (lowercaseFilename.compare(lowercaseFilename.length() - (filter.length() - 1),
                                                       filter.length() - 1, filter, 1, filter.length() - 1) == 0))
                        {
                            fileAccepted = true;
                            break;
                        }
                    }
                    // Check if the filter matches exactly with the filename
                    else if (lowercaseFilename == filter)
                    {
                        fileAccepted = true;
                        break;
                    }
                }

                if (!fileAccepted)
                    continue;
            }

            String fileSizeStr;
            if (!file.directory)
            {
                if (file.fileSize == 0)
                    fileSizeStr = U"0.0 KB";
                else if (file.fileSize < static_cast<std::uintmax_t>(100))
                    fileSizeStr = U"0.1 KB";
                else if (file.fileSize < static_cast<std::uintmax_t>(1000)*1000)
                    fileSizeStr = String::fromNumberRounded(file.fileSize / 1000.f, 1) + U" KB";
                else if (file.fileSize < static_cast<std::uintmax_t>(1000)*1000*1000)
                    fileSizeStr = String::fromNumberRounded(file.fileSize / 1000.f / 1000.f, 1) + U" MB";
                else if (file.fileSize < static_cast<std::uintmax_t>(1000)*1000*1000*1000)
                    fileSizeStr = String::fromNumberRounded(file.fileSize / 1000.f / 1000.f / 1000.f, 1) + U" GB";
                else if (file.fileSize < static_cast<std::uintmax_t>(1000)*1000*1000*1000*1000)
                    fileSizeStr = String::fromNumberRounded(file.fileSize / 1000.f / 1000.f / 1000.f / 1000.f, 1) + U" TB";
            }

            String modificationTimeStr;
            bool modificationTimeConverted = false;
            char buffer[19];
#if defined(TGUI_SYSTEM_WINDOWS) && defined(_MSC_VER)
            std::tm TimeStructure;
            if (localtime_s(&TimeStructure, &file.modificationTime) == 0)
            {
                if (std::strftime(&buffer[0], sizeof(buffer), "%e %b %Y  %R", &TimeStructure) != 0)
                    modificationTimeConverted = true;
            }
#elif defined(TGUI_SYSTEM_WINDOWS) && defined(__GNUC__) // MinGW doesn't support %e (day of the month without leading 0) and %R (same as %H:%M)
            if (strftime(&buffer[0], sizeof(buffer), "%d %b %Y  %H:%M", std::localtime(&file.modificationTime)) != 0)
                modificationTimeConverted = true;
#else
            if (strftime(&buffer[0], sizeof(buffer), "%e %b %Y  %R", std::localtime(&file.modificationTime)) != 0)
                modificationTimeConverted = true;
#endif
            if (modificationTimeConverted)
                modificationTimeStr = static_cast<char*>(buffer);

#if defined(TGUI_SYSTEM_WINDOWS)
            // Hide .lnk and .url extensions
            String filename = file.filename;
            const auto filenameLen = file.filename.length();
            if ((filename.length() > 4) && ((filename.compare(filenameLen - 4, 4, U".lnk", 4) == 0) || (filename.compare(filenameLen - 4, 4, U".url", 4) == 0)))
                filename.erase(filenameLen - 4, 4);

            const std::size_t itemIndex = m_listView->addItem({filename, fileSizeStr, modificationTimeStr});
#else
            const std::size_t itemIndex = m_listView->addItem({file.filename, fileSizeStr, modificationTimeStr});
#endif
            m_listView->setItemData(itemIndex, file.directory);

            if (icon.getData())
                m_listView->setItemIcon(itemIndex, icon);
        }

        m_listView->setVerticalScrollbarValue(oldScrollbarValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::filesSelected(std::vector<Filesystem::Path> selectedFiles)
    {
        m_selectedFiles = std::move(selectedFiles);

        if (!m_selectedFiles.empty())
            onFileSelect.emit(this, m_selectedFiles);
        else
            onCancel.emit(this);

        close();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::updateConfirmButtonEnabled()
    {
        const String& filename = m_editBoxFilename->getText();
        const bool enabled = ((filename != U".") && (filename != U"..")) // Always disabled for "." and ".." filenames
            && (m_selectingDirectory || !m_listView->getSelectedItemIndices().empty() || !filename.empty()) // Disabled when no file selected
            && (!m_fileMustExist || Filesystem::fileExists(m_currentDirectory / Filesystem::Path(filename))); // Disabled for non-existent files

        if (enabled != m_buttonConfirm->isEnabled())
            m_buttonConfirm->setEnabled(enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::confirmButtonPressed()
    {
        if (m_multiSelect && (m_listView->getSelectedItemIndices().size() > 1))
        {
            const auto& indices = m_listView->getSelectedItemIndices();

            std::vector<Filesystem::Path> paths;
            for (const auto index : indices)
            {
                // Filter out directories when we are attempting to select files
                if (!m_selectingDirectory && m_listView->getItemData<bool>(index))
                    continue;

                paths.push_back(m_currentDirectory / m_listView->getItem(index));
            }

            filesSelected(std::move(paths));
        }
        else // Only one file selected
        {
            const auto path = m_currentDirectory / Filesystem::Path(m_editBoxFilename->getText());

            // If we were looking for a file but a directory was selected then just enter that directory
            if (!m_selectingDirectory && (m_listView->getSelectedItemIndices().size() == 1)
                  && (m_listView->getItemData<bool>(*m_listView->getSelectedItemIndices().begin())))
            {
                changePath(m_currentDirectory / m_listView->getItem(*m_listView->getSelectedItemIndices().begin()), true);
                m_editBoxFilename->setText(U"");
            }
            else if (!m_selectingDirectory && !m_editBoxFilename->getText().empty() && Filesystem::directoryExists(path))
            {
                changePath(path, true);
                m_editBoxFilename->setText(U"");
            }
            else
                filesSelected({path});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::addCreateFolderButton()
    {
        m_buttonCreateFolder->setText("Create Folder");
        m_buttonCreateFolder->setOrigin(0, 1);
        m_buttonCreateFolder->setPosition("10", "100% - 10");

        m_buttonCreateFolder->onPress.disconnectAll();
        m_buttonCreateFolder->onPress([this]{ createCreateFolderDialog(); });

        add(m_buttonCreateFolder, "#TGUI_INTERNAL$ButtonCreateFolder#");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::createFolder(const String& name)
    {
        Filesystem::createDirectory(m_currentDirectory / name);
        changePath(m_currentDirectory, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::createCreateFolderDialog()
    {
        // Create a transparent background
        Panel::Ptr backgroundPanel = Panel::create({"100%", "100%"});
        backgroundPanel->getRenderer()->setBackgroundColor({0, 0, 0, 175});

        ChildWindow::Ptr createFolderWindow = ChildWindow::create(m_buttonCreateFolder->getText(), TitleButton::None);
        createFolderWindow->setPositionLocked(true);
        createFolderWindow->setPosition("50%", "50%");
        createFolderWindow->setSize("50%", "25%");
        createFolderWindow->setOrigin(0.5f, 0.5f);

        EditBox::Ptr folderNameEditBox = EditBox::create();
        folderNameEditBox->setPosition("50%", "30%");
        folderNameEditBox->setOrigin(0.5f, 0.5f);

        Button::Ptr cancelButton = Button::create("Cancel");
        cancelButton->setPosition("25%", "75%");
        cancelButton->setOrigin(0.5f, 0.5f);

        Button::Ptr confirmButton = Button::create("Confirm");
        confirmButton->setPosition("75%", "75%");
        confirmButton->setOrigin(0.5f, 0.5f);
        confirmButton->setEnabled(false);

        folderNameEditBox->onTextChange([this, editBox=folderNameEditBox.get(), button=confirmButton.get()] {
            const bool isValid = isValidFolderName(editBox->getText());
            button->setEnabled(isValid);
        });

        cancelButton->onPress([this] {
            destroyCreateFolderDialog();
        });

        confirmButton->onPress([this, editBox=folderNameEditBox.get()] {
            createFolder(editBox->getText());
            destroyCreateFolderDialog();
        });

        createFolderWindow->add(folderNameEditBox, "FolderNameEditBox");
        createFolderWindow->add(cancelButton);
        createFolderWindow->add(confirmButton);

        backgroundPanel->add(createFolderWindow);

        add(backgroundPanel, "TransparentBackgroundPanel");

        folderNameEditBox->setFocused(true);

        m_createFolderDialogOpen = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::destroyCreateFolderDialog()
    {
        remove(get("TransparentBackgroundPanel"));
        m_createFolderDialogOpen = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialog::isValidFolderName(const String& name)
    {
        if (name.empty() || (name == U".") || (name == U".."))
            return false;

#ifdef TGUI_SYSTEM_WINDOWS
        if (name.find_first_of(U"\\/:*?\"<>|") != String::npos)
#else
        if (name.contains(U'/'))
#endif
            return false;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::identifyChildWidgets()
    {
        m_buttonBack = get<Button>("#TGUI_INTERNAL$ButtonBack#");
        m_buttonForward = get<Button>("#TGUI_INTERNAL$ButtonForward#");
        m_buttonUp = get<Button>("#TGUI_INTERNAL$ButtonUp#");
        m_editBoxPath = get<EditBox>("#TGUI_INTERNAL$EditBoxPath#");
        m_listView = get<ListView>("#TGUI_INTERNAL$ListView#");
        m_labelFilename = get<Label>("#TGUI_INTERNAL$LabelFilename#");
        m_editBoxFilename = get<EditBox>("#TGUI_INTERNAL$EditBoxFilename#");
        m_comboBoxFileTypes = get<ComboBox>("#TGUI_INTERNAL$ComboBoxFileTypes#");
        m_buttonCancel = get<Button>("#TGUI_INTERNAL$ButtonCancel#");
        m_buttonConfirm = get<Button>("#TGUI_INTERNAL$ButtonConfirm#");
        m_buttonCreateFolder = get<Button>("#TGUI_INTERNAL$ButtonCreateFolder#");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::connectSignals()
    {
        m_closeButton->onPress.disconnectAll(); // Replace the signal handler of ChildWindow

        m_buttonBack->onPress.disconnectAll();
        m_buttonForward->onPress.disconnectAll();
        m_buttonUp->onPress.disconnectAll();
        m_editBoxPath->onReturnKeyPress.disconnectAll();
        m_comboBoxFileTypes->onItemSelect.disconnectAll();
        m_listView->onItemSelect.disconnectAll();
        m_listView->onHeaderClick.disconnectAll();
        m_listView->onDoubleClick.disconnectAll();
        m_buttonCancel->onPress.disconnectAll();
        m_buttonConfirm->onPress.disconnectAll();
        m_editBoxFilename->onTextChange.disconnectAll();

        m_buttonBack->onPress([this]{
            --m_pathHistoryIndex;
            changePath(m_pathHistory[m_pathHistoryIndex], false);
            historyChanged();
        });
        m_buttonForward->onPress([this]{
            ++m_pathHistoryIndex;
            changePath(m_pathHistory[m_pathHistoryIndex], false);
            historyChanged();
        });
        m_buttonUp->onPress([this]{
            auto parent = m_currentDirectory.getParentPath();

            // If the path ended with a slash but without a filename, then getParentPath simply
            // removed the slash. We however want to go up one level higher.
            if (m_currentDirectory.getFilename().empty())
                parent = parent.getParentPath();

#ifdef TGUI_SYSTEM_WINDOWS
            if (parent.asString() == m_currentDirectory.asString())
                parent = Filesystem::Path("");
#endif // TGUI_SYSTEM_WINDOWS

            changePath(parent, true);
        });
        m_editBoxPath->onReturnKeyPress([this]{
            changePath(Filesystem::Path(m_editBoxPath->getText()), true);
        });
        m_comboBoxFileTypes->onItemSelect([this](int selectedItem){
            TGUI_ASSERT(selectedItem >= 0, "There always needs to be a file type filter selected in FileDialog");
            TGUI_ASSERT(static_cast<std::size_t>(selectedItem) < m_fileTypeFilters.size(), "Can't select file type filter that doesn't exist");
            m_selectedFileTypeFilter = static_cast<std::size_t>(selectedItem);
            sortFilesInListView();
        });
        m_listView->onItemSelect([this](int itemIndex){
            updateConfirmButtonEnabled();
            if (itemIndex < 0)
                return;

            if (m_multiSelect && (m_listView->getSelectedItemIndices().size() > 1))
                m_editBoxFilename->setText(U"");
            else if (m_selectingDirectory || !m_listView->getItemData<bool>(static_cast<std::size_t>(itemIndex)))
                m_editBoxFilename->setText(m_listView->getItem(static_cast<std::size_t>(itemIndex)));
        });
        m_listView->onHeaderClick([this](int itemIndex){
            TGUI_ASSERT(itemIndex >= 0, "Can't click on list view header that doesn't exist");
            if (m_sortColumnIndex != static_cast<std::size_t>(itemIndex))
            {
                m_sortColumnIndex = static_cast<std::size_t>(itemIndex);
                m_sortInversed = false;
            }
            else
                m_sortInversed = !m_sortInversed;

            sortFilesInListView();
        });
        m_listView->onDoubleClick([this](int itemIndex){
            if (itemIndex < 0)
                return;

            if (m_listView->getItemData<bool>(static_cast<std::size_t>(itemIndex)))
            {
#ifdef TGUI_SYSTEM_WINDOWS
                if (m_currentDirectory.asString().empty())
                {
                    changePath(Filesystem::Path(m_listView->getItem(static_cast<std::size_t>(itemIndex))), true);
                    return;
                }
#endif
                changePath(m_currentDirectory / m_listView->getItem(static_cast<std::size_t>(itemIndex)), true);
                if (m_selectingDirectory)
                    m_editBoxFilename->setText(U"");
            }
            else
            {
                m_editBoxFilename->setText(m_listView->getItem(static_cast<std::size_t>(itemIndex)));
                filesSelected({m_currentDirectory / Filesystem::Path(m_editBoxFilename->getText())});
            }
        });
        m_buttonCancel->onPress([this]{ filesSelected({}); });
        m_buttonConfirm->onPress([this]{ confirmButtonPressed(); });
        m_editBoxFilename->onTextChange([this]{
            // Deselect items when the user starts typing while there are still selected items
            if (!m_listView->getSelectedItemIndices().empty() && !m_editBoxFilename->getText().empty()
             && (m_listView->getItem(*m_listView->getSelectedItemIndices().begin()) != m_editBoxFilename->getText()))
            {
                m_listView->setSelectedItems({});
            }

            updateConfirmButtonEnabled();
        });

        m_closeButton->onPress([this]{ filesSelected({}); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& FileDialog::getSignal(String signalName)
    {
        if (signalName == onFileSelect.getName())
            return onFileSelect;
        else if (signalName == onCancel.getName())
            return onCancel;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::rendererChanged(const String& property)
    {
        if (property == U"ListView")
        {
            m_listView->setRenderer(getSharedRenderer()->getListView());
        }
        else if (property == U"EditBox")
        {
            const auto& renderer = getSharedRenderer()->getEditBox();
            m_editBoxFilename->setRenderer(renderer);
            m_editBoxPath->setRenderer(renderer);
        }
        else if (property == U"Button")
        {
            const auto& renderer = getSharedRenderer()->getButton();
            m_buttonCancel->setRenderer(renderer);
            m_buttonConfirm->setRenderer(renderer);
            m_buttonCreateFolder->setRenderer(renderer);

            if (!getSharedRenderer()->getBackButton())
                m_buttonBack->setRenderer(renderer);
            if (!getSharedRenderer()->getForwardButton())
                m_buttonForward->setRenderer(renderer);
            if (!getSharedRenderer()->getUpButton())
                m_buttonUp->setRenderer(renderer);
        }
        else if (property == U"BackButton")
        {
            if (getSharedRenderer()->getBackButton())
                m_buttonBack->setRenderer(getSharedRenderer()->getBackButton());
            else
                m_buttonBack->setRenderer(getSharedRenderer()->getButton());
        }
        else if (property == U"ForwardButton")
        {
            if (getSharedRenderer()->getForwardButton())
                m_buttonForward->setRenderer(getSharedRenderer()->getForwardButton());
            else
                m_buttonForward->setRenderer(getSharedRenderer()->getButton());
        }
        else if (property == U"UpButton")
        {
            if (getSharedRenderer()->getUpButton())
                m_buttonUp->setRenderer(getSharedRenderer()->getUpButton());
            else
                m_buttonUp->setRenderer(getSharedRenderer()->getButton());
        }
        else if (property == U"FilenameLabel")
        {
            m_labelFilename->setRenderer(getSharedRenderer()->getFilenameLabel());
        }
        else if (property == U"FileTypeComboBox")
        {
            m_comboBoxFileTypes->setRenderer(getSharedRenderer()->getFileTypeComboBox());
        }
        else if (property == U"ArrowsOnNavigationButtonsVisible")
        {
            if (getSharedRenderer()->getArrowsOnNavigationButtonsVisible())
            {
                m_buttonBack->setText(U"\u2190");
                m_buttonForward->setText(U"\u2192");
                m_buttonUp->setText(U"\u2191");
            }
            else
            {
                m_buttonBack->setText(U"");
                m_buttonForward->setText(U"");
                m_buttonUp->setText(U"");
            }
        }
        else if (property == U"Font")
        {
            ChildWindow::rendererChanged(property);

            m_buttonBack->setInheritedFont(m_fontCached);
            m_buttonForward->setInheritedFont(m_fontCached);
            m_buttonUp->setInheritedFont(m_fontCached);
            m_editBoxPath->setInheritedFont(m_fontCached);
            m_listView->setInheritedFont(m_fontCached);
            m_labelFilename->setInheritedFont(m_fontCached);
            m_editBoxFilename->setInheritedFont(m_fontCached);
            m_comboBoxFileTypes->setInheritedFont(m_fontCached);
            m_buttonCancel->setInheritedFont(m_fontCached);
            m_buttonConfirm->setInheritedFont(m_fontCached);
            m_buttonCreateFolder->setInheritedFont(m_fontCached);
        }
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> FileDialog::save(SavingRenderersMap& renderers) const
    {
        auto node = ChildWindow::save(renderers);

        // Child widgets are saved indirectly by saving the child window.
        // The list view however contained items which shouldn't be saved, so we removed the nodes that were created for them.
        const auto listViewNodeIt = std::find_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
            return child->name == U"ListView.\"#TGUI_INTERNAL$ListView#\"";
        });
        TGUI_ASSERT(listViewNodeIt != node->children.end(), "FileDialog::save couldn't find its ListView");
        const auto& listViewNode = *listViewNodeIt;
        listViewNode->children.erase(std::remove_if(listViewNode->children.begin(), listViewNode->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
            return child->name == U"Item";
        }), listViewNode->children.end());

        // We currently don't save the path, so the text in the path edit box shouldn't be saved either
        const auto pathEditBoxNodeIt = std::find_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
            return child->name == U"EditBox.\"#TGUI_INTERNAL$EditBoxPath#\"";
        });
        TGUI_ASSERT(pathEditBoxNodeIt != node->children.end(), "FileDialog::save couldn't find its path EditBox");
        const auto& pathEditBoxNode = *pathEditBoxNodeIt;
        pathEditBoxNode->propertyValuePairs.erase(U"Text");

        node->propertyValuePairs[U"FileMustExist"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_fileMustExist));
        node->propertyValuePairs[U"SelectingDirectory"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_selectingDirectory));
        node->propertyValuePairs[U"MultiSelect"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_multiSelect));

        if (!m_fileTypeFilters.empty())
        {
            for (const auto& filter : m_fileTypeFilters)
            {
                TGUI_EMPLACE_BACK(filterNode, node->children)
                filterNode = std::make_unique<DataIO::Node>();
                filterNode->name = "FileTypeFilter";

                const std::vector<String>& patternList = filter.second;
                String patternListStr = "[";
                for (std::size_t j = 0; j < patternList.size(); ++j)
                {
                    if (j > 0)
                        patternListStr += ", ";

                    patternListStr += Serializer::serialize(patternList[j]);
                }
                patternListStr += "]";

                filterNode->propertyValuePairs[U"Description"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(filter.first));
                filterNode->propertyValuePairs[U"Pattern"] = std::make_unique<DataIO::ValueNode>(patternListStr);
            }

            if (m_fileTypeFilters.size() > 1)
                node->propertyValuePairs[U"SelectedFileTypeFilter"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_selectedFileTypeFilter));
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialog::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        std::vector<std::pair<String, std::vector<String>>> fileTypeFilters;
        for (const auto& childNode : node->children)
        {
            if (childNode->name != U"FileTypeFilter")
                continue;

            if (!childNode->propertyValuePairs[U"Description"])
                throw Exception{U"Failed to parse 'FileTypeFilter' property, no Description property found"};

            if (!childNode->propertyValuePairs[U"Pattern"])
                throw Exception{U"Failed to parse 'FileTypeFilter' property, no Pattern property found"};
            if (!childNode->propertyValuePairs[U"Pattern"]->listNode)
                throw Exception{U"Failed to parse 'Pattern' property inside the 'FileTypeFilter' property, expected a list as value"};

            String description = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs[U"Description"]->value).getString();

            std::vector<String> patterns;
            for (const auto& item : childNode->propertyValuePairs[U"Pattern"]->valueList)
                patterns.push_back(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());

            fileTypeFilters.emplace_back(description, patterns);
        }

        // We have to remove FileTypeFilter nodes before calling the load function on the base class, because Container will
        // assume that all child sections are widgets.
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
            return child->name == U"FileTypeFilter";
        }), node->children.end());

        // Remove the widgets that the constructor created because they will be created when loading the child window
        removeAllWidgets();

        ChildWindow::load(node, renderers);

        identifyChildWidgets();
        if (!m_buttonCreateFolder)
            m_buttonCreateFolder = Button::create();

        if (!m_buttonBack || !m_buttonForward || !m_buttonUp || !m_editBoxPath || !m_listView || !m_labelFilename
         || !m_editBoxFilename || !m_comboBoxFileTypes || !m_buttonCancel || !m_buttonConfirm)
            throw Exception{U"Failed to find all internal child widgets while loading FileDialog"};

        connectSignals();

        if (node->propertyValuePairs[U"FileMustExist"])
            setFileMustExist(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"FileMustExist"]->value).getBool());
        if (node->propertyValuePairs[U"SelectingDirectory"])
            setSelectingDirectory(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"SelectingDirectory"]->value).getBool());
        if (node->propertyValuePairs[U"MultiSelect"])
            setMultiSelect(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"MultiSelect"]->value).getBool());

        if (!fileTypeFilters.empty())
        {
            std::size_t filterIndex = 0;
            if (node->propertyValuePairs[U"SelectedFileTypeFilter"])
                filterIndex = static_cast<std::size_t>(Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"SelectedFileTypeFilter"]->value).getNumber());

            setFileTypeFilters(fileTypeFilters, filterIndex);
        }

        sortFilesInListView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr FileDialog::clone() const
    {
        return std::make_shared<FileDialog>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
