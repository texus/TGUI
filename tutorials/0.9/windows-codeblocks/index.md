---
layout: page
title: CodeBlocks
breadcrumb: codeblocks
---

### Getting TGUI

There are currently no precompiled libraries for MinGW yet.

Download the source code from the [download page](/download) and follow the ['building with CMake' tutorial](../cmake) for a guide on how to compile TGUI yourself.


### Adding TGUI to your project

To add TGUI to your CodeBlocks project, start by opening the project build options.  
[![CodeBlocks Project Build Options](/resources/Tutorials/0.9/CodeBlocksOpeningProjectBuildOptions.png){:width="395" height="244"}](/resources/Tutorials/0.9/CodeBlocksOpeningProjectBuildOptions.png)

In both debug and release mode (at the same time by clicking on the project name at the top left), add the compiler and linker search directories. The compiler directory is the "include" folder from TGUI while the linker directory is the one that contains the .a file (will be the "lib" folder if you downloaded precompiled libraries, or a lib subfolder in the build directory if you build TGUI yourself with CMake and didn't copy the files to somewhere else). The "C:\TGUI_Path\" is to be replaced with the location of the TGUI folder on your computer, you can place the folder anywhere you want.  
[![CodeBlocks Compiler search directory](/resources/Tutorials/0.9/CodeBlocksSearchDirectoriesCompiler.png){:width="738" height="250"}](/resources/Tutorials/0.9/CodeBlocksSearchDirectoriesCompiler.png)
[![CodeBlocks Linker search directory](/resources/Tutorials/0.9/CodeBlocksSearchDirectoriesLinker.png){:width="738" height="250"}](/resources/Tutorials/0.9/CodeBlocksSearchDirectoriesLinker.png)

GCC 6 and newer will automatically use c++14, but if you have an older MinGW version then you will also need to add the "-std=c++14" compiler flag.  
[![CodeBlocks c++14 compiler flag](/resources/Tutorials/0.9/CodeBlocksCompilerFlagsCpp14.png){:width="738" height="380"}](/resources/Tutorials/0.9/CodeBlocksCompilerFlagsCpp14.png)

If you want to use TGUI with a .dll file then follow the instruction in the "Dynamic linking" section below. If you want the TGUI library to be linked into your executable (so that you don't need to include a .dll with your exe) then follow the "Static linking" section.


### Dynamic linking

In the "Release" configuration, add "tgui" to the link libraries.  
[![CodeBlocks Linker settings (Dynamic, Release)](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsDynamicRelease.png){:width="738" height="320"}](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsDynamicRelease.png)

In the "Debug" configuration, add "tgui-d" to the link libraries.  
[![CodeBlocks Linker settings (Dynamic, Debug)](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsDynamicDebug.png){:width="738" height="320"}](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsDynamicDebug.png)

Note that the order in which you list the libraries is important. You must list TGUI above the the libraries on which it depends (SFML in the above screenshots).

Don't forget to copy tgui.dll or tgui-d.dll (depending on whether the executable is build in Debug or Release mode) next to your executable.


### Static linking

When linking statically, you have to add "TGUI_STATIC" to the compiler defines.  
[![CodeBlocks Compiler Defines](/resources/Tutorials/0.9/CodeBlocksCompilerDefinesStatic.png){:width="738" height="250"}](/resources/Tutorials/0.9/CodeBlocksCompilerDefinesStatic.png)

In the "Release" configuration, add "tgui-s" to the link libraries.  
[![CodeBlocks Linker settings (Static, Release)](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsStaticRelease.png){:width="738" height="320"}](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsStaticRelease.png)

In the "Debug" configuration, add "tgui-s-d" to the link libraries.  
[![CodeBlocks Linker settings (Static, Debug)](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsStaticDebug.png){:width="738" height="320"}](/resources/Tutorials/0.9/CodeBlocksLinkerSettingsStaticDebug.png)

Note that the order in which you list the libraries is important. You must list TGUI above the the libraries on which it depends (SFML in the above screenshots).
