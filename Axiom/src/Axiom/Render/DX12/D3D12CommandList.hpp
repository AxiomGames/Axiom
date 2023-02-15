#pragma once
#include "../CommandList.hpp"

class ID3D12CommandList;

struct D3D12Shader : IShader
{
	ID3D12RootSignature* m_RootSignature;
};

class D3D12CommandList : ICommandList
{
public:
	void Initialize(IDeviceContext* deviceContext) override;
	void Close() override;
	void Reset(ICommandAllocator* commandAllocator) override;
	void Release() override;

	void SetPipelineState(IPipeline* pipeline) override;
	void DrawIndexedInstanced(int numIndex, int numInstance, int startIndex, int startVertex, int startInstance) override;
	void SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) override;
	void ClearRenderTarget(IImage* image, float color[4]) override;
	void SetRenderTargets(IImage** images, int numImages) override;

	void SetVertexBuffers(IBuffer** vertexBuffers, int numVertexBuffers) override;
	void SetIndexBuffers(IBuffer** indexBuffers, int numIndexBuffers) override;
	void SetViewport(const ViewportDesc& desc) override;
	void SetScissorRects(Rect** rects, int numRects) override;
	void Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) override;
private:
	ID3D12GraphicsCommandList6* m_CmdList = nullptr;
	ID3D12Device8* m_Device = nullptr;
};