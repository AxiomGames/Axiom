
#include <exception>
#include "D3D12Context.hpp"
#include "D3D12Core.hpp"
#include "D3D12CommandList.hpp"
#include "d3d12x.h"
#include <cassert>

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

	
	m_FenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	ax_assert(m_FenceEvent);

	windowSize = window->GetSize();

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

	switch (desc.ResourceUsage)
	{
	case EResourceUsage::IndexBuffer:
		buffer->indexBufferView.BufferLocation = buffer->mpSysMemBuffer->GetGPUVirtualAddress();
		buffer->indexBufferView.SizeInBytes = desc.Size;
		buffer->indexBufferView.Format = DX12::ToDX12Format(EImageFormat::R32U); // only uint vertices for now
		break;
	case EResourceUsage::VertexBuffer:
		buffer->vertexBufferView.BufferLocation = buffer->mpSysMemBuffer->GetGPUVirtualAddress();
		buffer->vertexBufferView.SizeInBytes = desc.Size;
		buffer->vertexBufferView.StrideInBytes = desc.ElementByteStride;
		break;
		default: assert(false && "unknown resource type for dx12 buffer creation");
	};

	// todo give a name
	// buffer->mpVidMemBuffer->SetName(L"AX vertex buffer vid mem");
	// buffer->mpSysMemBuffer->SetName(L"AX vertex buffer sys mem");
	return (IBuffer*)buffer;
}

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
	psoDesc.NumRenderTargets = info.numRenderTargets;
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
		local_layout[i].SemanticName = layout.name;
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
		desc.FrontCounterClockwise = false;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;
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
	outPipeline->info = info;
	return outPipeline;
}

ICommandAllocator* D3D12Context::CreateCommandAllocator(ECommandListType type)
{
	D3D12CommandAllocator* allocator = new D3D12CommandAllocator();
	m_Device->CreateCommandAllocator((D3D12_COMMAND_LIST_TYPE)type, IID_PPV_ARGS(&allocator->allocator));
	return static_cast<ICommandAllocator*>(allocator);
}

ICommandList* D3D12Context::CreateCommandList(ICommandAllocator* commandAllocator, ECommandListType type)
{
	D3D12CommandList* commandList = new D3D12CommandList();
	D3D12_COMMAND_LIST_TYPE d3dType = (D3D12_COMMAND_LIST_TYPE)type;
	DXCall(m_Device->CreateCommandList(0, d3dType, (ID3D12CommandAllocator*)commandAllocator, nullptr, IID_PPV_ARGS(&commandList->m_CmdList)));
	return static_cast<ICommandList*>(commandList);
}

ICommandQueue* D3D12Context::CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) 
{
	D3D12_COMMAND_QUEUE_DESC cmdQueuedesc{};
	cmdQueuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueuedesc.NodeMask = 0;
	cmdQueuedesc.Priority = pow((int)priority, 10);
	cmdQueuedesc.Type = (D3D12_COMMAND_LIST_TYPE)((uint32)type); // our enum is directly convertible to dx12 command list type enum
	D3D12CommandQueue* commandQueue;

	DXCall(m_Device->CreateCommandQueue(&cmdQueuedesc, IID_PPV_ARGS(&commandQueue->queue)));
	return static_cast<ICommandQueue*>(commandQueue);
}
	
ISwapChain* D3D12Context::CreateSwapChain(ICommandQueue* commandQueue, EImageFormat format)
{
	DX12SwapChainDesc swapchainDesc =
	{
		.width = windowSize.x,
		.height = windowSize.y,
		.device = m_Device,
		.factory = DXFactory,
		.commandQueue = (ID3D12CommandQueue*)commandQueue,
		.format = DX12::ToDX12Format(format),
		.hwnd = m_Window->GetHWND()
	};
	return new D3D12SwapChain(swapchainDesc);
}

IFence* D3D12Context::CreateFence()
{
	ID3D12Fence1* fence;
	DXCall(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	return (IFence*)fence;
}

void D3D12Context::WaitFence(IFence* fence)
{
	D3D12Fence* dxFence = (D3D12Fence*)fence;

	if (dxFence->fence->GetCompletedValue() < dxFence->value)
	{
		DXCall(dxFence->fence->SetEventOnCompletion(dxFence->value, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void D3D12Context::DestroyResource(IGraphicsResource* resource)
{
	resource->Release();
	delete resource;
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
	
D3D12Context::~D3D12Context()
{
	ax_assert(!m_CmdQueue && !m_CmdList && !m_Fence);
}

void D3D12Context::Release()
{
	ReleaseResource(DXFactory);

	CloseHandle(m_FenceEvent);
	m_FenceEvent = nullptr;

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
