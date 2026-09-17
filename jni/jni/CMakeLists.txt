cmake_minimum_required(VERSION 3.12)
project(samp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O3 -s")

include_directories(
        ${CMAKE_SOURCE_DIR}/vendor
        ${CMAKE_SOURCE_DIR}/../santrope-tea-gtasa/encryption
)

find_package(shadowhook REQUIRED CONFIG)

add_library(libopenal STATIC IMPORTED)
set_target_properties(libopenal PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/openal/libopenal.a)

add_library(libopus STATIC IMPORTED)
set_target_properties(libopus PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/opus/libopus.a)

add_library(libenet STATIC IMPORTED)
set_target_properties(libenet PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/enet/libenet.a)

file(GLOB_RECURSE FILE_LIST
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/game/*.cpp
        ${CMAKE_SOURCE_DIR}/game/Pipelines/*.cpp
        ${CMAKE_SOURCE_DIR}/game/Pipelines/CustomBuilding/*.cpp
        ${CMAKE_SOURCE_DIR}/game/Pipelines/CustomCar/*.cpp
        ${CMAKE_SOURCE_DIR}/game/Core/*.cpp
        ${CMAKE_SOURCE_DIR}/game/Entity/*.cpp
        ${CMAKE_SOURCE_DIR}/net/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        ${CMAKE_SOURCE_DIR}/game/RW/RenderWare.cpp
        ${CMAKE_SOURCE_DIR}/gui/*.cpp
        ${CMAKE_SOURCE_DIR}/voice/*.cpp
        ${CMAKE_SOURCE_DIR}/cryptors/*.cpp
        ${CMAKE_SOURCE_DIR}/gtasa/*.cpp
        ${CMAKE_SOURCE_DIR}/../santrope-tea-gtasa/encryption/*.cpp
        ${CMAKE_SOURCE_DIR}/../santrope-tea-gtasa/encryption/*.c
        ${CMAKE_SOURCE_DIR}/vendor/ini/*.cpp
        ${CMAKE_SOURCE_DIR}/vendor/RakNet/*.cpp
        ${CMAKE_SOURCE_DIR}/vendor/RakNet/SAMP/*.cpp
        ${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp
        ${CMAKE_SOURCE_DIR}/vendor/hash/md5.cpp
)

find_package(shadowhook REQUIRED CONFIG)

add_library(samp SHARED ${FILE_LIST})

target_link_libraries(samp
        libopenal
        libopus
        libenet
        log
        OpenSLES
        GLESv2
        EGL
        shadowhook::shadowhook
)