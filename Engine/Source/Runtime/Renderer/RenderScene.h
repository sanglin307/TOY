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
	PrimitiveComponent* Component;
};

class RenderScene final: public IRenderScene
{
public:
	RenderScene(GameWorld* world, SceneRenderer* renderer);
	virtual ~RenderScene();

	virtual void AddPrimitive(PrimitiveComponent* primitive) override;
	virtual void RemovePrimitive(PrimitiveComponent* primitive) override;

private:
	GameWorld* _World;
	SceneRenderer* _Renderer;
	std::vector<RenderCluster*> _Clusters;
};
