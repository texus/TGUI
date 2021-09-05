TGUI - Texus' Graphical User Interface
=======================================

TGUI is a cross-platform modern c++ GUI library.  
Although originally a library for SFML, it now also has backends for SDL and GLFW (using OpenGL for rendering).

For more information, take a look at the [website](https://tgui.eu).


Status
------

[![CI Github Actions](https://github.com/texus/TGUI/workflows/CI/badge.svg?branch=0.9)](https://github.com/texus/TGUI/actions)
[![Code coverage](https://codecov.io/gh/texus/TGUI/branch/0.9/graph/badge.svg)](https://codecov.io/gh/texus/TGUI/branch/0.9)

OS support for each backend:  
|                     | Windows             | Linux               | macOS               | Android             | iOS                 |
|---------------------|---------------------|---------------------|---------------------|---------------------|---------------------|
| SFML\_GRAPHICS      | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark: :question: |
| SFML\_OPENGL3       | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark:  | N/A                 | N/A                 |
| SDL\_OPENGL3        | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark:  | N/A                 | N/A                 |
| SDL\_GLES2          |                     | :heavy_check_mark:  | N/A                 |                     |                     |
| SDL\_TTF\_OPENGL3   | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark:  | N/A                 | N/A                 |
| SDL\_TTF\_GLES2     |                     | :heavy_check_mark:  | N/A                 | :heavy_check_mark:  |                     |
| GLFW\_OPENGL3       | :heavy_check_mark:  | :heavy_check_mark:  | :heavy_check_mark:  | N/A                 | N/A                 |
| GLFW\_GLES2         |                     | :heavy_check_mark:  | N/A                 | N/A                 | N/A                 |


Dependencies for each backend:  
|                     | Window backend        | Font backend            | Rendering backend       |
|---------------------|-----------------------|-------------------------|-------------------------|
| SFML\_GRAPHICS      | sfml-window  (>= 2.5) | sfml-graphics  (>= 2.5) | sfml-graphics  (>= 2.5) |
| SFML\_OPENGL3       | sfml-window  (>= 2.5) | FreeType  (>= 2.6)      | OpenGL  (>= 3.3)        |
| SDL\_OPENGL3        | SDL2  (>= 2.0.6)      | FreeType  (>= 2.6)      | OpenGL  (>= 3.3)        |
| SDL\_GLES2          | SDL2  (>= 2.0.6)      | FreeType  (>= 2.6)      | OpenGL ES  (>= 2.0)     |
| SDL\_TTF\_OPENGL3   | SDL2  (>= 2.0.6)      | SDL2_ttf  (>= 2.0.14)   | OpenGL  (>= 3.3)        |
| SDL\_TTF\_GLES2     | SDL2  (>= 2.0.6)      | SDL2_ttf  (>= 2.0.14)   | OpenGL ES  (>= 2.0)     |
| GLFW\_OPENGL3       | GLFW  (>= 3.2)        | FreeType  (>= 2.6)      | OpenGL  (>= 3.3)        |
| GLFW\_GLES2         | GLFW  (>= 3.2)        | FreeType  (>= 2.6)      | OpenGL ES  (>= 2.0)     |



Download
--------

You can download the source code of the current development version from the [git repository](https://github.com/texus/TGUI/tree/0.9): [Download ZIP](https://github.com/texus/TGUI/archive/0.9.zip)

Precompiled libraries for some compilers are available for each [release](https://github.com/texus/TGUI/releases).


Learn
-----

* [Tutorials](https://tgui.eu/tutorials/0.9)
* [Documentation](https://tgui.eu/documentation/0.9)
* [Examples](https://tgui.eu/examples/0.9)
* [Forum](https://forum.tgui.eu)


Contact
-------

Name: Bruno Van de Velde

E-mail: vdv_b@tgui.eu

[![Discord](https://img.shields.io/badge/chat-on_discord-7389D8.svg?logo=discord&logoColor=ffffff&labelColor=6A7EC2)](https://discord.gg/Msf4vyx)

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/texusius)

[![Patreon](https://tgui.eu/resources/PatreonThin.png)](https://www.patreon.com/tgui)
