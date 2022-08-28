#include <iostream>
#include <chrono>
#include <string>
#include "Axiom/App/Application.hpp"
#include "Axiom/Core/String.hpp"
#include "Axiom/Core/Array.hpp"
#include "Axiom/App/SystemWindow.hpp"
#include "Axiom/Graphics/RenderDevice.hpp"
#include "Axiom/Framework/ECS.hpp"
#include <entt/entity/registry.hpp>

#include <DXGI.h>
#include <D3D11.h>

using namespace ax;

const char* shader_blit = R"(
struct FullScreenQuadOutput
{
	float4 position     : SV_Position;
	float2 uv           : TEXCOORD;
};
FullScreenQuadOutput MainVS(uint id : SV_VertexID)
{
	FullScreenQuadOutput OUT;
	uint u = ~id & 1;
	uint v = (id >> 1) & 1;
	OUT.uv = float2(u, v);
	OUT.position = float4(OUT.uv * 2 - 1, 0, 1);
	// In D3D (0, 0) stands for upper left corner
	OUT.uv.y = 1.0 - OUT.uv.y;
	return OUT;
}

float4 MainPS(FullScreenQuadOutput IN) : SV_Target
{
	return float4(0.2, 0.0, 0.4, 1.0);
}
)";

class ScopedTimer
{
public:
	explicit ScopedTimer(const std::string& name)
	{
		m_name = name;
		m_begin = std::chrono::steady_clock::now();
	}

	virtual ~ScopedTimer()
	{
		auto end = std::chrono::steady_clock::now();

		auto count = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_begin).count();
		std::cout << "[" << m_name << "] Elapsed: " << count << "ms\n";
	}

protected:
	std::string m_name;
	std::chrono::steady_clock::time_point m_begin;
};

struct TestComponent : ComponentBase
{
	CLASS_OBJ(TestComponent, ComponentBase);
	int A;

	TestComponent() : A(1000) {}
	TestComponent(int a) : A(a) {}
};

int TestEnTT()
{
	int a = 0;

	{
		ScopedTimer timer("EnTT");

		int count = 100'000;

		entt::registry registry;

		{
			ScopedTimer timer2("First component");
			auto entity0 = registry.create();
			registry.emplace<TestComponent>(entity0, 10);
		}

		{
			ScopedTimer timer2("Creation");

			for (int i = 0; i < count; ++i)
			{
				auto entity0 = registry.create();
				registry.emplace<TestComponent>(entity0, 10);
			}
		}

		{
			ScopedTimer timer3("LoopThrough");
			auto view = registry.view<TestComponent>();

			for (const auto& it : view)
			{
				TestComponent& cmp = view.get<TestComponent>(it);

				a += cmp.A;
			}
		}
	}

	printf("Sum: %d\n", a);

	return 0;
}

int main2()
{
	printf("Begin EnTT\n");
	TestEnTT();

	printf("\n");
	printf("Begin ours ecs\n");

	int a = 0;

	{
		ScopedTimer timer("ESC");

		int count = 100'000;

		ECSRegistry registry;

		{
			ScopedTimer timer2("First component");
			EntityID entity0 = registry.NewEntity();
			registry.AddComponent<TestComponent>(entity0, 10);
		}

		{
			ScopedTimer timer2("Creation");

			for (int i = 0; i < count; ++i)
			{
				EntityID entity = registry.NewEntity();
				registry.AddComponent<TestComponent>(entity, 10);
			}
		}

		{
			ScopedTimer timer3("LoopThrough");
			for (TestComponent* test : registry.GetComponents<TestComponent>())
			{
				a += test->A;
			}
		}
	}

	printf("Sum: %d", a);

	return 0;
}

int mainaa()
{
	Array<int> one;
	one.Add(0);
	one.Add(1);

	Array<int> two;
	one.Add(2);
	one.Add(3);

	one.Insert(two);

	for (int i : one)
	{
		printf("num: %d\n", i);
	}


	Array<TestComponent*> arr;
	arr.Add(new TestComponent());
	arr.Add(nullptr);
	(*arr.begin())->A = 10;
	arr.Remove(arr.begin() + 1);
	arr.Add(new TestComponent(100));

	printf("Arr size: %d\n", arr.Size());

	for (TestComponent* cmp : arr)
	{
		printf("A = %d\n", cmp->A);
	}

	//////////////

	CMBAllocator allocator;
	int* number = allocator.Alloc<int>(666);
	printf("Num: %d\n", *number);
	allocator.Free<int>(number);

	//////////////

	ECSRegistry registry;

	EntityID entity0 = registry.NewEntity();
	TestComponent* component = registry.AddComponent<TestComponent>(entity0);
	component->A = 700;

	if (registry.HasComponent<TestComponent>(entity0))
	{
		printf("Has component\n");
	}

	if (TestComponent* tc1 = registry.GetComponent<TestComponent>(entity0))
	{
		printf("TestComponent A = %d\n", tc1->A);
	}

	EntityID entity1 = registry.NewEntity();
	TestComponent* component2 = registry.AddComponent<TestComponent>(entity1);
	component2->A = 123;

	registry.RemoveComponent<TestComponent>(entity0);
	//registry.RemoveEntity(entity0);

	for (TestComponent* cmp : registry.GetComponents<TestComponent>())
	{
		printf("TestComponent all A = %d\n", cmp->A);
	}

	return 0;
}

