LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := cheat
LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CPP_FEATURES := exceptions rtti
LOCAL_LDLIBS := -llog -lm -ldl
LOCAL_CFLAGS := -O2 -fvisibility=hidden -fPIC
LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions

include $(BUILD_SHARED_LIBRARY)