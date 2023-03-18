#include "D3D12SwapChain.hpp"
#include "D3D12Context.hpp"
#include "d3d12x.h"

D3D12SwapChain::D3D12SwapChain(const DX12SwapChainDesc& arguments)
{
    m_Device = arguments.device;

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = arguments.Width;
	swapchainDesc.Height = arguments.Height;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = g_NumBackBuffers;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Format = arguments.format;
	swapchainDesc.Flags = 0; //desc.bVSync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING

	IDXGISwapChain1* swapChain1;

	DXCall(arguments.factory->CreateSwapChainForHwnd(
		arguments.commandQueue,
		arguments.hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapChain1
	));

	// remove comptr referance and use as member 
	swapChain1->QueryInterface(IID_PPV_ARGS(&this->m_SwapChain));
	swapChain1->Release();

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.NumDescriptors = g_NumBackBuffers;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	DXCall(m_Device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));
	
	// initialize render targets
	const UINT RTVDescSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE hRTV{ m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	for (int i = 0; i < g_NumBackBuffers; i++)
	{
		D3D12Image* img = new D3D12Image();
		m_BackBuffers[i] = img;
		img->CPUDescHandle = hRTV;
		
		DXCall(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&img->VidMemBuffer)));
		m_Device->CreateRenderTargetView(img->VidMemBuffer, nullptr, img->CPUDescHandle);
	
		hRTV.ptr += RTVDescSize;
		D3D12SetName(img->VidMemBuffer, "Swapchain Render Target %d", i);
	}

    // create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DXCall(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_depthStencilDescriptorHeap)));
    
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT, 
        arguments.Width,
        arguments.Height, 1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    );

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;
    
	m_DepthStencilBuffer = new D3D12Image();

    m_Device->CreateCommittedResource(
      &heapProperties,
      D3D12_HEAP_FLAG_NONE,
      &resourceDesc,
      D3D12_RESOURCE_STATE_DEPTH_WRITE,
      &clearValue,
      IID_PPV_ARGS(&m_DepthStencilBuffer->VidMemBuffer)
    );

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_DepthStencilBuffer->CPUDescHandle = m_depthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    m_Device->CreateDepthStencilView(
		m_DepthStencilBuffer->VidMemBuffer, &depthStencilDesc,
        m_DepthStencilBuffer->CPUDescHandle);
}

void D3D12SwapChain::Present(bool gsync, uint32 flags)
{
	m_SwapChain->Present(gsync, flags);
}

void D3D12SwapChain::Release()
{
	for (int i = 0; i < g_NumBackBuffers; i++)
	{
		m_BackBuffers[i]->VidMemBuffer->Release();
        delete m_BackBuffers[i];
	}
    
    m_DepthStencilBuffer->VidMemBuffer->Release();
    delete m_DepthStencilBuffer;

	m_depthStencilDescriptorHeap->Release();
}

