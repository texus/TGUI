# Add an option to choose the backend (SFML by default)
tgui_set_option(TGUI_BACKEND "SFML" STRING "Backend to use for rendering and some OS interaction")
set_property(CACHE TGUI_BACKEND PROPERTY STRINGS Custom SFML SDL)

function(tgui_remove_backend_options)
    unset(TGUI_HAS_BACKEND_SFML CACHE)
    unset(TGUI_HAS_BACKEND_SDL CACHE)
endfunction()

set(DescriptionBuildWitSFML "TRUE to include conversion functions between util classes from SFML and TGUI (e.g. between sf::String and tgui::String)")

if(TGUI_BACKEND STREQUAL "Custom")
    tgui_set_option(TGUI_HAS_BACKEND_SFML FALSE BOOL "TRUE to build the SFML backend")
    tgui_set_option(TGUI_HAS_BACKEND_SDL FALSE BOOL "TRUE to build the SDL backend")
else()
    unset(TGUI_HAS_BACKEND_SFML CACHE)
    unset(TGUI_HAS_BACKEND_SDL CACHE)

    if(TGUI_BACKEND STREQUAL "SFML")
        set(TGUI_HAS_BACKEND_SFML TRUE)
    elseif(TGUI_BACKEND STREQUAL "SDL")
        set(TGUI_HAS_BACKEND_SDL TRUE)
    else()
        message(FATAL_ERROR "TGUI_BACKEND was set to an unknown backend")
    endif()

    # The backend options shouldn't be changeable by the user (because he selected one with TGUI_BACKEND),
    # but they still need to be available in other parts of the project (e.g. in the examples).
    set(TGUI_HAS_BACKEND_SFML ${TGUI_HAS_BACKEND_SFML} PARENT_SCOPE)
    set(TGUI_HAS_BACKEND_SDL ${TGUI_HAS_BACKEND_SDL} PARENT_SCOPE)
endif()

# If we aren't building SFML then we don't need to keep the SFML_DIR variable (but don't erase its value if it had one)
if(NOT TGUI_HAS_BACKEND_SFML AND SFML_DIR STREQUAL "SFML_DIR-NOTFOUND")
    unset(SFML_DIR CACHE)
endif()

if(TGUI_HAS_BACKEND_SFML)
    include(Backends/SFML/CMakeLists.txt)
endif()

if(TGUI_HAS_BACKEND_SDL)
    include(Backends/SDL/CMakeLists.txt)
endif()

add_library(tgui-default-backend-interface INTERFACE)
if(TGUI_HAS_BACKEND_SFML)
    target_link_libraries(tgui-default-backend-interface INTERFACE tgui-sfml-interface)
elseif(TGUI_HAS_BACKEND_SDL)
    target_link_libraries(tgui-default-backend-interface INTERFACE tgui-sdl-interface)
endif()
