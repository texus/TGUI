set(TEST_SOURCES
    main.cpp
    Layouts.cpp
)

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests ${PROJECT_NAME} ${TGUI_EXT_LIBS})

# for gcc >= 4.0 on Windows, apply the SFML_USE_STATIC_STD_LIBS option if it is enabled
if(SFML_OS_WINDOWS AND SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4")
    if(SFML_USE_STATIC_STD_LIBS AND NOT SFML_COMPILER_GCC_TDM)
        set_target_properties(tests PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
    elseif(NOT SFML_USE_STATIC_STD_LIBS AND SFML_COMPILER_GCC_TDM)
        set_target_properties(tests PROPERTIES LINK_FLAGS "-shared-libgcc -shared-libstdc++")
    endif()
endif()

# Add the install rule for the executable
install(TARGETS tests
        RUNTIME DESTINATION ${INSTALL_MISC_DIR}/tests/tests COMPONENT tests
        BUNDLE DESTINATION ${INSTALL_MISC_DIR}/tests/tests COMPONENT tests)

# install the examples
install(DIRECTORY "${CMAKE_SOURCE_DIR}/tests/"
        DESTINATION "${INSTALL_MISC_DIR}/tests/"
        COMPONENT tests)
