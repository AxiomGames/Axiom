#include "Application.hpp"
#include <string.h> // strcmp
#include <iostream>
#include "Axiom/Core/AssetManager.hpp"

bool ax_IsRunning = true;

void ParseArgs(int argc, char* argv[])
{
	if (argc > 0)
	{
		std::cout << "arg0: " << argv[0] << std::endl;

		if (!strcmp(argv[0], "-build"))
		{
			AssetManager::Initialize(argv[1]);
		}
		else if (!strcmp(argv[0], "-editor"))
		{
			AssetManager::Initialize(nullptr);
		}
	}
	else
	{
		AssetManager::Initialize(nullptr);
		std::cout << "no args" << std::endl;
	}
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