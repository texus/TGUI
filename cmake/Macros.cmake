# Macro that helps defining an option
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Generate a TGUIConfig.cmake file (and associated files)
function(tgui_export_target export_name)
    include(CMakePackageConfigHelpers)
    if(CMAKE_VERSION VERSION_LESS 3.11)
        set(CVF_VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})
        configure_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfigVersion.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/TGUIConfigVersion.cmake" @ONLY)
    else()
        write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/TGUIConfigVersion.cmake"
                                         VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                                         COMPATIBILITY SameMinorVersion)
    endif()

    if (TGUI_SHARED_LIBS)
        set(targets_config_filename TGUISharedTargets.cmake)
    else()
        set(targets_config_filename TGUIStaticTargets.cmake)
    endif()

    export(EXPORT ${export_name}
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${targets_config_filename}")

    if (TGUI_BUILD_FRAMEWORK)
        set(config_package_location "TGUI.framework/Resources/CMake")
    else()
        set(config_package_location lib/cmake/TGUI)
    endif()
    configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/TGUIConfig.cmake"
        INSTALL_DESTINATION "${config_package_location}")

    install(EXPORT ${export_name}
            FILE ${targets_config_filename}
            DESTINATION ${config_package_location})

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/TGUIConfig.cmake"
                  "${CMAKE_CURRENT_BINARY_DIR}/TGUIConfigVersion.cmake"
            DESTINATION ${config_package_location}
            COMPONENT devel)
endfunction()
