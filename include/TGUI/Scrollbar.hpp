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


#ifndef TGUI_SCROLLBAR_HPP
#define TGUI_SCROLLBAR_HPP


#include <TGUI/Widget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class ScrollbarRenderer;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Scrollbar widget
    ///
    /// Signals:
    ///     - ValueChanged (The value of the scrollbar has changed)
    ///         * Optional parameter int: New value
    ///         * Uses Callback member 'value'
    ///
    ///     - Inherited signals from Widget
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Scrollbar : public Widget
    {
    public:

        typedef std::shared_ptr<Scrollbar> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const Scrollbar> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Scrollbar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~Scrollbar() {}


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Create the scrollbar
        ///
        /// @param themeFileFilename  Filename of the theme file.
        /// @param section             The section in the theme file to read.
        ///
        /// @throw Exception when the theme file could not be opened.
        /// @throw Exception when the theme file did not contain the requested section with the needed information.
        /// @throw Exception when one of the images, described in the theme file, could not be loaded.
        ///
        /// When an empty string is passed as filename, the built-in white theme will be used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Scrollbar::Ptr create(const std::string& themeFileFilename = "", const std::string& section = "Scrollbar");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another scrollbar
        ///
        /// @param scrollbar  The other scrollbar
        ///
        /// @return The new scrollbar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Scrollbar::Ptr copy(Scrollbar::ConstPtr scrollbar);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        ///
        /// @return Reference to the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<ScrollbarRenderer> getRenderer() const
        {
            return std::static_pointer_cast<ScrollbarRenderer>(m_renderer);
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
        virtual void setPosition(const Layout& position) override;
        using Transformable::setPosition;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the scrollbar.
        ///
        /// @param size  The new size of the scrollbar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout& size) override;
        using Transformable::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a maximum value.
        ///
        /// @param maximum  The new maximum value
        ///
        /// When the value is bigger than (maximum - low value), the value is set to maximum - low value.
        /// The default maximum value is 10.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaximum(unsigned int maximum);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the current value.
        ///
        /// @param value  The new value
        ///
        /// The value has to be smaller than maximum - low value.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setValue(unsigned int value);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the 'low value'.
        ///
        /// @param lowValue  The new low value.
        ///                  In e.g. a list box, this value is the amount of items that fit inside the list box.
        ///
        /// Until the maximum is bigger than this value, no scrollbar will be drawn.
        /// You can however choose to always draw the scrollbar by calling setAutoHide(false).
        /// The default low value is 6.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setLowValue(unsigned int lowValue);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the maximum value.
        ///
        /// @return The current maximum value
        ///
        /// The default maximum value is 10.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getMaximum() const
        {
            return m_maximum;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the current value.
        ///
        /// @return The current value
        ///
        /// The default value is 0.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getValue() const
        {
            return m_value;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the low value.
        ///
        /// @see setLowValue
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getLowValue() const
        {
            return m_lowValue;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes how much the value changes when pressing one of the arrows of the scrollbar
        ///
        /// @param scrollAmount  How far should the scrollbar scroll when an arrow is clicked?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowScrollAmount(unsigned int scrollAmount)
        {
            m_scrollAmount = scrollAmount;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns how much the value changes when pressing one of the arrows of the scrollbar
        ///
        /// @return How far should the scrollbar scroll when an arrow is clicked?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getArrowScrollAmount()
        {
            return m_scrollAmount;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether the scrollbar should hide automatically or not.
        ///
        /// When true (default), the scrollbar will not be drawn when the maximum is smaller than the low value.
        ///
        /// @param autoHide  Should the scrollbar be invisible when you can't scroll?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setAutoHide(bool autoHide)
        {
            m_autoHide = autoHide;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the scrollbar is hiding automatically or not.
        ///
        /// When true (default), the scrollbar will not be drawn when the maximum is smaller than the low value.
        /// So when you can't scroll, the scrollbar won't be drawn.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getAutoHide() const
        {
            return m_autoHide;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the transparency of the widget.
        ///
        /// @param transparency  The transparency of the widget.
        ///                      0 is completely transparent, while 255 (default) means fully opaque.
        ///
        /// Note that this will only change the transparency of the images. The parts of the widgets that use a color will not
        /// be changed. You must change them yourself by setting the alpha channel of the color.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setTransparency(unsigned char transparency) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool mouseOnWidget(float x, float y) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMousePressed(float x, float y) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMouseReleased(float x, float y) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseMoved(float x, float y) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseWheelMoved(int delta, int x, int y) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void widgetFocused() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Widget::Ptr clone() override
        {
            return std::make_shared<Scrollbar>(*this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        enum class Part
        {
            Track,
            Thumb,
            ArrowUp,
            ArrowDown
        };

        // Keep track on which part of the scrollbar the mouse is standing
        Part m_mouseHoverOverPart = Part::Thumb;

        // When the mouse went down, did it go down on top of the thumb? If so, where?
        bool m_mouseDownOnThumb = false;
        sf::Vector2f m_mouseDownOnThumbPos;

        unsigned int m_maximum = 10;
        unsigned int m_value = 0;

        // Maximum should be above this value before the scrollbar is needed
        unsigned int m_lowValue = 6;

        // Is the scrollbar draw vertically?
        bool m_verticalScroll = true;

        // Does the image lie vertically?
        bool m_verticalImage = true;

        // How far should the value change when pressing one of the arrows?
        unsigned int m_scrollAmount = 1;

        // When no scrollbar is needed, should the scrollbar be drawn or stay hidden?
        bool m_autoHide = true;

        // Did the mouse went down on one of the arrows?
        bool m_mouseDownOnArrow = false;

        sf::FloatRect m_track;
        sf::FloatRect m_thumb;
        sf::FloatRect m_arrowUp;
        sf::FloatRect m_arrowDown;

        // ListBox, ComboBox and TextBox can access the scrollbar directly
        friend class ListBox;
        friend class ComboBox;
        friend class TextBox;
        friend class ChatBox;

        friend class ScrollbarRenderer;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API ScrollbarRenderer : public WidgetRenderer
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param scrollbar  The scrollbar that is connected to the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ScrollbarRenderer(Scrollbar* scrollbar) : m_scrollbar{scrollbar} {}


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Dynamically change a property of the renderer, without even knowing the type of the widget.
        ///
        /// This function should only be used when you don't know the type of the widget.
        /// Otherwise you can make a direct function call to make the wanted change.
        ///
        /// @param property  The property that you would like to change
        /// @param value     The new value that you like to assign to the property
        /// @param rootPath  Path that should be placed in front of any resource filename
        ///
        /// @throw Exception when the property does not exist for this widget.
        /// @throw Exception when the value is invalid for this property.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setProperty(std::string property, const std::string& value, const std::string& rootPath = getResourcePath()) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image of the track that is displayed when the mouse is not on top of the scrollbar
        ///
        /// When all normal are set, then the alternative color properties will be ignored.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTrackNormalImage(const std::string& filename,
                                 const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                 const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                 bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image of the track that is displayed when the mouse is standing on top of the scrollbar
        ///
        /// This image is ignored when no normal track image has been set.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTrackHoverImage(const std::string& filename,
                                const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image of the thumb that is displayed when the mouse is not on top of the scrollbar
        ///
        /// When all normal are set, then the alternative color properties will be ignored.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setThumbNormalImage(const std::string& filename,
                                 const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                 const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                 bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image of the thumb that is displayed when the mouse is standing on top of the scrollbar
        ///
        /// This image is ignored when no normal thumb image has been set.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setThumbHoverImage(const std::string& filename,
                                const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image that is used as the up arrow.
        ///
        /// When all normal are set, then the alternative color properties will be ignored.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowUpNormalImage(const std::string& filename,
                                   const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                   const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                   bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image that is used as the up arrow when the mouse is on top of this arrow.
        ///
        /// When hover image is ignored if the normal image has not been set.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowUpHoverImage(const std::string& filename,
                                   const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                   const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                   bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image that is used as the down arrow.
        ///
        /// When all normal are set, then the alternative color properties will be ignored.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowDownNormalImage(const std::string& filename,
                                     const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                     const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                     bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the image that is used as the down arrow when the mouse is on top of this arrow.
        ///
        /// When hover image is ignored if the normal image has not been set.
        ///
        /// Pass an empty string to unset the image.
        ///
        /// @param filename   Filename of the image to load.
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image.
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param repeated   Should the image be repeated or stretched when the size is bigger than the image?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowDownHoverImage(const std::string& filename,
                                     const sf::IntRect& partRect = sf::IntRect(0, 0, 0, 0),
                                     const sf::IntRect& middlePart = sf::IntRect(0, 0, 0, 0),
                                     bool repeated = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the track.
        ///
        /// @param color  New track color
        ///
        /// This color will overwrite the color for both the normal and hover state.
        ///
        /// Note that this color is ignored when a track and thumb image have been set.
        ///
        /// @see setTrackColorNormal
        /// @see setTrackColorHover
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTrackColor(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the track in the normal state (mouse not on the track).
        ///
        /// @param color  New track color
        ///
        /// Note that this color is ignored when a track and thumb image have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTrackColorNormal(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the track in hover state (mouse on top of the track).
        ///
        /// @param color  New track color
        ///
        /// Note that this color is ignored when a track and thumb image have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTrackColorHover(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the thumb.
        ///
        /// @param color  New thumb color
        ///
        /// This color will overwrite the color for both the normal and hover state.
        ///
        /// Note that this color is ignored when a track and thumb image have been set.
        ///
        /// @see setTrackColorNormal
        /// @see setTrackColorHover
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setThumbColor(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the thumb in the normal state (mouse not on the thumb).
        ///
        /// @param color  New thumb color
        ///
        /// Note that this color is ignored when a track and thumb image have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setThumbColorNormal(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the thumb in hover state (mouse on top of the thumb).
        ///
        /// @param color  New thumb color
        ///
        /// Note that this color is ignored when a track and thumb image have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setThumbColorHover(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the background color of the arrows.
        ///
        /// @param color  New arrow background color
        ///
        /// This color will overwrite the color for both normal and hover states.
        ///
        /// Note that this color is ignored when all normal images have been set.
        ///
        /// @see setArrowBackgroundColorNormal
        /// @see setArrowBackgroundColorHover
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowBackgroundColor(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the background color of the arrows in the normal state (mouse not on arrow).
        ///
        /// @param color  New background color
        ///
        /// Note that this color is ignored when all normal images have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowBackgroundColorNormal(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the background color of the arrows in the hover state (mouse standing on top of the arrow).
        ///
        /// @param color  New arrow background color
        ///
        /// Note that this color is ignored when all normal images have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowBackgroundColorHover(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the arrows.
        ///
        /// @param color  New arrow color
        ///
        /// This color will overwrite the color for both normal and hover states.
        ///
        /// Note that this color is ignored when all normal images have been set.
        ///
        /// @see setArrowColorNormal
        /// @see setArrowColorHover
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowColor(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the arrows in the normal state (mouse not on arrow).
        ///
        /// @param color  New arrow color
        ///
        /// Note that this color is ignored when all normal images have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowColorNormal(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the arrows in the hover state (mouse standing on top of the arrow).
        ///
        /// @param color  New arrow color
        ///
        /// Note that this color is ignored when all normal images have been set.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setArrowColorHover(const sf::Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::shared_ptr<WidgetRenderer> clone(Widget* widget) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ScrollbarRenderer(const ScrollbarRenderer&) = default;
        ScrollbarRenderer& operator=(const ScrollbarRenderer&) = delete;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        Scrollbar* m_scrollbar;

        Texture    m_textureTrackNormal;
        Texture    m_textureTrackHover;
        Texture    m_textureThumbNormal;
        Texture    m_textureThumbHover;
        Texture    m_textureArrowUpNormal;
        Texture    m_textureArrowUpHover;
        Texture    m_textureArrowDownNormal;
        Texture    m_textureArrowDownHover;

        sf::Color  m_trackColorNormal           = {255, 255, 255};
        sf::Color  m_trackColorHover            = {255, 255, 255};
        sf::Color  m_thumbColorNormal           = {220, 220, 220};
        sf::Color  m_thumbColorHover            = {210, 210, 210};
        sf::Color  m_arrowBackgroundColorNormal = {245, 245, 245};
        sf::Color  m_arrowBackgroundColorHover  = {255, 255, 255};
        sf::Color  m_arrowColorNormal           = { 60,  60,  60};
        sf::Color  m_arrowColorHover            = {  0,   0,   0};

        friend class Scrollbar;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SCROLLBAR_HPP
