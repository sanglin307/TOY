#include "Private.h"

GameEngine& GameEngine::Instance()
{
	static GameEngine Inst;
	return Inst;
}

IRendererModule& GameEngine::GetRenderer()
{
	static IRendererModule* module = nullptr;
	if (module == nullptr)
	{
		module = static_cast<IRendererModule*>(ModuleManager::LoadModule("Renderer"));
	}

	return *module;
}

IRHIModule& GameEngine::GetRHI()
{
	IRHIModule* module = nullptr;
	if (_RenderConfig.API == RenderAPI::DX12)
	{
		module = static_cast<IRHIModule*>(ModuleManager::LoadModule("DX12"));
	}

	check(module);

	return *module;
}

void GameEngine::ParseCmds(const std::set<std::string>& cmds)
{
	_RenderConfig = RenderConfig{
		.API = RenderAPI::DX12,
		.FrameCount = 3,
		.FrameWidth = 1280,
		.FrameHeight = 800
	};

	if (cmds.size() > 0)
	{
		for (auto cmd : cmds)
		{
			LOG_INFO(Engine, std::format("launch argument:{}", cmd));
		}
	}

	_Params = cmds;
}



void GameEngine::Init(std::any hwnd)
{
	LogUtil::Init();

	_FrameRate.Init();

	GetRHI().Init();

	_GameViewport = new GameViewport(hwnd, _RenderConfig);
 
	GetRenderer().Init();
	GameWorld::Instance().Init();
}

void GameEngine::Destroy()
{
	GetRHI().GetDevice()->WaitGPUIdle();

	GameWorld::Instance().Destroy();

	GetRenderer().Destroy();

	delete _GameViewport;
	_GameViewport = nullptr;

	GetRHI().Destroy();

	_FrameRate.Destroy();
	LogUtil::Destroy();
}

void GameEngine::Update()
{
	double delta = _FrameRate.Update();
	LogUtil::Update(delta);

	GameWorld::Instance().Update(delta);

	GetRenderer().Render(_GameViewport->GetRHI());
}

void GameEngine::FrameSize(u32& Width, u32& Height)
{
	Width = _RenderConfig.FrameWidth;
	Height = _RenderConfig.FrameHeight;
}