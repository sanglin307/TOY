#pragma once

class GameWorld
{
public:

	static GameWorld& Instance();

	void Init();
	void Destroy();

	void Update(double delta);

private:
	GameWorld() = default;
	GameWorld(const GameWorld& rhs) = delete;
	GameWorld(GameWorld&& rhs) = delete;
	GameWorld& operator=(const GameWorld& rhs) = delete;
	GameWorld& operator=(GameWorld&& rhs) = delete;

private:
	std::vector<PrimitiveComponent*> _Primitives;
	IRenderScene*  _RenderScene;
};
