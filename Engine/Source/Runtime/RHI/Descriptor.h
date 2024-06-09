#pragma once

class DescriptorManager
{
public:
    RHI_API DescriptorManager(RenderDevice* device);
    RHI_API ~DescriptorManager();

    DescriptorHeap* GetRVTHeap() { return _RVTDescriptorHeap; }
    DescriptorHeap* GetShaderResourceHeap() { return _ShaderResourceDescriptorHeap; }

private:

    DescriptorHeap* _ShaderResourceDescriptorHeap;
    DescriptorHeap* _SamplerDescriptorHeap;
    DescriptorHeap* _RVTDescriptorHeap;
    DescriptorHeap* _DSVDescriptorHeap;

    RenderDevice* _Device;

    static constexpr u32 GlobalDescriptorNumber = 2000;
    static constexpr u32 SamplerDescriptorNumber = 500;
    static constexpr u32 RVTDescriptorNumber = 64;
    static constexpr u32 DSVDescriptorNumber = 32;
};