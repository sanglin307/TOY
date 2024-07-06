#pragma once

class Node
{
public:
	~Node();
	Node(const std::string& name);
	void SetTranslate(const float3& translate);
	void SetRotation(const quaternion& rot);
	void SetScale(const float3& scale);

	const float3& GetTranslate() const
	{
		return _Translate;
	}

	const quaternion& GetRotation() const
	{
		return _Rotation;
	}

	const float3& GetScale() const
	{
		return _Scale;
	}

	void RegisteToScene();

	float4x4& GetWorldMatrix();

	Component* FindFirstComponent(ComponentType type);

	void Attach(Component* c);
	void AddChild(Node* node);
	void SetParent(Node* node);

	void Update(double delta);
private:

	void UpdateWorldMatrix();

	std::string _Name;
	std::vector<Component*> _Components;
	float3 _Translate;
	quaternion _Rotation;
	float3 _Scale;
	float4x4 _WorldMatrix;

	bool _WorldMatrixInvalid = false;

	Node* _Parent = nullptr;
	std::vector<Node*> _Children;

};