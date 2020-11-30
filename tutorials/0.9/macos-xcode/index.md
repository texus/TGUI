---
layout: page
title: Xcode
breadcrumb: xcode
---

Tip: The SFML example comes with a `resourcePath()` function which you have to put before the paths to load resources. You can use `tgui::setResourcePath(resourcePath())` at the beginning of your program so that everything tgui loads starts with that folder.

### Getting TGUI

You can download the source code from the [download page](/download) and follow the ['building with CMake' tutorial](../cmake) for a guide on how to build TGUI.  
After you ran `make` like explained in the tutorial, you should call `sudo make install` to install the libraries which you just built.

Instructions are provided below for using either a dylib or framework library. 

### Using TGUI (dylib)

Step 1: Create a macOS Command Line project in Xcode and select C++ from the “Language” dropdown menu when prompted to “choose options for your new project.”
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_01_001.png)
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_01_002_EXP.png)
  
Step 2: Navigate to Targets —> General —> Frameworks and Libraries. Select the “+” icon to add the TGUI and SFML dylibs to the project.
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_02.1.png)
 
Step 2.2: Select “Add Files” from the “Add Other...” dropdown menu. Navigate to the TGUI and SFML dylib locations and include their respective libraries. If you installed using homebrew, this location should be /usr/local/Cellar. If you installed using CMake, this location is the “lib” folder in your build location.
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_02.2.png) 
 
Step 3: Navigate to Targets —> Signing & Capabilities, and select the “Disable Library Validation” option from the “Hardened Runtime” dropdown.
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_03.1.png)

Step 4: Navigate to Targets —> Build Phases. Drag “Embed Libraries” to a location before “Compile Sources.”
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_04.1.png)
 
Step 5: Navigate to Project —> Build Settings —> Search Paths —> Header Search Paths. Double click the blank space next to “Header Search Paths,” select the “+” icon, then enter the paths to the library headers. If you installed with homebrew, this location should be /usr/local/include. Otherwise, the “include” folders will be located in your initial TGUI and SFML download packages.
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/Step_05.png)
 
You should now be able to run TGUI in Xcode.
![](https://github.com/HerbertWest265/TGUI/tree/0.9-dev/doc/xcodemedia/TGUI_HW.png)
 

### Using TGUI (framework)

<p><span class="Red">Frameworks no longer worked when last tested. Unless you have experience with macOS it is recommended to use dylib files for now.</span></p>

All you need to do to use it is drag the TGUI framework into your project. You might also need to copy the framework to /Library/Frameworks.  
[![XCode drag framework](/resources/XCodeDragFramework.jpg){:width="397" height="214"}](/resources/XCodeDragFramework.jpg)

A screen like below will appear where you normally don't need to change anything, just hit "Finish". The "Copy items if needed" can remain unchecked, but it doesn't matter if it is checked.  
[![XCode add framework](/resources/XCodeAddFramework.jpg){:width="732" height="431"}](/resources/XCodeAddFramework.jpg)
