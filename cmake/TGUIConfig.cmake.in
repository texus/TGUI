# This script provides the TGUI libraries as imported targets
# ===========================================================
#
# Usage
# -----
#
# Examples:
#   find_package(TGUI 0.8)
#   find_package(TGUI 0.8 REQUIRED)
#
# By default, the dynamic libraries of TGUI will be found. To find the static ones instead,
# you must set the TGUI_STATIC_LIBRARIES variable to TRUE before calling find_package(TGUI ...).
# example:
#   set(TGUI_STATIC_LIBRARIES TRUE)
#   find_package(TGUI 0.8 REQUIRED)
#
# On macOS by default CMake will search for frameworks. If you want to use static libraries and have installed
# both TGUI frameworks and TGUI static libraries, your must set CMAKE_FIND_FRAMEWORK to "NEVER" or "LAST"
# in addition to setting TGUI_STATIC_LIBRARIES to TRUE. Otherwise CMake will check the frameworks bundle config and
# fail after finding out that it does not provide static libraries. Please refer to CMake documentation for more details.
#
# Additionally, keep in mind that TGUI frameworks are only available as release libraries unlike dylibs which
# are available for both release and debug modes.
#
# If TGUI is not installed in a standard path, you can use the TGUI_DIR CMake variable
# to tell CMake where TGUI's config file is located (PREFIX/lib/cmake/TGUI for a library-based installation,
# and PREFIX/TGUI.framework/Resources/CMake on macOS for a framework-based installation).
#
# Output
# ------
#
# This script defines the following variable:
# - TGUI_FOUND:        true if all the required modules are found
#
# And the following target:
#   - tgui
#
# The TGUI target is the same for both Debug and Release build configurations and will automatically provide
# correct settings based on your currently active build configuration. The TGUI target name also does not change
# when using dynamic or static TGUI libraries.
#
# example:
#   find_package(TGUI 0.8 REQUIRED)
#   add_executable(myapp ...)
#   target_link_libraries(myapp tgui)

set(FIND_TGUI_PATHS
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    ${TGUI_ROOT}
    $ENV{TGUI_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

# Choose which target definitions must be imported
if (TGUI_STATIC_LIBRARIES)
	set(TGUI_IS_FRAMEWORK_INSTALL "@TGUI_BUILD_FRAMEWORKS@")
    if (TGUI_IS_FRAMEWORK_INSTALL)
        message(WARNING "Static frameworks are not supported by TGUI. Clear TGUI_DIR cache entry, \
and either change TGUI_STATIC_LIBRARIES or CMAKE_FIND_FRAMEWORK before calling find_package(TGUI)")
    endif()
    set(config_name "Static")
else()
    set(config_name "Shared")
endif()
set(targets_config_file "${CMAKE_CURRENT_LIST_DIR}/TGUI${config_name}Targets.cmake")

# Generate imported targets for TGUI
if (EXISTS "${targets_config_file}")
    # Set TGUI_FOUND to TRUE by default, may be overwritten by one of the includes below
    set(TGUI_FOUND TRUE)
    include("${targets_config_file}")
else()
    set(FIND_TGUI_ERROR "Requested TGUI configuration (${config_name}) was not found")
    set(TGUI_FOUND FALSE)
endif()

if (NOT TGUI_FOUND)
    if(TGUI_FIND_REQUIRED)
        # fatal error
        message(FATAL_ERROR "${FIND_TGUI_ERROR}")
    elseif(NOT TGUI_FIND_QUIETLY)
        # error but continue
        message(STATUS "${FIND_TGUI_ERROR}")
    endif()
endif()

if (TGUI_FOUND AND NOT TGUI_FIND_QUIETLY)
    message(STATUS "Found TGUI @TGUI_VERSION_MAJOR@.@TGUI_VERSION_MINOR@.@TGUI_VERSION_PATCH@ in ${CMAKE_CURRENT_LIST_DIR}")
endif()
