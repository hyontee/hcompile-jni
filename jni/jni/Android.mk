LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := apk
LOCAL_SRC_FILES := main.cpp

LOCAL_C_INCLUDES := $(NDK_ROOT)/sources/android/native_app_glue

LOCAL_LDLIBS    := -llog -landroid
LOCAL_STATIC_LIBRARIES := android_native_app_glue

# без этого линкер может выкинуть точку входа NativeActivity как "неиспользуемую"
LOCAL_LDFLAGS := -u ANativeActivity_onCreate

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
