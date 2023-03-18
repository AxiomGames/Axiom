
#include "D3D12Context.hpp"
#include "D3D12Core.hpp"
#include "d3d12x.h"
#include "D3D12MemoryAllocator.hpp"

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
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
		DXCall(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif
	
	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
	ax_assert(m_FenceEvent);

	windowSize = window->GetSize();
    m_Window = window;
    
    D3D12MA::ALLOCATOR_DESC allocatorDesc{};
    allocatorDesc.pDevice = m_Device;
    allocatorDesc.pAdapter = adapter;
	allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;
    DXCall(D3D12MA::CreateAllocator(&allocatorDesc, &m_Allocator));
    // create descriptor heaps for each heap type cbv_srv_uav, sampler, rtv, dsv
    // this heaps will going to be inside of d3d12 context because creating rhi for this is hard not compatible with vk, 
    // so since we create this heaps here these are not thread safe
    
    LPCWSTR names[] = { L"DX12Ctx CBV_SRV_UAV", L"DX12Ctx Sampler", L"DX12Ctx RTV", L"DX12Ctx DSV" };

    // create cbv_srv_uav heaps for each frame, number of descriptors are now restricted to 32
    for (int i = 0; i < g_NumBackBuffers; ++i)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors = 32;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; 
    
        DXCall(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_CBV_SRV_UAV_HEAPS[i].Heap)));
        m_CBV_SRV_UAV_HEAPS[i].IncrementSize = m_Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
		m_CBV_SRV_UAV_HEAPS[i].Heap->SetName(names[0]);
	}

    // start from 1 because cbv_srv_uav is created, this loop will create sampler, rtv, dsv descriptor heaps
    for (int i = 1; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)     
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)i;
        heapDesc.NumDescriptors = 32;
        heapDesc.Flags = (D3D12_DESCRIPTOR_HEAP_FLAGS)(i == 1); // if sampler, flag is shader_visible. otherwise flag is none
        DXCall(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorHeaps[i].Heap)));
        m_DescriptorHeaps[i].IncrementSize = m_Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
        m_DescriptorHeaps[i].Heap->SetName(names[i]);
    }
}

static D3D12_RESOURCE_DESC CreateResourceDesc(const BufferDesc& desc, bool isTexture)
{
    // texture arrays are not supported yet
    D3D12_RESOURCE_DESC bufferResourceDesc = {};
    bufferResourceDesc.Dimension = (D3D12_RESOURCE_DIMENSION)desc.BufferType;
    bufferResourceDesc.Alignment = 0;
    bufferResourceDesc.Width  = desc.Width;
    bufferResourceDesc.Height = desc.Height;
    bufferResourceDesc.DepthOrArraySize = 1;
    bufferResourceDesc.MipLevels = 1;
    bufferResourceDesc.Format = DX12::ToDX12Format(desc.Format);
    bufferResourceDesc.SampleDesc.Count = 1;
    bufferResourceDesc.SampleDesc.Quality = 0;
    bufferResourceDesc.Layout = isTexture ? D3D12_TEXTURE_LAYOUT_UNKNOWN : D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;    
    return bufferResourceDesc;
}

static void IncrementBufferPointer(D3D12Buffer* buffer, D3D12DescriptorHeap& descHeap)
{
    // incrementing heap offset here, we will add other buffers and increment the pointers
    buffer->CPUDescHandle = descHeap.Heap->GetCPUDescriptorHandleForHeapStart();
    buffer->GPUDescHandle = descHeap.Heap->GetGPUDescriptorHandleForHeapStart();
    buffer->CPUDescHandle.ptr += descHeap.IncrementSize * descHeap.Offset;    
    buffer->GPUDescHandle.ptr += descHeap.IncrementSize * descHeap.Offset;    
    descHeap.Offset++;
}

