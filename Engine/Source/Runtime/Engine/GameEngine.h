#pragma once

#include <set>
#include <string>

#include "../Core/Types.h"
#include "Defines.h"

class GameEngine
{
public:
	ENGINE_API static GameEngine& Instance();
	ENGINE_API void Init(const std::set<std::string>& cmds);
	ENGINE_API void Destroy();

	ENGINE_API void Update();
	ENGINE_API void Render();

	ENGINE_API u32 GetWidth() const { return Width; }
	ENGINE_API u32 GetHeight() const { return Height; }
	ENGINE_API std::string& GetTitle() { return Title; }

	GameEngine(const GameEngine& rhs) = delete;
	GameEngine(GameEngine&& rhs) = delete;
	GameEngine& operator=(const GameEngine& rhs) = delete;
	GameEngine& operator=(GameEngine&& rhs) = delete;

private:
	GameEngine() = default;
	std::set<std::string> Params;

	std::string Title = "TOY";
	u32 Width = 1280;
	u32 Height = 720;
};
