#pragma once

#include "CommandList.hpp"
#include "PipelineState.hpp"
#include <Axiom/App/Window.hpp>

// nice documentation for better understandig RHI
// https://alain.xyz/blog/comparison-of-modern-graphics-apis

class IDeviceContext 
{
public:
	virtual void Initialize(SharedPtr<INativeWindow> window) = 0;
	virtual IBuffer* CreateBuffer(const BufferDesc& description, ICommandList* commandList) = 0;

	virtual IPipeline* CreateGraphicsPipeline(PipelineInfo& info) = 0;
	virtual ICommandAllocator* CreateCommandAllocator(ECommandListType type) = 0; // aka command pool in vulkan
	virtual ICommandList* CreateCommandList(ICommandAllocator* allocator, ECommandListType type) = 0;
	virtual ICommandQueue* CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) = 0; 
	virtual ISwapChain* CreateSwapChain(ICommandQueue* commandQueue, EImageFormat format) = 0;
	virtual IFence* CreateFence() = 0;
	virtual IShader* CreateShader(const char* sourceCode, const char* functionName, EShaderType shaderType) = 0;
    virtual DescriptorSet* CreateDescriptorSet(DescriptorSetDesc& desc) = 0;

	virtual void DestroyResource(IGraphicsResource* resource) = 0;

	virtual void WaitFence(IFence* fence, uint32 fenceValue) = 0;
	virtual void Release() = 0;

	// todo add coppy buffer 
};