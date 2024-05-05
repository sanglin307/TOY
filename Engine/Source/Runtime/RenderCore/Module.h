#pragma once

class IRendererModule : public ModuleInterface
{
public:
    virtual void Init() override {}
    virtual void Destroy() override {}
    virtual void Init(RenderAPI api) = 0;
    virtual void Render(RHIViewport* viewport) = 0;
};