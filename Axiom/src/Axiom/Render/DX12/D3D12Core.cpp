#include "D3D12Core.hpp"
#include "D3D12CommonHeaders.hpp"

#ifdef AX_WIN

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

	constexpr uint32 CommandFrameCount = 3;

	class D3D12Command
	{
	private:
		struct D3D12CommandFrame
		{
			ID3D12CommandAllocator* CommandAllocator = nullptr;
			uint64 FenceValue = 0;

			void Release()
			{
				ReleaseResource(CommandAllocator);
			}

			void Wait(HANDLE fenceEvent, ID3D12Fence1* fence)
			{
				ax_assert(fenceEvent && fence);

				if (fence->GetCompletedValue() < FenceValue)
				{
					DXCall(fence->SetEventOnCompletion(FenceValue, fenceEvent));
					WaitForSingleObject(fenceEvent, INFINITE);
				}
			}
		};
	private:
		ID3D12CommandQueue* m_CmdQueue = nullptr;
		ID3D12GraphicsCommandList6* m_CmdList = nullptr;
		D3D12CommandFrame m_CmdFrames[CommandFrameCount]{};
		uint32 m_FrameIndex = 0;

		ID3D12Fence1* m_Fence = nullptr;
		HANDLE m_FenceEvent = nullptr;
		uint64 m_FenceValue = 0;
	public:
		D3D12Command(ID3D12Device8* device, D3D12_COMMAND_LIST_TYPE cmdType)
		{
			HRESULT hr = S_OK;

			D3D12_COMMAND_QUEUE_DESC desc{};
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Type = cmdType;

			DXCall(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CmdQueue)));

			if (FAILED(hr))
			{
				Release();
				return;
			}

			for (uint32 i = 0; i < CommandFrameCount; ++i)
			{
				D3D12CommandFrame& cmdFrame = m_CmdFrames[i];

				DXCall(hr = device->CreateCommandAllocator(cmdType, IID_PPV_ARGS(&cmdFrame.CommandAllocator)));

				if (FAILED(hr))
				{
					Release();
					return;
				}
			}

			DXCall(hr = device->CreateCommandList(0, cmdType, m_CmdFrames[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_CmdList)));

			if (FAILED(hr))
			{
				Release();
				return;
			}

			DXCall(m_CmdList->Close());

			DXCall(hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

			if (FAILED(hr))
			{
				Release();
				return;
			}

			m_FenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			ax_assert(m_FenceEvent);
		}

		~D3D12Command()
		{
			ax_assert(!m_CmdQueue && !m_CmdList && !m_Fence);
		}

		void Flush()
		{
			for (uint32 i = 0; i < CommandFrameCount; ++i)
			{
				D3D12CommandFrame& cmdFrame = m_CmdFrames[i];
				cmdFrame.Wait(m_FenceEvent, m_Fence);
			}
			m_FrameIndex = 0;
		}

		void Release()
		{
			Flush();
			ReleaseResource(m_Fence);
			m_FenceValue = 0;

			CloseHandle(m_FenceEvent);
			m_FenceEvent = nullptr;

			ReleaseResource(m_CmdQueue);
			ReleaseResource(m_CmdList);

			for (uint32 i = 0; i < CommandFrameCount; ++i)
			{
				m_CmdFrames[i].Release();
			}
		}

		void BeginFrame()
		{
			D3D12CommandFrame& frame = m_CmdFrames[m_FrameIndex];

			// Wait for GPU
			frame.Wait(m_FenceEvent, m_Fence);

			DXCall(frame.CommandAllocator->Reset());
			DXCall(m_CmdList->Reset(frame.CommandAllocator, nullptr));
		}

		void EndFrame()
		{
			DXCall(m_CmdList->Close());

			// This is for future where we can first create command lists in other threads and then supply it here
			ID3D12CommandList* commandLists[] {m_CmdList};
			m_CmdQueue->ExecuteCommandLists(_countof(commandLists), &commandLists[0]);

			m_FenceValue++;
			D3D12CommandFrame& frame = m_CmdFrames[m_FrameIndex];
			frame.FenceValue = m_FenceValue;
			m_CmdQueue->Signal(m_Fence, m_FenceValue);

			m_FrameIndex = (m_FrameIndex + 1) % CommandFrameCount;
		}

		ID3D12CommandQueue* GetCommandQueue()
		{ return m_CmdQueue; }

		ID3D12GraphicsCommandList6* GetCommandList()
		{ return m_CmdList; }

		[[nodiscard]] uint32 GetFrameIndex() const
		{ return m_FrameIndex; }
	};

	IDXGIFactory7* DXFactory = nullptr;
	ID3D12Device8* MainDevice = nullptr;
	D3D12Command* RenderCmd = nullptr;

	void Initialize()
	{
		InitializeDXFactory(&DXFactory);
		IDXGIAdapter4* adapter = DetermineMainAdapter(DXFactory);
		D3D_FEATURE_LEVEL maxFeatureLevel = GetMaxFeatureLevel(adapter);

		HRESULT hr = S_OK;
		DXCall(hr = D3D12CreateDevice(adapter, maxFeatureLevel, IID_PPV_ARGS(&MainDevice)));

		if (FAILED(hr))
		{
			return;
		}

		MainDevice->SetName(L"MAIN DEVICE");

		RenderCmd = new D3D12Command(MainDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DXCall(MainDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		};
#endif
	}

	void Shutdown()
	{
		RenderCmd->Release();

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
		ReleaseResource(main_device);
#endif
	}

	void Render()
	{
		RenderCmd->BeginFrame();

		RenderCmd->EndFrame();
	}
}

#endif