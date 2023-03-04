#pragma once
#include "PipelineState.hpp"

struct ICommandAllocator : IGraphicsResource
{
	virtual void Reset() = 0;
};

class IDeviceContext;

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

	virtual void SetImageBarrier(IImage* pBuffer, const PipelineBarrier& pBarrier) = 0;
	virtual void SetVertexBuffers(IBuffer** vertexBuffers, uint32 numVertexBuffers) = 0;
	virtual void SetIndexBuffer(IBuffer* indexBuffer) = 0;
	virtual void SetViewports(uint32 numViewports, const ViewportDesc* desc) = 0;
	virtual void SetScissorRects(uint32 numRects, GraphicsRect* rects) = 0;
	virtual void Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) = 0;
};

struct ICommandQueue : IGraphicsResource	
{
	virtual void Signal(IFence* fence, uint64 value) = 0;
	virtual void ExecuteCommandLists(ICommandList** commandLists, uint32 numCommands) = 0;
};

