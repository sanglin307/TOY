#include "Private.h"

CommandManager* GCommandManager = nullptr;
CommandManager& CommandManager::Instance()
{
	check(GCommandManager);
	return *GCommandManager;
}

void CommandManager::Init(u32 frameCount)
{
	GCommandManager = this;
	_FrameCount = frameCount;
	_DirectCommandAllocator = new CommandAllocator * [frameCount];
	_ComputeCommandAllocator = new CommandAllocator * [frameCount];
	_CopyCommandAllocator = new CommandAllocator * [frameCount];

	RenderDevice& device = RenderDevice::Instance();

	for (u32 index = 0; index < frameCount; index++)
	{
		_DirectCommandAllocator[index]= device.CreateCommandAllocator(CommandType::Direct);
		_ComputeCommandAllocator[index] = device.CreateCommandAllocator(CommandType::Compute);
		_CopyCommandAllocator[index] = device.CreateCommandAllocator(CommandType::Copy);
	}

	_DirectCommandQueue = device.CreateCommandQueue(CommandType::Direct);
	_DirectCommandList = device.CreateCommandList(_DirectCommandAllocator[0], CommandType::Direct);

	_ComputeCommandQueue = device.CreateCommandQueue(CommandType::Compute);
	_ComputeCommandList = device.CreateCommandList(_ComputeCommandAllocator[0], CommandType::Compute);

	_CopyCommandQueue = device.CreateCommandQueue(CommandType::Copy);
	_CopyCommandList = device.CreateCommandList(_CopyCommandAllocator[0], CommandType::Copy);
}

void CommandManager::Destroy()
{
	if (_CopyCommandList)
	{
		delete _CopyCommandList;
		_CopyCommandList = nullptr;
	}

	if (_CopyCommandQueue)
	{
		delete _CopyCommandQueue;
		_CopyCommandQueue = nullptr;
	}

	if (_ComputeCommandList)
	{
		delete _ComputeCommandList;
		_ComputeCommandList = nullptr;
	}

	if (_ComputeCommandQueue)
	{
		delete _ComputeCommandQueue;
		_ComputeCommandQueue = nullptr;
	}

	if (_DirectCommandList)
	{
		delete _DirectCommandList;
		_DirectCommandList = nullptr;
	}

	if (_DirectCommandQueue)
	{
		delete _DirectCommandQueue;
		_DirectCommandQueue = nullptr;
	}

	for (u32 index = 0; index < _FrameCount; index++)
	{
		delete _CopyCommandAllocator[index];
		delete _ComputeCommandAllocator[index];
		delete _DirectCommandAllocator[index];
	}

	delete[] _CopyCommandAllocator;
	delete[] _ComputeCommandAllocator;
	delete[] _DirectCommandAllocator;
}

void CommandManager::Prepare(u32 frameIndex)
{
	_DirectCommandList->Prepare(_DirectCommandAllocator[frameIndex]);
}

void CommandManager::End(Texture2DResource* presentResource)
{
	_DirectCommandList->End(presentResource);

	std::vector<CommandList*> commandList;
	commandList.push_back(_DirectCommandList);
	_DirectCommandQueue->Excute(commandList);
}

void CommandManager::SetViewport(const Viewport& viewport)
{
	_DirectCommandList->SetViewport(viewport);
}
void CommandManager::SetScissorRect(u32 left, u32 top, u32 right, u32 bottom)
{
	_DirectCommandList->SetScissorRect(left, top, right, bottom);
}
void CommandManager::SetRenderTargets(std::vector<Texture2DResource*>& rts, Texture2DResource* depthStencil)
{
	_DirectCommandList->SetRenderTargets(rts, depthStencil);
}
void CommandManager::ClearRenderTarget(Texture2DResource* renderTarget, std::array<float, 4>& colors)
{
	_DirectCommandList->ClearRenderTarget(renderTarget, colors);
}
void CommandManager::SetGraphicsRootSignature(RootSignature* signature)
{
	_DirectCommandList->SetGraphicsRootSignature(signature);
}

void CommandManager::CopyResource(RenderResource* dstRes, RenderResource* srcRes)
{
	_DirectCommandList->CopyResource(dstRes, srcRes);
}