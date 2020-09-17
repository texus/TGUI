---
layout: page
title: Loading from zip file
breadcrumb: loading from zip file
---

TGUI doesn't support loading files from a zip file, you will have to do the actual loading yourself. TGUI just provides a way to override the loading functions which allows you to load the files in any way you want (not limited to loading from zip files).

The first thing to do is add code for loading the themes. For this you have to create a class that inherits from `tgui::DefaultThemeLoader` and overrides the `readFile` function. The custom code you have to write should somehow load the theme file and store its contents in a stringstream.
```c++
class CustomThemeLoader : public tgui::DefaultThemeLoader
{
protected:
    std::unique_ptr<tgui::DataIO::Node> readFile(const tgui::String& themeName) const override
    {
        std::stringstream ss;

        // Load the theme file from the zip here and store the file contents in the string stream.
        // On error, either return nullptr or throw an exception.

        return tgui::DataIO::parse(ss);
    }
};
```

Even when theme files are taken care of, images will still load from filenames by default. In order to change this you will need to write a function that somehow loads the image and store its size and an array of RGBA pixels.
```c++
bool customTextureLoader(tgui::BackendTextureBase& texture, const tgui::String& imageId)
{
    tgui::Vector2u imageSize;
    std::uint8_t* pixels;

    // Load the image from the zip file here.
    // Set imageSize to the width and height of the loaded image.
    // Set pixels to the address of an RGBA pixel array of imageSize.x * imageSize.y * 4 bytes 
    // Return false on error.

    return texture.loadFromPixelData(imageSize, pixels);
}
```

Just declaring the loading functions of course doesn't do anything yet. To activate the custom loading functions you need to execute the following lines. After those 2 function calls are made, your custom loading code will be called when new themes and textures are loaded.
```c++
tgui::Theme::setThemeLoader(std::make_shared<CustomThemeLoader>());
tgui::Texture::setBackendTextureLoader(customTextureLoader);
```
