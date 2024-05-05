#pragma once


class RendererModule : public IRendererModule
{
public:
    virtual void Init(RenderAPI api) override;
    virtual void Destroy() override;
    virtual void Render(RHIViewport* viewport) override;

    static RendererModule& Instance();

private:
    RenderScene* _Scene;

    RenderDevice* _Device;
    RenderContext* _Context;

    DescriptorHeap* _GlobalDescriptorHeap;
    DescriptorHeap* _SamplerDescriptorHeap;
    DescriptorHeap* _RVTDescriptorHeap;
    DescriptorHeap* _DSVDescriptorHeap;

    SwapChain* _SwapChain = nullptr;
    u32 _FrameIndex = 0;

    Fence* _Fence;

    static constexpr u32 SwapChainFrameCount = 3;
 
};