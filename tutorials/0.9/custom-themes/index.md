---
layout: page
title: Custom themes
breadcrumb: custom themes
---

Custom themes can be shared with other TGUI users via the [Themes forum section](https://forum.tgui.eu/themes/).

To create your own themes, you can use the existing themes (which can be found in the "themes" subfolder of TGUI) as a starting point.

### Syntax

The theme file contains of multiple sections, each having a name and its contents between braces. Within a section, you write property-value pairs with an equal sign in between and a semicolon at the end. Whitespace is ignored.
```
Button {
  Property1 = Value1;
  Property2 = Value2;
}
```

The name of the section can be freely chosen. The name will however usually be the same as the widget type, as this is what a widget will attempt to load from the default theme (e.g. slider widget will search for a section called "Slider"). If you set a renderer to the widget after loading then you can hower choose to load a section with any name. So you can have sections like "BlueButton" and "RedButton", the created buttons just won't load them by default.

The property names have to correspond with what the widget supports. If the section is going to be used as renderer for a RadioButton then the properties should match the ones found in the [RadioButtonRenderer](https://tgui.eu/documentation/0.9/classtgui_1_1RadioButtonRenderer.html). The renderer has e.g. setTextColor and getTextColor functions, so the property in the theme file would be called TextColor (case-sensitive).

Different properties have different types. Which type should be obvious when looking at the renderer. The "TextColor" property for example should be given a color as value. An explanation on how a value of each type should look like can be found below.

Comments can also be added to the file by using either `//` for single line comments or wrapping the contents in `/*` and `*/`.

### Strings and numbers

Although simple strings don't need quotes around them, it is recommended to always write quotes around your string value. Escaping works the same way as in c++, by using a backslash in front of the character:
```
Text = "String with \\, \" and even a \n inside it";
```

Numbers don't require much explanation:
```
Opacity = 0.7;
DistanceToSide = 5;
```

### Colors

There are 3 ways to encode a color:
- As hexadecimal RGB or RGBA value, e.g. #FF00FF
- Using either "rgb" or "rgba" followed by comma-separated values, e.g. rgba(255, 255, 0, 127)
- Using a predefined constant, e.g. Red

The predefined colors should be treated as case-sensitive (even though TGUI currently allows case-insensitive color values). The possible values correspond to the [color constants in c++](https://tgui.eu/documentation/0.9/classtgui_1_1Color.html#pub-static-attribs).

```
TextColor = #00FF007F;
BackgroundColor = rgb(0, 255, 255);
BorderColor = Green;
```

### Textures

Values of textures can contain multiple optional properties, but the only mandatory part of the value is a filename between quotes. Unless an absolute path is provided, the path will be considered relative to the location of the theme file.
```
Texture = "image.png";
```

A "Part" rectangle can be provided to load only a part of the image. If not present then the entire image would be loaded. The following line would load an area of 50x30 pixels starting from position (20,10) in the image:
```
Texture = "image.png" Part(20, 10, 50, 30);
```

A "Middle" rectangle can be provided to specify how the image should be scaled. If a Part rectangle is given then the Middle rectangle is relative to it (i.e. the part is first extracted from the image and the Middle rectangle acts as if this part of the image is the entire image). Just like the Part rectangle, the Middle is provided in the (left, top, width, height) format.
```
Texture = "image.png" Part(20, 10, 50, 30) Middle(6, 6, 38, 18);
```

There are 4 ways that an image can be scaled:
- If no Middle rectangle is provided or it has the same size as the image then the image is stretched normally.
- If the Middle rectangle has a left and top different from 0 then [9-slice scaling](https://en.wikipedia.org/wiki/9-slice_scaling) is used. The Middle rectangle divides the image in 9 areas. The corners are never stretched, the sides are only stretched in one direction and the center part is stretched in both directions to fill the remaining space.
- If the left differs from 0 but the top is 0 then the middle rectangle only splits the image in 3 parts (assuming the height of the middle rectangle is the same as the height of the part rectangle, otherwise 9-slice scaling is still used). The left and right parts of the image will maintain their ratio when scaled while the center part is stretched in both directions. This is useful for buttons that have half a circle on each side.
- If the left is 0 but the top differs from 0 (and width of middle rect corresponds to the width of the part rect), then the middle rectangle splits the image in 3 parts above each other. The top and bottom part will maintain their ratio when scaled while the center part is stretched in both directions.

Because in almost all cases you want the space on the left and top in the middle rectangle to be equal to the space on the right and bottom, you can also specify it with only 2 values:
```
Texture = "image.png" Part(20, 10, 50, 30) Middle(6, 6);
```

At the very end of the value, the "Smooth" or "NoSmooth" word can be added to override the default setting. The NoSmooth option will use Nearest Neighbor while the Smooth option will choose bilinear interpolation when scaling the image.
```
Texture = "image.png" NoSmooth;
Texture = "image.png" Middle(10, 10) Smooth;
```


### Outline

Borders and Padding properties use an Outline type. Four values have to be provided, representing the border/padding thickness on the left, top, right and bottom sides. When only 2 values are provided then they are parsed as left/right and top/bottom thickness. If only one value is provided then the outline will have the same size on all sides.
```
Borders = (1, 2, 3, 4);
Borders = (1, 2);  // Same as (1, 2, 1, 2)
Borders = (1); // Same as (1, 1, 1, 1)
```

### Nested sections

Some widgets might contain other widgets (e.g. ChildWindow contains a close button and multiple widgets contain a scrollbar). In these cases their renderer also contains the renderer for the subwidget.
```
TextArea {
  BackgroundColor = Red;  // Background color of ListBox
  ScrollBar = {
    BackgroundColor = Green;  // Background color of vertical and horizontal scrollbar inside TextArea
  };
}
```

### References to other sections

You probably want all scrollbars to look the same way. Instead of repeating the renderer properties for the scrollbar in each scrollable widget, you can also have a single scrollbar section and refer to it inside the scrollable widgets. When refering to another section, the value should consist of an ampersand followed by the section name.
```
CustomScrollbarSection {
}

TextArea {
  Scrollbar = &CustomScrollbarSection;
}
```
