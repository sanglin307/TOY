#pragma once

class GameEngine
{
public:

	ENGINE_API static GameEngine& Instance();

	ENGINE_API void ParseCmds(const std::set<std::string>& cmds);
	ENGINE_API void Init(std::any hwnd);
	ENGINE_API void Destroy();

	ENGINE_API void Update();

	ENGINE_API void FrameSize(u32& Width, u32& Height);

private:
	GameEngine() = default;
	GameEngine(const GameEngine& rhs) = delete;
	GameEngine(GameEngine&& rhs) = delete;
	GameEngine& operator=(const GameEngine& rhs) = delete;
	GameEngine& operator=(GameEngine&& rhs) = delete;

private:
	std::set<std::string> _Params;
	FrameRate _FrameRate;
	RenderConfig _RenderConfig;
	Viewport* _GameViewport;
};
