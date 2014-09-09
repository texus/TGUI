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


#ifndef TGUI_CALLBACK_HPP
#define TGUI_CALLBACK_HPP

#include <map>
#include <list>
#include <functional>

#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Struct that tells more about the callback that happened.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API Callback
    {
        // The callback id that was passed to the widget. It is used to identify from what widget the callback came from.
        unsigned int id = 0;

        // How did the callback occur?
        unsigned int trigger = 0;

        // Pointer to the widget
        Widget* widget = nullptr;

        // The type of the widget
        WidgetType widgetType = WidgetType::Unknown;

        // When the mouse has something to do with the callback then this data will be filled
        sf::Vector2i mouse;

        // This text is only used by some widgets, but it can be set together with some other member
        sf::String text;

        // Only one of these things can be filled at a given time
        bool         checked = false;
        int          value = 0;
        unsigned int index = 0;
        sf::Vector2f value2d;
        sf::Vector2f position;
        sf::Vector2f size;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Used internally by all widgets to handle callbacks.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API CallbackManager
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Bind a function to one or more specific callback trigger(s).
        ///
        /// @param trigger  In which situation(s) do you want the callback function to be called?
        /// @param func     Free function without parameters.
        ///                 This can actually also take more advanced stuff like a functor or lambda function.
        ///
        /// Usage examples:
        /// @code
        /// void function() { std::cout << "Focused" << std::endl; }
        /// widget->bindCallback(tgui::Widget::Focus, function);
        ///
        /// widget->bindCallback(tgui::Widget::Focus, [](){ std::cout << "Focused" << std::endl; });
        ///
        /// void function2(tgui::Button::Ptr button) { std::cout << button->getSize().x << std::endl; }
        /// button->bindCallback(tgui::Button::LeftMouseClicked, std::bind(function2, button));
        ///
        /// class MyClass {
        ///     void function() { std::cout << "Focused" << std::endl; };
        /// };
        /// MyClass myObj;
        /// widget->bindCallback(tgui::Widget::Focus, std::bind(&MyClass::function, &myObj));
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindCallback(unsigned int trigger, std::function<void()> func);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Bind a function to one or more specific callback trigger(s).
        ///
        /// @param trigger  In which situation(s) do you want the callback function to be called?
        /// @param func     Free function with a constant reference to a Callback widget as parameter.
        ///
        /// Usage examples:
        /// @code
        /// void function(const tgui::Callback& callback) { std::cout << callback.checked << std::endl; }
        /// checkbox->bindCallbackEx(tgui::Checkbox::Checked | tgui::Checkbox::Unchecked, function);
        ///
        /// editbox->bindCallbackEx(tgui::EditBox::TextChanged, [](const tgui::Callback& c){ std::cout << c.text << std::endl; });
        ///
        /// class MyClass {
        ///     void function(const tgui::Callback& callback) { std::cout << callback.value << std::endl; };
        /// };
        /// MyClass myObj;
        /// slider->bindCallbackEx(tgui::Slider::ValueChanged, std::bind(&MyClass::function, &myObj, std::placeholders::_1));
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindCallbackEx(unsigned int trigger, std::function<void(const Callback&)> func);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Bind one or more specific callback trigger(s) to the parent widget.
        ///
        /// @param trigger  In which situation(s) do you want the widget to alert its parent about a callback?
        ///
        /// The widget will tell its parent about the callback and you will receive the callback through the parent widget.
        /// If the callback reaches the window, then you will later have to poll the callbacks from this window.
        ///
        /// Usage example:
        /// @code
        /// widget->bindCallback(tgui::Widget::Focus);
        /// widget->setCallbackId(5);
        ///
        /// tgui::Callback callback;
        /// while (gui.pollCallback(callback))
        /// {
        ///      if (callback.id == 5)
        ///          std::cout << "Widget focused" << std::endl;
        /// }
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindCallback(unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Unbind all callback function bound to the given trigger.
        ///
        /// @param trigger  In which situation(s) do you no longer want to retreive callback?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void unbindCallback(unsigned int trigger);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Unbind all callback functions bound to any callback.
        ///
        /// After calling this function, the widget will no longer send any callback.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void unbindAllCallback();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the callback id of the widget.
        ///
        /// @param callbackId  The new callback id
        ///
        /// When receiving callback in a function which takes a Callback as parameter (or when polling the callbacks),
        /// you can find a callbackId in this Callback struct which can identify the widget.
        /// This function changes the id that this widget sends on a callback.
        ///
        /// By default, the callback id will be 0.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setCallbackId(unsigned int callbackId)
        {
            m_callback.id = callbackId;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the callback id of the widget.
        ///
        /// @return Callback id
        ///
        /// @see setCallbackId
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getCallbackId()
        {
            return m_callback.id;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Map the callback function to the needed trigger(s).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mapCallback(unsigned int trigger, const std::function<void()>& function);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::map<unsigned int, std::list<std::function<void()>>> m_callbackFunctions;

        Callback m_callback;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CALLBACK_HPP