void D3D12Context::DirectUploadBuffer(BufferDesc& desc, D3D12Buffer* buffer, ID3D12GraphicsCommandList6* cmdList)
{
    D3D12MA::ALLOCATION_DESC bufferUploadAllocDesc = {};
    bufferUploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
    // bufferResourceDesc.Width = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    bool isTexture = uint32(desc.BufferType) > 1;
    D3D12_RESOURCE_DESC bufferResourceDesc = CreateResourceDesc(desc, isTexture);

    D3D12MA::Allocation* bufferUploadHeapAllocation = nullptr;
    DXCall(m_Allocator->CreateResource(
        &bufferUploadAllocDesc,
        &bufferResourceDesc, // resource description for a buffer
        D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
        nullptr,
        &bufferUploadHeapAllocation,
        IID_PPV_ARGS(&buffer->UploadResource)));
}

void D3D12Context::AllocateAndUploadBuffer(BufferDesc& desc, D3D12Buffer* buffer, ID3D12GraphicsCommandList6* pCmd)
{
    D3D12_RESOURCE_STATES resourceState = DX12::ToDX12ResourceUsage(desc.ResourceUsage);
    bool isTexture = uint32(desc.BufferType) > 1;
    
    // create default heap, default heap is memory on the GPU. Only the GPU has access to this memory
    // To get data into this heap, we will have to upload the data using an upload heap
    D3D12_RESOURCE_DESC bufferResourceDesc = CreateResourceDesc(desc, isTexture);

    D3D12MA::ALLOCATION_DESC bufferAllocDesc = {};
    bufferAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
    DXCall(m_Allocator->CreateResource(
    	&bufferAllocDesc,
    	&bufferResourceDesc, // resource description for a buffer
    	D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
    	// from the upload heap to this heap
    	nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
    	&buffer->VidAlloc,
    	IID_PPV_ARGS(&buffer->VidMemBuffer)));
    

    if (isTexture) 
    {
        // todo mipmaps
        m_Device->GetCopyableFootprints(&bufferResourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &bufferResourceDesc.Width); 
        bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    	bufferResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    	bufferResourceDesc.Height = 1;
    	bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    }
    
    // create upload heap. upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
    // We will upload the buffer using this heap to the default heap
    D3D12MA::ALLOCATION_DESC bufferUploadAllocDesc = {};
    bufferUploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
    
    D3D12MA::Allocation* bufferUploadHeapAllocation = nullptr;
    DXCall(m_Allocator->CreateResource(
    	&bufferUploadAllocDesc,
    	&bufferResourceDesc, // resource description for a buffer
    	D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
    	nullptr,
    	&bufferUploadHeapAllocation,
        IID_PPV_ARGS(&buffer->UploadResource)));
    
    
    // store buffer in upload heap
    D3D12_SUBRESOURCE_DATA resourceData = {};
    resourceData.pData = desc.Data; // pointer to our data
    resourceData.RowPitch = isTexture ? desc.Width * GetImageFormatSize(desc.Format) : desc.Width; // size of all data
    resourceData.SlicePitch = isTexture ? resourceData.RowPitch * desc.Height : desc.Width; // also the size of data
    
    // we are now creating a command with the command list to copy the data from
    // the upload heap to the default heap
    UpdateSubresources(pCmd, buffer->VidMemBuffer, buffer->UploadResource, 0, 0, 1, &resourceData);
    
    // transition the buffer data from copy destination state to buffer state
    D3D12_RESOURCE_BARRIER vbBarrier = {};
    vbBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    vbBarrier.Transition.pResource = buffer->VidMemBuffer;
    vbBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    vbBarrier.Transition.StateAfter = isTexture ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : resourceState; // todo change this
    vbBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pCmd->ResourceBarrier(1, &vbBarrier);
}

