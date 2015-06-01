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


#ifndef TGUI_BOXLAYOUT_HPP
#define TGUI_BOXLAYOUT_HPP

#include <TGUI/Container.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Abstract class for layout container.
    ///
    /// Signals:
    ///     - Inherited signals from Container
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BoxLayout : public Container
    {
    public:

        typedef std::shared_ptr<BoxLayout> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const BoxLayout> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a widget at the end of the layout.
        ///
        /// @param widget      Pointer to the widget you would like to add
        /// @param widgetName  An identifier to access to the widget later
        ///
        /// Usage example:
        /// @code
        /// tgui::Picture::Ptr pic = tgui::Picture::create();// Create a picture
        /// layout->add(pic);                                // Add the picture to the layout
        /// layout->remove(pic);                             // Remove the picture from the container
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void add(const tgui::Widget::Ptr& widget, const sf::String& widgetName = "") override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Insert a widget to the layout. The widget is inserted before position.
        ///
        /// @param widget      Pointer to the widget you would like to add
        /// @param position    Position of the widget in the container
        /// @param widgetName  An identifier to access to the widget later
        ///
        /// Usage example:
        /// @code
        /// tgui::Picture::Ptr pic = tgui::Picture::create();// Create a picture
        /// layout->insert(pic, 2);                          // Add the picture after the second element in the layout
        /// layout->remove(pic);                             // Remove the picture from the container
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void insert(unsigned int position, const tgui::Widget::Ptr& widget, const sf::String& widgetName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a space at the end of the layout.
        /// Spaces are just hidden and disabled tgui::Button,
        /// so the widget of the space can be retrieved by get(spaceName) after the space is added.
        /// This allow to modify the space, for making e.g. a separation line with a custom texture.
        ///
        /// @param spaceName An identifier to access to the space later.
        ///
        /// Usage example:
        /// @code
        /// layout->addSpace("some space identifier");                           // Add the space at the end of the layout
        /// tgui::Button::Ptr spaceButton = layout->get("some space identifier");// Get the space widget
        /// spaceButton->show();                                                 // Make the space visible
        /// spaceButton->getRenderer()->setNormalImage("spaceTexture.png");      // And change its texture
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void addSpace(const sf::String& spaceName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Insert a space to the layout. The space is inserted before position.
        /// Spaces are just hidden and disabled tgui::Button,
        /// so the widget of the space can be retrieved by get(spaceName) after the space is added.
        /// This allow to modify the space, for making e.g. a separation line with a custom texture.
        ///
        /// @param position  Position of the space in the container
        /// @param spaceName An identifier to access to the space later
        ///
        /// Usage example:
        /// @code
        /// layout->insertSpace(2, "some space identifier");                     // Insert a space before the second element
        /// tgui::Button::Ptr spaceButton = layout->get("some space identifier");// Get the space widget
        /// spaceButton->show();                                                 // Make the space visible
        /// spaceButton->getRenderer()->setNormalImage("spaceTexture.png");      // And change its texture
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void insertSpace(unsigned int position, const sf::String& spaceName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a single widget that was added to the container.
        ///
        /// @param widget  Pointer to the widget to remove
        ///
        /// Usage example:
        /// @code
        /// tgui::Picture::Ptr pic(layout, "picName");
        /// tgui::Picture::Ptr pic2(layout, "picName2");
        /// layout.remove(pic);
        /// layout.remove(layout.get("picName2"));
        /// @endcode
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void remove(const tgui::Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a single widget that was added to the container.
        ///
        /// @param position  Position in the layout of the widget to remove
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void remove(unsigned int position);



        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the widget at the given position in the layout.
        ///
        /// @param position  Position in the layout of the widget
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Widget::Ptr get(unsigned int position);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the ratio of a widget.
        /// The ratio is the size that will have a widget relatively to others.
        /// By default, the ratio is equal to 1.
        /// So setting a ratio to 2 means that the widget will be 2 times larger than others.
        ///
        /// @param widget  Pointer to the widget
        /// @param ratio   New ratio to set to the widget
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setRatio(const Widget::Ptr& widget, float ratio);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the ratio of a widget.
        /// The ratio is the size that will have a widget relatively to others.
        /// By default, the ratio is equal to 1.
        /// So setting a ratio to 2 means that the widget will be 2 times larger than others.
        ///
        /// @param position  Position in the layout of the widget
        /// @param ratio     New ratio to set to the widget
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setRatio(unsigned int position, float ratio);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all widgets that were added to the container.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllWidgets();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual bool mouseOnWidget(float x, float y) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Reposition all the widgets.
        // This function must be override.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void updatePosition() = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

		std::vector<tgui::Widget::Ptr> m_layoutWidgets;///< The widgets, stored in the same order as displayed.
		std::vector<float> m_widgetsRatio;             ///< The ratio of each widget.


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BOXLAYOUT_HPP
