#include "Application.hpp"

bool ax_IsRunning = true;

AX_NAMESPACE

int Main(int argc, char* argv[])
{
	while (ax_IsRunning)
	{
		ax::Application* app = CreateApplication(argc, argv);
		app->Run();
		delete app;
	}
	return 0;
}

AX_END_NAMESPACE

#if defined(_WIN32) && defined(STANDALONE_APP)
#include <Windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	return ax::Main(__argc, __argv);
}
#else
int main(int argc, char* argv[])
{
	return ax::Main(argc, argv);
}
#endif