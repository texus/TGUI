---
layout: page
title: Selecting a backend in CMake
breadcrumb: selecting backend in cmake
---
The TGUI\_BACKEND setting can be set to several values which are explained below. The SFML backend is selected by default.

### SFML

When the SFML backend is selected, TGUI will use the sfml-graphics module for rendering.

If SFML is not automatically found by CMake then the SFML\_DIR variable needs to be filled in. This property should contain the directory that contains the SFMLConfig.cmake file (just the path without the filename).  
If you downloaded or installed SFML then the file can usually by found in the lib/cmake/SFML folder. If you build SFML yourself without installing it then SFML\_DIR will be the build folder.  
Note that the the file you need is called exactly "SFMLConfig.cmake", the "SFMLConfig.cmake.in" file is not the correct one.

### SDL

A prototype implementation for SDL is also provided with TGUI. It uses SDL2, SDL\_Image, SDL\_TTF and OpenGL (4.3 or higher).

If SDL2 isn't found automatically in CMake then fill in the SDL2\_PATH variable with the root folder of the development libraries that you download from the [SDL download page](https://libsdl.org/download-2.0.php).  
If SDL\_image isn't found automatically then fill in the SDL2\_IMAGE\_PATH variable with the root folder of the development libraries that can be downloaded from the [SDL\_image download page](https://www.libsdl.org/projects/SDL_image/).  
If SDL\_ttf isn't found automatically then fill in the SDL2\_TTF\_PATH variable with the root folder of the development libraries that can be downloaded from the [SDL\_ttf download page](https://www.libsdl.org/projects/SDL_ttf/).  


### Custom

While the other options are about building a single backend, it is also possible to build TGUI without any integrated backends (to use your own custom backend) or to build multiple backends so that you can choose one at runtime.

After you select the "Custom" backend in CMake, you need to press the "Configure" button to see more options.
The following options will become available:

**TGUI\_BACKEND\_SFML**: Sets whether the SFML backend should be build.

**TGUI\_BACKEND\_SDL**: Sets whether the SDL backend should be build.

**TGUI\_BUILD\_WITH\_SFML**: Sets whether TGUI should include conversion functions in util classes (e.g. implicit converters between sf::Color and tgui::Color or sf::String and tgui::String). This is required when using TGUI\_BACKEND\_SFML, but could also be useful if you have a custom backend that still uses SFML utility classes. If you are not using SFML in your project at all then you should leave this option unchecked.
