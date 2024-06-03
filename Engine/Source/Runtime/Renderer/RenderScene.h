#pragma once


class RenderScene final: public IRenderScene
{
public:
	RenderScene(GameWorld* world);
	virtual ~RenderScene();

	virtual void AddPrimitive(PrimitiveComponent* primitive) override;
	virtual void RemovePrimitive(PrimitiveComponent* primitive) override;

private:
	GameWorld* _World;
	std::vector<PrimitiveComponent*> _Primitives;
};
