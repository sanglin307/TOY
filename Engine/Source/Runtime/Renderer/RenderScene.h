#pragma once

class SceneRenderer;

struct PrimitiveSceneInfo
{
	InputLayout VertexLayout;
	std::vector<MeshVertexBuffer>  VertexBuffers;
	RenderBuffer* IndexBuffer = nullptr;
	u64 PrimitiveId;
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
	std::vector<PrimitiveSceneInfo*> _Primitives;
};
