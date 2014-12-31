/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <map>
#include <list>
#include <functional>

#include <TGUI/Global.hpp>
#include <TGUI/SharedWidgetPtr.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Struct that tells more about the callback that happened.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API Callback
    {
        Callback() : widget(nullptr) {};

        // The callback id that was passed to the widget. It is used to identify from what widget the callback came from.
        unsigned int id;

        // How did the callbak occur?
        unsigned int trigger;

        // Pointer to the widget
        Widget* widget;

        // The type of the widget
        WidgetTypes widgetType;

        // When the mouse has something to do with the callback then this data will be filled
        sf::Vector2i mouse;

        // This text is only used by some widgets, but it can be set together with some other member
        sf::String text;

        // Only one of these things can be filled at a given time
        bool         checked;
        int          value;
        sf::Vector2f value2d;
        sf::Vector2f position;
        sf::Vector2f size;
        unsigned int index;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used internally by all widgets to handle callbacks.
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
        /// widget->bindCallback(function, tgui::Widget::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindCallback(std::function<void()> func, unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func      Member function without parameters.
        /// \param classPtr  Pointer to the widget of the class.
        /// \param trigger   In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// class myClass {
        ///     void function() {};
        /// } myObj;
        /// widget->bindCallback(&myClass::function, &myObj, tgui::Widget::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        void bindCallback(void (T::*func)(), T* const classPtr, unsigned int trigger)
        {
            mapCallback(std::bind(func, classPtr), trigger);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func     Free function with a constant reference to a Callback widget as parameter.
        /// \param trigger  In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// void function(const Callback& callback) {}
        /// widget->bindCallbackEx(function, tgui::Widget::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindCallbackEx(std::function<void(const Callback&)> func, unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to one or more specific callback trigger(s).
        ///
        /// \param func      Member function with a constant reference to a Callback widget as parameter.
        /// \param classPtr  Pointer to the widget of the class.
        /// \param trigger   In which situation(s) do you want the callback function to be called?
        ///
        /// Usage example:
        /// \code
        /// class myClass {
        ///     void function(const Callback& callback) {};
        /// } myObj;
        /// widget->bindCallbackEx(&myClass::function, &myObj, tgui::Widget::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        void bindCallbackEx(void (T::*func)(const Callback&), T* const classPtr, unsigned int trigger)
        {
            mapCallback(std::bind(func, classPtr, std::ref(m_Callback)), trigger);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind one or more specific callback trigger(s) to the parent widget.
        ///
        /// \param trigger  In which situation(s) do you want the widget to alert its parent about a callback?
        ///
        /// The widget will tell its parent about the callback and you will receive the callback through the parent widget.
        /// If the callback reaches the window, then you will later have to poll the callbacks from this window.
        ///
        /// Usage example:
        /// \code
        /// widget->bindCallback(tgui::Widget::Focus);
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindCallback(unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Unbind all callback function bound to the given trigger.
        ///
        /// \param trigger  In which situation(s) do you no longer want to retreive callback?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void unbindCallback(unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Unbind all callback functions bound to any callback.
        ///
        /// After calling this function, the widget will no longer send any callback.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void unbindAllCallback();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Map the callback function to the needed trigger(s).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mapCallback(const std::function<void()>& function, unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::map<unsigned int, std::list<std::function<void()>>> m_CallbackFunctions;

        Callback m_Callback;


        template <class T>
        friend class SharedWidgetPtr;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CALLBACK_HPP

