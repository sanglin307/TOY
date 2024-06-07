#include "Private.h"

RenderContext::RenderContext(CommandType type, CommandAllocator* allocator, ContextManager* manager)
	:_Type(type),
	_Allocator(allocator),
	_Manager(manager)
{
}
 
ContextManager::ContextManager(RenderDevice* device)
{
	_Device = device;
	for (u32 index = 0; index < CommandAllocatorNumber; index++)
	{
		_DirectCommandAllocators.push_back(device->CreateCommandAllocator(CommandType::Direct));
		_DirectContexts.push_back(device->CreateCommandContext(_DirectCommandAllocators[index], CommandType::Direct));
		_FenceValues[index] = 0;
	}
	_DirectCommandQueue = device->CreateCommandQueue(CommandType::Direct);
	_FrameFence = device->CreateFence(0);

	_ComputeCommandAllocator = device->CreateCommandAllocator(CommandType::Compute);
	_ComputeCommandQueue = device->CreateCommandQueue(CommandType::Compute);
	_ComputeContext = device->CreateCommandContext(_ComputeCommandAllocator, CommandType::Compute);

	_CopyCommandAllocator = device->CreateCommandAllocator(CommandType::Copy);
	_CopyCommandQueue = device->CreateCommandQueue(CommandType::Copy);
	_CopyContext = device->CreateCommandContext(_CopyCommandAllocator, CommandType::Copy);
	_CopyQueueFence = device->CreateFence(_CopyQueueFenceValue);
}

void ContextManager::SwitchToNextFrame(u32 lastFrameIndex, u32 nextFrameIndex)
{
	u64& lastFenceValue = _FenceValues[lastFrameIndex];
	u64& nextFenceValue = _FenceValues[nextFrameIndex];

	_DirectCommandQueue->Signal(_FrameFence, lastFenceValue);

	if (_FrameFence->GetCompletedValue() < nextFenceValue)
	{
		_FrameFence->CpuWait(nextFenceValue);
	}

	nextFenceValue = lastFenceValue + 1; // next frame fence value.
}

void ContextManager::WaitGPUIdle()
{
	u64 lastFenceValue = 0;
	for (u32 i = 0; i < CommandAllocatorNumber; i++)
	{
		if (_FenceValues[i] > lastFenceValue)
			lastFenceValue = _FenceValues[i];
	}

	_DirectCommandQueue->Signal(_FrameFence, lastFenceValue);
	_FrameFence->CpuWait(lastFenceValue);
}

void ContextManager::AddCopyNum()
{
	_CopyQueueFenceValue++;
	_ContainCopyOp = true;
}

void ContextManager::CommitCopyCommand()
{
	if (!_ContainCopyOp)
		return;

	_CopyContext->Close();

	RenderContext* ctxs[] = { _CopyContext };
	_CopyCommandQueue->Excute(1, ctxs);
	_CopyCommandQueue->Signal(_CopyQueueFence, _CopyQueueFenceValue);

	_ContainCopyOp = false;
}

void ContextManager::GpuWaitCopyFinish()
{
	check(_ContainCopyOp);  // must finish commit.
	_DirectCommandQueue->Wait(_CopyQueueFence, _CopyQueueFenceValue);
}

ContextManager::~ContextManager()
{
	delete _CopyQueueFence;
	_CopyQueueFence = nullptr;

	delete _CopyContext;
	_CopyContext = nullptr;

	delete _CopyCommandQueue;
	_CopyCommandQueue = nullptr;

	delete _CopyCommandAllocator;
	_CopyCommandAllocator = nullptr;

	delete _ComputeContext;
	_ComputeContext = nullptr;

	delete _ComputeCommandQueue;
	_ComputeCommandQueue = nullptr;

	delete _ComputeCommandAllocator;
	_ComputeCommandAllocator = nullptr;

	delete _DirectCommandQueue;
	_DirectCommandQueue = nullptr;

	for (u32 index = 0; index < CommandAllocatorNumber; index++)
	{
		delete _DirectContexts[index];
		delete _DirectCommandAllocators[index];
	}

}