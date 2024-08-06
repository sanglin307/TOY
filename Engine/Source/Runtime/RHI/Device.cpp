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

void RenderDevice::AddDelayDeleteResource(RenderResource* res, DelayDeleteResourceType type, u64 fenceValue)
{
	_DelayDeleteResources.push_back(DelayDeleteResource{
		.Type = type,
		.FenceValue = fenceValue,
		.Resource = res
		});
}

void RenderDevice::CleanDelayDeleteResource()
{
	u64 frameFence = _ContextManager->GetFrameFenceCompletedValue();
	u64 copyFence = _ContextManager->GetCopyQueueFenceCompletedValue();

	auto iter = _DelayDeleteResources.begin();
	while(iter != _DelayDeleteResources.end())
	{
		if (iter->Type == DelayDeleteResourceType::Frame && iter->FenceValue <= frameFence && frameFence > 0 ||
			iter->Type == DelayDeleteResourceType::CopyQueue && iter->FenceValue <= copyFence && copyFence > 0 ) 
		{
			delete iter->Resource;
			iter = _DelayDeleteResources.erase(iter);
		}
		else
			iter++;
	}
}

RenderDevice::RenderDevice()
{
	_Formats.resize((u32)PixelFormat::MAX);

	for (u32 f = (u32)PixelFormat::R32G32B32A32_TYPELESS; f <= (u32)PixelFormat::R32G32B32A32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 32,
			 .BitSize = 128,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32G32B32_TYPELESS; f <= (u32)PixelFormat::R32G32B32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 32,
			 .BitSize = 96,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R16G16B16A16_TYPELESS; f <= (u32)PixelFormat::R16G16B16A16_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 16,
			 .BitSize = 64,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32G32_TYPELESS; f <= (u32)PixelFormat::R32G32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 32,
			 .BitSize = 64,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32G8X24_TYPELESS; f <= (u32)PixelFormat::X32_TYPELESS_G8X24_UINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 32,
			 .BitSize = 64,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R10G10B10A2_TYPELESS; f <= (u32)PixelFormat::R10G10B10A2_UINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 10,  // largest..
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	_Formats[(u32)PixelFormat::R11G11B10_FLOAT] = PixelFormatInfo{
			 .ComponentBitSize = 11,  // largest..
			 .BitSize = 32,
			 .CompressFormat = false
	};


	for (u32 f = (u32)PixelFormat::R8G8B8A8_TYPELESS; f <= (u32)PixelFormat::R8G8B8A8_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R16G16_TYPELESS; f <= (u32)PixelFormat::R16G16_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 16,
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R32_TYPELESS; f <= (u32)PixelFormat::R32_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 32,
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R24G8_TYPELESS; f <= (u32)PixelFormat::X24_TYPELESS_G8_UINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 24,  // take large one.
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R8G8_TYPELESS; f <= (u32)PixelFormat::R8G8_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 16,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R16_TYPELESS; f <= (u32)PixelFormat::R16_SINT; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 16,
			 .BitSize = 16,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::R8_TYPELESS; f <= (u32)PixelFormat::A8_UNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 8,
			 .CompressFormat = false
		};
	}

	_Formats[(u32)PixelFormat::R1_UNORM] = PixelFormatInfo{
			 .ComponentBitSize = 1,
			 .BitSize = 1,   
			 .CompressFormat = false
	};

	_Formats[(u32)PixelFormat::R9G9B9E5_SHAREDEXP] = PixelFormatInfo{
			 .ComponentBitSize = 14,  // largest..
			 .BitSize = 32,
			 .CompressFormat = false
	};

	for (u32 f = (u32)PixelFormat::R8G8_B8G8_UNORM; f <= (u32)PixelFormat::G8R8_G8B8_UNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::BC1_TYPELESS; f <= (u32)PixelFormat::BC1_UNORM_SRGB; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 6,
			 .BitSize = 4,
			 .CompressFormat = true
		};
	}

	for (u32 f = (u32)PixelFormat::BC2_TYPELESS; f <= (u32)PixelFormat::BC3_UNORM_SRGB; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 6,
			 .BitSize = 8,
			 .CompressFormat = true
		};
	}

	for (u32 f = (u32)PixelFormat::BC4_TYPELESS; f <= (u32)PixelFormat::BC4_SNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 4,
			 .CompressFormat = true
		};
	}

	for (u32 f = (u32)PixelFormat::BC5_TYPELESS; f <= (u32)PixelFormat::BC5_SNORM; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 8,
			 .CompressFormat = true
		};
	}

	_Formats[(u32)PixelFormat::B5G6R5_UNORM] = PixelFormatInfo{
			 .ComponentBitSize = 6,
			 .BitSize = 16,
			 .CompressFormat = false
	};

	_Formats[(u32)PixelFormat::B5G5R5A1_UNORM] = PixelFormatInfo{
			 .ComponentBitSize = 5,
			 .BitSize = 16,
			 .CompressFormat = false
	};

	for (u32 f = (u32)PixelFormat::B8G8R8A8_UNORM; f <= (u32)PixelFormat::B8G8R8X8_UNORM_SRGB; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 8,
			 .BitSize = 32,
			 .CompressFormat = false
		};
	}

	for (u32 f = (u32)PixelFormat::BC6H_TYPELESS; f <= (u32)PixelFormat::BC6H_SF16; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 16,
			 .BitSize = 8,
			 .CompressFormat = true
		};
	}

	for (u32 f = (u32)PixelFormat::BC7_TYPELESS; f <= (u32)PixelFormat::BC7_UNORM_SRGB; f++)
	{
		_Formats[f] = PixelFormatInfo{
			 .ComponentBitSize = 7,
			 .BitSize = 8,
			 .CompressFormat = true
		};
	}
}

u8 RenderDevice::GetPixelComponentBitSize(PixelFormat format)
{
    return _Formats[(u32)format].ComponentBitSize;
}

 

u8 RenderDevice::GetPixelBitSize(PixelFormat format)
{
    return _Formats[(u32)format].BitSize;
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

			offset += GetPixelBitSize(pixel)/8;
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
			if (input.SemanticName == "POSITION")
			{
				PixelFormat pixel = GetInputLayoutPixelFormat(input.ComponentType, input.ComponentMask);
				inputLayout.Desc.push_back(InputLayoutDesc{
					.SemanticName = input.SemanticName,
					.SemanticIndex = input.SemanticIndex,
					.Format = pixel,
					.SlotIndex = 0,
					.SlotOffset = 0,
					.SlotClass = GetSlotClass(input.SemanticName,input.SemanticIndex),
					.InstanceStepRate = 0
					});
			}
			else
			{
				PixelFormat pixel = GetInputLayoutPixelFormat(input.ComponentType, input.ComponentMask);
				inputLayout.Desc.push_back(InputLayoutDesc{
					.SemanticName = input.SemanticName,
					.SemanticIndex = input.SemanticIndex,
					.Format = pixel,
					.SlotIndex = 1,
					.SlotOffset = offset,
					.SlotClass = GetSlotClass(input.SemanticName,input.SemanticIndex),
					.InstanceStepRate = 0
					});

				offset += GetPixelBitSize(pixel) / 8;
			}
		}
		else
			check(0); // todo.
    }

}