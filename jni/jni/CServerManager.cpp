//
// Created by Venchik on 24.04.2025.
//
#include "main.h"
#include "CServerManager.h"

const char* g_szServerNames[] = {
        weikton("[1] VENTARO RP (#1)"),
        weikton("[2] VENTARO RP (Production)")
};
constexpr size_t MAX_SERVERS = sizeof(g_szServerNames)
        / sizeof(g_szServerNames[0]);

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
        // CServerInstance::create(weikton("188.127.241.74"), 1, 11, 3688, true)
     CServerInstance::create(weikton("188.127.241.74"), 1, 11, 3688, true),
     CServerInstance::create(weikton("188.127.241.74"), 1, 9, 3688, true)
};