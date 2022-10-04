#pragma once

#include "../Core/Common.hpp"

class AX_API Application
{
private:
	bool m_IsRunning = true;
public:
	virtual ~Application() = default;

	void Run();
	void Shutdown();
	void Restart();

	virtual void OnInit() = 0;
	virtual void OnShutdown() {}
};

extern Application* CreateApplication(int argc, char* argv[]);