IBuffer* D3D12Context::CreateBuffer(BufferDesc& desc, ICommandList* commandList) 
{
    D3D12Buffer* buffer = new D3D12Buffer();
    ID3D12GraphicsCommandList6* pCmd = static_cast<D3D12CommandList*>(commandList)->m_CmdList;
    
    desc.Height = Math::Max(1, desc.Height); // minimum height for dx12 is 1

    // if EResourceUsage is constant buffer align buffer size
    if (desc.ResourceUsage == EResourceUsage::ConstantBuffer) desc.Width = (desc.Width + 255) & ~255;

    if (uint32(desc.flags & EBufferDescFlags::DirectUpload))
    {
        DirectUploadBuffer(desc, buffer, pCmd);
    }
    else
    {
        AllocateAndUploadBuffer(desc, buffer, pCmd);
    }
    
    constexpr EResourceUsage CBV_UAV = EResourceUsage::ConstantBuffer | EResourceUsage::UnorderedAccess;
    D3D12DescriptorHeap& descHeap = m_CBV_SRV_UAV_HEAPS[desc.FrameIndex]; //m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
    
    // is our buffer cbv or uav?
    if (uint32(desc.ResourceUsage & CBV_UAV)) // no need for vertex or index buffers
    {
        IncrementBufferPointer(buffer, descHeap);
    }

    switch (desc.ResourceUsage)
    {
    case EResourceUsage::VertexBuffer:
    {
        buffer->vertexBufferView.BufferLocation = buffer->VidMemBuffer->GetGPUVirtualAddress();  
        buffer->vertexBufferView.SizeInBytes = desc.Width;
        buffer->vertexBufferView.StrideInBytes = desc.ElementByteStride; 
        // wchar_t name[256];
        // wsprintfW(name, L"%s %s", L"D3D12Context Vertex Buffer", "other information");
        buffer->VidMemBuffer->SetName(L"D3D12Context Vertex Buffer");
    }
    break;
    case EResourceUsage::IndexBuffer:
    {    
        buffer->indexBufferView.BufferLocation = buffer->VidMemBuffer->GetGPUVirtualAddress();  
        buffer->indexBufferView.SizeInBytes = desc.Width;
        buffer->indexBufferView.Format = DXGI_FORMAT_R32_UINT; 
        buffer->VidMemBuffer->SetName(L"D3D12Context Index Buffer");
    }
    break;
    case EResourceUsage::ConstantBuffer:
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
        cbvDesc.BufferLocation = buffer->UploadResource->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = desc.Width;
        m_Device->CreateConstantBufferView(&cbvDesc, buffer->CPUDescHandle);
        buffer->UploadResource->Map(0, nullptr, &buffer->MapPtr);
        buffer->GPUDescHandle = {cbvDesc.BufferLocation};
        buffer->UploadResource->SetName(L"D3D12Context Constant Buffer");
    }
    break;
    case EResourceUsage::UnorderedAccess:
    {	
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = desc.Width / desc.ElementByteStride;
        uavDesc.Buffer.StructureByteStride = desc.ElementByteStride; 
        m_Device->CreateUnorderedAccessView(buffer->VidMemBuffer, nullptr, &uavDesc, buffer->CPUDescHandle);
        buffer->VidMemBuffer->SetName(L"D3D12Context UAV Buffer");
    }
    break;
    default: ax_assert(false); break;
    }

    return static_cast<IBuffer*>(buffer);
}

IImage* D3D12Context::CreateImage(BufferDesc& desc, ICommandList* commandList)
{
    D3D12Image* buffer = new D3D12Image();
    ID3D12GraphicsCommandList6* pCmd = static_cast<D3D12CommandList*>(commandList)->m_CmdList;
    
    IncrementBufferPointer(buffer, m_CBV_SRV_UAV_HEAPS[0]);
    AllocateAndUploadBuffer(desc, buffer, pCmd);

    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    viewDesc.Format = DX12::ToDX12Format(desc.Format);
    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // todo change
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    viewDesc.Texture2D.MipLevels = 1;
    m_Device->CreateShaderResourceView(buffer->VidMemBuffer, &viewDesc, buffer->CPUDescHandle);
    buffer->VidMemBuffer->SetName(L"D3D12Context Image");
    return buffer;
}

