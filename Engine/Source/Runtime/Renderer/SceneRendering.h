#pragma once

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

	RenderDevice* GetDevice() { return _Device; }

private:
	RenderDevice* _Device;
	RenderConfig _RenderConfig;
	RenderScene* _Scene;
	RenderBuffer* _ViewUniformBuffer;
	std::array<RenderPass*, (u32)RenderPassType::Max> _Passes;
};