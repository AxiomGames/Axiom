#include <iostream>
#include <chrono>
#include <string>
#include "Axiom/Core/String.hpp"
#include "Axiom/Core/Array.hpp"
#include "Axiom/App/SystemWindow.hpp"
#include "Axiom/Graphics/RenderDevice.hpp"

#include <DXGI.h>
#include <D3D11.h>

using namespace Ax;

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

int main()
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
}