LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libopenal
LOCAL_SRC_FILES := vendor/openal/libopenal.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libbass
LOCAL_SRC_FILES := libbass.so

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libopus
LOCAL_SRC_FILES := vendor/opus/libopus.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libenet
LOCAL_SRC_FILES := vendor/enet/libenet.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

include $(CLEAR_VARS)

APP_OPTIM := RELEASE

LOCAL_MODULE := NARKOTA
LOCAL_LDLIBS := -llog -landroid

# NARKOTA
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/RW/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gtare/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/network/*.cpp)

FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/ini/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/md5.cpp)

FILE_LIST += $(wildcard $(LOCAL_PATH)/cryptors/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/*.c)

LOCAL_STATIC_LIBRARIES := libopenal libopus libenet
LOCAL_SHARED_LIBRARIES := libbass

APP_OPTIM := release

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions
include $(BUILD_SHARED_LIBRARY)