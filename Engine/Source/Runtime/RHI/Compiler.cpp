#include "Private.h"
 
#ifdef WINDOWS
#define NOMINMAX
#include <wrl/client.h>
using namespace Microsoft::WRL;

#include "dxcapi.h"    // load from thirdparty folder.
#include "d3d12shader.h"

#endif

static std::map<D3D_SHADER_VARIABLE_TYPE, ShaderVariableType> sVariableTypeMap;
struct ShaderVariableTypeMapInitializer
{
    ShaderVariableTypeMapInitializer()
    {
        check(sVariableTypeMap.size() == 0);
        if (sVariableTypeMap.size() == 0)
        {
            sVariableTypeMap.insert(std::pair<D3D_SHADER_VARIABLE_TYPE, ShaderVariableType>(D3D_SVT_VOID, ShaderVariableType::VOID_));
#define VARTYPE(T)  sVariableTypeMap.insert(std::pair<D3D_SHADER_VARIABLE_TYPE,ShaderVariableType>(D3D_SVT_##T,ShaderVariableType::T));
            VARTYPE(BOOL)
            VARTYPE(INT)
            VARTYPE(FLOAT)
            VARTYPE(TEXTURE)
            VARTYPE(TEXTURE1D)
            VARTYPE(TEXTURE2D)
            VARTYPE(TEXTURE3D)
            VARTYPE(TEXTURECUBE)
            VARTYPE(SAMPLER)
            VARTYPE(SAMPLER1D)
            VARTYPE(SAMPLER2D)
            VARTYPE(SAMPLER3D)
            VARTYPE(SAMPLERCUBE)
            VARTYPE(UINT)
            VARTYPE(UINT8)
            VARTYPE(TEXTURE1DARRAY)
            VARTYPE(TEXTURE2DARRAY)
            VARTYPE(TEXTURE2DMS)
            VARTYPE(TEXTURE2DMSARRAY)
            VARTYPE(TEXTURECUBEARRAY)
            VARTYPE(DOUBLE)
            VARTYPE(RWTEXTURE1D)
            VARTYPE(RWTEXTURE1DARRAY)
            VARTYPE(RWTEXTURE2D)
            VARTYPE(RWTEXTURE2DARRAY)
            VARTYPE(RWTEXTURE3D)
            VARTYPE(RWBUFFER)
            VARTYPE(BYTEADDRESS_BUFFER)
            VARTYPE(RWBYTEADDRESS_BUFFER)
            VARTYPE(STRUCTURED_BUFFER)
            VARTYPE(RWSTRUCTURED_BUFFER)
            VARTYPE(APPEND_STRUCTURED_BUFFER)
            VARTYPE(CONSUME_STRUCTURED_BUFFER)
            VARTYPE(MIN8FLOAT)
            VARTYPE(MIN10FLOAT)
            VARTYPE(MIN16FLOAT)
            VARTYPE(MIN12INT)
            VARTYPE(MIN16INT)
            VARTYPE(MIN16UINT)
            VARTYPE(INT16)
            VARTYPE(UINT16)
            VARTYPE(FLOAT16)
            VARTYPE(INT64)
            VARTYPE(UINT64)
#undef VARTYPE
        }
    }
};
ShaderVariableTypeMapInitializer sShaderVariableTypeMapInitializer;

std::wstring GetTargetProfile(ShaderProfile profile)
{
    // use max 6.6
    constexpr int ver = 6;
    switch (profile)
    {
    case ShaderProfile::Vertex:
        return std::format(L"vs_6_{}", ver);
    case ShaderProfile::Pixel:
        return std::format(L"ps_6_{}", ver);
    case ShaderProfile::Compute:
        return std::format(L"cs_6_{}", ver);
    case ShaderProfile::Geometry:
        return std::format(L"gs_6_{}", ver);
    case ShaderProfile::Domain:
        return std::format(L"ds_6_{}", ver);
    case ShaderProfile::Hull:
        return std::format(L"hs_6_{}", ver);
    case ShaderProfile::Mesh:
        return std::format(L"ms_6_{}", ver);
    case ShaderProfile::Amplification:
        return std::format(L"as_6_{}", ver);
    case ShaderProfile::Lib:
        return std::format(L"lib_6_{}", ver);
    }

    check(0);
    return L"";
}

