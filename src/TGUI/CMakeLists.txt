set(TGUI_SRC
    Animation.cpp
    BoxLayout.cpp
    Clipboard.cpp
    Color.cpp
    Container.cpp
    Font.cpp
    Global.cpp
    Gui.cpp
    HorizontalLayout.cpp
    Layout.cpp
    Signal.cpp
    Texture.cpp
    TextureManager.cpp
    Transformable.cpp
    VerticalLayout.cpp
    Widget.cpp
    Loading/DataIO.cpp
    Loading/Deserializer.cpp
    Loading/ObjectConverter.cpp
    Loading/Serializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
    Loading/WidgetConverter.cpp
    Loading/WidgetLoader.cpp
    Loading/WidgetSaver.cpp
    Widgets/Button.cpp
    Widgets/Canvas.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ComboBox.cpp
    Widgets/EditBox.cpp
    Widgets/Grid.cpp
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
    Widgets/Tab.cpp
    Widgets/TextBox.cpp
    Widgets/ToolTip.cpp
    Widgets/devel/RichTextLabel.cpp
    Widgets/devel/TableItem.cpp
    Widgets/devel/TableRow.cpp
    Widgets/devel/Table.cpp
)

# Determine library suffixes depending on static/shared configuration
if(TGUI_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${TGUI_SRC})
    set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )

    # Set the version and soversion of the target (for compatible systems -- mostly Linuxes)
    # Except for Android which strips soversion suffixes
    if(NOT SFML_OS_ANDROID)
        set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
        set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
    endif()

    # The library should be linked to sfml, unless you are on linux
    if (NOT SFML_OS_LINUX)
        target_link_libraries( ${PROJECT_NAME} ${TGUI_EXT_LIBS} )

        # on Windows/gcc get rid of "lib" prefix for shared libraries,
        # and transform the ".dll.a" suffix into ".a" for import libraries
        if (SFML_OS_WINDOWS AND SFML_COMPILER_GCC)
            set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
            set_target_properties(${PROJECT_NAME} PROPERTIES IMPORT_SUFFIX ".a")
        endif()
    endif()

else()
    add_library(${PROJECT_NAME} STATIC ${TGUI_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX -s)
    set_target_properties(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX -s)
    set_target_properties(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX -s)
endif()

# Enable automatic reference counting on iOS
if (SFML_OS_IOS)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

# for gcc >= 4.0 on Windows, apply the TGUI_USE_STATIC_STD_LIBS option if it is enabled
if(SFML_OS_WINDOWS AND SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4")
    if(TGUI_USE_STATIC_STD_LIBS AND NOT SFML_COMPILER_GCC_TDM)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
    elseif(NOT TGUI_USE_STATIC_STD_LIBS AND SFML_COMPILER_GCC_TDM)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-shared-libgcc -shared-libstdc++")
    endif()
endif()

# if using gcc >= 4.0 or clang >= 3.0 on a non-Windows platform, we must hide public symbols by default
# (exported ones are explicitely marked)
if(NOT SFML_OS_WINDOWS AND ((SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4") OR (SFML_COMPILER_CLANG AND NOT SFML_CLANG_VERSION VERSION_LESS "3")))
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fvisibility=hidden)
endif()

# Build frameworks or dylibs
if(SFML_OS_MACOSX AND TGUI_SHARED_LIBS)
    if(TGUI_BUILD_FRAMEWORK)
        # Adapt target to build frameworks instead of dylibs
        set_target_properties(${PROJECT_NAME} PROPERTIES
                              FRAMEWORK TRUE
                              FRAMEWORK_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                              MACOSX_FRAMEWORK_IDENTIFIER org.tgui.${PROJECT_NAME}
                              MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                              MACOSX_FRAMEWORK_BUNDLE_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

        # Install the header files to the framework
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Headers
                           COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include/TGUI $<TARGET_FILE_DIR:${PROJECT_NAME}>/Headers
                           COMMAND ${CMAKE_COMMAND} -E create_symlink Versions/${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}/Headers $<TARGET_FILE_DIR:${PROJECT_NAME}>/../../Headers)

        # The framework has to be with a capital letter (because it includes the header files which must be found in a "TGUI" directory)
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME TGUI)
    endif()

    # Adapt install directory to allow distributing dylibs/frameworks in user’s frameworks/application bundle
    set_target_properties(${PROJECT_NAME} PROPERTIES
                          BUILD_WITH_INSTALL_RPATH 1
                          INSTALL_NAME_DIR "@rpath")
endif()

# Install library
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin COMPONENT bin
        LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT bin
        ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT devel
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_FRAMEWORK_PREFIX} COMPONENT bin
)

# Build tgui-activity on android
if (SFML_OS_ANDROID)
    add_subdirectory(Android)
endif()
