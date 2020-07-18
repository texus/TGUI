TGUI 0.9-dev
------------

- Added support for setting mouse cursor + use them on resizable child windows
- Added timers and optional gui.mainLoop()
- Added experimental setOrigin, setScale and setRotation functions to Widget
- Added ThumbWithinTrack to Slider renderer to have thumb align with track on sides
- ListBox and ListView can now store user data in their items
- Rewrote signal system again, `b->connect("Pressed",...)` is now `b->onPress(...)`
- Replaced all std::string and sf::String by tgui::String
- Replaced Text, Color, Rect and Vector2 classes from SFML with own versions
- Replaced sf::Text::Style with tgui::TextStyle
- Replaced setView in Gui by view/viewport setters with absolute or relative values
- Signal names, renderer names and widget/renderer properties are now case-sensitive
- Added setWidth and setHeight helper functions
- Added getCheckedRadioButton to RadioButtonGroup
- Textures are now smooth by default
- Size of ChildWindow now includes borders and title bar
- When setView is not called on Gui, view will now scale with window size
- Menus of menu bar are now always on top of all other widgets
- Swapped padding and alignment parameters of addWidget in Grid
- Moved uncheckRadioButtons from Container to RadioButtonGroup
- Changed return type of addItem in ListBox to return index of the item
- Changed Knob value type from int to float
- Changed default value of ChangeItemOnScroll in ComboBox to false
- Changed default value of ExpandDirection in ComboBox to Automatic
- Changed default value of ScrollbarPolicy in Label to Automatic
- Renamed getMenuList in MenuBar to getMenus
- Renamed mouseOnWidget to isMouseOnWidget
- Default scroll amount in ScrollablePanel now depends on global text size
- Gui::getFont now returns the global font if no font was set in the Gui
- Container now translates the widget position before calling draw function
- Dragging scrollbar inside child window didn't work when mouse left child window
- Removed all code that was marked as deprecated

Older Releases
--------------

See the website for changelogs of older releases: https://tgui.eu/changelog/

