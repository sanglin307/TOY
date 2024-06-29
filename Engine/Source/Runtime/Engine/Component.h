#pragma once

class Node;

class Component
{
public:
	virtual ~Component();
	virtual void Attach(Node* node);

protected:
	Node* _Owner = nullptr;
};