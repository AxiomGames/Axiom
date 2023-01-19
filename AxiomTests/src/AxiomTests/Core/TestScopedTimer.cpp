#include "doctest.h"

#include "Axiom/Core/Logger.hpp"

#ifdef AX_LOG
#undef AX_LOG
#endif
#define AX_LOG(format, ...) CHECK(strstr(format, "msgTest") != nullptr)

#include "Axiom/Core/ScopedTimer.hpp"

#include <chrono>
#include <thread>



TEST_CASE("testScopedTimer")
{
	{
		ScopedTimer scopedTimer("msgTest");
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		CHECK(scopedTimer.GetTime() != 0.0f);
	}

	{
		ScopedTimer scopedTimer("msgTest");
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

}
