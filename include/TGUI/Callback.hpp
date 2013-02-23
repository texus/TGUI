/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_CALLBACK_HPP
#define TGUI_CALLBACK_HPP

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <map>
#include <list>

#include <TGUI/Defines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Object;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Struct that tells more about the callback that happened.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API Callback
    {
        // The id that was passed to the object. It is used to identify from what object the callback came from.
        unsigned int callbackId;

        // How did the callbak occur?
        unsigned int trigger;

        // Pointer to the object
        Object* object;

        // The type of the object
        ObjectTypes objectType;

        // This text is only used by some objects, but it is not allowed to be in the union
        sf::String text;

        // When the mouse has something to do with the callback then this data will be filled
        struct Value2i
        {
            int x;
            int y;

            Value2i& operator=(Vector2i vector) { x = vector.x; y = vector.y; return *this; }
            operator Vector2i() const { return Vector2i(x, y); }
        } mouse;

        struct Value2d
        {
            float x;
            float y;

            Value2d& operator=(Vector2f vector) { x = vector.x; y = vector.y; return *this; }
            operator Vector2f() const { return Vector2f(x, y); }
        };

        // Only one of these things can be filled at a given time
        union
        {
            bool    checked;
            int     value;
            Value2d value2d;
            Value2d position;
            Value2d size;
            unsigned int index;
        };
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used internally to store the callback functions.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API CallbackFunction
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallbackFunction();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Contructor that initializes the simple function.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallbackFunction(boost::function<void()> function);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Contructor that initializes the extended function.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallbackFunction(boost::function<void(const Callback&)> function);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        boost::function<void()> simpleFunction;
        boost::function<void(const Callback&)> extendedFunction;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used internally by all objects to handle callbacks.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API CallbackManager
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func     Free function without parameters.
        ///                 This can actually also take more advanced stuff like a functor or lambda function.
        /// \param trigger  In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// void function() {}
        /// object->bindCallback(function, tgui::Object::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void bindCallback(boost::function<void()> func, unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func      Member function without parameters.
        /// \param classPtr  Pointer to the object of the class.
        /// \param trigger   In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// class myClass {
        ///     void function() {};
        /// } myObj;
        /// object->bindCallback(&myClass::function, &myObj, tgui::Object::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        void bindCallback(void (T::*func)(), T* const classPtr, unsigned int trigger)
        {
            mapCallback(CallbackFunction(boost::function<void()>(boost::bind(func, classPtr))), trigger);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func     Free function with a constant reference to a Callback object as parameter.
        /// \param trigger  In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// void function(const Callback& callback) {}
        /// object->bindCallbackEx(function, tgui::Object::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void bindCallbackEx(boost::function<void(const Callback&)> func, unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func      Member function with a constant reference to a Callback object as parameter.
        /// \param classPtr  Pointer to the object of the class.
        /// \param trigger   In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// class myClass {
        ///     void function(const Callback& callback) {};
        /// } myObj;
        /// object->bindCallbackEx(&myClass::function, &myObj, tgui::Object::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        void bindCallbackEx(void (T::*func)(const tgui::Callback&), T* const classPtr, unsigned int trigger)
        {
            mapCallback(CallbackFunction(boost::function<void(const Callback&)>(boost::bind(func, classPtr, _1))), trigger);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind one or more specific callback trigger(s) to the parent object.
        ///
        /// \param trigger  In which situation(s) do you want the object to alert its parent about a callback?
        ///
        /// The object will tell its parent about the callback and you will receive the callback through the parent object.
        /// If the callback reaches the window, then you will later have to poll the callbacks from this window.
        ///
        /// Usage example:
        /// \code
        /// object->bindCallback(tgui::Object::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void bindCallback(unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Unbind all callback function bound to the given trigger.
        ///
        /// \param trigger  In which situation(s) do you no longer want to retreive callback?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unbindCallback(unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Unbind all callback functions bound to any callback.
        ///
        /// After calling this function, the object will no longer send any callback.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unbindAllCallback();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Map the callback function to the needed trigger(s).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mapCallback(const CallbackFunction& function, unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::map< unsigned int, std::list<CallbackFunction> > m_CallbackFunctions;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CALLBACK_HPP

