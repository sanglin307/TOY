#pragma once

enum class PrimitiveType
{
	Mesh = 0,
};

class PrimitiveComponent : public Component
{
public:
	virtual PrimitiveType GetType() = 0;

	virtual ComponentType GetComponentType() override
	{
		return ComponentType::Primitive;
	}

	virtual void RegisteToScene() override;

protected:
};