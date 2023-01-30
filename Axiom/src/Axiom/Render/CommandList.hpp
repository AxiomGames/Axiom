#pragma once
#include "PipelineState.hpp"
#include "Buffer.hpp"

struct ICommandAllocator 
{
	
};

class IDeviceContext;

struct ICommandList
{
	virtual void Initialize(IDeviceContext* deviceContext) = 0;
	virtual void Close() = 0;
	virtual void Reset(ICommandAllocator* commandAllocator) = 0;

	virtual void SetPipelineState(IPipeline* pipeline) = 0;
	virtual void DrawIndexedInstanced(int numIndex, int numInstance, int startIndexLocation, int startVertexLocation, int startInstanceLocation) = 0;

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

