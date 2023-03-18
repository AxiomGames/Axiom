#pragma once

#include "Axiom/CoreMinimal.hpp"
#include "../Types.hpp"
#include "../PipelineState.hpp"

#ifdef AX_WIN32

// forward declare
struct IDXGIFactory7;
struct IDXGIAdapter4;
struct D3D12_MEMCPY_DEST;
struct D3D12_SUBRESOURCE_DATA;
struct D3D12_PLACED_SUBRESOURCE_FOOTPRINT;

class ID3D12GraphicsCommandList;
class ID3D12Resource;

enum D3D_FEATURE_LEVEL;
enum DXGI_FORMAT;
enum D3D12_RESOURCE_STATES;
enum D3D12_ROOT_PARAMETER_TYPE;
enum D3D12_SHADER_VISIBILITY;

typedef size_t SIZE_T;
typedef uint32 UINT;
typedef uint64 UINT64;

namespace DX12
{
	bool InitializeDXFactory(IDXGIFactory7** factory);

	IDXGIAdapter4* DetermineMainAdapter(IDXGIFactory7* factory);

	D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter);

	DXGI_FORMAT ToDX12Format(VertexAttribType type);
    DXGI_FORMAT ToDX12Format(EGraphicsFormat type);
	D3D12_RESOURCE_STATES ToDX12ResourceUsage(EResourceUsage usage);
	D3D12_RESOURCE_STATES ToDX12PipelineStage(EPipelineStage stage);
    D3D12_ROOT_PARAMETER_TYPE ToDX12DescriptorType(EDescriptorType descType);
    D3D12_SHADER_VISIBILITY ToDX12ShaderVisibility(EShaderType shaderType);
}
#endif