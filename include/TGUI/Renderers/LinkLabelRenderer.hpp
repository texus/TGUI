////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_LINK_LABEL_RENDERER_HPP
#define TGUI_LINK_LABEL_RENDERER_HPP

#include <TGUI/Renderers/LabelRenderer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API LinkLabelRenderer : public LabelRenderer
    {
    public:
        using LabelRenderer::LabelRenderer;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the text when the mouse is on top of the link
        ///
        /// @param color  New text color in the hover state
        ///
        /// If no hover color is set then the TextColor property will be used.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextColorHover(Color color);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the text when the mouse is on top of the link
        ///
        /// @return Text color in the hover state
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] Color getTextColorHover() const;
    };
} // namespace tgui

#endif // TGUI_LINK_LABEL_RENDERER_HPP
