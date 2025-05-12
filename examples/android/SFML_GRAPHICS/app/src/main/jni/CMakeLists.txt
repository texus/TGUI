cmake_minimum_required(VERSION 3.22)
project(tgui-example CXX)

set(sfml_version 3)  # Choose whether you want to use SFML 2 or SFML 3

set(SFML_DIR "${CMAKE_ANDROID_NDK}/sources/third_party/sfml/lib/${CMAKE_ANDROID_ARCH_ABI}/cmake/SFML")
set(TGUI_DIR "${CMAKE_ANDROID_NDK}/sources/third_party/tgui/lib/${CMAKE_ANDROID_ARCH_ABI}/cmake/TGUI")

# Create the libapp.so library that contains the application's c++ code
add_library(app SHARED)

# Find and link the dependencies
if (sfml_version EQUAL 3)
    target_sources(app PRIVATE main-sfml3.cpp)

    find_package(SFML 3 REQUIRED COMPONENTS Graphics Main CONFIG)
    target_link_libraries(app PRIVATE SFML::Graphics)

    target_link_libraries(app PUBLIC
      -Wl,--whole-archive
      SFML::Main
      -Wl,--no-whole-archive
    )
else()
    target_sources(app PRIVATE main-sfml2.cpp)

    find_package(SFML 2 REQUIRED COMPONENTS graphics window system CONFIG)
    target_link_libraries(app PRIVATE sfml-graphics sfml-window sfml-system)

    target_link_libraries(app PUBLIC
      -Wl,--whole-archive
      sfml-main
      -Wl,--no-whole-archive
    )
endif()

find_package(TGUI 1 REQUIRED CONFIG)
target_link_libraries(app PRIVATE TGUI::TGUI android log)
