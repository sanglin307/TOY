#pragma once

enum class RenderAPI
{
    DX12,
    Vulkan,
    Metal,
};


struct RenderConfig
{
    RenderAPI API;
    u32 FrameCount;
    u32 FrameWidth;
    u32 FrameHeight;

};

class IRendererModule : public ModuleInterface
{
public:
    virtual void Render(Swapchain* viewport) = 0;
};

class IRHIModule : public ModuleInterface
{
public:
    virtual RenderDevice* GetDevice() = 0;
};
 