ShaderVariableClass TranslateVariableClass(D3D_SHADER_VARIABLE_CLASS c)
{
    switch (c)
    {
    case D3D_SVC_SCALAR:
        return ShaderVariableClass::SCALAR;
    case D3D_SVC_VECTOR:
        return ShaderVariableClass::VECTOR;
    case D3D_SVC_MATRIX_ROWS:
        return ShaderVariableClass::MATRIX_ROWS;
    case D3D_SVC_MATRIX_COLUMNS:
        return ShaderVariableClass::MATRIX_COLUMNS;
    case D3D_SVC_OBJECT:
        return ShaderVariableClass::OBJECT;
    case D3D_SVC_STRUCT:
        return ShaderVariableClass::STRUCT;
    default:
        check(0);  // todo.
        break;
    }

    return ShaderVariableClass::MAX;
}

ShaderComponentType TranslateComponentType(D3D_REGISTER_COMPONENT_TYPE c)
{
    switch (c)
    {
    case D3D_REGISTER_COMPONENT_UINT32:
        return ShaderComponentType::UINT32;
    case D3D_REGISTER_COMPONENT_SINT32:
        return ShaderComponentType::SINT32;
    case D3D_REGISTER_COMPONENT_FLOAT32:
        return ShaderComponentType::FLOAT32;
    default:
        check(0);  // todo.
        break;
    }

    return ShaderComponentType::MAX;
}

ShaderInputType TranslateShaderInputType(D3D_SHADER_INPUT_TYPE t)
{
    switch (t)
    {
    case D3D_SIT_CBUFFER:
        return ShaderInputType::CBUFFER;
    case D3D_SIT_TBUFFER:
        return ShaderInputType::TBUFFER;
    case D3D_SIT_TEXTURE:
        return ShaderInputType::TEXTURE;
    case D3D_SIT_SAMPLER:
        return ShaderInputType::SAMPLER;
    case D3D_SIT_UAV_RWTYPED:
        return ShaderInputType::UAV_RWTYPED;
    case D3D_SIT_STRUCTURED:
        return ShaderInputType::STRUCTURED;
    case D3D_SIT_UAV_RWSTRUCTURED:
        return ShaderInputType::UAV_RWSTRUCTURED;
    case D3D_SIT_BYTEADDRESS:
        return ShaderInputType::BYTEADDRESS;
    case D3D_SIT_UAV_RWBYTEADDRESS:
        return ShaderInputType::UAV_RWBYTEADDRESS;
    case D3D_SIT_UAV_APPEND_STRUCTURED:
        return ShaderInputType::UAV_APPEND_STRUCTURED;
    case D3D_SIT_UAV_CONSUME_STRUCTURED:
        return ShaderInputType::UAV_CONSUME_STRUCTURED;
    case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
        return ShaderInputType::UAV_RWSTRUCTURED_WITH_COUNTER;
    case D3D_SIT_RTACCELERATIONSTRUCTURE:
        return ShaderInputType::RTACCELERATIONSTRUCTURE;
    case D3D_SIT_UAV_FEEDBACKTEXTURE:
        return ShaderInputType::UAV_FEEDBACKTEXTURE;
    default:
        check(0);  // todo.
        break;
    }

    return ShaderInputType::MAX;
}

ShaderVariableType TranslateVariableType(D3D_SHADER_VARIABLE_TYPE t)
{
    auto iter = sVariableTypeMap.find(t);
    if (iter != sVariableTypeMap.end())
        return iter->second;
    else
    {
        check(0); // not implemented. todo.
    }

    return ShaderVariableType::MAX;
}


