#include "Application.hpp"

extern bool ax_IsRunning;

AX_NAMESPACE

void Application::Run()
{
	OnInit();

	while (m_IsRunning)
	{
		int a = 0;
	}

	OnShutdown();
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

AX_END_NAMESPACE