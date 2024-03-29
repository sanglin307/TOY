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
        u32 Size;
        bool GPUVisible;
    };

    virtual ~DescriptorHeap() {};
    virtual std::any Handle() { return nullptr; }

protected:
    Config _Config;
};

 
class RenderResource
{
public :

    virtual ~RenderResource() {};
    virtual std::any Handle() { return nullptr; }
};

enum class ViewDimension
{
    Buffer = 0,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture2DMS,
    Texture2DMSArray,
    Texture3D
};

class ResourceView
{
public:
    virtual ~ResourceView() {};
};

class RenderTargetView : public ResourceView
{
public:
    struct Config
    {
        PixelFormat Format;
        ViewDimension Dimension;
        u64 FirstElement = 0;
        u32 NumElements = 0;
        u32 MipSlice = 0;
        u32 PlaneSlice = 0;
        u32 FirstArraySlice = 0;
        u32 ArraySize = 0;
        u32 FirstDepthSlice = 0;
        i32 DepthSize = -1;
    };

    ~RenderTargetView() {};

protected:
    RenderResource* _Resource = nullptr;
    Config _Config;
};

class UnorderedAccessView : public ResourceView
{
private:
    RenderResource* _Resource = nullptr;
    RenderResource* _CounterResource = nullptr;
};

class ShaderResourceView : public ResourceView
{
private:
    RenderResource* _Resource = nullptr;
};

class ConstantBufferView : public ResourceView
{
private:
};

class DepthStencilView : public ResourceView
{
private:
    RenderResource* _Resource = nullptr;
};

class SwapChain
{
public:
    struct Config
    {
        u32 Width;
        u32 Height;
        u16 SampleCount;
        u16 SampleQuality;
        u32 BufferCount;
        PixelFormat Format;
    };

    virtual u32 CurrentFrameIndex() { return 0; };
    virtual ~SwapChain() {};
    virtual std::any Handle() { return nullptr; }

protected:
    Config _Config;
    DescriptorHeap* _DescriptorHeap = nullptr;
    std::vector<RenderResource*> _RenderTargets;
    std::vector<RenderTargetView*> _RenderTargetViews;
};