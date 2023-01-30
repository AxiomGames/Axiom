
#include "D3D12Context.hpp"
#include "D3D12Core.hpp"
#include <exception>
#include "d3d12x.h"

#include "../Shader.hpp"

struct D3D12Shader : IShader
{

};

// view.BufferLocation = mpSysMemBuffer->GetGPUVirtualAddress();
// view.SizeInBytes = sizeof(T) * size;
// view.Format = TypeToFormat();

struct D3D12Buffer : IBuffer
{
	ID3D12Resource* mpVidMemBuffer;
	ID3D12Resource* mpSysMemBuffer;
};

Vector2i windowSize;

void D3D12Context::Initialize(SharedPtr<INativeWindow> window)
{
	DX12::InitializeDXFactory(&DXFactory);
	IDXGIAdapter4* adapter = DX12::DetermineMainAdapter(DXFactory);
	D3D_FEATURE_LEVEL maxFeatureLevel = DX12::GetMaxFeatureLevel(adapter);

	DXCall(D3D12CreateDevice(adapter, maxFeatureLevel, IID_PPV_ARGS(&m_Device)));

	m_Device->SetName(L"MAIN DEVICE");

#ifdef _DEBUG
	{
		ComPtr<ID3D12InfoQueue> infoQueue;
		DXCall(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	};
#endif

	DXCall(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	m_FenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	ax_assert(m_FenceEvent);

	windowSize = window->GetSize();

	// create rendering commandQueue
	D3D12_COMMAND_QUEUE_DESC cmdQueuedesc{};
	cmdQueuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueuedesc.NodeMask = 0;
	cmdQueuedesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueuedesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DXCall(m_Device->CreateCommandQueue(&cmdQueuedesc, IID_PPV_ARGS(&m_CmdQueue)));

	// create swapchain
	DX12SwapChainDesc swapchainDesc =
	{
		.width = window->GetWidth(),
		.height = window->GetHeight(),
		.device = m_Device,
		.factory = DXFactory,
		.commandQueue = m_CmdQueue,
		.format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.hwnd = window->GetHWND()
	};

	m_SwapChain = new D3D12SwapChain(swapchainDesc);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DXCall(m_Device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)) != S_OK);
}

IBuffer* D3D12Context::CreateBuffer(const BufferDesc& desc, ICommandList* commandList) 
{
	D3D12Buffer* buffer = new D3D12Buffer();

	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(desc.Size, D3D12_RESOURCE_FLAG_NONE); // todo add options instead of none
	D3D12_RESOURCE_STATES resourceState = DX12::ToDX12ResourceUsage(desc.ResourceUsage);
	ID3D12GraphicsCommandList6* pCmd = (ID3D12GraphicsCommandList6*)commandList;

	DXCall(m_Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		resourceState,
		nullptr, IID_PPV_ARGS(&buffer->mpVidMemBuffer)
	));

	auto sysHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	DXCall(m_Device->CreateCommittedResource(
		&sysHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&buffer->mpSysMemBuffer)
	));

	// copy to sys
	void* mappedMem;
	D3D12_RANGE range;
	buffer->mpSysMemBuffer->Map(0, &range, &mappedMem);
	memcpy(mappedMem, desc.Data, desc.Size);
	buffer->mpSysMemBuffer->Unmap(0, &range);

	// upload to gpu
	auto transion0 = CD3DX12_RESOURCE_BARRIER::Transition(buffer->mpVidMemBuffer,
		resourceState,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	pCmd->ResourceBarrier(1, &transion0);

	pCmd->CopyBufferRegion(buffer->mpVidMemBuffer, 0, buffer->mpSysMemBuffer, 0, desc.Size);

	auto transion1 = CD3DX12_RESOURCE_BARRIER::Transition(buffer->mpVidMemBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		resourceState
	);
	
	pCmd->ResourceBarrier(1, &transion1);
	// todo give a name
	// buffer->mpVidMemBuffer->SetName(L"AX vertex buffer vid mem");
	// buffer->mpSysMemBuffer->SetName(L"AX vertex buffer sys mem");
	return (IBuffer*)buffer;
}

void D3D12Context::DestroyBuffer(IBuffer* buffer) 
{
	D3D12Buffer* dxBuffer = (D3D12Buffer*)buffer;
	if (dxBuffer->BufferData)
	{
		ReleaseResource(dxBuffer->mpVidMemBuffer);
		ReleaseResource(dxBuffer->mpSysMemBuffer);
		// free(dxBuffer->BufferData); // todo something else
	}
}

ENUM_FLAGS(D3D12_ROOT_SIGNATURE_FLAGS, uint32)

