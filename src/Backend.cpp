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


#include <TGUI/Backend.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Timer.hpp>
#include <TGUI/Font.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        std::shared_ptr<BackendBase> globalBackend = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isBackendSet()
    {
        return (globalBackend != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setBackend(std::shared_ptr<BackendBase> backend)
    {
        TGUI_ASSERT(!backend || !globalBackend, "setBackend() was called with a backend while there already was a backend");

        globalBackend = backend;

        if (backend)
        {
            Font::setGlobalFont(backend->createDefaultFont());
        }
        else // We are destroying the backend
        {
            // Stop all timers (as they could contain resources that have to be destroyed before the main function exits)
            Timer::clearTimers();

            // Destroy the global font
            Font::setGlobalFont(nullptr);

            // Destroy the global theme
            Theme::setDefault(nullptr);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendBase> getBackend()
    {
        TGUI_ASSERT(globalBackend != nullptr, "getBackend() was called while there is no backend");
        return globalBackend;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendBase::setDestroyOnLastGuiDetatch(bool destroyOnDetatch)
    {
        m_destroyOnLastGuiDetatch = destroyOnDetatch;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendBase::setClipboard(const String& contents)
    {
        m_clipboardContents = contents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String BackendBase::getClipboard() const
    {
        return m_clipboardContents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<std::uint8_t[]> BackendBase::readFileFromAndroidAssets(const String&, std::size_t&) const
    {
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
