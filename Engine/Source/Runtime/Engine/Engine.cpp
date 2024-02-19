#include "Engine.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Device.h"

GameEngine& GameEngine::Instance()
{
	static GameEngine Inst;
	return Inst;
}

void GameEngine::ParseCmds(const std::set<std::string>& cmds)
{
	_Params = cmds;

	_Config.Width = 1280;
	_Config.Height = 720;
	_Config.SampleCount = 1;
	_Config.BufferCount = 3;
	_Config.API = RenderAPI::DX12;
	_Config.Title = "TOY";

	if (cmds.contains("-vulkan"))
		_Config.API = RenderAPI::Vulkan;
}

void GameEngine::Init(void* hwnd)
{
	LogUtil::Init();
	_FrameRate.Init();

	Renderer::Instance().Init(_Config,hwnd);	
}

void GameEngine::Destroy()
{
	Renderer::Instance().Destroy();
	_FrameRate.Destroy();
	LogUtil::Destroy();
}

void GameEngine::Update()
{
	double delta = _FrameRate.Update();
	LogUtil::Update(delta);
}

void GameEngine::Render()
{

}