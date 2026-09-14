LOCAL_PATH := $(call my-dir)

# Prebuilt libdobby
include $(CLEAR_VARS)
LOCAL_MODULE := libdobby
LOCAL_SRC_FILES := vendor/Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# Plugin module
include $(CLEAR_VARS)
LOCAL_MODULE := plugin

# Исходники
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# Include paths
LOCAL_C_INCLUDES := $(LOCAL_PATH)/vendor/Dobby/include \
                    $(LOCAL_PATH)/vendor/RakNet \
                    $(LOCAL_PATH)/vendor/RakNet/SAMP

# Статическая библиотека
LOCAL_STATIC_LIBRARIES := libdobby

# Системные библиотеки
LOCAL_LDLIBS := -llog

# Флаги
LOCAL_CPPFLAGS := -Wno-error=format-security -std=c++17 -O3 -fvisibility=hidden

include $(BUILD_SHARED_LIBRARY)
