TGUI 0.10.0  (TBD)
------------------

- Black, BabyBlue and TransparentGrey themes can now be used for all widgets
- Textures can now be loaded from base64 string
- ListView columns can now be resizable
- ListView icons can also be saved in form file
- Added changeMenuItem function to MenuBar to change the text of a menu
- Added SizeHorizontal and SizeVertical mouse cursors
- Word-wrapped lines no longer begin with whitespace
- Filter CR in TextArea to prevent issues when pasting CRLF on Windows


TGUI 0.10-beta (19 March 2022)
------------------------------

- Added SDL\_Renderer backend (SDL + SDL\_ttf)
- Added GLFW/OpenGL backend (glfw3 + OpenGL >= 3.3 or GLES >= 2.0)
- Added SFML/OpenGL backend (sfml-window + OpenGL >= 3.3)
- Existing SFML-Graphics and SDL/OpenGL backends were redesigned
- Added moveWithAnimation and resizeWithAnimation functions to Widget
- Added case-insensitive variants of startsWith and endsWith to String
- ClientSize of ChildWindow can now be a layout instead of only a constant
- Typing in FileDialog now selects the first file starting with the typed letter
- TextSize can now also be set in theme file
- Tool tips are now shown on disabled widgets by default


Older Releases
--------------

See the website for changelogs of older releases: https://tgui.eu/changelog/
