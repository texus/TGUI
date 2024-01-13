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

TEST_CASE("[Filesystem]")
{
    SECTION("Paths")
    {
        REQUIRE(tgui::Filesystem::Path("/a/b/c").asString() == "/a/b/c");
        REQUIRE((tgui::Filesystem::Path("d") / tgui::Filesystem::Path("e") / "f").asString() == "d/e/f");

        tgui::Filesystem::Path path("/x");
        path /= tgui::Filesystem::Path("y");
        path /= "z.txt";
        REQUIRE(path.asString() == "/x/y/z.txt");
        REQUIRE(path.getParentPath().asString() == "/x/y");

        REQUIRE(tgui::Filesystem::Path("/x").getParentPath().asString() == "/");
        REQUIRE(tgui::Filesystem::Path("/").getParentPath().asString() == "/");
        REQUIRE(tgui::Filesystem::Path("x").getParentPath().asString() == "");

        REQUIRE(!tgui::Filesystem::Path("/a/b/c").isEmpty());
        REQUIRE(tgui::Filesystem::Path("").isEmpty());
        REQUIRE(tgui::Filesystem::Path().isEmpty());
    }

    SECTION("Absolute / relative")
    {
#ifdef TGUI_SYSTEM_WINDOWS
        REQUIRE(tgui::Filesystem::Path("C:/a/b/").isAbsolute());
        REQUIRE(!tgui::Filesystem::Path("C:/a/b/").isRelative());

        REQUIRE(tgui::Filesystem::Path("D:/a/b").isAbsolute());
        REQUIRE(!tgui::Filesystem::Path("D:/a/b").isRelative());
#else
        REQUIRE(tgui::Filesystem::Path("/a/b/").isAbsolute());
        REQUIRE(!tgui::Filesystem::Path("/a/b/").isRelative());

        REQUIRE(tgui::Filesystem::Path("/a/b").isAbsolute());
        REQUIRE(!tgui::Filesystem::Path("/a/b").isRelative());
#endif
        REQUIRE(!tgui::Filesystem::Path("a/b/").isAbsolute());
        REQUIRE(tgui::Filesystem::Path("a/b/").isRelative());

        REQUIRE(!tgui::Filesystem::Path("a/b").isAbsolute());
        REQUIRE(tgui::Filesystem::Path("a/b").isRelative());
    }

    SECTION("File/directory exists")
    {
        REQUIRE(!tgui::Filesystem::directoryExists("nonexistent-dir"));
        REQUIRE(!tgui::Filesystem::fileExists("nonexistent-file"));

        REQUIRE(tgui::Filesystem::directoryExists("resources"));
        REQUIRE(tgui::Filesystem::fileExists("resources/image.png"));
    }

    SECTION("Creating directory")
    {
        REQUIRE(tgui::Filesystem::createDirectory("filesystem-test-dir"));
        REQUIRE(tgui::Filesystem::directoryExists("filesystem-test-dir"));
    }
}
