#include "D3D12SwapChain.hpp"
#include "D3D12Context.hpp"

D3D12SwapChain::D3D12SwapChain(const DX12SwapChainDesc& arguments)
{
	m_Device = arguments.device;

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = arguments.width;
	swapchainDesc.Height = arguments.height;
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
		D3D12Image*& img = m_BackBuffers[i];
		img->DescriptorHandle = hRTV;
		hRTV.ptr += RTVDescSize;
		
		ID3D12Resource* pBackBuffer = NULL;
		DXCall(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)));
		m_Device->CreateRenderTargetView(pBackBuffer, nullptr, img->DescriptorHandle);
		img->Resource = pBackBuffer;
		D3D12SetName(img->Resource, "Swapchain Render Target %d", i);
	}
}

void D3D12SwapChain::Present(bool gsync, uint32 flags)
{
	m_SwapChain->Present(gsync, flags);
}

void D3D12SwapChain::Release()
{
	for (int i = 0; i < g_NumBackBuffers; i++)
	{
		m_BackBuffers[i]->Resource->Release();
	}
}

