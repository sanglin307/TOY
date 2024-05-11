#pragma once


class RendererModule : public IRendererModule
{
public:
    virtual void Init() override;
    virtual void Destroy() override;
    virtual void Render(RHIViewport* viewport) override;

private:
    RenderScene* _Scene;
    RenderDevice* _Device;
    RenderConfig _RenderConfig;
};