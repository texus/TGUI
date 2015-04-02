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


#ifndef TGUI_WINDOW_HPP
#define TGUI_WINDOW_HPP


#include <queue>

#include <TGUI/Container.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Gui : public sf::NonCopyable
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /// If you use this constructor then you will still have to call the setWindow yourself.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Gui();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Construct the gui and set the window on which the gui should be drawn.
        ///
        /// \param window  The sfml window that will be used by the gui.
        ///
        /// If you use this constructor then you will no longer have to call setWindow yourself.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Gui(sf::RenderWindow& window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Construct the gui and set the target on which the gui should be drawn.
        ///
        /// \param window  The render target that will be used by the gui.
        ///
        /// If you use this constructor then you will no longer have to call setWindow yourself.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Gui(sf::RenderTarget& window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the window on which the gui should be drawn.
        ///
        /// \param window  The sfml window that will be used by the gui.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setWindow(sf::RenderWindow& window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the target on which the gui should be drawn.
        ///
        /// \param window  The render target that will be used by the gui.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setWindow(sf::RenderTarget& window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the window on which the gui is being drawn.
        ///
        /// \return The sfml that is used by the gui.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::RenderTarget* getWindow() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Passes the event to the widgets.
        ///
        /// \param event  The event that was polled from the gui
        /// \param resetView  Reset the view of the window while calculation the mouse coordinates.
        ///                   This parameter must be the same as the one passed to the draw function.
        ///                   If false, the current view will be used, otherwise the view will be reset.
        ///                   If false then make sure the same view is set when calling this function and when calling draw.
        ///
        /// \return Has the event been consumed?
        ///         When this function returns false, then the event was ignored by all widgets.
        ///
        /// You should call this function in your event loop.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool handleEvent(sf::Event event, bool resetView = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Draws all the widgets that were added to the gui.
        ///
        /// \param resetView  Reset the view of the window while drawing the gui.
        ///                   This parameter must be the same as the one passed to the handleEvent function.
        ///                   If false, the current view will be used, otherwise the view will be reset.
        ///                   If false then make sure the same view is set when calling this function and when calling handleEvent.
        ///
        /// When this function ends, the view will never be changed. Any changes to the view are temporary.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(bool resetView = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the next callback from the callback queue.
        ///
        /// This function works just like the pollEvent function from sfml.
        ///
        /// The gui stores the callback of all the widgets.
        /// This function will return the next callback and then remove it from the queue.
        ///
        /// Note that more than one callbacks may be present in the queue, thus you should always call this
        /// function in a loop to make sure that you process every callback.
        ///
        /// \param callback  An empty tgui::Callback widget that will be (partly) filled when there is a callback.
        ///
        /// \return
        ///        - true when there is another callback. The \a callback parameter will be filled with information.
        ///        - false when there is no callback. The \a callback parameter remains uninitialized and may not be used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool pollCallback(Callback& callback);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Check if the window is focused.
        ///
        /// When the window is unfocused, animations (e.g. flashing caret of an edit box) will be paused.
        ///
        /// \return Is the window currently focused?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool hasFocus() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the size of the container.
        ///
        /// \return Size of the container.
        ///
        /// This size will equal the size of the window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::Vector2f getSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the internal container of the Gui.
        ///
        /// This could be useful when having a function that should accept both the gui and e.g. a child window as parameter.
        ///
        /// \warning Not all functions in the Container class make sense for the Gui (which is the reason that the Gui does not
        ///          inherit from Container). So calling some functions (e.g. setSize) will have no effect.
        ///
        /// \return Reference to the internal Container class
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Container& getContainer();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the global font.
        ///
        /// This font will be used by all widgets that are created after calling this function.
        ///
        /// \param filename  Path of the font file to load
        ///
        /// \return True if loading succeeded, false if it failed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setGlobalFont(const std::string& filename);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the global font.
        ///
        /// This font will be used by all widgets that are created after calling this function.
        ///
        /// \param font  Font to copy
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setGlobalFont(const sf::Font& font);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the global font.
        ///
        /// This is the font that is used for newly created widget by default.
        ///
        /// \return global font
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Font& getGlobalFont() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns a list of all the widgets.
        ///
        /// \return Vector of all widget pointers
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::vector< Widget::Ptr >& getWidgets();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns a list of the names of all the widgets.
        ///
        /// \return Vector of all widget names
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::vector<sf::String>& getWidgetNames();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Adds a widget to the container.
        ///
        /// \param widgetPtr   Pointer to the widget you would like to add
        /// \param widgetName  If you want to access the widget later then you must do this with this name
        ///
        /// Usage example:
        /// \code
        /// tgui::Picture::Ptr pic(container); // Create a picture and add it to the container
        /// container.remove(pic);             // Remove the picture from the container
        /// container.add(pic);                // Add the picture to the container again
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void add(const Widget::Ptr& widgetPtr, const sf::String& widgetName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns a pointer to an earlier created widget.
        ///
        /// \param widgetName The name that was given to the widget when it was added to the container.
        /// \param recursive  Should the function also search for widgets inside containers that are inside this container?
        ///
        /// \return Pointer to the earlier created widget
        ///
        /// \warning This function will return nullptr when an unknown widget name was passed.
        ///
        /// Usage example:
        /// \code
        /// tgui::Picture::Ptr pic(container, "picName");
        /// tgui::Picture::Ptr pic2 = container.get("picName");
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr get(const sf::String& widgetName, bool recursive = false) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns a pointer to an earlier created widget.
        ///
        /// \param widgetName The name that was given to the widget when it was added to the container.
        /// \param recursive  Should the function also search for widgets inside containers that are inside this container?
        ///
        /// \return Pointer to the earlier created widget.
        ///         The pointer will already be casted to the desired type.
        ///
        /// \warning This function will return nullptr when an unknown widget name was passed.
        ///
        /// Usage example:
        /// \code
        /// tgui::Picture::Ptr pic(container, "picName");
        /// tgui::Picture::Ptr pic2 = container.get<tgui::Picture>("picName");
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        typename T::Ptr get(const sf::String& widgetName, bool recursive = false) const
        {
            return m_Container.get<T>(widgetName, recursive);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Makes a copy of any existing widget and returns the pointer to the new widget.
        ///
        /// \param oldWidget     A pointer to the old widget.
        /// \param newWidgetName If you want to access the widget later then you must do this with this name
        ///
        /// \return Pointer to the new widget
        ///
        /// Usage example:
        /// \code
        /// tgui::Picture::Ptr pic(container, "picName");
        /// tgui::Picture::Ptr pic2 = container.copy(pic, "picName_2");
        /// tgui::Picture::Ptr pic3 = container.copy(container.get("picName"), "picName_3");
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr copy(const Widget::Ptr& oldWidget, const sf::String& newWidgetName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes a single widget that was added to the container.
        ///
        /// \param widget  Pointer to the widget to remove
        ///
        /// Usage example:
        /// \code
        /// tgui::Picture::Ptr pic(container, "picName");
        /// tgui::Picture::Ptr pic2(container, "picName2");
        /// container.remove(pic);
        /// container.remove(container.get("picName2"));
        /// \endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void remove(const Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes all widgets that were added to the container.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllWidgets();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the name of a widget.
        ///
        /// \param widget  Widget of which the name should be changed
        /// \param name    New name for the widget
        ///
        /// \return True when the name was changed, false when the widget wasn't part of this container.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setWidgetName(const Widget::Ptr& widget, const std::string& name);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the name of a widget.
        ///
        /// \param widget  Widget of which the name should be retrieved
        /// \param name    Name for the widget
        ///
        /// \return False is returned when the widget wasn't part of this container.
        ///         In this case the name parameter is left unchanged.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getWidgetName(const Widget::Ptr& widget, std::string& name) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Focuses a widget.
        ///
        /// The previously focused widget will be unfocused.
        ///
        /// \param widget  The widget that has to be focused.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void focusWidget(Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Focuses the next widget.
        ///
        /// The currently focused widget will be unfocused, even if it was the only widget.
        /// When no widget was focused, the first widget in the container will be focused.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void focusNextWidget();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Focuses the previous widget.
        ///
        /// The currently focused widget will be unfocused, even if it was the only widget.
        /// When no widget was focused, the last widget in the container will be focused.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void focusPreviousWidget();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Unfocus all the widgets.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void unfocusWidgets();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Uncheck all the radio buttons.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void uncheckRadioButtons();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Places a widget before all other widgets.
        ///
        /// \param widget  The widget that should be moved to the front
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void moveWidgetToFront(Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Places a widget behind all other widgets.
        ///
        /// \param widget  The widget that should be moved to the back
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void moveWidgetToBack(Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to the callbacks of all child widgets.
        ///
        /// When a child widget tells this widget about the callback then the global callback function(s) will be called.
        /// If no global callback function has been bound then the callback is passed to the parent of this widget.
        ///
        /// \param func  Pointer to a free function with a reference to a Callback widget as parameter.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void bindGlobalCallback(std::function<void(const Callback&)> func);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Bind a function to the callbacks of all child widgets.
        ///
        /// When a child widget tells this widget about the callback then the global callback function(s) will be called.
        /// If no global callback function has been bound then the callback is passed to the parent of this widget.
        ///
        /// \param func      Pointer to a member function with a reference to a Callback widget as parameter.
        /// \param classPtr  Pointer to the widget of the class.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        void bindGlobalCallback(void (T::*func)(const Callback&), const T* const classPtr)
        {
            m_Container.bindGlobalCallback(func, classPtr);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Unbind the global callback function(s).
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void unbindGlobalCallback();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Loads a form from a file.
        ///
        /// The widgets will be loaded and added to the container.
        /// Note that even when this function fails, some widgets might have been loaded already.
        ///
        /// \param filename  Filename of the widget file that is to be loaded
        ///
        /// \return
        ///        - true on success
        ///        - false when the file could not be opened
        ///        - false when the file contains a mistake
        ///        - false when one of the widgets couldn't be loaded
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool loadWidgetsFromFile(const std::string& filename);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Save the widgets to a file.
        ///
        /// \param filename  Filename of the widget file that is to be created
        ///
        /// \return
        ///        - true on success
        ///        - false when the file couldn't be created
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool saveWidgetsToFile(const std::string& filename);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update the internal clock to make animation possible. This function is called automatically by the draw function.
        // You will thus only need to call it yourself when you are drawing everything manually.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateTime(const sf::Time& elapsedTime);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // TGUI uses this function internally to handle widget callbacks.
        // When you tell a widget to send its callbacks to its parent then this function is called.
        //
        // When one or more global callback functions were set then these functions will be called.
        // Otherwise, the callback will be added to the callback queue and you will be able to poll it later with pollCallback.
        //
        // You can use this function to fake a widget callback.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void addChildCallback(const Callback& callback);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        // This will store all widget callbacks until you pop them with getCallback
        std::queue<Callback> m_Callback;

        // The internal clock which is used for animation of widgets
        sf::Clock m_Clock;

        // The sfml window or other target to draw on
        sf::RenderTarget* m_Window;

        // Does m_Window contains a sf::RenderWindow?
        bool m_accessToWindow;

        // Internal container to store all widgets
        GuiContainer m_Container;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template<class T>
        friend class SharedWidgetPtr;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_WINDOW_HPP
