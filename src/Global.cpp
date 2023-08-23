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


#include <TGUI/Global.hpp>
#include <TGUI/Backend/Window/Backend.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <functional>
    #include <sstream>
    #include <locale>
    #include <limits>
#endif

#include <stdio.h> // C header for compatibility with _wfopen_s, NOLINT(modernize-deprecated-headers)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER == 20
namespace
{
    // Many compilers don't seem to support std::stringstream::view() yet, but I don't want to
    // limit the use of other c++20 features. So in c++20 mode we just check if the function exists.

    // This struct will be used when the view() member doesn't exist.
    template <typename StreamType, typename = void>
    struct StreamToStringViewConverter
    {
        // We can't return string_view because the string has to outlive the view,
        // but the returned string will be implicitly converted to a string_view later.
        TGUI_NODISCARD static std::string convert(const StreamType& stream)
        {
            return stream.str();
        }
    };

    // This struct will be used when the stringstream has a view() member function.
    template <typename StreamType>
    struct StreamToStringViewConverter<StreamType, std::void_t<decltype(&StreamType::view)>>
    {
        TGUI_NODISCARD static std::string_view convert(const StreamType& stream)
        {
            return stream.view();
        }
    };
}
#endif

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
            // On Windows, we use _wfopen_s with MSVC, MinGW-w64 and Clang (both LLVM Clang and Clang-CL).
            // With MinGW.org based TDM-GCC, we can't use _wfopen_s so we call _wfopen if the function is defined (i.e. if __STRICT_ANSI__ is undefined).
            // If _wfopen is unavailable, we simply use fopen (and hope that the system uses UTF-8 or that filename only contains ASCII characters).
#if defined(TGUI_SYSTEM_WINDOWS) && (defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR) || defined(__clang__))
            FILE* rawFilePtr = nullptr;
            if (_wfopen_s(&rawFilePtr, filename.toWideString().c_str(), L"rb") != 0)
                return nullptr;
#elif defined(TGUI_SYSTEM_WINDOWS) && !defined(__STRICT_ANSI__)
            FILE* rawFilePtr = _wfopen(filename.toWideString().c_str(), L"rb");
#else
            FILE* rawFilePtr = fopen(filename.toStdString().c_str(), "rb");
#endif
            if (!rawFilePtr)
                return nullptr;

            auto closeFileFunc = [](FILE* fp){ fclose(fp); };
            std::unique_ptr<FILE, decltype(closeFileFunc)> file(rawFilePtr, closeFileFunc);

            if (fseek(file.get(), 0, SEEK_END) != 0)
                return nullptr;

            const long bytesInFile = ftell(file.get());

            if (fseek(file.get(), 0, SEEK_SET) != 0)
                return nullptr;

            if ((bytesInFile <= 0) || (bytesInFile == std::numeric_limits<long>::max()))
            {
                // When filename was a directory on linux, ftell returned the maximum long value instead of -1
                return nullptr;
            }

            fileSize = static_cast<std::size_t>(bytesInFile);
            auto buffer = MakeUniqueForOverwrite<std::uint8_t[]>(fileSize);
            if (fread(buffer.get(), 1, fileSize, file.get()) != fileSize)
                return nullptr;

            return buffer;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool writeFile(const String& filename, const std::stringstream& stream)
    {
#if TGUI_COMPILED_WITH_CPP_VER > 20
        return writeFile(filename, stream.view());
#elif TGUI_COMPILED_WITH_CPP_VER == 20
        const auto& stringOrView = StreamToStringViewConverter<std::stringstream>::convert(stream);
        return writeFile(filename, stringOrView);
#else
        return writeFile(filename, stream.str());
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool writeFile(const String& filename, CharStringView stringView)
    {
        // On Windows, we use _wfopen_s with MSVC, MinGW-w64 and Clang (both LLVM Clang and Clang-CL).
        // With MinGW.org based TDM-GCC, we can't use _wfopen_s so we call _wfopen if the function is defined (i.e. if __STRICT_ANSI__ is undefined).
        // If _wfopen is unavailable, we simply use fopen (and hope that the system uses UTF-8 or that filename only contains ASCII characters).
#if defined(TGUI_SYSTEM_WINDOWS) && (defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR) || defined(__clang__))
        FILE* file = nullptr;
        if (_wfopen_s(&file, filename.toWideString().c_str(), L"w") != 0)
            return false;
#elif defined(TGUI_SYSTEM_WINDOWS) && !defined(__STRICT_ANSI__)
        FILE* file = _wfopen(filename.toWideString().c_str(), L"w");
#else
        FILE* file = fopen(filename.toStdString().c_str(), "w");
#endif
        if (!file)
            return false;

        const bool success = (fwrite(stringView.data(), 1, stringView.size(), file) == stringView.size());

        (void)fclose(file);
        return success;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
