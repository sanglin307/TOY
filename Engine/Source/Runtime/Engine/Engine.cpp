#include "Engine.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Device.h"

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
	Renderer::Instance().Init(_Config,hwnd);	
}

void GameEngine::Destroy()
{
	Renderer::Instance().Destroy();
	LogUtil::Destroy();
}

void GameEngine::Update()
{

}

void GameEngine::Render()
{

}