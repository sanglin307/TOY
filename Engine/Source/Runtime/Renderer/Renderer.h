#pragma once


class RendererModule final: public IRendererModule
{
public:
    virtual void Init() override;
    virtual void Destroy() override;
    virtual void Render(Swapchain* viewport) override;
    virtual IRenderScene* AllocateScene(GameWorld* world) override;
    virtual void RemoveScene(IRenderScene* scene) override;

private:
    RenderScene*  _Scene;
    RenderDevice* _Device;
    RenderConfig _RenderConfig;
    SceneRenderer* _SceneRender;
};