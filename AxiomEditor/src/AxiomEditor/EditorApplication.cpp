#include "EditorApplication.hpp"
#include "Axiom/Core/TypeID.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include "Axiom/Engine.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Core/Optional.hpp"
#include <GLFW/glfw3.h>
#include "Axiom/Render/Renderer.hpp"

#include <entt/entt.hpp>

void EditorApplication::OnInit()
{
	GEngine->Initialize<WindowManager>();

	SharedPtr<UIWindow> window = UINew(UIWindow).AutoCenter(true)
							.Size(Vector2i(1440, 900))
							.Title("Axiom");

	GEngine->Get<WindowManager>()->AddWindow(window, true);
	m_GraphicsRenderer = new GraphicsRenderer(window);
}

void EditorApplication::OnUpdate(float delta)
{
	m_GraphicsRenderer->Render();
}

void EditorApplication::OnShutdown()
{
	GEngine->Destroy<WindowManager>();
	delete m_GraphicsRenderer;
}

Application* CreateApplication(int argc, char* argv[])
{
	return new EditorApplication();
}