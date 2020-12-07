LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SRC_FILES := example.cpp

LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++14

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf tgui

LOCAL_LDLIBS := -lGLESv3 -llog

include $(BUILD_SHARED_LIBRARY)
