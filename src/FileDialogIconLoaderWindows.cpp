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

#if defined(TGUI_SYSTEM_WINDOWS)

#include <TGUI/extlibs/IncludeWindows.hpp>

#if defined(__has_include)
    #if __has_include (<shellapi.h>)
        #define TGUI_SHELL_API_HEADER_INCLUDED
        #include <shellapi.h>

        // MinGW.org based TDM-GCC doesn't define SHGFI_ADDOVERLAYS
        #ifndef SHGFI_ADDOVERLAYS
            #define SHGFI_ADDOVERLAYS   0x000000020
        #endif
    #endif
#endif

#if !defined(TGUI_SHELL_API_HEADER_INCLUDED)
    // We define the required contents of shellapi.h ourselves if the Windows API can't be found
    #ifndef SHSTDAPI_
        #if !defined(_SHELL32_)
            #define SHSTDAPI_(type)   extern "C" DECLSPEC_IMPORT type STDAPICALLTYPE
        #else
            #define SHSTDAPI_(type)   STDAPI_(type)
        #endif
    #endif

    #define SHGFI_ICON              0x000000100     // get icon
    #define SHGFI_LARGEICON         0x000000000     // get large icon
    #define SHGFI_USEFILEATTRIBUTES 0x000000010     // use passed dwFileAttribute
    #define SHGFI_ADDOVERLAYS       0x000000020     // apply the appropriate overlays

    typedef struct _SHFILEINFOW
    {
        HICON hIcon;
        int   iIcon;
        DWORD dwAttributes;
        WCHAR szDisplayName[MAX_PATH];
        WCHAR szTypeName[80];
    } SHFILEINFOW;

    SHSTDAPI_(DWORD_PTR) SHGetFileInfoW(_In_ LPCWSTR pszPath, DWORD dwFileAttributes, _Inout_updates_bytes_opt_(cbFileInfo) SHFILEINFOW* psfi, UINT cbFileInfo, UINT uFlags);
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class FileDialogIconLoaderWindows : public FileDialogIconLoader
    {
    public:
        FileDialogIconLoaderWindows();
        ~FileDialogIconLoaderWindows() override;

        TGUI_NODISCARD bool update() override;
        TGUI_NODISCARD bool supportsSystemIcons() const override;
        TGUI_NODISCARD bool hasGenericIcons() const override;
        TGUI_NODISCARD Texture getGenericFileIcon(const Filesystem::FileInfo& file) override;
        void requestFileIcons(const std::vector<Filesystem::FileInfo>&) override;
        TGUI_NODISCARD std::vector<Texture> retrieveFileIcons() override;

    private:
        struct IconData
        {
            std::unique_ptr<std::uint8_t[]> pixels = nullptr;
            unsigned int width = 0;
            unsigned int height = 0;
        };

        struct ThreadData
        {
            decltype(&SHGetFileInfoW) dllGetFileInfoFuncHandle = nullptr;
            std::vector<Filesystem::FileInfo> files; // List of files for which the thread should load the icons
            std::vector<IconData> icons; // List of loaded icons
            HANDLE cancelEvent = nullptr;
            HANDLE finishedEvent = nullptr;
        };

        TGUI_NODISCARD static IconData loadIconPixels(decltype(&SHGetFileInfoW) dllGetFileInfoFuncHandle, const String& filename, bool isDirectory);
        TGUI_NODISCARD static Texture loadIcon(decltype(&SHGetFileInfoW) dllGetFileInfoFuncHandle, const String& filename, bool isDirectory);
        static DWORD WINAPI loadIconsThread(void* parameter);

    private:
        HMODULE m_dllModuleHandle = nullptr;
        decltype(&SHGetFileInfoW) m_dllGetFileInfoFuncHandle = nullptr;

        Optional<Texture> m_defaultFileIcon;
        Optional<Texture> m_defaultFolderIcon;

        bool m_threadStarted = false; // Is the thread potentially running?

        HANDLE m_thread = nullptr;
        HANDLE m_cancelEvent = nullptr;
        HANDLE m_finishedEvent = nullptr;
        ThreadData m_threadData;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<FileDialogIconLoader> FileDialogIconLoader::createInstance()
    {
        return std::make_shared<FileDialogIconLoaderWindows>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogIconLoaderWindows::FileDialogIconLoaderWindows()
    {
        m_dllModuleHandle = LoadLibraryW(L"shell32.dll");
        if (m_dllModuleHandle)
        {
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
            m_dllGetFileInfoFuncHandle = reinterpret_cast<decltype(&SHGetFileInfoW)>(GetProcAddress(m_dllModuleHandle, "SHGetFileInfoW"));
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
        }

        if (!m_dllGetFileInfoFuncHandle)
            return;

        m_cancelEvent = CreateEventW(nullptr, true, false, nullptr);
        m_finishedEvent = CreateEventW(nullptr, true, false, nullptr);

        m_threadData.dllGetFileInfoFuncHandle = m_dllGetFileInfoFuncHandle;
        m_threadData.cancelEvent = m_cancelEvent;
        m_threadData.finishedEvent = m_finishedEvent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogIconLoaderWindows::~FileDialogIconLoaderWindows()
    {
        // If the thread is still running then stop it
        if (m_threadStarted)
            SignalObjectAndWait(m_cancelEvent, m_thread, INFINITE, false);

        if (m_finishedEvent)
            CloseHandle(m_finishedEvent);

        if (m_cancelEvent)
            CloseHandle(m_cancelEvent);

        if (m_dllModuleHandle)
            FreeLibrary(m_dllModuleHandle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture FileDialogIconLoaderWindows::getGenericFileIcon(const Filesystem::FileInfo& file)
    {
        if (file.directory)
        {
            if (!m_defaultFolderIcon)
                m_defaultFolderIcon = loadIcon(m_dllGetFileInfoFuncHandle, "DefaultFolderIcon", true); // The string passed to this function can be anything

            return *m_defaultFolderIcon;
        }
        else
        {
            if (!m_defaultFileIcon)
                m_defaultFileIcon = loadIcon(m_dllGetFileInfoFuncHandle, "DefaultFileIcon", false); // The string passed to this function can be anything

            return *m_defaultFileIcon;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FileDialogIconLoaderWindows::requestFileIcons(const std::vector<Filesystem::FileInfo>& files)
    {
        if (!m_dllGetFileInfoFuncHandle || !m_cancelEvent || !m_finishedEvent)
            return;

        if (m_threadStarted)
        {
            TGUI_ASSERT(m_thread != nullptr, "Thead handle has to be valid if thread is running");

            // Abort the old thread that was still running.
            // We don't check the return type, if it differs from WAIT_OBJECT_0 then we have a big problem
            SignalObjectAndWait(m_cancelEvent, m_thread, INFINITE, false);
            ResetEvent(m_cancelEvent);
        }

        ResetEvent(m_finishedEvent);

        m_threadData.icons.clear();
        m_threadData.files = files;
        m_threadStarted = true;
        m_thread = CreateThread(nullptr, 0, &loadIconsThread, &m_threadData, 0, nullptr);
        if (!m_thread)
        {
            m_threadStarted = false;
            m_threadData.files.clear();
            return;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialogIconLoaderWindows::update()
    {
        if (!m_threadStarted)
            return false;

        return WaitForSingleObject(m_finishedEvent, 0) == WAIT_OBJECT_0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialogIconLoaderWindows::supportsSystemIcons() const
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FileDialogIconLoaderWindows::hasGenericIcons() const
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<Texture> FileDialogIconLoaderWindows::retrieveFileIcons()
    {
        std::vector<Texture> icons;
        if (!m_threadStarted)
            return icons;

        if (WaitForSingleObject(m_finishedEvent, 0) != WAIT_OBJECT_0)
            return icons;

        for (const auto& iconData : m_threadData.icons)
        {
            TGUI_EMPLACE_BACK(texture, icons);
            if (iconData.pixels)
                texture.loadFromPixelData({iconData.width, iconData.height}, iconData.pixels.get()); // Don't care if this fails or succeeds, we add the texture either way
        }

        m_threadStarted = false;
        m_threadData.files.clear();

        return icons;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FileDialogIconLoaderWindows::IconData FileDialogIconLoaderWindows::loadIconPixels(decltype(&SHGetFileInfoW) dllGetFileInfoFuncHandle, const String& filename, bool isDirectory)
    {
        IconData iconData;
        if (!dllGetFileInfoFuncHandle)
            return iconData;

        SHFILEINFOW shellFileInfo;
        std::memset(&shellFileInfo, 0, sizeof(SHFILEINFOW));

        const DWORD fileAttributes = isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
        const UINT flags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_ADDOVERLAYS | SHGFI_LARGEICON;
        if (dllGetFileInfoFuncHandle(filename.toWideString().c_str(), fileAttributes, &shellFileInfo, sizeof(shellFileInfo), flags) == 0)
            return iconData;

        if (!shellFileInfo.hIcon)
            return iconData;

        ICONINFO iconInfo;
        if (!GetIconInfo(shellFileInfo.hIcon, &iconInfo))
        {
            DestroyIcon(shellFileInfo.hIcon);
            return iconData;
        }

        HDC hDC = nullptr;

        auto releaseResources = [&] {
            if (iconInfo.hbmColor)
                DeleteObject(iconInfo.hbmColor);

            if (iconInfo.hbmMask)
                DeleteObject(iconInfo.hbmMask);

            if (hDC)
                DeleteDC(hDC);

            DestroyIcon(shellFileInfo.hIcon);
        };

        BITMAP bitmap;
        if (GetObjectW(iconInfo.hbmColor, sizeof(BITMAP), &bitmap) == 0)
        {
            releaseResources();
            return iconData;
        }

        const DWORD iconWidth = static_cast<DWORD>(bitmap.bmWidth);
        const DWORD iconHeight = static_cast<DWORD>(bitmap.bmHeight);

        hDC = CreateCompatibleDC(nullptr);
        if (!hDC)
        {
            releaseResources();
            return iconData;
        }

        BITMAPINFO infoheader;
        infoheader.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        infoheader.bmiHeader.biWidth = static_cast<LONG>(iconWidth);
        infoheader.bmiHeader.biHeight = static_cast<LONG>(iconHeight);
        infoheader.bmiHeader.biPlanes = 1;
        infoheader.bmiHeader.biCompression = BI_RGB;

        if (bitmap.bmBitsPixel == 32)
        {
            const DWORD bitmapSize = iconWidth * iconHeight * 4;

            infoheader.bmiHeader.biBitCount = 32;
            infoheader.bmiHeader.biSizeImage = bitmapSize;

            // Allocate memory for the icon BGRA bitmap
            auto buffer = MakeUniqueForOverwrite<std::uint8_t[]>(2 * static_cast<std::size_t>(bitmapSize));
            std::uint8_t* pixelsBGRA = &buffer[0];

            if (!GetDIBits(hDC, iconInfo.hbmColor, 0, iconHeight, static_cast<void*>(pixelsBGRA), &infoheader, DIB_RGB_COLORS))
            {
                releaseResources();
                return iconData;
            }

            iconData.pixels = MakeUniqueForOverwrite<std::uint8_t[]>(static_cast<std::size_t>(iconWidth) * iconHeight * 4);
            iconData.width = iconWidth;
            iconData.height = iconHeight;

            std::uint8_t* imagePixels = iconData.pixels.get();
            for (DWORD y = 0; y < iconHeight; ++y)
            {
                for (DWORD x = 0; x < iconWidth; ++x)
                {
                    // Source pixels are stored in BGRA with the origin at the bottom left of the bitmap
                    const DWORD srcPos = ((iconHeight - 1 - y) * (iconWidth * 4)) + (x * 4);
                    const DWORD destPos = (y * iconWidth * 4) + (x * 4);
                    imagePixels[destPos] = pixelsBGRA[srcPos + 2]; // R
                    imagePixels[destPos + 1] = pixelsBGRA[srcPos + 1]; // G
                    imagePixels[destPos + 2] = pixelsBGRA[srcPos]; // B
                    imagePixels[destPos + 3] = pixelsBGRA[srcPos + 3]; // A
                }
            }
        }
        else // Icon bitmap doesn't contain an alpha channel
        {
            // Each scanline consists of 3 bytes per pixel, rounded upwards to multiple of 4
            const DWORD bytesPerScanLine = (iconWidth * 3 + 3) & 0xFFFFFFFC;
            const DWORD bitmapSize = bytesPerScanLine * iconHeight;

            infoheader.bmiHeader.biBitCount = 24;
            infoheader.bmiHeader.biSizeImage = bitmapSize;

            // Allocate memory for 2 bitmaps: the icon BGR bitmap and the icon mask bitmap
            auto buffer = MakeUniqueForOverwrite<std::uint8_t[]>(2 * static_cast<std::size_t>(bitmapSize));
            std::uint8_t* pixelsBGR = &buffer[0];
            std::uint8_t* pixelsAlpha = &buffer[bitmapSize];

            if (!GetDIBits(hDC, iconInfo.hbmColor, 0, iconHeight, static_cast<void*>(pixelsBGR), &infoheader, DIB_RGB_COLORS))
            {
                releaseResources();
                return iconData;
            }

            if (!GetDIBits(hDC, iconInfo.hbmMask, 0, iconHeight, static_cast<void*>(pixelsAlpha), &infoheader, DIB_RGB_COLORS))
            {
                releaseResources();
                return iconData;
            }

            iconData.pixels = MakeUniqueForOverwrite<std::uint8_t[]>(static_cast<std::size_t>(iconWidth) * iconHeight * 4);
            iconData.width = iconWidth;
            iconData.height = iconHeight;

            std::uint8_t* imagePixels = iconData.pixels.get();
            for (DWORD y = 0; y < iconHeight; ++y)
            {
                for (DWORD x = 0; x < iconWidth; ++x)
                {
                    // Source pixels are stored in BGR with the origin at the bottom left of the bitmap
                    const DWORD srcPos = ((iconHeight - 1 - y) * (iconWidth * 3)) + (x * 3);
                    const DWORD destPos = (y * iconWidth * 4) + (x * 4);
                    imagePixels[destPos] = pixelsBGR[srcPos + 2]; // R
                    imagePixels[destPos + 1] = pixelsBGR[srcPos + 1]; // G
                    imagePixels[destPos + 2] = pixelsBGR[srcPos]; // B
                    imagePixels[destPos + 3] = (pixelsAlpha[srcPos] ? 0 : 255); // A
                }
            }
        }

        releaseResources();
        return iconData;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture FileDialogIconLoaderWindows::loadIcon(decltype(&SHGetFileInfoW) dllGetFileInfoFuncHandle, const String& filename, bool isDirectory)
    {
        const auto iconData = loadIconPixels(dllGetFileInfoFuncHandle, filename, isDirectory);

        Texture texture;
        if (iconData.pixels)
            texture.loadFromPixelData({iconData.width, iconData.height}, iconData.pixels.get()); // Don't care if this fails or succeeds, we return the texture anyway

        return texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DWORD WINAPI FileDialogIconLoaderWindows::loadIconsThread(void* parameter)
    {
        TGUI_ASSERT(parameter != nullptr, "Thread parameter can't be a nullptr");
        auto threadData = static_cast<ThreadData*>(parameter);

        threadData->icons.clear();
        threadData->icons.reserve(threadData->files.size());
        for (const auto& file : threadData->files)
        {
            if (WaitForSingleObject(threadData->cancelEvent, 0) == WAIT_OBJECT_0)
                return 0; // Abort execution as the user no longer needs the icons that we were loading

            threadData->icons.push_back(loadIconPixels(threadData->dllGetFileInfoFuncHandle, file.filename, file.directory));
        }

        SetEvent(threadData->finishedEvent);
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // TGUI_SYSTEM_WINDOWS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
