#pragma once


class RendererModule : public IRendererModule
{
public:
    virtual void Init() override;
    virtual void Destroy() override;
    virtual void CreateRenderer(std::any hwnd, const RenderConfig& config) override;
    virtual void Render() override;
    static RendererModule& Instance();

    inline RenderConfig& Config() { return _Config; }
    inline RenderDevice& GetDevice() { return *_Device; }
    inline CommandManager& GetCommandManager() { return *_CommandMgr; }
    inline DescriptorHeap* RVTDescriptorHeap() { return _RVTDescriptorHeap; }

private:

    void PreRender();
    void PostRender();

private:
    RenderDevice* _Device;
    CommandManager* _CommandMgr;

    DescriptorHeap* _GlobalDescriptorHeap;
    DescriptorHeap* _SamplerDescriptorHeap;
    DescriptorHeap* _RVTDescriptorHeap;
    DescriptorHeap* _DSVDescriptorHeap;

    SwapChain* _SwapChain = nullptr;
    u32 _FrameIndex = 0;

    Fence* _Fence;

    RenderScene* _Scene;
    std::any _HWND;

    RenderConfig _Config;
 
};

#define RD RendererModule::Instance().GetDevice()
#define RC RendererModule::Instance().GetCommandManager()