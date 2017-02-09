set(TEST_SOURCES
    main.cpp
    Animation.cpp
    Borders.cpp
    Clipboard.cpp
    Color.cpp
    CompareFiles.cpp
    Container.cpp
    Font.cpp
    Layouts.cpp
    Sprite.cpp
    Signal.cpp
    Tests.cpp
    Text.cpp
    Texture.cpp
    TextureManager.cpp
    ToolTip.cpp
    Widget.cpp
    Loading/DataIO.cpp
    Loading/Serializer.cpp
    Loading/Deserializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
    Widgets/Button.cpp
    Widgets/Canvas.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ComboBox.cpp
    Widgets/EditBox.cpp
    Widgets/Grid.cpp
    Widgets/HorizontalLayout.cpp
    Widgets/Knob.cpp
    Widgets/Label.cpp
    Widgets/ListBox.cpp
    Widgets/MenuBar.cpp
    Widgets/MessageBox.cpp
    Widgets/Panel.cpp
    Widgets/Picture.cpp
    Widgets/ProgressBar.cpp
    Widgets/RadioButton.cpp
    Widgets/Scrollbar.cpp
    Widgets/Slider.cpp
    Widgets/SpinButton.cpp
    Widgets/Tabs.cpp
    Widgets/TextBox.cpp
    Widgets/VerticalLayout.cpp
)

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
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/expected $<TARGET_FILE_DIR:tests>/expected
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/fonts/DejaVuSans.ttf $<TARGET_FILE_DIR:tests>/resources
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/resources $<TARGET_FILE_DIR:tests>/resources
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/widgets/Knob $<TARGET_FILE_DIR:tests>/resources/Knob
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/widgets/Black.png $<TARGET_FILE_DIR:tests>/resources
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/widgets/Black.txt $<TARGET_FILE_DIR:tests>/resources)

# Add the install rule for the executable
install(TARGETS tests
        RUNTIME DESTINATION ${INSTALL_MISC_DIR}/tests/ COMPONENT tests
        BUNDLE DESTINATION ${INSTALL_MISC_DIR}/tests/ COMPONENT tests)

# Install the tests
install(DIRECTORY "${CMAKE_SOURCE_DIR}/tests/resources"
        DESTINATION "${INSTALL_MISC_DIR}/tests"
        COMPONENT tests)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/widgets/"
        DESTINATION "${INSTALL_MISC_DIR}/tests/resources"
        COMPONENT tests)
