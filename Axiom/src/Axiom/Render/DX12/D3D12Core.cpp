#include "D3D12Core.hpp"
#include "D3D12CommonHeaders.hpp"
#include "D3D12Context.hpp"
#include "D3D12SwapChain.hpp"
#include <cassert>

#ifdef AX_WIN32

namespace DX12
{
	bool InitializeDXFactory(IDXGIFactory7** factory)
	{
		uint32 dx_factory_flags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug3> debugInterface;
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
	
	constexpr D3D_FEATURE_LEVEL minimum_feature_evel = D3D_FEATURE_LEVEL_11_0;

	IDXGIAdapter4* DetermineMainAdapter(IDXGIFactory7* factory)
	{
		IDXGIAdapter4* adapter;

		for (uint32 i = 0; factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; i++)
		{
			if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_evel, __uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}

			ReleaseResource(adapter);
		}

		return nullptr;
	}

	D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter)
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

		ComPtr<ID3D12Device> device;
		DXCall(D3D12CreateDevice(adapter, minimum_feature_evel, IID_PPV_ARGS(&device)));
		DXCall(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfo, sizeof(featureLevelInfo)));
		return featureLevelInfo.MaxSupportedFeatureLevel;
	}


	DXGI_FORMAT ToDX12Format(VertexAttribType type)
	{
		switch (type)
		{
		case VertexAttribType::Float:   return DXGI_FORMAT_R32_FLOAT; 
		case VertexAttribType::Float2:  return DXGI_FORMAT_R32G32_FLOAT;   
		case VertexAttribType::Float3:  return DXGI_FORMAT_R32G32B32_FLOAT;    
		case VertexAttribType::Float4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;    
		case VertexAttribType::Half:    return DXGI_FORMAT_R16_FLOAT;    
		case VertexAttribType::Half2:   return DXGI_FORMAT_R16G16_FLOAT;    
		case VertexAttribType::Half3:   assert(false && "Not Supported!");  return DXGI_FORMAT_UNKNOWN;  
		case VertexAttribType::Half4:   return DXGI_FORMAT_R16G16B16A16_FLOAT;  
		case VertexAttribType::UInt:    return DXGI_FORMAT_R32_UINT;    
		default: assert(false && "Unknown Format!"); return DXGI_FORMAT_UNKNOWN; 
		}
	}

	// todo use compile time lookup table like in ToDX12PipelineStage
	DXGI_FORMAT ToDX12Format(EImageFormat type)
	{
		switch (type)
		{
		case EImageFormat::UNKNOWN:     return DXGI_FORMAT_UNKNOWN;
		case EImageFormat::RGBA8:       return DXGI_FORMAT_R8G8B8A8_UNORM; 
		case EImageFormat::RGBA8_SRGBF: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; 
		case EImageFormat::RGBA16F:     return DXGI_FORMAT_R16G16B16A16_FLOAT; 
		case EImageFormat::RGBA32F:     return DXGI_FORMAT_R32G32B32A32_FLOAT; 
		case EImageFormat::R32U:        return DXGI_FORMAT_R32_UINT; 
		case EImageFormat::R32F:        return DXGI_FORMAT_R32_FLOAT; 
		case EImageFormat::D32F:        return DXGI_FORMAT_D32_FLOAT; 
		case EImageFormat::D32FS8U:     return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case EImageFormat::RG32F:       return DXGI_FORMAT_R32G32_FLOAT; 
		case EImageFormat::RG32I:       return DXGI_FORMAT_R32G32_UINT; 
		case EImageFormat::RG16F:       return DXGI_FORMAT_R16G16_FLOAT; 
		   default: assert(false && "Unknown Format!");  return DXGI_FORMAT_UNKNOWN;
		}
	}
	
	D3D12_RESOURCE_STATES ToDX12ResourceUsage(EResourceUsage usage)
	{
		switch (usage)
		{
		case EResourceUsage::VertexBuffer:    return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case EResourceUsage::IndexBuffer:     return D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case EResourceUsage::ConstantBuffer:  return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case EResourceUsage::TransferSrc:     return D3D12_RESOURCE_STATE_COPY_SOURCE; 
		case EResourceUsage::TransferDst:     return D3D12_RESOURCE_STATE_COPY_DEST; 
		case EResourceUsage::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case EResourceUsage::Present:         return D3D12_RESOURCE_STATE_PRESENT; 
		case EResourceUsage::Unknown:         return D3D12_RESOURCE_STATE_COMMON;
		case EResourceUsage::RenderTarget:    return D3D12_RESOURCE_STATE_RENDER_TARGET;
		default: assert(false && "Unknown Format"); return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	D3D12_RESOURCE_STATES ToDX12PipelineStage(EPipelineStage stage)
	{
		constexpr D3D12_RESOURCE_STATES stages[22] =
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
}

#endif