IPipeline* D3D12Context::CreateGraphicsPipeline(PipelineInfo& info)
{
	D3D12Pipeline* outPipeline = new D3D12Pipeline();
	HRESULT hr;

	// Create the root signature
	{
		using RSFlag = D3D12_ROOT_SIGNATURE_FLAGS;
		RSFlag flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		flags &= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		flags |= (RSFlag)(uint32(info.HullShader == nullptr) * (uint32)D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
		flags |= (RSFlag)(uint32(info.DomainShader == nullptr) * (uint32)D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
		flags |= (RSFlag)(uint32(info.GeometryShader == nullptr) * (uint32)D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
		
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, flags);

		ID3DBlob* signature = nullptr;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
		if (FAILED(hr)) throw std::exception("root signature creation failed!");
		hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&outPipeline->RootSignature));
		if (FAILED(hr)) throw std::exception("root signature creation failed!");
	}

	const uint32 msaaQuality = 0; // D3D12Backend::GetMSAAQuality();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	memset(&psoDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.NodeMask = 1;
	psoDesc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)info.primitiveType;
	psoDesc.pRootSignature = outPipeline->RootSignature;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = 1;
	psoDesc.DSVFormat = DX12::ToDX12Format(info.DepthStencilFormat);
	psoDesc.SampleDesc.Quality = msaaQuality ? (msaaQuality - 1) : 0;
	psoDesc.SampleDesc.Count = msaaQuality ? msaaQuality : 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	
	constexpr D3D12_BLEND blendFactorLUT[] = {
		D3D12_BLEND_ZERO,             
		D3D12_BLEND_ONE,              
		D3D12_BLEND_SRC_COLOR,        
		D3D12_BLEND_INV_SRC_COLOR,    
		D3D12_BLEND_SRC_ALPHA,        
		D3D12_BLEND_INV_SRC_ALPHA,    
		D3D12_BLEND_DEST_ALPHA,       
		D3D12_BLEND_INV_DEST_ALPHA,   
		D3D12_BLEND_DEST_COLOR,       
		D3D12_BLEND_INV_DEST_COLOR,   
		D3D12_BLEND_SRC_ALPHA_SAT,    
		D3D12_BLEND_BLEND_FACTOR,     
		D3D12_BLEND_INV_BLEND_FACTOR, 
		D3D12_BLEND_SRC1_COLOR,       
		D3D12_BLEND_INV_SRC1_COLOR,   
		D3D12_BLEND_SRC1_ALPHA,       
		D3D12_BLEND_INV_SRC1_ALPHA,   
	};

	D3D12_BLEND_DESC& blendDesc = psoDesc.BlendState;
	blendDesc.AlphaToCoverageEnable = info.AlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = info.IndependentBlendEnable;

	for (int i = 0; i < info.numRenderTargets; ++i)
	{
		psoDesc.RTVFormats[i] = DX12::ToDX12Format(info.RTVFormats[i]);
		BlendDesc& bDesc = info.RenderTargetBlendDescs[i];
		blendDesc.RenderTarget[i].BlendEnable = bDesc.BlendEnable;
		blendDesc.RenderTarget[i].SrcBlend = blendFactorLUT[(uint32)bDesc.SrcBlend];
		blendDesc.RenderTarget[i].DestBlend = blendFactorLUT[(uint32)bDesc.DestBlend];
		blendDesc.RenderTarget[i].SrcBlendAlpha = blendFactorLUT[(uint32)bDesc.SrcBlendAlpha];
		blendDesc.RenderTarget[i].DestBlendAlpha= blendFactorLUT[(uint32)bDesc.DestBlendAlpha];
		blendDesc.RenderTarget[i].BlendOp = (D3D12_BLEND_OP)(bDesc.BlendOp);
		blendDesc.RenderTarget[i].BlendOpAlpha = (D3D12_BLEND_OP)(bDesc.BlendOpAlpha);
	}

	psoDesc.VS = *(D3D12_SHADER_BYTECODE*)(&info.VertexShader->byteCode); // convert IShader::ByteCode to D3D12_SHADER_BYTECODE these structs are identical

	D3D12_INPUT_ELEMENT_DESC local_layout[16] = {};

	for (size_t i = 0ull, offset = 0ull; i < info.numInputLayout; ++i)
	{
		InputLayout& layout = info.inputLayouts[i];
		uint32 elemSize = VertexAttribSize(layout.Type);
		local_layout[i].SemanticName = layout.name.data;
		local_layout[i].SemanticIndex = 0;
		local_layout[i].Format = DX12::ToDX12Format(layout.Type);
		local_layout[i].InputSlot = 0;
		local_layout[i].AlignedByteOffset = offset;
		local_layout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		local_layout[i].InstanceDataStepRate = 0; // not used
		offset += elemSize;
	}

	psoDesc.InputLayout = { local_layout, _countof(local_layout) };

	psoDesc.PS = *(D3D12_SHADER_BYTECODE*)(&info.FragmentShader->byteCode);

	// Create the rasterizer state
	{
		D3D12_RASTERIZER_DESC& desc = psoDesc.RasterizerState;
		desc.FillMode = (D3D12_FILL_MODE)info.fillMode;
		desc.CullMode = (D3D12_CULL_MODE)info.cullMode;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		desc.ForcedSampleCount = 0;
		desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	// Create depth-stencil State
	if (info.DepthStencilFormat != EImageFormat::UNKNOWN)
	{
		D3D12_DEPTH_STENCIL_DESC& desc = psoDesc.DepthStencilState;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.BackFace = desc.FrontFace;
	}

	DXCall(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&outPipeline->PipelineState)));
	return outPipeline;
}