ShaderReflection* GenerateReflection(ComPtr<ID3D12ShaderReflection> ref)
{
    ShaderReflection* reflectData = new ShaderReflection;
    D3D12_SHADER_DESC desc;
    ref->GetDesc(&desc);

    reflectData->InstructionCount = desc.InstructionCount;
    reflectData->TempRegisterCount = desc.TempRegisterCount;
    reflectData->StaticFlowControlCount = desc.StaticFlowControlCount;
    reflectData->DynamicFlowControlCount = desc.DynamicFlowControlCount;
    ref->GetThreadGroupSize(&reflectData->ThreadGroupSizeX, &reflectData->ThreadGroupSizeY, &reflectData->ThreadGroupSizeZ);

    // const buffers.
    reflectData->ConstBuffers.reserve(desc.ConstantBuffers);
    for (u32 i = 0; i < desc.ConstantBuffers; i++)
    {
        ID3D12ShaderReflectionConstantBuffer* cb = ref->GetConstantBufferByIndex(i);
        D3D12_SHADER_BUFFER_DESC bufferDesc;
        cb->GetDesc(&bufferDesc);
        reflectData->ConstBuffers.push_back(SRConstBuffer{
            .Name = bufferDesc.Name,
            .Size = bufferDesc.Size
            });

        reflectData->ConstBuffers[i].Variables.reserve(bufferDesc.Variables);
        for (u32 j = 0; j < bufferDesc.Variables; j++)
        {
            ID3D12ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
            D3D12_SHADER_VARIABLE_DESC varDesc;
            var->GetDesc(&varDesc);

            ID3D12ShaderReflectionType* varType = var->GetType();
            D3D12_SHADER_TYPE_DESC typeDesc;
            varType->GetDesc(&typeDesc);
            reflectData->ConstBuffers[i].Variables.push_back(SRVariable{
                .VarName = varDesc.Name,
                .TypeName = typeDesc.Name,
                .Type = TranslateVariableType(typeDesc.Type),
                .Class = TranslateVariableClass(typeDesc.Class),
                .Offset = varDesc.StartOffset,
                .Size = varDesc.Size,
                .TextureOffset = varDesc.StartTexture,
                .TextureSize = varDesc.TextureSize,
                .SamplerOffset = varDesc.StartSampler,
                .SamplerSize = varDesc.SamplerSize
                });

        }
    }

    // bound resources.
    reflectData->BoundResources.reserve(desc.BoundResources);
    for (u32 i = 0; i < desc.BoundResources; i++)
    {
        D3D12_SHADER_INPUT_BIND_DESC desc;
        ref->GetResourceBindingDesc(i, &desc);
        reflectData->BoundResources.push_back(SRBoundResource{
            .Name = desc.Name,
            .Type = TranslateShaderInputType(desc.Type),
            .BindPoint = desc.BindPoint,
            .BindCount = desc.BindCount,
            .BindSpace = desc.Space
            });
    }

    // input parameter.
    reflectData->InputParameter.reserve(desc.InputParameters);
    for (u32 i = 0; i < desc.InputParameters; i++)
    {
        D3D12_SIGNATURE_PARAMETER_DESC desc;
        ref->GetInputParameterDesc(i, &desc);
        reflectData->InputParameter.push_back(SRSignatureParameter{
            .SemanticName = desc.SemanticName,
            .SemanticIndex = desc.SemanticIndex,
            .ComponentType = TranslateComponentType(desc.ComponentType),
            .ComponentMask = desc.Mask
            });
    }

    //output parameter
    reflectData->OutputParameter.reserve(desc.OutputParameters);
    for (u32 i = 0; i < desc.OutputParameters; i++)
    {
        D3D12_SIGNATURE_PARAMETER_DESC desc;
        ref->GetOutputParameterDesc(i, &desc);
        reflectData->OutputParameter.push_back(SRSignatureParameter{
            .SemanticName = desc.SemanticName,
            .SemanticIndex = desc.SemanticIndex,
            .ComponentType = TranslateComponentType(desc.ComponentType),
            .ComponentMask = desc.Mask
            });
    }


    return reflectData;
}

