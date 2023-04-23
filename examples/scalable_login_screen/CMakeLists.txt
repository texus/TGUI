tgui_add_example(scalable_login_screen SOURCES ScalableLoginScreen.cpp)

add_custom_command(TARGET scalable_login_screen
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/xubuntu_bg_aluminium.jpg" "$<TARGET_FILE_DIR:scalable_login_screen>/"
                   VERBATIM)