#include "Axiom/Core/Array.hpp"
#include "Axiom/Core/MemoryArchive.hpp"

ax::Application* CreateApplication(int argc, char* argv[])
{
	MemoryArchive archive;
	archive.SetSaving(true);
	Array<int> test;
	test.Add(10);
	test.Add(50);
	archive << test;
	String str = "yooo123";
	archive << str;

	Array<int> newOne;
	archive.SetSaving(false);
	archive.SetLoading(true);
	archive.ResetCursor();
	archive << newOne;

	for (int i = 0; i < newOne.Size(); ++i)
	{
		int a = newOne[i];
		int b = 0;
	}

	String newString;
	archive << newString;

	std::cout << newString << std::endl;
	std::cout << "yoi" << std::endl;

	return nullptr;
}

int oldMain(int argc, char* argv[])
{
	SystemWindow* window = SystemWindow::Create(WindowDesc(), true);
	RenderContext renderContext;

	Array<AdapterData> adapters = RenderContext::GetAdapters();

	for (const AdapterData& adapterData: adapters)
	{
		std::wcout << adapterData.Description.Description << std::endl;
	}

	renderContext.CreateRenderContext(&adapters[0]);
	renderContext.CreateSwapChain(window);

	float clearColor[4] = {0.5f, 0.8f, 0.3f, 1.0f};

	D3D11_VIEWPORT viewport;
	viewport.Width = 1280;
	viewport.Height = 720;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;


	ID3DBlob* vsShaderBlob = renderContext.CompileShader(ShaderCompileDesc{
		.Source = shader_blit,
		.Name = "Blit",
		.Macros = {},
		.EntryPoint = "MainVS",
		.Profile = "vs_5_0"
	});

	ID3DBlob* psShaderBlob = renderContext.CompileShader(ShaderCompileDesc{
		.Source = shader_blit,
		.Name = "Blit",
		.Macros = {},
		.EntryPoint = "MainPS",
		.Profile = "ps_5_0"
	});

	ID3D11VertexShader* vs;
	if (FAILED(renderContext.Device->CreateVertexShader(vsShaderBlob->GetBufferPointer(), vsShaderBlob->GetBufferSize(), nullptr, &vs)))
	{
		std::cout << "Creating VS failed" << std::endl;
		exit(-1);
	}

	ID3D11PixelShader* ps;
	if (FAILED(renderContext.Device->CreatePixelShader(psShaderBlob->GetBufferPointer(), psShaderBlob->GetBufferSize(), nullptr, &ps)))
	{
		std::cout << "Creating VS failed" << std::endl;
		exit(-1);
	}

	// Init raster state
	ID3D11RasterizerState* rasterizerState;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	if (FAILED(renderContext.Device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)))
	{
		std::cout << "Could not create raster state" << std::endl;
		exit(-1);
	}

	while (window->ShouldClose() == false)
	{
		SystemWindow::PoolEvents();

		window->UpdateSwapChain(&renderContext);

		renderContext.DeviceContext->OMSetRenderTargets(1, &window->BackBufferView, nullptr);
		renderContext.DeviceContext->RSSetViewports(1, &viewport);

		renderContext.DeviceContext->ClearRenderTargetView(window->BackBufferView, clearColor);

		renderContext.DeviceContext->IASetInputLayout(nullptr);
		renderContext.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		renderContext.DeviceContext->VSSetShader(vs, nullptr, 0);
		renderContext.DeviceContext->PSSetShader(ps, nullptr, 0);

		//renderContext.DeviceContext->PSSetConstantBuffers(0, 1, &cbuffer);

		renderContext.DeviceContext->RSSetState(rasterizerState);
		renderContext.DeviceContext->Draw(4, 0);

		window->SwapChain->Present(0, 0);
	}

	renderContext.DestroyRenderContext();

	delete window;

	glfwTerminate();

	return 0;
}