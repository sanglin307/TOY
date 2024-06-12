#include "Private.h"

void GraphicPipeline::BindParameter(const std::string& name, RenderResource* resource)
{
	auto iter = _ShaderParameters.find(name);
	check(iter != _ShaderParameters.end());
	iter->second->Resource = resource;
}

void GraphicPipeline::BindParameter(RenderContext* ctx)
{
	for (auto pair : _ShaderParameters)
	{
		if (!pair.second->Resource)
			continue;

		ctx->SetGraphicShaderParameter(pair.second);
	}
}

void GraphicPipeline::AddParameter(ShaderParameter* parameter)
{
	_ShaderParameters[parameter->Name] = parameter;
}

ShaderParameter* GraphicPipeline::GetParameter(const std::string& name)
{
	auto iter = _ShaderParameters.find(name);
	if (iter == _ShaderParameters.end())
		return nullptr;

	return iter->second;
}