ComPtr<IDxcUtils> _DXCUtils;
ComPtr<IDxcCompiler3> _DXCCompiler;
ComPtr<IDxcIncludeHandler> _DXCIncludeHandler;

class CustomIncludeHandler : public IDxcIncludeHandler
{
public:
    HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
    {
        ComPtr<IDxcBlobEncoding> pEncoding;
        std::filesystem::path path = PathUtil::Shaders() / pFilename;
        if (IncludedFiles.find(path.string()) != IncludedFiles.end())
        {
            // Return empty string blob if this file has been included before
            static const char nullStr[] = " ";
            _DXCUtils->CreateBlobFromPinned(nullStr, ARRAYSIZE(nullStr), DXC_CP_ACP, pEncoding.GetAddressOf());
            *ppIncludeSource = pEncoding.Detach();
            return S_OK;
        }

        HRESULT hr = _DXCUtils->LoadFile(path.wstring().c_str(), nullptr, pEncoding.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            IncludedFiles.insert(path.string());
            *ppIncludeSource = pEncoding.Detach();
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override { return E_NOINTERFACE; }
    ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
    ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

    std::unordered_set<std::string> IncludedFiles;
};

void ShaderCompiler::Init()
{
    check(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_DXCUtils))));
    check(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_DXCCompiler))));

    _DXCUtils->CreateDefaultIncludeHandler(&_DXCIncludeHandler);
}

