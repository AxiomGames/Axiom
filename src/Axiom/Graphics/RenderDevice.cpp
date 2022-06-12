#include "RenderDevice.hpp"

#include "Axiom/App/SystemWindow.hpp"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3native.h>

#include <d3dcompiler.h>

AX_NAMESPACE

RenderContext::RenderContext()
{

}

bool RenderContext::CreateRenderContext(AdapterData* adapter)
{
	uint32_t DeviceCreationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D3D_DRIVER_TYPE DriverType = adapter == nullptr ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN;

	const D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
	D3D_FEATURE_LEVEL CreatedFeatureLevel;

	HRESULT Hr = D3D11CreateDevice(adapter != nullptr ? adapter->Adapter : nullptr, DriverType, nullptr, DeviceCreationFlags, FeatureLevels, sizeof(FeatureLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION, &Device, &CreatedFeatureLevel, &DeviceContext);
	return !FAILED(Hr);
}

void RenderContext::DestroyRenderContext()
{
	DeviceContext->Release();
	Device->Release();

	DeviceContext = nullptr;
	Device = nullptr;
}

bool RenderContext::CreateSwapChain(SystemWindow* systemWindow)
{
	HWND hwnd = glfwGetWin32Window(systemWindow->Window);
	ax::Vector2i size = systemWindow->GetSize();

	// Created swap chain

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	memset(&SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = size.width;
	SwapChainDesc.BufferDesc.Height = size.height;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = hwnd;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.Windowed = !systemWindow->Description.Fullscreen;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice* DXGIDevice;
	HRESULT Hr = Device->QueryInterface(__uuidof(IDXGIDevice), (void**) &DXGIDevice);
	if (!SUCCEEDED(Hr))
	{
		return false;
	}

	IDXGIAdapter* DXGIAdapter;
	Hr = DXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**) &DXGIAdapter);
	if (!SUCCEEDED(Hr))
	{
		return false;
	}

	IDXGIFactory* DXGIFactory;
	Hr = DXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**) &DXGIFactory);
	if (!SUCCEEDED(Hr))
	{
		return false;
	}

	Hr = DXGIFactory->CreateSwapChain(DXGIDevice, &SwapChainDesc, &systemWindow->SwapChain);
	if (!SUCCEEDED(Hr))
	{
		return false;
	}

	Hr = DXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	if (!SUCCEEDED(Hr))
	{
		return false;
	}

	return true;
}

bool RenderContext::CreateBackBuffer(SystemWindow* systemWindow)
{
	systemWindow->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &systemWindow->BackBuffer);

	if (systemWindow->BackBufferView != nullptr)
	{
		systemWindow->BackBufferView->Release();
	}

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	HRESULT Hr = Device->CreateRenderTargetView(systemWindow->BackBuffer, &RTVDesc, &systemWindow->BackBufferView);
	if (!SUCCEEDED(Hr))
	{
		return false;
	}

	return true;
}

Array<AdapterData> RenderContext::GetAdapters()
{
	Array<AdapterData> adapters;

	IDXGIFactory* factory;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);

	if (FAILED(hr))
	{
		// TODO: Throw error
		return adapters;
	}

	IDXGIAdapter* adapter;
	uint32_t index = 0;

	while (SUCCEEDED(factory->EnumAdapters(index, &adapter)))
	{
		AdapterData adapterData(adapter);
		hr = adapter->GetDesc(&adapterData.Description);

		if (FAILED(hr))
		{
			// TODO: Throw error
			break;
		}

		adapters.Emplace(std::forward<AdapterData>(adapterData));
		index++;
	}

	return adapters;
}

ID3DBlob* RenderContext::CompileShader(const ShaderCompileDesc& desc) const
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

	if (ShaderDebug)
	{
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D_SHADER_MACRO* macros = nullptr;

	HRESULT hr = D3DCompile(desc.Source.CStr(), desc.Source.Length(), desc.Name.CStr(), macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, desc.EntryPoint.CStr(), desc.Profile.CStr(), flags, 0, &shaderBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			//std::cerr << (char*)errorBlob->GetBufferPointer() << std::endl;
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return nullptr;
	}

	return shaderBlob;
}

AX_END_NAMESPACE