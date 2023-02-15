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
	
	void Release() override
	{ ReleaseResource(Resource); }
};

struct D3D12Shader : IShader
{

};

struct D3D12Pipeline : IPipeline
{
	ID3D12PipelineState* PipelineState;
	ID3D12RootSignature* RootSignature;
};