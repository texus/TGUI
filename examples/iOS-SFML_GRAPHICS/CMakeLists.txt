set(RESOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/Background-Landscape.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/Background-Portrait.png")

set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

tgui_add_example(ios_demo SOURCES main.cpp ${RESOURCES})

# The app needs an identifier and signing to work correctly
set_property(TARGET ios_demo PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.tgui.ios_demo")