IPipeline* D3D12Context::CreateGraphicsPipeline(PipelineInfo& info)
{
	D3D12Pipeline* outPipeline = new D3D12Pipeline();
	const uint32 msaaQuality = 0; // D3D12Backend::GetMSAAQuality();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.NodeMask = 1;
	psoDesc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)info.primitiveType;
	psoDesc.pRootSignature = outPipeline->RootSignature;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = info.numRenderTargets;
	psoDesc.DSVFormat = DX12::ToDX12Format(info.DepthStencilFormat);
	psoDesc.SampleDesc.Quality = msaaQuality ? (msaaQuality - 1) : 0;
	psoDesc.SampleDesc.Count = msaaQuality ? msaaQuality : 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    psoDesc.pRootSignature = CreateRootSignature(info.descriptorSet);
	outPipeline->RootSignature = psoDesc.pRootSignature;

	D3D12_BLEND_DESC& blendDesc = psoDesc.BlendState;
	blendDesc.AlphaToCoverageEnable = info.AlphaToCoverageEnable;
	blendDesc.IndependentBlendEnable = info.IndependentBlendEnable;
	
	for (int i = 0; i < info.numRenderTargets; ++i)
	{
		psoDesc.RTVFormats[i] = DX12::ToDX12Format(info.RTVFormats[i]);
		BlendDesc& bDesc = info.RenderTargetBlendDescs[i];
		blendDesc.RenderTarget[i].BlendEnable = bDesc.BlendEnable;
		blendDesc.RenderTarget[i].SrcBlend = (D3D12_BLEND)bDesc.SrcBlend;
		blendDesc.RenderTarget[i].DestBlend = (D3D12_BLEND)bDesc.DestBlend;
		blendDesc.RenderTarget[i].SrcBlendAlpha = (D3D12_BLEND)bDesc.SrcBlendAlpha;
		blendDesc.RenderTarget[i].DestBlendAlpha = (D3D12_BLEND)bDesc.DestBlendAlpha;
		blendDesc.RenderTarget[i].BlendOp = (D3D12_BLEND_OP)bDesc.BlendOp;
		blendDesc.RenderTarget[i].BlendOpAlpha = (D3D12_BLEND_OP)bDesc.BlendOpAlpha;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	
	psoDesc.VS = { info.VertexShader->byteCode.blob, info.VertexShader->byteCode.blobSize }; // convert IShader::ByteCode to D3D12_SHADER_BYTECODE these structs are identical

	D3D12_INPUT_ELEMENT_DESC local_layout[16] = {};

	for (size_t i = 0ull, offset = 0ull; i < info.numInputLayout; ++i)
	{
		InputLayout& layout = info.inputLayouts[i];
		uint32 elemSize = VertexAttribSize(layout.Type);
		local_layout[i].SemanticName = layout.name.data();
		local_layout[i].SemanticIndex = 0;
		local_layout[i].Format = DX12::ToDX12Format(layout.Type);
		local_layout[i].InputSlot = 0;
		local_layout[i].AlignedByteOffset = offset;
		local_layout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		local_layout[i].InstanceDataStepRate = 0; // not used
		offset += elemSize;
	}

	psoDesc.InputLayout = { local_layout, info.numInputLayout };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	psoDesc.PS = { info.FragmentShader->byteCode.blob, info.FragmentShader->byteCode.blobSize};

	// Create the rasterizer state
	{
		D3D12_RASTERIZER_DESC& desc = psoDesc.RasterizerState;
        desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		desc.FillMode = (D3D12_FILL_MODE)info.fillMode;
		desc.CullMode = (D3D12_CULL_MODE)info.cullMode;
	}

	// Create depth-stencil State
    if (info.DepthStencilFormat != EGraphicsFormat::UNKNOWN)
	{
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); 
	}

	DXCall(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&outPipeline->PipelineState)));
	outPipeline->info = info;
    return static_cast<IPipeline*>(outPipeline);
}

ICommandAllocator* D3D12Context::CreateCommandAllocator(ECommandListType type)
{
	D3D12CommandAllocator* allocator = new D3D12CommandAllocator();
	DXCall(m_Device->CreateCommandAllocator((D3D12_COMMAND_LIST_TYPE)type, IID_PPV_ARGS(&allocator->allocator)));
	return static_cast<ICommandAllocator*>(allocator);
}

