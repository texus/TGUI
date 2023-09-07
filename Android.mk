# This file is used to build TGUI on Android with the SDL-TTF-GLES2 backend.
# For the SFML_GRAPHICS or SDL_RENDERER backends, CMake is used and this file can be ignored.
# No other backends are currently supported.

LOCAL_PATH := $(call my-dir)

###########################
#
# TGUI shared library
#
###########################

include $(CLEAR_VARS)

LOCAL_MODULE := tgui

SDL_PATH := $(LOCAL_PATH)/../SDL
SDL_TTF_PATH := $(LOCAL_PATH)/../SDL_ttf

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(SDL_PATH)/include $(SDL_TTF_PATH)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    $(subst $(LOCAL_PATH)/,, \
        $(wildcard $(LOCAL_PATH)/src/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Loading/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Renderers/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Widgets/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/Font/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/Font/SDL_ttf/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/Renderer/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/Renderer/GLES2/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/Window/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/Window/SDL/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Backend/SDL-TTF-GLES2.cpp) \
    )

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf

LOCAL_CFLAGS += \
    -Wall \
    -Wextra \
    -Wshadow \
    -Wnon-virtual-dtor \
    -Wcast-align \
    -Wunused \
    -Woverloaded-virtual \
    -Wpedantic \
    -Wdouble-promotion \
    -Wformat=2

LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_LDLIBS := -landroid -lGLESv1_CM -lGLESv2 -llog

# Don't strip debug symbols for debug libraries
ifeq ($(NDK_DEBUG),1)
    cmd-strip :=
endif

include $(BUILD_SHARED_LIBRARY)


###########################
#
# TGUI static library
#
###########################

LOCAL_MODULE := tgui-s

LOCAL_MODULE_FILENAME := libtgui-s

LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_LDLIBS := 
LOCAL_EXPORT_LDLIBS := -landroid -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_STATIC_LIBRARY)

