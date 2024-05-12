#pragma once


class RenderScene final: public IRenderScene
{
public:
	RenderScene(GameWorld* world);
	virtual ~RenderScene();

private:
	GameWorld* _World;
};
