---
layout: page
title: Xcode
breadcrumb: xcode
---

Tip: The sfml example comes with a `resourcePath()` function which you have to put before the paths to load resources. You can use `tgui::setResourcePath(resourcePath())` at the beginning of your program so that everything tgui loads starts with that folder.

### CMake

- You will need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).
- On some macs you might have to manually install the Command Line Tools to run CMake. You can install it by running "xcode-select --install" in the terminal.
- Also make sure that you already have sfml on your computer. It would be even better to make sure you have sfml working before trying to use TGUI. If something goes wrong afterwards, you can be sure that the problem does not lie with sfml.

Set the location of the TGUI directory, set the build directory (usually a new empty folder) and press "Configure".  
[![CMake Build Directory](/resources/XCode-0.7/CMakeBuildDirectory.jpg){:width="692" height="412"}](/resources/XCode-0.7/CMakeBuildDirectory.jpg)

Next select your generator, you probably want "Unix Makefiles". Just keep the "Use default native compilers" option checked and ignore the other three options.  
[![CMake Select Generator](/resources/XCode-0.7/CMakeSelectGenerator.jpg){:width="498" height="244"}](/resources/XCode-0.7/CMakeSelectGenerator.jpg)

You can now configure the settings. But watch out for which SFML version was found: if you are planning on using the SFML frameworks in your project, then don't let CMake find the SFML dylibs (or vice versa) because your program will simply not work. Normally you only need to change the TGUI_BUILD_FRAMEWORK option, check it to build frameworks (recommended) or uncheck it to build dylibs.  
[![CMake Configure](/resources/XCode-0.7/CMakeConfigure.jpg){:width="692" height="608"}](/resources/XCode-0.7/CMakeConfigure.jpg)

After setting the options like you want them you need to press Configure again. Now you should have reached the message "Configuring done" at the bottom. All you have to do now is press Generate.  
[![CMake Generate](/resources/XCode-0.7/CMakeGenerate.jpg){:width="692" height="608"}](/resources/XCode-0.7/CMakeGenerate.jpg)


### Building the library

You are not done yet. CMake created a Makefile in the build directory that will build the library (if you used the Unix Makefiles generator). Open the terminal, go to the directory were the Makefile is and type "make -j2" (or a bigger number than 2 if you have more cores). When this is done, type "sudo make install" and enter your password.


### Using TGUI (framework)

If you chose to build a framework in cmake then the make install command from the previous step installed the framework to /Library/Frameworks. All you need to do to use it is dragging the TGUI framework into your project.  
[![XCode drag framework](/resources/XCodeDragFramework.jpg){:width="397" height="214"}](/resources/XCodeDragFramework.jpg)

A screen like below will appear where you normally don't need to change anything, just hit "Finish". The "Copy items if needed" can remain unchecked, but it doesn't matter if it is checked.  
[![XCode add framework](/resources/XCodeAddFramework.jpg){:width="732" height="431"}](/resources/XCodeAddFramework.jpg)


### Using TGUI (dylib)

If you chose to build a dylib in cmake then you have to add the dylib to your linker settings. First, go to the Build Settings.  
[![XCode Build Settings](/resources/XCodeBuildSettings.jpg){:width="826" height="536"}](/resources/XCodeBuildSettings.jpg)

Under `Linking` in `Other Linker Flags`, add `-ltgui` or `-ltgui-d`.  
If you don’t see the `Linking` section, then you might need to select `All` instead of `Basic`.  
[![XCode Linker Flags](/resources/XCodeLinkerFlags.jpg){:width="826" height="522"}](/resources/XCodeLinkerFlags.jpg)

