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

	void AddCluster(RenderCluster* cluster);
	void RemoveCluster(RenderCluster* cluster);

	RenderBuffer* GetViewUniformBuffer()
	{
		return _ViewUniformBuffer;
	}

	const SceneTextures& GetSceneTextures() const
	{
		return _SceneTextures;
	}

	RenderDevice* GetDevice() { return _Device; }

	void InitSceneTextures();

private:

	void InitRenderPass();

	RenderDevice* _Device;
	RenderScene* _Scene;
	RenderBuffer* _ViewUniformBuffer = nullptr;
	SceneTextures _SceneTextures = {};
	std::array<RenderPass*, (u32)RenderPassType::Max> _Passes;
};