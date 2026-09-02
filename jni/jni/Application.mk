# Собираем только под arm64-v8a (под неё же и лежит папка lib/arm64-v8a)
APP_ABI := arm64-v8a

APP_PLATFORM := android-24
APP_STL := c++_static
APP_OPTIM := release

# Имя модуля должно совпадать с LOCAL_MODULE из Android.mk
APP_MODULES := apk
