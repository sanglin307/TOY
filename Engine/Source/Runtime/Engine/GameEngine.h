#pragma once

#include <set>
#include <string>

#include "../Core/Types.h"
#include "../Renderer/Device.h"
#include "Defines.h"

class GameEngine
{
public:

	ENGINE_API static GameEngine& Instance()
	{
		static GameEngine Inst;
		return Inst;
	}

	ENGINE_API void Init(const std::set<std::string>& cmds);
	ENGINE_API void Destroy();

	ENGINE_API void Update();
	ENGINE_API void Render();

	ENGINE_API u32 GetWidth() const { return Width; }
	ENGINE_API u32 GetHeight() const { return Height; }
	ENGINE_API std::string& GetTitle() { return Title; }

private:
	GameEngine() = default;
	GameEngine(const GameEngine& rhs) = delete;
	GameEngine(GameEngine&& rhs) = delete;
	GameEngine& operator=(const GameEngine& rhs) = delete;
	GameEngine& operator=(GameEngine&& rhs) = delete;

private:
	std::set<std::string> Params;

	RenderDeviceType DeviceType = RenderDeviceType::DX12;
	std::string Title = "TOY";
	u32 Width = 1280;
	u32 Height = 720;
};
