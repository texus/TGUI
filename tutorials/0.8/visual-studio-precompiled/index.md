---
layout: page
title: Visual Studio (precompiled libs)
breadcrumb: visual studio (precompiled libs)
---

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
