#pragma once

enum class RenderPassType
{
	Test = 0,
	Max
};


class RenderPass
{
public:
	virtual ~RenderPass() {};
	virtual void Init(RenderDevice* device,SceneRenderer* renderer) = 0;
	virtual void Render(RenderDevice* device, RenderContext* ctx) = 0;
	virtual void AddCluster(RenderCluster* cluster) = 0;
	virtual void RemoveCluster(RenderCluster* cluster) = 0;

protected:

	void BindVertexStreams(GraphicPipeline* pso, RenderCluster* cluster, RenderContext* ctx);

	RenderPassType _Type;
	RenderDevice* _Device;
	SceneRenderer* _Renderer;
};


class RenderPassTest final : public RenderPass
{
public:
	virtual ~RenderPassTest();
	virtual void Init(RenderDevice* device, SceneRenderer* renderer) override;
	virtual void Render(RenderDevice* device, RenderContext* ctx) override;
	virtual void AddCluster(RenderCluster* cluster) override;
	virtual void RemoveCluster(RenderCluster* cluster) override;

private:
	std::set<RenderCluster*> _Clusters;
	GraphicPipeline* PSO = nullptr;
	RenderTexture* _texture1;
	RenderTexture* _texture2;
	Sampler* _sampler;
};