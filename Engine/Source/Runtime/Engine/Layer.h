#pragma once

class Layer
{
public:
	Layer(const std::string& name);
	void AddNode(Node* node);
private:
	std::vector<Node*> _Nodes;
	std::string _Name;
};