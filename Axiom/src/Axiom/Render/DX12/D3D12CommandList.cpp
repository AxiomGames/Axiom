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
	D3D12Pipeline* dx12Pipeline = static_cast<D3D12Pipeline*>(pipeline);
	m_CmdList->SetPipelineState(dx12Pipeline->PipelineState);
	m_CmdList->SetGraphicsRootSignature(dx12Pipeline->RootSignature);
}

void D3D12CommandList::DrawIndexedInstanced(uint32 numIndex, uint32 numInstance, uint32 startIndex, int startVertex, uint32 startInstance) 
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
	barrier.Transition.StateBefore = DX12::ToDX12PipelineStage(pBarrier.CurrentStage);
	barrier.Transition.StateAfter = DX12::ToDX12PipelineStage(pBarrier.NextStage);
	m_CmdList->ResourceBarrier(1, &barrier);
}


void D3D12CommandList::SetImageBarrier(IImage* pImage, const PipelineBarrier& pBarrier) 
{
	D3D12Image* dxBuffer = (D3D12Image*)pImage;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = dxBuffer->Resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = DX12::ToDX12PipelineStage(pBarrier.CurrentStage);
	barrier.Transition.StateAfter = DX12::ToDX12PipelineStage(pBarrier.NextStage);
	m_CmdList->ResourceBarrier(1, &barrier);
}

void D3D12CommandList::ClearRenderTarget(IImage* pImage, float color[4])
{
	D3D12Image* dxBuffer = (D3D12Image*)pImage;
	m_CmdList->ClearRenderTargetView(dxBuffer->DescriptorHandle, color, 0, 0);
}

// todo add depth render target
void D3D12CommandList::SetRenderTargets(IImage** images, uint32 numImages)
{
	D3D12_CPU_DESCRIPTOR_HANDLE descHandles[16];
	for (int i = 0; i < numImages; ++i)
	{
		descHandles[i] = static_cast<D3D12Image*>(images[i])->DescriptorHandle;
	}
	m_CmdList->OMSetRenderTargets((UINT)numImages, descHandles, false, nullptr);
}

void D3D12CommandList::SetVertexBuffers(IBuffer** vertexBuffers, uint32 numVertexBuffers) 
{
	D3D12_VERTEX_BUFFER_VIEW views[16];

	for (int i = 0; i < numVertexBuffers; ++i)
	{
		views[i] = static_cast<D3D12Buffer*>(vertexBuffers[i])->vertexBufferView;
	}
	m_CmdList->IASetVertexBuffers(0, numVertexBuffers, views);
}

void D3D12CommandList::SetIndexBuffer(IBuffer* indexBuffer)  
{
	m_CmdList->IASetIndexBuffer(&static_cast<D3D12Buffer*>(indexBuffer)->indexBufferView);
}

void D3D12CommandList::SetViewport(const ViewportDesc& desc) 
{
	m_CmdList->RSSetViewports(1, (const D3D12_VIEWPORT*)(&desc));
}

void D3D12CommandList::SetScissorRects(GraphicsRect* rects, uint32 numRects)
{
	m_CmdList->RSSetScissorRects(numRects, (const D3D12_RECT*)(&rects));
}

void D3D12CommandList::Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) 
{
	m_CmdList->Dispatch(groupX, groupY, groupZ);
}
