#pragma once

class CommandQueue
{
public:
	enum class Type
	{
		Direct = 0,
		Bundle,
		Compute,
		Copy
	};

	virtual ~CommandQueue() {};
protected:
	Type _Type;
};