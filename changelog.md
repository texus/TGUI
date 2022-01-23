TGUI 0.9.4  (TBD)
-----------------

- Default theme no longer affects loading widgets from file


TGUI 0.9.3  (15 January 2022)
-----------------------------

- Added RoundedBorderRadius property to Panel renderer
- Touch events didn't work in SDL backend
- Default DistanceToSide in Tabs is now rounded to nearest integer
- Adding widget to container no longer overwrites name if no name is given
- Container::get now also searches inside SubwidgetContainer widgets
- Renamed onAnimationFinish to onShowEffectFinish
- Renamed ShowAnimationType to ShowEffectType
- Loading BMP files didn't work
- Scrollbar in Label didn't inherit opacity
- showWithEffect/hideWithEffect didn't work properly is position was percentage


TGUI 0.9.2  (1 November 2021)
-----------------------------

- Arrow keys can now be used to navigate TreeView
- Control/shift now affect using arrow keys in multi-select ListView
- Added Texture::setDefaultSmooth to choose default interpolation setting
- String::split now takes an optional boolean to trim the returned values
- Kerning calculation now takes bold text style into account (only if SFML >= 2.6)
- Added setWidgetIndex and getWidgetIndex functions to container
- Added String::contains function
- saveWidgetsToFile now makes paths relative to the form path
- Tool tips will no longer be displayed outside the window when close to the side
- Exception is now thrown when loading a font fails
- TabContainer can now create the panels internally
- Button text was lost when copying button
- Down state of button wasn't displayed properly anymore when a focus state existed
- Image in BitmapButton was searched in wrong directory when loading from form file
- Form files didn't load on Windows when path contained non-ANSI characters
- Modification time in FileDialog was empty when MinGW compiler was used
- Clipping in SDL backend was incorrect when view and viewport were changed
- Opacity of color wheel wasn't set when making ColorPicker transparent


TGUI 0.9.1  (12 February 2021)
------------------------------

- Added new SpinControl widget (combination of EditBox and SpinButton) ([PR #135](https://github.com/texus/TGUI/pull/135))
- Added new TabContainer widget (combination of Tabs with Panel below) ([PR #139](https://github.com/texus/TGUI/pull/139))
- Holding down arrow on SpinButton will now keep chaning the value ([PR #137](https://github.com/texus/TGUI/pull/137))
- Added insertItem function to ListView ([PR #138](https://github.com/texus/TGUI/pull/138))
- Up and down arrows now change selected item in ListBox and ListView ([PR #146](https://github.com/texus/TGUI/pull/146))
- Horizontal scrollbar can now depend on item width in ListView ([PR #147](https://github.com/texus/TGUI/pull/147))
- Added support to copy selected ListView items to clipboard ([PR #148](https://github.com/texus/TGUI/pull/148))
- Add setTextSize for SubwidgetContainer ([PR #149](https://github.com/texus/TGUI/pull/149))
- Added SDL/OpenGL backend as alternative for default SFML backend
- Rewrote signal system again, `b->connect("Pressed",...)` is now `b->onPress(...)`
- Added new FileDialog widget
- Added new ToggleButton widget
- Added new SeparatorLine widget
- Added support for setting mouse cursor + use them on resizable child windows
- Added timers and optional gui.mainLoop()
- Added experimental setOrigin, setScale and setRotation functions to Widget
- Added ThumbWithinTrack to Slider renderer to have thumb align with track on sides
- Added DoubleClick signal to Panel
- Added ViewChanged signal to GuiBase
- Added hover and selected border colors for Tabs
- Added TextureSelectedTrack property to RangeSlider renderer
- Added String::fromNumberRounded to convert float to string with a fixed amount of decimals
- Added startsWith and endsWith helper functions to String
- Added RoundedBorderRadius property to button renderer
- Separators can be added to MenuBar by inserting menu items with "-" string
- ListBox and ListView can now store user data in their items
- ListView icons can be given a fixed size to rescale all icons to requested size
- Replaced all std::string and sf::String by tgui::String
- Replaced Text, Color, Rect and Vector2 classes from SFML with own versions
- Replaced sf::Text::Style with tgui::TextStyle
- Replaced setView in Gui by view/viewport setters with absolute or relative values
- Added onClosing signal to ChildWindow to have a better way to abort closing it
- Signal names, renderer names and widget/renderer properties are now case-sensitive
- Added setWidth and setHeight helper functions
- Added getCheckedRadioButton to RadioButtonGroup
- Textures are now smooth by default
- Size of ChildWindow now includes borders and title bar
- When setView is not called on Gui, view will now scale with window size
- Menus of menu bar are now always on top of all other widgets
- Swapped padding and alignment parameters of addWidget in Grid
- Moved uncheckRadioButtons from Container to RadioButtonGroup
- Theme::setDefault now takes a shared_ptr or a filename as parameter
- Changed return type of addItem in ListBox/ComboBox to return index of the item
- Changed Knob value type from int to float
- Changed default value of ChangeItemOnScroll in ComboBox to false
- Changed default value of ExpandDirection in ComboBox to Automatic
- Changed default value of ScrollbarPolicy in Label to Automatic
- Renamed TextBox to TextArea
- Renamed getMenuList in MenuBar to getMenus
- Renamed mouseOnWidget to isMouseOnWidget
- Renamed TextStyle class to TextStyles and TextStyles::Style enum to TextStyle
- Default scroll amount in ScrollablePanel now depends on global text size
- Gui::getFont now returns the global font if no font was set in the Gui
- Container now translates the widget position before calling draw function
- Dragging scrollbar inside child window didn't work when mouse left child window
- Selected part of RangeSlider wasn't drawn when using textures
- Removed all code that was marked as deprecated
- Binding left and top in layouts now works correctly when the origin is changed
- Some other small changes that weren't added to the changelog


Older Releases
--------------

See the website for changelogs of older releases: https://tgui.eu/changelog/
