#ifndef NATIVECRASH_H
#define NATIVECRASH_H

// Устанавливает обработчики сигналов (SIGSEGV, SIGABRT, SIGBUS, SIGILL, SIGFPE).
// При падении пишет сигнал, адрес ошибки и backtrace в файл
// "<g_pszStorage>SAMP/native_crash_log.txt", не требуя adb/ПК.
// Вызывать как можно раньше — до FindLibrary()/установки хуков.
void InstallNativeCrashHandler();

#endif // NATIVECRASH_H
