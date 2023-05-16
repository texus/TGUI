TGUI 1.0  (TBD)
---------------

- Added PanelListBox widget ([PR #193](https://github.com/texus/TGUI/pull/193))
- FileDialog can now create new folders ([PR #192](https://github.com/texus/TGUI/pull/192))
- Improved scrolling with nested scrollbars
- Added UseWideArrows property to SpinControl
- Support two finger scrolling on touch screens
- Added getWindow() function to Gui
- handleEvent now always returns true for scroll events when mouse is on top of a widget


TGUI 1.0-beta  (10 December 2022)
---------------------------------

- Added RichTextLabel widget
- Added font scaling to keep text sharp while view is smaller than window size
- Added tab alignment and fixed tab size to TabContainer ([PR #174](https://github.com/texus/TGUI/pull/174))
- onFileSelect signal in FileDialog is no longer called on cancel
- Removed padding from RadioButtonGroup
- Close button was broken when ChildWindow was copied


TGUI 1.0-alpha  (17 September 2022)
-----------------------------------

- Added SDL\_Renderer backend (SDL + SDL\_ttf)
- Added GLFW/OpenGL backend (glfw3 + OpenGL >= 3.3 or GLES >= 2.0)
- Added SFML/OpenGL backend (sfml-window + OpenGL >= 3.3)
- Existing SFML-Graphics and SDL/OpenGL backends were redesigned
- Black, BabyBlue and TransparentGrey themes can now be used for all widgets
- ClientSize of ChildWindow can now be a layout instead of only a constant
- Typing in FileDialog now selects the first file starting with the typed letter
- TextSize can now also be set in theme file
- Added moveWithAnimation and resizeWithAnimation functions to Widget
- Added case-insensitive variants of startsWith and endsWith to String
- Textures can now be loaded from base64 string
- ListView columns can now be resizable
- ListView icons can also be saved in form file
- Position of text in buttons can now be changed
- Added changeMenuItem function to MenuBar to change the text of a menu
- Added SizeHorizontal and SizeVertical mouse cursors
- Added hasUserData to Widget
- Added LabelAlignment and ButtonAlignment to MessageBox
- Added ScrollbarValue to Label
- Theme files now support global properties
- Theme files now support inheritance between sections
- Tool tips are now shown on disabled widgets by default
- Word-wrapped lines no longer begin with whitespace
- Filter CR in TextArea to prevent issues when pasting CRLF on Windows


Older Releases
--------------

See the website for changelogs of older releases: https://tgui.eu/changelog/
