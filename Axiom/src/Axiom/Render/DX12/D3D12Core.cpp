#include "D3D12Core.hpp"
#include "D3D12CommonHeaders.hpp"
#include "D3D12Context.hpp"
#include "D3D12SwapChain.hpp"

#ifdef AX_WIN32

namespace DX12
{
	bool InitializeDXFactory(IDXGIFactory7** factory)
	{
		uint32 dx_factory_flags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug3> debugInterface;
		DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
		debugInterface->EnableDebugLayer();

		dx_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT hr = CreateDXGIFactory2(dx_factory_flags, IID_PPV_ARGS(factory));

		if (FAILED(hr))
		{
			std::cerr << "Failed to init DX factory" << std::endl;
			return false;
		}

		return true;
	}

	constexpr D3D_FEATURE_LEVEL minimum_feature_level = D3D_FEATURE_LEVEL_11_0;

	IDXGIAdapter4* DetermineMainAdapter(IDXGIFactory7* factory)
	{
		IDXGIAdapter4* adapter;

		for (uint32 i = 0; factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; i++)
		{
			if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}

			ReleaseResource(adapter);
		}

		return nullptr;
	}

	D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter)
	{
		constexpr D3D_FEATURE_LEVEL featureLevels[4] {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_12_1
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelInfo;
		featureLevelInfo.NumFeatureLevels = _countof(featureLevels);
		featureLevelInfo.pFeatureLevelsRequested = featureLevels;

		ComPtr<ID3D12Device> device;
		DXCall(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)));
		DXCall(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfo, sizeof(featureLevelInfo)));
		return featureLevelInfo.MaxSupportedFeatureLevel;
	}

	IDXGIFactory7* DXFactory = nullptr;
	ID3D12Device8* MainDevice = nullptr;
	D3D12Context* RenderCtx = nullptr;

	void Initialize(SharedPtr<GLFWNativeWindow> window)
	{
		InitializeDXFactory(&DXFactory);
		IDXGIAdapter4* adapter = DetermineMainAdapter(DXFactory);
		D3D_FEATURE_LEVEL maxFeatureLevel = GetMaxFeatureLevel(adapter);

		DXCall(D3D12CreateDevice(adapter, maxFeatureLevel, IID_PPV_ARGS(&MainDevice)));

		MainDevice->SetName(L"MAIN DEVICE");

#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DXCall(MainDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		};
#endif

		RenderCtx = new D3D12Context(MainDevice, window, DXFactory, MainDevice);
	}

	void Shutdown()
	{
		RenderCtx->Release();

		ReleaseResource(DXFactory);

#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DXCall(MainDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
		};

		ComPtr<ID3D12DebugDevice2> debugDevice;
		DXCall(MainDevice->QueryInterface(IID_PPV_ARGS(&debugDevice)));
		ReleaseResource(MainDevice);
		DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
#else
		ReleaseResource(MainDevice);
#endif
	}

	void Render()
	{
		RenderCtx->BeginFrame();
		RenderCtx->Render();
		RenderCtx->EndFrame();
	}
}

#endif