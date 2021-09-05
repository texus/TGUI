NDK_TOOLCHAIN_VERSION := clang
APP_STL := c++_static
APP_MODULES := sfml-activity-d tgui-activity-d tgui-example
APP_OPTIM := debug
APP_CPPFLAGS += -fexceptions -frtti -std=c++14 -g -ggdb -O0
