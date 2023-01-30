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
		case VertexAttribType::Float:   return DXGI_FORMAT_R32_FLOAT; break;
		case VertexAttribType::Float2:  return DXGI_FORMAT_R32G32_FLOAT;    break;
		case VertexAttribType::Float3:  return DXGI_FORMAT_R32G32B32_FLOAT;    break;
		case VertexAttribType::Float4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;    break;
		case VertexAttribType::Half:    return DXGI_FORMAT_R16_FLOAT;    break;
		case VertexAttribType::Half2:   return DXGI_FORMAT_R16G16_FLOAT;    break;
		case VertexAttribType::Half3:   assert(false && "Not Supported!");  return DXGI_FORMAT_UNKNOWN;    break;
		case VertexAttribType::Half4:   return DXGI_FORMAT_R16G16B16A16_FLOAT;  break;
		case VertexAttribType::UInt:    return DXGI_FORMAT_R32_UINT;    break;
		default: assert(false && "Unknown Format!"); return DXGI_FORMAT_UNKNOWN; break;
		}
	}

	DXGI_FORMAT ToDX12Format(EImageFormat type)
	{
		switch (type)
		{
		case EImageFormat::RGBA8F:      break;
		case EImageFormat::RGBA8_SRGBF: break;
		case EImageFormat::BGRA8F:      break;
		case EImageFormat::RGBA16F:     break;
		case EImageFormat::RGBA32F:     break;
		case EImageFormat::R32U:        break;
		case EImageFormat::R32F:        break;
		case EImageFormat::D32F:        break;
		case EImageFormat::D32FS8U:     break;
		case EImageFormat::RG32F:       break;
		case EImageFormat::RG32I:       break;
		case EImageFormat::RG16F:       break;
		default: assert(false && "Unknown Format!");  return DXGI_FORMAT_UNKNOWN; break;
		}
	}
	
	D3D12_RESOURCE_STATES ToDX12ResourceUsage(EResourceUsage usage)
	{
		switch (usage)
		{
		case EResourceUsage::VertexBuffer:    return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER; break;
		case EResourceUsage::IndexBuffer:     return D3D12_RESOURCE_STATE_INDEX_BUFFER; break;
		case EResourceUsage::ConstantBuffer:  return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER; break;
		case EResourceUsage::TransferSrc:     return D3D12_RESOURCE_STATE_COPY_SOURCE; break;
		case EResourceUsage::TransferDst:     return D3D12_RESOURCE_STATE_COPY_DEST; break;
		case EResourceUsage::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS; break;
		case EResourceUsage::Present:
		case EResourceUsage::Unknown: return D3D12_RESOURCE_STATE_COMMON; break;
		default: assert(false && "Unknown Format"); return D3D12_RESOURCE_STATE_COMMON; break;
		}
	}
}

#endif