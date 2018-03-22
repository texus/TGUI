/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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

// Code based on http://codereview.stackexchange.com/questions/20058/c11-any-class

#ifndef TGUI_ANY_HPP
#define TGUI_ANY_HPP

#include <type_traits>
#include <utility>
#include <typeinfo>

namespace tgui
{
    struct Any
    {
        template<class T>
        using StorageType = typename std::decay<T>::type;

        bool is_null() const
        {
            return ptr == nullptr;
        }

        bool not_null() const
        {
            return ptr != nullptr;
        }

        template<typename U>
        Any(U&& value)
            : ptr{new Derived<StorageType<U>>(std::forward<U>(value))}
        {
        }

        template<class U>
        bool is() const
        {
            typedef StorageType<U> T;
            return dynamic_cast<Derived<T>*>(ptr);
        }

        template<class U>
        StorageType<U>& as() const
        {
            typedef StorageType<U> T;
            auto derived = dynamic_cast<Derived<T>*>(ptr);
            if (!derived)
                throw std::bad_cast();

            return derived->value;
        }

        template<class U>
        operator U()
        {
            return as<StorageType<U>>();
        }

        Any()
            : ptr(nullptr)
        {
        }

        Any(const Any& that)
            : ptr(that.clone())
        {
        }

        Any(Any&& that)
            : ptr(that.ptr)
        {
            that.ptr = nullptr;
        }

        Any& operator=(const Any& a)
        {
            if (ptr == a.ptr)
                return *this;

            auto old_ptr = ptr;

            ptr = a.clone();

            if (old_ptr)
                delete old_ptr;

            return *this;
        }

        Any& operator=(Any&& a)
        {
            if (ptr == a.ptr)
                return *this;

            std::swap(ptr, a.ptr);

            return *this;
        }

        ~Any()
        {
            delete ptr;
        }

    private:
        struct Base
        {
            virtual ~Base() {}
            virtual Base* clone() const = 0;
        };

        template<typename T>
        struct Derived : Base
        {
            template<typename U>
            Derived(U&& val) :
                value(std::forward<U>(val))
            {
            }

            Base* clone() const override
            {
                return new Derived<T>(value);
            }

            T value;
        };

        Base* clone() const
        {
            if (ptr)
                return ptr->clone();
            else
                return nullptr;
        }

        Base* ptr;
    };
}

#endif // TGUI_ANY_HPP
