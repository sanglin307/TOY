#pragma once

struct Transform
{
	float3 Translate;
	quaternion Rotation;
	float3 Scale;
	float4x4 WorldMatrix;
	bool WorldMatrixInvalid;
};

enum class ComponentType
{
	Camera = 0,
	Light,
	Primitive,
	Max
};

class Node;

class Component
{
public:
	virtual ~Component();
	void Attach(Node* node);
	void Detach();

	virtual ComponentType GetComponentType() {
		return ComponentType::Max;
	}

	virtual void Registe() {}
	virtual void UnRegiste() {}

	virtual void Update(double delta) {};

protected:
	Node* _Owner = nullptr;
};