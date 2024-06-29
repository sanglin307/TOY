#include "Private.h"

GameWorld& GameWorld::Instance()
{
	static GameWorld inst;
	return inst;
}


void GameWorld::Init()
{
	_RenderScene = GameEngine::Instance().GetRenderer().AllocateScene(this);
}

void GameWorld::Destroy()
{	
	GameEngine::Instance().GetRenderer().RemoveScene(_RenderScene);
}

void GameWorld::Update(double delta)
{

}