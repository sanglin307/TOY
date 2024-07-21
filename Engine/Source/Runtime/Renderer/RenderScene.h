#pragma once

class SceneRenderer;

struct RenderCluster
{
	struct Stream
	{
		VertexAttribute Attribute;
		RenderBuffer* Buffer;
		u64 ByteOffset;
	};

	~RenderCluster();
	std::vector<Stream>  VertexStreams;
	RenderBuffer* IndexBuffer = nullptr;
	Material* Material = nullptr;
	PrimitiveData* Primitive;
	PrimitiveComponent* Component;
};

struct RenderLight
{
	~RenderLight();
	LightData* Light;
	LightComponent* Component = nullptr;
};

class RenderScene final: public IRenderScene
{
public:
	RenderScene(GameWorld* world, SceneRenderer* renderer);
	virtual ~RenderScene();

	virtual void AddPrimitive(const Transform& trans, PrimitiveComponent* primitive) override;
	virtual void RemovePrimitive(PrimitiveComponent* primitive) override;

	virtual void AddLight(const Transform& trans,LightComponent* light) override;
	virtual void RemoveLight(LightComponent* light) override;

	u32 GetLightNum()
	{
		return (u32)_Lights.size();
	}

private:

	GameWorld* _World;
	SceneRenderer* _Renderer;
	std::vector<RenderCluster*> _Clusters;
	std::vector<PrimitiveData> _PrimitiveDataBuffer;

	std::vector<RenderLight*> _Lights;
	std::vector<LightData> _LightDataBuffer;
	
};
