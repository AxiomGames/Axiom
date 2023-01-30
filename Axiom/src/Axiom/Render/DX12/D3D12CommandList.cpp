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
