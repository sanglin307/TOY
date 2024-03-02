#include "Private.h"

Renderer& Renderer::Instance()
{
	static Renderer Inst;
	return Inst;
}

void Renderer::ParseCmds(const std::set<std::string>& cmds)
{
	_Config = {
		.API = RenderAPI::DX12,
		.RenderPath = RenderPathType::VisibilityBuffer,
		.FrameCount = 3,
		.FrameWidth = 1280,
		.FrameHeight = 800,
	};
}

void Renderer::Init(std::any hwnd)
{
	_HWND = hwnd;
 
	_Scene = new RenderScene;
	_Scene->Init();

	if (_Config.RenderPath == RenderPathType::VisibilityBuffer)
	{
		_RenderPath = new VisibilityBufferRP;
		_RenderPath->Init();
	}
}

void Renderer::Render()
{
	_RenderPath->Render();
}

void Renderer::Destroy()
{
	_Scene->Destroy();
	delete _Scene;
	_Scene = nullptr;

	_RenderPath->Destroy();
	delete _RenderPath;
	_RenderPath = nullptr;
}