ICommandAllocator* D3D12Context::CreateCommandAllocator(ECommandListType type)
{
	ID3D12CommandAllocator* allocator;
	m_Device->CreateCommandAllocator((D3D12_COMMAND_LIST_TYPE)type, IID_PPV_ARGS(&allocator));
	return (ICommandAllocator*)allocator;
}

ICommandList* D3D12Context::CreateCommandList(ICommandAllocator* commandAllocator, ECommandListType type)
{
	ID3D12CommandList* commandList;
	D3D12_COMMAND_LIST_TYPE d3dType = (D3D12_COMMAND_LIST_TYPE)type;
	DXCall(m_Device->CreateCommandList(0, d3dType, (ID3D12CommandAllocator*)commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
	return (ICommandList*)commandList;
}

ICommandQueue* D3D12Context::CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) 
{
	D3D12_COMMAND_QUEUE_DESC cmdQueuedesc{};
	cmdQueuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueuedesc.NodeMask = 0;
	cmdQueuedesc.Priority = pow((int)priority, 10);
	cmdQueuedesc.Type = (D3D12_COMMAND_LIST_TYPE)((uint32)type); // our enum is directly convertible to dx12 command list type enum
	ID3D12CommandQueue* commandQueue;

	DXCall(m_Device->CreateCommandQueue(&cmdQueuedesc, IID_PPV_ARGS(&commandQueue)));
	return (ICommandQueue*)commandQueue;
}
	
IShader* D3D12Context::CreateShader(const char* sourceCode, const char* functionName, EShaderType shaderType)
{
	D3D12Shader* shader = new D3D12Shader();
	ID3DBlob* errorBlob, *blob;
#ifdef  DEBUG
	D3D_SHADER_MACRO shaderMacros[] = { {"DEBUG", "1"}, {nullptr, nullptr} };
#else 
	D3D_SHADER_MACRO shaderMacros[] = { {"RELEASE", "1"}, {nullptr, nullptr} };
#endif //  DEBUG

	if (FAILED(D3DCompile(sourceCode, strlen(sourceCode), nullptr,
		shaderMacros, nullptr, functionName, "vs_5_0", 0, 0, &blob, &errorBlob)))
	{
		AX_ERROR("Shader Compiling Error:\n %s", (char*)errorBlob->GetBufferPointer());
		throw std::exception("shader compiling failed!");
	}
	shader->byteCode.blobSize = blob->GetBufferSize();
	shader->byteCode.blob = (void*)blob;
	shader->sourceCode = sourceCode;
	return (IShader*)shader;
}

void D3D12Context::DestroyShader(IShader* shader) 
{
	// ReleaseResource(vertexShaderBlob); 
}
	
void D3D12Context::SetBufferBarrier(IBuffer* pBuffer, const PipelineBarrier& pBarrier) 
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_SwapChain->GetBackBufferResource(m_FrameIndex);
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
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

void D3D12Context::Render(ICommandList** commandLists, size_t numCommandLists)
{
	// todo record command lists in other threads and push with this function
	if constexpr (false)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_SwapChain->GetBackBufferResource(m_FrameIndex);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_CmdList->ResourceBarrier(1, &barrier);

		float clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
		auto backBufferDescriptorHandle = m_SwapChain->GetBackBufferDescriptorHandle(m_FrameIndex);
		m_CmdList->ClearRenderTargetView(backBufferDescriptorHandle, clearColor, 0, nullptr);
		m_CmdList->OMSetRenderTargets(1, &backBufferDescriptorHandle, false, nullptr);
		m_CmdList->SetDescriptorHeaps(1, &m_DescriptorHeap);

		CD3DX12_VIEWPORT viewport(0.0f, 0.0f, (float)windowSize.x, (float)windowSize.y);

		// m_CommandList->RSSetViewports(1, &vp);
		// m_CommandList->RSSetScissorRects(1, &rect);
		// m_CommandList->IASetVertexBuffers(0, 1, m_VertexBuffer->GetViewPtr());
		// m_CommandList->IASetIndexBuffer(m_IndexBuffer->GetViewPtr());
		// m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// m_CommandList->SetPipelineState(m_PipelineState);
		// m_CommandList->SetGraphicsRootSignature(m_RootSignature);
		// 
		// const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		// m_CommandList->OMSetBlendFactor(blend_factor);
		// 
		// m_CommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_CmdList->ResourceBarrier(1, &barrier);
		m_CmdList->Close();
	}

	m_CmdQueue->ExecuteCommandLists(numCommandLists, (ID3D12CommandList**)commandLists);

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
	ReleaseResource(DXFactory);

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

#ifdef _DEBUG
	{
		ComPtr<ID3D12InfoQueue> infoQueue;
		DXCall(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
	};

	ComPtr<ID3D12DebugDevice2> debugDevice;
	DXCall(m_Device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
	ReleaseResource(m_Device);
	DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
#else
	ReleaseResource(m_Device);
#endif
}