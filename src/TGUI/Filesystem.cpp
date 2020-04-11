/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Filesystem.hpp>
#include <cstdlib> // getenv
#if !defined(TGUI_SYSTEM_WINDOWS)
    #include <unistd.h> // getuid
    #include <pwd.h> // getpwuid
#endif

#if !defined(TGUI_USE_STD_FILESYSTEM)
    #if defined(TGUI_SYSTEM_WINDOWS)
        #ifndef NOMINMAX // MinGW already defines this which causes a warning without this check
            #define NOMINMAX
        #endif
        #define NOMB
        #define VC_EXTRALEAN
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #else
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <errno.h>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path::Path(const String& path)
#ifdef TGUI_USE_STD_FILESYSTEM
        : m_path(std::u32string(path))
#endif
    {
#if !defined(TGUI_USE_STD_FILESYSTEM)
        String part;
        for (const char32_t c : path)
        {
    #if defined(TGUI_SYSTEM_WINDOWS)
            if ((c == '/') || (c == '\\'))
    #else
            if (c == '/')
    #endif
            {
                // If this is the first part then check if we are parsing an absolute path
                if (m_parts.empty() && !m_absolute && m_root.empty())
                {
    #if defined(TGUI_SYSTEM_WINDOWS)
                    if ((part.empty()) || ((part.length() == 2) && (part[1] == U':')))
                    {
                        m_root = part;
                        m_absolute = true;
                    }
    #else
                    if (part.empty())
                        m_absolute = true;
    #endif
                    else // The path is relative, there is no root part
                        m_parts.push_back(part);
                }
                else // There have already been other parts, so just add this one
                {
                    if (!part.empty())
                        m_parts.push_back(part);
                }

                part.clear();
            }
            else
                part.push_back(c);
        }

        if (!part.empty())
            m_parts.push_back(part);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::Path::isEmpty() const
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.empty();
#else
        return !m_absolute && m_root.empty() && m_parts.empty();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Filesystem::Path::asString() const
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.generic_u32string();
#else
        String str = m_root;
        if (m_absolute)
            str += U'/';

        if (!m_parts.empty())
            str += m_parts[0];

        for (unsigned int i = 1; i < m_parts.size(); ++i)
            str += U'/' + m_parts[i];

        return str;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path Filesystem::Path::getParentPath() const
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        Path parentPath;
        parentPath.m_path = m_path.parent_path();
#else
        Path parentPath(*this);
        if (!parentPath.m_parts.empty())
            parentPath.m_parts.pop_back();
#endif
        return parentPath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_SYSTEM_WINDOWS
    std::wstring Filesystem::Path::asNativeString() const
    {
    #ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.wstring();
    #else
        String str = m_root;
        if (m_absolute)
            str += U'\\';

        if (!m_parts.empty())
            str += m_parts[0];

        for (unsigned int i = 1; i < m_parts.size(); ++i)
            str += U'\\' + m_parts[i];

        return std::wstring(str);

    #endif
    }
#else
    std::string Filesystem::Path::asNativeString() const
    {
    #ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.generic_string();
    #else
        return std::string(asString());
    #endif
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path Filesystem::Path::operator/(const Path& path) const
    {
        return Path(*this) /= path;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path& Filesystem::Path::operator/=(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        m_path /= path.m_path;
#else
        if (!path.m_absolute)
            m_parts.insert(m_parts.end(), path.m_parts.begin(), path.m_parts.end());
        else // The path to append is absolute, so just replace the current path
            *this = path;
#endif
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::directoryExists(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return std::filesystem::is_directory(path);
#elif defined(TGUI_SYSTEM_WINDOWS)
        const DWORD attrib = GetFileAttributesW(path.asNativeString().c_str());
        return ((attrib != INVALID_FILE_ATTRIBUTES) && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat fileInfo;
        return (stat(path.asNativeString().c_str(), &fileInfo) == 0) && (fileInfo.st_mode & S_IFDIR);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::fileExists(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return std::filesystem::exists(path);
#elif defined(TGUI_SYSTEM_WINDOWS)
        const DWORD attrib = GetFileAttributesW(path.asNativeString().c_str());
        return attrib != INVALID_FILE_ATTRIBUTES;
#else
        struct stat fileInfo;
        return stat(path.asNativeString().c_str(), &fileInfo) == 0;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::createDirectory(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return std::filesystem::create_directory(path);
#elif defined(TGUI_SYSTEM_WINDOWS)
        const DWORD status = CreateDirectoryW(path.asNativeString().c_str(), NULL);
        return (status != 0) || (GetLastError() == ERROR_ALREADY_EXISTS);
#else
        const int status = mkdir(path.asNativeString().c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        return (status == 0) || (errno == EEXIST);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path Filesystem::getLocalDataDirectory()
    {
        tgui::Filesystem::Path localDataDir;
#ifdef TGUI_SYSTEM_WINDOWS
    #if defined (_MSC_VER)
        wchar_t* appDataDir;
        size_t len;
        if ((_wdupenv_s(&appDataDir, &len, L"LOCALAPPDATA") == 0) && appDataDir)
        {
            localDataDir = tgui::Filesystem::Path(std::wstring(appDataDir));
            free(appDataDir);
        }
    #else
        const char* appDataDir = std::getenv("LOCALAPPDATA");
        if (appDataDir)
            localDataDir = tgui::Filesystem::Path(appDataDir);
    #endif
#else
        const char* homeDir = std::getenv("HOME");
        if (!homeDir)
            homeDir = getpwuid(getuid())->pw_dir;
        if (homeDir)
        {
    #ifdef TGUI_SYSTEM_MACOS
            localDataDir = tgui::Filesystem::Path(homeDir) / U"Library" / U"Application Support";
    #else
            localDataDir = tgui::Filesystem::Path(homeDir) / U".local" / U"share";
    #endif
        }
#endif

        return localDataDir;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
