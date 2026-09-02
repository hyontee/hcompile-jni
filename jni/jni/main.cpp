#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <cstring>
#include <cstdint>

#define LOG_TAG "privet"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ------- Простой пиксельный шрифт 5x7 -------
// Кириллические буквы визуально совпадают с этими начертаниями:
// п=П(как греч. Pi), р=Р(как лат. P), и=И, в=В(как лат. B), е=Е(как лат. E), т=Т(как лат. T)
struct Glyph { uint8_t rows[7]; }; // каждый байт: биты 4..0 = пиксели слева направо

static const Glyph GL_P  = {{0b11111,0b10001,0b10001,0b10001,0b10001,0b10001,0b10001}}; // п
static const Glyph GL_R  = {{0b11110,0b10001,0b10001,0b11110,0b10000,0b10000,0b10000}}; // р
static const Glyph GL_I  = {{0b10001,0b11001,0b10101,0b10011,0b10001,0b10001,0b10001}}; // и
static const Glyph GL_V  = {{0b11110,0b10001,0b10001,0b11110,0b10001,0b10001,0b11110}}; // в
static const Glyph GL_E  = {{0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b11111}}; // е
static const Glyph GL_T  = {{0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100}}; // т
static const Glyph GL_EX = {{0b00100,0b00100,0b00100,0b00100,0b00100,0b00000,0b00100}}; // !
static const Glyph GL_SP = {{0,0,0,0,0,0,0}};

static const Glyph* glyphFor(char c) {
    switch (c) {
        case 'p': return &GL_P;
        case 'r': return &GL_R;
        case 'i': return &GL_I;
        case 'v': return &GL_V;
        case 'e': return &GL_E;
        case 't': return &GL_T;
        case '!': return &GL_EX;
        default:  return &GL_SP;
    }
}

// "привет!" закодировано латинскими буквами-заменителями (см. таблицу выше)
static const char* WORD = "privet!";

static void drawGlyph(uint32_t* pixels, int32_t stride, int ox, int oy,
                       const Glyph& g, int scale, uint32_t color) {
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (g.rows[row] & (1 << (4 - col))) {
                for (int sy = 0; sy < scale; ++sy) {
                    for (int sx = 0; sx < scale; ++sx) {
                        int x = ox + col * scale + sx;
                        int y = oy + row * scale + sy;
                        pixels[y * stride + x] = color;
                    }
                }
            }
        }
    }
}

static void drawText(ANativeWindow_Buffer* buf, const char* text, int scale, uint32_t color) {
    int len = (int)strlen(text);
    int glyphW = (5 + 1) * scale; // +1 колонка отступа между буквами
    int glyphH = 7 * scale;
    int totalW = len * glyphW;
    int ox = (buf->width - totalW) / 2;
    int oy = (buf->height - glyphH) / 2;
    if (ox < 0) ox = 0;
    if (oy < 0) oy = 0;

    uint32_t* pixels = static_cast<uint32_t*>(buf->bits);
    for (int i = 0; i < len; ++i) {
        drawGlyph(pixels, buf->stride, ox + i * glyphW, oy, *glyphFor(text[i]), scale, color);
    }
}

static void redraw(android_app* app) {
    if (!app->window) return;

    ANativeWindow_setBuffersGeometry(app->window, 0, 0, WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(app->window, &buffer, nullptr) != 0) {
        LOGI("ANativeWindow_lock failed");
        return;
    }

    // фон
    uint32_t bg = 0xFF202020;
    uint32_t* px = static_cast<uint32_t*>(buffer.bits);
    for (int32_t y = 0; y < buffer.height; ++y) {
        for (int32_t x = 0; x < buffer.width; ++x) {
            px[y * buffer.stride + x] = bg;
        }
    }

    int scale = buffer.width / 120;
    if (scale < 4) scale = 4;

    drawText(&buffer, WORD, scale, 0xFFFFFFFF);

    ANativeWindow_unlockAndPost(app->window);
}

static void handleCmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            redraw(app);
            break;
        case APP_CMD_WINDOW_RESIZED:
        case APP_CMD_CONTENT_RECT_CHANGED:
            redraw(app);
            break;
        default:
            break;
    }
}

void android_main(android_app* app) {
    app->onAppCmd = handleCmd;

    int events;
    android_poll_source* source;
    while (true) {
        while (ALooper_pollAll(-1, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }
            if (app->destroyRequested != 0) {
                return;
            }
        }
    }
}
