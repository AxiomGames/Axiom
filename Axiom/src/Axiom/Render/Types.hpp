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

enum class EResourceUsage : uint32
{
    Unknown         = 1 << 0,
    VertexBuffer    = 1 << 1,
    IndexBuffer     = 1 << 2,
    ConstantBuffer  = 1 << 3,
    ShaderResource  = 1 << 4,
    RenderTarget    = 1 << 5,
    DepthStencil    = 1 << 6,
    TransferSrc     = 1 << 7,
    TransferDst     = 1 << 8,
    UnorderedAccess = 1 << 9,
    HostVisible     = 1 << 10,

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
    Half4,
    UInt
};

static constexpr uint32 VertexAttribSize(VertexAttribType type)
{
    const size_t sizeLUT[] =
    {
        0u                   ,
        sizeof(float) * 1u   ,
        sizeof(float) * 2u   ,
        sizeof(float) * 3u   ,
        sizeof(float) * 4u   ,
        sizeof(short) * 1u   ,
        sizeof(short) * 2u   ,
        sizeof(short) * 3u   ,
        sizeof(short) * 4u   ,
        sizeof(unsigned) * 1u
    };
    return sizeLUT[(uint32)type];
}

enum class EGraphicsFormat
{
    UNKNOWN,
    RGBA8,
    RGBA8_SRGBF,
    RGBA16F,
    RGBA32F,
    R32U,
    R32F,
    D32F,
    Depth24Stencil8,
    RG32F,
    RG32U,
    RG16F
};
ENUM_FLAGS(EGraphicsFormat, uint32)

constexpr uint32 GetImageFormatSize(EGraphicsFormat format)
{
    constexpr uint32 imageFormatSizeLUT[] =
    {
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
    return imageFormatSizeLUT[(uint32)format];
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

enum class EShaderType : uint32
{
    None     ,
    Vertex   ,
    Pixel    ,
    Compute  ,
    Hull     ,
    Domain   ,
    All      = 6
};

enum class EFiltering : uint32
{
    Nearest = 0,
    Linear
};
ENUM_FLAGS(EFiltering, uint32)

enum class ETextureAddressMode : uint32
{
    Unknown,
    Wrap,
    Mirror,
    Clamp,
    ClampToEdge
};
ENUM_FLAGS(ETextureAddressMode, uint32)

enum class ETextureBorderColor : uint32
{
    TransparentBlack,
    OpaqueBlack,
    OpaqueWhite
};

struct SamplerInfo
{
    EFiltering MinFilter, MagFilter, MipFilter;
    ETextureAddressMode AdressU, AdressV, AdressW;
    ETextureBorderColor BorderColor;
    EShaderType ShaderVisibility = EShaderType::Pixel;
};

struct IImage : IGraphicsResource
{
		
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

enum class EBufferType : uint32
{
    Unknown  ,
    Buffer   , // vertex, index, srv, uniform, structured buffers
    Texture1D,
    Texture2D,
    Texture3D
};

enum class EBufferDescFlags : uint32
{
    Unknown,
    DirectUpload = 1 << 0, // no staging for example constant buffers
    TextureArray = 1 << 1,
    TextureCube  = 1 << 2
};
ENUM_FLAGS(EBufferDescFlags, uint32);

struct BufferDesc
{
	EResourceUsage ResourceUsage = EResourceUsage::Unknown;
    EBufferType BufferType = EBufferType::Unknown;
    EGraphicsFormat Format = EGraphicsFormat::UNKNOWN;
    uint64 Width, Height = 1; // if buffer is one dimensional Width is equal to size, no need to specify height
	uint64 ElementByteStride = 0;
	void* Data;
    EBufferDescFlags flags;
    int FrameIndex = 0; // required if object is going to allocate for each frame
};

struct IBuffer : IGraphicsResource
{
    BufferDesc Desc;
};

struct ISwapChain : IGraphicsResource
{
    virtual uint32 GetCurrentBackBufferIndex() = 0;
	virtual void Present(bool gsync, uint32 flags) = 0;
	virtual IImage* GetBackBuffer(int index) = 0;
    virtual IImage* GetDepthStencilBuffer() = 0;
};

struct ICommandAllocator : IGraphicsResource
{
    virtual void Reset() = 0;
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

