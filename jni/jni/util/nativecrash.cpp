#include "nativecrash.h"

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <unwind.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <android/log.h>

extern const char* g_pszStorage;

namespace
{
	struct BacktraceState
	{
		void** current;
		void** end;
	};

	_Unwind_Reason_Code UnwindCallback(struct _Unwind_Context* context, void* arg)
	{
		BacktraceState* state = static_cast<BacktraceState*>(arg);
		uintptr_t pc = _Unwind_GetIP(context);
		if (pc)
		{
			if (state->current == state->end)
				return _URC_END_OF_STACK;
			*state->current++ = reinterpret_cast<void*>(pc);
		}
		return _URC_NO_REASON;
	}

	size_t CaptureBacktrace(void** buffer, size_t max)
	{
		BacktraceState state = { buffer, buffer + max };
		_Unwind_Backtrace(UnwindCallback, &state);
		return state.current - buffer;
	}

	const char* GetStoragePath()
	{
		return g_pszStorage ? g_pszStorage : "/storage/emulated/0/Wizzi/";
	}

	void EnsureLogDirExists()
	{
		char dirPath[512];
		snprintf(dirPath, sizeof(dirPath), "%sSAMP", GetStoragePath());
		mkdir(GetStoragePath(), 0777);
		mkdir(dirPath, 0777);
	}

	void WriteCrashReport(int signum, siginfo_t* info)
	{
		EnsureLogDirExists();

		char path[512];
		snprintf(path, sizeof(path), "%sSAMP/native_crash_log.txt", GetStoragePath());

		FILE* f = fopen(path, "a");
		if (!f)
		{
			__android_log_print(ANDROID_LOG_ERROR, "NativeCrash", "Could not open %s", path);
			return;
		}

		time_t now = time(nullptr);
		char timeBuf[64];
		strftime(timeBuf, sizeof(timeBuf), "%d.%m.%Y %H:%M:%S", localtime(&now));

		fprintf(f, "===== NATIVE CRASH =====\n");
		fprintf(f, "Время: %s\n", timeBuf);
		fprintf(f, "Сигнал: %d (%s)\n", signum, strsignal(signum));
		if (info)
			fprintf(f, "Адрес ошибки (faulting address): %p\n", info->si_addr);

		void* buffer[64];
		size_t count = CaptureBacktrace(buffer, 64);
		fprintf(f, "Backtrace (%zu кадров):\n", count);

		for (size_t i = 0; i < count; ++i)
		{
			Dl_info dlinfo;
			memset(&dlinfo, 0, sizeof(dlinfo));
			if (dladdr(buffer[i], &dlinfo) && dlinfo.dli_fname)
			{
				uintptr_t base = reinterpret_cast<uintptr_t>(dlinfo.dli_fbase);
				uintptr_t addr = reinterpret_cast<uintptr_t>(buffer[i]);
				fprintf(f, "  #%02zu  %s + 0x%lx  (pc %p)\n",
					i, dlinfo.dli_fname, static_cast<unsigned long>(addr - base), buffer[i]);
			}
			else
			{
				fprintf(f, "  #%02zu  <неизвестно>  (pc %p)\n", i, buffer[i]);
			}
		}

		fprintf(f, "=========================\n\n");
		fflush(f);
		fclose(f);

		__android_log_print(ANDROID_LOG_ERROR, "NativeCrash", "Crash log saved to %s", path);
	}

	void CrashSignalHandler(int signum, siginfo_t* info, void* /*ucontext*/)
	{
		WriteCrashReport(signum, info);

		// возвращаем системный обработчик и поднимаем сигнал заново,
		// чтобы Android всё равно корректно завершил процесс
		signal(signum, SIG_DFL);
		raise(signum);
	}
}

void InstallNativeCrashHandler()
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = CrashSignalHandler;
	sa.sa_flags = SA_SIGINFO;

	sigaction(SIGSEGV, &sa, nullptr);
	sigaction(SIGABRT, &sa, nullptr);
	sigaction(SIGBUS, &sa, nullptr);
	sigaction(SIGILL, &sa, nullptr);
	sigaction(SIGFPE, &sa, nullptr);
}
