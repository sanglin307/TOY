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
	virtual void Init(RenderDevice* device) = 0;
	virtual void Render(RenderDevice* device, RenderContext* ctx) = 0;
	virtual void AddPrimitive(PrimitiveSceneInfo* primitive) = 0;
	virtual void RemovePrimitive(PrimitiveSceneInfo* primitive) = 0;

protected:
	RenderPassType _Type;
};


class RenderPassTest final : public RenderPass
{
public:
	virtual ~RenderPassTest();
	virtual void Init(RenderDevice* device) override;
	virtual void Render(RenderDevice* device, RenderContext* ctx) override;
	virtual void AddPrimitive(PrimitiveSceneInfo* primitive) override;
	virtual void RemovePrimitive(PrimitiveSceneInfo* primitive) override;

	struct SceneConstantBuffer
	{
		glm::vec4 offset;
		float padding[60]; // Padding so the constant buffer is 256-byte aligned.
	};

private:
	std::vector<MeshCommand*> _Commands;
	GraphicPipeline* PSO = nullptr;
	RenderBuffer* UniformBuffer;
	SceneConstantBuffer UniformData = {};
	RenderTexture* _texture;
	Sampler* _sampler;
};