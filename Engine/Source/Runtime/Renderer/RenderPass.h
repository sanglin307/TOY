#pragma once


constexpr inline u32 DivideRoundup(u32 nominator, u32 denominator)
{
	return (nominator + denominator - 1) / denominator;
};


enum class RenderPassType
{
	Test = 0,
	Max
};

struct RenderCommand
{
	PrimitiveComponent* Component = nullptr;
	std::vector<RenderBuffer*> VertexBuffers;
	RenderBuffer* IndexBuffer = nullptr;
	MaterialData   Material;
	RenderTexture* BaseColor = nullptr;
	RenderTexture* Normal = nullptr;
	RenderTexture* RoughMetelness = nullptr;
	RenderTexture* Emissive = nullptr;
};

class RenderPass
{
public:
	virtual ~RenderPass() {};
	virtual void Init(RenderDevice* device,SceneRenderer* renderer) = 0;
	virtual void Render(ViewInfo& view, Swapchain* sc,RenderContext* ctx) = 0;
	virtual void AddCluster(RenderCluster* cluster) = 0;
	virtual void RemoveCluster(RenderCluster* cluster) = 0;

protected:
	RenderPassType _Type;
	RenderDevice* _Device;
	SceneRenderer* _Renderer;
};


class RenderPassTest final : public RenderPass
{
public:
	virtual ~RenderPassTest();
	virtual void Init(RenderDevice* device, SceneRenderer* renderer) override;
	virtual void Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx) override;
	virtual void AddCluster(RenderCluster* cluster) override;
	virtual void RemoveCluster(RenderCluster* cluster) override;

private:
	std::list<RenderCommand*> _Commands;
	GraphicPipeline* ScenePso = nullptr;
	ComputePipeline* TonemapPso = nullptr;
	RenderBuffer* MaterialBuffer = nullptr;
	Sampler* _Sampler;
};