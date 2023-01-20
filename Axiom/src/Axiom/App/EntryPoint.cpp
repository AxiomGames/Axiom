#include "Application.hpp"
#include <string.h> // strcmp
#include <iostream>

bool ax_IsRunning = true;

void ParseArgs(int argc, char* argv[])
{
	
}

int Main(int argc, char* argv[])
{
	ParseArgs(argc, argv);

	while (ax_IsRunning)
	{
		Application* app = CreateApplication(argc, argv);
		app->Run();
		delete app;
	}
	return 0;
}

#if defined(_WIN32) && defined(STANDALONE_APP)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	return ax::Main(__argc, __argv);
}
#else

int main(int argc, char* argv[])
{
	return Main(argc, argv);
}
#endif