#include "Command.h"

D3D12_COMMAND_LIST_TYPE DX12CommandQueue::TranslateType(const CommandQueue::Type t)
{
	if (t == CommandQueue::Type::Direct)
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	else if (t == CommandQueue::Type::Bundle)
		return D3D12_COMMAND_LIST_TYPE_BUNDLE;
	else if (t == CommandQueue::Type::Compute)
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	else if (t == CommandQueue::Type::Copy)
		return D3D12_COMMAND_LIST_TYPE_COPY;
	else
	{
		assert(0);
		return D3D12_COMMAND_LIST_TYPE_NONE;
	}
}

DX12CommandQueue::~DX12CommandQueue()
{
	if(_Handle)
		_Handle->Release();

	_Handle = nullptr;
}