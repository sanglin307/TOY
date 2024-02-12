#include "GameEngine.h"

GameEngine& GameEngine::Instance()
{
	static GameEngine Inst;
	return Inst;

}

void GameEngine::Init(const std::set<std::string>& cmds)
{

}

void GameEngine::Destroy()
{

}

void GameEngine::Update()
{

}

void GameEngine::Render()
{

}