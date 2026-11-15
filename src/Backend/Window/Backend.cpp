////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Backend/Font/BackendFontFactory.hpp>
#include <TGUI/Backend/Renderer/BackendRenderer.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>

#include <TGUI/DefaultFont.hpp>
#include <TGUI/Font.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Timer.hpp>

#if defined(TGUI_SYSTEM_WINDOWS)
    #include <TGUI/extlibs/IncludeWindows.hpp>
#elif defined(TGUI_SYSTEM_EMSCRIPTEN)
    #include <emscripten.h>
#elif defined(TGUI_SYSTEM_MACOS) || defined(TGUI_SYSTEM_LINUX)
    #include <cerrno>
    #include <spawn.h>
    #include <sys/wait.h>
    #if defined(TGUI_SYSTEM_MACOS)
        #include <crt_externs.h>
    #else
extern char** environ;
    #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        std::shared_ptr<Backend> globalBackend = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isBackendSet()
    {
        return globalBackend != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setBackend(std::shared_ptr<Backend> backend)
    {
        TGUI_ASSERT(!backend || !globalBackend, "setBackend() was called with a backend while there already was a backend");

        // Do some cleanup when we destroy the backend
        if (!backend)
        {
            // Stop all timers (as they could contain resources that have to be destroyed before the main function exits)
            Timer::clearTimers();

            // Destroy the global font
            Font::setGlobalFont(nullptr);

            // Destroy the global theme
            Theme::setDefault(nullptr);
        }

        globalBackend = std::move(backend);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Backend> getBackend()
    {
        TGUI_ASSERT(globalBackend != nullptr, "getBackend() was called while there is no backend");
        return globalBackend;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::setDestroyOnLastGuiDetatch(bool destroyOnDetatch)
    {
        m_destroyOnLastGuiDetatch = destroyOnDetatch;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::attachGui(BackendGui* gui)
    {
        m_guis.insert(gui);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::detatchGui(BackendGui* gui)
    {
        // Don't check if it existed, detach is called for every gui while attached is only called for properly initialized guis
        m_guis.erase(gui);

        if (m_destroyOnLastGuiDetatch && m_guis.empty())
            setBackend(nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font Backend::createDefaultFont()
    {
        if (m_fontBackend)
            return {static_cast<const unsigned char*>(defaultFontBytes), sizeof(defaultFontBytes)};
        return {};
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::setFontScale(float scale)
    {
        if (scale == m_fontScale)
            return;

        m_fontScale = scale;

        // Invalidate all font textures
        for (auto* font : m_registeredFonts)
            font->setFontScale(scale);

        // Rasterize SVG images again
        for (auto* sprite : m_registeredSvgSprites)
            sprite->updateVertices();

        // Update the size of all texts in all widgets
        for (const auto& gui : m_guis)
        {
            for (const auto& widget : gui->getWidgets())
                widget->updateTextSize();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Backend::getFontScale() const
    {
        return m_fontScale;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void Backend::openVirtualKeyboard(const FloatRect&)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::closeVirtualKeyboard()
    {
    }
#endif
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Backend::isKeyboardModifierPressed(Event::KeyModifier)
    {
        // All backends must either override this function or the one inside the gui class.
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::setClipboard(const String& contents)
    {
        m_clipboardContents = contents;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Backend::getClipboard() const
    {
        return m_clipboardContents;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Backend::openUrl(const String& url)
    {
        if (url.empty())
            return false;

#if defined(TGUI_SYSTEM_EMSCRIPTEN)
        // Browsers block popups that aren't opened in response to a user action, so this only works while handling events.
        // The "noopener" prevents the opened page from accessing our page via window.opener.
        // window.open returns null when "noopener" is passed, so we can't detect whether the popup was blocked.
        EM_ASM({ window.open(UTF8ToString($0), "_blank", "noopener"); }, url.toStdString().c_str());
        return true;

#elif defined(TGUI_SYSTEM_WINDOWS)
        // We load shell32.dll dynamically, so that the user doesn't need to link to it when linking TGUI statically
        HMODULE dllModuleHandle = LoadLibraryW(L"shell32.dll");
        if (!dllModuleHandle)
            return false;

        using ShellExecuteWFuncType = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
    #if defined(__clang__)
        #pragma clang diagnostic push
        #if defined(__clang_major__) && (__clang_major__ >= 19)
            #pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
        #endif
    #elif defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-function-type"
    #endif
        const auto shellExecuteFunc = reinterpret_cast<ShellExecuteWFuncType>(GetProcAddress(dllModuleHandle, "ShellExecuteW"));
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #elif defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif

        bool success = false;
        if (shellExecuteFunc)
        {
            // ShellExecuteW returns a value greater than 32 on success
            const HINSTANCE result = shellExecuteFunc(nullptr, L"open", url.toWideString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            success = (reinterpret_cast<INT_PTR>(result) > 32);
        }

        FreeLibrary(dllModuleHandle);
        return success;

#elif defined(TGUI_SYSTEM_MACOS) || defined(TGUI_SYSTEM_LINUX)
        // Don't allow the url to be interpreted as a command line option
        if (url[0] == U'-')
            return false;

        const std::string urlStr = url.toStdString();
    #if defined(TGUI_SYSTEM_MACOS)
        // The 'open' command returns immediately after passing the url to the browser
        char** env = *_NSGetEnviron();
        const char* argv[] = {"open", urlStr.c_str(), nullptr};
    #else
        // The xdg-open command might not return until the browser is closed (e.g. when it falls back to $BROWSER).
        // We let the shell run it in the background so that we don't block. The url is passed as a separate argument
        // instead of being part of the command string, so that it can't be used to execute other shell commands.
        char** env = environ;
        const char* argv[] = {"/bin/sh", "-c", "xdg-open \"$1\" >/dev/null 2>&1 &", "sh", urlStr.c_str(), nullptr};
    #endif

        pid_t pid;
        if (posix_spawnp(&pid, argv[0], nullptr, nullptr, const_cast<char* const*>(argv), env) != 0)
            return false;

        int status = 0;
        while (waitpid(pid, &status, 0) < 0)
        {
            if (errno != EINTR)
                return false;
        }

        return WIFEXITED(status) && (WEXITSTATUS(status) == 0);

#else
        return false;
#endif
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<std::uint8_t[]> Backend::readFileFromAndroidAssets(const String&, std::size_t&) const
    {
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendFont> Backend::createFont()
    {
        return getFontBackend()->createFont();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendText> Backend::createText()
    {
        return getRenderer()->createText();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> Backend::createTexture()
    {
        return getRenderer()->createTexture();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Backend::hasRenderer() const
    {
        return m_renderer != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendRenderer> Backend::getRenderer() const
    {
        TGUI_ASSERT(m_renderer != nullptr,
                    "getRenderer() called on backend while no BackendRenderer was created yet. "
                    "Has a window been attached to a gui object yet?");
        return m_renderer;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::setRenderer(std::shared_ptr<BackendRenderer> renderer)
    {
        m_renderer = std::move(renderer);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Backend::hasFontBackend() const
    {
        return m_fontBackend != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendFontFactory> Backend::getFontBackend() const
    {
        TGUI_ASSERT(m_fontBackend != nullptr,
                    "getFontBackend() called on backend while no BackendFont was created yet. "
                    "Has a window been attached to a gui object yet?");
        return m_fontBackend;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::setFontBackend(std::shared_ptr<BackendFontFactory> fontBackend)
    {
        m_fontBackend = std::move(fontBackend);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::registerFont(BackendFont* font)
    {
        m_registeredFonts.insert(font);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::unregisterFont(BackendFont* font)
    {
        m_registeredFonts.erase(font);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::registerSvgSprite(Sprite* sprite)
    {
        m_registeredSvgSprites.insert(sprite);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Backend::unregisterSvgSprite(Sprite* sprite)
    {
        m_registeredSvgSprites.erase(sprite);
    }
} // namespace tgui
