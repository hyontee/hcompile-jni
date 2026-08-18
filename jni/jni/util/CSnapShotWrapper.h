//
// Created by Error on 18.10.2024.
//

#ifndef BRSELL_CSNAPSHOTWRAPPER_H
#define BRSELL_CSNAPSHOTWRAPPER_H
#include "../main.h"
#include "../game/game.h"
#include "../game/RW/RenderWare.h"
#include "util/CJavaWrapper.h"
#include <mutex>
#include <queue>
static class CSnapShotWrapper {

    enum SnapType{
        SNAP_PED,
        SNAP_CAR,
        SNAP_OBJ,
        SNAP_PLATE,
        SNAP_TEXTURE,
        SNAP_PLAYER
    };
public:
    static struct QueueItem{
        static uint8_t type;
        static int id;
        static jobject ImageView;
        static VECTOR vecOffset;
        static float zoom;
        static int color1;
        static int color2;
    };
    static struct PlateItem{
        static uint32_t dwType;
        static  char szNumber[30];
        static char szRegion[10];
    };
	static jmethodID constructorID; 
    static jclass clazz;
    static std::mutex queueMutex;
    static bool isProcessing;
	static void AddToQueue(QueueItem item);
	static std::queue<QueueItem> itemQueue;
    static QueueItem queueItem;
    static jbyteArray ConvertTextureToByte(RwTexture * texture, bool isTexture);
    static void SetToImageView(jbyteArray jByteArray, uint32_t width, uint32_t height, jobject imageview);
    static void Process();
    static void startProcess(jobject image_view,
                             int type, int modelid,
                             int color1, int color2,
                             float zoom,
                             float off_set_x,
                             float off_set_y,
                             float off_set_z);
};

#endif //BRSELL_CSNAPSHOTWRAPPER_H
