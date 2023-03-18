#pragma once

#include "PipelineState.hpp"
#include <Axiom/App/Window.hpp>

// nice documentation for better understandig RHI
// https://alain.xyz/blog/comparison-of-modern-graphics-apis

struct ICommandList;

struct ICommandQueue : IGraphicsResource	
{
    virtual void Signal(IFence* fence, uint64 value) = 0;
    virtual void ExecuteCommandLists(ICommandList** commandLists, uint32 numCommands) = 0;
};

struct IDeviceContext 
{
	virtual void Initialize(SharedPtr<INativeWindow> window) = 0;
	virtual IBuffer* CreateBuffer(const BufferDesc& description, ICommandList* commandList) = 0;

	virtual IPipeline* CreateGraphicsPipeline(PipelineInfo& info) = 0;
	virtual ICommandAllocator* CreateCommandAllocator(ECommandListType type) = 0; // aka command pool in vulkan
	virtual ICommandList* CreateCommandList(ICommandAllocator* allocator, ECommandListType type) = 0;
	virtual ICommandQueue* CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) = 0; 
    virtual ISwapChain* CreateSwapChain(ICommandQueue* commandQueue, EGraphicsFormat format) = 0;
	virtual IFence* CreateFence() = 0;
	virtual IShader* CreateShader(const char* sourceCode, const char* functionName, EShaderType shaderType) = 0;
    
    virtual void MapBuffer(IBuffer* buffer, void const* data, uint64 size) = 0;

	virtual void DestroyResource(IGraphicsResource* resource) = 0;

	virtual void WaitFence(IFence* fence, uint32 fenceValue) = 0;
	virtual void Release() = 0;

	// todo add coppy buffer 
};

struct ICommandList : IGraphicsResource
{
    virtual void Initialize(IDeviceContext* deviceContext) = 0;
    virtual void Close() = 0;
    virtual void Reset(ICommandAllocator* commandAllocator, IPipeline* pipeline) = 0;

    virtual void SetPipelineState(IPipeline* pipeline) = 0;
    virtual void DrawIndexedInstanced(uint32 numIndex, uint32 numInstance, uint32 startIndex, int startVertex, uint32 startInstance) = 0;
    virtual void SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) = 0;
    virtual void ClearRenderTarget(IImage* image, float color[4]) = 0;
    virtual void ClearDepthStencil(IImage* image) = 0;
    virtual void SetRenderTargets(IImage** images, uint32 numImages, IImage* depthStencil) = 0;
    virtual void SetConstantBufferView(int index, IBuffer* buffer) = 0;
    virtual void SetTexture(uint32 index, IBuffer* texture) = 0;

    virtual void SetImageBarrier(IImage* pBuffer, const PipelineBarrier& pBarrier) = 0;
    virtual void SetVertexBuffers(IBuffer** vertexBuffers, uint32 numVertexBuffers) = 0;
    virtual void SetIndexBuffer(IBuffer* indexBuffer) = 0;
    virtual void SetViewports(uint32 numViewports, const ViewportDesc* desc) = 0;
    virtual void SetScissorRects(uint32 numRects, GraphicsRect* rects) = 0;
    
    virtual void SetGraphicsPushConstants(IPipeline* pipeline, EShaderType stage, void* data, size_t size) = 0;
    virtual void Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) = 0;
};
