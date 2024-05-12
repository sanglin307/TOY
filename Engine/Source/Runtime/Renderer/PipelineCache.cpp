#include "Private.h"

std::map<std::string, ShaderResource*> ShaderManager::_ShaderMap;

void ShaderManager::Add(ShaderResource* shader)
{
	if (_ShaderMap.contains(shader->GetHash()))
	{
		auto oldShader = _ShaderMap.find(shader->GetHash())->second;
		if (oldShader == shader)
			return;
		else
		{
			delete oldShader;
			_ShaderMap.erase(shader->GetHash());
		}
	}

	_ShaderMap.insert(std::pair<std::string, ShaderResource*>(shader->GetHash(), shader));
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

std::set<RootSignature*> RootSignatureManager::_RootSignatures;

void RootSignatureManager::Add(RootSignature* rs)
{
	_RootSignatures.insert(rs);
}

void RootSignatureManager::Remove(RootSignature* rs)
{
	_RootSignatures.erase(rs);
}

void RootSignatureManager::Destroy()
{
	for (auto iter : _RootSignatures)
	{
		delete iter;
	}

	_RootSignatures.clear();
}