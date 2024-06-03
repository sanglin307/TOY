#include "Private.h"
 

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

	return nullptr;
}

GraphicPipeline* RenderDevice::LoadGraphicPipeline(const GraphicPipeline::Desc& desc)
{
	u64 hash = desc.HashResult();
	auto iter = _PipelineCache.find(hash);
	if (iter != _PipelineCache.end())
	{
		return iter->second;
	}

	return nullptr;
}

void RenderDevice::InitPipelineCache()
{
	std::vector<GraphicPipeline::Desc> graphicPipelines;

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
		psotable.for_each([&graphicPipelines](const toml::key& key, toml::table& val)
			{
				GraphicPipeline::Desc pd;
				pd.Name = key.str();
				auto parse_shader = [&val](ShaderCreateDesc& desc, const char* node_name, ShaderProfile profile) {
					auto s_node = val.get(node_name);
					if (s_node != nullptr)
					{
						check(s_node->is_table());
						const toml::table* vst = s_node->as_table();
						desc.Path = vst->at("path").value_or("");
						desc.Entry = vst->at("entry").value_or("");
						desc.Profile = profile;
						const toml::array& ma = *(vst->at("macro").as_array());
						ma.for_each([&desc](toml::value<std::string> e)
							{
								desc.Macros.push_back(e.get());
							});
					}
					};
				parse_shader(pd.VS, "vs", ShaderProfile::Vertex);
				parse_shader(pd.PS, "ps", ShaderProfile::Pixel);

				graphicPipelines.push_back(pd);
			});
	}

	if (graphicPipelines.size() > 0)
	{
		for (auto& gp : graphicPipelines)
		{
			CreateGraphicPipeline(gp);
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

InputSlotClass GetSlotClass(const std::string& semanticName,u32 semanticIndex)
{
    // if(semanticName == "xxxx")
    //     return InputSlotClass::PerInstance;

    return InputSlotClass::PerVertex;
}

void RenderDevice::CreateInputLayout(const std::vector<ShaderResource*>& shaders, InputSlotMapping slotMapping, std::vector<InputLayoutDesc>& inputLayout)
{
    auto iter = std::find_if(shaders.begin(), shaders.end(), [](ShaderResource* s) -> bool { return s->GetProfile() == ShaderProfile::Vertex; });
    if (iter == shaders.end())
    {
        LOG_ERROR(RenderDevice, "No vertex shader found to fill inputLayout!");
        return;
    }

    ShaderReflection* reflection = (*iter)->GetReflection();
    if (!reflection || reflection->InputParameter.size() == 0)
    {
        LOG_ERROR(RenderDevice, std::format("vertex shader don't contain correct reflection information ({})", (*iter)->GetDebugName()));
        return;
    }

    u32 offset = 0;
    for (const SRSignatureParameter& input : reflection->InputParameter)
    {
		if (slotMapping == InputSlotMapping::Interleaved)
		{
			PixelFormat pixel = GetInputLayoutPixelFormat(input.ComponentType, input.ComponentMask);
			inputLayout.push_back(InputLayoutDesc{
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
			inputLayout.push_back(InputLayoutDesc{
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