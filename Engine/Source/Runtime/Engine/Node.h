#pragma once

class Node
{
public:
	~Node();
	Node(const std::string& name);
	void SetTranslate(const float3& translate);
	void SetRotation(const quaternion& rot);
	void SetScale(const float3& scale);
 
	Transform& GetTransform();

	void Attach(Component* c);
	void Detach(Component* c);

	void Registe();
	void UnRegiste();

	ENGINE_API float4x4& GetWorldMatrix();

	Component* FindFirstComponent(ComponentType type);

	
	void AddChild(Node* node);
	void SetParent(Node* node);

	void Update(double delta);
private:

	void UpdateWorldMatrix();

	std::string _Name;
	std::vector<Component*> _Components;
	Transform _Transform;

	Node* _Parent = nullptr;
	std::vector<Node*> _Children;

};