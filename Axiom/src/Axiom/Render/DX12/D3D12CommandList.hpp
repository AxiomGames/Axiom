#pragma once
#include "D3D12Resource.hpp"

class ID3D12RootSignature;
class ID3D12GraphicsCommandList6;
class ID3D12Device8;

struct D3D12Shader : public IShader
{
	ID3D12RootSignature* m_RootSignature;
	void Release() override
	{ m_RootSignature->Release(); }
};

class D3D12CommandList : public ICommandList
{
public:
	void Initialize(IDeviceContext* deviceContext) override;
	void Close() override;
	void Reset(ICommandAllocator* commandAllocator, IPipeline* pipeline) override;
	void Release() override;

	void SetPipelineState(IPipeline* pipeline) override;
	void DrawIndexedInstanced(uint32 numIndex, uint32 numInstance, uint32 startIndex, int startVertex, uint32 startInstance) override;
	void SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) override;
	void ClearRenderTarget(IImage* image, float color[4]) override;
    void ClearDepthStencil(IImage* image) override;
    void SetRenderTargets(IImage** images, uint32 numImages, IImage* depthStencil) override;

	void SetImageBarrier(IImage* pBuffer, const PipelineBarrier& pBarrier) override;
	void SetVertexBuffers(IBuffer** vertexBuffers, uint32 numVertexBuffers) override;
	void SetIndexBuffer(IBuffer* indexBuffers) override;
	void SetViewports(uint32 numViewports, const ViewportDesc* desc) override;
	void SetScissorRects(uint32 numRects, GraphicsRect* rects) override;
    void SetGraphicsPushConstants(IPipeline* pipeline, EShaderType stage, void* data, size_t size) override;
    void Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) override;
	
public:
	ID3D12GraphicsCommandList6* m_CmdList = nullptr;
private:
	ID3D12Device8* m_Device = nullptr;
};

struct D3D12CommandQueue : public ICommandQueue
{
	ID3D12CommandQueue* queue;
	
	void Release() override
	{ queue->Release(); }

	void Signal(IFence* fence, uint64 value) override
	{
		D3D12Fence* dxFence = static_cast<D3D12Fence*>(fence);
		queue->Signal(dxFence->fence, value);
	}

	void ExecuteCommandLists(ICommandList** commandLists, uint32 numCommands) override
	{
		ID3D12GraphicsCommandList6* dxCommandLists[16];
		for (uint32 i = 0; i < numCommands; i++)
		{
			dxCommandLists[i] = static_cast<D3D12CommandList*>(commandLists[i])->m_CmdList;
		}
		queue->ExecuteCommandLists(numCommands, (ID3D12CommandList* const*)dxCommandLists);
	}
};