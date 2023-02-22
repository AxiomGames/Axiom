#pragma once

#include "D3D12CommonHeaders.hpp"
#include "../PipelineState.hpp"

struct D3D12Buffer : IBuffer
{
	ID3D12Resource* mpVidMemBuffer;
	ID3D12Resource* mpSysMemBuffer;
	
	union
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};

	void Release() override
	{
		ReleaseResource(mpVidMemBuffer);
		ReleaseResource(mpSysMemBuffer);
	}
};

struct D3D12Image : IImage
{
	ID3D12Resource* Resource;
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle;

	void Release() override
	{ ReleaseResource(Resource); }
};

struct D3D12Shader : public IShader
{
	void Release() override
	{ } // todo delete shader code
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
public:
	ID3D12Fence1* fence;
	uint64 value;
};

struct D3D12CommandAllocator : ICommandAllocator
{
	ID3D12CommandAllocator* allocator;
	void Reset() override
	{ allocator->Reset(); }
	void Release() override
	{ allocator->Release(); }
};