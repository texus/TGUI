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


#ifndef TGUI_SHARED_OBJECTS_PTR_HPP
#define TGUI_SHARED_OBJECTS_PTR_HPP

#include <cassert>

#include <TGUI/Group.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class GroupObject;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ReferenceCount
    {
        unsigned int count;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class TGUI_API SharedObjectPtr
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedObjectPtr() :
        m_ObjectPtr(NULL)
        {
            m_RefCount = new ReferenceCount;
            m_RefCount->count = 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedObjectPtr(Group& group, const sf::String& objectName = "")
        {
            m_RefCount = new ReferenceCount;
            m_RefCount->count = 1;

            m_ObjectPtr = new T();
            group.add(*this, objectName);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedObjectPtr(const SharedObjectPtr<T>& copy)
        {
            m_ObjectPtr = copy.get();

            m_RefCount = copy.getRefCount();
            m_RefCount->count += 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <class U>
        SharedObjectPtr(const SharedObjectPtr<U>& copy)
        {
            m_ObjectPtr = static_cast<T*>(copy.get());

            m_RefCount = copy.getRefCount();
            m_RefCount->count += 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ~SharedObjectPtr()
        {
            if (m_RefCount->count == 1)
            {
                delete m_ObjectPtr;
                delete m_RefCount;
            }
            else
                m_RefCount->count -= 1;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedObjectPtr<T>& operator=(const SharedObjectPtr<T>& copy)
        {
            if (this != &copy)
            {
                if (m_RefCount->count == 1)
                {
                    delete m_ObjectPtr;
                    delete m_RefCount;
                }
                else
                    m_RefCount->count -= 1;

                m_ObjectPtr = copy.get();

                m_RefCount = copy.getRefCount();
                m_RefCount->count += 1;
            }

            return *this;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <class U>
        SharedObjectPtr<T>& operator=(const SharedObjectPtr<U>& copy)
        {
            if (m_RefCount->count == 1)
            {
                delete m_ObjectPtr;
                delete m_RefCount;
            }
            else
                m_RefCount->count -= 1;

            m_ObjectPtr = static_cast<T*>(copy.get());

            m_RefCount = copy.getRefCount();
            m_RefCount->count += 1;

            return *this;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void init(Group& group, const sf::String& objectName = "")
        {
            if (m_ObjectPtr == NULL)
            {
                m_ObjectPtr = new T();
                group.add(*this, objectName);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool operator!() const
        {
            return m_ObjectPtr == NULL;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool operator ==(const SharedObjectPtr<U>& right) const
        {
            return m_ObjectPtr == right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool operator ==(const SharedObjectPtr<T>& right) const
        {
            return m_ObjectPtr == right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator ==(const SharedObjectPtr<T>& left, const U* right)
        {
            return left.m_ObjectPtr == right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator ==(const SharedObjectPtr<T>& left, const T* right)
        {
            return left.m_ObjectPtr == right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator ==(const U* left, const SharedObjectPtr<T>& right)
        {
            return left == right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator ==(const T* left, const SharedObjectPtr<T>& right)
        {
            return left == right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool operator !=(const SharedObjectPtr<U>& right) const
        {
            return m_ObjectPtr != right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool operator !=(const SharedObjectPtr<T>& right) const
        {
            return m_ObjectPtr != right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator !=(const SharedObjectPtr<T>& left, const U* right)
        {
            return left.m_ObjectPtr != right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator !=(const SharedObjectPtr<T>& left, const T* right)
        {
            return left.m_ObjectPtr != right;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename U>
        bool friend operator !=(const U* left, const SharedObjectPtr<T>& right)
        {
            return left != right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool friend operator !=(const T* left, const SharedObjectPtr<T>& right)
        {
            return left != right.m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        T& operator*() const
        {
            assert(m_ObjectPtr != NULL);
            return *m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        T* operator->() const
        {
            assert(m_ObjectPtr != NULL);
            return m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        T* get() const
        {
            return m_ObjectPtr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ReferenceCount* getRefCount() const
        {
            return m_RefCount;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SharedObjectPtr clone() const
        {
            SharedObjectPtr<T> pointer;
            pointer.m_ObjectPtr = m_ObjectPtr->clone();
            return pointer;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      private:

        T* m_ObjectPtr;
        ReferenceCount* m_RefCount;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SHARED_OBJECTS_PTR_HPP
