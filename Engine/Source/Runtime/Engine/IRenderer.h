#pragma once

class PrimitiveComponent;
class IRenderScene
{
public:
    virtual ~IRenderScene() {}
    virtual void AddPrimitive(PrimitiveComponent* primitive) = 0;
    virtual void RemovePrimitive(PrimitiveComponent* primitive) = 0;
};

class GameWorld;
class IRendererModule : public ModuleInterface
{
public:
    virtual void Render(Swapchain* viewport) = 0;
    virtual IRenderScene* AllocateScene(GameWorld* world) = 0;
    virtual void RemoveScene(IRenderScene* scene) = 0;
};