ICommandList* D3D12Context::CreateCommandList(ICommandAllocator* commandAllocator, ECommandListType type)
{
	D3D12CommandList* commandList = new D3D12CommandList();
	ID3D12CommandAllocator* dxCmdAlloc = static_cast<D3D12CommandAllocator*>(commandAllocator)->allocator;
	D3D12_COMMAND_LIST_TYPE d3dType = (D3D12_COMMAND_LIST_TYPE)type;
	DXCall(m_Device->CreateCommandList(0, d3dType, dxCmdAlloc, nullptr, IID_PPV_ARGS(&commandList->m_CmdList)));
	commandList->Initialize(this);
	return static_cast<ICommandList*>(commandList);
}

ICommandQueue* D3D12Context::CreateCommandQueue(ECommandListType type, ECommandQueuePriority priority) 
{
	const int priorities[3] = { 0, 10, 10000 };
	D3D12_COMMAND_QUEUE_DESC cmdQueuedesc{};
	cmdQueuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueuedesc.NodeMask = 0;
	cmdQueuedesc.Priority = priorities[(int)priority];
	cmdQueuedesc.Type = (D3D12_COMMAND_LIST_TYPE)((uint32)type); // our enum is directly convertible to dx12 command list type enum
	D3D12CommandQueue* cmdQueue = new D3D12CommandQueue();

	DXCall(m_Device->CreateCommandQueue(&cmdQueuedesc, IID_PPV_ARGS(&cmdQueue->queue)));
	return static_cast<ICommandQueue*>(cmdQueue);
}
	
ISwapChain* D3D12Context::CreateSwapChain(ICommandQueue* commandQueue, EGraphicsFormat format)
{
	ID3D12CommandQueue* dxCommandQueue = static_cast<D3D12CommandQueue*>(commandQueue)->queue;
	DX12SwapChainDesc swapchainDesc =
	{
		.Width = windowSize.x,
		.Height = windowSize.y,
		.device = m_Device,
		.factory = DXFactory,
		.commandQueue = dxCommandQueue ,
		.format = DX12::ToDX12Format(format),
		.hwnd = m_Window->GetHWND() // last time we was here todo TODO
	};
	return new D3D12SwapChain(swapchainDesc);
}

IFence* D3D12Context::CreateFence()
{
	D3D12Fence* dxFence = new D3D12Fence();
	DXCall(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dxFence->fence)));
	return static_cast<IFence*>(dxFence);
}

// move this three function below txo command list
void D3D12Context::MapBuffer(IBuffer* buffer, void const* data, uint64 size)
{
    D3D12Buffer* dxBuffer = static_cast<D3D12Buffer*>(buffer);
    memcpy(dxBuffer->MapPtr, data, size);
}

