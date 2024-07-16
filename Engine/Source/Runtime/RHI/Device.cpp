#include "Private.h"

#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

PixelFormat GetInputLayoutPixelFormat(ShaderComponentType componentType, u8 componentMask)
{
    if (componentType == ShaderComponentType::FLOAT32)
    {
        if (componentMask == 0b1111)
            return PixelFormat::R32G32B32A32_FLOAT;
        else if (componentMask == 0b0111)
            return PixelFormat::R32G32B32_FLOAT;
        else if (componentMask == 0b0011)
            return PixelFormat::R32G32_FLOAT;
        else if (componentMask == 0b0001)
            return PixelFormat::R32_FLOAT;
        else
			check(0);
    }
    else if (componentType == ShaderComponentType::UINT32)
    {
        if (componentMask == 0b1111)
            return PixelFormat::R32G32B32A32_UINT;
        else if (componentMask == 0b0111)
            return PixelFormat::R32G32B32_UINT;
        else if (componentMask == 0b0011)
            return PixelFormat::R32G32_UINT;
        else if (componentMask == 0b0001)
            return PixelFormat::R32_UINT;
        else
			check(0);
    }
    else if (componentType == ShaderComponentType::SINT32)
    {
        if (componentMask == 0b1111)
            return PixelFormat::R32G32B32A32_SINT;
        else if (componentMask == 0b0111)
            return PixelFormat::R32G32B32_SINT;
        else if (componentMask == 0b0011)
            return PixelFormat::R32G32_SINT;
        else if (componentMask == 0b0001)
            return PixelFormat::R32_SINT;
        else
			check(0);
    }
    else
		check(0);

    return PixelFormat::UNKNOWN;
}

RenderDevice::~RenderDevice()
{
}

ShaderResource* RenderDevice::LoadShader(const ShaderCreateDesc& desc)
{
	if (desc.Path != "" && desc.Entry != "")
	{
		u64 sHash = desc.HashResult();
		auto siter = _ShaderCache.find(sHash);
		if (siter != _ShaderCache.end())
		{
			return siter->second;
		}
		else
		{
			ShaderResource* shader = ShaderCompiler::CompileHLSL(desc);
			if (shader != nullptr)
			{
				_ShaderCache[sHash] = shader;
				return shader;
			}
		}
	}

	check(0);
	return nullptr;
}

RenderPipeline* RenderDevice::LoadPipeline(const std::string& name)
{
	auto iter = _PipelineCacheByName.find(name);
	if (iter != _PipelineCacheByName.end())
	{
		return iter->second;
	}

	return nullptr;
}

RenderPipeline* RenderDevice::LoadPipeline(const u64 hash)
{
	auto iter = _PipelineCache.find(hash);
	if (iter != _PipelineCache.end())
	{
		return iter->second;
	}

	return nullptr;
}

VertexAttribute RenderDevice::TranslateSemanticToAttribute(const std::string& semanticName, u32 semanticIndex)
{
	if (semanticName == "POSITION")
		return VertexAttribute::Position;

	if (semanticName == "NORMAL")
		return VertexAttribute::Normal;

	if (semanticName == "TANGENT")
		return VertexAttribute::Tangent;

	if (semanticName == "TEXCOORD")
	{
		return (VertexAttribute)((u32)VertexAttribute::UV0 + semanticIndex);
	}

	if (semanticName == "COLOR")
	{
		return (VertexAttribute)((u32)VertexAttribute::Color0 + semanticIndex);
	}

	check(0);
	return VertexAttribute::Max;
}

