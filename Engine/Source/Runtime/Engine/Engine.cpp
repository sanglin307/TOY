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
	if (cmds.size() > 0)
	{
		for (auto cmd : cmds)
		{
			TOY_Log(Engine, std::format("launch argument:{}", cmd));
		}
	}

	_Params = cmds;
	Renderer::Instance().ParseCmds(cmds);
}

void GameEngine::Init(std::any hwnd)
{
	LogUtil::Init();
	_FrameRate.Init();

	Renderer::Instance().Init(hwnd);	
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

	//Update world

	Renderer::Instance().Render();
}

void GameEngine::FrameSize(u32& Width, u32& Height)
{
	const RenderConfig& Config = Renderer::Instance().Config();
	Width = Config.FrameWidth;
	Height = Config.FrameHeight;
}