/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TGUI/Filesystem.hpp"
#include "TGUI/Global.hpp"
#include "Tests.hpp"

TEST_CASE("[Global]")
{
    SECTION("clamp")
    {
        REQUIRE(tgui::clamp(0, 1, 3) == 1);
        REQUIRE(tgui::clamp(1, 1, 3) == 1);
        REQUIRE(tgui::clamp(2, 1, 3) == 2);
        REQUIRE(tgui::clamp(3, 1, 3) == 3);
        REQUIRE(tgui::clamp(4, 1, 3) == 3);
        REQUIRE(tgui::clamp(5, 6, 6) == 6);
        REQUIRE(tgui::clamp(6, 6, 6) == 6);
        REQUIRE(tgui::clamp(7, 6, 6) == 6);
    }

    SECTION("Text size")
    {
        REQUIRE(tgui::getGlobalTextSize() == 13);
        tgui::setGlobalTextSize(42);
        REQUIRE(tgui::getGlobalTextSize() == 42);
        // We need to set the global text size back to the default or other tests will fail
        REQUIRE_NOTHROW(tgui::setGlobalTextSize(13));
        REQUIRE(tgui::getGlobalTextSize() == 13);
    }

    SECTION("Double click time")
    {
        REQUIRE(tgui::getDoubleClickTime() == std::chrono::milliseconds(500));
        tgui::setDoubleClickTime(std::chrono::milliseconds(666));
        REQUIRE(tgui::getDoubleClickTime() == std::chrono::milliseconds(666));
        // We need to set the global double click time back to the default or other tests will fail
        REQUIRE_NOTHROW(tgui::setDoubleClickTime(std::chrono::milliseconds(500)));
        REQUIRE(tgui::getDoubleClickTime() == std::chrono::milliseconds(500));
    }

    SECTION("Resource path")
    {
        REQUIRE(tgui::getResourcePath() == tgui::Filesystem::Path(""));
        tgui::setResourcePath(tgui::Filesystem::Path("/Some_arbitrary_path"));
        REQUIRE(tgui::getResourcePath() == tgui::Filesystem::Path("/Some_arbitrary_path/"));
        tgui::setResourcePath(tgui::Filesystem::Path("/Some_other_arbitrary_path/"));
        REQUIRE(tgui::getResourcePath() == tgui::Filesystem::Path("/Some_other_arbitrary_path/"));
        tgui::setResourcePath(tgui::Filesystem::Path("arbitrary_path"));
        REQUIRE(tgui::getResourcePath() == tgui::Filesystem::Path("arbitrary_path/"));
        // We need to set the global resource path back to the default or other tests will fail
        REQUIRE_NOTHROW(tgui::setResourcePath(tgui::Filesystem::Path()));
        REQUIRE(tgui::getResourcePath() == tgui::Filesystem::Path());
    }

    SECTION("Edit cursor blink rate")
    {
        REQUIRE(tgui::getEditCursorBlinkRate() == std::chrono::milliseconds(500));
        tgui::setEditCursorBlinkRate(std::chrono::milliseconds(123));
        REQUIRE(tgui::getEditCursorBlinkRate() == std::chrono::milliseconds(123));
        // We need to set the global edit cursor blink time back to the default or other tests will fail
        REQUIRE_NOTHROW(tgui::setEditCursorBlinkRate(std::chrono::milliseconds(500)));
        REQUIRE(tgui::getEditCursorBlinkRate() == std::chrono::milliseconds(500));
    }
}
