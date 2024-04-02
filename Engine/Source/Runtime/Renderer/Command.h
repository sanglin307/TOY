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