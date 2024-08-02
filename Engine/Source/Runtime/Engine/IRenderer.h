#pragma once

struct Transform;
class PrimitiveComponent;
class LightComponent;
class SkyComponent;

class IRenderScene
{
public:
    virtual ~IRenderScene() {}
    virtual void AddPrimitive(const Transform& trans, PrimitiveComponent* primitive) = 0;
    virtual void RemovePrimitive(PrimitiveComponent* primitive) = 0;
    virtual void AddLight(const Transform& trans, LightComponent* light) = 0;
    virtual void RemoveLight(LightComponent* light) = 0;
    virtual void AddSky(SkyComponent* sky) = 0;
    virtual void RemoveSky(SkyComponent* sky) = 0;
};

class GameWorld;
class IRendererModule : public ModuleInterface
{
public:
    virtual void Render(ViewInfo& info, Swapchain* viewport) = 0;
    virtual IRenderScene* AllocateScene(GameWorld* world) = 0;
    virtual void RemoveScene(IRenderScene* scene) = 0;
    virtual void OnResize(u32 width, u32 height) = 0;
};