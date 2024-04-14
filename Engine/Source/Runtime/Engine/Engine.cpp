#include "Private.h"

GameEngine& GameEngine::Instance()
{
	static GameEngine Inst;
	return Inst;
}

IRendererModule& GetRendererModule()
{
	static IRendererModule* module = nullptr;
	if (module == nullptr)
	{
		module = static_cast<IRendererModule*>(ModuleManager::LoadModule("Renderer"));
	}

	return *module;
}

void GameEngine::ParseCmds(const std::set<std::string>& cmds)
{
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

	_RenderConfig = RenderConfig{
		.API = RenderAPI::DX12,
		.FrameCount = 3,
		.FrameWidth = 1280,
		.FrameHeight = 800
	};

	GetRendererModule().CreateRenderer(hwnd, _RenderConfig);
}

void GameEngine::Destroy()
{
	GetRendererModule().Destroy();
	_FrameRate.Destroy();
	LogUtil::Destroy();
}

void GameEngine::Update()
{
	double delta = _FrameRate.Update();
	LogUtil::Update(delta);

	//Update world

	GetRendererModule().Render();
}

void GameEngine::FrameSize(u32& Width, u32& Height)
{
	Width = _RenderConfig.FrameWidth;
	Height = _RenderConfig.FrameHeight;
}