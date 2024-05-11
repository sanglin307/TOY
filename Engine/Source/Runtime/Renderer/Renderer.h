#pragma once


class RendererModule : public IRendererModule
{
public:
    virtual void Init() override;
    virtual void Destroy() override;
    virtual void Render(Swapchain* viewport) override;

private:
    RenderScene* _Scene;
    RenderDevice* _Device;
    RenderConfig _RenderConfig;
};