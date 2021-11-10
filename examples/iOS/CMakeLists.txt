set(RESOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/Background-Landscape.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/Background-Portrait.png")

set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

macro(tgui_add_ios_example backend)
    tgui_add_example(ios_demo_${backend} SOURCES iOS-example-${backend}.cpp ${RESOURCES})

    # The app needs an identifier and signing to work correctly
    set_property(TARGET ios_demo_${backend} PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
    set_target_properties(ios_demo_${backend} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "eu.tgui.ios_demo"
        MACOSX_BUNDLE_BUNDLE_NAME "ios_demo_${backend}"
    )
endmacro()

if(TGUI_HAS_BACKEND_SFML_GRAPHICS)
    tgui_add_ios_example(SFML_GRAPHICS)
endif()
if(TGUI_HAS_BACKEND_SDL_RENDERER)
    tgui_add_ios_example(SDL_RENDERER)
endif()
