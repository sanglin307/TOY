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
protected:
	CommandType _Type;
};

class CommandAllocator
{
public:
	virtual ~CommandAllocator() {};

protected:
	CommandType _Type;

};