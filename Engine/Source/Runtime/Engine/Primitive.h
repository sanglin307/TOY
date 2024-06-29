#pragma once

enum class PrimitiveType
{
	Mesh = 0,
};

class PrimitiveComponent : public Component
{
public:
	virtual PrimitiveType GetType() = 0;
	u32 GetPrimitiveId()
	{
		return _PrimitiveId;
	}
protected:
	u32 _PrimitiveId = 0;
};