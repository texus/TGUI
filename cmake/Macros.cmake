# Macro that helps defining an option
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Set the compile options used by all targets
function(tgui_set_global_compile_flags target)
    if(TGUI_COMPILER_MSVC OR (TGUI_OS_WINDOWS AND TGUI_COMPILER_CLANG AND NOT MINGW))
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target}
                               PRIVATE
                               -Wall
                               -Wextra
                               -Wshadow
                               -Wnon-virtual-dtor
                               -Wold-style-cast
                               -Wcast-align
                               -Wunused
                               -Woverloaded-virtual
                               -Wpedantic
                               -Wdouble-promotion
                               -Wformat=2)
    endif()

    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(${CMAKE_VERSION} VERSION_LESS "3.8.0")
        set_target_properties(${target} PROPERTIES CXX_STANDARD_REQUIRED ON)
        set_target_properties(${target} PROPERTIES CXX_STANDARD ${TGUI_CXX_STANDARD})
    else() # CMake 3.8 or newer
        target_compile_features(${target} PUBLIC cxx_std_${TGUI_CXX_STANDARD})
    endif()
endfunction()

# Set the appropriate standard library on each platform for the given target
function(tgui_set_stdlib target)
    # Use libc++ on macOS
    if(TGUI_OS_MACOS)
        if(${CMAKE_GENERATOR} MATCHES "Xcode")
            set_property(TARGET ${target} PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        else()
            target_compile_options(${target} PRIVATE "-stdlib=libc++")
            target_link_libraries(${target} PRIVATE "-stdlib=libc++")
        endif()
    endif()

    # Apply the TGUI_USE_STATIC_STD_LIBS option on windows when using GCC.
    # For VC++ this was already handled earlier as it required a global change.
    if(TGUI_OS_WINDOWS AND TGUI_COMPILER_GCC)
        if(TGUI_USE_STATIC_STD_LIBS AND NOT TGUI_COMPILER_GCC_TDM)
            target_link_libraries(${target} PRIVATE "-static-libgcc" "-static-libstdc++")
        elseif(NOT TGUI_USE_STATIC_STD_LIBS AND TGUI_COMPILER_GCC_TDM)
            target_link_libraries(${target} PRIVATE "-shared-libgcc" "-shared-libstdc++")
        endif()
    endif()
endfunction()

# Generate a TGUIConfig.cmake file (and associated files)
function(tgui_export_target export_name)
    include(CMakePackageConfigHelpers)
    if(CMAKE_VERSION VERSION_LESS 3.11)
        set(CVF_VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})
        configure_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfigVersion.cmake.in" "${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake" @ONLY)
    else()
        write_basic_package_version_file("${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
                                         VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                                         COMPATIBILITY SameMinorVersion)
    endif()

    if (TGUI_SHARED_LIBS)
        set(targets_config_filename TGUISharedTargets.cmake)
    else()
        set(targets_config_filename TGUIStaticTargets.cmake)
    endif()

    export(EXPORT ${export_name}
           FILE "${PROJECT_BINARY_DIR}/${targets_config_filename}")

    if (TGUI_BUILD_FRAMEWORK)
        set(config_package_location "TGUI.framework/Resources/CMake")
    else()
        set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/TGUI)
    endif()
    configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfig.cmake.in" "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
        INSTALL_DESTINATION "${config_package_location}")

    install(EXPORT ${export_name}
            FILE ${targets_config_filename}
            DESTINATION ${config_package_location})

    install(FILES "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
                  "${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
            DESTINATION ${config_package_location}
            COMPONENT devel)
endfunction()
