#pragma once

enum class PixelFormat : u32
{
    UNKNOWN = 0,
    R32G32B32A32_TYPELESS,
    R32G32B32A32_FLOAT,
    R32G32B32A32_UINT,
    R32G32B32A32_SINT,
    R32G32B32_TYPELESS,
    R32G32B32_FLOAT,
    R32G32B32_UINT,
    R32G32B32_SINT,
    R16G16B16A16_TYPELESS,
    R16G16B16A16_FLOAT,
    R16G16B16A16_UNORM,
    R16G16B16A16_UINT,
    R16G16B16A16_SNORM,
    R16G16B16A16_SINT,
    R32G32_TYPELESS,
    R32G32_FLOAT,
    R32G32_UINT,
    R32G32_SINT,
    R32G8X24_TYPELESS,
    D32_FLOAT_S8X24_UINT,
    R32_FLOAT_X8X24_TYPELESS,
    X32_TYPELESS_G8X24_UINT,
    R10G10B10A2_TYPELESS,
    R10G10B10A2_UNORM,
    R10G10B10A2_UINT,
    R11G11B10_FLOAT,
    R8G8B8A8_TYPELESS,
    R8G8B8A8_UNORM,
    R8G8B8A8_UNORM_SRGB,
    R8G8B8A8_UINT,
    R8G8B8A8_SNORM,
    R8G8B8A8_SINT,
    R16G16_TYPELESS,
    R16G16_FLOAT,
    R16G16_UNORM,
    R16G16_UINT,
    R16G16_SNORM,
    R16G16_SINT,
    R32_TYPELESS,
    D32_FLOAT,
    R32_FLOAT,
    R32_UINT,
    R32_SINT,
    R24G8_TYPELESS,
    D24_UNORM_S8_UINT,
    R24_UNORM_X8_TYPELESS,
    X24_TYPELESS_G8_UINT,
    R8G8_TYPELESS,
    R8G8_UNORM,
    R8G8_UINT,
    R8G8_SNORM,
    R8G8_SINT,
    R16_TYPELESS,
    R16_FLOAT,
    D16_UNORM,
    R16_UNORM,
    R16_UINT,
    R16_SNORM,
    R16_SINT,
    R8_TYPELESS,
    R8_UNORM,
    R8_UINT,
    R8_SNORM,
    R8_SINT,
    A8_UNORM,
    R1_UNORM,
    R9G9B9E5_SHAREDEXP,
    R8G8_B8G8_UNORM,
    G8R8_G8B8_UNORM,
    BC1_TYPELESS,
    BC1_UNORM,
    BC1_UNORM_SRGB,
    BC2_TYPELESS,
    BC2_UNORM,
    BC2_UNORM_SRGB,
    BC3_TYPELESS,
    BC3_UNORM,
    BC3_UNORM_SRGB,
    BC4_TYPELESS,
    BC4_UNORM,
    BC4_SNORM,
    BC5_TYPELESS,
    BC5_UNORM,
    BC5_SNORM,
    B5G6R5_UNORM,
    B5G5R5A1_UNORM,
    B8G8R8A8_UNORM,
    B8G8R8X8_UNORM,
    R10G10B10_XR_BIAS_A2_UNORM,
    B8G8R8A8_TYPELESS,
    B8G8R8A8_UNORM_SRGB,
    B8G8R8X8_TYPELESS,
    B8G8R8X8_UNORM_SRGB,
    BC6H_TYPELESS,
    BC6H_UF16,
    BC6H_SF16,
    BC7_TYPELESS,
    BC7_UNORM,
    BC7_UNORM_SRGB,
    MAX
};

struct PixelFormatInfo
{
    u8         ComponentNum;
    u8         ComponentSize;
    u8         ByteSize;
    bool       CompressFormat;
    std::any   PlatformFormat;
};

enum class DescriptorType
{
    CBV_SRV_UAV = 0,
    Sampler,
    RVT,
    DSV,
    Max
};

class DescriptorHeap
{
public:
    struct Config
    {
        DescriptorType Type;
        u32 Number;
        u32 Stride;
        bool GPUVisible;
    };

