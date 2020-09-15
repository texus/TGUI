---
layout: page
title: Xcode
breadcrumb: xcode
---

Tip: The SFML example comes with a `resourcePath()` function which you have to put before the paths to load resources. You can use `tgui::setResourcePath(resourcePath())` at the beginning of your program so that everything tgui loads starts with that folder.

### Getting TGUI

You can download the source code from the [download page](/download) and follow the ['building with CMake' tutorial](../cmake) for a guide on how to build TGUI.  
After you ran `make` like explained in the tutorial, you should call `sudo make install` to install the libraries which you just built.

It is recommended to build and use a framework (because they are easier to add to your application), but you can alternatively build the library as a dylib. Instructions are provided below for using either options.

### Using TGUI (framework)

All you need to do to use it is dragging the TGUI framework into your project. You might also need to copy the framework to /Library/Frameworks.  
[![XCode drag framework](/resources/XCodeDragFramework.jpg){:width="397" height="214"}](/resources/XCodeDragFramework.jpg)

A screen like below will appear where you normally don't need to change anything, just hit "Finish". The "Copy items if needed" can remain unchecked, but it doesn't matter if it is checked.  
[![XCode add framework](/resources/XCodeAddFramework.jpg){:width="732" height="431"}](/resources/XCodeAddFramework.jpg)


### Using TGUI (dylib)

If you installed TGUI with brew or you chose to build a dylib in cmake then you have to add the dylib to your linker settings. First, go to the Build Settings.  
[![XCode Build Settings](/resources/XCodeBuildSettings.jpg){:width="826" height="536"}](/resources/XCodeBuildSettings.jpg)

Under `Linking` in `Other Linker Flags`, add `-ltgui`.  
If you don’t see the `Linking` section, then you might need to select `All` instead of `Basic`.  
[![XCode Linker Flags](/resources/XCodeLinkerFlags.jpg){:width="826" height="522"}](/resources/XCodeLinkerFlags.jpg)

