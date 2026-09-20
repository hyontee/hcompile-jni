LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := guiinspector
LOCAL_SRC_FILES := guiinspector.cpp
LOCAL_CPPFLAGS := -std=c++17 -fexceptions -frtti
LOCAL_LDLIBS := -llog -ldl
include $(BUILD_SHARED_LIBRARY)
