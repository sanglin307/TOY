#include "GameEngine.h"
#include "../Renderer/Device.h"
#include "../DX12/Device.h"

void GameEngine::Init(const std::set<std::string>& cmds)
{
	if (cmds.contains("-vulkan"))
		DeviceType = RenderDeviceType::Vulkan;

	if (DeviceType == RenderDeviceType::DX12)
	{
		RenderDevice<DX12Device>::Instance().Init();
	}
	
}

void GameEngine::Destroy()
{
	if (DeviceType == RenderDeviceType::DX12)
	{
		RenderDevice<DX12Device>::Instance().Destroy();
	}
}

void GameEngine::Update()
{

}

void GameEngine::Render()
{

}