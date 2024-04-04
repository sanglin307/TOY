#pragma once

class ModuleInterface
{
public:
	virtual void Init() = 0;
	virtual void Destroy() = 0;

};

class ModuleManager
{
public:
	CORE_API static ModuleInterface* LoadModule(const std::string& name);

private:
	static std::map<std::string, ModuleInterface*> _ModuleMap;
};

typedef ModuleInterface* (*fnCreateModule)();