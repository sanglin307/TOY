#include "Private.h"

void CommandManager::Init()
{
	_DirectCommandQueue = RHI.CreateCommandQueue(CommandType::Direct);
	_DirectCommandAllocator = RHI.CreateCommandAllocator(CommandType::Direct);
	_DirectCommandList = RHI.CreateCommandList(_DirectCommandAllocator, CommandType::Direct);

	_ComputeCommandQueue = RHI.CreateCommandQueue(CommandType::Compute);
	_ComputeCommandAllocator = RHI.CreateCommandAllocator(CommandType::Compute);
	_ComputeCommandList = RHI.CreateCommandList(_ComputeCommandAllocator, CommandType::Compute);

	_CopyCommandQueue = RHI.CreateCommandQueue(CommandType::Copy);
	_CopyCommandAllocator = RHI.CreateCommandAllocator(CommandType::Copy);
	_CopyCommandList = RHI.CreateCommandList(_CopyCommandAllocator, CommandType::Copy);
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

	if (_CopyCommandAllocator)
	{
		delete _CopyCommandAllocator;
		_CopyCommandAllocator = nullptr;
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

	if (_DirectCommandAllocator)
	{
		delete _DirectCommandAllocator;
		_DirectCommandAllocator = nullptr;
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

	if (_DirectCommandAllocator)
	{
		delete _DirectCommandAllocator;
		_DirectCommandAllocator = nullptr;
	}
}