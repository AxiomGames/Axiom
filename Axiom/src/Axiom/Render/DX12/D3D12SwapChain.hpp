#pragma once
#include "D3D12Core.hpp"
#include "D3D12CommonHeaders.hpp"

struct DX12SwapChainDesc
{
	int width, height;
	ID3D12Device8* device;
	IDXGIFactory7* factory;
	ID3D12CommandQueue* commandQueue;
	DXGI_FORMAT format;
	HWND hwnd;
};

class D3D12SwapChain
{
public:
	D3D12SwapChain(const DX12SwapChainDesc& swapChainDesc);

	void Release();
	void Present(bool gsync, uint32 flags);
	// getters
	ID3D12Resource* GetBackBufferResource(int index) 
	{ return m_RenderTargetResources[index]; }
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferDescriptorHandle(int index)
	{ return m_RenderTargetDescriptors[index]; }
	
	ID3D12DescriptorHeap** GetDescriptorHeapPtr() 
	{ return &m_DescriptorHeap; }
	
private:
	IDXGISwapChain4* m_SwapChain;
	ID3D12Device8* m_Device;
	ID3D12DescriptorHeap* m_DescriptorHeap;
	ID3D12Resource* m_RenderTargetResources[g_NumBackBuffers] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_RenderTargetDescriptors[g_NumBackBuffers] = {};
	ID3D12Fence* m_Fence;
};