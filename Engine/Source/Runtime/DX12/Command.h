#pragma once

#include "../Renderer/Command.h"
#include "Defines.h"
#include "Device.h"

class DX12CommandQueue : public CommandQueue
{
	friend class DX12Device;
public:
	static D3D12_COMMAND_LIST_TYPE TranslateType(const CommandQueue::Type t);

	ID3D12CommandQueue* Handle() { return _Handle; };
	
	virtual ~DX12CommandQueue();

private:
	DX12CommandQueue(ID3D12CommandQueue* handle)
		:_Handle(handle)
	{
	}

	ID3D12CommandQueue* _Handle = nullptr;
};