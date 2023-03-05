#pragma once

#include "Axiom/Core/Common.hpp"
#include "Axiom/Core/Enum.hpp"

enum class EPrimitiveType : uint32
{
    Unknown,
    Point,
    Line,
    Triangle,
    Patch,
    TriangleStrip
};
ENUM_FLAGS(EPrimitiveType, uint32)

enum class ECompareOp : uint32
{
    Never = 0,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};
ENUM_FLAGS(ECompareOp, uint32)

enum class EStencilOp : uint32
{
    Keep = 0,
    Zero,
    Replace,
    IncrAndClamp,
    DecrAndClamp,
    Invert,
    IncrAndWrap,
    DecrAndWrap
};
ENUM_FLAGS(EStencilOp, uint32)

enum class ECullMode : uint32
{
    Unknown,
    None,
    Front,
    Back
};
ENUM_FLAGS(ECullMode, uint32)

enum class EFillMode : uint32
{
    Wireframe = 2,
    Fill = 3, // dx12 Solid
};
ENUM_FLAGS(EFillMode, uint32)

enum class EBlendFactor : uint32
{
    Zero = 0,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DstAlpha,
    InvDstAlpha,
    DstColor,
    InvDstColor,
    SrcAlphaSat,
    ConstantFactor,
    InvConstantFactor,
    Src1Color,
    InvSrc1Color,
    Src1Alpha,
    InvSrc1Alpha
};
ENUM_FLAGS(EBlendFactor, uint32)

enum class EBlendOp : uint32
{
    None,
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};
ENUM_FLAGS(EBlendOp, uint32)

enum class EFiltering : uint32
{
    Nearest = 0,
    Linear
};
ENUM_FLAGS(EFiltering, uint32)

enum class ETextureAddressMode : uint32
{
    Wrap = 0,
    Mirror,
    ClampToEdge,
    ClampToBorder
};
ENUM_FLAGS(ETextureAddressMode, uint32)

enum class EResourceUsage : uint32
{
    Unknown = 1 << 0,
    VertexBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    ConstantBuffer = 1 << 3,
    ShaderResource = 1 << 4,
    RenderTarget = 1 << 5,
    DepthStencil = 1 << 6,
    TransferSrc = 1 << 7,
    TransferDst = 1 << 8,
    UnorderedAccess = 1 << 9,
    HostVisible = 1 << 10,

    Present = ShaderResource | RenderTarget
};
ENUM_FLAGS(EResourceUsage, uint32)

enum class ECommandListType : uint32
{
    Direct,
    Bundle,
    Compute,
    Copy,
    Count
};

ENUM_FLAGS(ECommandListType, uint32)

enum class ECommandQueuePriority : uint32
{
    Normal  ,
    High    ,
    Realtime
};
ENUM_FLAGS(ECommandQueuePriority, uint32)

enum class VertexAttribType : uint32
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Half,
    Half2,
    Half3,
    Half4,
    UInt
};

static constexpr uint32 VertexAttribSize(VertexAttribType type)
{
    switch (type)
    {
    case VertexAttribType::None:   return 0u;  break;
    case VertexAttribType::Float:  return sizeof(float) * 1u; break;
    case VertexAttribType::Float2: return sizeof(float) * 2u; break;
    case VertexAttribType::Float3: return sizeof(float) * 3u; break;
    case VertexAttribType::Float4: return sizeof(float) * 4u; break;
    case VertexAttribType::Half:   return sizeof(short) * 1u; break;
    case VertexAttribType::Half2:  return sizeof(short) * 2u; break;
    case VertexAttribType::Half3:  return sizeof(short) * 3u; break;
    case VertexAttribType::Half4:  return sizeof(short) * 4u; break;
    case VertexAttribType::UInt:   return sizeof(unsigned) * 1u; break;
    default: return 0u; break;
    }
}

enum class EImageFormat
{
    UNKNOWN,
    RGBA8,
    RGBA8_SRGBF,
    BGRA8F,
    RGBA16F,
    RGBA32F,
    R32U,
    R32F,
    D32F,
    D32FS8U,
    RG32F,
    RG32I,
    RG16F
};
ENUM_FLAGS(EImageFormat, uint32)

constexpr uint32 kImageFormatSizeLUT[] = {
        0,                   //  UNKNOWN,
        sizeof(uint8) * 4,   //  RGBA8F,
        sizeof(uint8) * 4,   //  RGBA8_SRGBF,
        sizeof(uint8) * 4,   //  BGRA8F,
        sizeof(uint16) * 4,  //  RGBA16F,
        sizeof(uint32) * 4,  //  RGBA32F,
        sizeof(uint32),      //  R32U,
        sizeof(uint32),      //  R32F,
        sizeof(uint32),      //  D32F,
        sizeof(uint32),      //  D32FS8U,
        sizeof(uint32) * 2,  //  RG32F,
        sizeof(uint32) * 2,  //  RG32I,
        sizeof(uint16) * 2   //  RG16F,
};

constexpr uint32 GetImageFormatSize(EImageFormat format)
{
    return kImageFormatSizeLUT[(uint32)format];
}

enum class ECPUAccessFlag
{
    None,
    Read,
    Write
};
ENUM_FLAGS(ECPUAccessFlag, uint32)

struct IGraphicsResource
{
	virtual void Release() = 0;
};

struct IImage : IGraphicsResource
{
		
};

enum class EShaderType : uint32
{
    None     = 0,
    Vertex   = 1 << 0,
    Fragment = 1 << 1,
    Compute  = 1 << 2,
    Hull     = 1 << 3,
    Domain   = 1 << 4
};
ENUM_FLAGS(EShaderType, uint32);

struct IShader : IGraphicsResource
{
    const char* sourceCode = nullptr;
    struct ByteCode
    {
        void* blob = nullptr;
        size_t blobSize = 0;
    } byteCode{};
};

struct BufferDesc
{
	EResourceUsage ResourceUsage = EResourceUsage::Unknown;
	uint64 Size = 0;
	uint64 ElementByteStride = 0;
	void* Data;
};

struct IBuffer : IGraphicsResource
{
	EResourceUsage ResourceUsage;
	uint64 SizeInBytes = 0;
	uint64 ElementByteStride = 0;
	uint64 DataLen = 0;
	uint64 DataOffset = 0;

	void* BufferData;
};

struct ISwapChain : IGraphicsResource
{
    virtual uint32 GetCurrentBackBufferIndex() = 0;
	virtual void Present(bool gsync, uint32 flags) = 0;
	virtual IImage* GetBackBuffer(int index) = 0;
    virtual IImage* GetDepthStencilBuffer() = 0;
};

struct ViewportDesc
{
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;

    ViewportDesc(
        float topLeftX,
        float topLeftY,
        float width,
        float height,
        float minDepth = 0.0f,
        float maxDepth = 1.0f
    ) : TopLeftX(topLeftX),
        TopLeftY(topLeftY),
        Width(width),
        Height(height),
        MinDepth(minDepth),
        MaxDepth(maxDepth)
    {}
};

struct GraphicsRect
{
    uint32 Left, Top, Front, Right, Bottom, Back;
    GraphicsRect(
        uint32 left,
        uint32 top,
        uint32 front,
        uint32 right,
        uint32 bottom,
        uint32 back
    ) : Left(left),
        Top(top),
        Front(front),
        Right(right),
        Bottom(bottom),
        Back(back)
    {}
};