#pragma once

enum class CommandType
{
	Direct = 0,
	Bundle,
	Compute,
	Copy
};

class CommandQueue
{
public:
	virtual ~CommandQueue() {};
	virtual std::any Handle() { return nullptr; }
protected:
	CommandType _Type;
};

class CommandAllocator
{
public:
	virtual ~CommandAllocator() {};
	virtual std::any Handle() { return nullptr; }

protected:
	CommandType _Type;

};

class CommandList
{
public:
	virtual ~CommandList() {};
	virtual std::any Handle() { return nullptr; }

protected:
	CommandType _Type;
	CommandAllocator* _Allocator;
};

class CommandManager
{
public:
	void Init();
	void Destroy();

	CommandQueue* DirectQueue() { return _DirectCommandQueue; }
	CommandList* DirectList() { return _DirectCommandList; }

	CommandQueue* ComputeQueue() { return _ComputeCommandQueue; }
	CommandList* ComputeList() { return _ComputeCommandList; }

	CommandQueue* CopyQueue() { return _CopyCommandQueue; }
	CommandList* CopyList() { return _CopyCommandList; }

private:
	CommandQueue* _DirectCommandQueue = nullptr;
	CommandAllocator* _DirectCommandAllocator = nullptr;
	CommandList* _DirectCommandList = nullptr;

	CommandQueue* _ComputeCommandQueue = nullptr;
	CommandAllocator* _ComputeCommandAllocator = nullptr;
	CommandList* _ComputeCommandList = nullptr;

	CommandQueue* _CopyCommandQueue = nullptr;
	CommandAllocator* _CopyCommandAllocator = nullptr;
	CommandList* _CopyCommandList = nullptr;
};