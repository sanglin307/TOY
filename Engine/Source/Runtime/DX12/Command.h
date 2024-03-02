#pragma once

class DX12CommandQueue : public CommandQueue
{
	friend class DX12Device;
public:
	virtual std::any Handle() { return _Handle.Get(); };
	virtual ~DX12CommandQueue() { _Handle.Reset(); }
private:
	DX12CommandQueue(CommandType type, ComPtr<ID3D12CommandQueue> handle)
		:_Handle(handle)
	{
		_Type = type;
	}

	ComPtr<ID3D12CommandQueue> _Handle;
};

class DX12CommandAllocator : public CommandAllocator
{
	friend class DX12Device;
public:
	virtual ~DX12CommandAllocator() { _Handle.Reset(); }
	virtual std::any Handle() { return _Handle.Get(); }
private:
	DX12CommandAllocator(CommandType type, ComPtr<ID3D12CommandAllocator> handle)
		:_Handle(handle)
	{
		_Type = type;
	}

	ComPtr<ID3D12CommandAllocator> _Handle;
	
};