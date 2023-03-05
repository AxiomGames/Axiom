#pragma once

#include "Types.hpp"

struct SampleDesc
{
	uint32 count;
	uint32 quality;
};

enum class ColorWriteEnable : uint32
{
	Red   = 1u,
	Green = 2u,
	Blue  = 4u,
	Alpha = 8u,
	All = 1u | 2u | 4u | 8u,
};

// Incomplete pipeline stage enums.
enum class EPipelineStage : uint32
{
	Unknown          = 0u,
	VertexBuffer     = 1u << 1u,
	ConstantBuffer   = 1u << 2u,
	IndexBuffer      = 1u << 3u,
	RenderTarget     = 1u << 4u,
	UnorderedAccess  = 1u << 5u,
	DepthWrite       = 1u << 6u,
	DepthRead        = 1u << 7u,
	ShaderResource   = 1u << 8u,
	StreamOut        = 1u << 9u,
	IndirectArgument = 1u << 10u,
	CopyDest         = 1u << 11u,
	CopySource       = 1u << 12u,
	ResolveDest      = 1u << 13u,
	ResolveSource    = 1u << 14u,
	InputAttachment  = 1u << 15u,
	Present          = 1u << 16u,
	BuildAsRead      = 1u << 17u,
	BuildAsWrite     = 1u << 18u,
	RayTracing       = 1u << 19u,
	Common           = 1u << 20u,
	ShadingRate      = 1u << 21u,
	MaxBit = ShadingRate,
	GenericRead = VertexBuffer |
	ConstantBuffer |
	IndexBuffer |
	ShaderResource |
	IndirectArgument |
	CopySource
};
ENUM_FLAGS(EPipelineStage, uint32);

enum class EPipelineAccess : uint32
{
	None = 0u,
	VertexRead        = 1u << 0u,
	IndexRead         = 1u << 1u,
	ShaderRead        = 1u << 2u,
	ShaderWrite       = 1u << 3u,
	RenderTargetRead  = 1u << 4u,
	RenderTargetWrite = 1u << 5u,
	DepthStencilRead  = 1u << 6u,
	DepthStencilWrite = 1u << 7u,
	TransferRead      = 1u << 8u,
	TransferWrite     = 1u << 9u,
	MemoryRead        = 1u << 10u,
	MemoryWrite       = 1u << 11u,
	// API exclusive pipeline enums
	HostRead  = 1u << 20u,
	HostWrite = 1u << 21u
};
ENUM_FLAGS(EPipelineAccess, uint32);

enum class EDescriptorType : uint32
{
	Unknown,
	Sampler,
	ShaderResource,
	ConstantBuffer,
	UnorderedAccessImage,
	UnorderedAccessBuffer,
	PushConstant,
	Count
};

struct Sampler
{ };

struct DescriptorBindingDesc
{
	uint32 BindingID = ~0u;
	EDescriptorType Type = EDescriptorType::Unknown;
    EShaderType ShaderVisibility = EShaderType::None;
	uint32 ArraySize = 1;

	union
	{
		Sampler* sampler = nullptr;
		IBuffer* buffer;
		IImage* image;
	};
};

struct DescriptorSetDesc
{
	uint32 BindingCount = 0u;
    DescriptorBindingDesc Bindings[8]{};
};


struct DescriptorSet : IGraphicsResource
{
    uint32 BindingCount = 0;
    EDescriptorType BindingTypes[8]{};
};

struct PipelineBarrier
{
	EResourceUsage CurrentUsage = EResourceUsage::Unknown;
	EPipelineStage CurrentStage = EPipelineStage::Unknown;
	EPipelineAccess CurrentAccess = EPipelineAccess::None;
	EResourceUsage NextUsage = EResourceUsage::Unknown;
	EPipelineStage NextStage = EPipelineStage::Unknown;
	EPipelineAccess NextAccess = EPipelineAccess::None;
};

struct BlendDesc
{
	bool BlendEnable;

	EBlendFactor SrcBlend = EBlendFactor::One;
	EBlendFactor DestBlend = EBlendFactor::Zero;
	EBlendOp BlendOp = EBlendOp::Add;
	EBlendFactor SrcBlendAlpha = EBlendFactor::One;
	EBlendFactor DestBlendAlpha = EBlendFactor::Zero;
	EBlendOp BlendOpAlpha = EBlendOp::Add;
	ColorWriteEnable RenderTargetWriteMask = ColorWriteEnable::All;
};

#include <string_view>

typedef std::string_view StringView;

struct InputLayout
{
	StringView name{};// string view
	VertexAttribType Type = VertexAttribType::None;
};

struct PipelineInfo
{
	static constexpr int MaxRenderTargets = 16;

	IShader* VertexShader      = nullptr;
	IShader* FragmentShader    = nullptr;
	IShader* HullShader        = nullptr;
	IShader* DomainShader      = nullptr;
	IShader* GeometryShader    = nullptr;
	IShader* TesellationShader = nullptr;

	EPrimitiveType primitiveType = EPrimitiveType::Triangle;
	ECullMode cullMode = ECullMode::None;
	EFillMode fillMode = EFillMode::Fill;
	bool counterClockWise = false;

	int32 numRenderTargets = 1;
	EImageFormat DepthStencilFormat = EImageFormat::UNKNOWN;
	EImageFormat RTVFormats[MaxRenderTargets] = {};

	bool AlphaToCoverageEnable = false;
	bool IndependentBlendEnable = false;

	BlendDesc RenderTargetBlendDescs[MaxRenderTargets] = {};
	
	int32 descriptorSetCount = 0;
	DescriptorSetDesc descriptorSet = {};
	
	uint32 numInputLayout = 0u;
	InputLayout inputLayouts[8] = {};

	bool enableMultiSampling = false;
	SampleDesc sampleDesc = {}; // multisampling parameters
};

struct IPipeline : IGraphicsResource
{
	PipelineInfo info;
};

struct IFence : IGraphicsResource
{
};