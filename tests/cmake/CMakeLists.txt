cmake_minimum_required(VERSION 3.5)
project(TGUI-CMake-test)

add_executable(TguiTest main.cpp)

find_package(SFML 2 COMPONENTS graphics window system REQUIRED)
find_package(TGUI REQUIRED)
target_link_libraries(TguiTest PRIVATE tgui sfml-graphics)

set_target_properties(TguiTest PROPERTIES CXX_STANDARD 14)
set_target_properties(TguiTest PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(TguiTest PROPERTIES CXX_EXTENSIONS OFF)
