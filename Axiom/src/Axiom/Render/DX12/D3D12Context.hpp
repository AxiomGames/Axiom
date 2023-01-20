#pragma once

#include "D3D12CommonHeaders.hpp"
#include "D3D12SwapChain.hpp"
#include "D3D12Renderer.hpp"

class D3D12Context
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
	ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
	D3D12CommandFrame m_CmdFrames[g_NumBackBuffers]{};
	D3D12SwapChain* m_SwapChain = nullptr;
	DX12Renderer* m_Renderer = nullptr;

	uint32 m_FrameIndex = 0;

	ID3D12Fence1* m_Fence = nullptr;
	HANDLE m_FenceEvent = nullptr;
	uint64 m_FenceValue = 0;
public:
	
	D3D12Context(ID3D12Device8* device, SharedPtr<GLFWNativeWindow> window, IDXGIFactory7* DXFactory, ID3D12Device8* MainDevice);
	
	~D3D12Context();

	void Flush();

	void Release();

	void BeginFrame();

	void Render();

	void EndFrame();

	ID3D12CommandQueue* GetCommandQueue() { return m_CmdQueue; }

	ID3D12GraphicsCommandList6* GetCommandList() { return m_CmdList; }

	[[nodiscard]] uint32 GetFrameIndex() const { return m_FrameIndex; }
};