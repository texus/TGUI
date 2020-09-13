LOCAL_PATH := $(call my-dir)

# TGUI library in release mode
include $(CLEAR_VARS)
LOCAL_MODULE := tgui
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libtgui.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := sfml-graphics sfml-window sfml-system

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_SHARED_LIBRARY)
endif

# TGUI library in debug mode
include $(CLEAR_VARS)
LOCAL_MODULE := tgui-d
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libtgui-d.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := sfml-graphics-d sfml-window-d sfml-system-d

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_SHARED_LIBRARY)
endif


# TGUI Activity library in release mode
include $(CLEAR_VARS)
LOCAL_MODULE := tgui-activity
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libtgui-activity.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_SHARED_LIBRARY)
endif

# TGUI Activity library in debug mode
include $(CLEAR_VARS)
LOCAL_MODULE := tgui-activity-d
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libtgui-activity-d.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_SHARED_LIBRARY)
endif


$(call import-module, third_party/sfml)
