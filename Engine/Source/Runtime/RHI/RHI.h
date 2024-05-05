#pragma once


class IRHIModule : public ModuleInterface
{
public:
    virtual void Init() override {};
    virtual void Destroy() override {};
    virtual RenderDevice* CreateDevice() = 0;
    virtual RenderContext* CreateContext(u32 frameCount) = 0;
};



