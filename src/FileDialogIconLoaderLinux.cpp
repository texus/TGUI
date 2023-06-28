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


#include <TGUI/FileDialogIconLoader.hpp>
#include <TGUI/Exception.hpp>

#if defined(TGUI_SYSTEM_LINUX)

#include <fstream>
#include <atomic>
#include <thread>
#include <cstdlib> // getenv
#include <dlfcn.h> // dlopen, dlsym, dlclose

#if defined(__has_include)
    #if __has_include (<magic.h>)
        #define TGUI_MAGIC_HEADER_INCLUDED
        #include <magic.h>
    #endif
#endif

#if !defined(TGUI_MAGIC_HEADER_INCLUDED)
    // We load libmagic dynamically instead of requiring that the development files are installed
    #define	MAGIC_MIME_TYPE 0x000010
    extern "C"
    {
        using magic_t = struct magic_set*;
        magic_t magic_open(int);
        void magic_close(magic_t);
        int magic_load(magic_t, const char *);
        const char* magic_file(magic_t, const char*);
    }
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class FileDialogIconLoaderLinux : public FileDialogIconLoader
    {
    public:
        FileDialogIconLoaderLinux();
        ~FileDialogIconLoaderLinux() override;

        TGUI_NODISCARD bool update() override;
        TGUI_NODISCARD bool supportsSystemIcons() const override;
        TGUI_NODISCARD bool hasGenericIcons() const override;
        TGUI_NODISCARD Texture getGenericFileIcon(const Filesystem::FileInfo& file) override;
        void requestFileIcons(const std::vector<Filesystem::FileInfo>&) override;
        TGUI_NODISCARD std::vector<Texture> retrieveFileIcons() override;

    private:
        void initMagic();
        void loadMIMEToIconsMap(const String& filename);
        void loadIconPaths();
        void readGlobFile();
        void preloadLookupTables();
        void loadFileIcons();

    private:
        bool m_lookupTablesLoaded = false;

        bool m_magicInitialized = false;
        void* m_magicDllHandle = nullptr;
        magic_t m_magicSet = nullptr;
        decltype(&magic_file) m_magicFileFunc = nullptr;
        decltype(&magic_close) m_magicCloseFunc = nullptr;

        std::vector<String> m_dataDirs;
        std::map<String, std::vector<String>> m_extensionToMimeMap;
        std::map<String, String> m_mimeToIconMap;
        std::map<String, Filesystem::Path> m_iconNameToIconPathMap;

        std::map<String, Texture> m_iconCache;

        std::thread m_preloadLookupTablesThread;
        std::thread m_loadFileIconsThread;
        std::atomic<bool> m_preloadLookupTablesThreadFinished;
        std::atomic<bool> m_loadFileIconsThreadFinished;
        bool m_preloadLookupTablesThreadStarted = false;
        bool m_loadFileIconsThreadStarted = false;

        std::vector<Filesystem::FileInfo> m_files;
        std::vector<std::pair<String, String>> m_fileIcons;

        std::atomic<bool> m_cancelThreads;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<FileDialogIconLoader> FileDialogIconLoader::createInstance()
    {
        return std::make_shared<FileDialogIconLoaderLinux>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogIconLoaderLinux::FileDialogIconLoaderLinux() :
        m_preloadLookupTablesThreadFinished(false),
        m_loadFileIconsThreadFinished(false),
        m_cancelThreads(false)
    {
        const char* dataHomeDir = std::getenv("XDG_DATA_HOME");
        if (dataHomeDir && dataHomeDir[0])
            m_dataDirs.emplace_back(dataHomeDir);
        else
            m_dataDirs.emplace_back((Filesystem::getHomeDirectory() / ".local" / "share").asString());

        const char* dataDirs = std::getenv("XDG_DATA_DIRS");
        if (dataDirs && dataDirs[0])
        {
            for (const String& dir : String(dataDirs).split(':'))
            {
                if (!dir.empty())
                    m_dataDirs.emplace_back(dir);
            }
        }

        if (m_dataDirs.empty())
        {
            m_dataDirs.emplace_back("/usr/local/share/");
            m_dataDirs.emplace_back("/usr/share/");
        }

        initMagic();

        m_preloadLookupTablesThreadStarted = true;
        m_preloadLookupTablesThread = std::thread(&FileDialogIconLoaderLinux::preloadLookupTables, this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogIconLoaderLinux::~FileDialogIconLoaderLinux()
    {
        if (m_magicSet && m_magicCloseFunc)
            m_magicCloseFunc(m_magicSet);

        if (m_magicDllHandle)
            dlclose(m_magicDllHandle);

        if (m_preloadLookupTablesThreadStarted || m_loadFileIconsThreadStarted)
            m_cancelThreads = true;

        if (m_preloadLookupTablesThreadStarted)
            m_preloadLookupTablesThread.join();

        if (m_loadFileIconsThreadStarted)
            m_loadFileIconsThread.join();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialogIconLoaderLinux::update()
    {
        if (m_preloadLookupTablesThreadStarted) // Only bother checking the thread status if it could be running
        {
            // Check if the thread is still running with an atomic operation
            if (m_preloadLookupTablesThreadFinished)
            {
                m_preloadLookupTablesThreadStarted = false; // We no longer need to check the thread again
                m_preloadLookupTablesThread.join();

                if (m_iconNameToIconPathMap.empty())
                    return false; // The thread has finished but it failed so nothing was loaded

                m_lookupTablesLoaded = true;

                // If icons were already requested then immediately start handling the request
                if (!m_files.empty())
                {
                    requestFileIcons(m_files);
                    return false; // Wait until icons are loaded
                }
                else // No file list was provided for detailed icons, but generic file/folder icons can now be used
                    return true;
            }
        }
        else if (m_loadFileIconsThreadStarted)
        {
            // Check if the thread is still running with an atomic operation
            if (m_loadFileIconsThreadFinished)
            {
                m_loadFileIconsThreadStarted = false; // We no longer need to check the thread again
                m_loadFileIconsThread.join();
                return true;
            }
        }

        // No thread has finished executing this frame, so there is nothing to do
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialogIconLoaderLinux::supportsSystemIcons() const
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialogIconLoaderLinux::hasGenericIcons() const
    {
        return m_lookupTablesLoaded;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture FileDialogIconLoaderLinux::getGenericFileIcon(const Filesystem::FileInfo& file)
    {
        auto pathIt = m_iconNameToIconPathMap.end();
        if (file.directory)
        {
            pathIt = m_iconNameToIconPathMap.find(U"folder");
            if (pathIt == m_iconNameToIconPathMap.end())
                pathIt = m_iconNameToIconPathMap.find(U"inode-directory");
        }
        else
        {
            pathIt = m_iconNameToIconPathMap.find("text-x-generic");
            if (pathIt == m_iconNameToIconPathMap.end())
                pathIt = m_iconNameToIconPathMap.find("text-plain");
        }

        if (pathIt == m_iconNameToIconPathMap.end())
            return {}; // No generic icon exists

        // If the icon is already cached then we don't have to load it again.
        // The icon will also be cached by the texture manager, but that cache is cleared each time a new
        // folder is opened (because the icons wouldn't be in use any more for a moment), while this cache
        // keeps all icons until the file dialog is destroyed.
        const auto iconIt = m_iconCache.find(pathIt->first);
        if (iconIt != m_iconCache.end())
            return iconIt->second;

        // The icon exists but isn't in the cache yet, so load it and add it to the cache
        try
        {
            Texture iconTexture(pathIt->second.asString());
            m_iconCache[pathIt->first] = iconTexture;
            return iconTexture;
        }
        catch (const Exception&)
        {
            // Failed to load the icon. Add an empty texture to the cache to prevent attempting to load it again
            m_iconCache[pathIt->first] = {};
            return {};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialogIconLoaderLinux::requestFileIcons(const std::vector<Filesystem::FileInfo>& files)
    {
        // If the thread was already running then cancel it
        if (m_loadFileIconsThreadStarted)
        {
            m_cancelThreads = true;
            m_loadFileIconsThread.join();

            m_cancelThreads = false;
            m_loadFileIconsThreadStarted = false;
            m_loadFileIconsThreadFinished = false;
            m_fileIcons.clear();
        }

        // Now that any previous thread is cancelled, we can safely change the requested files
        m_files = files;

        // If the initial preloading thread is still running then we can't do anything here.
        // As soon as the thread finishes (assuming it hasn't failed), this function will be
        // called again.
        if (!m_lookupTablesLoaded)
            return;

        m_loadFileIconsThreadFinished = false;
        m_loadFileIconsThreadStarted = true;
        m_loadFileIconsThread = std::thread(&FileDialogIconLoaderLinux::loadFileIcons, this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<Texture> FileDialogIconLoaderLinux::retrieveFileIcons()
    {
        std::vector<Texture> icons;
        if (m_loadFileIconsThreadStarted)
            return icons; // Thread is still running, this shouldn't happen

        for (const auto& pair : m_fileIcons)
        {
            const String& iconName = pair.first;
            const String& filename = pair.second;

            if (filename.empty())
            {
                icons.emplace_back();
                continue;
            }

            // If the icon is already cached then we don't have to load it again.
            // The icon will also be cached by the texture manager, but that cache is cleared each time a new
            // folder is opened (because the icons wouldn't be in use any more for a moment), while this cache
            // keeps all icons until the file dialog is destroyed.
            const auto iconIt = m_iconCache.find(iconName);
            if (iconIt != m_iconCache.end())
            {
                icons.push_back(iconIt->second);
                continue;
            }

            // The icon exists but isn't in the cache yet, so load it and add it to the cache
            try
            {
                Texture iconTexture(filename);
                m_iconCache[iconName] = iconTexture;
                icons.emplace_back(std::move(iconTexture));
            }
            catch (const Exception&)
            {
                // Failed to load the icon. Add an empty texture to the cache to prevent attempting to load it again
                m_iconCache[iconName] = {};
                icons.emplace_back();
            }
        }

        m_files.clear();
        return icons;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialogIconLoaderLinux::initMagic()
    {
        // There might not be a "libmagic.so" file (seen on Kubuntu 20.04), so we search for "libmagic.so.1" instead.
        // Both "libmagic.so.1" and "libmagic.so.1.0.0" existed on all tested systems.
        m_magicDllHandle = dlopen("libmagic.so.1", RTLD_NOW);
        if (!m_magicDllHandle)
            return;

        auto magicOpenFunc = reinterpret_cast<decltype(&magic_open)>(dlsym(m_magicDllHandle, "magic_open"));
        auto magicLoadFunc = reinterpret_cast<decltype(&magic_load)>(dlsym(m_magicDllHandle, "magic_load"));
        m_magicFileFunc = reinterpret_cast<decltype(&magic_file)>(dlsym(m_magicDllHandle, "magic_file"));
        m_magicCloseFunc = reinterpret_cast<decltype(&magic_close)>(dlsym(m_magicDllHandle, "magic_close"));
        if (!magicOpenFunc || !magicLoadFunc || !m_magicFileFunc || !m_magicCloseFunc)
            return;

        m_magicSet = magicOpenFunc(MAGIC_MIME_TYPE);
        if (!m_magicSet)
            return;

        if (magicLoadFunc(m_magicSet, nullptr) != 0)
            return;

        m_magicInitialized = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Called within separate thread
    void FileDialogIconLoaderLinux::loadMIMEToIconsMap(const String& filename)
    {
        std::string line;
        std::ifstream fileStream(filename.toStdString());
        while (std::getline(fileStream, line))
        {
            const std::size_t colonPos = line.find(':');
            if (colonPos == std::string::npos)
                continue;

            // No mutex needed, m_mimeToIconMap is not accessed by main thread until loading is complete
            m_mimeToIconMap[line.substr(0, colonPos)] = line.substr(colonPos + 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Called within separate thread
    void FileDialogIconLoaderLinux::loadIconPaths()
    {
        // There seems to be no standardized or reliable way to figure out what the current selected theme is.
        // Picking the right folders and icon sizes is also complicated (altough this part is standardized),
        // so for now we just hardcode some directories to be able to see some icons on most distros.
        // The "places" folder contains the folder icon, which is why only loading "mimetypes" isn't enough.

        std::vector<Filesystem::Path> potentialIconPaths = {
            Filesystem::Path("/usr/share/icons/elementary-xfce/mimes/32/"),
            Filesystem::Path("/usr/share/icons/elementary-xfce/places/32/"),
            Filesystem::Path("/usr/share/icons/Faenza/mimetypes/32/"),
            Filesystem::Path("/usr/share/icons/Faenza/places/32/"),
            Filesystem::Path("/usr/share/icons/bloom/mimetypes/32/"), // Deepin
            Filesystem::Path("/usr/share/icons/bloom/places/32/"),
            Filesystem::Path("/usr/share/icons/nuoveXT2/32x32/mimetypes/"), // LXDE
            Filesystem::Path("/usr/share/icons/nuoveXT2/32x32/places/"),
            Filesystem::Path("/usr/share/icons/mate/32x32/mimetypes/"), // MATE
            Filesystem::Path("/usr/share/icons/mate/32x32/places/"),
            Filesystem::Path("/usr/share/icons/breeze/mimetypes/32/"), // KDE
            Filesystem::Path("/usr/share/icons/breeze/places/32/"),
            Filesystem::Path("/usr/share/icons/Adwaita/32x32/mimetypes/"), // GTK3 default theme
            Filesystem::Path("/usr/share/icons/Adwaita/32x32/places/"),
            Filesystem::Path("/usr/share/icons/Adwaita/scalable/mimetypes"),
            Filesystem::Path("/usr/share/icons/gnome/32x32/mimetypes/"),
            Filesystem::Path("/usr/share/icons/gnome/32x32/places/"),
            Filesystem::Path("/usr/share/icons/hicolor/48x48/mimetypes/"), // Fallback theme (mostly useless)
            Filesystem::Path("/usr/share/icons/hicolor/48x48/places/"),
        };

        std::map<String, std::pair<Filesystem::Path, int>> foundIcons;
        for (std::size_t i = 0; i < potentialIconPaths.size(); ++i)
        {
            const Filesystem::Path& iconDir = potentialIconPaths[i];
            if (!Filesystem::directoryExists(iconDir))
                continue;

            if (m_cancelThreads)
                return;

            const std::size_t themePriority = (potentialIconPaths.size() - i);

            auto icons = Filesystem::listFilesInDirectory(iconDir);
            for (const auto& iconFile : icons)
            {
                if (iconFile.directory || (iconFile.filename.length() <= 4))
                    continue;

                const bool isSvg = iconFile.filename.compare(iconFile.filename.length() - 4, 4, U".svg") == 0;
                const bool isPng = iconFile.filename.compare(iconFile.filename.length() - 4, 4, U".png") == 0;
                if (!isSvg && !isPng)
                    continue;

                // If both an SVG and a PNG file exist then we will select the svg file
                int themeIconPriority = 10 * themePriority;
                if (isSvg)
                    themeIconPriority += 2;
                else if (isPng)
                    themeIconPriority += 1;

                const String iconName = iconFile.filename.substr(0, iconFile.filename.length() - 4);

                // Only add the icon if it wasn't already there (with an equal or higher priority)
                const auto it = foundIcons.find(iconName);
                if (it != foundIcons.end())
                {
                    if (it->second.second >= themeIconPriority)
                        continue;
                }

                foundIcons[iconName] = std::make_pair(iconDir / iconFile.filename, themeIconPriority);

                // The generic-icons file in the mime folder misses a lot of icons even though the icons exist.
                // Many icon filenames match the MIME-type, so we make use of this to extend our icon mapping.
                const auto firstDashPos = iconName.find('-');
                if (firstDashPos != String::npos)
                {
                    const String category = iconName.substr(0, firstDashPos);
                    if (category != U"x")
                        m_mimeToIconMap[category + '/' + iconName.substr(firstDashPos + 1)] = iconName;
                }
            }
        }

        // Store the icons that had the highest priority (those with lower priorities have already been overwritten).
        // No mutex is needed, m_iconNameToIconPathMap is not accessed by main thread until loading is complete
        for (const auto& entry : foundIcons)
        {
            const String& iconName = entry.first;
            const Filesystem::Path& iconPath = entry.second.first;
            m_iconNameToIconPathMap[iconName] = iconPath;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Called within separate thread
    void FileDialogIconLoaderLinux::readGlobFile()
    {
        // We don't actually follow the specification (https://specifications.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-latest.html),
        // instead we use these files to load the right icon most of the time.

        std::ifstream fileStream;

        // Find the first valid globs file
        bool version2 = false;
        for (const auto& dir : m_dataDirs)
        {
            fileStream.open((dir + "/mime/globs2").toStdString());
            if (fileStream.good())
            {
                version2 = true;
                break;
            }

            fileStream.open((dir + "/mime/globs").toStdString());
            if (fileStream.good())
                break;
        }

        std::string line;
        while (std::getline(fileStream, line))
        {
            // Ignore comments
            if (line.empty() || line[0] == '#')
                continue;

            const std::vector<String> parts = String(line).split(':');

            const String* mimePart;
            const String* patternPart;
            if (version2)
            {
                // Ignore invalid lines or lines that are case-sensitive (which have ":cs" at the end)
                if ((parts.size() != 3) || parts[1].empty() || parts[2].empty())
                    continue;

                mimePart = &parts[1];
                patternPart = &parts[2];
            }
            else // Old globs file
            {
                // Ignore invalid lines
                if ((parts.size() != 2) || parts[0].empty() || parts[1].empty())
                    continue;

                mimePart = &parts[0];
                patternPart = &parts[1];
            }

            // Ignore patterns that match something other than the extension
            if ((patternPart->length() < 3) || ((*patternPart)[0] != '*') || ((*patternPart)[1] != '.'))
                continue;

            m_extensionToMimeMap[patternPart->substr(2)].push_back(*mimePart);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Function executed in separate thread
    void FileDialogIconLoaderLinux::preloadLookupTables()
    {
        // Loop over base directories in reverse order so that e.g. the first directory in the
        // list can overwrite settings from the second directory.
        // No mutex needed, m_dataDirs is initialized before thread is started and it never changed
        for (auto it = m_dataDirs.crbegin(); it != m_dataDirs.crend(); ++it)
        {
            loadMIMEToIconsMap(*it + "/mime/generic-icons");
            loadMIMEToIconsMap(*it + "/mime/icons");
        }

        if (m_cancelThreads)
            return;

        loadIconPaths();

        if (m_cancelThreads)
            return;

        readGlobFile();

        m_preloadLookupTablesThreadFinished = true; // Inform the main thread that we are done, using an atomic operation
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Function executed in separate thread
    void FileDialogIconLoaderLinux::loadFileIcons()
    {
        m_fileIcons.clear();
        m_fileIcons.reserve(m_files.size());
        for (const auto& file : m_files)
        {
            if (m_cancelThreads)
                return;

            String firstMimeMatch;
            std::vector<String> mimeTypesBasedOnGlob;

            // Find the mime type based on the file extension
            auto pos = file.filename.find('.');
            while ((pos != String::npos) && (pos + 1 < file.filename.length()))
            {
                const auto extIt = m_extensionToMimeMap.find(file.filename.substr(pos + 1).toLower());
                if (extIt != m_extensionToMimeMap.end())
                {
                    mimeTypesBasedOnGlob = extIt->second;
                    break;
                }

                pos = file.filename.find('.', pos + 1);
            }

            auto pathIt = m_iconNameToIconPathMap.end();

            // If there is a single glob match then we use that one
            if (mimeTypesBasedOnGlob.size() == 1)
            {
                firstMimeMatch = mimeTypesBasedOnGlob[0];

                const auto mimeIt = m_mimeToIconMap.find(mimeTypesBasedOnGlob[0]);
                if (mimeIt != m_mimeToIconMap.end())
                    pathIt = m_iconNameToIconPathMap.find(mimeIt->second);
            }

            // If there are none or multipe glob matches then find out which icon to load based on the MIME type of the file
            const char* mimeTypeFromMagic = nullptr;
            if (m_magicInitialized)
                mimeTypeFromMagic = m_magicFileFunc(m_magicSet, file.path.asNativeString().c_str());
            if (mimeTypeFromMagic)
            {
                if (firstMimeMatch.empty())
                    firstMimeMatch = mimeTypeFromMagic;

                const auto mimeIt = m_mimeToIconMap.find(mimeTypeFromMagic);
                if (mimeIt != m_mimeToIconMap.end())
                    pathIt = m_iconNameToIconPathMap.find(mimeIt->second);
            }

            // If no match is found on mime type and there were multiple glob matches then just select the first match
            if ((pathIt == m_iconNameToIconPathMap.end()) && (mimeTypesBasedOnGlob.size() > 1))
            {
                for (const auto& mimeType : mimeTypesBasedOnGlob)
                {
                    if (firstMimeMatch.empty())
                        firstMimeMatch = mimeType;

                    const auto mimeIt = m_mimeToIconMap.find(mimeType);
                    if (mimeIt == m_mimeToIconMap.end())
                        continue;

                    pathIt = m_iconNameToIconPathMap.find(mimeIt->second);
                    if (pathIt != m_iconNameToIconPathMap.end())
                        break;
                }
            }

            // If no exact mime type could be found then try to use a generic one based on the category
            if ((pathIt == m_iconNameToIconPathMap.end()) && !firstMimeMatch.empty())
            {
                const auto slashPos = firstMimeMatch.find('/');
                if (slashPos != String::npos)
                {
                    const String iconName = firstMimeMatch.substr(0, slashPos) + U"-x-generic";
                    pathIt = m_iconNameToIconPathMap.find(iconName);
                }

                if (pathIt == m_iconNameToIconPathMap.end())
                {
                    // Executables are sometimes identified as "application/x-sharedlib". If there is no icon for such type then just use an
                    // icon for executables for files with such type.
                    // Executables were detected by libmagic as application/x-pie-executable, but this didn't exist in my /usr/share/mime.
                    // So if we couldn't find anything then at least don't show a text icon if "executable" is literally in the MIME type name.
                    if ((firstMimeMatch == U"application/x-sharedlib") || (firstMimeMatch.find(U"exec") != String::npos))
                        pathIt = m_iconNameToIconPathMap.find(U"application-x-executable");
                }
            }

            // If the icon can't be found then fall back to some hardcoded generic icons which hopefully will be found
            if (pathIt == m_iconNameToIconPathMap.end())
            {
                if (file.directory)
                {
                    pathIt = m_iconNameToIconPathMap.find(U"folder");
                    if (pathIt == m_iconNameToIconPathMap.end())
                        pathIt = m_iconNameToIconPathMap.find(U"inode-directory");
                }
                else
                {
                    pathIt = m_iconNameToIconPathMap.find("text-x-generic");
                    if (pathIt == m_iconNameToIconPathMap.end())
                        pathIt = m_iconNameToIconPathMap.find("text-plain");
                }
            }

            if (pathIt != m_iconNameToIconPathMap.end())
                m_fileIcons.emplace_back(pathIt->first, pathIt->second.asString());
            else
                m_fileIcons.emplace_back("", ""); // We couldn't find any icon, so we can't provide a texture
        }

        m_loadFileIconsThreadFinished = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // TGUI_SYSTEM_LINUX

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
