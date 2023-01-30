#pragma once

#include "D3D12CommonHeaders.hpp"
#include "D3D12SwapChain.hpp"
#include "D3D12Renderer.hpp"
#include "D3D12Shader.hpp"
#include "../DeviceContext.hpp"

struct D3D12Pipeline : IPipeline
{
	ID3D12PipelineState* PipelineState;
	ID3D12RootSignature* RootSignature;
};

class D3D12Context : IDeviceContext
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

public:
	
	~D3D12Context();
	
	void Initialize(SharedPtr<INativeWindow> window) override;
	
	IBuffer* CreateBuffer(const BufferDesc& description, ICommandList* commandList) override;
	void DestroyBuffer(IBuffer* buffer) override;
	
	IPipeline* CreateGraphicsPipeline(PipelineInfo& info) override;
	ICommandAllocator* CreateCommandAllocator(ECommandListType type) override;
	ICommandList* CreateCommandList(ICommandAllocator* allocator, ECommandListType type) override;
	ICommandQueue* CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) override;
	
	IShader* CreateShader(const char* sourceCode, const char* functionName, EShaderType shaderType) override;
	void DestroyShader(IShader* shader) override;
	
	void SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) override;

	void Release() override;

	void BeginFrame() override;

	void Render(ICommandList** commandLists, size_t numCommandLists) override;

	void EndFrame() override;

	ID3D12CommandQueue* GetCommandQueue() { return m_CmdQueue; }

	ID3D12GraphicsCommandList6* GetCommandList() { return m_CmdList; }

	[[nodiscard]] uint32 GetFrameIndex() const { return m_FrameIndex; }

	ID3D12Device8* GetDevice() { return m_Device; };
private:
	void Flush();

private:
	ID3D12CommandQueue* m_CmdQueue = nullptr;
	ID3D12GraphicsCommandList6* m_CmdList = nullptr;
	ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
	D3D12CommandFrame m_CmdFrames[g_NumBackBuffers]{};
	D3D12SwapChain* m_SwapChain = nullptr;
	DX12Renderer* m_Renderer = nullptr;
	ID3D12Device8* m_Device = nullptr;
	IDXGIFactory7* DXFactory = nullptr;

	uint32 m_FrameIndex = 0;

	ID3D12Fence1* m_Fence = nullptr;
	HANDLE m_FenceEvent = nullptr;
	uint64 m_FenceValue = 0;
};