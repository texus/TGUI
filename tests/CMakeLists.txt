set(TEST_SOURCES
    main.cpp
    Layouts.cpp
    Loading/Serializer.cpp
    Loading/Deserializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
)

# The tests require c++14
if(SFML_COMPILER_GCC OR SFML_COMPILER_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests ${PROJECT_NAME} ${TGUI_EXT_LIBS})

# For gcc >= 4.0 on Windows, apply the SFML_USE_STATIC_STD_LIBS option if it is enabled
if(SFML_OS_WINDOWS AND SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4")
    if(SFML_USE_STATIC_STD_LIBS AND NOT SFML_COMPILER_GCC_TDM)
        set_target_properties(tests PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
    elseif(NOT SFML_USE_STATIC_STD_LIBS AND SFML_COMPILER_GCC_TDM)
        set_target_properties(tests PROPERTIES LINK_FLAGS "-shared-libgcc -shared-libstdc++")
    endif()
endif()

# Copy the resources folder to the build directory to execute the tests without installing them
add_custom_command(TARGET tests
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/resources $<TARGET_FILE_DIR:tests>/resources)

# Add the install rule for the executable
install(TARGETS tests
        RUNTIME DESTINATION ${INSTALL_MISC_DIR}/tests/ COMPONENT tests
        BUNDLE DESTINATION ${INSTALL_MISC_DIR}/tests/ COMPONENT tests)

# Install the tests
install(DIRECTORY "${CMAKE_SOURCE_DIR}/tests/"
        DESTINATION "${INSTALL_MISC_DIR}/tests/"
        COMPONENT tests)
