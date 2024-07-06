#pragma once

class GameEngine
{
public:

	ENGINE_API static GameEngine& Instance();

	ENGINE_API void PreInit(const std::vector<std::string>& cmds);
	ENGINE_API void Init(std::any hwnd);
	ENGINE_API void Destroy();

	ENGINE_API void Update();

	ENGINE_API RenderConfig& GetRenderConfig() { return _RenderConfig; }

	IRHIModule& GetRHI();
	IRendererModule& GetRenderer();
	GameViewport* GetViewport()
	{
		return _GameViewport;
	}

private:
	GameEngine() = default;
	GameEngine(const GameEngine& rhs) = delete;
	GameEngine(GameEngine&& rhs) = delete;
	GameEngine& operator=(const GameEngine& rhs) = delete;
	GameEngine& operator=(GameEngine&& rhs) = delete;

	void InitConfig();

private:
	std::vector<std::string> _Params;
	FrameRate _FrameRate;
	RenderConfig _RenderConfig;
	GameViewport* _GameViewport;
};
