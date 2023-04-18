#include "D3D12Core.hpp"

#ifdef AX_WIN32

#include "D3D12CommonHeaders.hpp"
#include "D3D12Context.hpp"
#include "D3D12SwapChain.hpp"
#include <cassert>

bool DX12::InitializeDXFactory(IDXGIFactory7** factory)
{
	uint32 dx_factory_flags = 0;
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug3> debugInterface;
	DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
	debugInterface->SetEnableGPUBasedValidation(true);

	dx_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT hr = CreateDXGIFactory2(dx_factory_flags, IID_PPV_ARGS(factory));
       
	if (FAILED(hr))
	{
		std::cerr << "Failed to init DX factory" << std::endl;
		return false;
	}
	return true;
}
	
IDXGIAdapter4* DX12::DetermineMainAdapter(IDXGIFactory7* factory)
{
	IDXGIAdapter4* adapter;

	for (uint32 i = 0; 
        factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))
            != DXGI_ERROR_NOT_FOUND; i++)
	{
        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
		{
			return adapter;
		}

		ReleaseResource(adapter);
	}

	return nullptr;
}

D3D_FEATURE_LEVEL DX12::GetMaxFeatureLevel(IDXGIAdapter4* adapter)
{
	constexpr D3D_FEATURE_LEVEL featureLevels[4] {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelInfo;
	featureLevelInfo.NumFeatureLevels = _countof(featureLevels);
	featureLevelInfo.pFeatureLevelsRequested = featureLevels;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
    DXCall(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
	DXCall(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfo, sizeof(featureLevelInfo)));
	return featureLevelInfo.MaxSupportedFeatureLevel;
}

DXGI_FORMAT DX12::ToDX12Format(VertexAttribType type)
{
    const DXGI_FORMAT formatLUT[] = {
        DXGI_FORMAT_UNKNOWN,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,   
        DXGI_FORMAT_R32G32B32_FLOAT,    
        DXGI_FORMAT_R32G32B32A32_FLOAT,    
        DXGI_FORMAT_R16_FLOAT,
        DXGI_FORMAT_R16G16_FLOAT,    
        DXGI_FORMAT_R16G16B16A16_FLOAT,  
        DXGI_FORMAT_R32_UINT
    };
    return formatLUT[(uint32)type];
}

D3D12_ROOT_PARAMETER_TYPE DX12::ToDX12DescriptorType(EDescriptorType descType)
{
    const D3D12_ROOT_PARAMETER_TYPE descriptorTypeLUT[] = {
        D3D12_ROOT_PARAMETER_TYPE_SRV,              
        D3D12_ROOT_PARAMETER_TYPE_SRV,              
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,              
        D3D12_ROOT_PARAMETER_TYPE_CBV,              
        D3D12_ROOT_PARAMETER_TYPE_UAV,              
        D3D12_ROOT_PARAMETER_TYPE_UAV,              
        D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,  
    };
    return descriptorTypeLUT[(uint32)descType];
}

D3D12_SHADER_VISIBILITY DX12::ToDX12ShaderVisibility(EShaderType shaderType)
{
    const D3D12_SHADER_VISIBILITY visibilityLUT[] = {
        D3D12_SHADER_VISIBILITY_ALL,
        D3D12_SHADER_VISIBILITY_VERTEX,
        D3D12_SHADER_VISIBILITY_PIXEL,
        D3D12_SHADER_VISIBILITY_ALL,
        D3D12_SHADER_VISIBILITY_HULL,
        D3D12_SHADER_VISIBILITY_DOMAIN,
        D3D12_SHADER_VISIBILITY_ALL
    };
    return visibilityLUT[(uint32)shaderType];
}

DXGI_FORMAT DX12::ToDX12Format(EGraphicsFormat type)
{
    const DXGI_FORMAT formatLUT[] = 
    {
        DXGI_FORMAT_UNKNOWN            ,
        DXGI_FORMAT_R8G8B8A8_UNORM     , 
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 
        DXGI_FORMAT_R16G16B16A16_FLOAT , 
        DXGI_FORMAT_R32G32B32A32_FLOAT , 
        DXGI_FORMAT_R32_UINT           , 
        DXGI_FORMAT_R32_FLOAT          , 
        DXGI_FORMAT_D32_FLOAT          , 
        DXGI_FORMAT_D24_UNORM_S8_UINT  ,
        DXGI_FORMAT_R32G32_FLOAT       ,  
        DXGI_FORMAT_R32G32_UINT        , 
        DXGI_FORMAT_R16G16_FLOAT        
    };
    return formatLUT[(uint32)type];
}

D3D12_RESOURCE_STATES DX12::ToDX12ResourceUsage(EResourceUsage usage)
{
    const D3D12_RESOURCE_STATES resourceStateLUT[] = {
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        D3D12_RESOURCE_STATE_INDEX_BUFFER,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_COPY_SOURCE, 
        D3D12_RESOURCE_STATE_COPY_DEST, 
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PRESENT, 
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COMMON
    };
    return resourceStateLUT[TrailingZeroCount((uint32)usage)];
}

D3D12_RESOURCE_STATES DX12::ToDX12PipelineStage(EPipelineStage stage)
{
	const D3D12_RESOURCE_STATES stages[22] =
	{
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_DEPTH_READ,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_STREAM_OUT,
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_RESOLVE_DEST,
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE
	};
	return stages[TrailingZeroCount((uint32)stage)];
}

#endif