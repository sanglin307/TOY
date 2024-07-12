#include "Private.h"

Node::~Node()
{
	for (Component* c : _Components)
	{
		delete c;
	}

	for (Node* c : _Children)
	{
		delete c;
	}

}

Node::Node(const std::string& name)
{
	_Name = name;
}

void Node::SetTranslate(const float3& translate)
{
	_Translate = translate;
	_WorldMatrixInvalid = true;
}

void Node::SetRotation(const quaternion& rot)
{
	_Rotation = rot;
	_WorldMatrixInvalid = true;
}

void Node::SetScale(const float3& scale)
{
	_Scale = scale;
	_WorldMatrixInvalid = true;
}

float4x4& Node::GetWorldMatrix()
{
	if (_WorldMatrixInvalid)
	{
		UpdateWorldMatrix();
	}

	return _WorldMatrix;
}

void Node::AddChild(Node* node)
{
	check(std::find(_Children.begin(), _Children.end(), node) == _Children.end());
	_Children.push_back(node);
	node->SetParent(this);
}

void Node::SetParent(Node* node)
{
	_Parent = node;
}

void Node::RegisteToScene()
{
	for (auto c : _Components)
		c->RegisteToScene();

	for (auto c : _Children)
		c->RegisteToScene();
}

Component* Node::FindFirstComponent(ComponentType type)
{
	for (auto c : _Components)
	{
		if (c->GetComponentType() == type)
		{
			return c;
		}
	}

	return nullptr;
}

void Node::Attach(Component* c)
{
	check(std::find(_Components.begin(), _Components.end(), c) == _Components.end());
	_Components.push_back(c);
	c->Attach(this);
}

void Node::UpdateWorldMatrix()
{
	_WorldMatrixInvalid = false;
	float4x4 parentMat = float4x4::identity();
	if (_Parent)
		parentMat = _Parent->GetWorldMatrix();

	_WorldMatrix = parentMat * float4x4::translation(_Translate) * float4x4(_Rotation) * float4x4::scale(_Scale);
	for (auto n : _Children)
	{
		n->UpdateWorldMatrix();
	}
}

void Node::Update(double delta)
{
	if (_WorldMatrixInvalid)
	{
		UpdateWorldMatrix();
	}

	// component first
	for (auto c : _Components)
	{
		c->Update(delta);
	}

	for (auto n : _Children)
	{
		n->Update(delta);
	}
}