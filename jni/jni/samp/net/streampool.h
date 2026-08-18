
#include <thread>
#include <mutex>
#include <queue>
struct BUFFERED_COMMAND_STREAMPOOL
{
    int iID;
    int iPlayerID;
    int iVehicleID;
    int iType;
    const void* pData;
    enum { BC_DELETSTREAM, BC_ADDSTREAM, BC_ATTACH, BC_VOLUME, BC_ACTIVATE, BC_DEATTACH, BC_PAUSE, BC_PLAY, BC_UPDATE } command;
};
#include "..//vendor/raknet/SingleProducerConsumer.h"
#include "game/stream.h"

class CStreamPool
{
    static inline std::mutex queueMutex;
    static inline std::queue<BUFFERED_COMMAND_STREAMPOOL*> list;
public:
    static inline std::vector<CStream*> m_pStaticStream;
    static inline CStream* m_pStreams[MAX_STREAMS];
    static inline bool	m_bSlotState[MAX_STREAMS];
    static inline HSTREAM m_hIndividualStream;
    //RwMatrix m_matListener;
    static bool bShutdownThread;
    std::thread* pThread;
    static DataStructures::SingleProducerConsumer<BUFFERED_COMMAND_STREAMPOOL> bufferedCommands;

    static bool m_bIsDeactivated;

    static inline bool m_bWasPaused;
    static inline char m_szIndividualLastLink[256];
    static inline int m_bIndividualNeedReplay;
public:
    CStreamPool();
    ~CStreamPool();

    void Deactivate();
    void Activate();

    CStream* GetStream(int iID);
    CStream* AddStream(int iID, CVector pPos, int iVirtualWorld, PLAYERID playerid, VEHICLEID vehicleid, int iInterior, float fDistance, const char* szUrl);
    void DeleteStreamByID(int iID);

    static void PlayIndividualStream(const char* szUrl, int type = BASS_SAMPLE_LOOP);
    static void StopIndividualStream();


    void SetStreamVolume(int iID, float fVolume);

    static void Process();

    void addStaticStream(float x, float y, float z, int interior, float dist, const char *url);

    void AttachToPlayer(int iID, PLAYERID id);

    void AttachToVehicle(int iID, VEHICLEID id);

    void DeAttachStreamByID(int iID);


    void PauseStream(int iID);

    void PlayStream(int iID);


    void UpdateAllVolumes();
};