#pragma once

class IRenderScene
{
public:
    virtual ~IRenderScene() {}
};

class GameWorld;
class IRendererModule : public ModuleInterface
{
public:
    virtual void Render(Swapchain* viewport) = 0;
    virtual IRenderScene* AllocateScene(GameWorld* world) = 0;
    virtual void RemoveScene(IRenderScene* scene) = 0;
};