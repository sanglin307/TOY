#include "Private.h"

#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

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

void GameEngine::PreInit(const std::vector<std::string>& cmds)
{
	if (cmds.size() > 0)
	{
		for (auto cmd : cmds)
		{
			LOG_INFO(Engine, std::format("launch argument:{}", cmd));
		}
	}

	_Params = cmds;

	LogUtil::Init();
	InitConfig();
	_FrameRate.Init();
}



void GameEngine::Init(std::any hwnd)
{
	GetRHI().Init();

	_GameViewport = new GameViewport(hwnd, _RenderConfig);
 
	GetRenderer().Init();

	std::string scenefile = "Box.glb";
	if (_Params.size() > 0)
	{
		//check first param 
		if (_Params[0].find(".") != std::string::npos)
		{
			scenefile = _Params[0];
		}
	}

	GameWorld::Instance().Init(scenefile);
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

void GameEngine::InitConfig()
{
	std::filesystem::path epath = PathUtil::Config() / "Engine.toml";
	toml::parse_result engineConfig = toml::parse_file(epath.c_str());
	if (!engineConfig)
	{
		LOG_ERROR(EngineConfig, std::format("parse Engine.toml error {}",engineConfig.error().description()));
		return;
	}

	toml::table engineTable = std::move(engineConfig).table();

	auto rconfig = engineTable["RenderConfig"];
	std::string_view api = rconfig["API"].value_or("DX12");

	RenderAPI renderAPI;
	if (std::strcmp(api.data(), "DX12") == 0)
		renderAPI = RenderAPI::DX12;
	else if (std::strcmp(api.data(), "Vulkan") == 0)
		renderAPI = RenderAPI::Vulkan;
	else if (std::strcmp(api.data(), "Metal") == 0)
		renderAPI = RenderAPI::Metal;
	else
		check(0);

	_RenderConfig = RenderConfig{
		.API = renderAPI,
		.FrameWidth = rconfig["FrameWidth"].value_or(1600u),
		.FrameHeight = rconfig["FrameHeight"].value_or(900u)
	};

}