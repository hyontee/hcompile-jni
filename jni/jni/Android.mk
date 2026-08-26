LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libopenal 
LOCAL_SRC_FILES := vendor/openal/libopenal.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libopus 
LOCAL_SRC_FILES := vendor/opus/libopus.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libenet 
LOCAL_SRC_FILES := vendor/enet/libenet.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := samp
LOCAL_LDLIBS := -llog -lOpenSLES

LOCAL_C_INCLUDES += $(wildcard $(LOCAL_PATH)/vendor/)
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/compat
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../santrope-tea-gtasa/encryption

# samp
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/RW/RenderWare.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/voice/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/cryptors/*.cpp)
FILE_LIST += $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/CTEA.cpp
FILE_LIST += $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/CTinyEncrypt.cpp
FILE_LIST += $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/CXTEA3.cpp
FILE_LIST += $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/aes.c

# vendor
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/ini/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/md5.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_STATIC_LIBRARIES := libopenal libopus libenet
LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions
include $(BUILD_SHARED_LIBRARY)