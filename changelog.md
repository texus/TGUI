TGUI 1.1  (4 November 2023)
---------------------------

- Added AutoLayout that lets widget fill entire side of parent
- Any column in ListView can now be auto-sized and expanded
- Added methods for arrow key navigation between widgets
- Added getColumnDesignWidth function to ListView
- Added TextOutlineColor and TextOutlineThickness to ProgressBar renderer
- MiddleRect of Texture can now be changed after loading
- Hover state is now reset when mouse leaves the window


TGUI 1.0  (30 September 2023)
-----------------------------

- Added PanelListBox widget ([PR #193](https://github.com/texus/TGUI/pull/193))
- FileDialog can now create new folders ([PR #192](https://github.com/texus/TGUI/pull/192))
- Added MessageBox::changeButtons to set multiple buttons at once ([PR #215](https://github.com/texus/TGUI/pull/215))
- Added methods to ScrollablePanel to check if scrollbar is currently shown ([PR #213](https://github.com/texus/TGUI/pull/213))
- Pressing the tab key can now insert custom text in TextArea ([PR #211](https://github.com/texus/TGUI/pull/211))
- Widgets in SubwidgetContainer didn't inherit the font of the container ([PR #208](https://github.com/texus/TGUI/pull/208))
- Added onCaretPositionChange, getCaretLine() and getCaretColumn() to TextArea ([PR #207](https://github.com/texus/TGUI/pull/207))
- Added onCaretPositionChange signal to EditBox ([PR #206](https://github.com/texus/TGUI/pull/206))
- Added missing getSignal() functions to TabContainer and SpinControl ([PR #204](https://github.com/texus/TGUI/pull/204))
- BoxLayout::setWidgetIndex didn't immediately update the positions ([PR #203](https://github.com/texus/TGUI/pull/203))
- Added tab alignment and fixed tab size to TabContainer ([PR #174](https://github.com/texus/TGUI/pull/174))
- Added SDL\_Renderer, GLFW/OpenGL and SFML/OpenGL backends
- Added RichTextLabel widget
- Support two finger scrolling on touch screens
- Added font scaling to keep text sharp while view is smaller than window size
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
- Improved scrolling with nested scrollbars
- Added UseWideArrows property to SpinControl
- Added moveWithAnimation and resizeWithAnimation functions to Widget
- Added case-insensitive variants of startsWith and endsWith to String
- Tool tips are now shown on disabled widgets by default
- Word-wrapped lines no longer begin with whitespace
- IME pre-edit window will be positioned next to the text cursor on Windows
- Typing in FileDialog now selects the first file starting with the typed letter
- Added getWindow() function to Gui
- handleEvent now always returns true for scroll events when mouse is on top of a widget
- onFileSelect signal in FileDialog is no longer called on cancel
- Renamed onSelectionChanged to onSelectionChange in TabContainer
- TabContainer now inherits from Container instead of SubwidgetsContainer
- Removed padding from RadioButtonGroup
- Black, BabyBlue and TransparentGrey themes can now be used for all widgets
- ClientSize of ChildWindow can now be a layout instead of only a constant
- TextSize can now also be set in theme file
- Many bug fixes and minor improvements


Older Releases
--------------

See the website for changelogs of older releases: https://tgui.eu/changelog/
