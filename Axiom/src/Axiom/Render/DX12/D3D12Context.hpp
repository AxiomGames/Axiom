#pragma once

#include "D3D12CommonHeaders.hpp"
#include "D3D12SwapChain.hpp"
#include "../DeviceContext.hpp"
#include "D3D12Resource.hpp"

class D3D12Context : public IDeviceContext
{
public:
	~D3D12Context();
	void Initialize(SharedPtr<INativeWindow> window) override;
	
	IBuffer* CreateBuffer(const BufferDesc& description, ICommandList* commandList) override;
	
	IPipeline* CreateGraphicsPipeline(PipelineInfo& info) override;
	ICommandAllocator* CreateCommandAllocator(ECommandListType type) override;
	ICommandList* CreateCommandList(ICommandAllocator* allocator, ECommandListType type) override;
	ICommandQueue* CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) override;
	ISwapChain* CreateSwapChain(ICommandQueue* commandQueue, EImageFormat format) override;
	IFence* CreateFence() override;
    DescriptorSet* CreateDescriptorSet(DescriptorSetDesc& desc) override;

	IShader* CreateShader(const char* sourceCode, const char* functionName, EShaderType shaderType) override;
	
	void DestroyResource(IGraphicsResource* resource) override;

	void WaitFence(IFence* fence, uint32 value) override;

	void Release() override;
    
	ID3D12Device8* GetDevice() { return m_Device; };

private:
	SharedPtr<INativeWindow> m_Window;
	ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
	ID3D12Device8* m_Device = nullptr;
	IDXGIFactory7* DXFactory = nullptr;

	HANDLE m_FenceEvent = nullptr;
};