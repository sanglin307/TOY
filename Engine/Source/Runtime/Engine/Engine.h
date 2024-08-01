#pragma once

class GameEngine
{
public:

	ENGINE_API static GameEngine& Instance();

	ENGINE_API void PreInit(const std::vector<std::string>& cmds);
	ENGINE_API void Init(void* hwnd);
	ENGINE_API void Destroy();

	ENGINE_API void Update();

	ENGINE_API RenderConfig& GetRenderConfig() { return _RenderConfig; }

	IRHIModule& GetRHI();
	IRendererModule& GetRenderer();
	GameViewport* GetViewport()
	{
		return _GameViewport;
	}

	ENGINE_API void OnViewportResize(u32 width, u32 height, bool minimized);

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
	bool _WindowsVisible;
};
