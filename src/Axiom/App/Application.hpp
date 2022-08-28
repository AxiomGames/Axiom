#pragma once

#include "Axiom/Core/Common.hpp"

AX_NAMESPACE
class Application
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

AX_END_NAMESPACE

extern ax::Application* CreateApplication(int argc, char* argv[]);