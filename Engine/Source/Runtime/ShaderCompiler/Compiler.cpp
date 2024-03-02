#include "Private.h"
 
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

    assert(0);
    return L"";
}

bool ShaderCompiler::CompileHLSL(const CompileArgs& args)
{
    ComPtr<IDxcUtils> _DXCUtils;
    ComPtr<IDxcCompiler3> _DXCCompiler;
    ComPtr<IDxcIncludeHandler> _DXCIncludeHandler;

    assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_DXCUtils))));
    assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_DXCCompiler))));

    _DXCUtils->CreateDefaultIncludeHandler(&_DXCIncludeHandler);


    std::filesystem::path fileNamePath = PathUtil::Shaders() / args.FileName;

    constexpr u32 ArgsBufferLength = 256;
    WCHAR argsBuffer[ArgsBufferLength];
    u32 argsOffset = 0;
    std::vector<LPCWSTR> argsParam;

    auto fillArgs = [&argsBuffer, &ArgsBufferLength, &argsOffset, &argsParam](const std::wstring& arg) 
        {
            assert(argsOffset + arg.size() < ArgsBufferLength);
            argsParam.push_back(&argsBuffer[argsOffset]);
            arg.copy(&argsBuffer[argsOffset], arg.size());
            argsOffset += (u32)arg.size();
            // \0 process
            if (argsBuffer[argsOffset - 1] != 0)
            {
                argsBuffer[argsOffset++] = 0;
            }
        };

   
    fillArgs(PlatformUtils::UTF8ToUTF16(args.FileName));
    fillArgs(L"-E");
    fillArgs(PlatformUtils::UTF8ToUTF16(args.EntryName));
    fillArgs(L"-T");
    fillArgs(GetTargetProfile(args.Profile));
    fillArgs(DXC_ARG_DEBUG_NAME_FOR_SOURCE);

    if (args.Debug)
    {
        fillArgs(DXC_ARG_DEBUG);
        fillArgs(DXC_ARG_SKIP_OPTIMIZATIONS); // disable optimization.
    }

    if (args.Defines.size() > 0)
    {
        for (auto d : args.Defines)
        {
            fillArgs(L"-D");
            fillArgs(PlatformUtils::UTF8ToUTF16(d));
        }
    }

    // load source file.
    ComPtr<IDxcBlobEncoding> dxcSource;
    assert(SUCCEEDED(_DXCUtils->LoadFile(fileNamePath.wstring().c_str(), nullptr, &dxcSource)));
    DxcBuffer sourceBuffer = {
         .Ptr = dxcSource->GetBufferPointer(),
         .Size = dxcSource->GetBufferSize(),
         .Encoding = DXC_CP_ACP,
    };

    ComPtr<IDxcResult> dxcResults;
    assert(SUCCEEDED(_DXCCompiler->Compile(&sourceBuffer, argsParam.data(), (u32)argsParam.size(), _DXCIncludeHandler.Get(), IID_PPV_ARGS(&dxcResults))));

    ComPtr<IDxcBlobUtf8> dxcErrors;
    dxcResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxcErrors), nullptr);

    if (dxcErrors.Get() != nullptr && dxcErrors->GetStringLength() != 0)
    {
        std::string errors(dxcErrors->GetStringPointer(), dxcErrors->GetStringLength());
        TOY_Warning(ShaderCompile, std::format("Shader {} compile warning & error : {}", args.FileName, errors));
    }

    HRESULT hrStatus;
    dxcResults->GetStatus(&hrStatus);
    if (FAILED(hrStatus))
    {
        TOY_Error(ShaderCompile, std::format("Shader {} compile failed", args.FileName));
        return false;
    }

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
    }

    std::filesystem::path path = PathUtil::ShaderOutput();
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }

    std::filesystem::path folder = path / std::format("{}_{}", args.FileName, shaderHash);
    if (!std::filesystem::exists(folder))
    {
        std::filesystem::create_directory(folder);
    }

    ComPtr<IDxcBlob> dxcShader;
    ComPtr<IDxcBlobUtf16> dxcShaderName;
    dxcResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&dxcShader), &dxcShaderName);
    if (dxcShader.Get() != nullptr)
    {
        std::filesystem::path binfile = folder / std::format("{}.bin", args.FileName);
        std::ofstream fout(binfile, std::ios::binary | std::ios::trunc);
        fout.write((const char*)dxcShader->GetBufferPointer(), dxcShader->GetBufferSize());
        fout.close();
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
        std::filesystem::path reffile = folder / std::format("{}.ref", args.FileName);
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
    }

    ComPtr<IDxcBlob> dxcRootSigData;
    dxcResults->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(&dxcRootSigData), nullptr);
    if (dxcRootSigData != nullptr)
    {
        DxcBuffer RSData = {
            .Ptr = dxcRootSigData->GetBufferPointer(),
            .Size = dxcRootSigData->GetBufferSize(),
            .Encoding = DXC_CP_ACP
        };

    }

    return true;
}