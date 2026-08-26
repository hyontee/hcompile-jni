# Android NDK r16 configuration.
# The bundled prebuilt libraries (OpenAL/Opus/ENet/BASS) are ARMv7 (32-bit),
# so this project must be built for armeabi-v7a.
APP_ABI := armeabi-v7a
APP_PLATFORM := android-14
APP_MODULES := samp
APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION := 4.9
