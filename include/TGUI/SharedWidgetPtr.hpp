/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_SHARED_WIDGET_PTR_HPP
#define TGUI_SHARED_WIDGET_PTR_HPP

#include <cassert>

#include <TGUI/Container.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class ContainerWidget;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class SharedWidgetPtr
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedWidgetPtr() :
        m_WidgetPtr(nullptr)
        {
            m_RefCount = new unsigned int;
            *m_RefCount = 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedWidgetPtr(Container& container, const sf::String& widgetName = "")
        {
            m_RefCount = new unsigned int;
            *m_RefCount = 1;

            m_WidgetPtr = new T();
            container.add(*this, widgetName);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedWidgetPtr(const SharedWidgetPtr<T>& copy)
        {
            m_WidgetPtr = copy.get();

            m_RefCount = copy.getRefCount();
            *m_RefCount += 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <class U>
        SharedWidgetPtr(const SharedWidgetPtr<U>& copy)
        {
            m_WidgetPtr = static_cast<T*>(copy.get());

            m_RefCount = copy.getRefCount();
            *m_RefCount += 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ~SharedWidgetPtr()
        {
            if (*m_RefCount == 1)
            {
                delete m_WidgetPtr;
                delete m_RefCount;
            }
            else
                *m_RefCount -= 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedWidgetPtr<T>& operator=(const SharedWidgetPtr<T>& copy)
        {
            if (this != &copy)
            {
                if (*m_RefCount == 1)
                {
                    delete m_WidgetPtr;
                    delete m_RefCount;
                }
                else
                    *m_RefCount -= 1;

                m_WidgetPtr = copy.get();

                m_RefCount = copy.getRefCount();
                *m_RefCount += 1;
            }

            return *this;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <class U>
        SharedWidgetPtr<T>& operator=(const SharedWidgetPtr<U>& copy)
        {
            if (*m_RefCount == 1)
            {
                delete m_WidgetPtr;
                delete m_RefCount;
            }
            else
                *m_RefCount -= 1;

            m_WidgetPtr = static_cast<T*>(copy.get());

            m_RefCount = copy.getRefCount();
            *m_RefCount += 1;

            return *this;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void init(Container& container, const sf::String& widgetName = "")
        {
            if (m_WidgetPtr != nullptr)
            {
                if (*m_RefCount == 1)
                {
                    delete m_WidgetPtr;
                    delete m_RefCount;
                }
                else
                    *m_RefCount -= 1;
            }

            m_WidgetPtr = new T();
            container.add(*this, widgetName);

            m_RefCount = new unsigned int;
            *m_RefCount = 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool operator!() const
        {
            return m_WidgetPtr == nullptr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool operator ==(const SharedWidgetPtr<U>& right) const
        {
            return m_WidgetPtr == right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool operator ==(const SharedWidgetPtr<T>& right) const
        {
            return m_WidgetPtr == right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator ==(const SharedWidgetPtr<T>& left, const U* right)
        {
            return left.m_WidgetPtr == right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator ==(const SharedWidgetPtr<T>& left, const T* right)
        {
            return left.m_WidgetPtr == right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator ==(const U* left, const SharedWidgetPtr<T>& right)
        {
            return left == right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator ==(const T* left, const SharedWidgetPtr<T>& right)
        {
            return left == right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool operator !=(const SharedWidgetPtr<U>& right) const
        {
            return m_WidgetPtr != right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool operator !=(const SharedWidgetPtr<T>& right) const
        {
            return m_WidgetPtr != right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator !=(const SharedWidgetPtr<T>& left, const U* right)
        {
            return left.m_WidgetPtr != right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator !=(const SharedWidgetPtr<T>& left, const T* right)
        {
            return left.m_WidgetPtr != right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator !=(const U* left, const SharedWidgetPtr<T>& right)
        {
            return left != right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator !=(const T* left, const SharedWidgetPtr<T>& right)
        {
            return left != right.m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        T& operator*() const
        {
            assert(m_WidgetPtr != nullptr);
            return *m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        T* operator->() const
        {
            assert(m_WidgetPtr != nullptr);
            return m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        T* get() const
        {
            return m_WidgetPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        unsigned int* getRefCount() const
        {
            return m_RefCount;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedWidgetPtr clone() const
        {
            SharedWidgetPtr<T> pointer;
            pointer.m_WidgetPtr = m_WidgetPtr->clone();
            return pointer;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      private:

        T* m_WidgetPtr;
        unsigned int* m_RefCount;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SHARED_WIDGET_PTR_HPP
