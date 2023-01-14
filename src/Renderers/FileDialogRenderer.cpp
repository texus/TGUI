/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Renderers/FileDialogRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_RENDERER(FileDialogRenderer, ListView, "ListView")
    TGUI_RENDERER_PROPERTY_RENDERER(FileDialogRenderer, EditBox, "EditBox")
    TGUI_RENDERER_PROPERTY_RENDERER(FileDialogRenderer, FilenameLabel, "FilenameLabel")
    TGUI_RENDERER_PROPERTY_RENDERER(FileDialogRenderer, FileTypeComboBox, "FileTypeComboBox")
    TGUI_RENDERER_PROPERTY_RENDERER(FileDialogRenderer, Button, "Button")
    TGUI_RENDERER_PROPERTY_RENDERER_WITH_DEFAULT(FileDialogRenderer, BackButton, "BackButton", nullptr)
    TGUI_RENDERER_PROPERTY_RENDERER_WITH_DEFAULT(FileDialogRenderer, ForwardButton, "ForwardButton", nullptr)
    TGUI_RENDERER_PROPERTY_RENDERER_WITH_DEFAULT(FileDialogRenderer, UpButton, "UpButton", nullptr)
    TGUI_RENDERER_PROPERTY_BOOL(FileDialogRenderer, ArrowsOnNavigationButtonsVisible, "ArrowsOnNavigationButtonsVisible")
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
