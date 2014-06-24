/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_SHARED_WIDGET_PTR_INL
#define TGUI_SHARED_WIDGET_PTR_INL

#include <TGUI/Container.hpp>
#include <TGUI/Gui.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>::SharedWidgetPtr() :
    m_widgetPtr(nullptr)
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>::SharedWidgetPtr(std::nullptr_t) :
    m_widgetPtr(nullptr),
    m_refCount (nullptr)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>::SharedWidgetPtr(Gui& gui, const sf::String& widgetName) :
    m_widgetPtr(nullptr)
    {
        init();
        gui.m_container->add(*this, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>::SharedWidgetPtr(Container& container, const sf::String& widgetName) :
    m_widgetPtr(nullptr)
    {
        init();
        container.add(*this, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>::SharedWidgetPtr(const SharedWidgetPtr<T>& copy)
    {
        if (copy.get() != nullptr)
        {
            m_widgetPtr = copy.get();

            m_refCount = copy.getRefCount();
            *m_refCount += 1;
        }
        else
        {
            m_widgetPtr = nullptr;
            m_refCount = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    template <class U>
    SharedWidgetPtr<T>::SharedWidgetPtr(const SharedWidgetPtr<U>& copy)
    {
        if (copy.get() != nullptr)
        {
            m_widgetPtr = static_cast<T*>(copy.get());

            m_refCount = copy.getRefCount();
            *m_refCount += 1;
        }
        else
        {
            m_widgetPtr = nullptr;
            m_refCount = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>::~SharedWidgetPtr()
    {
        reset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T>& SharedWidgetPtr<T>::operator=(const SharedWidgetPtr<T>& copy)
    {
        if (this != &copy)
        {
            reset();

            if (copy.get() != nullptr)
            {
                m_widgetPtr = copy.get();

                m_refCount = copy.getRefCount();
                *m_refCount += 1;
            }
            else
            {
                m_widgetPtr = nullptr;
                m_refCount = nullptr;
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    template <class U>
    SharedWidgetPtr<T>& SharedWidgetPtr<T>::operator=(const SharedWidgetPtr<U>& copy)
    {
        reset();

        if (copy.get() != nullptr)
        {
            m_widgetPtr = static_cast<T*>(copy.get());

            m_refCount = copy.getRefCount();
            *m_refCount += 1;
        }
        else
        {
            m_widgetPtr = nullptr;
            m_refCount = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    void SharedWidgetPtr<T>::init()
    {
        reset();

        m_refCount = new unsigned int;
        *m_refCount = 1;

        m_widgetPtr = new T();
        m_widgetPtr->m_callback.widget = get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    void SharedWidgetPtr<T>::reset()
    {
        if (m_widgetPtr != nullptr)
        {
            if (*m_refCount == 1)
            {
                delete m_widgetPtr;
                delete m_refCount;

                m_widgetPtr = nullptr;
                m_refCount = nullptr;
            }
            else
                *m_refCount -= 1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    bool SharedWidgetPtr<T>::operator!() const
    {
        return m_widgetPtr == nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    template <typename U>
    bool SharedWidgetPtr<T>::operator ==(const SharedWidgetPtr<U>& right) const
    {
        return m_widgetPtr == right.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    bool SharedWidgetPtr<T>::operator ==(const SharedWidgetPtr<T>& right) const
    {
        return m_widgetPtr == right.m_widgetPtr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    template <typename U>
    bool SharedWidgetPtr<T>::operator !=(const SharedWidgetPtr<U>& right) const
    {
        return m_widgetPtr != right.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    bool SharedWidgetPtr<T>::operator !=(const SharedWidgetPtr<T>& right) const
    {
        return m_widgetPtr != right.m_widgetPtr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    T& SharedWidgetPtr<T>::operator*() const
    {
        assert(m_widgetPtr != nullptr);
        return *m_widgetPtr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    T* SharedWidgetPtr<T>::operator->() const
    {
        assert(m_widgetPtr != nullptr);
        return m_widgetPtr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    T* SharedWidgetPtr<T>::get() const
    {
        return m_widgetPtr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    unsigned int* SharedWidgetPtr<T>::getRefCount() const
    {
        return m_refCount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    SharedWidgetPtr<T> SharedWidgetPtr<T>::clone() const
    {
        if (m_widgetPtr != nullptr)
        {
            SharedWidgetPtr<T> pointer = nullptr;

            pointer.m_refCount = new unsigned int;
            *pointer.m_refCount = 1;

            pointer.m_widgetPtr = m_widgetPtr->clone();
            pointer.m_widgetPtr->m_callback.widget = pointer.get();
            return pointer;
        }
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SHARED_WIDGET_PTR_INL
