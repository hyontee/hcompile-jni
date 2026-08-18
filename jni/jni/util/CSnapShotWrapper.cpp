
#include "CSnapShotWrapper.h"
#include "CSnapShots.h"
#include "../game/CCustomPlateManager.h"
#include <condition_variable>

std::condition_variable queueCondition;
extern CJavaWrapper* g_pJavaWrapper;
jclass CSnapShotWrapper::clazz = nullptr;

bool CSnapShotWrapper::isProcessing = false;
#include <queue>
uint8_t CSnapShotWrapper::QueueItem::type = 0;
int CSnapShotWrapper::QueueItem::id = 0;
jobject CSnapShotWrapper::QueueItem::ImageView = nullptr;
VECTOR CSnapShotWrapper::QueueItem::vecOffset;
float CSnapShotWrapper::QueueItem::zoom = 0;
int CSnapShotWrapper::QueueItem::color1 = 0;
int CSnapShotWrapper::QueueItem::color2 = 0;
std::queue<CSnapShotWrapper::QueueItem> CSnapShotWrapper::itemQueue;
uint32_t CSnapShotWrapper::PlateItem::dwType = 0;
char CSnapShotWrapper::PlateItem::szRegion[10];
char CSnapShotWrapper::PlateItem::szNumber[30];
#include "../game/snapshothelper.h"
extern CSnapShotHelper* pSnapShotHelper;
std::mutex CSnapShotWrapper::queueMutex;
CSnapShotWrapper::QueueItem CSnapShotWrapper::queueItem;
jbyteArray CSnapShotWrapper::ConvertTextureToByte(RwTexture * texture, bool isTexture) {
    JNIEnv* pEnv = g_pJavaWrapper->GetEnv();
    uint32_t dest = 512 * 512 * 4;
    jbyte* bytes = new jbyte[dest];
    uintptr_t textureBuffer = (uintptr_t)texture->raster;

    //(( void (*)(uintptr_t, int))(SA_FUNC(0x001D0958)))(*(uintptr_t *)(textureBuffer + *(uintptr_t *)(SA_ADDR(0x6B31FC)) + 0x18), isTexture);

    //(( void (*)(int, int, int, int, uintptr_t, uintptr_t, jbyte*))(SA_FUNC(0x1B4C20)))(0, 0, 512, 512, 0x1908, 0x1401, bytes);
	
    for (uint32_t i = 0; i < dest; i += 4) {
	//	if(i == 4){
			//Log("%d %d %d", (int)bytes[i], (int)bytes[i + 1], (int)bytes[i + 2]);
	//	}
		
        if (bytes[i] == 123 && bytes[i + 1] == -100 && bytes[i + 2] == -58) {
            bytes[i] = 0; 
            bytes[i + 1] = 0;
            bytes[i + 2] = 0;
			
            bytes[i + 3] = 0; 
        }
    }
	

    jbyteArray jByteArray = pEnv->NewByteArray(dest);
    pEnv->SetByteArrayRegion(jByteArray, 0, dest, bytes);
    delete bytes;

    return jByteArray;
}

void CSnapShotWrapper::SetToImageView(jbyteArray jByteArray, uint32_t width, uint32_t height, jobject imageview) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();
	
	if(imageview){
		jmethodID method = env->GetStaticMethodID(CSnapShotWrapper::clazz, "onNativeRendered", "(Landroid/widget/ImageView;[B)V");
		env->CallStaticVoidMethod(CSnapShotWrapper::clazz, method, imageview, jByteArray);
	}

}
void CSnapShotWrapper::Process() {
	std::unique_lock<std::mutex> lock(queueMutex);
	 if (!itemQueue.empty()) {
	  QueueItem queueItem2 = itemQueue.front();
	  

	  bool isTexture = false;
	  RwTexture *snapshot = nullptr;
	  jbyteArray bytes = nullptr;

	  uintptr_t *dwModelArray2 = (uintptr_t *)(SA_ADDR(0x91DCB8));
	  if (dwModelArray2[queueItem2.id]) {
		switch (queueItem2.type) {
		  case SNAP_PED:
			snapshot = CSnapShots::CreatePedSnapShot(
			  queueItem2.id, 0xFFFFFFFF, &queueItem2.vecOffset, queueItem2.zoom);
			isTexture = true;
			break;
		  case SNAP_CAR:
			snapshot = CSnapShots::CreateVehicleSnapShot(
			  queueItem2.id, 0xFFFFFFFF, &queueItem2.vecOffset, queueItem2.zoom,
			  queueItem2.color1, queueItem2.color2);
			isTexture = true;
			break;
		  case SNAP_OBJ:
			snapshot = CSnapShots::CreateObjectSnapShot(
			  queueItem2.id, 0xFFFFFFFF, &queueItem2.vecOffset, queueItem2.zoom);
			isTexture = true;
			break;
		  case SNAP_TEXTURE:
			snapshot = CSnapShots::CreateTextureSnapShot("vehiclelights128");
			isTexture = true;
			break;
		  default:
			snapshot = nullptr;
			isTexture = false;
			break;
		}
	  } else {
		if (queueItem2.type == SNAP_TEXTURE) {
		  snapshot = CSnapShots::CreateTextureSnapShot("vehiclelights128");
		  isTexture = true;
		} else {
		  snapshot = CSnapShots::CreateObjectSnapShot(18631, 0xFFFFFFFF, &queueItem2.vecOffset, 0.8f);
		  isTexture = true;
		}
	  }

	  if (snapshot) {
		bytes = CSnapShotWrapper::ConvertTextureToByte(snapshot, isTexture);
		if (bytes == nullptr) {
		  Log("Error converting texture to byte array.");
		} else {
		  try {
			CSnapShotWrapper::SetToImageView(bytes, snapshot->raster->width, snapshot->raster->height, queueItem2.ImageView);
		  } catch (const std::exception& e) {
			Log("Error setting texture to ImageView: %s", e.what());
		  }
		}
		*((uintptr_t *)snapshot + 0x15) = 0;
		try {
		  RwTextureDestroy(snapshot);
		} catch (const std::exception& e) {
		  Log("Error destroying texture: %s", e.what());
		}
	  } else {
		Log("No generated tex for snap. type = %d, model = %d", queueItem2.type, queueItem2.id);
	  }
		itemQueue.pop(); 
		
	 }
	 
	

}
void CSnapShotWrapper::AddToQueue(QueueItem item) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        itemQueue.push(item); // Добавляем элемент в очередь
    }
    queueCondition.notify_one(); // Уведомляем один поток, ожидающий в queueCondition
}
void
CSnapShotWrapper::startProcess(jobject image_view, int type, int modelid, int color1, int color2,
                               float zoom, float off_set_x, float off_set_y, float off_set_z) {

    CSnapShotWrapper::queueMutex.lock();
    Log("StartProcessng");
    CSnapShotWrapper::QueueItem::type = type;
    CSnapShotWrapper::QueueItem::id = modelid;
    CSnapShotWrapper::QueueItem::ImageView = image_view;
    CSnapShotWrapper::QueueItem::color1 = color1;
    CSnapShotWrapper::QueueItem::color2 = color2;
    CSnapShotWrapper::QueueItem::zoom = zoom;
    CSnapShotWrapper::QueueItem::vecOffset.X = off_set_x;
    CSnapShotWrapper::QueueItem::vecOffset.Y = off_set_y;
    CSnapShotWrapper::QueueItem::vecOffset.Z = off_set_z;
    CSnapShotWrapper::isProcessing = true;


    CSnapShotWrapper::queueMutex.unlock();
}