// Create the root signature 
ID3D12RootSignature* D3D12Context::CreateRootSignature(DescriptorSetDesc& desc)
{
	ID3D12RootSignature* result = nullptr;
    D3D12_ROOT_PARAMETER bindings[8]{};

    // this is a range of descriptors inside a descriptor heap
    D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1]; // only one range right now
    descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
    descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
    descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
    descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
    descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

    // create a descriptor table
    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
    descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
    descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array

    for (uint32 bindingIndex = 0u; bindingIndex < desc.BindingCount; bindingIndex++)
    {
		DescriptorBindingDesc& element = desc.Bindings[bindingIndex];
        D3D12_ROOT_PARAMETER& binding = bindings[bindingIndex];
        binding.ParameterType = DX12::ToDX12DescriptorType(element.Type);
        binding.ShaderVisibility = DX12::ToDX12ShaderVisibility(element.ShaderVisibility);
        binding.Descriptor.RegisterSpace = 0;
        binding.Descriptor.ShaderRegister = bindingIndex;

        if (binding.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            binding.DescriptorTable = descriptorTable;
    }

    D3D12_STATIC_SAMPLER_DESC samplers[4]{};
    
    for (uint32 i = 0; i < desc.NumSamplers; ++i)
    {
		D3D12_STATIC_SAMPLER_DESC& sampler = samplers[i];
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

        uint32 minFilterVal = (uint32)desc.Samplers[i].MinFilter;
        uint32 magFilterVal = (uint32)desc.Samplers[i].MagFilter;
        uint32 mipFilterVal = (uint32)desc.Samplers[i].MipFilter;
		bool UseAnisotropy = false; // for now

        sampler.Filter = // compose min, mag, and mip filters and generate d3d12filter
            (D3D12_FILTER)(mipFilterVal | (magFilterVal << 2) | (minFilterVal << 4) | (UseAnisotropy << 6));

        sampler.AddressU = (D3D12_TEXTURE_ADDRESS_MODE)desc.Samplers[i].AdressU;
        sampler.AddressV = (D3D12_TEXTURE_ADDRESS_MODE)desc.Samplers[i].AdressV;
        sampler.AddressW = (D3D12_TEXTURE_ADDRESS_MODE)desc.Samplers[i].AdressW;
        sampler.MipLODBias = sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = (D3D12_STATIC_BORDER_COLOR)desc.Samplers[i].BorderColor;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = DX12::ToDX12ShaderVisibility(desc.Samplers[i].ShaderVisibility);
    }
	// for now
    const D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
                                             | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS 
                                             | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS 
                                             | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
    rootSignatureDesc.Desc_1_0.NumParameters = desc.BindingCount;
    rootSignatureDesc.Desc_1_0.pParameters = bindings;
	rootSignatureDesc.Desc_1_0.NumStaticSamplers = desc.NumSamplers;
	rootSignatureDesc.Desc_1_0.pStaticSamplers = samplers;
	rootSignatureDesc.Desc_1_0.Flags = flags;

    ID3DBlob* rootSigBlob = nullptr, *errorBlob = nullptr;
    D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &rootSigBlob, &errorBlob);
    
    if (errorBlob != nullptr)
    {
        AX_ERROR("shader compilation failed with error: \n%s", (const char*)errorBlob->GetBufferPointer());
        ax_assert(false);
    }
    m_Device->CreateRootSignature(
        0u,
        rootSigBlob->GetBufferPointer(), 
        rootSigBlob->GetBufferSize(), 
        IID_PPV_ARGS(&result)
    );
    return result;
}

void D3D12Context::WaitFence(IFence* fence, uint32 fenceValue)
{
	D3D12Fence* dxFence = static_cast<D3D12Fence*>(fence);

	if (dxFence->fence->GetCompletedValue() < fenceValue)
	{
		DXCall(dxFence->fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
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
	ax_assert((shaderType == EShaderType::Vertex || shaderType == EShaderType::Fragment) && "Shader type is not supported!");
	
	D3D12Shader* shader = new D3D12Shader();
	ID3DBlob* errorBlob, *blob;
#ifdef _DEBUG
	D3D_SHADER_MACRO shaderMacros[] = { {"DEBUG", "1"}, {nullptr, nullptr} };
#else 
	D3D_SHADER_MACRO shaderMacros[] = { {"RELEASE", "1"}, {nullptr, nullptr} };
#endif //  DEBUG
	
	const char* shaderModel = shaderType == EShaderType::Vertex ? "vs_5_1" : "ps_5_1";

	if (FAILED(D3DCompile(sourceCode, strlen(sourceCode), nullptr,
		shaderMacros, nullptr, functionName, shaderModel, 0, 0, &blob, &errorBlob)))
	{
		AX_ERROR("Shader Compiling Error:\n %s", (char*)errorBlob->GetBufferPointer());
		throw std::exception("shader compiling failed!");
	}
	shader->byteCode.blobSize = blob->GetBufferSize(); 
	shader->byteCode.blob = blob->GetBufferPointer();
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
    m_Allocator->Release();
#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
		DXCall(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue)));

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
	};

	Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debugDevice;
	DXCall(m_Device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
	ReleaseResource(m_Device);
	DXCall(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
#else
	ReleaseResource(m_Device);
#endif
}
