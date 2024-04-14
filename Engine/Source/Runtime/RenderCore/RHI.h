#pragma once

class RHIModule : public ModuleInterface
{
public:
    virtual void Init() override {}
    virtual void Destroy() override {}
    virtual RenderDevice* CreateDevice() = 0;
};