#pragma once

class Material
{
public:
	static Material* Create(const std::array<ShaderResource*, (size_t)ShaderProfile::MAX>& shaders);
	virtual ~Material();
private:
	std::array<ShaderResource*, (size_t)ShaderProfile::MAX> _Shaders;
};