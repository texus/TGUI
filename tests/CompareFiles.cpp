/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <fstream>
#include <cstring>

// Rendering isn't identical on different computers, so we just check that the image looks similar enough.
// The worst encountered difference was 1.43% on Label_Simple.png, so errors less than 1.5% of the entire
// image are allowed. Since 100% would mean going from a completely black to completely white image, the
// percentage is high enough to make minor errors go unnoticed, but it is better than not checking at all.
void compareImageFiles(const std::string& filename1, const std::string& filename2)
{
    sf::Image image1;
    if (!image1.loadFromFile(filename1))
        REQUIRE(image1.loadFromFile(filename1));

    sf::Image image2;
    if (!image2.loadFromFile(filename2))
        REQUIRE(image2.loadFromFile(filename2));

    if (image1.getSize() != image2.getSize())
        REQUIRE(image1.getSize() == image2.getSize());

    double totalDiff = 0;
    const sf::Uint8* pixels1 = image1.getPixelsPtr();
    const sf::Uint8* pixels2 = image2.getPixelsPtr();
    for (unsigned int y = 0; y < image1.getSize().y; ++y)
    {
        for (unsigned int x = 0; x < image1.getSize().x; ++x)
        {
            unsigned int index = ((y * x) + x) * 4;
            totalDiff += std::abs(pixels1[index+0] - pixels2[index+0]) / 255.0;
            totalDiff += std::abs(pixels1[index+1] - pixels2[index+1]) / 255.0;
            totalDiff += std::abs(pixels1[index+2] - pixels2[index+2]) / 255.0;
        }
    }

    double diffPercentage = (totalDiff * 100)  / (image1.getSize().x * image1.getSize().y * 3);
    REQUIRE(diffPercentage < 1.5);
}

bool compareFiles(const std::string& leftFileName, const std::string& rightFileName)
{
    std::ifstream leftFile;
    std::ifstream rightFile;
    char leftRead = 0;
    char rightRead = 0;
    bool result;

    // Open the two files.
    leftFile.open(leftFileName.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!leftFile.is_open())
        return false;

    rightFile.open(rightFileName.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!rightFile.is_open())
    {
        leftFile.close();
        return false;
    }

    result = true; // Files exist and are open, assuming equality unless a counterexamples shows up
    while (result && leftFile.good() && rightFile.good())
    {
        leftFile.get(leftRead);
        rightFile.get(rightRead);
        result = (leftRead == rightRead);
    }

    if (result)
    {
        // Last read was still equal, are we at the end of both files?
        result = (!leftFile.good()) && (!rightFile.good());
    }

    leftFile.close();
    rightFile.close();
    return result;
}

// Tests the file comparison function
TEST_CASE("compareFiles")
{
    std::ofstream myfile;
    myfile.open("file1.txt");
    myfile.close();
    myfile.open("file2.txt");
    myfile.close();

    // Compare 2 empty files
    REQUIRE(compareFiles("file1.txt", "file2.txt"));
    REQUIRE(compareFiles("file2.txt", "file1.txt"));

    // Compare an empty and a non-empty files
    myfile.open("file1.txt");
    myfile << "xxx" << std::endl << "yyy";
    myfile.close();
    REQUIRE(!compareFiles("file1.txt", "file2.txt"));
    REQUIRE(!compareFiles("file2.txt", "file1.txt"));

    // Compare two equal files
    myfile.open("file2.txt");
    myfile << "xxx" << std::endl << "yyy";
    myfile.close();
    REQUIRE(compareFiles("file1.txt", "file2.txt"));
    REQUIRE(compareFiles("file2.txt", "file1.txt"));

    // Compare 2 non-empty files which are off by a character in the middle
    myfile.open("file2.txt");
    myfile << "xxx" << std::endl << "xyy";
    myfile.close();
    REQUIRE(!compareFiles("file1.txt", "file2.txt"));
    REQUIRE(!compareFiles("file2.txt", "file1.txt"));

    // Compare 2 non-empty files where one is one character shorter than the other
    myfile.open("file2.txt");
    myfile << "xxx" << std::endl << "yy";
    myfile.close();
    REQUIRE(!compareFiles("file1.txt", "file2.txt"));
    REQUIRE(!compareFiles("file2.txt", "file1.txt"));

    // Compare existig against non existing file
    REQUIRE(!compareFiles("file1.txt", "nonexisting.txt"));
    REQUIRE(!compareFiles("nonexisting.txt", "file1.txt"));
}
