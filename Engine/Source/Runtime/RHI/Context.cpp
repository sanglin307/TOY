#include "Private.h"

RenderContext::RenderContext(CommandType type, CommandAllocator* allocator)
	:_Type(type),
	_Allocator(allocator)
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


	_ComputeCommandAllocator = device->CreateCommandAllocator(CommandType::Compute);
	_ComputeCommandQueue = device->CreateCommandQueue(CommandType::Compute);
	_ComputeContext = device->CreateCommandContext(_ComputeCommandAllocator, CommandType::Compute);

	_CopyCommandAllocator = device->CreateCommandAllocator(CommandType::Copy);
	_CopyCommandQueue = device->CreateCommandQueue(CommandType::Copy);
	_CopyContext = device->CreateCommandContext(_CopyCommandAllocator, CommandType::Copy);
}

ContextManager::~ContextManager()
{
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