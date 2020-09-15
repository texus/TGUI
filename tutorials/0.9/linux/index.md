---
layout: page
title: Linux
breadcrumb: linux
---

### Getting TGUI

You can download the source code from the [download page](/download) and follow the ['building with CMake' tutorial](../cmake) for a guide on how to build TGUI.  
After you ran `make` like explained in the tutorial, you should call `sudo make install` to install the libraries which you just built.


### Adding TGUI to your project if you compile from the terminal

To link to the TGUI libraries you only need to add `-ltgui` to your compiler command.  
For GCC &lt; 6 you will also need to specify `-std=c++14`.
```bash
g++ -std=c++14 main.cpp -ltgui -lsfml-graphics -lsfml-window -lsfml-system -o program
```


### Adding TGUI to your CodeBlocks project

Open the "Project build options" and choose whether you want to change Debug or Release target settings. You can click on the name of your project (here TGUI-Test) and set settings that apply to both Debug and Release.  
[![CodeBlocks Project Build Options](/resources/Tutorials/0.9/LinuxCodeBlocksProjectBuildOptions.png){:width="684" height="414"}](/resources/Tutorials/0.9/LinuxCodeBlocksProjectBuildOptions.png)

Add `tgui` to the "Link libraries" in the "Linker settings" tab.
[![CodeBlocks Linker Settings](/resources/Tutorials/0.9/LinuxCodeBlocksProjectBuildOptionsLinkerSettings.png){:width="736" height="308"}](/resources/Tutorials/0.9/LinuxCodeBlocksProjectBuildOptionsLinkerSettings.png)

If you are still using GCC &lt; 6 then you must also enable c++14 in your project.  
[![CodeBlocks Compiler Flags](/resources/Tutorials/0.9/LinuxCodeBlocksProjectCompilerFlags.png){:width="736" height="405"}](/resources/Tutorials/0.9/LinuxCodeBlocksProjectCompilerFlags.png)


### Potential error

If you get the following error when running your program then you did install TGUI correctly, but it is not in the shared library cache.  
`/pathname/of/program: error while loading shared libraries: libtgui.so: cannot open shared object file: no such file or directory`

Try to open the terminal and execute `sudo ldconfig`.

If that doesn’t solve it then make sure the library was installed to a place where your linux distro searches for it.  
The easiest workaround is probably to add a new line containing `/usr/local/lib/` (and nothing more) at the end of /etc/ld.so.conf and then run `sudo ldconfig` again.
