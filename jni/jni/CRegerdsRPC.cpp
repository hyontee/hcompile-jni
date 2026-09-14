//
// Created by Regerds on 01.11.2024.
//

#include <stdint.h>
#include <string.h>
#include <vector>
#include <stdio.h>
#include "CRegerdsRPC.h"
#include "BitStream.h"
#include "util/util.h"
#include "nvEvent/NvEventQueueActivity.h"
#include "network/CNetGame.h"

#define ID_FUEL_FILL_SHOW_RPC 99

#define ID_CAR_RADIAL_SHOW_RPC 100
#define ID_CAR_RADIAL_UPDATE_RPC 101

#define ID_SHOW_CLIENT_NOTIFICATION_RPC 102

#define ID_SHOW_DONATE_RPC 103

#define ID_SHOW_CLIENT_REGISTER_RPC 104

#define ID_SHOW_DINNER_DIALOG_RPC 105

#define ID_RADIAL_PACKAGE 199
#define ID_CLIENT_NOTIFICATION_PACKAGE 200
#define ID_FUEL_FILL_PACKAGE 201
#define ID_PLAYER_MENU_PACKAGE 202

void CRegerdsRPC::Packet_SyncCustomRPC(Packet* packet) {
    RakNet::BitStream bitStream((unsigned char*)packet->data, packet->length, false);

    uint8_t packetID;
    uint32_t rpcID;

    bitStream.Read(packetID);
    bitStream.Read(rpcID);

    switch (rpcID) {
        case ID_CAR_RADIAL_SHOW_RPC: {
            uint16_t arraySize;
            bitStream.Read(arraySize);

            std::vector<uint16_t> values;
            values.reserve(arraySize);

            for (uint16_t i = 0; i < arraySize; ++i) {
                uint16_t value;
                bitStream.Read(value);
                values.push_back(value);
            }

            int* params = new int[values.size()];
            for (size_t i = 0; i < values.size(); ++i) {
                params[i] = static_cast<int>(values[i]);
            }

            NvEventQueueActivity::CallCarRadialMenu(params);

            delete[] params;
            break;
        }

        case ID_CAR_RADIAL_UPDATE_RPC: {
            uint16_t index;
            bitStream.Read(index);

            uint16_t value;
            bitStream.Read(value);

            JNIEnv* env = NvEventQueueActivity::GetJNIEnv();
            if (env == nullptr) return;

            char buffer[0x7F];
            if (value == 1) {
                sprintf(buffer, R"({"t": %d, "s": true})", index);
            } else {
                sprintf(buffer, R"({"t": %d, "s": false})", index);
            }
            int length = strlen(buffer);

            jbyteArray byteArray = env->NewByteArray(length);
            env->SetByteArrayRegion(byteArray, 0, length, reinterpret_cast<const jbyte*>(buffer));

            NvEventQueueActivity::OnPacketIncoming(27, byteArray);

            env->DeleteLocalRef(byteArray);
            break;
        }

        case ID_FUEL_FILL_SHOW_RPC: {
            uint16_t cost_92, cost_95, cost_98, cost_100, cost_dt, maxFuel, mInitialHintLen;
            char buffer[4096], mInitialHint[256];

            bitStream.Read(cost_92);
            bitStream.Read(cost_95);
            bitStream.Read(cost_98);
            bitStream.Read(cost_100);
            bitStream.Read(cost_dt);

            bitStream.Read(maxFuel);

            bitStream.Read(mInitialHintLen);

            memset(mInitialHint, 0, sizeof(mInitialHint));
            memset(buffer, 0, sizeof(buffer));

            bitStream.Read(buffer, mInitialHintLen);
            cp1251_to_utf8(mInitialHint, buffer);

            int prices[] = {cost_92, cost_95, cost_98, cost_100, cost_dt};
            NvEventQueueActivity::ShowBrCarFuelFillDialog(prices, maxFuel, mInitialHint);
            break;
        }

        case ID_SHOW_CLIENT_NOTIFICATION_RPC: {
            uint16_t type, duration, id, subId, btnCaptionLen, captionLen;
            char buffer[4096], caption[256], btnCaption[256];

            bitStream.Read(type);
            bitStream.Read(duration);
            bitStream.Read(id);
            bitStream.Read(subId);

            bitStream.Read(btnCaptionLen);

            memset(btnCaption, 0, sizeof(btnCaption));
            memset(buffer, 0, sizeof(buffer));

            bitStream.Read(buffer, btnCaptionLen);
            cp1251_to_utf8(btnCaption, buffer);

            bitStream.Read(captionLen);

            memset(caption, 0, sizeof(caption));
            memset(buffer, 0, sizeof(buffer));

            bitStream.Read(buffer, captionLen);
            cp1251_to_utf8(caption, buffer);

            NvEventQueueActivity::ShowNotification(type, duration, id, subId, caption, btnCaption);
            break;
        }

        case ID_SHOW_DONATE_RPC: {
            uint16_t rub, coins;

            bitStream.Read(rub);
            bitStream.Read(coins);

            NvEventQueueActivity::ShowNewDonate(rub, coins);
            break;
        }
        case ID_SHOW_CLIENT_REGISTER_RPC: {
            uint16_t reg;
            bitStream.Read(reg);

            NvEventQueueActivity::ShoRegister(reg);
            break;
        }
        case ID_SHOW_DINNER_DIALOG_RPC: {
            NvEventQueueActivity::ShowDinnerDialog();
            break;
        }
    }
}

void CRegerdsRPC::SendRadialOperation(int i) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  251);
    bsSend.Write((uint32_t) ID_RADIAL_PACKAGE);

    bsSend.Write((uint16_t ) i);

    CNetGame::Instance()->m_rakClientInterface->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
}

void CRegerdsRPC::SendPlayerMenuRequestToServer(int i) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  251);
    bsSend.Write((uint32_t) ID_PLAYER_MENU_PACKAGE);

    bsSend.Write((uint16_t ) i);

    CNetGame::Instance()->m_rakClientInterface->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
}


void CRegerdsRPC::SendNotificationResponse(int subid, int type) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  251);
    bsSend.Write((uint32_t) ID_CLIENT_NOTIFICATION_PACKAGE);

    bsSend.Write((uint16_t ) subid);
    bsSend.Write((uint16_t ) type);

    CNetGame::Instance()->m_rakClientInterface->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
}


void CRegerdsRPC::SendFuelFillRequest(int i, int i2) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  251);
    bsSend.Write((uint32_t) ID_FUEL_FILL_PACKAGE);

    bsSend.Write((uint16_t ) i);
    bsSend.Write((uint16_t ) i2);

    CNetGame::Instance()->m_rakClientInterface->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_sendDinnerRequest(JNIEnv *env, jobject thiz, jint i) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  251);
    bsSend.Write((uint32_t) 203);

    bsSend.Write((uint16_t ) i);

    CNetGame::Instance()->m_rakClientInterface->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
}