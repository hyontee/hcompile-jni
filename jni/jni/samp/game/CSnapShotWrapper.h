//
// Created by Error on 18.10.2024.
//

#ifndef BRSELL_CSNAPSHOTWRAPPER_H
#define BRSELL_CSNAPSHOTWRAPPER_H
#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
#include "util/CJavaWrapper.h"
#include <mutex>
#include <queue>
#include <thread>
static class CSnapShotWrapper {

    CSnapShotWrapper() {}

    enum SnapType{
        SNAP_PED,
        SNAP_CAR,
        SNAP_OBJ,
        SNAP_PLATE,
        SNAP_TEXTURE,
        SNAP_PLAYER
    };
public:

    static struct PlateItem{
        static uint32_t dwType;
        static  char szNumber[30];
        static char szRegion[10];
    };
    static jclass clazz;

    static std::mutex queueMutex;

    static bool isProcessing;
    static jbyteArray ConvertTextureToByte(RwTexture * texture, bool isTexture);
    static void SetToImageView(uint32_t  id, jbyteArray jByteArray, uint32_t width, uint32_t height, jobject imageview);
    static void Process();


    class QueueItem{
    public:
         uint8_t type;
         uint32_t jid;
         int id;
         jobject ImageView;
         CVector vecRot;
         CVector vecOffset;
         float zoom;
         int color1;
         int color2;
    public:
         QueueItem();
    };

    static QueueItem *queueItem;

    static std::queue<QueueItem*> list;

};


#endif //BRSELL_CSNAPSHOTWRAPPER_H
