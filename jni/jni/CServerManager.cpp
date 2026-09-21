//
// Created by weikton on 24.04.2025.
//
#include "main.h"
#include "CServerManager.h"

const char* g_szServerNames[] = {
        weikton("[1] BRILLIANT RP (Тестовый сервер)"),
        weikton("[2] BRILLIANT RP (Production)")
};
constexpr size_t MAX_SERVERS = sizeof(g_szServerNames)
        / sizeof(g_szServerNames[0]);

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
        // CServerInstance::create(weikton("хост"), 1, колво строк в хосте, порт, войс)
     CServerInstance::create(weikton("5.39.108.49"), 1, 11, 1277, true),
     CServerInstance::create(weikton("127.0.0.1"), 1, 9, 7777, true)
};