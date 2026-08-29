LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := samp_utils
LOCAL_SRC_FILES := plugin.cpp
LOCAL_CFLAGS    := -O2 -w

include $(BUILD_SHARED_LIBRARY)
