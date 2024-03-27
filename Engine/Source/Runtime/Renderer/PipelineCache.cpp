#include "Private.h"

std::map<std::string, ShaderObject*> ShaderManager::_ShaderMap;

void ShaderManager::Add(ShaderObject* shader)
{
	if (_ShaderMap.contains(shader->Hash))
	{
		auto oldShader = _ShaderMap.find(shader->Hash)->second;
		if (oldShader == shader)
			return;
		else
		{
			delete oldShader;
			_ShaderMap.erase(shader->Hash);
		}
	}

	_ShaderMap.insert(std::pair<std::string, ShaderObject*>(shader->Hash, shader));
}

void ShaderManager::Remove(const std::string& hash)
{
	auto exist = _ShaderMap.find(hash);
	if (exist == _ShaderMap.end())
		return;

	delete exist->second;
	_ShaderMap.erase(hash);
}

void ShaderManager::Destroy()
{
	for (auto iter : _ShaderMap)
	{
		delete iter.second;
	}

	_ShaderMap.clear();
}

std::set<GraphicPipeline*> PipelineManager::_Pipelines;

void PipelineManager::Add(GraphicPipeline* pso)
{
	_Pipelines.insert(pso);
}

void PipelineManager::Remove(GraphicPipeline* pso)
{
	_Pipelines.erase(pso);
}

void PipelineManager::Destroy()
{
	for (auto iter : _Pipelines)
	{
		delete iter;
	}

	_Pipelines.clear();
}