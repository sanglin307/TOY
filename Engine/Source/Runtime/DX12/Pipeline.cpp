#include "Private.h"

DX12RootSignature::DX12RootSignature(DX12Device* device)
{
	std::vector<D3D12_ROOT_PARAMETER1> param;

	RootParameterNumber[(u32)ShaderBindType::RootCBV] = 4;
	RootParameterNumber[(u32)ShaderBindType::RootSRV] = 6;
	RootParameterNumber[(u32)ShaderBindType::RootUAV] = cRootDescriptorsNumberMax - RootParameterNumber[(u32)ShaderBindType::RootCBV] - RootParameterNumber[(u32)ShaderBindType::RootSRV];

	RootParameterNumber[(u32)ShaderBindType::TableOneCBV] = 4;
	RootParameterNumber[(u32)ShaderBindType::TableOneSRV] = 12;
	RootParameterNumber[(u32)ShaderBindType::TableOneUAV] = 8;
	RootParameterNumber[(u32)ShaderBindType::TableOneSampler] = cRootTableOneParameterNumberMax - RootParameterNumber[(u32)ShaderBindType::TableOneCBV]
		                                                        - RootParameterNumber[(u32)ShaderBindType::TableOneSRV] - RootParameterNumber[(u32)ShaderBindType::TableOneUAV];

	RootParameterNumber[(u32)ShaderBindType::TableMultipleCBV] = 2;
	RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV] = 2;
	RootParameterNumber[(u32)ShaderBindType::TableMultipleUAV] = 2;
	RootParameterNumber[(u32)ShaderBindType::TableMultipleSampler] = cRootTableMultipleParameterNumberMax - RootParameterNumber[(u32)ShaderBindType::TableMultipleCBV]
		                                                             - RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV] - RootParameterNumber[(u32)ShaderBindType::TableMultipleUAV];

	RootParameterOffset[(u32)ShaderBindType::RootCBV] = 0;
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::RootCBV]; i++)
	{
		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
			.Descriptor = D3D12_ROOT_DESCRIPTOR1 {
				.ShaderRegister = i,
				.RegisterSpace = 0,
				.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
		});
	}

	RootParameterOffset[(u32)ShaderBindType::RootSRV] = (u32)param.size();
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::RootSRV]; i++)
	{
		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV,
			.Descriptor = D3D12_ROOT_DESCRIPTOR1 {
				.ShaderRegister = i,
				.RegisterSpace = 0,
				.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::RootUAV] = (u32)param.size();
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::RootUAV]; i++)
	{
		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV,
			.Descriptor = D3D12_ROOT_DESCRIPTOR1 {
				.ShaderRegister = i,
				.RegisterSpace = 0,
				.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableOneCBV] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> cbvRange(RootParameterNumber[(u32)ShaderBindType::TableOneCBV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableOneCBV]; i++)
	{
		cbvRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			.NumDescriptors = 1,
			.BaseShaderRegister = RootParameterNumber[(u32)ShaderBindType::RootCBV] + i,
			.RegisterSpace = 0,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &cbvRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableOneSRV] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> srvRange(RootParameterNumber[(u32)ShaderBindType::TableOneSRV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableOneSRV]; i++)
	{
		srvRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			.NumDescriptors = 1,
			.BaseShaderRegister = RootParameterNumber[(u32)ShaderBindType::RootSRV] + i,
			.RegisterSpace = 0,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &srvRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableOneUAV] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> uavRange(RootParameterNumber[(u32)ShaderBindType::TableOneUAV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableOneUAV]; i++)
	{
		uavRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
			.NumDescriptors = 1,
			.BaseShaderRegister = RootParameterNumber[(u32)ShaderBindType::RootUAV] + i,
			.RegisterSpace = 0,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &uavRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableOneSampler] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> samplerRange(RootParameterNumber[(u32)ShaderBindType::TableOneSampler]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableOneSampler]; i++)
	{
		samplerRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
			.NumDescriptors = 1,
			.BaseShaderRegister = i,
			.RegisterSpace = 0,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &samplerRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableMultipleCBV] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> cbvTableRange(RootParameterNumber[(u32)ShaderBindType::TableMultipleCBV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableMultipleCBV]; i++)
	{
		cbvTableRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			.NumDescriptors = cRootTableDescriptorSizeMax,
			.BaseShaderRegister = i * cRootTableDescriptorSizeMax + RootParameterNumber[(u32)ShaderBindType::RootCBV] + RootParameterNumber[(u32)ShaderBindType::TableOneCBV],
			.RegisterSpace = 1,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &cbvTableRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}
	
	RootParameterOffset[(u32)ShaderBindType::TableMultipleSRV] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> srvTableRange(RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV]; i++)
	{
		srvTableRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			.NumDescriptors = cRootTableDescriptorSizeMax,
			.BaseShaderRegister = i * cRootTableDescriptorSizeMax + RootParameterNumber[(u32)ShaderBindType::RootSRV] + RootParameterNumber[(u32)ShaderBindType::TableOneSRV],
			.RegisterSpace = 1,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &srvTableRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableMultipleUAV] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> uavTableRange(RootParameterNumber[(u32)ShaderBindType::TableMultipleUAV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableMultipleUAV]; i++)
	{
		uavTableRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
			.NumDescriptors = cRootTableDescriptorSizeMax,
			.BaseShaderRegister = i * cRootTableDescriptorSizeMax + RootParameterNumber[(u32)ShaderBindType::RootUAV] + RootParameterNumber[(u32)ShaderBindType::TableOneUAV],
			.RegisterSpace = 1,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &uavTableRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	RootParameterOffset[(u32)ShaderBindType::TableMultipleSampler] = (u32)param.size();
	std::vector<D3D12_DESCRIPTOR_RANGE1> samplerTableRange(RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV]);
	for (u32 i = 0; i < RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV]; i++)
	{
		samplerTableRange[i] = D3D12_DESCRIPTOR_RANGE1{
			.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
			.NumDescriptors = cRootTableDescriptorSizeMax,
			.BaseShaderRegister = i * cRootTableDescriptorSizeMax + RootParameterNumber[(u32)ShaderBindType::TableOneSampler],
			.RegisterSpace = 1,
			.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		};

		param.push_back(D3D12_ROOT_PARAMETER1{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable = D3D12_ROOT_DESCRIPTOR_TABLE1 {
				.NumDescriptorRanges = 1,
				.pDescriptorRanges = &samplerTableRange[i]
				},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
			});
	}

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {
		.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
		.Desc_1_1 = {
		   .NumParameters = (u32)param.size(),
		   .pParameters = param.data(),
		   .NumStaticSamplers = 0,
		   .pStaticSamplers = nullptr,
		   .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
					D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED,
		}
	};

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	if (FAILED(D3D12SerializeVersionedRootSignature(&rsDesc, signature.GetAddressOf(), error.GetAddressOf())))
	{
		std::string se((const char*)error->GetBufferPointer(), error->GetBufferSize());
		LOG_ERROR(RootSignature, std::format("D3D12SerializeVersionedRootSignature Failed:{}", se));
		check(0);
	}

	_Handle = device->CreateRootSignature(signature);
}

