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


#include <TGUI/Global.hpp>
#include <TGUI/Backend.hpp>
#include <functional>
#include <sstream>
#include <locale>
#include <cctype> // isspace
#include <cmath> // abs
#include <stdio.h> // C header for compatibility with _wfopen_s

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        unsigned int globalTextSize = 13;
        Duration globalDoubleClickTime = std::chrono::milliseconds(500);
        Duration globalEditBlinkRate = std::chrono::milliseconds(500);
        Filesystem::Path globalResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setGlobalTextSize(unsigned int textSize)
    {
        globalTextSize = textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getGlobalTextSize()
    {
        return globalTextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setDoubleClickTime(Duration duration)
    {
        globalDoubleClickTime = duration;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Duration getDoubleClickTime()
    {
        return globalDoubleClickTime;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const Filesystem::Path& path)
    {
        globalResourcePath = path;

        // If e.g. "a/b" was stored in the path then assume "b" is a folder and turn it into "a/b/"
        if (!path.getFilename().empty())
            globalResourcePath = Filesystem::Path(path.asString() + '/');
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const String& path)
    {
        // Make sure the path ends with a slash, so that it is interpreted as a path instead of a filename
        if (!path.empty() && (path.back() != '/') && (path.back() != '\\'))
            globalResourcePath = Filesystem::Path(path + '/');
        else
            globalResourcePath = Filesystem::Path(path);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Filesystem::Path& getResourcePath()
    {
        return globalResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setEditCursorBlinkRate(Duration blinkRate)
    {
        globalEditBlinkRate = blinkRate;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Duration getEditCursorBlinkRate()
    {
        return globalEditBlinkRate;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<std::uint8_t[]> readFileToMemory(const String& filename, std::size_t& fileSize)
    {
#ifdef TGUI_SYSTEM_ANDROID
        // If the file does not start with a slash then load it from the assets
        if (!filename.empty() && (filename[0] != '/'))
        {
            return getBackend()->readFileFromAndroidAssets(filename, fileSize);
        }
        else
#endif
        {
#if defined(TGUI_SYSTEM_WINDOWS) // _wfopen_s is supported by MSVC and MinGW-w64
            FILE* file = nullptr;
            if (_wfopen_s(&file, filename.toWideString().c_str(), L"rb") != 0)
                return nullptr;
#else
            FILE*  file = fopen(filename.toStdString().c_str(), "rb");
#endif
            if (!file)
                return nullptr;

            fseek(file, 0, SEEK_END);
            const long bytesInFile = ftell(file);
            fseek(file, 0, SEEK_SET);
            if ((bytesInFile <= 0) || (bytesInFile == std::numeric_limits<long>::max()))
            {
                // When filename was a directory on linux, ftell returned the maximum long value instead of -1
                fclose(file);
                return nullptr;
            }

            fileSize = static_cast<std::size_t>(bytesInFile);
            auto buffer = std::make_unique<std::uint8_t[]>(fileSize);
            if (fread(buffer.get(), 1, fileSize, file) != fileSize)
            {
                fclose(file);
                return nullptr;
            }

            fclose(file);
            return buffer;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool writeFile(const String& filename, std::stringstream& stream)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 20
        return writeFile(filename, stream.view());
#else
        return writeFile(filename, stream.str());
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool writeFile(const String& filename, const CharStringView& stringView)
    {
#if defined(TGUI_SYSTEM_WINDOWS) // _wfopen_s is supported by MSVC and MinGW-w64
        FILE* file = nullptr;
        if (_wfopen_s(&file, filename.toWideString().c_str(), L"w") != 0)
            return false;
#else
        FILE* file = fopen(filename.toStdString().c_str(), "w");
#endif
        if (!file)
            return false;

        if (fwrite(stringView.data(), 1, stringView.size(), file) < stringView.size())
            return false;

        fclose(file);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
