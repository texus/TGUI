tgui_add_example(many_different_widgets SOURCES ManyDifferentWidgets.cpp)

add_custom_command(TARGET many_different_widgets
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/RedBackground.jpg" "$<TARGET_FILE_DIR:many_different_widgets>/"
                   VERBATIM)

add_custom_command(TARGET many_different_widgets
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           "${CMAKE_CURRENT_SOURCE_DIR}/../../themes"
                           "$<TARGET_FILE_DIR:many_different_widgets>/../../themes"
                   VERBATIM)
