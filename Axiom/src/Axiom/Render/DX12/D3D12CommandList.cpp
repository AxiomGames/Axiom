#include "D3D12CommandList.hpp"
#include "D3D12CommonHeaders.hpp"
#include "d3d12x.h"
#include "../DeviceContext.hpp"
#include "D3D12Context.hpp"

void D3D12CommandList::Close() 
{
	m_CmdList->Close();
}

void D3D12CommandList::Reset(ICommandAllocator* commandAllocator) 
{
	m_CmdList->Reset((ID3D12CommandAllocator*)commandAllocator, nullptr);
}

void D3D12CommandList::Release() 
{
	m_CmdList->Release();
}

void D3D12CommandList::Initialize(IDeviceContext* deviceContext)
{
	D3D12Context* context = (D3D12Context*)deviceContext;
	m_Device = context->GetDevice();
}

void D3D12CommandList::SetPipelineState(IPipeline* pipeline) 
{
	m_CmdList->SetPipelineState();
}

void D3D12CommandList::DrawIndexedInstanced(int numIndex, int numInstance, int startIndex, int startVertex, int startInstance) 
{
	m_CmdList->DrawIndexedInstanced(numIndex, numInstance, startIndex, startVertex, startInstance);
}

void D3D12CommandList::SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) 
{
	D3D12Buffer* dxBuffer = (D3D12Buffer*)pBuffer;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = dxBuffer->mpVidMemBuffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = DX12::ToDX12ResourceUsage(pBarrier.CurrentStage);
	barrier.Transition.StateAfter = DX12::ToDX12ResourceUsage(pBarrier.NextStage);
	m_CmdList->ResourceBarrier(1, &barrier);
}


void D3D12CommandList::SetImageBarrier(IImage* pBuffer, const PipelineBarrier& pBarrier) 
{
	D3D12Image* dxBuffer = (D3D12Image*)pBuffer;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = dxBuffer->Resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = DX12::ToDX12ResourceUsage(pBarrier.CurrentStage);
	barrier.Transition.StateAfter = DX12::ToDX12ResourceUsage(pBarrier.NextStage);
	m_CmdList->ResourceBarrier(1, &barrier);
}

void ClearRenderTarget(IImage* image, float color[4])
{
	
}

void SetRenderTargets(IImage** images, int numImages)
{
	
}

void D3D12CommandList::SetVertexBuffers(IBuffer** vertexBuffers, int numVertexBuffers) 
{
	m_CmdList->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers);
}

void D3D12CommandList::SetIndexBuffers(IBuffer** indexBuffers, int numIndexBuffers)  
{
	m_CmdList->IASetIndexBuffers(0, numIndexBuffers, indexBuffers);
}

void D3D12CommandList::SetViewport(const ViewportDesc& desc) 
{
	m_CmdList->RSSetViewports(1, (const D3D12_VIEWPORT*)(&desc));
}

void D3D12CommandList::SetScissorRects(Rect** rects, int numRects) 
{
	m_CmdList->RSSetViewports(1, *(const D3D12_VIEWPORT**)(&rects));
}

void D3D12CommandList::Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) 
{
	m_CmdList->Dispatch(groupX, groupY, groupZ);
}
