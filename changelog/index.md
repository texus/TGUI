---
layout: page
title: Changelog

changelog:
- version: 0
  minors:
  - version: 8
    patches:
    - version: 5
      date: 6 April 2019
      changes: |
        Big improvements to Gui Builder
        Svg images are now supported
        ComboBox can now contain some text when no item is selected
        Added function to ComboBox to disallow changing the selected item by scrolling the mouse wheel
        Added RightClicked signal to ListView
        Added functions to ListView to change existing items
        Support typing tabs in TextBox (if tab usage is disabled in gui)
        Added function to signals to temporarily disable callbacks
        Added addition and subtraction operators to Outline
        ChildWindow can now have a different border color in focused state
        Added function to select item in TreeView
        EditBox::setInputValidator now returns false when regex was invalid
        Let ComboBox send the ItemSelected event only after the mouse is released
        TitleBarHeight property of default renderer was ignored in ChildWindow
        Label didn't ignore events after ignoreMouseEvents was called
        Adding space around widgets in Grid to fill the given size wasn't working properly
        Loading widget from file failed when min or max was used in layout strings
    - version: 4
      date: 23 February 2019
      changes: |
        Added Changed signal to CheckBox and RadioButton (to more easily combine Checked and Unchecked)
        Added EscapeKeyPressed signal to ChildWindow
        ExpandDirection of ComboBox can now be set to Automatic
        Added min and max functions to layouts again
        Added horizontal grid lines to ListView
        Added option to ListView to expand the last column to fill the remaining space
        Allow a separator between the header and contents in a ListView
        Split separator in ListView into separator and vertical grid line
        Fixed corrupted white theme when DefaultTheme was initialized before Color constants
    - version: 3
      date: 27 January 2019
      changes: |
        ListView widget added
        EditBox can now have a suffix
        TextBox can now have a horizontal scrollbar
        Label can now have a vertical scrollbar
        Default scrollbar width wasn't always taken from texture size in widgets containing scrollbars
        Scrollbar wasn't drawn correctly when Maximum equaled ViewportSize with AutoHide disabled
        Default icons in TreeView didn't change color when item was selected
        Rounded icon position in TreeView to avoid bad icon alignment
        TreeView didn't handle opacity and font changes
        Sprites didn't keep their transparency when resized
        Texture filenames can now contain UTF8 characters on linux
        Added propery to widget renderer to set an opacity for the disabled state
        Fixed some bugs in saving and loading widget files ([#90](https://github.com/texus/TGUI/issues/90))
    - version: 2
      date: 16 December 2018
      changes: |
        TreeView widget added
        Text styles of lines in ChatBox can now be changed
        Clipping was broken when using multiple windows
        ScrollbablePanel didn't fully scroll to right with both scrollbars visible
    - version: 1
      date: 15 October 2018
      changes: |
        Submenus are now supported in MenuBar
        Menus can now be disabled in MenuBar and given a different text color
        You can now connect a signal handler to a single menu item in MenuBar
        ChildWindow position can be locked to disable dragging it
        Scrollbar thumb should not become smaller than the scrollbar width
        Percentage in layout no longer includes the outline of the parent
        MenuBar didn't work when moved and inverted menu direction was broken
        Text size in MenuBar was reset when changing font
        Handle delete button on android correctly when using SFML >= 2.5
        ChildWindow callback with unbound parameter caused crash
    - version: 0
      date: 5 August 2018
      changes: |
        Global default text size for more consistent texts in widgets
        Gui Builder was added
        A theme can be made the default to use it for all new widgets
        Renderers are decoupled from widgets, making them truly shared
        BitmapButton widget to have an icon next to the button caption
        RangeSlider widget to have two thumbs on a slider
        ScrollablePanel widget to have a Panel with automatic scrollbars
        Panel widget was split in Group, RadioButtonGroup and Panel widgets
        HorizontalLayout, VerticalLayout and HorizontalWrap to arrange widgets
        Relative layouts were improved
        Many other improvements
  - version: 7
    patches:
    - version: 8
      date: 5 August 2018
      changes: |
        EditBox::setInputValidator now throws instead of crashing on GCC < 4.9
    - version: 7
      date: 21 April 2018
      changes: |
        Fixed EditBox InputValidator in textEntered when text was selected
        Hover image of EditBox wasn't being used in Black theme
        Loading widgets from theme was partially broken on android
        ChatBox::setScrollbar didn't correctly position the scrollbar
        ChildWindow::setTitleButtons didn't set the button positions
        ComboBox::setListBox removed all items from combo box
        Added support for using SFMLConfig.cmake to find SFML
    - version: 6
      date: 25 November 2017
      changes: |
        Fixed _fullpath function not found in some MinGW versions
        Knob did not display correctly when using textures
        Fixed crash in layouts in specific case with GCC 7
        ChildWindow::setTitleButtons did not accept combinations of buttons
        Theme files missed MinimizeButton and MaximizeButton in ChildWindow section
        ComboBox::setItemsToDisplay did not show full list when 0 was passed
        Non-string layouts are now also copied when copying a widget
        OpenGL is no longer used for clipping
        Fixed crash that could occur when performing a division by 0 in the layouts
    - version: 5
      date: 11 September 2017
      changes: |
        Added closeMenu function to MenuBar
        System clipboard is now used on all platforms when using SFML >= 2.5
        Fixed FindTGUI.cmake script when patch version is not specified
        Label didn't send a SizeChanged signal when its text changed
        The size of a Grid was reset in removeAllWidgets
        Holding shift and pressing arrow keys will select text in EditBox
        Fixed syntax error in BabyBlue and TransparentGrey themes
        Resource path was not correctly used for all resources
        Picture::create did not store the filename used
    - version: 4
      date: 3 April 2017
      changes: |
        Theme and widget file parser now provides line number on error
        Scrollbar in ListBox remained in hover state until mouse left the ListBox
        The position of TextBox could be wrong when using layouts
        Fixed incorrect clipping when the viewport was changed
        Optional parameter from Checked/Unchecked signal could have been wrong
    - version: 3
      date: 9 February 2017
      changes: |
        Added create function to widgets and Theme which is now the preferred way to construct them
        Added setDefaultTextStyle to EditBoxRenderer
        Touch events were not handled properly when not using the default view
        Optimized EditBox::findCaretPosition
    - version: 2
      date: 2 December 2016
      changes: |
        EditBox did not send a TextChanged signal when pressing ctrl+X
        Added optional maximize and minimize buttons to ChildWindow ([#61](https://github.com/texus/TGUI/pull/61))
        Calling Picture::setTexture yourself now works properly
        Added optional parameter to Widget::disable to let mouse events pass through to the widgets behind it
        ChatBox didn't scroll down automatically when size was not a multiple of item heigh
        Clipping in ChatBox did not take padding into account
        Mouse wheel scroll on top of combo box changed the item internally but did not display the new item
        When ComboBox was destroyed while the list was still open then the list remained visible
        Fixed invalid memory reads when widget gets destroyed from inside a callback function
        Allow the ChildWindow to have maximum and minimum sizes ([#64](https://github.com/texus/TGUI/pull/64))
        Removed support for 32-bit on OS X
        Fixed texture rotation in some rare cases
        Menus in MenuBar didn't stay open when the menu bar was added inside a Panel
        Added EditBox::getCaretPosition
        Fixed clipping issues when not using the default view
        getAbsolutePosition no longer takes the position of the gui view into account
    - version: 1
      date: 12 June 2016
      changes: |
        Picture can now be loaded from a part of an sf::Texture
        Any texture in any widget can now be set to an sf::Texture
        Optimized adding lines to ChatBox
        You can now pass <i>const char*</i> directly to layout (instead of needing std::string)
        Panel widgets can now have borders around them
        Setting texture color did not work when using transparent widgets
        Passing std::bind expression as parameter to the connect function did not work on VS2013
        Fixed tab key not working for widgets inside several containers
        Fixed crash in Signal class under specific circumstances
    - version: 0
      date: 4 April 2016
      changes: |
        Experimental Android and iOS support
        Layout system for relative sizes and positions
        Rewritten callback system
        New loading system to share theme between widgets and allow customizing loading steps
        Exceptions are now used for error handing
        No longer only textured widgets, also colored widgets which do not need external files
        Many, many more changes
  - version: 6
    patches:
    - version: 10
      date: 24 January 2016
      changes: |
        Improved vertical text alignment in ListBox
        Fixed RadioButton images being used when CheckBox was loaded from widget file
    - version: 9
      date: 11 July 2015
      changes: |
        Allow loading custom sections in theme files
        Smooth scrolling in Slider and Scrollbar
        Added setAlignment function to EditBox
        Fixed clipping when drawing on Canvas right before drawing the gui
        Widget was not given a parent when it was copied
        SpinButton did not send mouse click callbacks
        Link to Headers inside mac framework had an absolute path
        Fixed scaling of form builder window
        Limit the amount of visible items in the widget selector in form builder
    - version: 8
      date: 10 May 2015
      changes: |
        Fixed segfault during handleEvent in a rare situation
        Removed warnings in ListBox when index was too high
        References from getWidgets and getWidgetNames should be const
        Worked around a syntax error when using SWIG ([#37](https://github.com/texus/TGUI/issues/37))
    - version: 7
      date: 7 February 2015
      changes: |
        Added support for frameworks on mac os x
        Added setTextStyle and getTextStyle to Label
        Allow widget changes before they are added to the gui
        Completely cache theme files on first access
        Fixed loading/saving in form builder on mac os x
        Fixed potential crash in VS2012
    - version: 6
      date: 21 September 2014
      changes: |
        Cache theme files for faster loading
        Added method to load a Picture from an sf::Texture
        You can no longer try to copy the Gui object
        First selected newline in TextBox wasn't being copied with ctlr+c
        Fixed variable in ListBox not being copied
        Dragging events were not passed through container widgets
        List from ComboBox did not shrink when removing items
        Fixed crash when removing widget in a specific situation
    - version: 5
      date: 13 August 2014
      changes: |
        Added FindTGUI.cmake
        Added recursive search with widget name
        Text can now be selected with ctrl+A
        Lines in ChatBox can now start from bottom
        Worked around bug in SFML 2.1 in Label
        Fixed bug in ChatBox with displaying some lines
        Fixed panel keeping hover state of widget
        Fixed potential crash when removing a widget
        Fixed bug with saving and loading in form builder
    - version: 4
      date: 28 June 2014
      changes: |
        Improved auto-sizing of text
        Allow rendering to a RenderTarget
        On windows the system clipboard will be used
        ListBox and ComboBox now have ids for their items
        ListBox and ComboBox now have methods to change items
        Fixed crash in TextBox when no font was set
        Fixed crash in EditBox when LimitTextWidth was set
        ScrollAmount now also taken into account when using the mouse wheel
        Title text in ChildWindow wasn't vertically centered
        Theme files edited on windows couldn't be loaded on unix
        ListBox::setSelectedItem should move the scrollbar if needed
        ChatBox didn't display correctly when text got split over multiple lines
    - version: 3
      date: 11 April 2014
      changes: |
        Added canvas widget to render sfml inbetween tgui widgets
        Add a function to TextBox to make it read-only
        Clipboard calculations didn't take viewport into account
        Fixed Tab text being wrongly clipped when tab width is too small
        BabyBlue theme didn't use correct EditBox images
        SharedWidgetPtr class was lacking a working == and != operator
    - version: 2
      date: 14 March 2014
      changes: |
        Fixed potential bug in setMaximum and setMinimum in Slider and Knob
        Fixed Knob widget not being displayed correctly when scaled
        Fixed invisible title bar of child window when setSize was not called
    - version: 1
      date: 9 February 2014
      changes: |
        Made several fixes to Scrollbar
        Checkbox now inherits from RadioButton instead of the other way around
        Text was also being selected in TextBox when clicking on the scrollbar
        Some fonts didn't display all lines in ChatBox
        Allow setting a default text color in ChatBox and passing different parameters to addLine
        Allow setting the line spacing in ChatBox
        Added a resetView parameter to handleEvent and set the default value on true (for draw function as well)
        Implemented getFullSize and thus changed behavior of getSize
        The TabKeyUsageEnabled property should now be accessed through a function call
        Implemented setResourcePath function to load everything from a relative path
        Support clang on linux
    - version: 0
      date: 16 January 2014
      changes: |
        Many things changed, no list of changes was maintained.
  - version: 5
    patches:
    - version: 2
      date: 17 May 2013
      changes: |
        Added ChatBox class.
        Fixed a bug in LoadingBar when minimum wasn't 0.
        Fixed a bug in slider when calling setVerticalScroll after setSize.
    - version: 1
      date: 8 February 2013
      changes: |
        Added support for changing the title alignment in ChildWindow.
        Fixed a minor bug with unfocusing objects.
        Fixed a small bug in the CMake script.
    - version: 0
      date: 23 December 2012
      changes: |
        New objects: ChildWindow, AnimatedPicture, SpinButton, Tab, Grid and more.
        Changed the add, get and copy functions from Group. This allows creating custom objects.
        Serious speed increase, especially with many objects or with ListBox, ComboBox and Label.
        ComboBox::load now also takes the height as parameter.
        EditBox and TextBox take the selection point color as extra parameter in changeColors.
        Improved documentation and tutorials.
        Panel can optionally load a background image.
        Objects in panel can now be focused with the tab key.
        You can now choose to only draw part of the label.
        Label has an optional background color.
        Double clicking in EditBox and TextBox now selects the text.
        Panel and ChildWindow can also load their objects from the Object Files.
        You can choose to disable the tab key usage.
        An easy install script was added for linux users.
        A LoadingBar can now have text on top of it.
        Callback now contains a pointer to the object.
        Unicode support through the use of sf::String.
        Many bug fixes.
  - version: 4
    patches:
    - version: 2
      date: 16 July 2012
      changes: |
        Bug fix: ComboBox wasn't responding correctly when scaled.
        Bug fix: ComboBox wasn't rendered correctly when using a scrollbar.
    - version: 1
      date: 15 July 2012
      changes: |
        Bug fix: Slider caused a runtime error on windows.
        Bug fix: Objects didn't behave correctly when the view was changed.
        Bug fix: When all the objects were hidding, the first one was still drawn.
    - version: 0
      date: 13 July 2012
      changes: |
        New objects: Panel, TextBox and SpriteSheet.
        Objects can be moved to the back or to the front.
        Scrolling in listbox has been improved (you can now set the scrollbar between two items).
        The images used in Checkbox can now have a different size.
        Hover images no longer have to be semi-transparent.
        Text width can now be limited in EditBox.
        A maximum item limit was added in ComboBox.
        You can now disable objects.
        Many bug fixes.
  - version: 3
    patches:
    - version: 7
      date: 5 April 2012
      changes: |
        Mac OS X is now supported (no libraries yet, but the source code can be used)
        Changing the global font no longer changes the font of already existing objects.
        You can now change the global font directly, the setGlobalFont function was removed.
        All objects now have a getSize and getScaledSize function.
        Label now sends a callback on click.
        You have an option too keep the scrollbar visible, even if you can't scroll.
        Bug fix: The getText function of EditBox returned a sf::String instead of an std::string.
        Bug fix: Arrow keys were not repeated in EditBox.
        Bug fix: Texture rect was not updated when calling the load function again.
        Bug fix: Callback from Picture didn't work with scaling.
        Bug fix: EditBox and ListBox hung when image was too small.
        Bug fix: Alpha value in colors was sometimes wrong.
        Bug fix: The displayed text in EditBox was wrong when calling setMaximumCharacters.
        Bug fix: ComboBox caused a crash when image was too small.
    - version: 6
      date: 13 March 2012
      changes: |
        A new object: Label.
        Bug fix: The draw function was overriding the one from sfml, it is now called drawGUI.
    - version: 5
      date: 12 March 2012
      changes: |
        The 'm_' prefix was dropped on public members.
        Picture now has a click callback.
        The background color of the selected text is now different when the EditBox is not focused.
        EditBox now also sends a callback when the return key was pressed.
        A new object: Panel.
        Bug fix: The text on the button was sometimes blurry with fixed text size.
        Bug fix: The arrow of ComboBox kept pointing down when the list was shown.
    - version: 4
      date: 10 March 2012
      changes: |
        setSize function added to all gui objects. It is recommended over the SetScale function.
        You can now change the font of EditBox, ListBox and ComboBox.
        A setGlobalFont function was added to Panel (also to Window) to set the font of all the objects.
        You can now add CallbackID to any object that is loaded from a file.
        A horizontal scrollbar has been added.
        Bug fix: Objects behind a picture still received events.
        Bug fix: Thumb position of Scrollbar was wrong while scaling.
        Bug fix: The program crashed after calling removeAllObjects.
    - version: 3
      date: 6 March 2012
      changes: |
        Scrolling was added in EditBox, there is no more text width limit by default.
        You can now get the colors that are used in EditBox, Listbox and ComboBox.
        You can now select an item in ListBox with the new setSelectedItem function.
        You can now use the scrollbar.
        Panels can now be used, which means that you can create multiple groups of linked radio buttons.
        You can now load all the objects (except Panel) from a file.
        Except for the .lib and .so files, I have included .a files.
        Bug fix: You couldn't call setBorders after setText in EditBox.
        Bug fix: Changing the maximum items and the item height in ListBox didn't work properly.
        Bug fix: Scrollbar didn't receive the mouse up event when releasing the mouse on ListBox.
        Bug fix: Calling a load function with an empty string made the program crash.
    - version: 2
      date: 20 February 2012
      changes: |
        ComboBox was added
        Bug fixes in EditBox, ListBox and Scrollbar
    - version: 1
      date: 15 February 2012
      changes: |
        Namespace was changed to lowercase.
        A setVerticalScroll function for changing m_VerticalScroll indirectly.
        Changed the name from setTextColors to changeColors inside EditBox to conform with the ListBox.
        Small bug fixes in EditBox and Listbox.
        Fixed wrong behavior of scrollbar.
        Colors of objects were changed to fit better with each other.
    - version: 0
      date: 12 February 2012
      changes: |
        First public release.
---

{% for major in page.changelog %}
  {% for minor in major.minors %}
    {% for patch in minor.patches %}
      {% capture version %}0.{{minor.version}}.{{patch.version}}{% endcapture %}
<h3 id="{{version}}">TGUI {{version}} <span class="ChangelogDate">({{patch.date}})</span></h3>
        {% assign changes = patch.changes | newline_to_br | strip_newlines | split: '<br />' %}
        {% for change in changes %}
- {{change}}
        {% endfor %}
    {% endfor %}

    {% if forloop.last == false %}
<br>
<hr>
    {% endif %}
  {% endfor %}
{% endfor %}
