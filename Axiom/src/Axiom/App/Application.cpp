#include "Application.hpp"
#include "Axiom/Core/Timer.hpp"
#include "Axiom/Engine.hpp"

#include <iostream>

extern bool ax_IsRunning;

GlobalEngineObjects* GEngine = nullptr;

void Application::Run()
{
	GEngine = new GlobalEngineObjects();

	OnInit();

	DeltaTimer deltaTimer;

	while (m_IsRunning)
	{
		float delta = deltaTimer.Update();
	}

	OnShutdown();
	delete GEngine;
}

void Application::Shutdown()
{
	ax_IsRunning = false;
	m_IsRunning = false;
}

void Application::Restart()
{
	m_IsRunning = false;
}
