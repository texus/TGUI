# Add an option to choose the backend (SFML by default)
tgui_set_option(TGUI_BACKEND "SFML" STRING "Backend to use for rendering and some OS interaction")
set_property(CACHE TGUI_BACKEND PROPERTY STRINGS Custom SFML SDL)

function(tgui_remove_backend_options)
    unset(TGUI_BUILD_WITH_SFML CACHE)
    unset(TGUI_BACKEND_SFML CACHE)
    unset(TGUI_BACKEND_SDL CACHE)
    set(TGUI_BUILD_WITH_SFML FALSE)
endfunction()

set(DescriptionBuildWitSFML "TRUE to include conversion functions between util classes from SFML and TGUI (e.g. between sf::String and tgui::String)")

if(TGUI_BACKEND STREQUAL "Custom")
    tgui_set_option(TGUI_BUILD_WITH_SFML FALSE BOOL ${DescriptionBuildWitSFML})
    tgui_set_option(TGUI_BACKEND_SFML FALSE BOOL "TRUE to build the SFML backend")
    tgui_set_option(TGUI_BACKEND_SDL FALSE BOOL "TRUE to build the SDL backend")

    # Automatically enable building with SFML when SFML backend is used
    if(TGUI_BACKEND_SFML AND NOT TGUI_BUILD_WITH_SFML)
        set(TGUI_BUILD_WITH_SFML TRUE CACHE BOOL ${DescriptionBuildWitSFML} FORCE)
        message(NOTICE "TGUI_BUILD_WITH_SFML has been changed from FALSE to TRUE because TGUI_BACKEND_SFML is set")
    endif()
else()
    unset(TGUI_BUILD_WITH_SFML CACHE)
    unset(TGUI_BACKEND_SFML CACHE)
    unset(TGUI_BACKEND_SDL CACHE)

    if(TGUI_BACKEND STREQUAL "SFML")
        set(TGUI_BUILD_WITH_SFML TRUE CACHE INTERNAL ${DescriptionBuildWitSFML})
        set(TGUI_BACKEND_SFML TRUE)
    else()
        set(TGUI_BUILD_WITH_SFML FALSE CACHE INTERNAL ${DescriptionBuildWitSFML})
        set(TGUI_BACKEND_SDL TRUE)
    endif()

    # The backend options shouldn't be changeable by the user (because he selected one with TGUI_BACKEND),
    # but they still need to be available in other parts of the project (e.g. in the examples).
    set(TGUI_BACKEND_SFML ${TGUI_BACKEND_SFML} PARENT_SCOPE)
    set(TGUI_BACKEND_SDL ${TGUI_BACKEND_SDL} PARENT_SCOPE)
endif()

# If we aren't building SFML then we don't need to keep the SFML_DIR variable (but don't erase its value if it had one)
if(NOT TGUI_BUILD_WITH_SFML AND SFML_DIR STREQUAL "SFML_DIR-NOTFOUND")
    unset(SFML_DIR CACHE)
endif()

if(TGUI_BACKEND_SFML)
    include(Backends/SFML/CMakeLists.txt)
endif()

if(TGUI_BACKEND_SDL)
    include(Backends/SDL/CMakeLists.txt)
endif()

if(NOT TGUI_BACKEND_SFML)
    if(TGUI_BUILD_GUI_BUILDER)
        message(FATAL_ERROR "Gui Builder currently requires the SFML backend. Uncheck TGUI_BUILD_GUI_BUILDER or change backend.")
    endif()
    if(TGUI_BUILD_TESTS)
        message(FATAL_ERROR "Tests currently require the SFML backend. Uncheck TGUI_BUILD_TESTS or change backend.")
    endif()
endif()
