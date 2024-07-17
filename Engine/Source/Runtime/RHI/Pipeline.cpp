#include "Private.h"

void RenderPipeline::BindParameter(const std::string& name, RenderResource* resource)
{
	auto iter = _ShaderParameters.find(name);
	//check(iter != _ShaderParameters.end());
	if (iter == _ShaderParameters.end())
		return;

	iter->second->Resource = resource;
}

void RenderPipeline::ClearUAV(RenderContext* ctx, const std::string& name, const Vector4f& value)
{
	auto iter = _ShaderParameters.find(name);
	if (iter == _ShaderParameters.end())
	{
		check(0);
		return;
	}

	const std::vector<RootSignatureParamDesc>& rsdesc = _RootSignature->GetParamDesc();

	DescriptorAllocation alloc;
	for (u32 i = 0; i < rsdesc.size(); i++)
	{
		if (rsdesc[i].Type == iter->second->BindType)
		{
			alloc = rsdesc[i].Alloc;
			break;
		}
	}

	ctx->ClearUnorderedAccessView(alloc, iter->second->TableOffset, iter->second->Resource, value.f);
}

void RenderPipeline::ClearUAV(RenderContext* ctx, const std::string& name, const Vector4u& value)
{
	auto iter = _ShaderParameters.find(name);
	if (iter == _ShaderParameters.end())
	{
		check(0);
		return;
	}

	const std::vector<RootSignatureParamDesc>& rsdesc = _RootSignature->GetParamDesc();

	DescriptorAllocation alloc;
	for (u32 i = 0; i < rsdesc.size(); i++)
	{
		if (rsdesc[i].Type == iter->second->BindType)
		{
			alloc = rsdesc[i].Alloc;
			break;
		}
	}

	ctx->ClearUnorderedAccessView(alloc, iter->second->TableOffset, iter->second->Resource, value.f);
}

void RenderPipeline::CommitParameter(RenderContext* ctx)
{
	for (auto pair : _ShaderParameters)
	{
		if (!pair.second->Resource)
			continue;

		ShaderBindType t = pair.second->BindType;
		if (t == ShaderBindType::RootCBV || t == ShaderBindType::RootSRV || t == ShaderBindType::RootUAV)
			ctx->SetRootDescriptorParameter(pair.second, _Type);
	}

	ctx->SetRootDescriptorTableParameter(_RootSignature, _CBVs,_Type);
	ctx->SetRootDescriptorTableParameter(_RootSignature, _SRVs,_Type);
	ctx->SetRootDescriptorTableParameter(_RootSignature, _UAVs,_Type);
	ctx->SetRootDescriptorTableParameter(_RootSignature, _Samplers,_Type);

}


void RenderPipeline::AllocateParameters(RootSignature* rs, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders)
{
	_RootSignature = rs;
	check(_ShaderParameters.size() == 0);
	const std::vector<RootSignatureParamDesc>& paramDesc = rs->GetParamDesc();
	const RootSignature::Desc& desc = rs->GetDesc();
	check(paramDesc.size() > 0);

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
						check(res.BindPoint + res.BindCount <= desc.RootCBVNum);
						param->BindType = ShaderBindType::RootCBV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::RootCBV] + res.BindPoint;
						param->TableOffset = 0;
						param->DescriptorNum = 1;
					}
					else if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableCBVNum);
						param->BindType = ShaderBindType::TableCBV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableCBV];
						param->TableOffset = res.BindPoint;
						param->DescriptorNum = res.BindCount;
						_CBVs.push_back(param);
					}
					else
						check(0);
				}
				else if ((res.Type == ShaderInputType::TBUFFER || res.Type == ShaderInputType::TEXTURE || res.Type == ShaderInputType::STRUCTURED ||
					res.Type == ShaderInputType::BYTEADDRESS))
				{
					if (res.BindSpace == RootSignature::cRootDescriptorSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.RootSRVNum);
						param->BindType = ShaderBindType::RootSRV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::RootSRV] + res.BindPoint;
						param->TableOffset = 0;
						param->DescriptorNum = 1;
					}
					else if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableSRVNum);
						param->BindType = ShaderBindType::TableSRV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableSRV];
						param->TableOffset = res.BindPoint;
						param->DescriptorNum = res.BindCount;
						_SRVs.push_back(param);
					}
				}
				else if ((res.Type == ShaderInputType::UAV_RWTYPED || res.Type == ShaderInputType::UAV_RWSTRUCTURED || res.Type == ShaderInputType::UAV_RWBYTEADDRESS ||
					res.Type == ShaderInputType::UAV_RWSTRUCTURED_WITH_COUNTER || res.Type == ShaderInputType::UAV_FEEDBACKTEXTURE || res.Type == ShaderInputType::UAV_APPEND_STRUCTURED ||
					res.Type == ShaderInputType::UAV_CONSUME_STRUCTURED))
				{
					if (res.BindSpace == RootSignature::cRootDescriptorSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.RootUAVNum);
						param->BindType = ShaderBindType::RootUAV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::RootUAV] + res.BindPoint;
						param->TableOffset = 0;
						param->DescriptorNum = 1;
					}
					else if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableUAVNum);
						param->BindType = ShaderBindType::TableUAV;
						param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableUAV];
						param->TableOffset = res.BindPoint;
						param->DescriptorNum = res.BindCount;
						_UAVs.push_back(param);
					}
				    else
					  check(0);
				}
				else if (res.Type == ShaderInputType::SAMPLER)
				{
					check(res.BindPoint + res.BindCount <= desc.TableSamplerNum);
					check(res.BindSpace == RootSignature::cDescriptorTableSpace);
					param->BindType = ShaderBindType::TableSampler;
					param->RootParamIndex = ParamOffset[(u32)ShaderBindType::TableSampler];
					param->TableOffset = res.BindPoint;
					param->DescriptorNum = res.BindCount;
					_Samplers.push_back(param);
				}
			}
		}
	}

}