#pragma once

class GameWorld
{
public:

	static GameWorld& Instance();

	void Init(const std::string& scenefile);
	void Destroy();

	void Update(double delta);

private:
	GameWorld() = default;
	GameWorld(const GameWorld& rhs) = delete;
	GameWorld(GameWorld&& rhs) = delete;
	GameWorld& operator=(const GameWorld& rhs) = delete;
	GameWorld& operator=(GameWorld&& rhs) = delete;

private:
	std::vector<Layer*> _Layers;
	IRenderScene*  _RenderScene;

	Camera* _DefaultCamera = nullptr;
};
