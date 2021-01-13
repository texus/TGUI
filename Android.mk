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
        $(wildcard $(LOCAL_PATH)/src/Backends/SDL/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Loading/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Renderers/*.cpp) \
        $(wildcard $(LOCAL_PATH)/src/Widgets/*.cpp) \
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

LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++14

LOCAL_LDLIBS := -landroid -lGLESv3 -llog

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

LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++14

LOCAL_LDLIBS := 
LOCAL_EXPORT_LDLIBS := -landroid -lGLESv3 -llog

include $(BUILD_STATIC_LIBRARY)

