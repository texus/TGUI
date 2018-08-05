---
layout: page
title: Visual Studio
breadcrumb: visual studio
redirect_from: "/tutorials/v07/visual-studio/index.html"
---

### CMake

- You will need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).
- Also make sure that you already have sfml on your computer. It would be even better to make sure you have sfml working before trying to use TGUI. If something goes wrong afterwards, you can be sure that the problem does not lie with sfml.

Set the location of the TGUI directory, set the build directory (usually a new empty folder) and press "Configure".  
[![CMake Build Directory](/resources/VS-0.7/CMakeBuildDirectory.jpg){:width="647" height="377"}](/resources/VS-0.7/CMakeBuildDirectory.jpg)

Next select your generator, which is your Visual Studio version in this guide. Just keep the “Use default native compilers” option checked and ignore the other three options.  
[![CMake Select Generator](/resources/VS-0.7/CMakeSelectGenerator.jpg){:width="319" height="262"}](/resources/VS-0.7/CMakeSelectGenerator.jpg)

If SFML cannot be found automatically you will receive an error.  
[![CMake SFML not found](/resources/VS-0.7/CMakeErrorFindingSFML.jpg){:width="647" height="401"}](/resources/VS-0.7/CMakeErrorFindingSFML.jpg)

If SFML was not found then you should set `SFML_DIR` to the folder containing SFMLConfig.cmake (SFML >= 2.5) or alternatively set the `SFML_ROOT` variable to the root SFML folder (the directory that contains the include and lib folders). When you use `SFML_ROOT` when you compiled sfml yourself then you will have to watch out for three things:

- You probably need to place the lib folder there yourself, with the libraries that you have build.
- This folder must contain the libraries directly (not inside a Debug or Release folder).
- If you choose to build static libraries below then the folder must of course contain the static sfml libraries.
    
You can now configure the settings. Set the `CMAKE_BUILD_TYPE` option either to "Debug" or to "Release" depending on the type of library you want. You should also look at the `TGUI_SHARED_LIBS` option, check it to build dynamic libraries (.dll files), uncheck it to build static libraries. This option has to match with the one chosen for SFML.
[![CMake Configure](/resources/VS-0.7/CMakeConfigure.jpg){:width="647" height="486"}](/resources/VS-0.7/CMakeConfigure.jpg)

After setting the options like you want them you need to press Configure again. Now you should have reached the message "Configuring done" at the bottom. All you have to do now is press Generate.
[![CMake Generate](/resources/VS-0.7/CMakeGenerate.jpg){:width="647" height="515"}](/resources/VS-0.7/CMakeGenerate.jpg)


### Building the library
You are not done yet. CMake created a Visual Studio project in the build directory that has to be build first.

Open the tgui.sln file. You still have to manually change the Solution Configuration to match the `CMAKE_BUILD_TYPE` option from CMake.  
[![Visual Studio Select Solution Configurations](/resources/VS-0.7/SelectSolutionConfigurations.jpg){:width="460" height="183"}](/resources/VS-0.7/SelectSolutionConfigurations.jpg)

Now hit "Build Solution" to build the TGUI libraries.  
[![Build Solution](/resources/VS-0.7/BuildSolution.jpg){:width="502" height="143"}](/resources/VS-0.7/BuildSolution.jpg)

Once this is done, you will find the libraries inside the lib subdirectory of the build folder. To make it easier to use tgui in your project, you should probably copy the .lib (and .dll) files to a new "lib" folder in the TGUI root directory.


### Using TGUI

Open the Project Properties.  
[![Visual Studio Project Properties](/resources/VS2010ProjectProperties.png){:width="448" height="378"}](/resources/VS2010ProjectProperties.png)

In both debug and release mode, add the include and library directories.  
[![Visual Studio Additional Include Directories](/resources/VS2010AdditionalIncludeDirectories.png){:width="853" height="605"}](/resources/VS2010AdditionalIncludeDirectories.png)
[![Visual Studio Additional Library Directories](/resources/VS2010AdditionalLibraryDirectories.png){:width="851" height="624"}](/resources/VS2010AdditionalLibraryDirectories.png)

In both debug and release mode (this time seperately), you must add the library to link with. When you are only going to use one mode then you obviously don't have to change the other one.  
When linking statically you will need to link to tgui-s.lib and tgui-s-d.lib instead of tgui.lib and tgui-d.lib. The order of linking is also important: first sfml, then tgui.  
[![Visual Studio Additional Dependencies (Release)](/resources/VS2010AdditionalDependenciesRelease.png){:width="852" height="607"}](/resources/VS2010AdditionalDependenciesRelease.png)
[![Visual Studio Additional Dependencies (Debug)](/resources/VS2010AdditionalDependenciesDebug.png){:width="852" height="603"}](/resources/VS2010AdditionalDependenciesDebug.png)

When linking dynamically, don’t forget to copy the needed dlls to the directory where your compiled executable is.

You should now be able to use TGUI.

