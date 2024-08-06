#pragma once


constexpr inline u32 DivideRoundup(u32 nominator, u32 denominator)
{
	return (nominator + denominator - 1) / denominator;
};


enum class RenderPassType
{
	Forward = 0,
	Sky,
	Tonemap,
	Max
};

struct RenderCommand
{
	u32 PrimitiveID = 0;
	PrimitiveComponent* Component = nullptr;
	std::vector<RenderBuffer*> VertexBuffers;
	RenderBuffer* PackedVertexBuffer = nullptr;
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
	RenderPass(RenderPassType type, RenderDevice* device, SceneRenderer* renderer)
		:_Type(type),_Device(device),_Renderer(renderer)
	{}

	virtual ~RenderPass() {};
	virtual void Render(ViewInfo& view, Swapchain* sc,RenderContext* ctx) = 0;
	virtual void AddCluster(u32 primitiveID, RenderCluster* cluster) {};
	virtual void RemoveCluster(RenderCluster* cluster) {};

protected:
	RenderPassType _Type;
	RenderDevice* _Device;
	SceneRenderer* _Renderer;
};


class RenderPassForward final : public RenderPass
{
public:
	RenderPassForward(RenderDevice* device, SceneRenderer* renderer);
	virtual ~RenderPassForward();
	virtual void Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx) override;
	virtual void AddCluster(u32 primitiveID, RenderCluster* cluster) override;
	virtual void RemoveCluster(RenderCluster* cluster) override;

private:
	std::list<RenderCommand*> _Commands;
	GraphicPipeline* _ScenePso = nullptr;
	RenderBuffer* _MaterialBuffer = nullptr;
};

class RenderPassTonemap final : public RenderPass
{
public:
	RenderPassTonemap(RenderDevice* device, SceneRenderer* renderer);
	virtual ~RenderPassTonemap() {};
	virtual void Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx) override;

private:
	ComputePipeline* _TonemapPso = nullptr;
};

class RenderPassSky final : public RenderPass
{
public:
	RenderPassSky(RenderDevice* device, SceneRenderer* renderer);
	virtual ~RenderPassSky() {};
	virtual void Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx) override;

private:
	GraphicPipeline* _SkyPso = nullptr;
	RenderBuffer* _SkyCB = nullptr;
};