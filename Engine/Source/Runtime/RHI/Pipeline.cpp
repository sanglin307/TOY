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

		ShaderBindType t = pair.second->BindType;
		if (t == ShaderBindType::RootCBV || t == ShaderBindType::RootSRV || t == ShaderBindType::RootUAV)
			ctx->SetGraphicShaderParameter(pair.second);
	}

	ctx->SetGraphicTableParameter(_RootSignature, _CBVs);
	ctx->SetGraphicTableParameter(_RootSignature, _SRVs);
	ctx->SetGraphicTableParameter(_RootSignature, _UAVs);
	ctx->SetGraphicTableParameter(_RootSignature, _Samplers);

}


void GraphicPipeline::AllocateParameters(RootSignature* rs, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders)
{
	_RootSignature = rs;
	check(_ShaderParameters.size() == 0);
	const std::vector<RootSignatureParamDesc>& paramDesc = rs->GetParamDesc();
	const RootSignature::Desc& desc = rs->GetDesc();
	check(paramDesc.size() > 0);

	u32 ParamNum[(u32)ShaderBindType::Max] = {};
	u32 ParamOffset[(u32)ShaderBindType::Max] = {};
	ShaderBindType t = paramDesc[0].Type;
	ParamOffset[(u32)t] = 0;
	for (u32 i = 1; i < paramDesc.size(); i++)
	{
		if (t != paramDesc[i].Type)
		{
			t = paramDesc[i].Type; // new 
			ParamOffset[(u32)t] = i;
		}
	}

	for (u32 i = 0; i < (u32)ShaderProfile::MAX; i++)
	{
		if (!shaders[i])
			continue;

		ShaderReflection* reflection = shaders[i]->GetReflection();
		if (reflection && reflection->BoundResources.size() > 0)
		{
			for (u32 r = 0; r < reflection->BoundResources.size(); r++)
			{
				if (_ShaderParameters.contains(reflection->BoundResources[r].Name))
					continue;

				ShaderParameter* param = new ShaderParameter;
				const SRBoundResource& res = reflection->BoundResources[r];
				param->Name = res.Name;
				_ShaderParameters[param->Name] = param;
				if (res.Type == ShaderInputType::CBUFFER)
				{
					if (res.BindSpace == RootSignature::cRootDescriptorSpace)
					{
						check(ParamNum[(u32)ShaderBindType::RootCBV] < desc.RootCBVNum);
						param->BindType = ShaderBindType::RootCBV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::RootCBV] + ParamNum[(u32)ShaderBindType::RootCBV];
						param->TableOffset = 0;
						ParamNum[(u32)ShaderBindType::RootCBV]++;
					}
					else
					{
						check(ParamNum[(u32)ShaderBindType::TableCBV] < desc.TableCBVNum);
						param->BindType = ShaderBindType::TableCBV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableCBV];
						param->TableOffset = res.BindPoint;
						ParamNum[(u32)ShaderBindType::TableCBV]++;
						_CBVs.push_back(param);
					}
				}
				else if ((res.Type == ShaderInputType::TBUFFER || res.Type == ShaderInputType::TEXTURE || res.Type == ShaderInputType::STRUCTURED ||
					res.Type == ShaderInputType::BYTEADDRESS))
				{
					if (res.BindSpace == RootSignature::cRootDescriptorSpace)
					{
						check(ParamNum[(u32)ShaderBindType::RootSRV] < desc.RootSRVNum);
						param->BindType = ShaderBindType::RootSRV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::RootSRV] + ParamNum[(u32)ShaderBindType::RootSRV];
						param->TableOffset = 0;
						ParamNum[(u32)ShaderBindType::RootSRV]++;
					}
					else
					{
						check(ParamNum[(u32)ShaderBindType::TableSRV] < desc.TableSRVNum);
						param->BindType = ShaderBindType::TableSRV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableSRV];
						param->TableOffset = res.BindPoint;
						ParamNum[(u32)ShaderBindType::TableSRV]++;
						_SRVs.push_back(param);
					}
				}
				else if ((res.Type == ShaderInputType::UAV_RWTYPED || res.Type == ShaderInputType::UAV_RWSTRUCTURED || res.Type == ShaderInputType::UAV_RWBYTEADDRESS ||
					res.Type == ShaderInputType::UAV_RWSTRUCTURED_WITH_COUNTER || res.Type == ShaderInputType::UAV_FEEDBACKTEXTURE || res.Type == ShaderInputType::UAV_APPEND_STRUCTURED ||
					res.Type == ShaderInputType::UAV_CONSUME_STRUCTURED))
				{
					if (res.BindSpace == RootSignature::cRootDescriptorSpace)
					{
						check(ParamNum[(u32)ShaderBindType::RootUAV] < desc.RootUAVNum);
						param->BindType = ShaderBindType::RootUAV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::RootUAV] + ParamNum[(u32)ShaderBindType::RootUAV];
						param->TableOffset = 0;
						ParamNum[(u32)ShaderBindType::RootUAV]++;
					}
					else
					{
						check(ParamNum[(u32)ShaderBindType::TableUAV] < desc.TableUAVNum);
						param->BindType = ShaderBindType::TableUAV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableUAV];
						param->TableOffset = res.BindPoint;
						ParamNum[(u32)ShaderBindType::TableUAV]++;
						_UAVs.push_back(param);
					}
				}
				else if (res.Type == ShaderInputType::SAMPLER)
				{
					check(ParamNum[(u32)ShaderBindType::TableSampler] < desc.TableSamplerNum);
					param->BindType = ShaderBindType::TableSampler;
					param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableSampler];
					param->TableOffset = res.BindPoint;
					ParamNum[(u32)ShaderBindType::TableSampler]++;
					_Samplers.push_back(param);
				}
			}
		}
	}

}