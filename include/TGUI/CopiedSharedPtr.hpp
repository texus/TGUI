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


#ifndef TGUI_COPIED_SHARED_PTR_HPP
#define TGUI_COPIED_SHARED_PTR_HPP


#include <TGUI/Config.hpp>


#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <memory>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename WidgetType>
    class CopiedSharedPtr
    {
    public:

        template <typename... Args>
        CopiedSharedPtr(Args&&... args) noexcept
            : m_WidgetPtr{std::make_shared<WidgetType>(std::forward<Args>(args)...)}
        {
        }

        CopiedSharedPtr(const CopiedSharedPtr& other) noexcept
            : m_WidgetPtr{std::make_shared<WidgetType>(*other.m_WidgetPtr)}
        {
        }

        CopiedSharedPtr(CopiedSharedPtr&& other) noexcept
            : m_WidgetPtr{std::move(other.m_WidgetPtr)}
        {
        }

        CopiedSharedPtr& operator=(const CopiedSharedPtr& other) noexcept
        {
            if (&other != this)
                m_WidgetPtr = std::make_shared<WidgetType>(*other.m_WidgetPtr);

            return *this;
        }

        CopiedSharedPtr& operator=(CopiedSharedPtr&& other) noexcept
        {
            if (&other != this)
                m_WidgetPtr = std::move(other.m_WidgetPtr);

            return *this;
        }

        operator bool() const noexcept
        {
            return (m_WidgetPtr != nullptr);
        }

        WidgetType& operator*() const noexcept
        {
            return *m_WidgetPtr;
        }

        WidgetType* operator->() const noexcept
        {
            return m_WidgetPtr.get();
        }

        TGUI_NODISCARD WidgetType* get() const noexcept
        {
            return m_WidgetPtr.get();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::shared_ptr<WidgetType> m_WidgetPtr;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_COPIED_SHARED_PTR_HPP
