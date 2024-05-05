#include "Private.h"

DX12RenderContext::DX12RenderContext(u32 frameCount)
{
	_FrameCount = frameCount;
	_DirectCommandAllocator = new CommandAllocator * [frameCount];
	_ComputeCommandAllocator = new CommandAllocator * [frameCount];
	_CopyCommandAllocator = new CommandAllocator * [frameCount];

	RenderDevice& device = RenderDevice::Instance();

	for (u32 index = 0; index < frameCount; index++)
	{
		_DirectCommandAllocator[index] = device.CreateCommandAllocator(CommandType::Direct);
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

DX12RenderContext::~DX12RenderContext()
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