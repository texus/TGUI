---
layout: page
title: Building TGUI from source code (with CMake)
breadcrumb: building with cmake
---

### Configuring CMake

You need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).

The source directory in CMake should be the TGUI directory that you downloaded. The build directory for the binaries is a new folder that you can choose in which CMake will output its files. You could just call it "build" or you could provide a unique name in case you want to build different configurations and place each in its own build folder. Once you filled in the two paths, you can click the "Configure" button. CMake will propose to create the folder for you when it didn't exist yet.  
[![CMake Build Directory](/resources/Tutorials/0.9/CMakeSrcAndBuildDirectories.png){:width="781" height="386"}](/resources/Tutorials/0.9/CMakeSrcAndBuildDirectories.png)

You will then be asked to select a generator. Which one you should select depends on your OS and compiler.
On Linux and macOS, you can select "Unix Makefiles". On Windows, you can select "Visual Studio xx yyyy" for Visual Studio and "CodeBlocks - MinGW Makefiles" for CodeBlocks. Normal "MinGW Makefiles" can be selected when using a MinGW compiler with a different IDE.
You can change the platform to "Win32" or "x64" if you want to change the default. The "Use default native compilers" should remain checked and the optional toolset edit box can remain empty.
Press "Finish" in this child window to continue.  
[![CMake Select Generator](/resources/Tutorials/0.9/CMakeGenerator.png){:width="781" height="568"}](/resources/Tutorials/0.9/CMakeGenerator.png)

At this point, CMake may complain that it can't find SFML. If this is the case then you must set SFML_DIR to tell CMake where to find it or change the TGUI\_BACKEND property if you do not wish to use SFML. Both of these things are explained in more detail in the ['selecting the backend' tutorial](../backends). This guide will continue with the assumption that SFML has been found.

CMake displays all new properties in red to indicate which properties already existed before pressing Configure and which are new. You do not have to worry about all properties being displayed in red or if later only some are red.  
[![CMake Select Generator](/resources/Tutorials/0.9/CMakeConfigureValues.png){:width="781" height="568"}](/resources/Tutorials/0.9/CMakeConfigureValues.png)

Now it is time to change some settings for TGUI. Below is a list of the most important settings that you can change with an explanation of what they do.

**CMAKE\_BUILD\_TYPE**:  
Sets whether you want to build "Release" or "Debug" libraries. On Windows you need to use Debug libraries when your project is in Debug mode and Release libraries when your project is in Release mode. On other operating systems you only need the Debug libraries if you actually want to be able to debug inside the TGUI libraries.

**TGUI\_BACKEND**:  
Sets the backend to use for rendering and OS interaction. More information on this can be found in the ['selecting the backend' tutorial](../backends).

**TGUI\_SHARED\_LIBS**:  
Sets whether you want to build shared or static libraries. Static libraries are included inside your executable while dynamic libraries will require you to include the library as a separate file when distributing your executable (the .dll file on Windows). Static TGUI libraries will have "-s" at the end of their name.  
Although it is possible to link SFML and TGUI in separate ways (by specifying a value for SFML\_STATIC\_LIBRARIES), you would typically link either everything dynamically or everything statically.

**TGUI\_CXX\_STANDARD**:  
Sets which c++ standard should be used by TGUI. This option is set to "14" by default for maximum compatibility (TGUI code requires at least c++14, you can't specify lower values). When set to "17", TGUI code will make use of slightly more efficient code (e.g. using std::variant and std::optional instead of emulating them with c++14 code). When set to "20", the tgui::String class will e.g. get conversion functions from and to std::u8string.  
No gui functionality is lost be keeping this property at "14", all widgets have the same features at any c++ version.  
Your own project has to use a c++ version that is equal or higher to the specified version. You can build a c++14 TGUI library and use it in a c++20 project, but you can't use a c++20 TGUI library if you only compile your own project with c++14.

**TGUI\_BUILD\_GUI\_BUILDER**:  
Sets whether the Gui Builder application should be build as well. The Gui Builder provides a way to visually design your forms.

**TGUI\_USE\_STATIC\_STD\_LIBS** (Windows only):  
Sets whether TGUI should link to the dynamic or static version of the std library. Note that this has no relation to TGUI\_SHARED\_LIBS and that you do NOT have to check this when you want static TGUI libraries.
This option adds "-static-libstdc++" to the linker for MinGW or uses "/MT" instead of "/MD" in Visual Studio. Those are settings that have to match with your project and other dependencies of your application, so if you aren't touching these settings on your own project then you must keep this option unchecked.

**TGUI\_GENERATE\_PDB** (Visual Studio only):  
Sets whether .pdb files should be generated with the debug library. PDB files provide the debug information for the TGUI libraries, they are needed to debug TGUI or to provide a usable call stack if a crash happens inside TGUI. You will usually not need these files, but Visual Studio will show warnings by default when they are missing.

**TGUI\_BUILD\_FRAMEWORK** (macOS only):  
Sets whether you want to build a .framework or a .dylib library.

Once you are done changing settings, press the "Configure" button to apply the settings and press "Generate" to create the project that will build the TGUI libraries.  
[![CMake Select Generator](/resources/Tutorials/0.9/CMakeGenerate.png){:width="781" height="568"}](/resources/Tutorials/0.9/CMakeGenerate.png)

CMake doesn't actually build libraries, it just generates the required files to compile TGUI with the compiler of your choice. So the last step is to actually build TGUI. The prodecure is explained below, depening on which 

### Visual Studio

You can press the "Open Project" button in CMake to open the Visual Studio project.

Visual Studio ignores the CMAKE\_BUILD\_TYPE setting, so you may still need to change the Solution Configuration to Debug or Release if it doesn't has the right value.  
[![Visual Studio Select Configuration](/resources/Tutorials/0.9/BuildTGUI-VisualStudio-SelectConfiguration.png){:width="460" height="158"}](/resources/Tutorials/0.9/BuildTGUI-VisualStudio-SelectConfiguration.png)

Now press "Build Solution" in the "Build" menu to build the TGUI libraries.  
[![Visual Studio Build Solution](/resources/Tutorials/0.9/BuildTGUI-VisualStudio.png){:width="642" height="296"}](/resources/Tutorials/0.9/BuildTGUI-VisualStudio.png)

Once this is done, you will find the TGUI library inside the `lib\Debug` or `lib\Release` subdirectory of the build folder.

### CodeBlocks

Open the tgui.cbp file that you find inside the build folder and press "Build" to create the TGUI libraries.  
[![CodeBlocks Build](/resources/Tutorials/0.9/BuildTGUI-CodeBlocks.png){:width="495" height="151"}](/resources/Tutorials/0.9/BuildTGUI-CodeBlocks.png)

Once this is done, you will find the TGUI library inside the `lib` subdirectory of the build folder.

### Makefiles

Open the build folder in a terminal and execute "make". To speed up the build you can also type e.g. "make -j4", which would use 4 threads to compile the library.  
Once building is done, you can use "sudo make install" on unix systems to install the TGUI libraries. This would make it easier to use them.

