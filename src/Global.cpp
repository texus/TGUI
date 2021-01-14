/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Duration.hpp>
#include <TGUI/Backend.hpp>
#include <functional>
#include <sstream>
#include <locale>
#include <cctype> // isspace
#include <cmath> // abs
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        unsigned int globalTextSize = 13;
        Duration globalDoubleClickTime = std::chrono::milliseconds(500);
        Duration globalEditBlinkRate = std::chrono::milliseconds(500);
        String globalResourcePath = "";
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

    void setResourcePath(const String& path)
    {
        globalResourcePath = path;

        if (!globalResourcePath.empty())
        {
            if (globalResourcePath[globalResourcePath.length()-1] != '/')
                globalResourcePath.push_back('/');
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& getResourcePath()
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
            FILE* file;
#if defined(_MSC_VER)
            if (_wfopen_s(&file, filename.toWideString().c_str(), L"rb") != 0)
                return nullptr;
#else
            file = fopen(filename.toStdString().c_str(), "rb");
            if (!file)
                return nullptr;
#endif
            fseek(file, 0, SEEK_END);
            const long bytesInFile = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (bytesInFile <= 0)
            {
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
