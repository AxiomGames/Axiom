#include "D3D12Context.hpp"

Vector2i windowSize;

D3D12Context::D3D12Context(ID3D12Device8* device, SharedPtr<GLFWNativeWindow> window, IDXGIFactory7* DXFactory, ID3D12Device8* MainDevice)
{
	HRESULT hr = S_OK;
	const D3D12_COMMAND_LIST_TYPE cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC cmdQueuedesc{};
	cmdQueuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueuedesc.NodeMask = 0;
	cmdQueuedesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueuedesc.Type = cmdType;

	DXCall(device->CreateCommandQueue(&cmdQueuedesc, IID_PPV_ARGS(&m_CmdQueue)));

	for (uint32 i = 0; i < g_NumBackBuffers; ++i)
	{
		D3D12CommandFrame& cmdFrame = m_CmdFrames[i];
		DXCall(device->CreateCommandAllocator(cmdType, IID_PPV_ARGS(&cmdFrame.CommandAllocator)));
	}

	DXCall(device->CreateCommandList(0, cmdType, m_CmdFrames[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_CmdList)));

	DXCall(m_CmdList->Close());

	DXCall(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	m_FenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	ax_assert(m_FenceEvent);

	windowSize = window->GetSize();

	// create swapchain
	SwapChainDesc swapchainDesc =
	{
		.width = window->GetWidth(),
		.height = window->GetHeight(),
		.device = MainDevice,
		.factory = DXFactory,
		.commandQueue = m_CmdQueue,
		.format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.hwnd = window->GetHWND()
	};

	m_SwapChain = new D3D12SwapChain(swapchainDesc);

	DXCall(m_CmdList->Reset(m_CmdFrames[0].CommandAllocator, nullptr));

	m_Renderer = new DX12Renderer(device, *m_SwapChain->GetDescriptorHeapPtr(), m_CmdList);
	m_CmdList->Close();

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DXCall(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)) != S_OK);
}

D3D12Context::~D3D12Context()
{
	ax_assert(!m_CmdQueue && !m_CmdList && !m_Fence);
}

void D3D12Context::Flush()
{
	for (uint32 i = 0; i < g_NumBackBuffers; ++i)
	{
		D3D12CommandFrame& cmdFrame = m_CmdFrames[i];
		cmdFrame.Wait(m_FenceEvent, m_Fence);
	}
	m_FrameIndex = 0;
}

void D3D12Context::BeginFrame()
{
	D3D12CommandFrame& frame = m_CmdFrames[m_FrameIndex];

	// Wait for GPU
	frame.Wait(m_FenceEvent, m_Fence);

	DXCall(frame.CommandAllocator->Reset());
	DXCall(m_CmdList->Reset(frame.CommandAllocator, nullptr));
}

void D3D12Context::Render()
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_SwapChain->GetBackBufferResource(m_FrameIndex);
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CmdList->ResourceBarrier(1, &barrier);

	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	auto backBufferDescriptorHandle = m_SwapChain->GetBackBufferDescriptorHandle(m_FrameIndex);
	m_CmdList->ClearRenderTargetView(backBufferDescriptorHandle, clearColor, 0, nullptr);
	m_CmdList->OMSetRenderTargets(1, &backBufferDescriptorHandle, false, nullptr);
	m_CmdList->SetDescriptorHeaps(1, &m_DescriptorHeap);

	CD3DX12_VIEWPORT viewport(0.0f, 0.0f, (float)windowSize.x, (float)windowSize.y);

	m_Renderer->Render(viewport);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CmdList->ResourceBarrier(1, &barrier);
	m_CmdList->Close();

	// This is for future where we can first create command lists in other threads and then supply it here
	ID3D12CommandList* commandLists[]{ m_CmdList };
	m_CmdQueue->ExecuteCommandLists(_countof(commandLists), &commandLists[0]);

	m_SwapChain->Present(false, 0);
}

void D3D12Context::EndFrame()
{
	m_FenceValue++;
	D3D12CommandFrame& frame = m_CmdFrames[m_FrameIndex];
	frame.FenceValue = m_FenceValue;
	m_CmdQueue->Signal(m_Fence, m_FenceValue);

	m_FrameIndex = (m_FrameIndex + 1) % g_NumBackBuffers;
}

void D3D12Context::Release()
{
	Flush();
	ReleaseResource(m_Fence);
	m_FenceValue = 0;

	CloseHandle(m_FenceEvent);
	m_FenceEvent = nullptr;

	ReleaseResource(m_CmdQueue);
	ReleaseResource(m_CmdList);

	for (uint32 i = 0; i < g_NumBackBuffers; ++i)
	{
		m_CmdFrames[i].Release();
	}
}