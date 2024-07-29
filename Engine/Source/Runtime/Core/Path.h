#pragma once

class CORE_API PathUtil
{
public:
	static const std::filesystem::path& Config();
	static const std::filesystem::path& Bin();
	static const std::filesystem::path& Logs();
	static const std::filesystem::path& Images();
	static const std::filesystem::path& glTFs();
	static const std::filesystem::path& Shaders();
	static const std::filesystem::path& ShaderOutput();
	static const std::filesystem::path& NvAftermathOutput();
};