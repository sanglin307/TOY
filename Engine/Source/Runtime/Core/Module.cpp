#include "Private.h"

std::map<std::string, ModuleInterface*> ModuleManager::_ModuleMap;

ModuleInterface* ModuleManager::LoadModule(const std::string& name)
{
	auto iter = _ModuleMap.find(name);
	if (iter != _ModuleMap.end())
		return iter->second;

	std::filesystem::path fileName = PathUtil::Bin() / std::format("{}.dll",name);
	if (!std::filesystem::exists(fileName))
	{
		return nullptr;
	}

	HMODULE handle = LoadLibraryA(fileName.string().c_str());
	if (handle == nullptr)
	{
		return nullptr;
	}

	fnCreateModule fn = (fnCreateModule)GetProcAddress(handle, "CreateModule");
	if (fn == nullptr)
	{
		return nullptr;
	}

	ModuleInterface* mi = fn();
	_ModuleMap.insert(std::pair<std::string, ModuleInterface*>(name, mi));

	return mi;

}