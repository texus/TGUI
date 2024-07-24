cmake_minimum_required(VERSION 3.16)
project(TGUI-CMake-test)

add_executable(TguiTest main.cpp)

find_package(TGUI 1 REQUIRED)
target_link_libraries(TguiTest PRIVATE TGUI::TGUI)

set_target_properties(TguiTest PROPERTIES CXX_STANDARD 20)
set_target_properties(TguiTest PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(TguiTest PROPERTIES CXX_EXTENSIONS OFF)
