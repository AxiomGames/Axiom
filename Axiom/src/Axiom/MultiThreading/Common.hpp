#pragma once

#include "../Core/Common.hpp"
#include <thread>
#include <cassert>

#ifdef _WIN32
extern "C" {
	__declspec(dllimport) uint64 __stdcall SetThreadAffinityMask(void* thread, uint64 mask);
	__declspec(dllimport) int   __stdcall SetThreadPriority(void* thread, int priority);
	__declspec(dllimport) long  __stdcall SetThreadDescription(void* thread, const wchar_t* description);
}
#endif

struct IExecutable
{
	IExecutable() = default;
	virtual void Execute() {}
};

inline void SetToUniqueCore(std::thread& worker, int coreIndex)
{
#ifdef _WIN32
	// https://github.com/turanszkij/JobSystem/blob/master/JobSystem.cpp#L111
	// Do Windows-specific thread setup:
	// Put each thread on to dedicated core
	void* handle = worker.native_handle();
	uint64 affinityMask = 1ull << coreIndex;
	uint64 affinity_result = SetThreadAffinityMask(handle, affinityMask);
	assert(affinity_result > 0);

	//// Increase thread priority:
	constexpr int THREAD_PRIORITY_HIGHEST = 2;
	bool priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
	assert(priority_result != 0);

	// Name the thread:
	constexpr const wchar_t* ThreadHeader = L"axThread";
	wchar_t threadName[13];
	memset(threadName, 0, sizeof(wchar_t) * 13);
	memcpy(threadName, ThreadHeader, 9 * sizeof(wchar_t));
	_itow(coreIndex, threadName + 8, 10);

	long hr = SetThreadDescription(handle, threadName);
	assert(hr >= 0);
#elif defined(PLATFORM_LINUX)
	#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); } while (0)

		int ret;
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		size_t cpusetsize = sizeof(cpuset);

		CPU_SET(coreIndex, &cpuset);
		ret = pthread_setaffinity_np(worker.native_handle(), cpusetsize, &cpuset);
		if (ret != 0)
		handle_error_en(ret, std::string(" pthread_setaffinity_np[" + std::to_string(coreIndex) + ']').c_str());

		// Name the thread
		std::string thread_name = "wi::job::" + std::to_string(coreIndex);
		ret = pthread_setname_np(worker.native_handle(), thread_name.c_str());
		if (ret != 0)
		handle_error_en(ret, std::string(" pthread_setname_np[" + std::to_string(coreIndex) + ']').c_str());
		#undef handle_error_en
#endif
}