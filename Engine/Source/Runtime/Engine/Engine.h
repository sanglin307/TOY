#pragma once

#include <set>
#include <string>

#include "../Core/Types.h"
#include "Defines.h"

enum class ENGINE_API RenderAPI
{
	DX12,
	Vulkan,
	Metal,
};

class GameEngine
{
public:
	struct Config
	{
		u32 Width;
		u32 Height;
		u16 SampleCount;
		u32 BufferCount;
		RenderAPI API;
		std::string Title;
	};

	ENGINE_API static GameEngine& Instance()
	{
		static GameEngine Inst;
		return Inst;
	}

	ENGINE_API void ParseCmds(const std::set<std::string>& cmds);
	ENGINE_API void Init(void* hwnd);
	ENGINE_API void Destroy();

	ENGINE_API void Update();
	ENGINE_API void Render();

	ENGINE_API u32 GetWidth() const { return _Config.Width; }
	ENGINE_API u32 GetHeight() const { return _Config.Height; }
	ENGINE_API std::string& GetTitle() { return _Config.Title; }

private:
	GameEngine() = default;
	GameEngine(const GameEngine& rhs) = delete;
	GameEngine(GameEngine&& rhs) = delete;
	GameEngine& operator=(const GameEngine& rhs) = delete;
	GameEngine& operator=(GameEngine&& rhs) = delete;

private:
	std::set<std::string> _Params;

	Config _Config;
};