void RenderDevice::InitPipelineCache()
{
	std::unordered_map<std::string, GraphicPipeline::Desc> graphicPipelines;
	std::unordered_map<std::string, ComputePipeline::Desc> computePipelines;

	// load all pso from PSO folder.
	for (const auto& file : std::filesystem::directory_iterator(PathUtil::PSO()))
	{
		if (file.path().filename().string().find(".toml") == std::string::npos)
			continue;

		toml::parse_result pso = toml::parse_file(file.path().c_str());
		if (!pso)
		{
			LOG_ERROR(PipelineManager, std::format("parse {} error {}", file.path().string(), pso.error().description()));
			continue;
		}

		toml::table psotable = std::move(pso).table();
		psotable.for_each([&graphicPipelines,&computePipelines,this](const toml::key& key, toml::table& val)
			{
				GraphicPipeline::Desc pd;
				ComputePipeline::Desc cd;

				auto parse_shader = [&val](ShaderCreateDesc& desc, const char* node_name, ShaderProfile profile) -> bool {
					auto s_node = val.get(node_name);
					if (s_node != nullptr)
					{
						check(s_node->is_table());
						const toml::table* vst = s_node->as_table();
						desc.Path = vst->at("Path").value_or("");
						desc.Entry = vst->at("Entry").value_or("");
						desc.Profile = profile;
						const toml::array& ma = *(vst->at("Macros").as_array());
						ma.for_each([&desc](toml::value<std::string> e)
							{
								desc.Macros.push_back(e.get());
							});
						return true;
					}
					return false;
				};

				auto parse_rvts = [&val,this](std::vector<PixelFormat>& rvtFormat, const char* node_name) -> bool {
					auto s_node = val.get(node_name);
					if (s_node != nullptr)
					{
						rvtFormat.clear();
						check(s_node->is_array());
						const toml::array& va = *(s_node->as_array());
						va.for_each([&rvtFormat,this](toml::value<std::string> e)
							{
								rvtFormat.push_back(this->GetFormatByName(e.get()));
							});
						return true;
					}
					return false;
					};

				std::string name = key.data();
				if (parse_shader(pd.VS, "VS", ShaderProfile::Vertex))
				{
					parse_shader(pd.PS, "PS", ShaderProfile::Pixel);
					parse_rvts(pd.RVTFormats, "RVTFormats");
					graphicPipelines[name] = pd;
				}

				if (parse_shader(cd.CS, "CS", ShaderProfile::Compute))
				{
					computePipelines[name] = cd;
				}
			});
	}

	if (graphicPipelines.size() > 0)
	{
		for (auto& gp : graphicPipelines)
		{
			CreateGraphicPipeline(gp.first,gp.second);
		}
	}

	if (computePipelines.size() > 0)
	{
		for (auto& cp : computePipelines)
		{
			CreateComputePipeline(cp.first, cp.second);
		}
	}
}

void RenderDevice::AddDelayDeleteResource(RenderResource* res, u64 copyFenceValue)
{
	_DelayDeleteResources.push_back(DelayDeleteResource{
		.FrameNum = _FrameNum,
		.CopyFenceValue = copyFenceValue,
		.Resource = res
		});
}

void RenderDevice::CleanDelayDeleteResource()
{
	std::list<DelayDeleteResource> needDeleteRes;
	auto iter = _DelayDeleteResources.begin();
	u64 maxCopyFenceValue = 0;
	while(iter != _DelayDeleteResources.end())
	{
		if (_FrameNum > iter->FrameNum + 1)  // delay 1 frame
		{
			if (iter->CopyFenceValue > maxCopyFenceValue)
				maxCopyFenceValue = iter->CopyFenceValue;

			needDeleteRes.push_back(*iter);
			auto diter = iter++;
			_DelayDeleteResources.erase(diter);
		}
		else
			iter++;
	}

	if (needDeleteRes.size() > 0)
	{
		_ContextManager->CpuWaitCopyFinish(maxCopyFenceValue);
		for (auto res : needDeleteRes)
		{
			delete res.Resource;
		}
	}
}

