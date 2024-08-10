#pragma once

struct SceneTextures
{
	RenderTexture* SceneColor;
	RenderTexture* SceneDepth;
	RenderTexture* ColorOutput;
};

class SceneRenderer
{
public:
	SceneRenderer(RenderDevice* device);
	~SceneRenderer();
	void Render(ViewInfo& view, Swapchain* viewport);
	void BindScene(RenderScene* scene);

	void AddCluster(u32 primitiveId,RenderCluster* cluster);
	void RemoveCluster(RenderCluster* cluster);

	RenderBuffer* GetLightsBuffer()
	{
		return _LightsBuffer;
	}

	RenderBuffer* GetPrimitivesBuffer()
	{
		return _PrimitivesBuffer;
	}

	const SceneTextures& GetSceneTextures() const
	{
		return _SceneTextures;
	}

	RenderDevice* GetDevice() { return _Device; }
	RenderScene* GetScene() { return _Scene; }
	void InitSceneTextures();

	void UpdateLightBuffer(const std::vector<LightData>& lightsData);
	void UpdatePrimitivesBuffer(const std::vector<PrimitiveData>& primitivesData);
private:

	void InitRenderPass();

	RenderDevice* _Device;
	RenderScene* _Scene;
	RenderBuffer* _LightsBuffer = nullptr;
	RenderBuffer* _PrimitivesBuffer = nullptr;
	SceneTextures _SceneTextures = {};
	std::array<RenderPass*, (u32)RenderPassType::Max> _Passes;
};