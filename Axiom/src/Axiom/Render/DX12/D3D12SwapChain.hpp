#pragma once
#include "D3D12Core.hpp"
#include "D3D12CommonHeaders.hpp"

struct DX12SwapChainDesc
{
	int Width, Height;
	ID3D12Device8* device;
	IDXGIFactory7* factory;
	ID3D12CommandQueue* commandQueue;
	DXGI_FORMAT format;
	HWND hwnd;
};

class D3D12Image;

class D3D12SwapChain : public ISwapChain
{
public:
	D3D12SwapChain(const DX12SwapChainDesc& swapChainDesc);

	void Release() override;
	void Present(bool gsync, uint32 flags) override;
	uint32 GetCurrentBackBufferIndex() override { return m_SwapChain->GetCurrentBackBufferIndex(); }

	IImage* GetBackBuffer(int index) override
	{  return (IImage*)m_BackBuffers[index]; }

    IImage* GetDepthStencilBuffer() override
    { return (IImage*)m_DepthStencilBuffer; }

	ID3D12DescriptorHeap** GetDescriptorHeapPtr() 
	{ return &m_DescriptorHeap; }
	
private:
	IDXGISwapChain4* m_SwapChain;
	ID3D12Device8* m_Device;
	ID3D12DescriptorHeap* m_DescriptorHeap;
	D3D12Image* m_BackBuffers[g_NumBackBuffers] = {};
    D3D12Image* m_DepthStencilBuffer;
	ID3D12Fence* m_Fence;
    ID3D12DescriptorHeap* m_depthStencilDescriptorHeap;
};