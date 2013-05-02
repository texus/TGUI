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

#ifndef TGUI_EDIT_BOX_HPP
#define TGUI_EDIT_BOX_HPP

/// \todo  Double clicking should only select one word in EditBox. To select the whole text, you should click 3 times.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API EditBox : public ClickableObject, public ObjectBorders
    {
  public:

        typedef SharedObjectPtr<EditBox> Ptr;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief The text alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct Alignment
        {
            /// \brief The text alignment
            enum Alignments
            {
                /// Put the text on the left side (default)
                Left,

                /// Center the text
                Center,

                /// Put the text on the right side (e.g. for numbers)
                Right
            };
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        EditBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Copy constructor
        ///
        /// \param copy  Instance to copy
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        EditBox(const EditBox& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~EditBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Overload of assignment operator
        ///
        /// \param right  Instance to assign
        ///
        /// \return Reference to itself
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        EditBox& operator= (const EditBox& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the object by calling the copy constructor.
        // This function calls new and if you use this function then you are responsible for calling delete.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual EditBox* clone();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Loads the edit box images and optionally a matching text color.
        ///
        /// \param pathname  The path to the folder that contains the images.
        ///                  The folder must also contain an info.txt file, which will give more information about the edit box.
        ///
        /// \return
        ///        - true on success
        ///        - false when the pathname is empty
        ///        - false when the info.txt file was not found
        ///        - false when the images couldn't be loaded
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool load(const std::string& pathname);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the size of the edit box.
        ///
        /// \param width   The new width of the edit box
        /// \param height  The new height of the edit box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSize(float width, float height);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the pathname that was used to load the edit box.
        ///
        /// \return Pathname used to load the edit box.
        ///        When the edit box has not been loaded yet then this function will return an empty string.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getLoadedPathname() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the text of the editbox.
        ///
        /// \param text  The new text.
        ///
        /// The last characters of the text might be removed in the following situations:
        /// - You have set a character limit and this text contains too much characters.
        /// - You have limited the text width and the text does not fit inside the EditBox.
        ///
        /// \see setMaximumCharacters
        /// \see limitTextWidth
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setText(const sf::String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the text inside the edit box. This text is not affected by the password character.
        ///
        /// \return The text of the edit box.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual sf::String getText() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the character size of the text.
        ///
        /// \param textSize  The new size of the text.
        ///                  If the size is 0 (default) then the text will be scaled to fit in the edit box.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setTextSize(unsigned int textSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the character size of the text.
        ///
        /// \return The text size.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int getTextSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the font of the text.
        ///
        /// When you don't call this function then the global font will be use.
        /// This global font can be changed with the setGlobalFont function from the parent.
        ///
        /// \param font  The new font.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setTextFont(const sf::Font& font);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the font of the text.
        ///
        /// \return  The font that is currently being used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Font* getTextFont() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Sets a password character.
        ///
        /// \param passwordChar  The new password character.
        ///                      If set to 0 then there is no password character.
        ///
        /// When the text width is limited then this function might remove the last characters in the text if they no
        /// longer fit in the EditBox. You can avoid this by setting LimitTextWidth to false (which is the default).
        ///
        /// \see limitTextWidth
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setPasswordChar(char passwordChar = '\0');


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the password character.
        ///
        /// \return  The password character that is currently being used.
        ///          When no password character is used then this function returns 0.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual char getPasswordChar() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Change the character limit.
        ///
        /// \param maxChars  The new character limit. Set it to 0 to disable the limit.
        ///
        /// This character limit is disabled by default.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setMaximumCharacters(unsigned int maxChars = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the character limit.
        ///
        /// \return The character limit.
        ///         The function will return 0 when there is no limit.
        ///
        /// There is no character limit by default.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int getMaximumCharacters() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the border width and border height of the edit box.
        ///
        /// When the text is auto-scaled then it will be drawn within these borders. The borders themselves are invisible.
        /// The borders are also used to define the clipping area.
        /// Note that these borders are scaled together with the image!
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
        /// \brief Changes the colors from the edit box.
        ///
        /// \param textColor                            The color of the text
        /// \param selectedTextColor                    The color of the text when it is selected
        /// \param selectedTextBackgroundColor          The color of the background of the text that is selected
        /// \param selectionPointColor                  The color of the flickering selection point
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void changeColors(const sf::Color& textColor                            = sf::Color(  0,   0,   0),
                                  const sf::Color& selectedTextColor                    = sf::Color(255, 255, 255),
                                  const sf::Color& selectedTextBackgroundColor          = sf::Color( 10, 110, 255),
                                  const sf::Color& selectionPointColor                  = sf::Color(110, 110, 255));


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the text color that will be used inside the edit box.
        ///
        /// \param textColor  The new text color.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setTextColor(const sf::Color& textColor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the text color of the selected text that will be used inside the edit box.
        ///
        /// \param selectedTextColor  The new text color.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectedTextColor(const sf::Color& selectedTextColor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the background color of the selected text that will be used inside the edit box.
        ///
        /// \param selectedTextBackgroundColor  The new background color.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectedTextBackgroundColor(const sf::Color& selectedTextBackgroundColor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the color that will be used inside the edit box for the flickering selection point.
        ///
        /// \param selectionPointColor  The color of the flickering selection point
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectionPointColor(const sf::Color& selectionPointColor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the text color that is currently being used inside the edit box.
        ///
        /// \return The text color that is currently being used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getTextColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the text color of the selected text that is currently being used inside the edit box.
        ///
        /// \return The selected text color that is currently being used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getSelectedTextColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the background color of the selected text that is currently being used inside the edit box.
        ///
        /// \return The background color of the selected text that is currently being used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getSelectedTextBackgroundColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the color that is currently being used inside the edit box for the flickering selection point.
        ///
        /// \return The color of the flickering selection point
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const sf::Color& getSelectionPointColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Should the text width be limited or should you be able to type even if the edit box is full?
        ///
        /// \param limitWidth  Should there be a text width limit or not.
        ///
        /// When set to true, you will no longer be able to add text when the edit box is full.
        /// The default value is false.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void limitTextWidth(bool limitWidth);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Sets the flickering selection point to after a specific character.
        ///
        /// \param charactersBeforeSelectionPoint  The new position.
        ///
        /// Normally you will not need this function.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectionPointPosition(unsigned int charactersBeforeSelectionPoint);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief This will change the width of the selection point.
        ///
        /// \param width  New width of the selection point
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSelectionPointWidth(unsigned int width = 2);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the width of the selection point.
        ///
        /// \return width  Width of the flickering selection point
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int getSelectionPointWidth() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes whether the edit box accepts all characters or only numeric input.
        ///
        /// \param numbersOnly  Should the edit box only accept numbers?
        ///
        /// By default the edit box accepts all text characters.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setNumbersOnly(bool numbersOnly);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Used to communicate with EventManager.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMousePressed(float x, float y);
        virtual void mouseMoved(float x, float y);
        virtual void keyPressed(sf::Keyboard::Key Key);
        virtual void textEntered(sf::Uint32 Key);
        virtual void objectUnfocused();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function will search where the selection point should be. It will not change the selection point.
        // It will return after which character the selection point should be.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int findSelectionPointPosition(float PosX);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Removes the selected characters. This function is called when pressing backspace, delete or a letter while there were
        // some characters selected.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void deleteSelectedCharacters();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function is called when the object is added to a group.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void initialize(tgui::Group *const group);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // When AnimationManager changes the elapsed time then this function is called.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void update();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Because this class is derived from sf::Drawable, you can just call the draw function from your sf::RenderTarget.
        // This function will be called and it will draw the edit box on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      public:

        enum EditBoxCallbacks
        {
            TextChanged = ObjectCallbacksCount * 1,
            ReturnKeyPressed = ObjectCallbacksCount * 2,
            AllEditBoxCallbacks = ObjectCallbacksCount * 4 - 1,
            EditBoxCallbacksCount = ObjectCallbacksCount * 4
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        // Is the selection point visible or not?
        bool          m_SelectionPointVisible;

        // The color of the flickering selection point
        sf::Color     m_SelectionPointColor;

        // The width in pixels of the flickering selection point
        unsigned int  m_SelectionPointWidth;

        // When this boolean is true then you can no longer add text when the EditBox is full.
        // Changing it to false will allow you to scroll the text (default).
        // You can change the boolean with the limitTextWidth(bool) function.
        bool          m_LimitTextWidth;

        // The text inside the edit box
        sf::String    m_DisplayedText;
        sf::String    m_Text;

        // This will store the size of the text ( 0 to auto size )
        unsigned int  m_TextSize;

        // The text alignment
        Alignment::Alignments m_TextAlignment;

        // The selection
        unsigned int  m_SelChars;
        unsigned int  m_SelStart;
        unsigned int  m_SelEnd;

        // The password character
        char          m_PasswordChar;

        // The maximum allowed characters.
        // Zero by default, meaning no limit.
        unsigned int  m_MaxChars;

        // If true then the image is split in three parts
        bool          m_SplitImage;

        // When the text width is not limited, you can scroll the edit box and only a part will be visible.
        unsigned int  m_TextCropPosition;

        // The rectangle behind the selected text will have this color
        sf::Color     m_SelectedTextBgrColor;

        // We need three SFML texts to draw our text, and one more for calculations.
        sf::Text      m_TextBeforeSelection;
        sf::Text      m_TextSelection;
        sf::Text      m_TextAfterSelection;
        sf::Text      m_TextFull;

        // The SFML textures
        sf::Texture*  m_TextureNormal_L;
        sf::Texture*  m_TextureNormal_M;
        sf::Texture*  m_TextureNormal_R;

        sf::Texture*  m_TextureHover_L;
        sf::Texture*  m_TextureHover_M;
        sf::Texture*  m_TextureHover_R;

        sf::Texture*  m_TextureFocused_L;
        sf::Texture*  m_TextureFocused_M;
        sf::Texture*  m_TextureFocused_R;

        // The SFML sprites
        sf::Sprite   m_SpriteNormal_L;
        sf::Sprite   m_SpriteNormal_M;
        sf::Sprite   m_SpriteNormal_R;

        sf::Sprite   m_SpriteHover_L;
        sf::Sprite   m_SpriteHover_M;
        sf::Sprite   m_SpriteHover_R;

        sf::Sprite   m_SpriteFocused_L;
        sf::Sprite   m_SpriteFocused_M;
        sf::Sprite   m_SpriteFocused_R;

        // The pathname used to load the edit box
        std::string m_LoadedPathname;

        // Is there a possibility that the user is going to double click?
        bool m_PossibleDoubleClick;

        bool m_NumbersOnly;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_EDIT_BOX_HPP
