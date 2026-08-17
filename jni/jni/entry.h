#pragma once

#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <jni.h>

#include "vendor/RakNet/BitStream.h"

#include "vendor/Dobby/include/dobby.h"

#include "game/rw/rw.h"
#include "game/hooks.h"

#include "app.h"
#include "plugin.h"

void* hack_thread(void* args);