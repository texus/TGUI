---
layout: page
title: Linux / FreeBSD
breadcrumb: linux
---

### Installing
Download the source code from the [download page](/download).

You will need to use CMake in order to build TGUI. How you do this depends on your distro, but it will be similar to “sudo apt-get install cmake” (Ubuntu) or “sudo pacman -S cmake” (ArchLinux).

Also make sure that you already have sfml on your computer. It would be even better to make sure you have sfml working before trying to use TGUI. If something goes wrong afterwards, you can be sure that the problem does not lie with sfml.

Type the following in the terminal in order to install tgui. Of course you will have to be inside the downloaded tgui directory. You can change the 2 in -j2 to a higher number if you have more cores.
```bash
cmake .
make -j2
sudo make install
```


### Using TGUI (Terminal)

The two things to add is linking to tgui and tell the compiler that it uses c++14.
```bash
g++ -std=c++14 main.cpp -ltgui -lsfml-graphics -lsfml-window -lsfml-system -o program
```


### Using TGUI (CodeBlocks)

Open the “Project build options” and choose whether you want to change debug or release target settings. Or you can just click on the name of your project (here TGUI_Test) and set them for both.  
[![CodeBlocks Project Build Options](/resources/LinuxCodeBlocksProjectBuildOptions.jpg){:width="641" height="355"}](/resources/LinuxCodeBlocksProjectBuildOptions.jpg)

Add ‘-ltgui’ to the “Linker settings” under “Other linker options”. The order should be as follows: first tgui, then the libraries from sfml.  
[![CodeBlocks Linker Settings](/resources/LinuxCodeBlocksProjectBuildOptionsLinkerSettings.jpg){:width="736" height="324"}](/resources/LinuxCodeBlocksProjectBuildOptionsLinkerSettings.jpg)

You must also enable c++14.  
[![CodeBlocks Compiler Flags](/resources/LinuxCodeBlocksProjectCompilerFlags.png){:width="839" height="741"}](/resources/LinuxCodeBlocksProjectCompilerFlags.png)

You should now be able to use TGUI.


### Possible error
It is possible to get the following error when running your program:  
`/pathname/of/program: error while loading shared libraries: libtgui.so: cannot open shared object file: no such file or directory`

If you get this then you did install tgui correctly, but it is not in the shared library cache.  
All you have to do is open the terminal, type `sudo ldconfig`, hit enter and type your password.

If that doesn’t solve it then make sure the library was installed to a place where your distro searches for it.  
The easiest solution is probably to add `/usr/local/lib/` at the end of /etc/ld.so.conf and then run `sudo ldconfig` again.
