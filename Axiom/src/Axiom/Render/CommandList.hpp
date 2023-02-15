#pragma once
#include "PipelineState.hpp"

struct ICommandAllocator : IGraphicsResource
{
	
};

class IDeviceContext;

struct ICommandList : IGraphicsResource
{
	virtual void Initialize(IDeviceContext* deviceContext) = 0;
	virtual void Close() = 0;
	virtual void Reset(ICommandAllocator* commandAllocator) = 0;

	virtual void SetPipelineState(IPipeline* pipeline) = 0;
	virtual void DrawIndexedInstanced(int numIndex, int numInstance, int startIndexLocation, int startVertexLocation, int startInstanceLocation) = 0;
	virtual void SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) = 0;
	virtual void ClearRenderTarget(IImage* image, float color[4]) = 0;
	virtual void SetRenderTargets(IImage** images, int numImages) = 0;

	virtual void SetVertexBuffers(IBuffer** vertexBuffers, int numVertexBuffers) = 0;
	virtual void SetIndexBuffers(IBuffer** indexBuffers, int numIndexBuffers) = 0;
	virtual void SetViewport(const ViewportDesc& desc) = 0;
	virtual void SetScissorRects(Rect** rects, int numRects) = 0;
	virtual void Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) = 0;
};

struct ICommandQueue	
{
	virtual void ExecuteCommandLists(ICommandList* const* commandLists, int numCommands) = 0;
};

