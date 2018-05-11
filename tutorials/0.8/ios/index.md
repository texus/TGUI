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
[![CMake specify generator](/resources/iOS/CMakeSpecifyGenerator.jpg){:width="671" height="514"}](/resources/iOS/CMakeSpecifyGenerator.jpg)
[![CMake specify toolchain file](/resources/iOS/CMakeSpecifyToolchainFile.jpg){:width="671" height="514"}](/resources/iOS/CMakeSpecifyToolchainFile.jpg)

If SFML isn't found then set SFML\_ROOT to the folder containing the include, extlibs and lib directories. You may have to create the lib directory yourself and copy the .a files into it. The lib folder should contain the .a files directly, there should NOT be a Debug or Release subfolder.  
[![CMake set SFML_ROOT](/resources/iOS/CMakeSFMLROOT.jpg){:width="654" height="489"}](/resources/iOS/CMakeSFMLROOT.jpg)

Verify that the FREETYPE\_LIBRARY is set to the libfreetype.a file from the SFML\_ROOT/extlibs/libs-ios folder. If this is not the case and a macOS library was found then you must manually set the correct path. If you want to use the library in the iOS simulator then you must also change the IOS\_PLATFORM value to SIMULATOR.  
[![CMake verify FREETYPE_LIBRARY and set IOS_PLATFORM](/resources/iOS/CMakeFreetypePlatformSimulator.jpg){:width="719" height="512"}](/resources/iOS/CMakeFreetypePlatformSimulator.jpg)

### Building the library
CMake will have generated an Xcode project. Go into your build folder, open the project and build it.  
[![XCode Build library](/resources/iOS/XCodeBuild.jpg){:width="334" height="99"}](/resources/iOS/XCodeBuild.jpg)

If all goes well, your build folder will contain a lib folder with the static tgui library in it (inside the Debug or Release subfolder).

### Using TGUI
Just drag the tgui-s.a (or tgui-s-d.a) file into your project.  
[![XCode drag framework](/resources/iOS/XCodeDragFramework.jpg){:width="306" height="218"}](/resources/iOS/XCodeDragFramework.jpg)

A screen like below will appear asking you what to do. Make sure that "Copy items if needed", "Create folder references" and your target are all checked.  
[![XCode add framework](/resources/iOS/XCodeAddFramework.jpg){:width="732" height="432"}](/resources/iOS/XCodeAddFramework.jpg)
