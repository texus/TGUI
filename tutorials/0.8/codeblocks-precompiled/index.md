---
layout: page
title: CodeBlocks (precompiled library)
breadcrumb: codeblocks (precompiled library)
---
Open the "Project build options".  
[![CodeBlocks Opening Project Build Options](/resources/CodeBlocks-0.7/OpeningProjectBuildOptions.jpg){:width="404" height="239"}](/resources/CodeBlocks-0.7/OpeningProjectBuildOptions.jpg)

On the left side you can choose whether you want to change debug or release target settings. Or you can just click on the name of your project (here TGUI-Test) to set them for both. TGUI requires C++14, so check the "-std=c++14" option.  
[![CodeBlocks Compiler Flags](/resources/CodeBlocks/CompilerFlags-std=c++14.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/CompilerFlags.jpg)

In the "Search directories" of the "Compiler" add the path to the include directory.  
[![CodeBlocks Compiler Search Directories](/resources/CodeBlocks-0.7/CompilerSearchDirectories.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/CompilerSearchDirectories.jpg)

In the "Search directories" of the "Linker" add the path to the lib directory.  
[![CodeBlocks Linker Search Directories](/resources/CodeBlocks-0.7/LinkerSearchDirectories.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/LinkerSearchDirectories.jpg)

In "Linker settings" under "Other linker options" add '-ltgui' in the 'Release' settings and/or add '-ltgui-d' in the 'Debug' settings. When linking statically, you have to use '-ltgui-s' and '-ltgui-s-d' instead of course. Also note that the order has to be correct: first tgui, then the libraries from sfml.  
[![CodeBlocks Linker Settings](/resources/CodeBlocks-0.7/LinkerOptions.jpg){:width="738" height="536"}](/resources/CodeBlocks-0.7/.jpg)

When using shared libraries, don't forget to copy tgui.dll to your project's folder.
