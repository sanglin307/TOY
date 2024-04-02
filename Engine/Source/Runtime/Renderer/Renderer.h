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
    inline CommandList* GetDirectCommandList() { return _DirectCommandList; }
    inline CommandList* GetComputeCommandList() { return _ComputeCommandList; }
    inline CommandList* GetCopyCommandList() { return _CopyCommandList; }

private:
    Renderer() = default;
    Renderer(const Renderer& rhs) = delete;
    Renderer(Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer& rhs) = delete;
    Renderer& operator=(Renderer&& rhs) = delete;

private:
    RenderDevice* _Device;

    CommandQueue* _DirectCommandQueue = nullptr;
    CommandAllocator* _DirectCommandAllocator = nullptr;
    CommandList* _DirectCommandList = nullptr;

    CommandQueue* _ComputeCommandQueue = nullptr;
    CommandAllocator* _ComputeCommandAllocator = nullptr;
    CommandList* _ComputeCommandList = nullptr;

    CommandQueue* _CopyCommandQueue = nullptr;
    CommandAllocator* _CopyCommandAllocator = nullptr;
    CommandList* _CopyCommandList = nullptr;

    SwapChain* _SwapChain = nullptr;
    u32 _FrameIndex = 0;

    RenderConfig _Config;
    RenderScene* _Scene;
    RenderPath* _RenderPath;
    std::any _HWND;
};

#define RHI Renderer::Instance().GetDevice()