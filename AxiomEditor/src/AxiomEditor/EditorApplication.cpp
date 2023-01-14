#include "EditorApplication.hpp"
#include "Axiom/Core/TypeID.hpp"
#include <iostream>
#include "Axiom/Engine.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Core/Optional.hpp"

#include <entt/entt.hpp>

#include "Axiom/Render/DX12/D3D12Core.hpp"

#include <Axiom/Core/Allocator.hpp>
#include <Axiom/Core/Memory.hpp>
#include <Axiom/Core/BlockAllocator.hpp>

void EditorApplication::OnInit()
{
	GEngine->Initialize<WindowManager>();

	SharedPtr<UIWindow> window = UINew(UIWindow)
									.AutoCenter(true)
									.Size(Vector2i(1270, 720))
									.Title("Axiom");

	GEngine->Get<WindowManager>()->AddWindow(window, true);

//	DX12::Initialize();

	struct Test
	{
		int A = 0;
	};

	Test* a = Memory::Alloc<Test>();
	Memory::FreeDestruct<int>(a);

	int* b = (int*)Memory::Malloc(sizeof(int));
	Memory::Free(b);

	int* c = (int*)Memory::MallocZeroed(sizeof(int));
	Memory::Free(c);


	IAllocator* allocator = new BlockAllocator();

	Test* d = allocator->Alloc<Test>();
	allocator->FreeDestruct<Test>(d);

	delete allocator;
}

void EditorApplication::OnShutdown()
{
//	DX12::Shutdown();

	GEngine->Destroy<WindowManager>();
}

void EditorApplication::OnUpdate(float delta)
{
//	DX12::Render();
}

Application* CreateApplication(int argc, char* argv[])
{
	return new EditorApplication();
}