#include "Private.h"

void RootSignatureDesc::AddSignature(SignatureType bindType, ShaderInputType inputType, const std::string& name)
{
    // don't bind sampler to root descriptor.
    check(bindType == SignatureType::Direct && inputType != ShaderInputType::SAMPLER);
	_SignatureMap[name] = SignatureElement{
		.BindType = bindType,
		.InputType = inputType,
		.Name = name
	};
}

void RootSignatureDesc::AddSignature(const std::vector<ShaderResource*>& shaders)
{
    for (auto s : shaders)
    {
        ShaderReflection* reflection = s->GetReflection();
        if (reflection && reflection->BoundResources.size() > 0)
        {
            for (SRBoundResource& r : reflection->BoundResources)
            {
                if (_SignatureMap.contains(r.Name))
                    continue;

                _SignatureMap[r.Name] = SignatureElement{
                            .BindType = SignatureType::Indirect,
                            .InputType = r.Type,
                            .Name = r.Name,
                            .BindPoint = r.BindPoint,
                            .BindSpace = r.BindSpace
                };
            }
        }
    }
}

void RootSignatureDesc::RemoveSignature(const std::string& name)
{
	_SignatureMap.erase(name);
}

void RootSignatureDesc::Extract(std::vector<SignatureElement*>& directSignature, std::vector<SignatureElement*>& indirectSignature)
{
	for(auto s : _SignatureMap)
	{
		if (s.second.BindType == SignatureType::Direct)
		{
			directSignature.push_back(&(s.second));
		}
		else
		{
			indirectSignature.push_back(&(s.second));
		}
	}
}