---
layout: page
title: iOS
breadcrumb: ios
---

### Requirements

TGUI requires the SFML backend on iOS, other backends are currently unsupported. At least SFML 2.5 is required.

You must have already build SFML for iOS and verified that you can run your sfml app on your device or emulator. Only then should you proceed with installing TGUI.

You will need to use CMake in order to build TGUI. You can download the latest version [here](https://cmake.org/download/).

The TGUI source code can be downloaded from the [download page](/download).

### CMake
Fill in the location of the source code (the TGUI folder that you downloaded) and a build directory (usually a new directory that you create inside the TGUI folder) and then click the Configure button. Select the "Unix Makefiles" generator, choose the "Specify toolchain file for cross-compiling" option and specify the iOS.toolchain.cmake file that is shipped with TGUI inside the cmake/toolchains folder.  
[![CMake specify generator](/resources/Tutorials/0.9/iosCMakeGenerator.png){:width="888" height="535"}](/resources/Tutorials/0.9/iosCMakeGenerator.png)
[![CMake specify toolchain file](/resources/Tutorials/0.9/iosCMakeToolchainFile.png){:width="888" height="535"}](/resources/Tutorials/0.9/iosCMakeToolchainFile.png)

If SFML isn't found then set SFML\_DIR to the folder where you build SFML.  
[![CMake set SFML_DIR](/resources/Tutorials/0.9/iosCMakeSFMLDIR.png){:width="888" height="534"}](/resources/Tutorials/0.9/iosCMakeSFMLDIR.png)

Verify that the FreeType_LIB (or FREETYPE\_LIBRARY) variable is set to the libfreetype.a file from the SFML/extlibs/libs-ios folder (you may have to check "Advanced" in the top right corner to see it). If this is not the case and a macOS library was found then you must manually set the correct path.  
[![CMake set FreeType_LIB](/resources/Tutorials/0.9/iosCMakeFreeType.png){:width="888" height="410"}](/resources/Tutorials/0.9/iosCMakeFreeType.png)

If you want to use the library in the iOS simulator then you must also change the IOS\_PLATFORM value from "OS" to "SIMULATOR".  
[![CMake set IOS_PLATFORM](/resources/Tutorials/0.9/iosCMakePlatformSimulator.png){:width="888" height="534"}](/resources/Tutorials/0.9/iosCMakePlatformSimulator.png)

### Building the library

If you selected "Unix Makefiles" as generator then CMake will have created a Makefile in the build directory. Open the Terminal app, change the directory to the build directory (using the "cd" command) and execute "make" in the terminal. You could also execute "make -j4" with "4" equal to the amount of threads you want to use in order to speed up the compilation. When it is done, you can use "sudo make install" to install the files.

If you chose the SIMULATOR platform and you get an error about the architecture being unsupported then you can try deleting the CMake cache and starting the tutorial again, but this time using the "Xcode" generator instead of "Unix Makefiles".

If you used "Xcode" as generator then you will need to build TGUI by opening the Xcode project which CMake generated in the build directory (you can use the "Open Project" button in CMake for this). There you must make sure the target at the top is set to ALL_BUILD (there seems to be an issue where Xcode selects a random target). Since Xcode ignores the Release/Debug choice in CMake and always builds Debug by default, you will also need to edit the sceme in Xcode if you want a Release library. Afterwards you can build the project.

If all goes well, your build folder will contain a lib folder with the static tgui library in it (inside a Debug or Release subfolder if you used Xcode).

### Using TGUI
Just drag the tgui-s.a (or tgui-s-d.a) file into your project.  
[![XCode drag framework](/resources/iOS/XCodeDragFramework.jpg){:width="306" height="218"}](/resources/iOS/XCodeDragFramework.jpg)

A screen like below will appear asking you what to do. Make sure that "Copy items if needed", "Create folder references" and your target are all checked before pressing Finish.  
[![XCode add framework](/resources/iOS/XCodeAddFramework.jpg){:width="730" height="430"}](/resources/iOS/XCodeAddFramework.jpg)
