#include "D3D12CommandList.hpp"
#include "D3D12CommonHeaders.hpp"
#include "d3d12x.h"
#include "../DeviceContext.hpp"
#include "D3D12Context.hpp"

void D3D12CommandList::Close() 
{
	m_CmdList->Close();
}

void D3D12CommandList::Reset(ICommandAllocator* commandAllocator, IPipeline* pipeline) 
{
	DXCall(
		m_CmdList->Reset(
			static_cast<D3D12CommandAllocator*>(commandAllocator)->allocator, 
			!pipeline ? nullptr : static_cast<D3D12Pipeline*>(pipeline)->PipelineState));
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
	D3D12Buffer* dxBuffer = static_cast<D3D12Buffer*>(pBuffer);

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
	D3D12Image* dxBuffer = static_cast<D3D12Image*>(pImage);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = dxBuffer->Resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = DX12::ToDX12PipelineStage(pBarrier.CurrentStage);
	barrier.Transition.StateAfter = DX12::ToDX12PipelineStage(pBarrier.NextStage);
	m_CmdList->ResourceBarrier(1, &barrier);
}

void D3D12CommandList::ClearRenderTarget(IImage* image, float color[4])
{
	D3D12Image* dxBuffer = static_cast<D3D12Image*>(image);
	m_CmdList->ClearRenderTargetView(dxBuffer->DescriptorHandle, color, 0, 0);
}

void D3D12CommandList::ClearDepthStencil(IImage* image)
{
    D3D12Image* dxBuffer = static_cast<D3D12Image*>(image);
    m_CmdList->ClearDepthStencilView(dxBuffer->DescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

// todo add depth render target
void D3D12CommandList::SetRenderTargets(IImage** images, uint32 numImages, IImage* depthStencil)
{
	D3D12_CPU_DESCRIPTOR_HANDLE descHandles[16];
	for (int i = 0; i < numImages; ++i)
	{
		descHandles[i] = static_cast<D3D12Image*>(images[i])->DescriptorHandle;
	}
    m_CmdList->OMSetRenderTargets(numImages, descHandles, false, depthStencil ? &((D3D12Image*)depthStencil)->DescriptorHandle : nullptr);
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
	m_CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3D12CommandList::SetViewports(uint32 numViewports, const ViewportDesc* desc)
{
	m_CmdList->RSSetViewports(numViewports, (const D3D12_VIEWPORT*)(desc));
}

void D3D12CommandList::SetScissorRects(uint32 numRects, GraphicsRect* rects)
{
	m_CmdList->RSSetScissorRects(numRects, (const D3D12_RECT*)(rects));
}

void D3D12CommandList::SetGraphicsPushConstants(IPipeline* pipeline, EShaderType stage, void* data, size_t size)
{
    m_CmdList->SetGraphicsRoot32BitConstants(0, size / sizeof(uint32), data, 0);
}

void D3D12CommandList::Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ) 
{
	m_CmdList->Dispatch(groupX, groupY, groupZ);
}
