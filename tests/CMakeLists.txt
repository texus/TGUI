set(TEST_SOURCES
    main.cpp
    Tests.cpp
    CompareFiles.cpp
    AbsoluteOrRelativeValue.cpp
    Animation.cpp
    Clipboard.cpp
    Clipping.cpp
    Color.cpp
    Container.cpp
    Focus.cpp
    Font.cpp
    Layouts.cpp
    Outline.cpp
    Sprite.cpp
    Signal.cpp
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
    Widgets/BitmapButton.cpp
    Widgets/Button.cpp
    Widgets/Canvas.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ComboBox.cpp
    Widgets/EditBox.cpp
    Widgets/Group.cpp
    Widgets/Grid.cpp
    Widgets/HorizontalLayout.cpp
    Widgets/HorizontalWrap.cpp
    Widgets/Knob.cpp
    Widgets/Label.cpp
    Widgets/ListBox.cpp
    Widgets/MenuBar.cpp
    Widgets/MessageBox.cpp
    Widgets/Panel.cpp
    Widgets/Picture.cpp
    Widgets/ProgressBar.cpp
    Widgets/RadioButton.cpp
    Widgets/RadioButtonGroup.cpp
    Widgets/RangeSlider.cpp
    Widgets/ScrollablePanel.cpp
    Widgets/Scrollbar.cpp
    Widgets/Slider.cpp
    Widgets/SpinButton.cpp
    Widgets/Tabs.cpp
    Widgets/TextBox.cpp
    Widgets/VerticalLayout.cpp
)

if (TGUI_OPTIMIZE_SINGLE_BUILD OR TGUI_OPTIMIZE_TESTS_SINGLE_BUILD)
    list(LENGTH TEST_SOURCES fileCount)
    if (TGUI_OPTIMIZE_SINGLE_BUILD_THREADS)
        set(threads ${TGUI_OPTIMIZE_SINGLE_BUILD_THREADS})
    else()
        include(ProcessorCount)
        ProcessorCount(threads)
    endif()

    if (${threads} LESS ${fileCount})
        math(EXPR threads "(${threads} / 2) + 1")
        math(EXPR fileCountMinusOne "${fileCount} - 1")
        math(EXPR threadsMinusOne "${threads} - 1")

        set(NEW_TEST_SOURCES "")
        foreach(i RANGE 0 ${threadsMinusOne})
            set(OutputFile "${CMAKE_CURRENT_SOURCE_DIR}/CombinedSources-Tests-${i}.cpp")
            file(WRITE "${OutputFile}" "")
            foreach(j RANGE ${i} ${fileCountMinusOne} ${threads})
                list(GET TEST_SOURCES ${j} inputFile)
                file(READ ${inputFile} CONTENTS)
                file(APPEND "${OutputFile}" "${CONTENTS}")
                list(APPEND NEW_TEST_SOURCES "${OutputFile}")
            endforeach()
        endforeach()

        set(TEST_SOURCES ${NEW_TEST_SOURCES})
    endif()
endif()

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tests PRIVATE tgui)

tgui_set_global_compile_flags(tests)
tgui_set_stdlib(tests)

if(TGUI_OPTIMIZE_SINGLE_BUILD AND TGUI_COMPILER_MSVC)
    target_compile_options(tests PRIVATE /bigobj)
endif()

# Copy the resources folder to the build directory to execute the tests without installing them
add_custom_command(TARGET tests
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/expected $<TARGET_FILE_DIR:tests>/expected
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/resources $<TARGET_FILE_DIR:tests>/resources
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/themes/Knob $<TARGET_FILE_DIR:tests>/resources/Knob
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/themes/Black.png $<TARGET_FILE_DIR:tests>/resources
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/themes/Black.txt $<TARGET_FILE_DIR:tests>/resources)

# Add the install rule for the executable
install(TARGETS tests
        RUNTIME DESTINATION ${TGUI_MISC_INSTALL_PREFIX}/tests/ COMPONENT tests
        BUNDLE DESTINATION ${TGUI_MISC_INSTALL_PREFIX}/tests/ COMPONENT tests)

# Install the resources next to the test executable
install(DIRECTORY "${PROJECT_SOURCE_DIR}/tests/resources"
        DESTINATION "${TGUI_MISC_INSTALL_PREFIX}/tests"
        COMPONENT tests)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/themes/"
        DESTINATION "${TGUI_MISC_INSTALL_PREFIX}/tests/resources"
        COMPONENT tests)
