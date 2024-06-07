#include "Private.h"

IMPLEMENT_MODULE(RendererModule)

IRHIModule& GetRHI(RenderAPI api)
{
	IRHIModule* module = nullptr;
	if (api == RenderAPI::DX12)
	{
		module = static_cast<IRHIModule*>(ModuleManager::LoadModule("DX12"));
	}

	check(module);

	return *module;
}

IRenderScene* RendererModule::AllocateScene(GameWorld* world)
{
	check(_Scene == nullptr);
	_Scene = new RenderScene(world,_SceneRender);
	return _Scene;
}

void RendererModule::RemoveScene(IRenderScene* scene)
{
	check(scene == _Scene);
	delete _Scene;
	_Scene = nullptr;
}

void RendererModule::Init()
{
	_RenderConfig = GameEngine::Instance().GetRenderConfig();
	_Device = GetRHI(_RenderConfig.API).GetDevice();
	_Device->InitPipelineCache();

	_SceneRender = new SceneRenderer(_Device);
}
 
void RendererModule::Render(Swapchain* sc)
{ 
	_SceneRender->Render(sc);	 
}

void RendererModule::Destroy()
{
	delete _SceneRender;
}