RenderDevice::RenderDevice()
{
	_Formats.resize((u32)PixelFormat::MAX);

	for (u32 f = (u32)PixelFormat::R32G32B32A32_TYPELESS; f <= (u32)PixelFormat::R32G32B32A32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 4,
			 .ByteSize = 16,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32G32B32_TYPELESS; f <= (u32)PixelFormat::R32G32B32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 3,
			 .ComponentSize = 4,
			 .ByteSize = 12,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R16G16B16A16_TYPELESS; f <= (u32)PixelFormat::R16G16B16A16_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 2,
			 .ByteSize = 8,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32G32_TYPELESS; f <= (u32)PixelFormat::R32G32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 2,
			 .ComponentSize = 4,
			 .ByteSize = 8,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32G8X24_TYPELESS; f <= (u32)PixelFormat::X32_TYPELESS_G8X24_UINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 2,
			 .ComponentSize = 4,
			 .ByteSize = 8,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R10G10B10A2_TYPELESS; f <= (u32)PixelFormat::R11G11B10_FLOAT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 1,  // average..
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	_Formats[(u32)PixelFormat::R11G11B10_FLOAT] = PixelFormatInfo{
			 .ComponentNum = 3,
			 .ComponentSize = 1,  // average..
			 .ByteSize = 4,
			 .CompressFormat = false
	};


	for (u32 f = (u32)PixelFormat::R8G8B8A8_TYPELESS; f <= (u32)PixelFormat::R8G8B8A8_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 1,
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R16G16_TYPELESS; f <= (u32)PixelFormat::R16G16_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 2,
			 .ComponentSize = 2,
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32_TYPELESS; f <= (u32)PixelFormat::R32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 1,
			 .ComponentSize = 4,
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R24G8_TYPELESS; f <= (u32)PixelFormat::X24_TYPELESS_G8_UINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 2,
			 .ComponentSize = 3,  // take large one.
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R8G8_TYPELESS; f <= (u32)PixelFormat::R8G8_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 2,
			 .ComponentSize = 1,
			 .ByteSize = 2,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R16_TYPELESS; f <= (u32)PixelFormat::R16_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 1,
			 .ComponentSize = 2,
			 .ByteSize = 2,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R8_TYPELESS; f <= (u32)PixelFormat::A8_UNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 1,
			 .ComponentSize = 1,
			 .ByteSize = 1,
			 .CompressFormat = false
		};
	}

	_Formats[(u32)PixelFormat::R1_UNORM] = PixelFormatInfo{
			 .ComponentNum = 1,
			 .ComponentSize = 0,
			 .ByteSize = 0,  // need specific calculate when using.
			 .CompressFormat = false
	};

	for (u32 f = (u32)PixelFormat::R9G9B9E5_SHAREDEXP; f <= (u32)PixelFormat::G8R8_G8B8_UNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 1,
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::BC1_TYPELESS; f <= (u32)PixelFormat::BC5_SNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 0,
			 .ComponentSize = 0,
			 .ByteSize = 0,
			 .CompressFormat = true
		};
	}

	_Formats[(u32)PixelFormat::B5G6R5_UNORM] = PixelFormatInfo{
			 .ComponentNum = 3,
			 .ComponentSize = 1,
			 .ByteSize = 2,
			 .CompressFormat = false
	};

	_Formats[(u32)PixelFormat::B5G5R5A1_UNORM] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 1,
			 .ByteSize = 2,
			 .CompressFormat = false
	};

	for (u32 f = (u32)PixelFormat::B8G8R8A8_UNORM; f <= (u32)PixelFormat::B8G8R8X8_UNORM_SRGB; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 4,
			 .ComponentSize = 1,
			 .ByteSize = 4,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::BC6H_TYPELESS; f <= (u32)PixelFormat::BC7_UNORM_SRGB; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentNum = 0,
			 .ComponentSize = 0,
			 .ByteSize = 0,
			 .CompressFormat = true
		};
	}
}

u8 RenderDevice::GetPixelComponentSize(PixelFormat format)
{
    return _Formats[(u32)format].ComponentSize;
}

u8 RenderDevice::GetPixelComponentNum(PixelFormat format)
{
    return _Formats[(u32)format].ComponentNum;
}

u8 RenderDevice::GetPixelSize(PixelFormat format)
{
    return _Formats[(u32)format].ByteSize;
}

bool RenderDevice::IsCompressedPixelFormat(PixelFormat format)
{
    return _Formats[(u32)format].CompressFormat;
}

bool RenderDevice::IsDepthPixelFormat(PixelFormat format)
{
	if (format == PixelFormat::D32_FLOAT_S8X24_UINT ||
		format == PixelFormat::D32_FLOAT ||
		format == PixelFormat::D24_UNORM_S8_UINT ||
		format == PixelFormat::D16_UNORM)
	{
		return true;
	}

	return false;
}

