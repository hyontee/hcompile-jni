#include <android_native_app_glue.h>
#include <android/log.h>
#include <string.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "NDKApp", __VA_ARGS__)

void android_main(struct android_app* app) {
    LOGI("Приложение запущено!");

    app->onAppCmd = [](struct android_app* app, int32_t cmd) {
        if (cmd == APP_CMD_INIT_WINDOW) {
            ANativeWindow* window = app->window;
            ANativeWindow_Buffer buffer;
            
            if (ANativeWindow_lock(window, &buffer, nullptr) == 0) {
                uint32_t* pixels = (uint32_t*)buffer.bits;
                for (int y = 0; y < buffer.height; y++) {
                    for (int x = 0; x < buffer.width; x++) {
                        pixels[y * buffer.stride + x] = 0xFFFFFFFF;
                    }
                }

                const char* text = "Привет!";
                int textLen = strlen(text);
                int startX = buffer.width / 2 - textLen * 8;
                int startY = buffer.height / 2;

                for (int y = startY - 10; y < startY + 10; y++) {
                    for (int x = startX; x < startX + textLen * 16; x++) {
                        if (x >= 0 && x < buffer.width && y >= 0 && y < buffer.height) {
                            pixels[y * buffer.stride + x] = 0xFF000000;
                        }
                    }
                }

                ANativeWindow_unlockAndPost(window);
                LOGI("Текст нарисован!");
            }
        }
    };

    while (1) {
        struct android_poll_source* source;
        int ident = ALooper_pollAll(-1, nullptr, nullptr, (void**)&source);
        if (source) {
            source->process(app, source);
        }
        if (app->destroyRequested) {
            LOGI("Приложение завершено");
            break;
        }
    }
}