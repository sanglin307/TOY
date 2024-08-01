#include "Private.h"

void RenderPipeline::BindParameter(const std::string& name, RenderResource* resource)
{
	auto iter = _ShaderParameters.find(name);
	//check(iter != _ShaderParameters.end());
	if (iter == _ShaderParameters.end())
		return;

	for (auto s : *iter->second)
	{
		s->Resource = resource;
	}
}


void RenderPipeline::CommitParameter(RenderContext* ctx)
{
	for (auto param : _RootParams)
	{
		ctx->SetRootDescriptorParameter(param, _Type);
	}

	ctx->SetRootDescriptorTableParameter(_CBVs,_Type);
	ctx->SetRootDescriptorTableParameter(_SRVs,_Type);
	ctx->SetRootDescriptorTableParameter(_UAVs,_Type);
	ctx->SetRootDescriptorTableParameter(_Samplers,_Type);
 
}


void RenderPipeline::AllocateParameters(RootSignature* rs, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders)
{
	_RootSignature = rs;
	check(_ShaderParameters.size() == 0);
	const std::vector<RootSignatureParamDesc>& paramDesc = rs->GetParamDesc();
	const RootSignature::Desc& desc = rs->GetDesc();
	check(paramDesc.size() > 0);

	auto GetRootParamIndex = [&paramDesc](ShaderBindType bindType, ShaderProfile profile, u32 bindPoint) -> u32 {
		for (u32 i = 0; i < paramDesc.size(); i++)
		{
			if (bindType == ShaderBindType::RootConstant || bindType == ShaderBindType::RootCBV || bindType == ShaderBindType::RootSRV || bindType == ShaderBindType::RootUAV)
			{
				if (paramDesc[i].Type == bindType && paramDesc[i].BindPoint == bindPoint)
					return i;
			}
			else
			{
				if (paramDesc[i].Profile == profile && paramDesc[i].Type == bindType)
					return i;
			}
		}

		check(0);
		return 0;
	};

	for (u32 i = 0; i < (u32)ShaderProfile::MAX; i++)
	{
		if (!shaders[i])
			continue;

		ShaderReflection* reflection = shaders[i]->GetReflection();
		if (reflection && reflection->BoundResources.size() > 0)
		{
			for (u32 r = 0; r < reflection->BoundResources.size(); r++)
			{
				ShaderParameter* param = new ShaderParameter;
				const SRBoundResource& res = reflection->BoundResources[r];
				param->Name = res.Name;
				param->Profile = ShaderProfile(i);
				auto iter = _ShaderParameters.find(param->Name);
				if (iter == _ShaderParameters.end())
				{
					std::vector<ShaderParameter*>* pv = new std::vector<ShaderParameter*>;
					pv->push_back(param);
					_ShaderParameters[param->Name] = pv;
				}
				else
				{
					iter->second->push_back(param);
				}
		
				if (res.Type == ShaderInputType::CBUFFER)
				{
					if (res.BindSpace == RootSignature::cRootDescriptorSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.RootCBVNum);
						param->BindType = ShaderBindType::RootCBV;
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::RootCBV, ShaderProfile(i),res.BindPoint);
						param->TableOffset = 0;
						param->DescriptorNum = 1;
						_RootParams.push_back(param);
					}
					else if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableCBVNum);
						param->BindType = ShaderBindType::TableCBV;
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::TableCBV, ShaderProfile(i), res.BindPoint);
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
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::RootSRV, ShaderProfile(i), res.BindPoint);
						param->TableOffset = 0;
						param->DescriptorNum = 1;
						_RootParams.push_back(param);
					}
					else if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableSRVNum);
						param->BindType = ShaderBindType::TableSRV;
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::TableSRV, ShaderProfile(i), res.BindPoint);
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
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::RootUAV, ShaderProfile(i), res.BindPoint);
						param->TableOffset = 0;
						param->DescriptorNum = 1;
						_RootParams.push_back(param);
					}
					else if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableUAVNum);
						param->BindType = ShaderBindType::TableUAV;
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::TableUAV, ShaderProfile(i), res.BindPoint);
						param->TableOffset = res.BindPoint;
						param->DescriptorNum = res.BindCount;
						_UAVs.push_back(param);
					}
				    else
					  check(0);
				}
				else if (res.Type == ShaderInputType::SAMPLER)
				{
					if (res.BindSpace == RootSignature::cDescriptorTableSpace)
					{
						check(res.BindPoint + res.BindCount <= desc.TableSamplerNum);
						param->BindType = ShaderBindType::TableSampler;
						param->RootParamIndex = GetRootParamIndex(ShaderBindType::TableSampler, ShaderProfile(i), res.BindPoint);
						param->TableOffset = res.BindPoint;
						param->DescriptorNum = res.BindCount;
						_Samplers.push_back(param);
					}
				}
			}
		}
	}

}