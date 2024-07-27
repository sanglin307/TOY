#include "Private.h"

Node::~Node()
{
	UnRegiste();
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
	_Transform.Translate = translate;
	_Transform.WorldMatrixInvalid = true;
}

void Node::SetRotation(const quaternion& rot)
{
	_Transform.Rotation = rot;
	_Transform.WorldMatrixInvalid = true;
}

void Node::SetScale(const float3& scale)
{
	_Transform.Scale = scale;
	_Transform.WorldMatrixInvalid = true;
}

Transform& Node::GetTransform()
{
	if (_Transform.WorldMatrixInvalid)
	{
		UpdateWorldMatrix();
	}

	return _Transform;
}

float4x4& Node::GetWorldMatrix()
{
	if (_Transform.WorldMatrixInvalid)
	{
		UpdateWorldMatrix();
	}

	return _Transform.WorldMatrix;
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

void Node::Registe()
{
	for (auto c : _Components)
		c->Registe();

	for (auto c : _Children)
		c->Registe();
}

void Node::UnRegiste()
{
	for (auto c : _Components)
		c->UnRegiste();

	for (auto c : _Children)
		c->UnRegiste();
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
	auto iter = std::find(_Components.begin(), _Components.end(), c);
	if (iter != _Components.end())
		return;

	_Components.push_back(c);
	c->Attach(this);
}

void Node::Detach(Component* c)
{
	auto iter = std::find(_Components.begin(), _Components.end(), c);
	if (iter == _Components.end())
		return;

	_Components.erase(iter);
	c->Detach();
}

void Node::UpdateWorldMatrix()
{
	_Transform.WorldMatrixInvalid = false;
	float4x4 parentMat = float4x4::identity();
	if (_Parent)
		parentMat = _Parent->GetWorldMatrix();

	_Transform.WorldMatrix = parentMat * float4x4::translation(_Transform.Translate) * float4x4(_Transform.Rotation) * float4x4::scale(_Transform.Scale);
	for (auto n : _Children)
	{
		n->UpdateWorldMatrix();
	}
}

void Node::Update(double delta)
{
	if (_Transform.WorldMatrixInvalid)
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