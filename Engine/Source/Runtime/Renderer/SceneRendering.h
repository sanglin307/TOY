#pragma once

class SceneRenderer
{
public:
	SceneRenderer(RenderDevice* device);
	~SceneRenderer();
	void Render(Swapchain* viewport);
	void BindScene(RenderScene* scene);

	void AddCluster(RenderCluster* cluster);
	void RemoveCluster(RenderCluster* cluster);

	RenderDevice* GetDevice() { return _Device; }
private:
	RenderDevice* _Device;
	RenderConfig _RenderConfig;
	RenderScene* _Scene;
	std::array<RenderPass*, (u32)RenderPassType::Max> _Passes;
};