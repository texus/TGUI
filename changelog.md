TGUI 1.10  (14 June 2025)
-------------------------

- Added MaxWidth property to RadioButton and CheckBox
- Added ClientPadding property to ChildWindow renderer
- Added option to scale image (stretching while maintaining ratio) before applying 9-slice scaling
- Show effects didn't take origin into account
- Changed scaling when object is smaller than corners of 9-slice
- Scrollbar now keeps scrolling when the mouse button stays pressed on top of an arrow
- Values change faster when holding down a spin button arrow


TGUI 1.9  (4 May 2025)
----------------------

- New backend: SDL_GPU (uses SDL3's new GPU API)
- RichTextLabel can now contain url tags and a new findLinkAtPos function can check if the mouse is on top of any
- Added mapPixelToCoords and mapCoordsToPixel to CanvasSFML
- Added tgui::literals::percent namespace with the "_percent" user-defined literal
- Added support for using raylib backend on Android via raymob
- Fixed behavior of using showWithEffect or hideWithEffect with a Fade type while another fade animation wasn't finished yet


TGUI 1.8  (23 February 2025)
----------------------------

- New widget: ContextMenu
- Added handleWindowEvents function to SFML backend (only for SFML 3)
- Added onRightClick signal to ListBox
- Added onTabRightClick signal to Tabs
- Added setSmooth and isSmooth to CanvasSFML
- Changed the way the view is handled in CanvasSFML
- Pressing escape now closes the menu from ContextMenu or MenuBar


TGUI 1.7  (22 December 2024)
----------------------------

- New widgets: GrowHorizontalLayout and GrowVerticalLayout
- Fixed some issues with font scale in SFML font backend


TGUI 1.6.1  (8 October 2024)
----------------------------

- Brightness slider in ColorPicker had a wrong size (regression in TGUI 1.6)
- ColorPickerRenderer was missing property to style edit boxes


TGUI 1.6  (6 October 2024)
--------------------------

- Added CloseBehavior property to ChildWindow
- Added addMultipleItems to ListBox and ComboBox
- Added getItemByIndex, getIndexById and getIdByIndex to ComboBox
- Added setSpinButtonWidth function to SpinControl
- Numpad keys may now move the cursor in text fields when Num Lock is off
- setOrientation in Slider or SpinButton will no longer flip width and height
- setSize in Slider or SpinButton no longer affects orientation once setOrientation is called
- Fixed crash on exit when tool tip was visible
- Fixed wrong arrow sizes for horizontal spin button
- Fixed view not being usable in CanvasSFML
- TreeView didn't visually update when calling setItemIndexInParent or changeItemHierarchy


TGUI 1.5  (25 August 2024)
--------------------------

- Added uniform scrollbar access to all widgets with a scrollbar
- Added getter for hovered item to ListBox, ListView, PanelListBox, Tabs and TreeView
- Added option to buttons to only repond to clicks and ignore space/return key presses
- Added setItemIndexInParent and getItemIndexInParent functions to TreeView
- Added changeItemHierarchy to TreeView
- Scrollbar::setOrientation will no longer flip width and height
- Scrollbar::setSize no longer affects orientation once setOrientation is called
- Grid didn't update its size in remove and setAutoSize functions while auto-sizing
- Opacity of ScrollablePanel wasn't applied to its scrollbars
- Setting opacity of SeparatorLine had no effect
- SFML backend no longer uses sf::Keyboard::isKeyPressed to check modifier keys


TGUI 1.4.1  (20 July 2024)
--------------------------

- Fixed infinite loop in Theme::replace (introduced in TGUI 1.4.0)
- PanelListBox now has proper background color and borders in White theme


TGUI 1.4  (15 July 2024)
------------------------

- New widget: SplitContainer
- Added MaxValue getter to Scrollbar
- Added ScrollbarMaxValue getters to widgets with a scrollbar
- Added getPixelsPerPoint() to BackendRenderTarget
- Inner size of ScrollablePanel now depends on shown scrollbars
- Replaced VerticalScroll with Orientation in Slider, Scrollbar and SpinButton
- Multiple fixes to EditBoxSlider widget


TGUI 1.3  (10 June 2024)
------------------------

- New backend: raylib
- New widget: EditBoxSlider ([PR #238](https://github.com/texus/TGUI/pull/238))
- All widgets can now be configured to ignore mouse events
- Added HorizontalLayout and VerticalLayout to replace widget-specific enums
- Added method to associate user data to combo box items
- Added onWindowFocus and onWindowUnfocus signals
- Renamed isKeptInParent to getKeepInParent in ChildWindow
- Renamed limitTextWidth to setTextWidthLimited in EditBox
- String::fromNumber now supports int8_t (but no longer accepts pointers)
- BackendTextureSFML::getInternalTexture() now returns a pointer
- BackendFontSFML::getInternalFont() now return a pointer


TGUI 1.2  (23 March 2024)
-------------------------

- Added Theme::replace function
- Added TreeView::changeItem function
- Added TreeView::getNode function
- Added ignoreMouseEvents function to canvas widgets
- Added Panel::setEventBubbling to enable more intuitive event propagation
- Replaced getWidgetAtPosition with getWidgetAtPos
- getWidgetBelowMouseCursor was given a parameter for recursive search
- Textures with different part rects were incorrectly considered equal
- showWithEffect didn't show widget if a hide animation was still playing
- Setting opacity of a SubWidgetContainer didn't work
- SubWidgetContainer didn't support show/hide animations


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
