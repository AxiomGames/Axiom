#include "EditorApplication.hpp"
#include "Axiom/Core/TypeID.hpp"
#include <iostream>
#include "Axiom/Engine.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Core/Optional.hpp"

void EditorApplication::OnInit()
{
	GEngine->Initialize<WindowManager>();

	SharedPtr<UIWindow> window = UINew(UIWindow)
									.AutoCenter(true)
									.Size(Vector2i(1270, 720))
									.Title("Axiom");

	GEngine->Get<WindowManager>()->AddWindow(window, true);
}

void EditorApplication::OnShutdown()
{
	GEngine->Destroy<WindowManager>();
}

Application* CreateApplication(int argc, char* argv[])
{
	return new EditorApplication();
}