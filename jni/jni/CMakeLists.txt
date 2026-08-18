cmake_minimum_required(VERSION 3.12)
project(samp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    add_definitions(-DVER_x32=true)
elseif(${ANDROID_ABI} STREQUAL "arm64-v8a")
    add_definitions(-DVER_x32=false)
endif()

# Add source files
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/**/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/**/**/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/**/**/**/**/**/**/**/**/**/*.c*
        ${CMAKE_CURRENT_LIST_DIR}/../santrope-tea-gtasa/encryption/*.c*
)

list(APPEND SOURCES ${CMAKE_CURRENT_LIST_DIR}/java_systems/CTabletMusic.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/CTabletMusic_JNI.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/CPassportOverlay.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/CMvdIdOverlay.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/badges.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/Statistics.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/CFamilyMenu.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/java_systems/CFamilyAction.cpp)
list(APPEND SOURCES${CMAKE_CURRENT_LIST_DIR}/vehicle_utils.cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/java_systems)

# Add include directories
include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/util
        ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui
        ${CMAKE_CURRENT_LIST_DIR}/vendor/
        ${CMAKE_CURRENT_LIST_DIR}/game/RW/
        ${CMAKE_CURRENT_LIST_DIR}/game/Core/
        ${CMAKE_CURRENT_LIST_DIR}/game/MobileMenu/
)

add_library(samp SHARED ${SOURCES})
target_link_libraries(samp log EGL GLESv3 opus shadowhook::shadowhook ${CMAKE_CURRENT_SOURCE_DIR}/vendor/bass/libs/${ANDROID_ABI}/libbass.so)

if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -s -g -fvisibility=default -pthread -Wall -fpack-struct=1  -O3 -fexceptions")
elseif(${ANDROID_ABI} STREQUAL "arm64-v8a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -s -g -fvisibility=default -pthread -Wall  -O3 -fexceptions")
endif()
