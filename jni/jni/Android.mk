LOCAL_PATH := $(call my-dir)

# Dobby
include $(CLEAR_VARS)
LOCAL_MODULE := libdobby
LOCAL_SRC_FILES := vendor/Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# Plugin
include $(CLEAR_VARS)
LOCAL_MODULE := plugin

LOCAL_SRC_FILES := plugin.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/vendor/Dobby/include

LOCAL_STATIC_LIBRARIES := libdobby
LOCAL_LDLIBS := -llog

LOCAL_CPPFLAGS := -std=c++17 -O3 -fvisibility=hidden -Wno-error=format-security

include $(BUILD_SHARED_LIBRARY)
