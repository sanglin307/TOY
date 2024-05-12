#pragma once


class RendererModule : public IRendererModule
{
public:
    virtual void Init() override;
    virtual void Destroy() override;
    virtual void Render(Swapchain* viewport) override;
    virtual IRenderScene* AllocateScene(GameWorld* world) override;
    virtual void RemoveScene(IRenderScene* scene) override;

private:
    std::set<IRenderScene*> _Scenes;
    RenderDevice* _Device;
    RenderConfig _RenderConfig;
};