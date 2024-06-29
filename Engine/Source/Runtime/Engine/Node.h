#pragma once

class Node
{
public:
	Node(const std::string& name);
	void SetTranslate(const glm::vec3& translate);
	void SetRotation(const glm::quat& rot);
	void SetScale(const glm::vec3& scale);

	const glm::vec3& GetTranslate() const
	{
		return _Translate;
	}

	const glm::quat& GetRotation() const
	{
		return _Rotation;
	}

	const glm::vec3& GetScale() const
	{
		return _Scale;
	}

	void Attach(Component* c);
	void AddChild(Node* node);
	void SetParent(Node* node);

private:
	std::string _Name;
	std::vector<Component*> _Components;
	glm::vec3 _Translate;
	glm::quat _Rotation;
	glm::vec3 _Scale;

	Node* _Parent = nullptr;
	std::vector<Node*> _Children;

};