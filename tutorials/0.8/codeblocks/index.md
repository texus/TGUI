---
layout: page
title: CodeBlocks
breadcrumb: codeblocks
---

### CMake

- You will need to use CMake in order to build TGUI. You can download the latest version [here](https://cmake.org/download/).
- Also make sure that you already have sfml on your computer. It would be even better to make sure you have sfml working before trying to use TGUI. If something goes wrong afterwards, you can be sure that the problem does not lie with sfml.

Set the location of the TGUI directory, set the build directory (usually a new empty folder) and press "Configure".  
[![CMake Build Directory](/resources/CodeBlocks-0.7/CMakeBuildDirectory.jpg){:width="647" height="349"}](/resources/CodeBlocks-0.7/CMakeBuildDirectory.jpg)

Next select your generator, when using CodeBlocks this is "CodeBlocks - MinGW Makefiles". Just keep the "Use default native compilers" option checked and ignore the other three options.  
[![CMake Select Generator](/resources/CodeBlocks-0.7/CMakeSelectGenerator.jpg){:width="310" height="256"}](/resources/CodeBlocks-0.7/CMakeSelectGenerator.jpg)

If SFML cannot be found automatically you will receive an error.  
[![CMake SFML not found](/resources/CodeBlocks-0.7/CMakeErrorFindingSFML.jpg){:width="647" height="414"}](/resources/CodeBlocks-0.7/CMakeErrorFindingSFML.jpg)

If SFML was not found then you should set SFML_DIR to the folder containing SFMLConfig.cmake (SFML >= 2.5) or alternatively set the SFML_ROOT variable to the root SFML folder (the directory that contains the include and lib folders). If you use SFML_ROOT when you compiled sfml yourself then you will have to watch out for two things:

- You probably need to place the lib folder there yourself, with the libraries that you have build.
- If you choose to build static libraries below then the folder must of course contain the static sfml libraries.
    
You can now configure the settings. Set the CMAKE_BUILD_TYPE option either to "Debug" or to "Release" depending on the type of library you want. You should also look at the TGUI_SHARED_LIBS option, check it to build dynamic libraries (.dll files), uncheck it to build static libraries. This option has to match with the one chosen for SFML.  
[![CMake Configure](/resources/CodeBlocks-0.7/CMakeConfigure.jpg){:width="647" height="502"}](/resources/CodeBlocks-0.7/CMakeConfigure.jpg)

After setting the options like you want them you need to press Configure again. Now you should have reached the message "Configuring done" at the bottom. All you have to do now is press Generate.  
[![CMake Generate](/resources/CodeBlocks-0.7/CMakeGenerate.jpg){:width="647" height="515"}](/resources/CodeBlocks-0.7/CMakeGenerate.jpg)


### Building the library
You are not done yet. CMake created a CodeBlocks project in the build directory that has to be build first.

Open the tgui.cbp file and hit "Build" to build the TGUI libraries.  
[![Build](/resources/CodeBlocks-0.7/Build.jpg){:width="448" height="200"}](/resources/CodeBlocks-0.7/Build.jpg)

Once this is done, you will find the libraries inside the lib subdirectory of the build folder. To make it easier to use tgui in your project, you should probably copy the .a (and .dll) files to a new "lib" folder in the TGUI root directory.


### Using TGUI

Open the "Project build options".  
[![CodeBlocks Opening Project Build Options](/resources/CodeBlocks-0.7/OpeningProjectBuildOptions.jpg){:width="404" height="239"}](/resources/CodeBlocks-0.7/OpeningProjectBuildOptions.jpg)

On the left side you can choose whether you want to change debug or release target settings. Or you can just click on the name of your project (here TGUI-Test) to set them for both. TGUI requires C++14, so check the "-std=c++14" option.  
[![CodeBlocks Compiler Flags](/resources/CodeBlocks/CompilerFlags-std=c++14.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/CompilerFlags.jpg)

In the "Search directories" of the "Compiler" add the path to the include directory.  
[![CodeBlocks Compiler Search Directories](/resources/CodeBlocks-0.7/CompilerSearchDirectories.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/CompilerSearchDirectories.jpg)

In the "Search directories" of the "Linker" add the path to the lib directory (or wherever you placed the library that you build with cmake).  
[![CodeBlocks Linker Search Directories](/resources/CodeBlocks-0.7/LinkerSearchDirectories.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/LinkerSearchDirectories.jpg)

In "Linker settings" under "Other linker options" add '-ltgui' in the 'Release' settings and/or add '-ltgui-d' in the 'Debug' settings. When linking statically, you have to use '-ltgui-s' and '-ltgui-s-d' instead of course. Also note that the order has to be correct: first tgui, then the libraries from sfml.  
[![CodeBlocks Linker Settings](/resources/CodeBlocks-0.7/LinkerOptions.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/.jpg)

When using shared libraries, don't forget to copy tgui.dll to your project's folder.

You should now be able to use TGUI.
