---
layout: page
title: Visual Studio
breadcrumb: visual studio
---

### Getting TGUI

There are several precompiled versions available on the [download page](/download). If any of those match your compiler then you can just download that version and can skip building TGUI.

If there is no precompiled version for your compiler, or if you want to change some settings, then you will need to compile TGUI yourself. Download the source code and follow the ['building with CMake' tutorial](../cmake) for a guide on how to do this.


### Adding TGUI to your project

To add TGUI to your Visual Studio project, start by opening the Project Properties.  
[![Visual Studio Project Properties](/resources/Tutorials/0.9/VisualStudioOpeningProjectProperties.png){:width="503" height="488"}](/resources/Tutorials/0.9/VisualStudioOpeningProjectProperties.png)

In both debug and release mode (at the same time by selecting "All Configurations"), add the include and library directories. The library directory is the one that contains the .lib file (will be the "lib" folder if you downloaded precompiled libraries, or a lib subfolder in the build directory if you build TGUI yourself with CMake and didn't copy the files to somewhere else). The "C:\TGUI_Path\" is to be replaced with the location of the TGUI folder on your computer, you can place the folder anywhere you want.  
[![Visual Studio Additional Include Directories](/resources/Tutorials/0.9/VisualStudioAdditionalIncludeDirectories.png){:width="786" height="461"}](/resources/Tutorials/0.9/VisualStudioAdditionalIncludeDirectories.png)
[![Visual Studio Additional Library Directories](/resources/Tutorials/0.9/VisualStudioAdditionalLibraryDirectories.png){:width="786" height="461"}](/resources/Tutorials/0.9/VisualStudioAdditionalLibraryDirectories.png)

If you want to use TGUI with a .dll file then follow the instruction in the "Dynamic linking" section below. If you want the TGUI library to be linked into your executable (so that you don't need to include a .dll with your exe) then follow the "Static linking" section.


### Dynamic linking

In the "Release" configuration, add "tgui.lib" to the additional linker dependencies.  
[![Visual Studio Additional Dependencies (Dynamic, Release)](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesDynamicRelease.png){:width="786" height="544"}](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesDynamicRelease.png)

In the "Debug" configuration, add "tgui-d.lib" to the additional linker dependencies.  
[![Visual Studio Additional Dependencies (Dynamic, Debug)](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesDynamicDebug.png){:width="786" height="544"}](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesDynamicDebug.png)

Note that the order in which you list the libraries is important. You must first list the libraries that TGUI depends on (SFML in the above screenshots) and place the TGUI library behind them.

Don't forget to copy tgui.dll or tgui-d.dll (depending on whether the executable is build in Debug or Release mode) next to your executable.


### Static linking

When linking statically, you have to add "TGUI_STATIC" to the preprocessor definitions.  
[![Visual Studio Preprocessor Definitions](/resources/Tutorials/0.9/VisualStudioPreprocessorDefinitionsStatic.png){:width="786" height="544"}](/resources/Tutorials/0.9/VisualStudioPreprocessorDefinitionsStatic.png)

In the "Release" configuration, add "tgui-s.lib" to the additional linker dependencies.  
[![Visual Studio Additional Dependencies (Static, Release)](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesStaticRelease.png){:width="786" height="544"}](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesStaticRelease.png)

In the "Debug" configuration, add "tgui-s-d.lib" to the additional linker dependencies.  
[![Visual Studio Additional Dependencies (Static, Debug)](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesStaticDebug.png){:width="786" height="544"}](/resources/Tutorials/0.9/VisualStudioAdditionalDependenciesStaticDebug.png)

Note that the order in which you list the libraries is important. You must first list the libraries that TGUI depends on (SFML in the above screenshots) and place the TGUI library behind them.