bool RenderDevice::IsStencilPixelFormat(PixelFormat format)
{
	if (format == PixelFormat::D32_FLOAT_S8X24_UINT ||
		format == PixelFormat::D24_UNORM_S8_UINT)
	{
		return true;
	}

	return false;
}

PixelFormat RenderDevice::GetDepthResourceFormat(PixelFormat format)
{
	if (format == PixelFormat::D32_FLOAT_S8X24_UINT)
		return PixelFormat::R32G8X24_TYPELESS;
	else if (format == PixelFormat::D32_FLOAT)
		return PixelFormat::R32_TYPELESS;
	else if (format == PixelFormat::D24_UNORM_S8_UINT)
		return PixelFormat::R24G8_TYPELESS;
	else if (format == PixelFormat::D16_UNORM)
		return PixelFormat::R16_TYPELESS;
	else
		check(0);

	return PixelFormat::UNKNOWN;
}

PixelFormat RenderDevice::GetFormatByName(const std::string& name)
{
	auto iter = _FormatNameMap.find(name);
	if (iter != _FormatNameMap.end())
		return iter->second;

	return PixelFormat::UNKNOWN;
}

PixelFormat RenderDevice::GetDepthShaderResourceFormat(PixelFormat format, bool useAsDepth)
{
	if (format == PixelFormat::D32_FLOAT_S8X24_UINT)
	{
		if (useAsDepth)
			return PixelFormat::R32_FLOAT_X8X24_TYPELESS;
		else
			return PixelFormat::X32_TYPELESS_G8X24_UINT;
	}
	else if (format == PixelFormat::D32_FLOAT)
	{
		return PixelFormat::R32_FLOAT;
	}
	else if (format == PixelFormat::D24_UNORM_S8_UINT)
	{
		if (useAsDepth)
			return PixelFormat::R24_UNORM_X8_TYPELESS;
		else
			return PixelFormat::X24_TYPELESS_G8_UINT;
	}
	else if (format == PixelFormat::D16_UNORM)
	{
		return PixelFormat::R16_UNORM;
	}
	else
		check(0);

	return PixelFormat::UNKNOWN;
}

InputSlotClass GetSlotClass(const std::string& semanticName,u32 semanticIndex)
{
    // if(semanticName == "xxxx")
    //     return InputSlotClass::PerInstance;

    return InputSlotClass::PerVertex;
}

 
void RenderDevice::CreateInputLayout(const ShaderResource* shader, InputSlotMapping slotMapping, InputLayout& inputLayout)
{
    check(shader->GetProfile() == ShaderProfile::Vertex);

    const ShaderReflection* reflection = shader->GetReflection();
    if (!reflection || reflection->InputParameter.size() == 0)
    {
        LOG_ERROR(RenderDevice, std::format("vertex shader don't contain correct reflection information ({})", shader->GetDebugName()));
        return;
    }

    u32 offset = 0;
    for (const SRSignatureParameter& input : reflection->InputParameter)
    {
		if (slotMapping == InputSlotMapping::Interleaved)
		{
			PixelFormat pixel = GetInputLayoutPixelFormat(input.ComponentType, input.ComponentMask);
			inputLayout.Desc.push_back(InputLayoutDesc{
				.SemanticName = input.SemanticName,
				.SemanticIndex = input.SemanticIndex,
				.Format = pixel,
				.SlotIndex = 0,
				.SlotOffset = offset,
				.SlotClass = GetSlotClass(input.SemanticName,input.SemanticIndex),
				.InstanceStepRate = 0
				});

			offset += GetPixelSize(pixel);
		}
		else if (slotMapping == InputSlotMapping::Seperated)
		{
			PixelFormat pixel = GetInputLayoutPixelFormat(input.ComponentType, input.ComponentMask);
			inputLayout.Desc.push_back(InputLayoutDesc{
				.SemanticName = input.SemanticName,
				.SemanticIndex = input.SemanticIndex,
				.Format = pixel,
				.SlotIndex = offset,
				.SlotOffset = 0,
				.SlotClass = GetSlotClass(input.SemanticName,input.SemanticIndex),
				.InstanceStepRate = 0
				});

			offset++;
		}
		else if (slotMapping == InputSlotMapping::Custom)
		{
			check(0); // todo.
		}
		else
			check(0); // todo.
    }

}