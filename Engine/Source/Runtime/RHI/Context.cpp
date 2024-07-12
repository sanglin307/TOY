#include "Private.h"

RenderContext::RenderContext(CommandType type, CommandAllocator* allocator, ContextManager* manager)
	:_Type(type),
	_Allocator(allocator),
	_Manager(manager),
	_State(ContextState::Close)
{
}

RenderContext* RenderContext::ReadyForRecord()
{
	if (_State == ContextState::Open)
		return this;

	Reset();
	return this;
}
 
ContextManager::ContextManager(RenderDevice* device)
{
	_Device = device;
	_Device->SetContextManager(this);

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

RenderContext* ContextManager::GetDirectContext(u32 index) 
{ 
	return _DirectContexts[index]->ReadyForRecord();
}

RenderContext* ContextManager::GetCopyContext()
{ 
	return _CopyContext->ReadyForRecord(); 
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
	{
		_NeedGpuWaitCopyFinish = false;
		return;
	}

	_CopyContext->Close();
	RenderContext* ctxs[] = { _CopyContext };
	_CopyCommandQueue->Excute(1, ctxs);
	_CopyCommandQueue->Signal(_CopyQueueFence, _CopyQueueFenceValue);

	_ContainCopyOp = false;
	_NeedGpuWaitCopyFinish = true;
}

void ContextManager::CpuWaitCopyFinish()
{
	if (_CopyQueueFence->GetCompletedValue() < _CopyQueueFenceValue)
	{
		_CopyQueueFence->CpuWait(_CopyQueueFenceValue);
	}
}

void ContextManager::GpuWaitCopyFinish()
{
	if (_NeedGpuWaitCopyFinish)
	{
		_DirectCommandQueue->Wait(_CopyQueueFence, _CopyQueueFenceValue);
		_NeedGpuWaitCopyFinish = false;
	}
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

	delete _FrameFence;
	_FrameFence = nullptr;

	delete _DirectCommandQueue;
	_DirectCommandQueue = nullptr;

	for (u32 index = 0; index < CommandAllocatorNumber; index++)
	{
		delete _DirectContexts[index];
		delete _DirectCommandAllocators[index];
	}

}