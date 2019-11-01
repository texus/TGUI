TGUI 0.9-dev
------------

- Replaced all std::string and sf::String by tgui::String
- Replaced Text, Color, Rect and Vector2 classes from SFML with own versions
- Replaced sf::Text::Style with tgui::TextStyle
- Swapped padding and alignment parameters of addWidget in Grid
- Changed Knob value type from int to float
- Changed default value of ChangeItemOnScroll in ComboBox to false
- Changed default value of ExpandDirection in ComboBox to Automatic
- Changed default value of ScrollbarPolicy in Label to Automatic
- Renamed getMenuList in MenuBar to getMenus
- Container now translates the widget position before calling draw function
- Removed all code that was marked as deprecated


Older Releases
--------------

See the website for changelogs of older releases: https://tgui.eu/changelog/

