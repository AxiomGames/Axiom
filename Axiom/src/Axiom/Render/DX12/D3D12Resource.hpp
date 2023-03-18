#pragma once

#include "D3D12CommonHeaders.hpp"
#include "../PipelineState.hpp"

namespace D3D12MA
{
    class Allocation;
}

struct D3D12Buffer : public IBuffer
{
    ID3D12Resource* VidMemBuffer{};
    ID3D12Resource* UploadResource{};
    D3D12MA::Allocation* VidAlloc{};
    void* MapPtr = nullptr;

    union
    {
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        struct
        {
            // for srv_cbv_uav
            D3D12_CPU_DESCRIPTOR_HANDLE CPUDescHandle;
            D3D12_GPU_DESCRIPTOR_HANDLE GPUDescHandle;
        };
    };
    
    void Release() override
    {
        // delete Allocation;
        ReleaseResource(UploadResource);
        ReleaseResource(VidMemBuffer);
    }
};

struct D3D12Image : IImage, D3D12Buffer
{
    void Release() override
    {
        D3D12Buffer::Release();
    }
};

struct D3D12Pipeline : public IPipeline
{
	ID3D12PipelineState* PipelineState;
	ID3D12RootSignature* RootSignature;
	void Release() override
	{
		ReleaseResource(PipelineState);
		ReleaseResource(RootSignature);
	}
};

struct D3D12Fence : public IFence
{
	void Release() override
	{ fence->Release(); }
	ID3D12Fence1* fence = nullptr;
};

struct D3D12CommandAllocator : public ICommandAllocator
{
	ID3D12CommandAllocator* allocator;
	void Reset() override
	{ 
		DXCall(allocator->Reset());
	}

	void Release() override
	{
		allocator->Release();
	}
};
