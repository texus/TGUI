# Create the target
add_library(tgui-activity SHARED TGUIActivity.cpp)

# Define the export symbol of the module
string(REPLACE "-" "_" NAME_UPPER "tgui-activity")
string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
target_compile_definitions(tgui-activity PRIVATE ${NAME_UPPER}_EXPORTS)

# Add a -d suffix when in debug mode
set_target_properties(tgui-activity PROPERTIES DEBUG_POSTFIX -d)

# Hide public symbols by default (exported ones are explicitly marked)
if(TGUI_COMPILER_GCC OR TGUI_COMPILER_CLANG)
    target_compile_options(tgui-activity PRIVATE -fvisibility=hidden)
endif()

if (CMAKE_ANDROID_STL_TYPE MATCHES "_shared")
    target_compile_definitions(tgui-activity PRIVATE STL_LIBRARY=${CMAKE_ANDROID_STL_TYPE})
endif()

tgui_set_global_compile_flags(tgui-activity)
target_compile_options(tgui-activity PRIVATE -fpermissive)
target_link_libraries(tgui-activity PRIVATE android log)

install(TARGETS tgui-activity
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        FRAMEWORK DESTINATION "." COMPONENT bin)

# Install Android.mk so the NDK knows how to set up TGUI
if(TGUI_OS_ANDROID)
    install(FILES Android.mk DESTINATION .)
endif()
