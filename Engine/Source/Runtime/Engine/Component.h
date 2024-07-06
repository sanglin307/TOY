#pragma once

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
	virtual void Attach(Node* node);
	virtual ComponentType GetComponentType() {
		return ComponentType::Max;
	}

	virtual void RegisteToScene() {}

	virtual void Update(double delta) {};
protected:
	Node* _Owner = nullptr;
};