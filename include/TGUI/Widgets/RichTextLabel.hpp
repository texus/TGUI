/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_RICH_TEXT_LABEL_HPP
#define TGUI_RICH_TEXT_LABEL_HPP

#include <TGUI/Widgets/Label.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief RichTextLabel widget
    ///
    /// Example usage:
    /// @code
    /// label->setText("The RichTextLabel widget supports formatting text with <b>bold</b>, <i>italics</i>, <u>underlined</u> "
    ///                "and even <s>strikethrough</s>. Each letter can have a separate <size=15>size</size> or <color=blue>color</color>. "
    ///                "This allows for some <b><color=#ff0000>C</color><color=#ffbf00>O</color><color=#80ff00>L</color>"
    ///                "<color=#00ff40>O</color><color=#00ffff>R</color><color=#0040ff>F</color><color=#7f00ff>U</color>"
    ///                "<color=#ff00bf>L</color></b> text. You can even include icons such as <img=Folder.png> and images:\n"
    ///                "<img=\"image.png\">\n\nLines that are too long will wrap around and a vertical scrollbar can be included "
    ///                "when there are <url=abc>too many lines</url>!");
    /// @endcode
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API RichTextLabel : public Label
    {
    public:

        using Ptr = std::shared_ptr<RichTextLabel>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const RichTextLabel>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "RichTextLabel"; //!< Type name of the widget

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        RichTextLabel(const char* typeName = StaticWidgetType, bool initRenderer = true);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new label widget
        ///
        /// @param text  The text to display
        ///
        /// @return The new label
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static RichTextLabel::Ptr create(const String& text = "");

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another label
        ///
        /// @param label  The other label
        ///
        /// @return The new label
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static RichTextLabel::Ptr copy(const RichTextLabel::ConstPtr& label);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the link at a given position if there is one
        ///
        /// @param pos  Position inside the label (i.e. relative to the position of the label)
        ///
        /// @returns Url value if the text at the given position is inside a <url> tag, an empty string otherwise
        ///
        /// Suppose the text in the label is set to "<url>red</url> or <url=blue>green</url>".
        /// If the position is on top of the word "red" then this function returns "red".
        /// If the position is on top of the word "green" then this function returns "blue".
        /// If the position is not on top of "red" and not on top of "green" then this function returns an empty string.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD String findLinkAtPos(Vector2f pos) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draw the widget to a render target
        ///
        /// @param target Render target to draw to
        /// @param states Current render states
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(BackendRenderTarget& target, RenderStates states) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Helper function used by rearrangeText() to create the lines before word-wrapping is applied
        ///
        /// @param textPiecesLines  Text pieces per line to be filled (a new piece is needed each time the text style changes)
        /// @param images           Image list to be filled (if the text contains image references)
        ///
        /// This function parses the contents of m_string and fills the textPiecesLines and images lists (which are empty when
        /// the function is called). It can be overridden in a derived class to change how the parsing works (e.g. to use BBCode
        /// instead of html tags).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void constructRichLineBlueprints(std::vector<std::vector<Text::Blueprint>>& textPiecesLines, std::vector<Texture>& images) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Name of the property that was changed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const String& property) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Rearrange the text (recreates m_textPieces), making use of the given size of maximum text width.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rearrangeText() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helper function used by rearrangeText() to calculate the height of a single line
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float calculateLineSpacing(const std::vector<Text::Blueprint>& line, float defaultLineSpacing);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helper function used by rearrangeText() to calculate the total height of all lines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float calculateTextHeight(const std::vector<std::vector<Text::Blueprint>>& textPiecesLines, float defaultLineSpacing);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Widget::Ptr clone() const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::vector<Sprite> m_images;
        std::map<std::pair<std::size_t, std::size_t>, String> m_links; // Key = indices for text piece in m_lines
        std::map<std::size_t, String> m_imageLinks; // Key = index in m_images

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_RICH_TEXT_LABEL_HPP
