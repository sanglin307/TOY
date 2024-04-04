#pragma once

enum class RenderAPI
{
    DX12,
    Vulkan,
    Metal,
};

enum class RenderPathType
{
    VisibilityBuffer = 0,
};

struct RenderConfig
{
    RenderAPI API;
    RenderPathType RenderPath;
    u32 FrameCount;
    u32 FrameWidth;
    u32 FrameHeight;

};

class RHIModule : public ModuleInterface
{
public:
    virtual void Init() override {}
    virtual void Destroy() override {}
    virtual RenderDevice* CreateDevice() = 0;
};

class Renderer
{
public:
    RENDERER_API static Renderer& Instance();
    RENDERER_API void ParseCmds(const std::set<std::string>& cmds);
    RENDERER_API void Init(std::any hwnd);
    RENDERER_API void Destroy();
    RENDERER_API void Render();
    RENDERER_API const RenderConfig& Config() const { return _Config; }
    const std::any HWND() const { return _HWND; }
    inline RenderDevice& GetDevice() { return *_Device; }
    RENDERER_API inline DescriptorHeap* RVTDescriptorHeap() { return _RVTDescriptorHeap; }
private:
    Renderer() = default;
    Renderer(const Renderer& rhs) = delete;
    Renderer(Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer& rhs) = delete;
    Renderer& operator=(Renderer&& rhs) = delete;

private:
    RenderDevice* _Device;
    CommandManager* _CommandMgr;

    DescriptorHeap* _GlobalDescriptorHeap;
    DescriptorHeap* _SamplerDescriptorHeap;
    DescriptorHeap* _RVTDescriptorHeap;
    DescriptorHeap* _DSVDescriptorHeap;

    SwapChain* _SwapChain = nullptr;
    u32 _FrameIndex = 0;

    RenderConfig _Config;
    RenderScene* _Scene;
    RenderPath* _RenderPath;
    std::any _HWND;
};

#define RHI Renderer::Instance().GetDevice()