# Create the target
add_library(tgui-activity SHARED TGUIActivity.cpp)

# Define the export symbol of the module
string(REPLACE "-" "_" NAME_UPPER "tgui-activity")
string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
set_target_properties(tgui-activity PROPERTIES DEFINE_SYMBOL ${NAME_UPPER}_EXPORTS)

# Add a -d suffix when in debug mode
set_target_properties(tgui-activity PROPERTIES DEBUG_POSTFIX -d)

# If using gcc >= 4.0 or clang >= 3.0 on a non-Windows platform, we must hide public symbols by default (exported ones are explicitly marked)
if((SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4") OR (SFML_COMPILER_CLANG AND NOT SFML_CLANG_VERSION VERSION_LESS "3"))
    set_target_properties(tgui-activity PROPERTIES COMPILE_FLAGS -fvisibility=hidden)
endif()

# tgui-activity library is our bootstrap activity and must not depend on stlport_shared (otherwise Android will fail to load it)
if (SFML_OS_ANDROID)
    set_target_properties(tgui-activity PROPERTIES COMPILE_FLAGS -fpermissive)
    set_target_properties(tgui-activity PROPERTIES LINK_FLAGS "-landroid -llog")

    # Restore the original compiler flags
    set(CMAKE_CXX_FLAGS "${TGUI_ACTIVITY_CMAKE_CXX_FLAGS}" CACHE STRING "C++ compiler flags" FORCE)

    # This is a workaround to compile tgui-activity without stlport_shared as dependency
    set(CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
endif()

# Install rule
install(TARGETS tgui-activity
        RUNTIME DESTINATION bin COMPONENT bin
        LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT bin
        ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT devel
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_FRAMEWORK_PREFIX} COMPONENT bin)
