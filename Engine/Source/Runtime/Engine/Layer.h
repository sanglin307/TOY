#pragma once

class Layer
{
public:
	~Layer();
	Layer(const std::string& name);
	void AddNode(Node* node);

	Component* FindFirstComponent(ComponentType type);

	void Update(double delta);
private:
	std::vector<Node*> _Nodes;
	std::string _Name;
};