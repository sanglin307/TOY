#pragma once

struct ViewInfo
{
	float4x4 View;
	float4x4 ViewInverse;
	float4x4 Project;
	float4x4 ProjectInverse;
	float4x4 ViewProject;
	float4x4 ViewProjectInverse;
	float3   ViewLocation;
	u32 FrameIndex;
};

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
    virtual void Render(ViewInfo& info, Swapchain* viewport) = 0;
    virtual IRenderScene* AllocateScene(GameWorld* world) = 0;
    virtual void RemoveScene(IRenderScene* scene) = 0;
};