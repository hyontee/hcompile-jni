LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := brbonus
LOCAL_SRC_FILES := brbonus.cpp padding.S
LOCAL_CPPFLAGS := -std=c++17 -fexceptions -frtti
LOCAL_C_INCLUDES := $(LOCAL_PATH)/vendor/Dobby
LOCAL_LDLIBS := -llog -ldl -landroid
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    LOCAL_STATIC_LIBRARIES := dobby64
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_STATIC_LIBRARIES := dobby32
endif
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dobby64
LOCAL_SRC_FILES := vendor/Dobby/arm64-v8a/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dobby32
LOCAL_SRC_FILES := vendor/Dobby/armeabi-v7a/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)
