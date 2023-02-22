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

class D3D12SwapChain : public ISwapChain
{
public:
	D3D12SwapChain(const DX12SwapChainDesc& swapChainDesc);

	void Release() override;
	void Present(bool gsync, uint32 flags) override;
	
	IImage* GetBackBuffer(int index) override
	{  return (IImage*)m_BackBuffers[index]; }
	
	ID3D12DescriptorHeap** GetDescriptorHeapPtr() 
	{ return &m_DescriptorHeap; }
	
private:
	IDXGISwapChain4* m_SwapChain;
	ID3D12Device8* m_Device;
	ID3D12DescriptorHeap* m_DescriptorHeap;
	D3D12Image* m_BackBuffers[g_NumBackBuffers] = {};
	ID3D12Fence* m_Fence;
};