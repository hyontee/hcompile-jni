LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := blackhook
LOCAL_SRC_FILES := hook.cpp
LOCAL_CPPFLAGS := -std=c++17 -fno-exceptions -fno-rtti
LOCAL_LDLIBS := -llog -ldl

include $(BUILD_SHARED_LIBRARY)
