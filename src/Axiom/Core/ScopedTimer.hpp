#include <chrono>
#include "Logger.hpp"
#include "String.hpp"

#ifndef NDEBUG
#	define AXTIMER(message) ScopedTimer timer = ScopedTimer(message);
#else
#   define AXTIMER(message) ScopedTimer timer = ScopedTimer(message);
#endif

struct ScopedTimer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
	bool printMilisecond;

	ax::String message;

	ScopedTimer(const ax::String& _message) : message(_message), printMilisecond(true)
	{
		start_point = std::chrono::high_resolution_clock::now();
	}

	const double GetTime()
	{
		using namespace std::chrono;
		auto end_point = high_resolution_clock::now();
		auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
		auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
		printMilisecond = false;
		return (end - start) * 0.001;
	}

	~ScopedTimer()
	{
		using namespace std::chrono;
		if (!printMilisecond) return;
		auto end_point = high_resolution_clock::now();
		auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
		auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
		auto _duration = end - start;
		message.AppendChar(' ');
		message.Append(_duration * 0.001f);
		message.Append("ms");
		Logger::ShowFileName(false);
		AXLOG(message.CStr());
		Logger::ShowFileName(true);
	}
};