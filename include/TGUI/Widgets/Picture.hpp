/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_PICTURE_HPP
#define TGUI_PICTURE_HPP


#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Picture widget
    ///
    /// Signals:
    ///     - DoubleClicked (double left clicked on top of the picture)
    ///         * Optional parameter sf::Vector2f: Position where you clicked
    ///         * Uses Callback member 'mouse'
    ///
    ///     - Inherited signals from ClickableWidget
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Picture : public ClickableWidget
    {
    public:

        typedef std::shared_ptr<Picture> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const Picture> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Picture();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor to creates the picture
        ///
        /// @param filename       The absolute or relative filename of the image that should be loaded
        /// @param fullyClickable This affects what happens when clicking on a transparent pixel in the image.
        ///                       Is the click caught by the picture, or does the event pass to the widgets behind it?
        ///
        /// @throw Exception when the image could not be loaded (probably not found)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Picture(const sf::String& filename, bool fullyClickable = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor to create the picture from an image
        ///
        /// @param texture  The texture to load the picture from
        /// @param fullyClickable This affects what happens when clicking on a transparent pixel in the image.
        ///                       Is the click caught by the picture, or does the event pass to the widgets behind it?
        ///
        /// @code
        /// auto picture1 = tgui::Picture::create({"image.png", {10, 10, 80, 80}});
        ///
        /// sf::Texture texture;
        /// texture.loadFromFile("image.png", {10, 10, 80, 80});
        /// auto picture2 = tgui::Picture::create(texture);
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Picture(const Texture& texture, bool fullyClickable = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image
        ///
        /// @param filename       The absolute or relative filename of the image that should be loaded
        /// @param fullyClickable This affects what happens when clicking on a transparent pixel in the image.
        ///                       Is the click caught by the picture, or does the event pass to the widgets behind it?
        ///
        /// @throw Exception when the image could not be loaded (probably not found)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTexture(const sf::String& filename, bool fullyClickable = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image
        ///
        /// @param texture  The texture to load the picture from
        /// @param fullyClickable This affects what happens when clicking on a transparent pixel in the image.
        ///                       Is the click caught by the picture, or does the event pass to the widgets behind it?
        ///
        /// @code
        /// picture1->setTexture({"image.png", {10, 10, 80, 80}});
        ///
        /// sf::Texture texture;
        /// texture.loadFromFile("image.png", {10, 10, 80, 80});
        /// picture2->setTexture(texture);
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTexture(const Texture& texture, bool fullyClickable = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new picture widget
        ///
        /// @param filename  The filename of the texture to load the picture from
        /// @param fullyClickable This affects what happens when clicking on a transparent pixel in the image.
        ///                       Is the click caught by the picture, or does the event pass to the widgets behind it?
        ///
        /// @return The new picture
        ///
        /// @code
        /// auto picture1 = Picture::create("image.png");
        ///
        /// auto picture2 = Picture::create({"image.png", {20, 15, 60, 40}}); // Load part of the image from (20,15) to (80,55)
        ///
        /// sf::Texture texture;
        /// texture.loadFromFile("image.png", {20, 15, 60, 40});
        /// auto picture3 = Picture::create(texture);
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Picture::Ptr create(const char* filename, bool fullyClickable = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new picture widget
        ///
        /// @param texture  The texture to load the picture from
        /// @param fullyClickable This affects what happens when clicking on a transparent pixel in the image.
        ///                       Is the click caught by the picture, or does the event pass to the widgets behind it?
        ///
        /// @return The new picture
        ///
        /// @code
        /// auto picture1 = Picture::create("image.png");
        ///
        /// auto picture2 = Picture::create({"image.png", {20, 15, 60, 40}}); // Load part of the image from (20,15) to (80,55)
        ///
        /// sf::Texture texture;
        /// texture.loadFromFile("image.png", {20, 15, 60, 40});
        /// auto picture3 = Picture::create(texture);
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Picture::Ptr create(const Texture& texture = {}, bool fullyClickable = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another picture
        ///
        /// @param picture  The other picture
        ///
        /// @return The new picture
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Picture::Ptr copy(Picture::ConstPtr picture);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the filename of the image that was used to load the widget.
        ///
        /// @return Filename of loaded image.
        ///         Empty string when no image was loaded yet or when it was loaded directly from a texture.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::String& getLoadedFilename() const
        {
            return m_loadedFilename;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Set the position of the widget
        ///
        /// This function completely overwrites the previous position.
        /// See the move function to apply an offset based on the previous position instead.
        /// The default position of a transformable widget is (0, 0).
        ///
        /// @param position  New position
        ///
        /// @see move, getPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setPosition(const Layout2d& position) override;
        using Transformable::setPosition;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the picture.
        ///
        /// @param size  The new size of the picture
        ///
        /// The image will be scaled to fit this size.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSize(const Layout2d& size) override;
        using Transformable::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enable or disable the smooth filter.
        ///
        /// When the filter is activated, the texture appears smoother so that pixels are less noticeable.
        /// However if you want the texture to look exactly the same as its source file, you should leave it disabled.
        /// The smooth filter is disabled by default.
        ///
        /// @param smooth True to enable smoothing, false to disable it
        ///
        /// @see isSmooth
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSmooth(bool smooth = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Tell whether the smooth filter is enabled or not.
        ///
        /// @return True if smoothing is enabled, false if it is disabled
        ///
        /// @see setSmooth
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isSmooth() const
        {
            return m_texture.isSmooth();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the opacity of the widget.
        ///
        /// @param opacity  The opacity of the widget. 0 means completely transparent, while 1 (default) means fully opaque.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setOpacity(float opacity) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool mouseOnWidget(float x, float y) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMouseReleased(float x, float y) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<Picture>(*this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function is called every frame with the time passed since the last frame.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void update(sf::Time elapsedTime) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        sf::String m_loadedFilename;

        Texture m_texture;

        // Set to false when clicks on transparent parts of the picture should go to the widgets behind the picture
        bool m_fullyClickable = true;

        // Will be set to true after the first click, but gets reset to false when the second click does not occur soon after
        bool m_possibleDoubleClick = false;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_PICTURE_HPP
