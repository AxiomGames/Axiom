#include "Application.hpp"
#include "Axiom/Core/Timer.hpp"
#include "Axiom/Engine.hpp"

#include "Axiom/App/PlatformBase.hpp"

#include <iostream>

extern bool ax_IsRunning;

GlobalEngineObjects* GEngine = nullptr;

void Application::Run()
{
	GEngine = new GlobalEngineObjects();

	Platform::Init();

	OnInit();

	DeltaTimer deltaTimer;



	while (m_IsRunning)
	{
		float delta = deltaTimer.Update();
		Platform::Update();

		OnUpdate(delta);
	}

	OnShutdown();
	delete GEngine;
	Platform::Shutdown();
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
