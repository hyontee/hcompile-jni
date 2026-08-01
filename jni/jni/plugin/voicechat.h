#pragma once

#include <cstdint>

// Хук CVoiceChatClient::OnPacketIncoming(ENetEvent* event).
//   ARM64 (sub_643E34): x0 = event
//   ARM   (sub_43F964): r0 = event
//
// Задача: SA-MP-сервер не умеет в ENet, осмысленный 246 от него не приходит.
// Пакет 246 в оригинале глушит локальный микрофон (m_nRecording=0 + CSoundInput::Stop).
// Здесь 246 игнорируем (микрофон не трогаем) и проигрываем звук. Остальное (244/245/...)
// отдаём оригиналу как есть. Сигнатура возвращает int — как в оригинале (ABI).

extern int (*orig_CVoiceChatClient__OnPacketIncoming)(void* event);
int hook_CVoiceChatClient__OnPacketIncoming(void* event);

// Короткий звук-уведомление (self-contained, OpenSL ES). Замени тело в .cpp под свой ассет.
void CVoiceNotify_PlaySound();
