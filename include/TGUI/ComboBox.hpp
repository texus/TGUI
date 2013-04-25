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


#ifndef TGUI_COMBO_BOX_HPP
#define TGUI_COMBO_BOX_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API ComboBox : public Object, public ObjectBorders
    {
      public:

        typedef SharedObjectPtr<ComboBox> Ptr;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Copy constructor
        ///
        /// \param copy  Instance to copy
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox(const ComboBox& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~ComboBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Overload of assignment operator
        ///
        /// \param right  Instance to assign
        ///
        /// \return Reference to itself
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox& operator= (const ComboBox& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the object by calling the copy constructor.
        // This function calls new and if you use this function then you are responsible for calling delete.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ComboBox* clone();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Loads the combo box.
        ///
        /// \param comboBoxPathname         The path to the folder that contains the arrow image(s).
        ///                                 The folder must also contain an info.txt file, which will give more information about the combo box.
        /// \param width                    The width of the combo box (borders included).
        /// \param height                   The height of the combo box (borders included).
        /// \param nrOfItemsInListToDisplay The amount of items that are displayed when you click on the arrow.
        ///                                 If there is no scrollbar then this will also be the maximum amount of items that the
        ///                                 combo box can contain.
        /// \param scrollbarPathname        The ScrollbarPathname is the pathname needed to load the scrollbar.
        ///                                 If not provided then there will be no possibility to scroll, thus no items will
        ///                                 be added when the combo box is full.
        /// \return
        ///        - true on success
        ///        - false when the pathname is empty
        ///        - false when the info.txt file was not found
        ///        - false when the images couldn't be loaded
        ///        - false when scrollbar couldn't be loaded (only if \a scrollbarPathname isn't empty)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool load(const std::string& comboBoxPathname,
                          float width,
                          float height,
                          unsigned int nrOfItemsInListToDisplay = 10,
                          const std::string& scrollbarPathname = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the size of the combo box.
        ///
        /// \param width   The new width of the combo box (borders included)
        /// \param height  The new height of the combo box (borders included)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSize(float width, float height);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the size of the combo box.
        ///
        /// \return The size of the combo box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Vector2f getSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the pathname that was used to load the combo box.
        ///
        /// \return The pathname used to load the combo box.
        ///         When the combo box has not been loaded yet then this function will return an empty string.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getLoadedPathname() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the pathname that was used to load the scrollbar.
        ///
        /// \return The pathname used to load the scrollbar.
        ///         When no scrollbar was loaded then this function will return an empty string.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getLoadedScrollbarPathname() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the number of items that are displayed in the list.
        ///
        /// \param nrOfItemsInListToDisplay  The maximum number of items to display when the list of items is shown.
        ///
        /// When there is no scrollbar then this is the maximum number of items.
        /// If there is one, then it will only become visible when there are more items than this number.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setItemsToDisplay(unsigned int nrOfItemsInListToDisplay);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the number of items that are displayed in the list.
        ///
        /// \return The maximum number of items to display when the list of items is shown.
        ///
        /// When there is no scrollbar then this is the maximum number of items.
        /// If there is one, then it will only become visible when there are more items than this number.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int getItemsToDisplay() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the colors that are used in the combo box.
        ///
        /// \param backgroundColor          The color of the background of the combo box
        /// \param textColor                The color of the text
        /// \param selectedBackgroundColor  The color of the background of the selected item in the list
        /// \param selectedTextColor        The color of the text when it is selected in the list
        /// \param borderColor              The color of the borders
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void changeColors(const sf::Color& backgroundColor         = sf::Color::White,
                                  const sf::Color& textColor               = sf::Color::Black,
                                  const sf::Color& selectedBackgroundColor = sf::Color(50, 100, 200),
                                  const sf::Color& selectedTextColor       = sf::Color::White,
                                  const sf::Color& borderColor             = sf::Color::Black);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the background color that will be used inside the combo box.
        ///
        /// \param backgroundColor  The color of the background of the combo box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setBackgroundColor(const sf::Color& backgroundColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the text color that will be used inside the combo box.
        ///
        /// \param textColor  The color of the text
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setTextColor(const sf::Color& textColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the background color of the selected text that will be used inside the combo box.
        ///
        /// \param selectedBackgroundColor  The color of the background of the selected item in the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the text color of the selected text that will be used inside the combo box.
        ///
        /// \param selectedTextColor  The color of the text when it is selected in the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectedTextColor(const sf::Color& selectedTextColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the border color text that will be used inside the combo box.
        ///
        /// \param borderColor  The color of the borders
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setBorderColor(const sf::Color& borderColor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the background color that is currently being used inside the combo box.
        ///
        /// \return The color of the background of the combo box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getBackgroundColor() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the text color that is currently being used inside the combo box.
        ///
        /// \return The color of the text
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getTextColor() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the background color of the selected text that is currently being used inside the combo box.
        ///
        /// \return  The color of the background of the selected item in the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getSelectedBackgroundColor() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the text color of the selected text that is currently being used inside the combo box.
        ///
        /// \return The color of the text when it is selected in the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getSelectedTextColor() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the border color that is currently being used inside the combo box.
        ///
        /// \return The color of the borders
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getBorderColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the text font.
        ///
        /// When you don't call this function then the global font will be use.
        /// This global font can be changed with the setGlobalFont function from the parent.
        ///
        /// \param font  The new font.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setTextFont(const sf::Font& font);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the text font.
        ///
        /// \return  Pointer to the font that is currently being used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Font* getTextFont() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the size of the borders.
        ///
        /// \param leftBorder    The width of the left border
        /// \param topBorder     The height of the top border
        /// \param rightBorder   The width of the right border
        /// \param bottomBorder  The height of the bottom border
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setBorders(unsigned int leftBorder   = 0,
                                unsigned int topBorder    = 0,
                                unsigned int rightBorder  = 0,
                                unsigned int bottomBorder = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Adds an item to the list, so that it can be selected later.
        ///
        /// \param itemName  The name of the item you want to add (this is the text that will be displayed inside the combo box)
        ///
        /// \return
        ///         -  The index of the item when it was successfully added.
        ///         -  -1 when the combo box wasn't loaded correctly
        ///         -  -1 when the list is full (you have set a maximum item limit and you are trying to add more items)
        ///         -  -1 when there is no scrollbar and you try to have more items than the number of items to display
        ///
        /// \warning The index returned by this function may no longer correct when an item is removed.
        ///
        /// \see setMaximumItems
        /// \see setItemsToDisplay
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int addItem(const sf::String& itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Selects an item from the list.
        ///
        /// When adding items to the combo box with the addItem function, none of them will be selected.
        /// If you don't want the combo box to stay empty until the user selects something, but you want a default item instead,
        /// then you can use this function to select an item.
        /// The first item that matches the name will be selected.
        ///
        /// \param itemName  The item you want to select
        ///
        /// \return
        ///         - true on success
        ///         - false when none of the items matches the name
        ///
        /// \see setSelectedItem(unsigned int)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool setSelectedItem(const sf::String& itemName);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Selects an item from the list.
        ///
        /// When adding items to the combo box with the addItem function, none of them will be selected.
        /// If you don't want the combo box to stay empty until the user selects something, but you want a default item instead,
        /// then you can use this function to select an item.
        ///
        /// \param index  The index of the item
        ///
        /// \return
        ///         - true on success
        ///         - false when the index was too high
        ///
        /// \see setSelectedItem(sf::String)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool setSelectedItem(unsigned int index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Deselects the selected item.
        ///
        /// The combo box will be empty after this function is called.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void deselectItem();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes an item from the list with a given index.
        ///
        /// \param index  The index of the item to remove
        ///
        /// \return
        ///        - true when the item was removed
        ///        - false when the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool removeItem(unsigned int index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes the first item from the list with a given name.
        ///
        /// \param itemName  The item to remove
        ///
        /// \return
        ///        - true when the item was removed
        ///        - false when the name didn't match any item
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool removeItem(const sf::String& itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes all items from the list.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void removeAllItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the item name of the item with the given index.
        ///
        /// \param index  The index of the item
        ///
        /// \return The requested item.
        ///         The string will be empty when the index was too high.
        ///
        /// \see getItemIndex
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual sf::String getItem(unsigned int index) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the index of the first item with the given name.
        ///
        /// \param itemName  The name of the item
        ///
        /// \return The index of the item that matches the name.
        ///         If none of the items matches then the returned index will be -1.
        ///
        /// \warning This index may become wrong when an item is removed from the list.
        ///
        /// \see getItem
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getItemIndex(const sf::String& itemName) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the list that contains all the items.
        ///
        /// \return The vector of strings
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::vector<sf::String>& getItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the currently selected item.
        ///
        /// \return The selected item.
        ///         When no item was selected then this function will return an empty string.
        ///
        /// \see getSelectedItemIndex
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual sf::String getSelectedItem() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the index of the selected item.
        ///
        /// \return The index of the selected item.
        ///         When no item was selected then this function returns -1.
        ///
        /// \warning This index may become wrong when an item is removed from the list.
        ///
        /// \see getSelectedItem
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int getSelectedItemIndex() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the scrollbar that is displayed next to the list.
        ///
        /// \param scrollbarPathname  The pathname needed to load the scrollbar
        ///
        /// \return
        ///        - true when the scrollbar was successfully loaded
        ///        - false when the loading of the scrollbar failed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool setScrollbar(const std::string& scrollbarPathname);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes the scrollbar.
        ///
        /// When there are too many items to fit in the list then the items will be removed.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void removeScrollbar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the maximum items that the combo box can contain.
        ///
        /// \param maximumItems  The maximum items inside the combo box.
        ///                      When the maximum is set to 0 then the limit will be disabled.
        ///
        /// If no scrollbar was loaded then there is always a limitation because there will be a limited space for the items.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setMaximumItems(unsigned int maximumItems = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the maximum items that the combo box can contain.
        ///
        /// \return The maximum items inside the list.
        ///         If the function returns 0 then there is no limit.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int getMaximumItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Used to communicate with EventManager.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool mouseOnObject(float x, float y);
        virtual void leftMousePressed(float x, float y);
        virtual void leftMouseReleased(float x, float y);
        virtual void mouseMoved(float x, float y);
        virtual void mouseWheelMoved(int delta);
        virtual void mouseNoLongerDown();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function is called when the object is added to a group.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void initialize(tgui::Group *const group);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Because this class is derived from sf::Drawable, you can just call the Draw function from your sf::RenderTarget.
        // This function will be called and it will draw the object on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      public:

        enum ComboBoxCallbacks
        {
            ItemSelected = ObjectCallbacksCount * 1,
            AllComboBoxCallbacks = ObjectCallbacksCount * 2 - 1,
            ComboBoxCallbacksCount = ObjectCallbacksCount * 2
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        // This boolean becomes true when you clicked on the combo box, when the list should be shown
        bool m_ShowList;

        // When the mouse is on top of the object, it might be on the list box part.
        bool m_MouseOnListBox;

        // The number of items to display. If there is a scrollbar then you can scroll to see the other.
        // If there is no scrollbar then this will be the maximum amount of items.
        unsigned int m_NrOfItemsToDisplay;

        // Internally a list box is used to store all items
        ListBox*     m_ListBox;

        // The pathname of the loaded scrollbar
        std::string  m_LoadedScrollbarPathname;

        // The textures for the arrow image
        sf::Texture* m_TextureNormal;
        sf::Texture* m_TextureHover;

        // The sprites for the arrow image
        sf::Sprite   m_SpriteNormal;
        sf::Sprite   m_SpriteHover;

        // The pathname that was used to load the combo box
        std::string  m_LoadedPathname;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_COMBO_BOX_HPP

