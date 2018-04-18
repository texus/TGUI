---
layout: page
title: iOS (experimental)
breadcrumb: ios
---

### Requirements
At least SFML 2.5 is required.

You must have already build SFML for iOS and verified that you can run your sfml app on your device or emulator. Only then should you proceed with installing TGUI.

You will need to use CMake in order to build TGUI. You can download the latest version [here](https://cmake.org/download/).

### CMake
Fill in the location of the source code (the TGUI folder that you downloaded) and a build directory (usually a new directory that you create inside the TGUI folder) and then click the configure button. Select the "XCode" generator, choose the "Specify toolchain file for cross-compiling" option and specify the iOS.toolchain.cmake file that is shipped with TGUI inside the cmake/toolchains folder.  
[![CMake specify generator](/resources/iOS-0.7/CMakeSpecifyGenerator.jpg){:width="671" height="514"}](/resources/iOS-0.7/CMakeSpecifyGenerator.jpg)
[![CMake specify toolchain file](/resources/iOS-0.7/CMakeSpecifyToolchainFile.jpg){:width="671" height="514"}](/resources/iOS-0.7/CMakeSpecifyToolchainFile.jpg)

There are no more screenshots for the next part, so read everything extra careful.
In order to make CMake find SFML and its dependencies, you may have to set the SFML\_ROOT entry to the location of your SFML folder, which contains the extlibs/libs-ios folder and provides a lib folder which directly contains the static sfml libraries (so no Debug/Release subfolder).
CMake seems to look for frameworks first. Even though it might configure fine, check to make sure that the FREETYPE\_LIBRARY and the JPEG\_LIBRARY are set to the .a files from the SFML\_ROOT/extlibs/libs-ios folder. If they contain other values then change these entries manually.
You should now be able to click Configure and Generate without problems.

### Building the library
CMake will have generated an Xcode project. Go into your build folder, open the project and build it.  
[![XCode Build library](/resources/iOS-0.7/XCodeBuild.jpg){:width="334" height="99"}](/resources/iOS-0.7/XCodeBuild.jpg)

If all goes well, your build folder will contain a lib folder with the static tgui library in it (inside the Debug or Release subfolder).

### Using TGUI
Just drag the tgui-s.a (or tgui-s-d.a) file into your project.  
[![XCode drag framework](/resources/iOS-0.7/XCodeDragFramework.jpg){:width="306" height="218"}](/resources/iOS-0.7/XCodeDragFramework.jpg)

A screen like below will appear asking you what to do. Make sure that "Copy items if needed", "Create folder references" and your target are all checked.  
[![XCode add framework](/resources/iOS-0.7/XCodeAddFramework.jpg){:width="732" height="432"}](/resources/iOS-0.7/XCodeAddFramework.jpg)