    virtual ~DescriptorHeap() {};
    virtual std::any Handle() { return nullptr; }
    virtual std::any GetCPUDescriptorHandle(u32 reserve = 0) { return nullptr; }
    virtual std::any GetGPUDescriptorHandle(u32 reserve = 0) { return nullptr; }
    u32 GetStride() { return _Config.Stride; }
  
protected:
    Config _Config;
    u32 _Offset = 0;
};

enum class ResourceState : u32
{
    Common = 0,
    VertexAndConstantBuffer = 0x1,
    IndexBuffer = 0x2,
    RenderTarget = 0x4,
    UnorderedAccess = 0x8,
    DepthWrite = 0x10,
    DepthRead = 0x20,
    NonPixelShaderResource = 0x40,
    PixelShaderResource = 0x80,
    StreamOut = 0x100,
    IndirectArgument = 0x200,
    CopyDest = 0x400,
    CopySource = 0x800,
    ResolveDest = 0x1000,
    ResolveSource = 0x2000,
    RaytracingAccelerationStructure = 0x400000,
    ShadingRateSource = 0x1000000,
    GenericRead = (((((VertexAndConstantBuffer | IndexBuffer) | NonPixelShaderResource) | PixelShaderResource) | IndirectArgument) | CopySource),
    AllShaderResource = (NonPixelShaderResource | PixelShaderResource),
    Present = 0,
    Predication = 0x200,
    Reserve = 0xFFFFFFFF   // used for reserve current state , don't change
};

enum class ResourceUsage : u32
{
    VertexBuffer = 1u << 0u,
    IndexBuffer = 1u << 1u,
    UniformBuffer = 1u << 2u,
    ShaderResource = 1u << 3u,
    StreamOutput = 1u << 4u,
    RenderTarget = 1u << 5u,
    DepthStencil = 1u << 6u,
    UnorderedAccess = 1u << 7u,
    IndirectDrawArgs = 1u << 8u,
    InputAttachment = 1u << 9u,
    RayTracing = 1u << 10u,
    ShadingRate = 1u << 11u
};

enum class ResourceDimension
{
    Buffer = 0,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray
};

enum class CpuAccessFlags : u32
{
    None = 0,
    Read = 1u << 0u,
    Write = 1u << 1u,
    ReadWrite = Read | Write
};


class RenderResource
{
public :
    virtual ResourceDimension GetDimension() = 0;
    virtual u32 GetUsage() = 0;
    virtual ~RenderResource() {};
    virtual std::any Handle() { return nullptr; }

    ResourceState State;
};

struct DelayDeleteResource
{
    u64 FrameNum;
    u32 DelayFrameNum;
    RenderResource* Resource;
};

class RenderBuffer : public RenderResource
{
public:
    struct Desc
    {
        u64 Size;
        u32 Stride;
        std::string Name;
        u32 Usage;
        u32 CpuAccess;
        bool Alignment;
        u8* InitData;
    };

    virtual ResourceDimension GetDimension() override { return ResourceDimension::Buffer; }
    virtual u32 GetUsage() override { return _Desc.Usage; }

protected:
    Desc _Desc;
};

class RenderTexture : public RenderResource
{
public:
    struct Desc
    {
        u32 Width;
        u32 Height;
        u32 Depth;
        PixelFormat Format;
        u32 Usage;
        ResourceDimension Dimension;
        u16 SampleCount = 1;
        u16 SampleQuality = 0;
    };

    virtual ResourceDimension GetDimension() override { return _Desc.Dimension; }
    virtual u32 GetUsage() override { return _Desc.Usage; }

protected:
    Desc _Desc;
};



class Swapchain
{
public:
    struct Desc
    {
        u32 Width;
        u32 Height;
        PixelFormat Format;
        std::any HWND;
        u32 FrameCount = 3;
        u16 SampleCount = 1;
        u16 SampleQuality = 0;
    };
    Swapchain(const Desc& info)
        :_Info(info)
    {}

    virtual ~Swapchain() {};

    virtual u32 GetCurrentFrameIndex() = 0;
    virtual RenderTexture* GetCurrentBackBuffer() = 0;
    virtual void Present(bool vSync) = 0;

protected:
    Desc _Info;
};