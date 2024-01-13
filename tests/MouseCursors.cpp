/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "Tests.hpp"

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Loading/ImageLoader.hpp>
#endif

TEST_CASE("[Mouse cursors]")
{
    SECTION("All cursor types")
    {
        std::array<tgui::Cursor::Type, 16> cursors = {{
            tgui::Cursor::Type::Arrow,
            tgui::Cursor::Type::Text,
            tgui::Cursor::Type::Hand,
            tgui::Cursor::Type::SizeLeft,
            tgui::Cursor::Type::SizeRight,
            tgui::Cursor::Type::SizeTop,
            tgui::Cursor::Type::SizeBottom,
            tgui::Cursor::Type::SizeTopLeft,
            tgui::Cursor::Type::SizeBottomRight,
            tgui::Cursor::Type::SizeBottomLeft,
            tgui::Cursor::Type::SizeTopRight,
            tgui::Cursor::Type::SizeHorizontal,
            tgui::Cursor::Type::SizeVertical,
            tgui::Cursor::Type::Crosshair,
            tgui::Cursor::Type::Help,
            tgui::Cursor::Type::NotAllowed
        }};

        bool customCursors = false;
        SECTION("System cursors")
        {
            // In this case, customCursors remains false
        }
        SECTION("Custom cursors")
        {
            customCursors = true;

            unsigned int index = 0;
            for (auto cursor : cursors)
            {
                tgui::Vector2u size;
                auto pixels = tgui::ImageLoader::loadFromFile("resources/Texture" + tgui::String(index % 8 + 1) + ".png", size);

                REQUIRE_NOTHROW(tgui::Cursor::setStyle(cursor, pixels.get(), size, {0,0}));

                ++index;
            }
        }

        // Change the mouse cursor to each cursor type
        for (auto cursor : cursors)
        {
            REQUIRE_NOTHROW(globalGui->setOverrideMouseCursor(cursor));
            REQUIRE_NOTHROW(globalGui->restoreOverrideMouseCursor());
        }

        // If custom images were loaded then restore the system cursors
        if (customCursors)
        {
            for (auto cursor : cursors)
            {
                REQUIRE_NOTHROW(tgui::Cursor::resetStyle(cursor));
            }
        }
    }

    SECTION("Stacking cursors")
    {
        REQUIRE_NOTHROW(globalGui->setOverrideMouseCursor(tgui::Cursor::Type::Text));
        REQUIRE_NOTHROW(globalGui->setOverrideMouseCursor(tgui::Cursor::Type::Hand));

        REQUIRE_NOTHROW(globalGui->restoreOverrideMouseCursor());
        REQUIRE_NOTHROW(globalGui->restoreOverrideMouseCursor());

        // No harm in trying to pop empty stack
        REQUIRE_NOTHROW(globalGui->restoreOverrideMouseCursor());
    }
}
