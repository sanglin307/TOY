#pragma once

#include "../Renderer/Command.h"
#include "Defines.h"
#include "Device.h"

class DX12CommandQueue : public CommandQueue
{
	friend class DX12Device;
public:
	ID3D12CommandQueue* Handle() { return _Handle; };
	
	virtual ~DX12CommandQueue() { _Handle->Release(); _Handle = nullptr; }

private:
	DX12CommandQueue(CommandType type, ID3D12CommandQueue* handle)
		:_Handle(handle)
	{
		_Type = type;
	}

	ID3D12CommandQueue* _Handle = nullptr;
};

class DX12CommandAllocator : public CommandAllocator
{
	friend class DX12Device;
public:
	virtual ~DX12CommandAllocator() { _Handle->Release(); _Handle = nullptr; }

private:
	DX12CommandAllocator(CommandType type,ID3D12CommandAllocator* handle)
		:_Handle(handle)
	{
		_Type = type;
	}

	ID3D12CommandAllocator* _Handle = nullptr;
	
};