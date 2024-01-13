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

#include <fstream>
#include <cstring>

#include "Tests.hpp"

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Loading/ImageLoader.hpp>
#endif

// Rendering isn't identical on different computers, so we just check that the image looks similar enough.
// By using software rendering, we can reduce the error margin from 1.5% to 0.05% though.
void compareImageFiles(const tgui::String& filename1, const tgui::String& filename2)
{
    tgui::Vector2u imageSize1;
    tgui::Vector2u imageSize2;
    auto imagePixels1 = tgui::ImageLoader::loadFromFile(filename1, imageSize1);
    auto imagePixels2 = tgui::ImageLoader::loadFromFile(filename2, imageSize2);

    if (!imagePixels1 || !imagePixels2 || (imageSize1 != imageSize2))
    {
        REQUIRE(imagePixels1);
        REQUIRE(imagePixels2);
        REQUIRE(imageSize1 == imageSize2);
    }

    double totalDiff = 0;
    for (unsigned int y = 0; y < imageSize1.y; ++y)
    {
        for (unsigned int x = 0; x < imageSize1.x; ++x)
        {
            unsigned int index = ((y * x) + x) * 4;
            totalDiff += std::abs(int(imagePixels1[index+0]) - int(imagePixels2[index+0])) / 255.0;
            totalDiff += std::abs(int(imagePixels1[index+1]) - int(imagePixels2[index+1])) / 255.0;
            totalDiff += std::abs(int(imagePixels1[index+2]) - int(imagePixels2[index+2])) / 255.0;
        }
    }

    const double diffPercentage = (totalDiff * 100)  / (imageSize1.x * imageSize1.y * 3);
    INFO("Filename: " + filename1.toStdString());

    REQUIRE(diffPercentage < 0.06);
}

// The compareFiles can't be used to compare empty files because it uses readFileToMemory which
// requires the file to have some contents.
bool compareFiles(const tgui::String& filename1, const tgui::String& filename2)
{
    std::size_t fileSize1;
    std::size_t fileSize2;
    auto fileContents1 = tgui::readFileToMemory(filename1, fileSize1);
    auto fileContents2 = tgui::readFileToMemory(filename2, fileSize2);
    if (!fileContents1 || !fileContents2)
        return false;

    if (fileSize1 != fileSize2)
        return false;

    for (std::size_t i = 0; i < fileSize1; ++i)
    {
        if (fileContents1[i] != fileContents2[i])
            return false;
    }

    return true;
}

// Tests the file comparison function
TEST_CASE("compareFiles")
{
    tgui::writeFile("file1.txt", "");
    tgui::writeFile("file2.txt", "");

    // Compare two equal files
    tgui::writeFile("file1.txt", "xxx\nyyy");
    tgui::writeFile("file2.txt", "xxx\nyyy");
    REQUIRE(compareFiles("file1.txt", "file2.txt"));
    REQUIRE(compareFiles("file2.txt", "file1.txt"));

    // Compare 2 non-empty files which are off by a character in the middle
    tgui::writeFile("file2.txt", "xxx\nxyy");
    REQUIRE(!compareFiles("file1.txt", "file2.txt"));
    REQUIRE(!compareFiles("file2.txt", "file1.txt"));

    // Compare 2 non-empty files where one is one character shorter than the other
    tgui::writeFile("file2.txt", "xxx\nyy");
    REQUIRE(!compareFiles("file1.txt", "file2.txt"));
    REQUIRE(!compareFiles("file2.txt", "file1.txt"));

    // Compare existig against a nonexistent file
    REQUIRE(!compareFiles("file1.txt", "nonexistent.txt"));
    REQUIRE(!compareFiles("nonexistent.txt", "file1.txt"));
}