ShaderResource* ShaderCompiler::CompileHLSL(const ShaderCreateDesc& args)
{
    std::filesystem::path fileNamePath = PathUtil::Shaders() / args.Path;

    constexpr u32 ArgsBufferLength = 256;
    WCHAR argsBuffer[ArgsBufferLength];
    u32 argsOffset = 0;
    std::vector<LPCWSTR> argsParam;

    auto fillArgs = [&argsBuffer, &ArgsBufferLength, &argsOffset, &argsParam](const std::wstring& arg) 
        {
            check(argsOffset + arg.size() < ArgsBufferLength);
            argsParam.push_back(&argsBuffer[argsOffset]);
            arg.copy(&argsBuffer[argsOffset], arg.size());
            argsOffset += (u32)arg.size();
            // \0 process
            if (argsBuffer[argsOffset - 1] != 0)
            {
                argsBuffer[argsOffset++] = 0;
            }
        };

   
    fillArgs(PlatformUtils::UTF8ToUTF16(args.Path));
    fillArgs(L"-E");
    fillArgs(PlatformUtils::UTF8ToUTF16(args.Entry));
    fillArgs(L"-T");
    fillArgs(GetTargetProfile(args.Profile));
    fillArgs(DXC_ARG_DEBUG_NAME_FOR_SOURCE);

    //if (args.Debug)
    {
        fillArgs(DXC_ARG_DEBUG);
        fillArgs(DXC_ARG_SKIP_OPTIMIZATIONS); // disable optimization.
    }

    if (args.Macros.size() > 0)
    {
        for (auto d : args.Macros)
        {
            fillArgs(L"-D");
            fillArgs(PlatformUtils::UTF8ToUTF16(d));
        }
    }

    // load source file.
    ComPtr<IDxcBlobEncoding> dxcSource;
    _DXCUtils->LoadFile(fileNamePath.wstring().c_str(), nullptr, &dxcSource);
    DxcBuffer sourceBuffer = {
         .Ptr = dxcSource->GetBufferPointer(),
         .Size = dxcSource->GetBufferSize(),
         .Encoding = DXC_CP_ACP,
    };

    ComPtr<IDxcResult> dxcResults;
    CustomIncludeHandler includeHandle;
    _DXCCompiler->Compile(&sourceBuffer, argsParam.data(), (u32)argsParam.size(), &includeHandle, IID_PPV_ARGS(&dxcResults));

    ComPtr<IDxcBlobUtf8> dxcErrors;
    dxcResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxcErrors), nullptr);

    std::string errors;
    if (dxcErrors.Get() != nullptr && dxcErrors->GetStringLength() != 0)
    {
        errors = std::string(dxcErrors->GetStringPointer(), dxcErrors->GetStringLength());
    }

    HRESULT hrStatus;
    dxcResults->GetStatus(&hrStatus);
    if (FAILED(hrStatus))
    {
        LOG_ERROR(ShaderCompiler, std::format("Shader {} {} compile error: {}", args.Path, args.Entry, errors));
        return nullptr;
    }

    ShaderResource::Desc desc;
    desc.Profile = args.Profile;
    desc.DebugName = std::format("File({}) Entry({})", args.Path, args.Entry);
    desc.EntryPoint = args.Entry;
    desc.Path = args.Path;

    std::string shaderHash;
    ComPtr<IDxcBlob> dxcHash;
    dxcResults->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&dxcHash), nullptr);
    if (dxcHash != nullptr)
    {
        DxcShaderHash* dxcHashBuf = (DxcShaderHash*)dxcHash->GetBufferPointer();
        for (int i = 0; i < _countof(dxcHashBuf->HashDigest); i++)
        {
            shaderHash += std::format("{:02X}", dxcHashBuf->HashDigest[i]);
        }

        desc.Hash = shaderHash;
    }

    std::filesystem::path path = PathUtil::ShaderOutput();
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }

    std::filesystem::path folder = path / std::format("{}_{}_{}", args.Path,args.Entry, shaderHash);
    if (!std::filesystem::exists(folder))
    {
        std::filesystem::create_directory(folder);
    }

    ComPtr<IDxcBlob> dxcShader;
    ComPtr<IDxcBlobUtf16> dxcShaderName;
    dxcResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&dxcShader), &dxcShaderName);
    if (dxcShader.Get() != nullptr)
    {
        std::filesystem::path binfile = folder / std::format("{}.bin", args.Path);
        std::ofstream fout(binfile, std::ios::binary | std::ios::trunc);
        fout.write((const char*)dxcShader->GetBufferPointer(), dxcShader->GetBufferSize());
        fout.close();
        desc.Blob.Size = dxcShader->GetBufferSize();
        desc.Blob.Data = new u8[desc.Blob.Size];
        std::memcpy(desc.Blob.Data, dxcShader->GetBufferPointer(), desc.Blob.Size);
    }

    ComPtr<IDxcBlob> dxcPDB;
    IDxcBlobUtf16* dxcPDBName;
    dxcResults->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&dxcPDB), &dxcPDBName);
    {
        std::filesystem::path pdbfile = folder / dxcPDBName->GetStringPointer();
        std::ofstream fout(pdbfile, std::ios::binary | std::ios::trunc);
        fout.write((const char*)dxcPDB->GetBufferPointer(), dxcPDB->GetBufferSize());
        fout.close();
    }


    ComPtr<IDxcBlob> dxcReflectionData;
    dxcResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&dxcReflectionData), nullptr);
    if (dxcReflectionData != nullptr)
    {
        std::filesystem::path reffile = folder / std::format("{}.ref", args.Path);
        std::ofstream fout(reffile, std::ios::binary | std::ios::trunc);
        fout.write((const char*)dxcReflectionData->GetBufferPointer(), dxcReflectionData->GetBufferSize());
        fout.close();

        // Create reflection interface.
        DxcBuffer ReflectionData;
        ReflectionData.Encoding = DXC_CP_ACP;
        ReflectionData.Ptr = dxcReflectionData->GetBufferPointer();
        ReflectionData.Size = dxcReflectionData->GetBufferSize();

        ComPtr<ID3D12ShaderReflection> dxcReflection;
        _DXCUtils->CreateReflection(&ReflectionData, IID_PPV_ARGS(&dxcReflection));

        desc.Reflection = GenerateReflection(dxcReflection);
    }

    return new ShaderResource(desc);
}
