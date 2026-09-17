LOCAL_PATH := $(call my-dir)

# ============================================================
# ShadowHook: локальный prebuilt для ndk-build
# Положи сюда:
# shadowhook/include/shadowhook.h
# shadowhook/prebuilt/armeabi-v7a/libshadowhook.so
# ============================================================
include $(CLEAR_VARS)
LOCAL_MODULE := shadowhook
LOCAL_SRC_FILES := shadowhook/prebuilt/$(TARGET_ARCH_ABI)/libshadowhook.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/shadowhook/include
include $(PREBUILT_SHARED_LIBRARY)

# ============================================================
# Static libraries
# ============================================================
include $(CLEAR_VARS)
LOCAL_MODULE := openal
LOCAL_SRC_FILES := vendor/openal/libopenal.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := opus
LOCAL_SRC_FILES := vendor/opus/libopus.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := enet
LOCAL_SRC_FILES := vendor/enet/libenet.a
include $(PREBUILT_STATIC_LIBRARY)

# ============================================================
# Main samp library
# ============================================================
include $(CLEAR_VARS)
LOCAL_MODULE := samp

LOCAL_CPPFLAGS := -std=c++20 -fexceptions -frtti -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O3
LOCAL_CFLAGS   := -fexceptions -fvisibility=hidden -fpack-struct=1 -O3
LOCAL_LDFLAGS  := -Wl,--gc-sections

# Все include-директории проекта.
LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH) -type d)

# В архиве исходников нет santrope-tea-gtasa.
# Если папка лежит рядом с jni, этот путь будет доступен.
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../santrope-tea-gtasa/encryption

# Все .cpp/.c из текущего проекта, кроме сторонних prebuilt.
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $2,$d))
LOCAL_SRC_FILES := $(call rwildcard,$(LOCAL_PATH)/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,$(LOCAL_PATH)/,*.c)

# Убираем путь $(LOCAL_PATH)/ из абсолютных путей.
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(LOCAL_SRC_FILES))

LOCAL_STATIC_LIBRARIES := openal opus enet
LOCAL_SHARED_LIBRARIES := shadowhook

LOCAL_LDLIBS := \
    -llog \
    -landroid \
    -lOpenSLES \
    -lGLESv2 \
    -lEGL \
    -ldl \
    -lz \
    -lm

include $(BUILD_SHARED_LIBRARY)
