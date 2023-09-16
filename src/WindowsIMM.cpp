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

#include <TGUI/Config.hpp>

#ifdef TGUI_SYSTEM_WINDOWS
#include <TGUI/WindowsIMM.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    WindowsIMM::ImmGetContextFunc WindowsIMM::m_dllImmGetContext = nullptr;
    WindowsIMM::ImmSetCompositionWindowFunc WindowsIMM::m_dllImmSetCompositionWindow = nullptr;
    WindowsIMM::ImmReleaseContextFunc WindowsIMM::m_dllImmReleaseContext = nullptr;

    HMODULE WindowsIMM::m_dllImmModuleHandle = nullptr;

    unsigned int WindowsIMM::m_referenceCount = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WindowsIMM::initialize()
    {
        ++m_referenceCount;
        if (m_referenceCount != 1)
            return;

        m_dllImmModuleHandle = LoadLibraryW(L"imm32.dll");
        if (m_dllImmModuleHandle)
        {
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
            m_dllImmGetContext = reinterpret_cast<ImmGetContextFunc>(GetProcAddress(m_dllImmModuleHandle, "ImmGetContext"));
            m_dllImmSetCompositionWindow = reinterpret_cast<ImmSetCompositionWindowFunc>(GetProcAddress(m_dllImmModuleHandle, "ImmSetCompositionWindow"));
            m_dllImmReleaseContext = reinterpret_cast<ImmReleaseContextFunc>(GetProcAddress(m_dllImmModuleHandle, "ImmReleaseContext"));
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WindowsIMM::release()
    {
        TGUI_ASSERT(m_referenceCount > 0, "WindowsIMM::release can't be called without a corresponding WindowsIMM::initialize() call!");
        --m_referenceCount;
        if (m_referenceCount > 0)
            return;

        if (m_dllImmModuleHandle)
        {
            FreeLibrary(m_dllImmModuleHandle);
            m_dllImmModuleHandle = nullptr;
            m_dllImmGetContext = nullptr;
            m_dllImmSetCompositionWindow = nullptr;
            m_dllImmReleaseContext = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WindowsIMM::setCandidateWindowPosition(HWND hWnd, Vector2f caretPos)
    {
        if (!m_dllImmGetContext || !m_dllImmSetCompositionWindow || !m_dllImmReleaseContext)
            return;

        if (HIMC hIMC = m_dllImmGetContext(hWnd))
        {
            COMPOSITIONFORM compositionForm;
            compositionForm.dwStyle = CFS_FORCE_POSITION;
            compositionForm.ptCurrentPos.x = static_cast<int>(std::round(caretPos.x));
            compositionForm.ptCurrentPos.y = static_cast<int>(std::round(caretPos.y));
            compositionForm.rcArea = {};
            m_dllImmSetCompositionWindow(hIMC, &compositionForm);

            m_dllImmReleaseContext(hWnd, hIMC);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
