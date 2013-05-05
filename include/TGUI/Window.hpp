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


#ifndef TGUI_WINDOW_HPP
#define TGUI_WINDOW_HPP

#include <queue>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Window : public Group
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor which will initialize the sf::RenderWindow internally.
        ///
        /// This constructor doesn't actually create the window, use the other constructors or call "create" to do so.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Window();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Constructor to tell this window that there is a seperate sf::RenderWindow object.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Window(sf::RenderWindow& window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Calling this constructor will create the sf::RenderWindow internally.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Window(sf::VideoMode mode, const std::string& title, sf::Uint32 style = sf::Style::Default, const sf::ContextSettings& settings = sf::ContextSettings());


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Calling this constructor will create the sf::RenderWindow internally.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        explicit Window(sf::WindowHandle handle, const sf::ContextSettings& settings = sf::ContextSettings());


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~Window();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Passes the event to the objects.
        ///
        /// \param event  The event that was polled from the window
        ///
        /// You should call this function in your event loop.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void handleEvent(sf::Event event);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Draws all the objects that were added to the window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawGUI();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the next callback from the callback queue.
        ///
        /// This function works just like the pollEvent function from sfml.
        ///
        /// The window stores the callback of all the objects.
        /// This function will return the next callback and then remove it from the queue.
        ///
        /// Note that more than one callbacks may be present in the queue, thus you should always call this
        /// function in a loop to make sure that you process every callback.
        ///
        /// \param callback  An empty tgui::Callback object that will be (partly) filled when there is a callback.
        ///
        /// \return
        ///        - true when there is another callback. The \a callback parameter will be filled with information.
        ///        - false when there is no callback. The \a callback parameter remains uninitialized and may not be used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool pollCallback(Callback& callback);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update the internal clock to make animation possible. This function is called automatically by window.drawGUI.
        // You will thus only need to call it yourself when you are drawing manually.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateTime(const sf::Time& elapsedTime);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // TGUI uses this function internally to handle object callbacks.
        // When you tell an object to send its callbacks to its parent then this function is called.
        //
        // When one or more global callback functions were set then these functions will be called.
        // Otherwise, the callback will be added to the callback queue and you will be able to poll it later with pollCallback.
        //
        // You can use this function to fake an object callback.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void addChildCallback(Callback& callback);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Used internally to get the size of the window.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f getDisplaySize();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Returns a pointer to the internal sf::RenderWindow.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::RenderWindow* getRenderWindow();


    // All functions below are from sf::RenderWindow, sf::Window and sf::RenderTarget

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Recreate the window
        ///
        /// If the window was already created, it closes it first.
        /// If \a style contains Style::Fullscreen, then \a mode must be a valid video mode.
        ///
        /// \param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
        /// \param title    Title of the window
        /// \param style    Window style
        /// \param settings Additional settings for the underlying OpenGL context
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void create(sf::VideoMode mode, const sf::String& title, sf::Uint32 style = sf::Style::Default, const sf::ContextSettings& settings = sf::ContextSettings());


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Create (or recreate) the window from an existing control
        ///
        /// Use this function if you want to create an OpenGL rendering area into an already existing control.
        /// If the window was already created, it closes it first.
        ///
        /// \param handle   Platform-specific handle of the control
        /// \param settings Additional settings for the underlying OpenGL context
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void create(sf::WindowHandle handle, const sf::ContextSettings& settings = sf::ContextSettings());


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the size of the rendering region of the window
        ///
        /// The size doesn't include the titlebar and borders of the window.
        ///
        /// \return Size in pixels
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Vector2u getSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Copy the current contents of the window to an image
        ///
        /// This is a slow operation, whose main purpose is to make screenshots of the application. If you want to update an
        /// image with the contents of the window and then use it for drawing, you should rather use a sf::Texture and its
        /// update(Window&) function. You can also draw things directly to a texture with the sf::RenderTexture class.
        ///
        /// \return Image containing the captured contents
        ///
        /// \warning You should never use this function unless you completely don't care about performance.
        ///          If you want to capture the window then you'll have to split tgui::Window and sf::RenderWindow.
        ///          Then you can call the capture function from the RenderWindow, which will be faster than this one.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::Image capture() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Close the window and destroy all the attached resources
        ///
        /// After calling this function, the Window instance remains valid and you can call create() to recreate the window.
        /// All other functions such as pollEvent() or display() will still work (i.e. you don't have to test isOpen() every time),
        /// and will have no effect on closed windows.
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void close();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Tell whether or not the window is open
        ///
        /// This function returns whether or not the window exists.
        /// Note that a hidden window (setVisible(false)) is open (therefore this function would return true).
        ///
        /// \return True if the window is open, false if it has been closed
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isOpen() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the settings of the OpenGL context of the window
        ///
        /// Note that these settings may be different from what was passed to the constructor or the create() function,
        /// if one or more settings were not supported. In this case, SFML chose the closest match.
        ///
        /// \return Structure containing the OpenGL context settings
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::ContextSettings& getSettings() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Pop the event on top of the event queue, if any, and return it
        ///
        /// This function is not blocking: if there's no pending event then it will return false and leave \a event unmodified.
        /// Note that more than one event may be present in the event queue, thus you should always call this function in a loop
        /// to make sure that you process every pending event.
        /// \code
        /// sf::Event event;
        /// while (window.pollEvent(event))
        /// {
        ///    // process event...
        /// }
        /// \endcode
        ///
        /// \param event Event to be returned
        ///
        /// \return True if an event was returned, or false if the event queue was empty
        ///
        /// \see waitEvent
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool pollEvent(sf::Event& event);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Wait for an event and return it
        ///
        /// This function is blocking: if there's no pending event then it will wait until an event is received.
        /// After this function returns (and no error occured), the \a event object is always valid and filled properly.
        /// This function is typically used when you have a thread that is dedicated to events handling: you want to make
        /// this thread sleep as long as no new event is received.
        /// \code
        /// sf::Event event;
        /// if (window.waitEvent(event))
        /// {
        ///    // process event...
        /// }
        /// \endcode
        ///
        /// \param event Event to be returned
        ///
        /// \return False if any error occured
        ///
        /// \see pollEvent
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool waitEvent(sf::Event& event);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the position of the window
        ///
        /// \return Position of the window, in pixels
        ///
        /// \see setPosition
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2i getPosition() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the position of the window on screen
        ///
        /// This function only works for top-level windows (i.e. it will be ignored for windows created from the handle
        ///                                                 of a child window/control).
        ///
        /// \param position New position, in pixels
        ///
        /// \see getPosition
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setPosition(const Vector2i& position);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the size of the rendering region of the window
        ///
        /// \param size New size, in pixels
        ///
        /// \see getSize
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Vector2u size);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the title of the window
        ///
        /// \param title New title
        ///
        /// \see setIcon
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTitle(const sf::String& title);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the window's icon
        ///
        /// \a pixels must be an array of \a width x \a height pixels in 32-bits RGBA format.
        ///
        /// The OS default icon is used by default.
        ///
        /// \param width  Icon's width, in pixels
        /// \param height Icon's height, in pixels
        /// \param pixels Pointer to the array of pixels in memory
        ///
        /// \see setTitle
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setIcon(unsigned int width, unsigned int height, const sf::Uint8* pixels);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Show or hide the window
        ///
        /// The window is shown by default.
        ///
        /// \param visible True to show the window, false to hide it
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setVisible(bool visible);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Enable or disable vertical synchronization
        ///
        /// Activating vertical synchronization will limit the number of frames displayed to the refresh rate of the monitor.
        /// This can avoid some visual artifacts, and limit the framerate to a good value (but not constant across different computers).
        ///
        /// Vertical synchronization is disabled by default.
        ///
        /// \param enabled True to enable v-sync, false to deactivate it
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setVerticalSyncEnabled(bool enabled);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Show or hide the mouse cursor
        ///
        /// The mouse cursor is visible by default.
        ///
        /// \param visible True to show the mouse cursor, false to hide it
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMouseCursorVisible(bool visible);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Enable or disable automatic key-repeat
        ///
        /// If key repeat is enabled, you will receive repeated KeyPressed events while keeping a key pressed.
        /// If it is disabled, you will only get a single event when the key is pressed.
        ///
        /// Key repeat is enabled by default.
        ///
        /// \param enabled True to enable, false to disable
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setKeyRepeatEnabled(bool enabled);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Limit the framerate to a maximum fixed frequency
        ///
        /// If a limit is set, the window will use a small delay after each call to display() to ensure that the current frame
        /// lasted long enough to match the framerate limit. SFML will try to match the given limit as much as it can,
        /// but since it internally uses sf::sleep, whose precision depends on the underlying OS, the results may be a little
        /// unprecise as well (for example, you can get 65 FPS when requesting 60).
        ///
        /// \param limit Framerate limit, in frames per seconds (use 0 to disable limit)
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setFramerateLimit(unsigned int limit);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the joystick threshold
        ///
        /// The joystick threshold is the value below which no JoystickMoved event will be generated.
        ///
        /// The threshold value is 0.1 by default.
        ///
        /// \param threshold New threshold, in the range [0, 100]
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setJoystickThreshold(float threshold);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Activate or deactivate the window as the current target
        ///        for OpenGL rendering
        ///
        /// A window is active only on the current thread, if you want to make it active on another thread you have to
        /// deactivate it on the previous thread first if it was active.
        /// Only one window can be active on a thread at a time, thus the window previously active (if any) automatically
        /// gets deactivated.
        ///
        /// \param active True to activate, false to deactivate
        ///
        /// \return True if operation was successful, false otherwise
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setActive(bool active = true) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Display on screen what has been rendered to the window so far
        ///
        /// This function is typically called after all OpenGL rendering has been done for the current frame, in order to show
        /// it on screen.
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void display();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the OS-specific handle of the window
        ///
        /// The type of the returned handle is sf::WindowHandle, which is a typedef to the handle type defined by the OS.
        /// You shouldn't need to use this function, unless you have very specific stuff to implement that SFML doesn't support,
        /// or implement a temporary workaround until a bug is fixed.
        ///
        /// \return System handle of the window
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::WindowHandle getSystemHandle() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Clear the entire target with a single color
        ///
        /// This function is usually called once every frame, to clear the previous contents of the target.
        ///
        /// \param color Fill color to use to clear the render target
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void clear(const sf::Color& color = sf::Color(0, 0, 0, 255));


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the current active view
        ///
        /// The view is like a 2D camera, it controls which part of the 2D scene is visible, and how it is viewed in
        /// the render-target. The new view will affect everything that is drawn, until another view is set.
        /// The render target keeps its own copy of the view object, so it is not necessary to keep the original one alive
        /// after calling this function.
        /// To restore the original view of the target, you can pass the result of getDefaultView() to this function.
        ///
        /// \param view New view to use
        ///
        /// \see getView, getDefaultView
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setView(const sf::View& view);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the view currently in use in the render target
        ///
        /// \return The view object that is currently used
        ///
        /// \see setView, getDefaultView
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::View& getView() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the default view of the render target
        ///
        /// The default view has the initial size of the render target, and never changes after the target has been created.
        ///
        /// \return The default view of the render target
        ///
        /// \see setView, getView
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::View& getDefaultView() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the viewport of a view, applied to this render target
        ///
        /// The viewport is defined in the view as a ratio, this function simply applies this ratio to the current dimensions
        /// of the render target to calculate the pixels rectangle that the viewport actually covers in the target.
        ///
        /// \param view The view for which we want to compute the viewport
        ///
        /// \return Viewport rectangle, expressed in pixels
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::IntRect getViewport(const sf::View& view) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Convert a point from target coordinates to world coordinates, using the current view
        ///
        /// This function is an overload of the mapPixelToCoords function that implicitely uses the current view.
        /// It is equivalent to:
        /// \code
        /// target.mapPixelToCoords(point, target.getView());
        /// \endcode
        ///
        /// \param point Pixel to convert
        ///
        /// \return The converted point, in "world" coordinates
        ///
        /// \see mapCoordsToPixel
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f mapPixelToCoords(const Vector2i& point) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Convert a point from target coordinates to world coordinates
        ///
        /// This function finds the 2D position that matches the given pixel of the render-target. In other words, it does
        /// the inverse of what the graphics card does, to find the initial position of a rendered pixel.
        ///
        /// Initially, both coordinate systems (world units and target pixels) match perfectly. But if you define a custom view
        /// or resize your render-target, this assertion is not true anymore, ie. a point located at (10, 50) in your
        /// render-target may map to the point (150, 75) in your 2D world -- if the view is translated by (140, 25).
        ///
        /// For render-windows, this function is typically used to find which point (or object) is located below the mouse cursor.
        ///
        /// This version uses a custom view for calculations, see the other overload of the function if you want to use
        /// the current view of the render-target.
        ///
        /// \param point Pixel to convert
        /// \param view The view to use for converting the point
        ///
        /// \return The converted point, in "world" units
        ///
        /// \see mapCoordsToPixel
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f mapPixelToCoords(const Vector2i& point, const sf::View& view) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Convert a point from world coordinates to target coordinates, using the current view
        ///
        /// This function is an overload of the mapCoordsToPixel function that implicitely uses the current view.
        /// It is equivalent to:
        /// \code
        /// target.mapCoordsToPixel(point, target.getView());
        /// \endcode
        ///
        /// \param point Point to convert
        ///
        /// \return The converted point, in target coordinates (pixels)
        ///
        /// \see mapPixelToCoords
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2i mapCoordsToPixel(const Vector2f& point) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Convert a point from world coordinates to target coordinates
        ///
        /// This function finds the pixel of the render-target that matches the given 2D point. In other words, it goes through
        /// the same process as the graphics card, to compute the final position of a rendered point.
        ///
        /// Initially, both coordinate systems (world units and target pixels) match perfectly. But if you define a custom view
        /// or resize your render-target, this assertion is not true anymore, ie. a point located at (150, 75) in your 2D world
        /// may map to the pixel (10, 50) of your render-target -- if the view is translated by (140, 25).
        ///
        /// This version uses a custom view for calculations, see the other overload of the function if you want to use
        /// the current view of the render-target.
        ///
        /// \param point Point to convert
        /// \param view The view to use for converting the point
        ///
        /// \return The converted point, in target coordinates (pixels)
        ///
        /// \see mapPixelToCoords
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2i mapCoordsToPixel(const Vector2f& point, const sf::View& view) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Draw a drawable object to the render-target
        ///
        /// \param drawable Object to draw
        /// \param states   Render states to use for drawing
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Draw primitives defined by an array of vertices
        ///
        /// \param vertices    Pointer to the vertices
        /// \param vertexCount Number of vertices in the array
        /// \param type        Type of primitives to draw
        /// \param states      Render states to use for drawing
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(const sf::Vertex* vertices, unsigned int vertexCount, sf::PrimitiveType type, const sf::RenderStates& states = sf::RenderStates::Default);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Save the current OpenGL render states and matrices
        ///
        /// This function can be used when you mix SFML drawing and direct OpenGL rendering. Combined with PopGLStates,
        /// it ensures that:
        /// \li SFML's internal states are not messed up by your OpenGL code
        /// \li your OpenGL states are not modified by a call to a SFML function
        ///
        /// More specifically, it must be used around code that calls Draw functions.
        /// Example:
        /// \code
        /// // OpenGL code here...
        /// window.pushGLStates();
        /// window.draw(...);
        /// window.draw(...);
        /// window.popGLStates();
        /// // OpenGL code here...
        /// \endcode
        ///
        /// Note that this function is quite expensive: it saves all the possible OpenGL states and matrices, even the ones you
        /// don't care about. Therefore it should be used wisely. It is provided for convenience, but the best results will
        /// be achieved if you handle OpenGL states yourself (because you know which states have really changed, and need to be
        /// saved and restored). Take a look at the resetGLStates function if you do so.
        ///
        /// \see popGLStates
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void pushGLStates();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Restore the previously saved OpenGL render states and matrices
        ///
        /// See the description of pushGLStates to get a detailed
        /// description of these functions.
        ///
        /// \see pushGLStates
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void popGLStates();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Reset the internal OpenGL states so that the target is ready for drawing
        ///
        /// This function can be used when you mix SFML drawing and direct OpenGL rendering, if you choose not to use
        /// pushGLStates/popGLStates. It makes sure that all OpenGL states needed by SFML are set, so that subsequent draw()
        /// calls will work as expected.
        ///
        /// Example:
        /// \code
        /// // OpenGL code here...
        /// glPushAttrib(...);
        /// window.resetGLStates();
        /// window.draw(...);
        /// window.draw(...);
        /// glPopAttrib(...);
        /// // OpenGL code here...
        /// \endcode
        ///
        /// If you have your own sfml window and passed a reference to the consructor of this window,
        /// then this function will just be called on that sfml window.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void resetGLStates();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        // This will store all object callbacks until you pop them with getCallback
        std::queue<Callback> m_Callback;

        // The internal clock which is used for animation of objects
        sf::Clock m_Clock;

        // The sfml window
        sf::RenderWindow* m_Window;

        // Was a reference passed to the sfml window or did we create it ourself?
        bool m_OwningWindow;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_WINDOW_HPP
