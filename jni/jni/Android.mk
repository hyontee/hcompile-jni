LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := plugin
LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src
LOCAL_CPPFLAGS := -std=c++17 -O2 -fPIC
LOCAL_LDFLAGS := -shared -Wl,-z,max-page-size=0x4000,-z,common-page-size=0x4000

include $(BUILD_SHARED_LIBRARY)