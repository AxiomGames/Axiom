#include "EditorApplication.hpp"
#include "Axiom/Core/TypeID.hpp"
#include <iostream>
#include "Axiom/Engine.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Core/Optional.hpp"

void EditorApplication::OnInit()
{
	GEngine->Initialize<WindowManager>();

	SharedPtr<UIWindow> window = MakeShared<UIWindow>();

	//GEngine->Get<WindowManager>()->AddWindow();
}

void EditorApplication::OnShutdown()
{
	GEngine->Destroy<WindowManager>();
}

Application* CreateApplication(int argc, char* argv[])
{
	return new EditorApplication();
}