ShaderParameter* DX12RootSignature::Allocate(const SRBoundResource& r)
{
	auto iter = _ParamsMap.find(r.Name);
	if (iter != _ParamsMap.end())
		return iter->second;

	ShaderParameter* param = new ShaderParameter;
	param->Name = r.Name;

	if (r.Type == ShaderInputType::CBUFFER)
	{
		if (r.BindCount > 1) // multple descriptor table
		{
			check(r.BindCount < cRootTableDescriptorSizeMax);
			if (RootParameterUsed[(u32)ShaderBindType::TableMultipleCBV] < RootParameterNumber[(u32)ShaderBindType::TableMultipleCBV])
			{
				param->BindType = ShaderBindType::TableMultipleCBV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableMultipleCBV] + RootParameterUsed[(u32)ShaderBindType::TableMultipleCBV]++;
			}
			else
			{
				check(0); // don't have enough space , adjust setting.
			}
		}
		else
		{
			if (RootParameterUsed[(u32)ShaderBindType::RootCBV] < RootParameterNumber[(u32)ShaderBindType::RootCBV])
			{
				param->BindType = ShaderBindType::RootCBV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::RootCBV] + RootParameterUsed[(u32)ShaderBindType::RootCBV]++;
			}
			else if (RootParameterUsed[(u32)ShaderBindType::TableOneCBV] < RootParameterNumber[(u32)ShaderBindType::TableOneCBV])
			{
				param->BindType = ShaderBindType::TableOneCBV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableOneCBV] + RootParameterUsed[(u32)ShaderBindType::TableOneCBV]++;
			}
		}
	}
	else if ((r.Type == ShaderInputType::TBUFFER || r.Type == ShaderInputType::TEXTURE || r.Type == ShaderInputType::STRUCTURED ||
		r.Type == ShaderInputType::BYTEADDRESS))
	{
		if (r.BindCount > 1) // multple descriptor table
		{
			check(r.BindCount < cRootTableDescriptorSizeMax);
			if (RootParameterUsed[(u32)ShaderBindType::TableMultipleSRV] < RootParameterNumber[(u32)ShaderBindType::TableMultipleSRV])
			{
				param->BindType = ShaderBindType::TableMultipleSRV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableMultipleSRV] + RootParameterUsed[(u32)ShaderBindType::TableMultipleSRV]++;
			}
			else
			{
				check(0); // don't have enough space , adjust setting.
			}
		}
		else
		{
			if (RootParameterUsed[(u32)ShaderBindType::RootSRV] < RootParameterNumber[(u32)ShaderBindType::RootSRV])
			{
				param->BindType = ShaderBindType::RootSRV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::RootSRV] + RootParameterUsed[(u32)ShaderBindType::RootSRV]++;
			}
			else if (RootParameterUsed[(u32)ShaderBindType::TableOneSRV] < RootParameterNumber[(u32)ShaderBindType::TableOneSRV])
			{
				param->BindType = ShaderBindType::TableOneSRV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableOneSRV] + RootParameterUsed[(u32)ShaderBindType::TableOneSRV]++;
			}
		}
	}
	else if ((r.Type == ShaderInputType::UAV_RWTYPED || r.Type == ShaderInputType::UAV_RWSTRUCTURED || r.Type == ShaderInputType::UAV_RWBYTEADDRESS ||
		r.Type == ShaderInputType::UAV_RWSTRUCTURED_WITH_COUNTER || r.Type == ShaderInputType::UAV_FEEDBACKTEXTURE || r.Type == ShaderInputType::UAV_APPEND_STRUCTURED ||
		r.Type == ShaderInputType::UAV_CONSUME_STRUCTURED))
	{
		if (r.BindCount > 1) // multple descriptor table
		{
			check(r.BindCount < cRootTableDescriptorSizeMax);
			if (RootParameterUsed[(u32)ShaderBindType::TableMultipleUAV] < RootParameterNumber[(u32)ShaderBindType::TableMultipleUAV])
			{
				param->BindType = ShaderBindType::TableMultipleUAV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableMultipleUAV] + RootParameterUsed[(u32)ShaderBindType::TableMultipleUAV]++;
			}
			else
			{
				check(0); // don't have enough space , adjust setting.
			}
		}
		else
		{
			if (RootParameterUsed[(u32)ShaderBindType::RootUAV] < RootParameterNumber[(u32)ShaderBindType::RootUAV])
			{
				param->BindType = ShaderBindType::RootUAV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::RootUAV] + RootParameterUsed[(u32)ShaderBindType::RootUAV]++;
			}
			else if (RootParameterUsed[(u32)ShaderBindType::TableOneUAV] < RootParameterNumber[(u32)ShaderBindType::TableOneUAV])
			{
				param->BindType = ShaderBindType::TableOneUAV;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableOneUAV] + RootParameterUsed[(u32)ShaderBindType::TableOneUAV]++;
			}
		}
	}
	else if (r.Type == ShaderInputType::SAMPLER)
	{
		if (r.BindCount > 1) // multple descriptor table
		{
			check(r.BindCount < cRootTableDescriptorSizeMax);
			if (RootParameterUsed[(u32)ShaderBindType::TableMultipleSampler] < RootParameterNumber[(u32)ShaderBindType::TableMultipleSampler])
			{
				param->BindType = ShaderBindType::TableMultipleSampler;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableMultipleSampler] + RootParameterUsed[(u32)ShaderBindType::TableMultipleSampler]++;
			}
			else
			{
				check(0); // don't have enough space , adjust setting.
			}
		}
		else
		{
			if (RootParameterUsed[(u32)ShaderBindType::TableOneSampler] < RootParameterNumber[(u32)ShaderBindType::TableOneSampler])
			{
				param->BindType = ShaderBindType::TableOneSampler;
				param->RootParamIndex = RootParameterOffset[(u32)ShaderBindType::TableOneSampler] + RootParameterUsed[(u32)ShaderBindType::TableOneSampler]++;
			}
		}
	}
	else
	{
		check(0);
	}

	param->Resource = nullptr; // always null, we use it in pso.

	return param;
}