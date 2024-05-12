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

class IRHIModule : public ModuleInterface
{
public:
    virtual RenderDevice* GetDevice() = 0;
};
 